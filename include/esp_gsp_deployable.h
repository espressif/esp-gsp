/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_gsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Parsed self-describing metadata for a deployable GSPB. */
typedef struct esp_gsp_deployable_bundle esp_gsp_deployable_bundle_t;

typedef struct {
    size_t struct_size;
    uint16_t scene_count;
    uint16_t runtime_abi_major;
    uint16_t runtime_abi_minor;
    uint16_t dynamic_image_slots;
    uint32_t target_profile_id;
    uint32_t content_id;
} esp_gsp_deployable_info_t;

/**
 * Opens a borrowed GSPB containing one GMD member for every GSB scene.
 *
 * The input bytes must be 64-byte aligned and remain immutable and valid
 * until the handle is closed. Pass true to verify CRC (the recommended default). When false,
 * structural validation still runs, and make_config preserves this CRC opt-out
 * for subsequent runtime creation.
 */
esp_gsp_err_t esp_gsp_deployable_bundle_open(
    const void *data, size_t size, bool verify_crc,
    esp_gsp_deployable_bundle_t **out_bundle);

/** Returns immutable package requirements discovered during open. */
esp_gsp_err_t esp_gsp_deployable_bundle_get_info(
    const esp_gsp_deployable_bundle_t *bundle,
    esp_gsp_deployable_info_t *out_info);

/**
 * Builds an application configuration backed by the opened bundle.
 * disable_bundle_crc reflects the verify_crc argument used when opening;
 * callers may override it before creating the UI.
 *
 * The deployable handle and its borrowed bytes must outlive the UI instance.
 * Stop the UI before calling esp_gsp_deployable_bundle_close().
 */
esp_gsp_err_t esp_gsp_deployable_bundle_make_config(
    const esp_gsp_deployable_bundle_t *bundle,
    esp_gsp_config_t *out_config);

/** Resolves a scene-local numeric CALL id to its stable authored-name key. */
esp_gsp_err_t esp_gsp_deployable_bundle_action_key(
    const esp_gsp_deployable_bundle_t *bundle, uint16_t scene_id,
    uint16_t action_id, uint32_t *out_key);

/** Releases parsed metadata. The borrowed GSPB bytes are never freed. */
void esp_gsp_deployable_bundle_close(
    esp_gsp_deployable_bundle_t *bundle);

#ifdef __cplusplus
}
#endif
