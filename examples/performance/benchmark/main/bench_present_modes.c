/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 *
 * On-device micro-bench of present hot paths. Traverses path × rotation ×
 * coverage and prints the same style of table as ci/host mode_bench.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_heap_caps.h"
#include "esp_timer.h"

#include "esp_display_present_dirty.h"
#include "esp_display_present_geometry.h"
#include "esp_display_present_ppa.h"
#include "esp_display_present_rotate.h"
#include "esp_display_present_types.h"

enum {
    LOGICAL_W = 480,
    LOGICAL_H = 800,
    COLOR_BYTES = 2,
    DRAWBUF_BYTES = 32768,
    DIRTY_H = 36,
    ITERS = 4,
};

typedef struct {
    char name[40];
    float us_per_frame;
} bench_row_t;

static size_t stride_bytes(uint16_t width)
{
    return (size_t)width * COLOR_BYTES;
}

static void physical_size(esp_display_present_rotation_t rotation,
                          uint16_t *out_w,
                          uint16_t *out_h)
{
    if (rotation == ESP_DISPLAY_PRESENT_ROTATE_90 ||
            rotation == ESP_DISPLAY_PRESENT_ROTATE_270) {
        *out_w = LOGICAL_H;
        *out_h = LOGICAL_W;
    } else {
        *out_w = LOGICAL_W;
        *out_h = LOGICAL_H;
    }
}

static const char *rotation_name(esp_display_present_rotation_t rotation)
{
    switch (rotation) {
    case ESP_DISPLAY_PRESENT_ROTATE_0:
        return "0";
    case ESP_DISPLAY_PRESENT_ROTATE_90:
        return "90";
    case ESP_DISPLAY_PRESENT_ROTATE_180:
        return "180";
    case ESP_DISPLAY_PRESENT_ROTATE_270:
        return "270";
    default:
        return "?";
    }
}

static uint16_t band_rows(void)
{
    size_t rows = DRAWBUF_BYTES / stride_bytes(LOGICAL_W);
    if (rows > LOGICAL_H) {
        rows = LOGICAL_H;
    }
    if (rows == 0) {
        rows = 1;
    }
    return (uint16_t)rows;
}

static void *alloc_fb(size_t bytes)
{
    void *p = heap_caps_malloc(
                  bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT | MALLOC_CAP_CACHE_ALIGNED);
    if (p == NULL) {
        p = heap_caps_malloc(bytes, MALLOC_CAP_8BIT | MALLOC_CAP_CACHE_ALIGNED);
    }
    return p;
}

static float bench_us(void (*fn)(void *), void *ctx)
{
    int64_t t0 = esp_timer_get_time();
    for (int i = 0; i < ITERS; ++i) {
        fn(ctx);
    }
    return (float)(esp_timer_get_time() - t0) / (float)ITERS;
}

typedef struct {
    uint8_t *dst;
    const uint8_t *src;
    uint16_t y0;
    uint16_t y1;
    uint16_t phys_w;
    uint16_t phys_h;
    esp_display_present_rotation_t rotation;
    void *ppa;
} rotate_ctx_t;

static void run_partition(void *ctx)
{
    rotate_ctx_t *c = ctx;
    uint16_t capacity = band_rows();
    uint16_t y = c->y0;
    while (y <= c->y1) {
        uint16_t remain = (uint16_t)(c->y1 - y + 1);
        uint16_t rows = remain < capacity ? remain : capacity;
        if (c->rotation == ESP_DISPLAY_PRESENT_ROTATE_0) {
            for (uint16_t row = 0; row < rows; ++row) {
                memcpy(c->dst + (size_t)(y + row) * stride_bytes(LOGICAL_W),
                       c->src + (size_t)row * stride_bytes(LOGICAL_W),
                       stride_bytes(LOGICAL_W));
            }
        } else {
            esp_display_present_rotate_copy_request_t request = {
                .source = {
                    .pixels = (void *)c->src,
                    .width = LOGICAL_W,
                    .height = rows,
                    .stride_bytes = stride_bytes(LOGICAL_W),
                    .color_bytes = COLOR_BYTES,
                },
                .source_origin = { .x = 0, .y = y },
                .destination = {
                    .pixels = c->dst,
                    .width = c->phys_w,
                    .height = c->phys_h,
                    .stride_bytes = stride_bytes(c->phys_w),
                    .color_bytes = COLOR_BYTES,
                },
                .logical_area = {
                    .x1 = 0, .y1 = y, .x2 = LOGICAL_W - 1,
                    .y2 = (int16_t)(y + rows - 1),
                },
                .rotation = c->rotation,
            };
            esp_display_present_rotate_copy(&request);
        }
        y = (uint16_t)(y + rows);
    }
}

static void run_transform(void *ctx)
{
    rotate_ctx_t *c = ctx;
    esp_display_present_rotate_copy_request_t request = {
        .source = {
            .pixels = (void *)c->src,
            .width = LOGICAL_W,
            .height = LOGICAL_H,
            .stride_bytes = stride_bytes(LOGICAL_W),
            .color_bytes = COLOR_BYTES,
        },
        .source_origin = { .x = 0, .y = 0 },
        .destination = {
            .pixels = c->dst,
            .width = c->phys_w,
            .height = c->phys_h,
            .stride_bytes = stride_bytes(c->phys_w),
            .color_bytes = COLOR_BYTES,
        },
        .logical_area = {
            .x1 = 0, .y1 = c->y0, .x2 = LOGICAL_W - 1, .y2 = c->y1,
        },
        .rotation = c->rotation,
    };
    if (c->ppa != NULL && c->rotation != ESP_DISPLAY_PRESENT_ROTATE_0) {
        (void)esp_display_present_ppa_rotate_copy(c->ppa, &request);
        return;
    }
    esp_display_present_rotate_copy(&request);
}

static void run_complement(void *ctx)
{
    rotate_ctx_t *c = ctx;
    esp_display_present_area_t dirty = {
        .x1 = 0, .y1 = c->y0, .x2 = LOGICAL_W - 1, .y2 = c->y1,
    };
    uint8_t joined = 0;
    esp_display_present_area_t unrendered[16];
    uint16_t count = 0;
    if (esp_display_present_build_unrendered_area_list(
                &dirty, &joined, 1, LOGICAL_W, LOGICAL_H, unrendered, 16,
                &count) != ESP_OK) {
        return;
    }
    for (uint16_t i = 0; i < count; ++i) {
        esp_display_present_area_t physical;
        if (esp_display_present_geometry_map_logical_area_to_physical(
                    c->rotation,
        (esp_display_present_size_t) {
        .width = LOGICAL_W, .height = LOGICAL_H,
    },
    &unrendered[i],
    &physical) != ESP_OK) {
            continue;
        }
        uint16_t w = (uint16_t)(physical.x2 - physical.x1 + 1);
        uint16_t h = (uint16_t)(physical.y2 - physical.y1 + 1);
        size_t row_bytes = stride_bytes(w);
        for (uint16_t row = 0; row < h; ++row) {
            size_t off =
                ((size_t)(physical.y1 + row) * c->phys_w +
                 (size_t)physical.x1) *
                COLOR_BYTES;
            memcpy(c->dst + off, c->src + off, row_bytes);
        }
    }
}

void bench_present_modes_run(void)
{
    const size_t max_fb_bytes =
        (size_t)LOGICAL_W * LOGICAL_H * COLOR_BYTES;
    uint8_t *tile = heap_caps_malloc(
                        DRAWBUF_BYTES,
                        MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    uint8_t *logical = alloc_fb(max_fb_bytes);
    uint8_t *draw_fb = alloc_fb(max_fb_bytes);
    uint8_t *disp_fb = alloc_fb(max_fb_bytes);
    if (tile == NULL || logical == NULL || draw_fb == NULL || disp_fb == NULL) {
        printf("bench: present modes skipped (OOM)\n");
        free(tile);
        free(logical);
        free(draw_fb);
        free(disp_fb);
        return;
    }
    memset(tile, 0x5A, DRAWBUF_BYTES);
    memset(logical, 0xA5, max_fb_bytes);
    memset(disp_fb, 0x3C, max_fb_bytes);

    void *ppa = NULL;
    if (esp_display_present_ppa_register_srm_client(2, &ppa) != ESP_OK) {
        ppa = NULL;
    }

    static const esp_display_present_rotation_t rotations[] = {
        ESP_DISPLAY_PRESENT_ROTATE_0,
        ESP_DISPLAY_PRESENT_ROTATE_90,
        ESP_DISPLAY_PRESENT_ROTATE_180,
        ESP_DISPLAY_PRESENT_ROTATE_270,
    };
    static const struct {
        const char *label;
        bool full;
    } covers[] = {
        { "dirty", false },
        { "full", true },
    };

    bench_row_t rows[48];
    size_t row_count = 0;
    const uint16_t dirty_y1 = (uint16_t)(DIRTY_H - 1);
    const uint16_t full_y1 = (uint16_t)(LOGICAL_H - 1);

    for (size_t ri = 0; ri < sizeof(rotations) / sizeof(rotations[0]); ++ri) {
        uint16_t phys_w = 0;
        uint16_t phys_h = 0;
        physical_size(rotations[ri], &phys_w, &phys_h);
        for (size_t ci = 0; ci < sizeof(covers) / sizeof(covers[0]); ++ci) {
            const uint16_t y1 = covers[ci].full ? full_y1 : dirty_y1;
            rotate_ctx_t ctx = {
                .dst = draw_fb,
                .src = tile,
                .y0 = 0,
                .y1 = y1,
                .phys_w = phys_w,
                .phys_h = phys_h,
                .rotation = rotations[ri],
                .ppa = ppa,
            };
            snprintf(rows[row_count].name, sizeof(rows[row_count].name),
                     "partition_rot%s_%s", rotation_name(rotations[ri]),
                     covers[ci].label);
            rows[row_count].us_per_frame = bench_us(run_partition, &ctx);
            ++row_count;

            ctx.src = logical;
            snprintf(rows[row_count].name, sizeof(rows[row_count].name),
                     "transform_rot%s_%s%s", rotation_name(rotations[ri]),
                     covers[ci].label, ppa != NULL ? "+ppa" : "");
            rows[row_count].us_per_frame = bench_us(run_transform, &ctx);
            ++row_count;
        }

        rotate_ctx_t complement = {
            .dst = draw_fb,
            .src = disp_fb,
            .y0 = 0,
            .y1 = dirty_y1,
            .phys_w = phys_w,
            .phys_h = phys_h,
            .rotation = rotations[ri],
        };
        snprintf(rows[row_count].name, sizeof(rows[row_count].name),
                 "repair_complement_rot%s", rotation_name(rotations[ri]));
        rows[row_count].us_per_frame = bench_us(run_complement, &complement);
        ++row_count;
    }

    float baseline = rows[0].us_per_frame > 0.0f ? rows[0].us_per_frame : 1.0f;
    printf("\n");
    printf("bench: present modes (logical %dx%d drawbuf=%d dirty_h=%d "
           "iters=%d ppa=%s)\n",
           LOGICAL_W, LOGICAL_H, DRAWBUF_BYTES, DIRTY_H, ITERS,
           ppa != NULL ? "yes" : "no");
    printf("bench: %-36s %10s %8s\n", "path", "us/frame", "rel");
    for (size_t i = 0; i < row_count; ++i) {
        printf("bench: %-36s %10.1f %7.2fx\n", rows[i].name,
               (double)rows[i].us_per_frame,
               (double)(rows[i].us_per_frame / baseline));
    }
    printf("\n");

    if (ppa != NULL) {
        esp_display_present_ppa_unregister_client(ppa);
    }
    free(tile);
    free(logical);
    free(draw_fb);
    free(disp_fb);
}
