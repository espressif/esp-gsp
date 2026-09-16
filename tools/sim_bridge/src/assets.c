/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#include "esp_gsp_assets.h"
#include "transport.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This is deliberately a small PC-side reader, rather than a second runtime
 * resource decoder.  The bridge host already accepts COPY uploads of standard
 * encoded images, so unwrap just those images from an external GSPB. */
#define ASSET_MAX_UPLOAD_BYTES (16U * 1024U * 1024U)
#define GSPB_HEADER_SIZE 32U
#define GSPB_ENTRY_SIZE 24U
#define GRB_ENTRY_SIZE 40U
#define SECTION_ENTRY_SIZE 16U

struct bridge_asset_target {
    struct bridge_asset_target *next;
    esp_gsp_handle_t gsp;
    esp_gsp_asset_target_t target;
    esp_gsp_asset_status_t status;
};

struct esp_gsp_assets {
    FILE *file;
    uint32_t size;
    uint16_t count;
    size_t budget;
    esp_gsp_assets_stats_t stats;
    uint32_t next_request;
    bool closing;
    struct bridge_asset_target *targets;
};

static uint16_t le16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint32_t crc_feed(uint32_t crc, const void *data, size_t size)
{
    const uint8_t *bytes = data;
    while (size--) {
        crc ^= *bytes++;
        for (unsigned bit = 0; bit < 8; ++bit) {
            crc = (crc >> 1) ^ ((crc & 1U) ? UINT32_C(0xedb88320) : 0);
        }
    }
    return crc;
}

static uint32_t crc32(const void *data, size_t size)
{
    return crc_feed(UINT32_MAX, data, size) ^ UINT32_MAX;
}

static gsp_err_t error_from_bridge(esp_gsp_err_t rc)
{
    if (rc == ESP_GSP_ERR_NOT_SUPPORTED) {
        return GSP_ERR_UNSUPPORTED;
    }
    if (rc == ESP_GSP_ERR_NOT_FOUND) {
        return GSP_ERR_NOT_FOUND;
    }
    if (rc == ESP_GSP_ERR_NO_MEM || rc == ESP_GSP_ERR_INVALID_SIZE) {
        return GSP_ERR_LIMIT_EXCEEDED;
    }
    if (rc == ESP_GSP_ERR_INVALID_ARG) {
        return GSP_ERR_INVALID_ARG;
    }
    if (rc == ESP_GSP_ERR_INVALID_STATE) {
        return GSP_ERR_INVALID_STATE;
    }
    return rc == ESP_GSP_OK ? GSP_OK : GSP_ERR_IO;
}

static bool target_same_slot(const esp_gsp_asset_target_t *a, const esp_gsp_asset_target_t *b)
{
    return a->kind == b->kind && a->bind == b->bind &&
           a->resource_slot == b->resource_slot &&
           (a->kind != ESP_GSP_ASSET_ROW || a->row.slot == b->row.slot);
}

static bool target_same_token(const esp_gsp_asset_target_t *a, const esp_gsp_asset_target_t *b)
{
    return target_same_slot(a, b) &&
           (a->kind != ESP_GSP_ASSET_ROW ||
            (a->row.instance == b->row.instance && a->row.item == b->row.item));
}

static struct bridge_asset_target *find_target(esp_gsp_assets_t *assets,
        esp_gsp_handle_t gsp,
        const esp_gsp_asset_target_t *target)
{
    for (struct bridge_asset_target *p = assets->targets; p; p = p->next)
        if (p->gsp == gsp && target_same_slot(&p->target, target)) {
            return p;
        }
    return NULL;
}

static struct bridge_asset_target *track_target(esp_gsp_assets_t *assets,
        esp_gsp_handle_t gsp,
        esp_gsp_asset_target_t target)
{
    struct bridge_asset_target *p = find_target(assets, gsp, &target);
    if (p) {
        return p;
    }
    p = calloc(1, sizeof(*p));
    if (!p) {
        return NULL;
    }
    p->gsp = gsp; p->target = target;
    p->next = assets->targets; assets->targets = p;
    return p;
}

static bool read_at(esp_gsp_assets_t *assets, uint32_t offset, void *out, size_t size)
{
    if (!assets || offset > assets->size || size > assets->size - offset ||
#if LONG_MAX < UINT32_MAX
            offset > LONG_MAX ||
#endif
            fseek(assets->file, (long)offset, SEEK_SET) != 0 ||
            fread(out, 1, size, assets->file) != size) {
        return false;
    }
    assets->stats.read_bytes += size;
    return true;
}

static esp_gsp_err_t package_crc(FILE *file, uint32_t size, uint32_t expected)
{
    uint8_t block[4096];
    uint32_t crc = UINT32_MAX;
    for (uint32_t offset = 0; offset < size;) {
        size_t n = size - offset;
        if (n > sizeof(block)) {
            n = sizeof(block);
        }
        if (fread(block, 1, n, file) != n) {
            return ESP_GSP_FAIL;
        }
        if (offset <= 12 && offset + n > 12) {
            size_t first = 12 - offset;
            size_t last = first + 4;
            if (last > n) {
                last = n;
            }
            memset(block + first, 0, last - first);
        }
        crc = crc_feed(crc, block, n);
        offset += n;
    }
    return (crc ^ UINT32_MAX) == expected ? ESP_GSP_OK : ESP_GSP_FAIL;
}

esp_gsp_err_t esp_gsp_assets_open(const char *path, uint32_t index_crc,
                                  const esp_gsp_assets_config_t *config,
                                  esp_gsp_assets_t **out_assets)
{
    (void)index_crc; /* PC storage is fast; always validate the complete package. */
    if (!out_assets) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *out_assets = NULL;
    if (!path || (config && config->read_budget_bytes > ASSET_MAX_UPLOAD_BYTES)) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    FILE *file = fopen(path, "rb");
    if (!file) {
        return ESP_GSP_FAIL;
    }
    uint8_t header[GSPB_HEADER_SIZE];
    esp_gsp_err_t rc = ESP_GSP_FAIL;
    if (fseek(file, 0, SEEK_END) != 0) {
        goto fail;
    }
    long length = ftell(file);
    if (length < (long)GSPB_HEADER_SIZE || (uint64_t)length > UINT32_MAX ||
            fseek(file, 0, SEEK_SET) != 0 || fread(header, 1, sizeof(header), file) != sizeof(header)) {
        goto fail;
    }
    uint32_t size = (uint32_t)length;
    uint16_t count = le16(header + 16);
    if (memcmp(header, "GSPB", 4) || le32(header + 8) != size ||
            le16(header + 18) != 0 || le32(header + 28) != 0 ||
            count == 0 || (uint64_t)GSPB_HEADER_SIZE + (uint64_t)count * GSPB_ENTRY_SIZE > size) {
        goto fail;
    }
    if (le16(header + 4) != 1 || le16(header + 6) > 3) {
        rc = ESP_GSP_ERR_NOT_SUPPORTED; goto fail;
    }
    if (fseek(file, 0, SEEK_SET) != 0 || package_crc(file, size, le32(header + 12)) != ESP_GSP_OK) {
        goto fail;
    }
    esp_gsp_assets_t *assets = calloc(1, sizeof(*assets));
    if (!assets) {
        rc = ESP_GSP_ERR_NO_MEM;
        goto fail;
    }
    assets->file = file; assets->size = size; assets->count = count;
    assets->budget = config && config->read_budget_bytes ? config->read_budget_bytes : ASSET_MAX_UPLOAD_BYTES;
    assets->stats.index_bytes = (size_t)GSPB_HEADER_SIZE + (size_t)count * GSPB_ENTRY_SIZE;
    assets->stats.encoded_budget_bytes = assets->budget;
    *out_assets = assets;
    return ESP_GSP_OK;
fail:
    fclose(file);
    return rc;
}

static esp_gsp_err_t member_entry(esp_gsp_assets_t *assets, uint16_t member, uint8_t out[GSPB_ENTRY_SIZE])
{
    return member < assets->count && read_at(assets, GSPB_HEADER_SIZE + (uint32_t)member * GSPB_ENTRY_SIZE,
            out, GSPB_ENTRY_SIZE) ? ESP_GSP_OK : ESP_GSP_FAIL;
}

static esp_gsp_err_t resolve_asset(esp_gsp_assets_t *assets, esp_gsp_asset_ref_t *asset)
{
    if (asset->member_count) {
        if ((uint32_t)asset->first_member + asset->member_count > assets->count) {
            return ESP_GSP_ERR_INVALID_ARG;
        }
        for (uint16_t i = 0; i < asset->member_count; ++i) {
            uint8_t entry[GSPB_ENTRY_SIZE];
            if (member_entry(assets, asset->first_member + i, entry) != ESP_GSP_OK) {
                return ESP_GSP_FAIL;
            }
            if (le16(entry) != 2 || le16(entry + 2) != (uint16_t)(asset->first_member + i) ||
                    le32(entry + 16) != asset->name_hash || le32(entry + 20) != 0) {
                return ESP_GSP_FAIL;
            }
        }
        return ESP_GSP_OK;
    }
    bool ended = false;
    for (uint16_t i = 0; i < assets->count; ++i) {
        uint8_t entry[GSPB_ENTRY_SIZE];
        if (member_entry(assets, i, entry) != ESP_GSP_OK) {
            return ESP_GSP_FAIL;
        }
        if (le32(entry + 16) == asset->name_hash) {
            if (ended || le16(entry) != 2 || le16(entry + 2) != i || asset->member_count == UINT16_MAX) {
                return ESP_GSP_FAIL;
            }
            if (!asset->member_count) {
                asset->first_member = i;
            }
            ++asset->member_count;
        } else if (asset->member_count) {
            ended = true;
        }
    }
    return asset->member_count ? ESP_GSP_OK : ESP_GSP_ERR_NOT_FOUND;
}

static bool valid_range(size_t total, uint32_t offset, uint32_t size)
{
    return offset <= total && size <= total - offset;
}

static esp_gsp_err_t unwrap_image(const uint8_t *grb, size_t size, uint32_t name_hash,
                                  const uint8_t **image, size_t *image_size)
{
    if (size < GSPB_HEADER_SIZE || memcmp(grb, "GRB1", 4) || le32(grb + 8) != size ||
            le16(grb + 18) != 0 || le32(grb + 28) != 0) {
        return ESP_GSP_FAIL;
    }
    if (le16(grb + 4) != 1 || le16(grb + 6) > 5) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    uint32_t expected = le32(grb + 12), crc = UINT32_MAX;
    crc = crc_feed(crc, grb, 12);
    uint32_t zero = 0;
    crc = crc_feed(crc, &zero, sizeof(zero));
    crc = crc_feed(crc, grb + 16, size - 16);
    if ((crc ^ UINT32_MAX) != expected) {
        return ESP_GSP_FAIL;
    }
    uint16_t section_count = le16(grb + 16);
    if (!section_count || (uint64_t)GSPB_HEADER_SIZE + (uint64_t)section_count * SECTION_ENTRY_SIZE > size) {
        return ESP_GSP_FAIL;
    }
    const uint8_t *table = NULL, *data = NULL;
    size_t table_size = 0, data_size = 0;
    uint32_t table_count = 0;
    for (uint16_t i = 0; i < section_count; ++i) {
        const uint8_t *section = grb + GSPB_HEADER_SIZE + (size_t)i * SECTION_ENTRY_SIZE;
        uint16_t type = le16(section), flags = le16(section + 2);
        uint32_t offset = le32(section + 4), section_size = le32(section + 8), count = le32(section + 12);
        if (!valid_range(size, offset, section_size)) {
            return ESP_GSP_FAIL;
        }
        if (type == 0x101) {
            if (table || !(flags & 1U)) {
                return ESP_GSP_FAIL;
            }
            table = grb + offset; table_size = section_size; table_count = count;
        } else if (type == 0x102) {
            if (data || !(flags & 1U) || count) {
                return ESP_GSP_FAIL;
            }
            data = grb + offset; data_size = section_size;
        }
    }
    if (!table || !data || table_count != 1 || table_size != GRB_ENTRY_SIZE) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    if (le32(table) != name_hash) {
        return ESP_GSP_FAIL;
    }
    uint8_t codec = table[4];
    uint32_t offset = le32(table + 16), encoded_size = le32(table + 20);
    if (!valid_range(data_size, offset, encoded_size) || !encoded_size) {
        return ESP_GSP_FAIL;
    }
    const uint8_t *encoded = data + offset;
    bool qoi = codec == 5 && encoded_size >= 4 && !memcmp(encoded, "qoif", 4);
    bool jpeg = codec == 2 && encoded_size >= 2 && encoded[0] == 0xff && encoded[1] == 0xd8;
    bool png = codec == 3 && encoded_size >= 8 && !memcmp(encoded, "\x89PNG\r\n\x1a\n", 8);
    if (!qoi && !jpeg && !png) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    *image = encoded; *image_size = encoded_size;
    return ESP_GSP_OK;
}

static esp_gsp_err_t upload_target(esp_gsp_handle_t gsp, esp_gsp_asset_target_t target,
                                   const void *data, size_t size)
{
    if (target.kind == ESP_GSP_ASSET_IMAGE) {
        return esp_gsp_set_image(gsp, target.bind, data, size);
    }
    if (target.kind == ESP_GSP_ASSET_ROW) {
        return esp_gsp_row_set_image(gsp, target.row, target.resource_slot, data, size);
    }
    if (target.kind == ESP_GSP_ASSET_WIDGET) {
        return esp_gsp_widget_set_image(gsp, target.bind, target.resource_slot, data, size);
    }
    return ESP_GSP_ERR_INVALID_ARG;
}

static esp_gsp_err_t show_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                 esp_gsp_asset_target_t target, esp_gsp_asset_ref_t asset, bool once)
{
    (void)once;
    if (!assets || !gsp) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (assets->closing) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    struct bridge_asset_target *tracked = track_target(assets, gsp, target);
    if (!tracked) {
        return ESP_GSP_ERR_NO_MEM;
    }
    tracked->target = target;
    tracked->status = (esp_gsp_asset_status_t) {
        .state = ESP_GSP_ASSET_READING,
        .request_id = ++assets->next_request, .last_error = GSP_OK
    };
    esp_gsp_err_t rc = resolve_asset(assets, &asset);
    if (rc == ESP_GSP_OK && asset.member_count != 1) {
        rc = ESP_GSP_ERR_NOT_SUPPORTED;
    }
    uint8_t entry[GSPB_ENTRY_SIZE];
    void *member = NULL;
    if (rc == ESP_GSP_OK && member_entry(assets, asset.first_member, entry) != ESP_GSP_OK) {
        rc = ESP_GSP_FAIL;
    }
    uint32_t offset = rc == ESP_GSP_OK ? le32(entry + 4) : 0;
    uint32_t member_size = rc == ESP_GSP_OK ? le32(entry + 8) : 0;
    if (rc == ESP_GSP_OK && (offset % 64 || !valid_range(assets->size, offset, member_size) ||
                             member_size > assets->budget || member_size > ASSET_MAX_UPLOAD_BYTES)) {
        rc = ESP_GSP_ERR_NOT_SUPPORTED;
    }
    if (rc == ESP_GSP_OK) {
        member = malloc(member_size);
        if (!member) {
            rc = ESP_GSP_ERR_NO_MEM;
        } else if (!read_at(assets, offset, member, member_size)) {
            rc = ESP_GSP_FAIL;
        }
    }
    const uint8_t *image = NULL; size_t image_size = 0;
    if (rc == ESP_GSP_OK) {
        rc = unwrap_image(member, member_size, asset.name_hash, &image, &image_size);
    }
    if (rc == ESP_GSP_OK) {
        rc = upload_target(gsp, target, image, image_size);
    }
    if (member) {
        free(member);
    }
    if (rc == ESP_GSP_OK) {
        tracked->status.state = ESP_GSP_ASSET_READY;
        tracked->status.last_error = GSP_OK;
        assets->stats.submitted_frames++;
    } else {
        tracked->status.state = ESP_GSP_ASSET_FAILED;
        tracked->status.last_error = error_from_bridge(rc);
        tracked->status.failed_stage = ESP_GSP_ASSET_READING;
        tracked->status.required_bytes = member_size;
    }
    return rc;
}

esp_gsp_err_t esp_gsp_assets_show(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                  uint16_t bind, esp_gsp_asset_ref_t asset, bool once)
{
    return show_target(assets, gsp, esp_gsp_asset_image_target(bind), asset, once);
}

esp_gsp_err_t esp_gsp_assets_show_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                       uint16_t bind, const char *name, bool once)
{
    if (!name || !*name) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    return esp_gsp_assets_show(assets, gsp, bind,
    (esp_gsp_asset_ref_t) {
        .name_hash = crc32(name, strlen(name))
    }, once);
}

esp_gsp_err_t esp_gsp_assets_show_row(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                      esp_gsp_row_t row, uint16_t slot, esp_gsp_asset_ref_t asset, bool once)
{
    return show_target(assets, gsp, esp_gsp_asset_row_target(row, slot), asset, once);
}

esp_gsp_err_t esp_gsp_assets_show_widget(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_widget_t widget, uint16_t slot, esp_gsp_asset_ref_t asset, bool once)
{
    return show_target(assets, gsp, esp_gsp_asset_widget_target(widget, slot), asset, once);
}

esp_gsp_err_t esp_gsp_assets_show_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_asset_target_t target, esp_gsp_asset_ref_t asset, bool once)
{
    return show_target(assets, gsp, target, asset, once);
}

esp_gsp_err_t esp_gsp_assets_show_target_name(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_asset_target_t target, const char *name, bool once)
{
    if (!name || !*name) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    return show_target(assets, gsp, target,
    (esp_gsp_asset_ref_t) {
        .name_hash = crc32(name, strlen(name))
    }, once);
}

esp_gsp_err_t esp_gsp_assets_get_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                        esp_gsp_asset_target_t target, esp_gsp_asset_status_t *out)
{
    if (!out) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *out = (esp_gsp_asset_status_t) {
        .state = ESP_GSP_ASSET_STOPPED
    };
    if (!assets || !gsp) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    struct bridge_asset_target *tracked = find_target(assets, gsp, &target);
    if (!tracked || !target_same_token(&tracked->target, &target)) {
        return ESP_GSP_ERR_NOT_FOUND;
    }
    *out = tracked->status;
    return ESP_GSP_OK;
}

gsp_err_t esp_gsp_assets_status(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                uint16_t bind, bool *pending)
{
    if (pending) {
        *pending = false;
    }
    esp_gsp_asset_status_t status;
    esp_gsp_err_t rc = esp_gsp_assets_get_status(assets, gsp, esp_gsp_asset_image_target(bind), &status);
    return rc == ESP_GSP_OK ? status.last_error : (rc == ESP_GSP_ERR_NOT_FOUND ? GSP_ERR_NOT_FOUND : GSP_ERR_INVALID_ARG);
}

esp_gsp_err_t esp_gsp_assets_stop_target(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
        esp_gsp_asset_target_t target)
{
    if (!assets || !gsp) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    struct bridge_asset_target *tracked = find_target(assets, gsp, &target);
    if (!tracked || !target_same_token(&tracked->target, &target)) {
        return ESP_GSP_ERR_NOT_FOUND;
    }
    tracked->status.state = ESP_GSP_ASSET_STOPPED;
    tracked->status.pending = false;
    return ESP_GSP_OK;
}

esp_gsp_err_t esp_gsp_assets_stop(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp, uint16_t bind)
{
    return esp_gsp_assets_stop_target(assets, gsp, esp_gsp_asset_image_target(bind));
}

esp_gsp_err_t esp_gsp_assets_get_stats(esp_gsp_assets_t *assets, esp_gsp_handle_t gsp,
                                       esp_gsp_assets_stats_t *out)
{
    (void)gsp;
    if (!out || !assets) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *out = assets->stats;
    return ESP_GSP_OK;
}

esp_gsp_err_t esp_gsp_assets_close_wait(esp_gsp_assets_t *assets, uint32_t timeout_ms)
{
    (void)timeout_ms;
    if (!assets) {
        return ESP_GSP_OK;
    }
    assets->closing = true;
    while (assets->targets) {
        struct bridge_asset_target *next = assets->targets->next;
        free(assets->targets); assets->targets = next;
    }
    fclose(assets->file); free(assets);
    return ESP_GSP_OK;
}

esp_gsp_err_t esp_gsp_assets_close(esp_gsp_assets_t *assets)
{
    return esp_gsp_assets_close_wait(assets, 5000);
}
