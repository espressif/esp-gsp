/* SPDX-License-Identifier: Apache-2.0 */
/* LCD-only adapter for the 480x480 GC9503 RGB panel.
 * Uses a 16-bit RGB565 bus at 16 MHz without a bounce buffer. */

#include "hw_init.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_lcd_gc9503.h"
#include "esp_lcd_panel_io_additions.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

static const char *TAG = "hw_s31_gc9503";

esp_err_t hw_lcd_init(esp_display_present_target_config_t *out_target_config)
{
    ESP_RETURN_ON_FALSE(out_target_config, ESP_ERR_INVALID_ARG, TAG, "missing target");
    const gpio_config_t vsync_pre = {
        .pin_bit_mask = 1ULL << GPIO_NUM_45,
                             .mode = GPIO_MODE_OUTPUT,
                             .pull_up_en = true,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&vsync_pre), TAG, "VSYNC precondition");
    ESP_RETURN_ON_ERROR(gpio_set_level(GPIO_NUM_45, 1), TAG, "VSYNC high");
    const spi_line_config_t lines = {
        .cs_io_type = IO_TYPE_GPIO,
        .cs_gpio_num = GPIO_NUM_38,
        .scl_io_type = IO_TYPE_GPIO,
        .scl_gpio_num = GPIO_NUM_61,
        .sda_io_type = IO_TYPE_GPIO,
        .sda_gpio_num = GPIO_NUM_60,
        .io_expander = NULL,
    };
    esp_lcd_panel_io_3wire_spi_config_t io_config = GC9503_PANEL_IO_3WIRE_SPI_CONFIG(lines, 0);
    esp_lcd_panel_io_handle_t io = NULL;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_3wire_spi(&io_config, &io), TAG, "three-wire panel IO");
    const esp_lcd_rgb_panel_config_t rgb = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_width = 16,
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .num_fbs = CONFIG_EXAMPLE_LCD_FRAME_BUFFERS,
        .dma_burst_size = 128,
        .bounce_buffer_size_px = 0,
        .hsync_gpio_num = GPIO_NUM_44,
        .vsync_gpio_num = GPIO_NUM_45,
        .de_gpio_num = GPIO_NUM_43,
        .pclk_gpio_num = GPIO_NUM_40,
        .disp_gpio_num = GPIO_NUM_NC,
        .data_gpio_nums = {
            8, 9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 33, 34, 35, 36
        },
        .timings = GC9503_480_480_PANEL_60HZ_RGB_TIMING(),
        .flags.fb_in_psram = true,
    };
    /* Send initialization commands before RGB creation, then release the
     * three-wire IO. The panel driver owns IO deletion. */
    gc9503_vendor_config_t vendor = {
        .rgb_config = &rgb,
        .flags = {.mirror_by_cmd = false, .auto_del_panel_io = true},
    };
    const esp_lcd_panel_dev_config_t config = {
        .reset_gpio_num = GPIO_NUM_NC,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 18,
        .vendor_config = &vendor,
    };
    esp_err_t ret;
    esp_lcd_panel_handle_t panel = NULL;
    /* On creation failure the driver's auto-delete path may already have
     * consumed IO. Propagate to app_main's fatal startup check, without
     * attempting a second deletion of that handle. */
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_gc9503(io, &config, &panel), TAG, "panel create");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_reset(panel), fail, TAG, "panel reset");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(panel), fail, TAG, "panel init");
    ESP_LOGI(TAG, "GC9503 480x480 RGB565 bus16 panel18 PCLK16MHz bounce0 fb=%u",
             (unsigned)CONFIG_EXAMPLE_LCD_FRAME_BUFFERS);
    *out_target_config = (esp_display_present_target_config_t) {
        .hw = {
            .panel = panel,
            .panel_type = ESP_DISPLAY_PRESENT_PANEL_RGB,
            .input_pixel_format = ESP_DISPLAY_PRESENT_PIXEL_FORMAT_RGB565,
            .rotation = HW_DISPLAY_ROTATION,
        },
        .fb.mode = CONFIG_EXAMPLE_LCD_FRAME_BUFFERS == 2
                   ? ESP_DISPLAY_PRESENT_MODE_DOUBLE_PARTIAL
                   : ESP_DISPLAY_PRESENT_MODE_AUTO,
    };
    return ESP_OK;

fail:
    if (panel) {
        esp_lcd_panel_del(panel);
    }
    return ret;
}
