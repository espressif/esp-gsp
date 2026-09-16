/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once
#include "esp_gsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_gsp_font_file esp_gsp_font_file_t;

/** Loads a TTF/OTF/TTC or a GSPB font catalog from a mounted filesystem.
 * max_bytes is a required, nonzero limit on the file size. Oversized files
 * fail before allocation. The entire file stays in RAM (PSRAM preferred on
 * ESP-IDF); this is not on-demand glyph IO. Catalogs are CRC-checked; dynamic
 * fonts are checked by FreeType when the UI starts. Their signature is checked
 * here. TTC uses its first face. Dynamic fonts require gsp_enable_freetype()
 * in source builds. With dynamic fallback, each static GFB must contain at
 * most 32768 glyphs; UI startup rejects larger packs with NOT_SUPPORTED.
 * Call from an application task: open performs blocking file IO. On failure
 * *out_font is NULL. Glyph caches and FreeType working memory are separate.
 */
esp_gsp_err_t esp_gsp_font_file_open(const char *path, size_t max_bytes,
                                     esp_gsp_font_file_t **out_font);

/** Applies a loaded font to an initialized configuration before UI startup.
 * Sets ttf/ttf_size for a dynamic font, or font_catalog for a linked catalog;
 * the other font source is preserved. No ownership is transferred. Keep the
 * handle alive until every UI using this configuration has stopped successfully.
 * Changing this configuration does not replace a font in a running UI.
 */
esp_gsp_err_t esp_gsp_font_file_apply(const esp_gsp_font_file_t *font,
                                      esp_gsp_config_t *config);

/** Frees a loaded font after all borrowing UIs have stopped, or startup failed.
 * NULL is accepted. Configurations that borrowed it must not be reused without
 * applying another font or clearing the corresponding font source fields.
 */
void esp_gsp_font_file_close(esp_gsp_font_file_t *font);

#ifdef __cplusplus
}
#endif
