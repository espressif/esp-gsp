/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#include "gsp/gsp_component.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gsp_runtime gsp_runtime_t;
typedef struct gsp_update_transaction gsp_update_transaction_t;

/** Deprecated 0.2.x raw runtime view. Prefer esp_gsp_component_*(). */
typedef struct {
    gsp_runtime_t *runtime;
    const gsp_component_entry_t *entry;
} gsp_component_view_t;

gsp_err_t gsp_component_find(const gsp_component_directory_t *directory,
                             gsp_runtime_t *runtime,
                             gsp_component_key_t key,
                             gsp_component_view_t *out_view);
gsp_err_t gsp_component_get(const gsp_component_view_t *view,
                            gsp_component_prop_kind_t prop,
                            gsp_value_t *out_value);
gsp_err_t gsp_component_get_property(
    const gsp_component_view_t *view, gsp_property_key_t property,
    gsp_value_t *out_value);
gsp_err_t gsp_component_get_property_info(
    const gsp_component_view_t *view, gsp_property_key_t property,
    gsp_property_info_t *out_info);
gsp_err_t gsp_component_get_property_info_at(
    const gsp_component_view_t *view, size_t index,
    gsp_property_info_t *out_info);
gsp_err_t gsp_component_set(gsp_update_transaction_t *transaction,
                            const gsp_component_view_t *view,
                            gsp_component_prop_kind_t prop,
                            const gsp_value_t *value);
gsp_err_t gsp_component_set_property(
    gsp_update_transaction_t *transaction,
    const gsp_component_view_t *view, gsp_property_key_t property,
    const gsp_value_t *value);
gsp_err_t gsp_component_set_many(
    gsp_update_transaction_t *transaction,
    const gsp_component_directory_t *directory,
    const gsp_component_update_t *updates,
    size_t count);
gsp_err_t gsp_component_set_properties(
    gsp_update_transaction_t *transaction,
    const gsp_component_directory_t *directory,
    const gsp_component_property_update_t *updates,
    size_t count);

#ifdef __cplusplus
}
#endif
