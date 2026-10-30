/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "transport.h"
#include "esp_gsp_updates.h"
#include "../update_wire.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMMANDS 128U

static void control(esp_gsp_handle_t ui, const char *method, const char *id);

typedef struct {
    const void *data;
    esp_gsp_image_options_t options;
    uint16_t bind;
    uint32_t command;
    bool completed;
} image_callbacks_t;

struct esp_gsp_update_receipt {
    struct esp_gsp_update_receipt *next;
    unsigned references;
    bool cancelled;
    char rpc_id[64];
    esp_gsp_update_status_t status;
    esp_gsp_update_options_t options;
    image_callbacks_t *images;
    size_t image_count;
    bool cancel_sent;
};

/* Like the other C Backend APIs, these calls run on the open/poll thread. */
bool esp_gsp_update_query(const esp_gsp_update_receipt_t *receipt,
                          esp_gsp_update_status_t *out)
{
    if (receipt == NULL || out == NULL) {
        return false;
    }
    *out = receipt->status;
    return true;
}

void esp_gsp_update_release(esp_gsp_update_receipt_t *receipt)
{
    if (receipt != NULL && --receipt->references == 0) {
        free(receipt->images);
        free(receipt);
    }
}

void esp_gsp_update_cancel(esp_gsp_update_receipt_t *receipt)
{
    if (receipt != NULL) {
        receipt->cancelled = true;
    }
}

esp_gsp_err_t esp_gsp_update_capabilities(esp_gsp_handle_t ui, esp_gsp_update_capabilities_t *out)
{
    if (ui == NULL || out == NULL) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *out = (esp_gsp_update_capabilities_t) {
        0
    };
    if (!bridge_updates_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    char *reply = bridge_rpc(ui, "update_capabilities", "{}");
    if (reply == NULL) {
        return ESP_GSP_FAIL;
    }
    esp_gsp_err_t ret = (esp_gsp_err_t)bridge_json_number(reply, "result_code", ESP_GSP_FAIL);
    if (ret == ESP_GSP_OK) {
        out->max_stage = (esp_gsp_update_stage_t)bridge_json_number(reply, "max_stage", 0);
        out->confirmation = (esp_gsp_update_confirmation_t)bridge_json_number(reply, "confirmation_code", 0);
        out->max_commands = (uint32_t)bridge_json_number(reply, "max_commands", 0);
        out->max_pending = (uint32_t)bridge_json_number(reply, "max_pending", 0);
        if (bridge_json_number(reply, "version", 0) != GSP_UPDATE_WIRE_VERSION ||
                out->max_stage < ESP_GSP_UPDATE_APPLIED || out->max_stage > ESP_GSP_UPDATE_PRESENTED ||
                out->confirmation > ESP_GSP_UPDATE_CONFIRM_FRAMEBUFFER ||
                out->max_commands == 0 || out->max_commands > MAX_COMMANDS || out->max_pending == 0) {
            *out = (esp_gsp_update_capabilities_t) {
                0
            };
            ret = ESP_GSP_ERR_NOT_SUPPORTED;
        }
    }
    free(reply);
    return ret;
}

static bool decode_status(const char *reply, esp_gsp_update_status_t *out)
{
    uint8_t bytes[GSP_UPDATE_WIRE_STATUS_WORDS * 4];
    if (reply == NULL || !bridge_json_hex_bytes(reply, "wire_hex", bytes, sizeof(bytes)) ||
            gsp_update_wire_read(bytes) != GSP_UPDATE_WIRE_VERSION) {
        return false;
    }
    uint32_t words[GSP_UPDATE_WIRE_STATUS_WORDS];
    for (unsigned index = 0; index < GSP_UPDATE_WIRE_STATUS_WORDS; ++index) {
        words[index] = gsp_update_wire_read(bytes + index * 4);
    }
    if (words[5] < ESP_GSP_UPDATE_ACCEPTED || words[5] > ESP_GSP_UPDATE_PRESENTED ||
            words[6] > ESP_GSP_UPDATE_TIMED_OUT || words[7] > ESP_GSP_UPDATE_CONFIRM_FRAMEBUFFER ||
            (words[15] != 0 && (words[15] < ESP_GSP_UPDATE_APPLIED || words[15] > ESP_GSP_UPDATE_PRESENTED)) ||
            ((words[6] == ESP_GSP_UPDATE_FAILED || words[6] == ESP_GSP_UPDATE_TIMED_OUT) != (words[15] != 0)) ||
            words[9] == 0 || words[9] > MAX_COMMANDS || words[10] > words[9] || words[13] > UINT16_MAX ||
            (words[11] != UINT32_MAX && words[11] >= words[9]) ||
            (words[5] >= ESP_GSP_UPDATE_APPLIED && words[10] != words[9]) ||
            (words[5] >= ESP_GSP_UPDATE_RENDERED && words[3] == 0 && words[4] == 0)) {
        return false;
    }
    *out = (esp_gsp_update_status_t) {
        .id = (uint64_t)words[2] << 32 | words[1],
        .frame_id = (uint64_t)words[4] << 32 | words[3],
        .stage = words[5], .result = words[6], .confirmation = words[7],
        .error = (gsp_err_t)(int32_t)words[8], .command_count = words[9],
        .applied_count = words[10], .failed_command = words[11],
        .scene_generation = words[12], .scene_id = (uint16_t)words[13],
        .frame_generation = words[14],
        .failed_stage = (uint16_t)words[15],
    };
    return out->id != 0;
}

static char *packet_json(const esp_gsp_update_command_t *commands, size_t count,
                         const esp_gsp_update_options_t *options, esp_gsp_err_t *error)
{
    size_t size = GSP_UPDATE_WIRE_HEADER + count * GSP_UPDATE_WIRE_COMMAND;
    for (size_t index = 0; index < count; ++index) {
        const esp_gsp_update_command_t *command = &commands[index];
        size_t bytes = command->kind == ESP_GSP_UPDATE_TEXT ? strlen(command->text) + 1U :
                       command->kind == ESP_GSP_UPDATE_IMAGE ? command->image.size : 0;
        if (bytes > GSP_UPDATE_WIRE_LIMIT - size) {
            *error = ESP_GSP_ERR_INVALID_SIZE;
            return NULL;
        }
        size += bytes;
    }
    uint8_t *packet = calloc(1, size);
    if (packet == NULL) {
        return NULL;
    }
    gsp_update_wire_write(packet, GSP_UPDATE_WIRE_VERSION);
    gsp_update_wire_write(packet + 4, (uint32_t)count);
    gsp_update_wire_write(packet + 8, options->complete_at);
    gsp_update_wire_write(packet + 12, UINT32_MAX);
    gsp_update_wire_write(packet + 16, options->timeout_ms);
    size_t offset = GSP_UPDATE_WIRE_HEADER + count * GSP_UPDATE_WIRE_COMMAND;
    for (size_t index = 0; index < count; ++index) {
        const esp_gsp_update_command_t *command = &commands[index];
        uint32_t words[8] = {command->kind, command->bind, 0};
        if (command->kind == ESP_GSP_UPDATE_PROPERTY) {
            words[2] = command->property.component; words[3] = command->property.property;
            words[4] = command->property.value.type;
            words[5] = words[4] == GSP_VALUE_BOOL ? command->property.value.data.boolean : command->property.value.data.u32;
        } else if (command->kind == ESP_GSP_UPDATE_TEXT || command->kind == ESP_GSP_UPDATE_IMAGE) {
            const void *data = command->kind == ESP_GSP_UPDATE_TEXT ? (const void *)command->text : command->image.data;
            size_t bytes = command->kind == ESP_GSP_UPDATE_TEXT ? strlen(command->text) + 1U : command->image.size;
            words[5] = (uint32_t)offset; words[6] = (uint32_t)bytes;
            words[7] = command->kind == ESP_GSP_UPDATE_IMAGE ? command->image.options.cache_key : 0;
            memcpy(packet + offset, data, bytes); offset += bytes;
        } else if (command->kind == ESP_GSP_UPDATE_SCENE) {
            words[1] = command->scene.id; words[5] = command->scene.transition;
        } else {
            words[5] = command->kind == ESP_GSP_UPDATE_VISIBLE ? command->visible :
                       command->kind == ESP_GSP_UPDATE_COLOR ? command->color : (uint32_t)command->value;
        }
        for (unsigned word = 0; word < 8; ++word) {
            gsp_update_wire_write(packet + GSP_UPDATE_WIRE_HEADER + index * GSP_UPDATE_WIRE_COMMAND + word * 4, words[word]);
        }
    }
    const char prefix[] = "{\"packet_hex\":\"";
    char *json = malloc(sizeof(prefix) - 1U + size * 2U + 3U);
    if (json != NULL) {
        static const char hex[] = "0123456789abcdef";
        memcpy(json, prefix, sizeof(prefix) - 1U);
        char *text = json + sizeof(prefix) - 1U;
        for (size_t index = 0; index < size; ++index) {
            *text++ = hex[packet[index] >> 4]; *text++ = hex[packet[index] & 15];
        }
        memcpy(text, "\"}", 3);
    }
    free(packet);
    return json;
}

esp_gsp_err_t esp_gsp_update_submit(esp_gsp_handle_t ui,
                                    const esp_gsp_update_command_t *commands, size_t count,
                                    const esp_gsp_update_options_t *options, esp_gsp_update_receipt_t **out)
{
    if (out != NULL) {
        *out = NULL;
    }
    if (ui == NULL || out == NULL || commands == NULL || count == 0 || count > MAX_COMMANDS) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!bridge_updates_enabled(ui)) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    size_t image_count = 0;
    for (size_t index = 0; index < count; ++index) {
        const esp_gsp_update_command_t *command = &commands[index];
        if (command->kind < ESP_GSP_UPDATE_PROPERTY || command->kind > ESP_GSP_UPDATE_SCENE ||
                (command->kind == ESP_GSP_UPDATE_SCENE && index != 0) ||
                (command->kind == ESP_GSP_UPDATE_TEXT && command->text == NULL)) {
            return ESP_GSP_ERR_INVALID_ARG;
        }
        if (command->kind == ESP_GSP_UPDATE_IMAGE) {
            ++image_count;
            const esp_gsp_image_options_t *image = &command->image.options;
            if (command->image.data == NULL || command->image.size == 0 ||
                    image->ownership < ESP_GSP_IMAGE_COPY || image->ownership > ESP_GSP_IMAGE_TAKE ||
                    (image->ownership == ESP_GSP_IMAGE_COPY && image->on_release != NULL) ||
                    (image->ownership == ESP_GSP_IMAGE_BORROW && image->on_release == NULL)) {
                return ESP_GSP_ERR_INVALID_ARG;
            }
        }
    }
    esp_gsp_update_receipt_t *receipt = calloc(1, sizeof(*receipt));
    if (receipt == NULL) {
        return ESP_GSP_ERR_NO_MEM;
    }
    receipt->references = 1;
    receipt->options = options != NULL ? *options : (esp_gsp_update_options_t) {
        0
    };
    esp_gsp_update_stage_t stage = receipt->options.complete_at != 0 ? receipt->options.complete_at : ESP_GSP_UPDATE_RENDERED;
    if (stage < ESP_GSP_UPDATE_APPLIED || stage > ESP_GSP_UPDATE_PRESENTED) {
        esp_gsp_update_release(receipt);
        return ESP_GSP_ERR_INVALID_ARG;
    }
    receipt->options.complete_at = stage;
    if (image_count != 0) {
        receipt->images = calloc(image_count, sizeof(*receipt->images));
    }
    esp_gsp_err_t allocation_error = ESP_GSP_ERR_NO_MEM;
    char *json = image_count == 0 || receipt->images != NULL
                 ? packet_json(commands, count, &receipt->options, &allocation_error) : NULL;
    if (json == NULL) {
        esp_gsp_update_release(receipt);
        return allocation_error;
    }
    char *reply = bridge_rpc(ui, "update_submit", json);
    free(json);
    esp_gsp_err_t ret = reply != NULL ? (esp_gsp_err_t)bridge_json_number(reply, "result_code", ESP_GSP_FAIL) : ESP_GSP_FAIL;
    if (ret == ESP_GSP_OK && (!bridge_json_token(reply, "id", receipt->rpc_id, sizeof(receipt->rpc_id)) ||
                              !decode_status(reply, &receipt->status) ||
                              receipt->status.command_count != count)) {
        if (receipt->rpc_id[0] != '\0') {
            control(ui, "cancel_update", receipt->rpc_id);
            control(ui, "release_update", receipt->rpc_id);
        }
        ret = ESP_GSP_FAIL;
    }
    free(reply);
    if (ret != ESP_GSP_OK) {
        esp_gsp_update_release(receipt);
        return ret;
    }
    for (size_t index = 0; index < count; ++index) {
        if (commands[index].kind == ESP_GSP_UPDATE_IMAGE) {
            receipt->images[receipt->image_count++] = (image_callbacks_t) {
                .data = commands[index].image.data, .options = commands[index].image.options,
                .bind = commands[index].bind, .command = (uint32_t)index,
            };
        }
    }
    ++receipt->references;
    esp_gsp_update_receipt_t **head = bridge_updates_slot(ui);
    receipt->next = *head; *head = receipt;
    *out = receipt;
    return ESP_GSP_OK;
}

static void notify_images(esp_gsp_handle_t ui, esp_gsp_update_receipt_t *receipt)
{
    for (size_t index = 0; index < receipt->image_count; ++index) {
        image_callbacks_t *image = &receipt->images[index];
        if (image->completed || (image->command >= receipt->status.applied_count &&
                                 receipt->status.result == ESP_GSP_UPDATE_PENDING)) {
            continue;
        }
        image->completed = true;
        gsp_err_t result = image->command < receipt->status.applied_count ? GSP_OK :
                           receipt->status.failed_command == image->command ? receipt->status.error : GSP_ERR_CANCELLED;
        if (image->options.on_complete != NULL) {
            image->options.on_complete(ui, image->bind, result, image->options.complete_ctx);
        }
        if (image->options.on_release != NULL) {
            image->options.on_release(image->options.release_ctx);
        } else if (image->options.ownership == ESP_GSP_IMAGE_TAKE) {
            free((void *)image->data);
        }
    }
}

static void control(esp_gsp_handle_t ui, const char *method, const char *id)
{
    char params[96];
    snprintf(params, sizeof(params), "{\"id\":\"%s\"}", id);
    (void)bridge_command(ui, method, params);
}

void bridge_updates_poll(esp_gsp_handle_t ui)
{
    esp_gsp_update_receipt_t **head = bridge_updates_slot(ui);
    esp_gsp_update_receipt_t **link = head;
    unsigned budget = 16;
    while (*link != NULL && budget-- != 0) {
        esp_gsp_update_receipt_t *receipt = *link;
        if (receipt->cancelled && !receipt->cancel_sent) {
            receipt->cancel_sent = true;
            control(ui, "cancel_update", receipt->rpc_id);
        }
        char params[96];
        snprintf(params, sizeof(params), "{\"id\":\"%s\"}", receipt->rpc_id);
        char *reply = bridge_rpc(ui, "query_update", params);
        esp_gsp_update_status_t observed;
        if (decode_status(reply, &observed) && observed.id == receipt->status.id &&
                observed.command_count == receipt->status.command_count &&
                (observed.result != ESP_GSP_UPDATE_COMPLETE || observed.stage >= receipt->options.complete_at)) {
            receipt->status = observed;
        } else {
            receipt->status.result = ESP_GSP_UPDATE_FAILED;
            receipt->status.error = GSP_ERR_IO;
            receipt->status.failed_stage = receipt->status.stage < receipt->options.complete_at
                                           ? receipt->status.stage + 1 : receipt->options.complete_at;
        }
        free(reply);
        bool terminal = receipt->status.result != ESP_GSP_UPDATE_PENDING;
        if (terminal) {
            *link = receipt->next;
            control(ui, "release_update", receipt->rpc_id);
        }
        notify_images(ui, receipt);
        if (!terminal) {
            link = &receipt->next;
            continue;
        }
        if (receipt->options.on_complete != NULL) {
            receipt->options.on_complete(&receipt->status, receipt->options.user_ctx);
        }
        esp_gsp_update_release(receipt);
        link = head; /* application callbacks may prepend a new group */
    }
}

void bridge_updates_close(esp_gsp_handle_t ui)
{
    esp_gsp_update_receipt_t **head = bridge_updates_slot(ui);
    while (*head != NULL) {
        esp_gsp_update_receipt_t *receipt = *head;
        *head = receipt->next;
        control(ui, "cancel_update", receipt->rpc_id);
        control(ui, "release_update", receipt->rpc_id);
        receipt->status.result = ESP_GSP_UPDATE_CANCELLED;
        receipt->status.error = GSP_ERR_CANCELLED;
        notify_images(ui, receipt);
        if (receipt->options.on_complete != NULL) {
            receipt->options.on_complete(&receipt->status, receipt->options.user_ctx);
        }
        esp_gsp_update_release(receipt);
    }
}
