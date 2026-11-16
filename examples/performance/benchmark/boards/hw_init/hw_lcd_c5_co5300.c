/* SPDX-License-Identifier: CC0-1.0 */
/* LCD-only adapter for the 466x466 CO5300 QSPI panel.
 * Uses a six-pixel horizontal gap and indirect PSRAM DMA. */

#include "hw_init.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "esp_lcd_co5300.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "hw_c5_co5300";

esp_err_t hw_lcd_init(esp_display_present_target_config_t *out_target_config)
{
    ESP_RETURN_ON_FALSE(out_target_config, ESP_ERR_INVALID_ARG, TAG, "missing target");
    ESP_RETURN_ON_FALSE(esp_psram_is_initialized(), ESP_ERR_INVALID_STATE, TAG, "PSRAM required");
    /* R7 can join touch reset GPIO23 to LCD reset GPIO3. */
    ESP_RETURN_ON_ERROR(gpio_set_direction(GPIO_NUM_23, GPIO_MODE_INPUT), TAG, "touch reset input");
    ESP_RETURN_ON_ERROR(gpio_set_level(GPIO_NUM_2, 0), TAG, "LCD power level");
    ESP_RETURN_ON_ERROR(gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT), TAG, "LCD power output");
    vTaskDelay(pdMS_TO_TICKS(20));

    esp_err_t ret;
    bool bus_initialized = false;
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_handle_t panel = NULL;
    const spi_bus_config_t bus = CO5300_PANEL_BUS_QSPI_CONFIG(
                                     GPIO_NUM_1, GPIO_NUM_0, GPIO_NUM_8, GPIO_NUM_6, GPIO_NUM_9,
                                     HW_LCD_H_RES * 16 * sizeof(uint16_t));
    ESP_GOTO_ON_ERROR(spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO), fail, TAG, "QSPI bus");
    bus_initialized = true;
    esp_lcd_panel_io_spi_config_t io_config = CO5300_PANEL_IO_QSPI_CONFIG(GPIO_NUM_7, NULL, NULL);
    io_config.pclk_hz = 40 * 1000 * 1000;
    /* This board uses indirect PSRAM DMA without TE synchronization. */
    io_config.flags.psram_dma_direct = false;
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                      &io_config, &io), fail, TAG, "panel IO");
    co5300_vendor_config_t vendor = {.flags.use_qspi_interface = true};
    const esp_lcd_panel_dev_config_t config = {
        .reset_gpio_num = GPIO_NUM_3,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_co5300(io, &config, &panel), fail, TAG, "panel create");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(panel), fail, TAG, "panel reset");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(panel), fail, TAG, "panel init");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_set_gap(panel, 6, 0), fail, TAG, "panel gap");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_disp_on_off(panel, true), fail, TAG, "panel on");
    ESP_LOGI(TAG, "CO5300 466x466 RGB565 QSPI40MHz gap=6,0 TE=off psram_dma_direct=0");
    *out_target_config = (esp_display_present_target_config_t) {
        .hw = {
            .panel = panel,
            .io = io,
            .panel_type = ESP_DISPLAY_PRESENT_PANEL_IO,
            .input_pixel_format = ESP_DISPLAY_PRESENT_PIXEL_FORMAT_RGB565,
            .rotation = HW_DISPLAY_ROTATION,
            .swap_bytes = true,
        },
        .fb.mode = ESP_DISPLAY_PRESENT_MODE_AUTO,
    };
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
    gpio_set_level(GPIO_NUM_2, 1);
    return ret;
}
