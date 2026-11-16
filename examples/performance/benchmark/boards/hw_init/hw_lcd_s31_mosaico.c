/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */

/* LCD-only adaptation of esp-mosaico-bsp onboard/display.c and
 * onboard/esp_mosaico.c. The caller initializes board power first. */

#include "hw_init.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_lcd_co5300.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

#if !CONFIG_IDF_TARGET_ESP32S31
#error "S31_MOSAICO requires ESP32-S31"
#endif

#define MOSAICO_LCD_WIDTH      480
#define MOSAICO_LCD_HEIGHT     480
#define MOSAICO_LCD_CLOCK_HZ   (40 * 1000 * 1000)
#define MOSAICO_LCD_CS         GPIO_NUM_50
#define MOSAICO_LCD_TE         GPIO_NUM_43
#define MOSAICO_LCD_DATA0      GPIO_NUM_36
#define MOSAICO_LCD_DATA1      GPIO_NUM_51
#define MOSAICO_LCD_DATA2      GPIO_NUM_35
#define MOSAICO_LCD_DATA3      GPIO_NUM_9

static const char *TAG = "hw_s31_mosaico";

/* BSP cold-start sequence with TE enabled. No retained splash is adopted. */
static const co5300_lcd_init_cmd_t s_vendor_init[] = {
    {0x11, NULL, 0, 600},
    {0xFE, (uint8_t[]){0x20}, 1, 0},
    {0x19, (uint8_t[]){0x10}, 1, 0},
    {0x1C, (uint8_t[]){0xA0}, 1, 0},
    {0xFE, (uint8_t[]){0x00}, 1, 0},
    {0xC4, (uint8_t[]){0x80}, 1, 0},
    {0x3A, (uint8_t[]){0x55}, 1, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x53, (uint8_t[]){0x20}, 1, 0},
    {0x51, (uint8_t[]){0xFF}, 1, 0},
    {0x63, (uint8_t[]){0xFF}, 1, 0},
    {0x2A, (uint8_t[]){0x00, 0x00, 0x01, 0xDF}, 4, 0},
    {0x2B, (uint8_t[]){0x00, 0x00, 0x01, 0xDF}, 4, 0},
    {0x29, NULL, 0, 600},
};

static esp_err_t get_lcd_pins(gpio_num_t *clock, gpio_num_t *reset)
{
    uint16_t version = 0;
    ESP_RETURN_ON_ERROR(esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA, &version, 16), TAG,
                        "read hardware version failed");
    switch (version) {
    case 0x0100:
        *clock = GPIO_NUM_44;
        *reset = GPIO_NUM_42;
        break;
    case 0x0101:
    case 0x0102:
        *clock = GPIO_NUM_42;
        *reset = GPIO_NUM_44;
        break;
    default:
        ESP_LOGE(TAG, "Unsupported hardware version v%u.%u (raw=0x%04X)",
                 (unsigned)(version >> 8), (unsigned)(version & 0xFFU), (unsigned)version);
        return ESP_ERR_NOT_SUPPORTED;
    }
    ESP_LOGI(TAG, "Hardware version v%u.%u: CLK=%d RST=%d",
             (unsigned)(version >> 8), (unsigned)(version & 0xFFU), (int)*clock, (int)*reset);
    return ESP_OK;
}

esp_err_t hw_lcd_init(esp_display_present_target_config_t *out_target_config)
{
    ESP_RETURN_ON_FALSE(out_target_config, ESP_ERR_INVALID_ARG, TAG, "missing target");
    gpio_num_t clock;
    gpio_num_t reset;
    ESP_RETURN_ON_ERROR(get_lcd_pins(&clock, &reset), TAG, "detect board mapping failed");

    esp_err_t ret;
    bool bus_initialized = false;
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_handle_t panel = NULL;
    /* The GSP presenter can submit a complete frame, unlike the BSP LVGL path. */
    const spi_bus_config_t bus = CO5300_PANEL_BUS_QSPI_CONFIG(
                                     clock, MOSAICO_LCD_DATA0, MOSAICO_LCD_DATA1, MOSAICO_LCD_DATA2, MOSAICO_LCD_DATA3,
                                     MOSAICO_LCD_WIDTH * MOSAICO_LCD_HEIGHT * sizeof(uint16_t));
    ESP_GOTO_ON_ERROR(spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO), fail, TAG, "QSPI bus");
    bus_initialized = true;
    const gpio_num_t pins[] = {
        clock, MOSAICO_LCD_DATA0, MOSAICO_LCD_DATA1, MOSAICO_LCD_DATA2, MOSAICO_LCD_DATA3,
    };
    for (size_t i = 0; i < sizeof(pins) / sizeof(pins[0]); ++i) {
        ESP_GOTO_ON_ERROR(gpio_set_drive_capability(pins[i], GPIO_DRIVE_CAP_2), fail, TAG,
                          "set QSPI GPIO%d drive strength", (int)pins[i]);
    }

    esp_lcd_panel_io_spi_config_t io_config = CO5300_PANEL_IO_QSPI_CONFIG(MOSAICO_LCD_CS, NULL, NULL);
    io_config.pclk_hz = MOSAICO_LCD_CLOCK_HZ;
    /* The presenter submits PSRAM-backed frames. Keep DMA in external RAM
     * rather than allocating an internal copy of a complete 480x480 frame. */
    io_config.flags.psram_dma_direct = true;
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                      &io_config, &io), fail, TAG, "panel IO");
    const co5300_vendor_config_t vendor = {
        .init_cmds = s_vendor_init,
        .init_cmds_size = sizeof(s_vendor_init) / sizeof(s_vendor_init[0]),
        .flags.use_qspi_interface = true,
    };
    const esp_lcd_panel_dev_config_t config = {
        .reset_gpio_num = reset,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = (void *) &vendor,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_co5300(io, &config, &panel), fail, TAG, "panel create");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(panel), fail, TAG, "panel reset");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(panel), fail, TAG, "panel init");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_set_gap(panel, 0, 0), fail, TAG, "panel gap");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_swap_xy(panel, false), fail, TAG, "panel axis swap");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_mirror(panel, false, false), fail, TAG, "panel mirror");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_disp_on_off(panel, true), fail, TAG, "panel on");

    *out_target_config = (esp_display_present_target_config_t) {
        .hw = {
            .panel = panel,
            .io = io,
            .panel_type = ESP_DISPLAY_PRESENT_PANEL_IO,
            .input_pixel_format = ESP_DISPLAY_PRESENT_PIXEL_FORMAT_RGB565,
            .rotation = ESP_DISPLAY_PRESENT_ROTATE_0,
            .swap_bytes = true,
            .te_enabled = true,
            .te_sync = {
                .gpio_num = MOSAICO_LCD_TE,
                .bus_freq_hz = MOSAICO_LCD_CLOCK_HZ,
                .data_lines = 4,
            },
        },
        .fb.mode = ESP_DISPLAY_PRESENT_MODE_AUTO,
    };
    ESP_LOGI(TAG, "CO5300 480x480 RGB565 QSPI40MHz TE=43 gap=0,0 rotation=0 drive=2 psram_dma_direct=1");
    return ESP_OK;

fail:
    if (panel) {
        esp_lcd_panel_del(panel);
    }
    if (io) {
        esp_lcd_panel_io_del(io);
    }
    if (bus_initialized) {
        spi_bus_free(SPI2_HOST);
    }
    return ret;
}
