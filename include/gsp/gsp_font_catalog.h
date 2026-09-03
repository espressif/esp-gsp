/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gsp/gsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gsp_font_catalog gsp_font_catalog_t;

/** Opens a borrowed GSPB containing GFB members keyed by content_id. */
gsp_err_t gsp_font_catalog_create(const void *data, size_t size,
                                  bool verify_crc,
                                  gsp_font_catalog_t **out_catalog);
void gsp_font_catalog_destroy(gsp_font_catalog_t *catalog);

/** Finds a GFB member by Asset ID (GSP_ERR_NOT_FOUND when absent). */
gsp_err_t gsp_font_catalog_find(const gsp_font_catalog_t *catalog,
                                uint32_t asset_id,
                                const void **out_data, size_t *out_size);

#ifdef __cplusplus
}
#endif
