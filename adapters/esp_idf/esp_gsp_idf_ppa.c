/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_PPA_SUPPORTED

#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_private/esp_cache_private.h"

/* Resolve the driver contract in the consuming build, including when the
 * renderer comes from a prebuilt archive compiled with a different IDF. */
size_t esp_gsp_idf_ppa_output_alignment(const void *buffer)
{
    uint32_t caps = MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA;
#ifdef ESP_GSP_PPA_HAS_PER_BUFFER_ALIGNMENT
    if (esp_ptr_internal(buffer)) {
        caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA;
    }
#else
    (void)buffer;
#endif
    size_t alignment = 0;
    if (esp_cache_get_alignment(caps, &alignment) != ESP_OK) {
        return 0;
    }
    return alignment;
}

#endif
