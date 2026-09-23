/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if SOC_DMA2D_SUPPORTED

#include "esp_attr.h"
#include "esp_idf_version.h"
#include "esp_private/dma2d.h"

/* IDF 6.0.1 changed this private layout. Keep the ability in the consuming
 * build so source and prebuilt cores both follow the final firmware's IDF. */
const void *esp_gsp_idf_dma2d_copy_ability(void)
{
    /* Short bursts reduce scanout contention; keep this in internal RAM. */
    static const DRAM_ATTR dma2d_transfer_ability_t ability = {
        .desc_burst_en = true,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 1)
        .data_burst_length = 16,
        .access_ext_mem = true,
#else
        .data_burst_length = DMA2D_DATA_BURST_LENGTH_16,
#endif
    };
    return &ability;
}

#endif
