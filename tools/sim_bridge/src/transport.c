/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#define _POSIX_C_SOURCE 200809L
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef SOCKET bridge_socket;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define socket_close closesocket
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
typedef int bridge_socket;
#define INVALID_SOCKET_VALUE (-1)
#define socket_close close
#endif
#include "transport.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_LIMIT (1024 * 1024)
#define EVENT_LIMIT 256
#define RPC_TIMEOUT_MS 5000
#define FENCE_LIMIT 16

struct bridge_fence {
    bool active;
    uint32_t id;
    uint64_t order;
    char wire[192];
    size_t size, sent;
    esp_gsp_err_t result;
};

struct bridge_timer {
    struct bridge_timer *next;
    uint64_t due;
    uint32_t period;
    esp_gsp_timer_cb_t callback;
    void *ctx;
    bool deleted;
};
struct gsp_ui_core {
    bridge_socket socket;
    uint32_t request_id;
    bool failed, polling;
    bool media_enabled, image_enabled, fence_enabled, drawing, closing;
    bool read_timeout;
    bool draining_canvas, canvas_queue_closed;
    char read_header[4096];
    size_t header_used, body_size, body_used;
    char *read_body;
    uint64_t fence_order;
    struct bridge_fence fences[FENCE_LIMIT];
    struct bridge_media *media;
    struct bridge_images *images;
    uint16_t scene;
    esp_gsp_event_cb_t callback;
    void *callback_ctx;
    esp_gsp_event_t events[EVENT_LIMIT];
    unsigned head, count;
    struct bridge_timer *timers;
};

struct bridge_media **bridge_media_slot(esp_gsp_handle_t ui)
{
    return ui ? &ui->media : NULL;
}
struct bridge_images **bridge_images_slot(esp_gsp_handle_t ui)
{
    return ui ? &ui->images : NULL;
}
bool bridge_images_enabled(esp_gsp_handle_t ui)
{
    return ui && ui->image_enabled;
}
bool bridge_media_enabled(esp_gsp_handle_t ui)
{
    return ui && ui->media_enabled;
}
bool bridge_canvas_queue_allowed(esp_gsp_handle_t ui)
{
    return ui && !ui->failed && !ui->drawing && !ui->closing && !ui->canvas_queue_closed;
}

static void drain_canvas(esp_gsp_handle_t ui)
{
    if (ui->draining_canvas || ui->drawing || ui->failed) {
        return;
    }
    ui->draining_canvas = true;
    bridge_canvas_drain(ui);
    ui->draining_canvas = false;
}
bool bridge_drawing(esp_gsp_handle_t ui)
{
    return ui && ui->drawing;
}
void bridge_set_drawing(esp_gsp_handle_t ui, bool drawing)
{
    ui->drawing = drawing;
}

uint64_t gsp_sim_bridge_time_ms(void)
{
#ifdef _WIN32
    return GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
#endif
}

/* Minimal JSON cursor: skip entire values, so keys inside strings or nested
 * values can never masquerade as a sibling key. Host output is UTF-8 JSON. */
static const char *space(const char *p)
{
    while (isspace((unsigned char) * p)) {
        ++p;
    }
    return p;
}
static const char *skip(const char *p, unsigned depth)
{
    p = space(p);
    if (depth > 32 || !*p) {
        return NULL;
    }
    if (*p == '"') {
        for (++p; *p; ++p) {
            if (*p == '"') {
                return p + 1;
            }
            if (*p == '\\') {
                ++p;
                if (*p == 'u') {
                    for (unsigned i = 0; i < 4; ++i) {
                        if (!isxdigit((unsigned char) * ++p)) {
                            return NULL;
                        }
                    }
                } else if (!*p || !strchr("\"\\/bfnrt", *p)) {
                    return NULL;
                }
            } else if ((unsigned char) * p < 32) {
                return NULL;
            }
        }
        return NULL;
    }
    if (*p == '{' || *p == '[') {
        bool object = *p == '{';
        char end = object ? '}' : ']';
        p = space(p + 1);
        if (*p == end) {
            return p + 1;
        }
        for (;;) {
            if (object) {
                if (*p != '"') {
                    return NULL;
                }
                p = skip(p, depth + 1);
                if (!p || *(p = space(p)) != ':') {
                    return NULL;
                }
                ++p;
            }
            p = skip(p, depth + 1);
            if (!p) {
                return NULL;
            }
            p = space(p);
            if (*p == end) {
                return p + 1;
            }
            if (*p != ',') {
                return NULL;
            }
            p = space(p + 1);
        }
    }
    if (!strncmp(p, "true", 4) || !strncmp(p, "null", 4)) {
        return p + 4;
    }
    if (!strncmp(p, "false", 5)) {
        return p + 5;
    }
    if (*p == '-') {
        ++p;
    }
    if (*p == '0') {
        ++p;
    } else {
        if (*p < '1' || *p > '9') {
            return NULL;
        }
        while (isdigit((unsigned char) * p)) {
            ++p;
        }
    }
    if (*p == '.') {
        if (!isdigit((unsigned char) * ++p)) {
            return NULL;
        }
        while (isdigit((unsigned char) * p)) {
            ++p;
        }
    }
    if (*p == 'e' || *p == 'E') {
        ++p;
        if (*p == '+' || *p == '-') {
            ++p;
        }
        if (!isdigit((unsigned char) * p)) {
            return NULL;
        }
        while (isdigit((unsigned char) * p)) {
            ++p;
        }
    }
    return p;
}

static const char *field(const char *json, const char *key)
{
    const char *p = space(json);
    if (*p++ != '{') {
        return NULL;
    }
    while (*(p = space(p)) == '"') {
        const char *end = skip(p, 0);
        if (!end) {
            return NULL;
        }
        bool match = (size_t)(end - p - 2) == strlen(key) && !memcmp(p + 1, key, strlen(key));
        p = space(end);
        if (*p++ != ':') {
            return NULL;
        }
        p = space(p);
        if (match) {
            return p;
        }
        p = skip(p, 0);
        if (!p) {
            return NULL;
        }
        p = space(p);
        if (*p++ != ',') {
            return NULL;
        }
    }
    return NULL;
}

int64_t bridge_json_number(const char *json, const char *key, int64_t fallback)
{
    const char *p = field(json, key);
    if (!p) {
        return fallback;
    }
    if (!strncmp(p, "true", 4)) {
        return 1;
    }
    if (!strncmp(p, "false", 5)) {
        return 0;
    }
    if (*p != '-' && !isdigit((unsigned char) * p)) {
        return fallback;
    }
    char *end;
    int64_t value = strtoll(p, &end, 10);
    return end == p || (*end && !strchr(",}] \r\n\t", *end)) ? fallback : value;
}

char *bridge_json_quote(const char *text)
{
    if (!text || strlen(text) > (FRAME_LIMIT - 2) / 6) {
        return NULL;
    }
    char *out = malloc(strlen(text) * 6 + 3), *p = out;
    if (!out) {
        return NULL;
    }
    *p++ = '"';
    for (const unsigned char *s = (const unsigned char *)text; *s; ++s) {
        if (*s == '"' || *s == '\\') {
            *p++ = '\\';
            *p++ = (char) * s;
        } else if (*s < 32) {
            sprintf(p, "\\u%04x", *s);
            p += 6;
        } else {
            *p++ = (char) * s;
        }
    }
    *p++ = '"'; *p = 0;
    return out;
}

int64_t bridge_json_first_number(const char *json, const char *key, int64_t fallback)
{
    const char *p = field(json, key);
    if (!p || *p != '[') {
        return fallback;
    }
    p = space(p + 1);
    char *end;
    int64_t value = strtoll(p, &end, 10);
    return end == p || *space(end) != ']' ? fallback : value;
}

static int readable(esp_gsp_handle_t ui, uint32_t ms)
{
    fd_set fds; FD_ZERO(&fds); FD_SET(ui->socket, &fds);
    struct timeval tv = { (long)(ms / 1000), (long)(ms % 1000) * 1000 };
    return select((int)ui->socket + 1, &fds, NULL, NULL, &tv);
}

static bool socket_retry(void)
{
#ifdef _WIN32
    int error = WSAGetLastError();
    return error == WSAEWOULDBLOCK || error == WSAEINTR;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR;
#endif
}

static int writable(esp_gsp_handle_t ui, uint32_t ms)
{
    fd_set fds; FD_ZERO(&fds); FD_SET(ui->socket, &fds);
    struct timeval tv = {(long)(ms / 1000), (long)(ms % 1000) * 1000};
    return select((int)ui->socket + 1, NULL, &fds, NULL, &tv);
}

static bool write_until(esp_gsp_handle_t ui, const char *data, size_t size,
                        size_t *sent, uint64_t deadline)
{
    while (*sent < size) {
        uint64_t now = gsp_sim_bridge_time_ms();
        if (now >= deadline) {
            return false;
        }
#ifdef MSG_NOSIGNAL
        int flags = MSG_NOSIGNAL;
#else
        int flags = 0;
#endif
        int n = send(ui->socket, data + *sent, (int)(size - *sent), flags);
        if (n > 0) {
            *sent += (size_t)n;
            continue;
        }
        if (n == 0 || !socket_retry()) {
            ui->failed = true;
            return false;
        }
        uint32_t wait = deadline - now > 1000 ? 1000 : (uint32_t)(deadline - now);
        int ready = writable(ui, wait);
        if (ready < 0 && !socket_retry()) {
            ui->failed = true;
            return false;
        }
    }
    return true;
}

static bool write_all(esp_gsp_handle_t ui, const char *data, size_t size)
{
    size_t sent = 0;
    return write_until(ui, data, size, &sent, gsp_sim_bridge_time_ms() + RPC_TIMEOUT_MS);
}

static bool fence_writes(esp_gsp_handle_t ui, uint64_t deadline)
{
    for (;;) {
        struct bridge_fence *first = NULL;
        for (unsigned i = 0; i < FENCE_LIMIT; ++i) {
            struct bridge_fence *f = &ui->fences[i];
            if (f->active && f->sent < f->size && (!first || f->order < first->order)) {
                first = f;
            }
        }
        if (!first) {
            return true;
        }
        if (!write_until(ui, first->wire, first->size, &first->sent, deadline)) {
            return false;
        }
    }
}

/* Partial headers and bodies survive a flush timeout; the next poll/RPC
 * resumes at the exact byte, rather than treating the suffix as a new frame. */
static int read_part(esp_gsp_handle_t ui, char *out, size_t size, uint64_t deadline)
{
    for (;;) {
        uint64_t now = gsp_sim_bridge_time_ms();
        if (now >= deadline) {
            ui->read_timeout = true;
            return 0;
        }
        uint32_t wait = deadline - now > 1000 ? 1000 : (uint32_t)(deadline - now);
        int ready = readable(ui, wait);
        if (!ready) {
            continue;
        }
        if (ready < 0) {
            if (socket_retry()) {
                continue;
            }
            ui->failed = true; return 0;
        }
        int n = recv(ui->socket, out, (int)size, 0);
        if (n > 0) {
            return n;
        }
        if (n < 0 && socket_retry()) {
            continue;
        }
        ui->failed = true; return 0;
    }
}

static char *read_frame(esp_gsp_handle_t ui, uint64_t deadline)
{
    ui->read_timeout = false;
    if (!ui->read_body) {
        while (ui->header_used < sizeof(ui->read_header) - 1) {
            if (!read_part(ui, ui->read_header + ui->header_used, 1, deadline)) {
                return NULL;
            }
            ui->read_header[++ui->header_used] = 0;
            if (ui->header_used >= 4 && !memcmp(ui->read_header + ui->header_used - 4, "\r\n\r\n", 4)) {
                break;
            }
        }
        if (ui->header_used == sizeof(ui->read_header) - 1) {
            ui->failed = true;
            return NULL;
        }
        char *line = ui->read_header;
        while (*line) {
            char *end = strstr(line, "\r\n");
            if (!end) {
                ui->failed = true;
                return NULL;
            }
            *end = 0;
            for (char *p = line; *p && *p != ':'; ++p) {
                *p = (char)tolower((unsigned char) * p);
            }
            if (!strncmp(line, "content-length:", 15)) {
                char *tail;
                unsigned long parsed = strtoul(line + 15, &tail, 10);
                if (*space(tail) || !parsed || parsed > FRAME_LIMIT || ui->body_size) {
                    ui->failed = true;
                    return NULL;
                }
                ui->body_size = parsed;
            }
            line = end + 2;
        }
        if (!ui->body_size) {
            ui->failed = true;
            return NULL;
        }
        ui->read_body = malloc(ui->body_size + 1);
        if (!ui->read_body) {
            ui->failed = true;
            return NULL;
        }
    }
    while (ui->body_used < ui->body_size) {
        int n = read_part(ui, ui->read_body + ui->body_used, ui->body_size - ui->body_used, deadline);
        if (!n) {
            return NULL;
        }
        ui->body_used += (size_t)n;
    }
    char *body = ui->read_body;
    body[ui->body_size] = 0;
    ui->read_body = NULL;
    ui->header_used = ui->body_size = ui->body_used = 0;
    const char *end = skip(body, 0);
    if (!end || *space(end)) {
        free(body);
        ui->failed = true;
        return NULL;
    }
    return body;
}

static bool notification(esp_gsp_handle_t ui, const char *body)
{
    int64_t id = bridge_json_number(body, "id", -1);
    if (id >= 0) {
        for (unsigned i = 0; i < FENCE_LIMIT; ++i) {
            struct bridge_fence *f = &ui->fences[i];
            if (f->active && f->id == (uint32_t)id) {
                const char *result = field(body, "result");
                f->result = result ? (esp_gsp_err_t)bridge_json_number(result, "result_code", ESP_GSP_FAIL) : ESP_GSP_FAIL;
                f->active = false;
                return true;
            }
        }
    }
    const char *method = field(body, "method"), *params = field(body, "params");
    if (!method || !params) {
        return false;
    }
    esp_gsp_event_t event = {0};
    if (!strncmp(method, "\"callback\"", 10)) {
        event.type = ESP_GSP_EVENT_CALL;
        event.action_id = (uint16_t)bridge_json_number(params, "action_id", 0);
        event.arg = (uint32_t)bridge_json_number(params, "arg", 0);
        event.scene_id = (uint16_t)bridge_json_number(params, "scene_id", ui->scene);
        event.list = (uint16_t)bridge_json_number(params, "list", ESP_GSP_LIST_NONE);
        event.item = (uint32_t)bridge_json_number(params, "item", 0);
    } else if (!strncmp(method, "\"scene_changed\"", 15)) {
        event.type = ESP_GSP_EVENT_SCENE_CHANGED;
        event.scene_id = (uint16_t)bridge_json_number(params, "to", ui->scene);
        ui->scene = event.scene_id;
        bridge_media_scene_changed(ui);
    } else {
        return bridge_media_notify(ui, method, params) && bridge_images_notify(ui, method, params);
    }
    if (ui->count == EVENT_LIMIT) {
        return false;    /* Never silently drop callbacks. */
    }
    ui->events[(ui->head + ui->count++) % EVENT_LIMIT] = event;
    return true;
}

char *bridge_rpc(esp_gsp_handle_t ui, const char *method, const char *params)
{
    if (ui) {
        drain_canvas(ui);
    }
    if (!ui || ui->failed) {
        return NULL;
    }
    size_t size = strlen(params) + strlen(method) + 128;
    if (size > FRAME_LIMIT) {
        return NULL;
    }
    char *body = malloc(size);
    if (!body) {
        return NULL;
    }
    uint32_t id = ++ui->request_id;
    int n = snprintf(body, size, "{\"jsonrpc\":\"2.0\",\"id\":%" PRIu32 ",\"method\":\"%s\",\"params\":%s}", id, method, params);
    char header[64];
    snprintf(header, sizeof(header), "Content-Length: %d\r\n\r\n", n);
    bool sent = fence_writes(ui, gsp_sim_bridge_time_ms() + RPC_TIMEOUT_MS) &&
                write_all(ui, header, strlen(header)) && write_all(ui, body, (size_t)n);
    free(body);
    uint64_t deadline = gsp_sim_bridge_time_ms() + RPC_TIMEOUT_MS;
    while (sent) {
        char *reply = read_frame(ui, deadline);
        if (!reply) {
            break;
        }
        if (bridge_json_number(reply, "id", -1) == id) {
            const char *result = field(reply, "result");
            if (!result) {
                fprintf(stderr, "sim_bridge: RPC %s failed: %s\n", method, reply);
                free(reply); return NULL;
            }
            memmove(reply, result, strlen(result) + 1);
            char *end = (char *)skip(reply, 0);
            if (!end) {
                free(reply);
                break;
            }
            *end = 0;
            return reply;
        }
        bool ok = notification(ui, reply);
        free(reply);
        if (!ok) {
            break;
        }
    }
    ui->failed = true;
    fprintf(stderr, "sim_bridge: disconnected, timed out, or invalid/overflowing reply\n");
    return NULL;
}

esp_gsp_err_t bridge_command(esp_gsp_handle_t ui, const char *method, const char *params)
{
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    char *reply = bridge_rpc(ui, method, params);
    if (!reply) {
        return ESP_GSP_FAIL;
    }
    int64_t rc = bridge_json_number(reply, "result_code", ESP_GSP_FAIL);
    free(reply);
    return (esp_gsp_err_t)rc;
}

esp_gsp_err_t bridge_binary(esp_gsp_handle_t ui, const char *headers, const void *data, size_t size)
{
    if (ui) {
        drain_canvas(ui);
    }
    if (!ui || !data || !size || size > 16U * 1024U * 1024U) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (ui->drawing || ui->failed) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!ui->media_enabled) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    uint32_t id = ++ui->request_id;
    char header[1024], expected[32];
    snprintf(expected, sizeof(expected), "\"%" PRIu32 "\"", id);
    int count = snprintf(header, sizeof(header),
                         "Content-Length: %zu\r\nContent-Type: application/vnd.esp-gsp.binary\r\n"
                         "X-GSP-Transfer-ID: %" PRIu32 "\r\n%s\r\n", size, id, headers);
    if (count < 0 || (size_t)count >= sizeof(header)) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    bool sent = fence_writes(ui, gsp_sim_bridge_time_ms() + RPC_TIMEOUT_MS) &&
                write_all(ui, header, (size_t)count) && write_all(ui, data, size);
    uint64_t deadline = gsp_sim_bridge_time_ms() + RPC_TIMEOUT_MS;
    while (sent) {
        char *body = read_frame(ui, deadline);
        if (!body) {
            break;
        }
        const char *method = field(body, "method"), *params = field(body, "params");
        const char *transfer = params ? field(params, "transfer_id") : NULL;
        if (method && !strncmp(method, "\"binary_result\"", 15) &&
                transfer && !strncmp(transfer, expected, strlen(expected))) {
            esp_gsp_err_t rc = (esp_gsp_err_t)bridge_json_number(params, "result_code", ESP_GSP_FAIL);
            if (!bridge_json_number(params, "ok", 0) && rc == ESP_GSP_OK) {
                rc = ESP_GSP_FAIL;
            }
            if (rc != ESP_GSP_OK) {
                fprintf(stderr, "sim_bridge: binary upload rejected: %s\n", body);
            }
            free(body); return rc;
        }
        bool ok = notification(ui, body);
        free(body);
        if (!ok) {
            break;
        }
    }
    ui->failed = true;
    return ESP_GSP_FAIL;
}

esp_gsp_err_t bridge_scalar(esp_gsp_handle_t ui, unsigned op, const uint32_t args[8], uint32_t out[8])
{
    if (bridge_drawing(ui) && !((op >= 1 && op <= 10) ||
                                op == GSP_BRIDGE_GET_PROPERTY || op == GSP_BRIDGE_CANVAS_INFO ||
                                op == GSP_BRIDGE_COMPONENT_RESOURCE_BIND)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    char params[320];
    snprintf(params, sizeof(params), "{\"op\":%u,\"args\":[%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 "]}",
             op, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
    char *reply = bridge_rpc(ui, "bridge_call", params);
    if (!reply) {
        return ESP_GSP_FAIL;
    }
    esp_gsp_err_t rc = (esp_gsp_err_t)bridge_json_number(reply, "result_code", ESP_GSP_FAIL);
    if (rc == ESP_GSP_OK && out) {
        const char *p = field(reply, "values");
        if (!p || *p++ != '[') {
            rc = ESP_GSP_FAIL;
        }
        for (unsigned i = 0; rc == ESP_GSP_OK && i < 8; ++i) {
            char *end;
            unsigned long long value = strtoull(space(p), &end, 10);
            if (end == space(p) || value > UINT32_MAX) {
                rc = ESP_GSP_FAIL;
                break;
            }
            out[i] = (uint32_t)value;
            p = space(end);
            if (*p++ != (i == 7 ? ']' : ',')) {
                rc = ESP_GSP_FAIL;
            }
        }
    }
    free(reply);
    return rc;
}

esp_gsp_err_t gsp_sim_bridge_open(const char *endpoint, esp_gsp_handle_t *out)
{
    if (!endpoint || !out) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    *out = NULL;
    unsigned a, b, c, d, port; int consumed = 0;
    if (sscanf(endpoint, "tcp://%u.%u.%u.%u:%u%n", &a, &b, &c, &d, &port, &consumed) != 5 ||
            endpoint[consumed] || a != 127 || b > 255 || c > 255 || d > 255 || !port || port > 65535) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
        return ESP_GSP_FAIL;
    }
#endif
    esp_gsp_handle_t ui = calloc(1, sizeof(*ui));
    if (!ui) {
#ifdef _WIN32
        WSACleanup();
#endif
        return ESP_GSP_ERR_NO_MEM;
    }
    ui->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ui->socket == INVALID_SOCKET_VALUE) {
        gsp_sim_bridge_close(ui);
        return ESP_GSP_FAIL;
    }
#ifndef _WIN32
    if (ui->socket >= FD_SETSIZE) {
        gsp_sim_bridge_close(ui);
        return ESP_GSP_FAIL;
    }
#endif
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    addr.sin_addr.s_addr = htonl((a << 24) | (b << 16) | (c << 8) | d);
    int one = 1;
    setsockopt(ui->socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof(one));
#ifdef SO_NOSIGPIPE
    setsockopt(ui->socket, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
#endif
#ifdef _WIN32
    DWORD timeout = RPC_TIMEOUT_MS;
#else
    struct timeval timeout = { RPC_TIMEOUT_MS / 1000, 0 };
#endif
    setsockopt(ui->socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
    if (connect(ui->socket, (struct sockaddr *)&addr, sizeof(addr))) {
        gsp_sim_bridge_close(ui); return ESP_GSP_FAIL;
    }
#ifdef _WIN32
    u_long nonblocking = 1;
    if (ioctlsocket(ui->socket, FIONBIO, &nonblocking)) {
#else
    int flags = fcntl(ui->socket, F_GETFL, 0);
    if (flags < 0 || fcntl(ui->socket, F_SETFL, flags | O_NONBLOCK) < 0) {
#endif
        gsp_sim_bridge_close(ui); return ESP_GSP_FAIL;
    }
    char *caps = bridge_rpc(ui, "capabilities", "{}");
    if (!caps || bridge_json_number(caps, "bridge_version", 0) != GSP_SIM_BRIDGE_VERSION ||
            bridge_json_number(caps, "abi_version", 0) != 1) {
        fprintf(stderr, "sim_bridge: host requires compatible bridge_version=1 and abi_version=1\n");
        free(caps); gsp_sim_bridge_close(ui); return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    ui->scene = (uint16_t)bridge_json_number(caps, "current_scene", 0);
    ui->media_enabled = bridge_json_number(caps, "bridge_media_version", 0) == 1;
    ui->image_enabled = bridge_json_number(caps, "bridge_image_version", 0) == 1;
    ui->fence_enabled = bridge_json_number(caps, "bridge_fence_version", 0) == 1;
    free(caps);
    *out = ui;
    return ESP_GSP_OK;
}

uint16_t gsp_sim_bridge_current_scene(esp_gsp_handle_t ui)
{
    return ui ? ui->scene : 0;
}

esp_gsp_err_t esp_gsp_on_event(esp_gsp_handle_t ui, esp_gsp_event_cb_t cb, void *ctx)
{
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!ui) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    ui->callback = cb; ui->callback_ctx = ctx;
    return ESP_GSP_OK;
}

void *esp_gsp_timer_create(esp_gsp_handle_t ui, uint32_t ms, esp_gsp_timer_cb_t cb, void *ctx)
{
    if (!ui || !ms || !cb || ui->failed || ui->drawing) {
        return NULL;
    }
    struct bridge_timer *timer = calloc(1, sizeof(*timer));
    if (!timer) {
        return NULL;
    }
    timer->period = ms; timer->due = gsp_sim_bridge_time_ms() + ms;
    timer->callback = cb; timer->ctx = ctx; timer->next = ui->timers;
    ui->timers = timer;
    return timer;
}

esp_gsp_err_t esp_gsp_timer_delete(esp_gsp_handle_t ui, void *handle)
{
    if (bridge_drawing(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!ui || !handle) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    for (struct bridge_timer *t = ui->timers; t; t = t->next) {
        if (t == handle && !t->deleted) {
            t->deleted = true;
            return ESP_GSP_OK;
        }
    }
    return ESP_GSP_ERR_NOT_FOUND;
}

esp_gsp_err_t gsp_sim_bridge_poll(esp_gsp_handle_t ui, uint32_t timeout_ms)
{
    if (!ui || ui->polling || ui->drawing || ui->closing || ui->failed) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    ui->polling = true;
    drain_canvas(ui);
    uint64_t now = gsp_sim_bridge_time_ms();
    uint32_t wait = timeout_ms;
    for (struct bridge_timer *t = ui->timers; t; t = t->next) {
        if (!t->deleted && t->due <= now + wait) {
            wait = t->due > now ? (uint32_t)(t->due - now) : 0;
        }
    }
    if (ui->count || bridge_media_pending(ui) || bridge_images_pending(ui)) {
        wait = 0;
    }
    if (!fence_writes(ui, gsp_sim_bridge_time_ms() + RPC_TIMEOUT_MS)) {
        ui->failed = true;
    }
    int ready = ui->failed ? -1 : readable(ui, wait);
    if (ready > 0) {
        char *body = read_frame(ui, gsp_sim_bridge_time_ms() + RPC_TIMEOUT_MS);
        if (!body || !notification(ui, body)) {
            ui->failed = true;
        }
        free(body);
    } else if (ready < 0) {
        ui->failed = true;
    }
    unsigned budget = EVENT_LIMIT;
    while (ui->count && budget-- && !ui->failed) {
        esp_gsp_event_t event = ui->events[ui->head];
        ui->head = (ui->head + 1) % EVENT_LIMIT; --ui->count;
        if (ui->callback) {
            ui->callback(ui, &event, ui->callback_ctx);
        }
    }
    now = gsp_sim_bridge_time_ms();
    for (struct bridge_timer *t = ui->timers; t && !ui->failed; t = t->next) {
        if (!t->deleted && t->due <= now) {
            t->due = now + t->period; /* coalesce missed ticks */
            t->callback(ui, t->ctx);
        }
    }
    struct bridge_timer **p = &ui->timers;
    while (*p) {
        if ((*p)->deleted) {
            struct bridge_timer *dead = *p;
            *p = dead->next;
            free(dead);
        } else {
            p = &(*p)->next;
        }
    }
    esp_gsp_err_t media_rc = ui->failed ? ESP_GSP_FAIL : bridge_media_poll(ui);
    if (!ui->failed) {
        bridge_images_poll(ui);
    }
    ui->polling = false;
    if (media_rc != ESP_GSP_OK) {
        return media_rc;
    }
    return ui->failed ? ESP_GSP_FAIL : ESP_GSP_OK;
}

void bridge_payloads_shutdown(esp_gsp_handle_t ui)
{
    /* Default main settles payload/context lifetimes before app_deinit. */
    ui->canvas_queue_closed = true;
    bridge_canvas_cancel(ui);
    ui->image_enabled = false;
    ui->drawing = true;
    bridge_images_close(ui, ui->failed ? GSP_ERR_IO : GSP_ERR_CANCELLED);
    ui->drawing = false;
}

esp_gsp_err_t esp_gsp_flush(esp_gsp_handle_t ui, uint32_t timeout_ms)
{
    if (!ui) {
        return ESP_GSP_ERR_INVALID_ARG;
    }
    if (ui->failed || ui->drawing || ui->closing || bridge_canvas_pending(ui)) {
        return ESP_GSP_ERR_INVALID_STATE;
    }
    if (!ui->fence_enabled) {
        return ESP_GSP_ERR_NOT_SUPPORTED;
    }
    uint64_t deadline = timeout_ms == UINT32_MAX ? UINT64_MAX : gsp_sim_bridge_time_ms() + timeout_ms;
    struct bridge_fence *f = NULL;
    for (unsigned i = 0; i < FENCE_LIMIT; ++i)
        if (!ui->fences[i].active) {
            f = &ui->fences[i];
            break;
        }
    if (!f) {
        return ESP_GSP_ERR_TIMEOUT;
    }
    memset(f, 0, sizeof(*f));
    f->active = true; f->id = ++ui->request_id; f->order = ++ui->fence_order;
    char body[128];
    int n = snprintf(body, sizeof(body), "{\"jsonrpc\":\"2.0\",\"id\":%" PRIu32 ",\"method\":\"render_fence\",\"params\":{}}", f->id);
    f->size = (size_t)snprintf(f->wire, sizeof(f->wire), "Content-Length: %d\r\n\r\n%s", n, body);
    /* Zero only admits a fence; no socket I/O and no business callback. */
    if (!timeout_ms) {
        return ESP_GSP_ERR_TIMEOUT;
    }
    if (!fence_writes(ui, deadline)) {
        return ui->failed ? ESP_GSP_FAIL : ESP_GSP_ERR_TIMEOUT;
    }
    while (f->active) {
        char *reply = read_frame(ui, deadline);
        if (!reply) {
            return ui->read_timeout && !ui->failed ? ESP_GSP_ERR_TIMEOUT : ESP_GSP_FAIL;
        }
        bool ok = notification(ui, reply);
        free(reply);
        if (!ok) {
            ui->failed = true;
            return ESP_GSP_FAIL;
        }
    }
    return f->result;
}

void gsp_sim_bridge_close(esp_gsp_handle_t ui)
{
    if (!ui || ui->closing) {
        return;
    }
    if (ui->polling || ui->drawing) {
        fprintf(stderr, "sim_bridge: defer close until after callbacks/poll\n");
        return;
    }
    gsp_err_t status = ui->failed ? GSP_ERR_IO : GSP_ERR_CANCELLED;
    ui->closing = true; ui->failed = true; ui->drawing = true;
    if (ui->socket != INVALID_SOCKET_VALUE) {
        socket_close(ui->socket);
    }
    bridge_media_close(ui);
    bridge_images_close(ui, status);
    free(ui->read_body);
    while (ui->timers) {
        struct bridge_timer *t = ui->timers;
        ui->timers = t->next;
        free(t);
    }
    free(ui);
#ifdef _WIN32
    WSACleanup();
#endif
}
