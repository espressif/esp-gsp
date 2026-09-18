/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#include "hw_init.h"

#if HW_USE_TOUCH

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#if CONFIG_IDF_TARGET_ESP32S3
#include "esp_psram.h"
#endif
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "hw_touch";

#define TOUCH_I2C_SPEED_HZ 400000

#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI
#include "esp_lcd_touch_gt911.h"
#define PIN_NUM_TOUCH_SDA 7
#define PIN_NUM_TOUCH_SCL 8
#define TOUCH_NAME "GT911"
/* The GT911 on this board is mounted inverted relative to the panel
 * scan direction: both axes mirror to align touch with pixels
 * (the integration maps physical samples into logical coordinates). */
#define TOUCH_MIRROR_X 1
#define TOUCH_MIRROR_Y 1

#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB
#include "esp_lcd_touch_gt1151.h"
#if CONFIG_IDF_TARGET_ESP32S31
#define PIN_NUM_TOUCH_SDA 0
#define PIN_NUM_TOUCH_SCL 1
#elif CONFIG_IDF_TARGET_ESP32S3
/* ESP32-S3-LCD-EV-Board SUB3 uses 8/18 on R8 modules and 47/48 on R16. */
#define PIN_NUM_TOUCH_SDA GPIO_NUM_8
#define PIN_NUM_TOUCH_SCL GPIO_NUM_18
#else
#error "RGB touch hw_init supports only ESP32-S3 and ESP32-S31"
#endif
#define TOUCH_NAME "GT1151"
#define TOUCH_MIRROR_X 0
#define TOUCH_MIRROR_Y 0

#elif CONFIG_EXAMPLE_LCD_INTERFACE_QSPI
#include "esp_lcd_touch_cst816s.h"
#define PIN_NUM_TOUCH_SDA 2
#define PIN_NUM_TOUCH_SCL 1
/* The CST816S sleeps between touches; the INT line is required to
 * wake it for register reads. */
#define PIN_NUM_TOUCH_INT 10
#define TOUCH_NAME "CST816S"
#define TOUCH_MIRROR_X 0
#define TOUCH_MIRROR_Y 0

#elif CONFIG_EXAMPLE_LCD_INTERFACE_QSPI_CO5300
#include "esp_lcd_touch_cst9217.h"
#define PIN_NUM_TOUCH_SDA 0
#define PIN_NUM_TOUCH_SCL 1
#define PIN_NUM_TOUCH_INT 6
#define TOUCH_NAME "CST9217"
#define TOUCH_MIRROR_X 0
#define TOUCH_MIRROR_Y 0

#elif CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM
#include "esp_lcd_touch_gt911.h"
#define PIN_NUM_TOUCH_SDA 8
#define PIN_NUM_TOUCH_SCL 18
#define PIN_NUM_TOUCH_INT 3
#define TOUCH_NAME "GT911"
#define TOUCH_MIRROR_X 0
#define TOUCH_MIRROR_Y 0
#endif

#ifndef PIN_NUM_TOUCH_INT
#define PIN_NUM_TOUCH_INT GPIO_NUM_NC
#endif

esp_err_t hw_touch_init(esp_lcd_touch_handle_t *out_touch)
{
    ESP_LOGI(TAG, "Initializing touch controller (%s)", TOUCH_NAME);
#if CONFIG_IDF_TARGET_ESP32S3 && CONFIG_EXAMPLE_LCD_INTERFACE_RGB
    gpio_num_t touch_sda = PIN_NUM_TOUCH_SDA;
    gpio_num_t touch_scl = PIN_NUM_TOUCH_SCL;
    if (esp_psram_get_size() > (8 * 1024 * 1024)) {
        touch_sda = GPIO_NUM_47;
        touch_scl = GPIO_NUM_48;
        ESP_LOGI(TAG, "ESP32-S3 RGB touch module profile: R16");
    } else {
        ESP_LOGI(TAG, "ESP32-S3 RGB touch module profile: R8/default");
    }
#else
    const gpio_num_t touch_sda = PIN_NUM_TOUCH_SDA;
    const gpio_num_t touch_scl = PIN_NUM_TOUCH_SCL;
#endif
    i2c_master_bus_handle_t bus;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = touch_sda,
        .scl_io_num = touch_scl,
        .i2c_port = I2C_NUM_0,
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_config, &bus), TAG,
                        "I2C bus");

#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI || \
    CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM
    esp_lcd_panel_io_i2c_config_t io_config =
        ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB
    esp_lcd_panel_io_i2c_config_t io_config =
        ESP_LCD_TOUCH_IO_I2C_GT1151_CONFIG();
#elif CONFIG_EXAMPLE_LCD_INTERFACE_QSPI_CO5300
    esp_lcd_panel_io_i2c_config_t io_config =
        ESP_LCD_TOUCH_IO_I2C_CST9217_CONFIG();
    io_config.transaction_timeout_ms = 100;
#else
    esp_lcd_panel_io_i2c_config_t io_config =
        ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
#endif
    io_config.scl_speed_hz = TOUCH_I2C_SPEED_HZ;
    esp_lcd_panel_io_handle_t io;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(bus, &io_config, &io),
                        TAG, "touch io");

    esp_lcd_touch_config_t touch_config = {
        .x_max = HW_LCD_H_RES,
        .y_max = HW_LCD_V_RES,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = PIN_NUM_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .mirror_x = TOUCH_MIRROR_X,
            .mirror_y = TOUCH_MIRROR_Y,
        },
    };
#if CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI || \
    CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM
    esp_err_t ret = esp_lcd_touch_new_i2c_gt911(io, &touch_config,
                    out_touch);
#elif CONFIG_EXAMPLE_LCD_INTERFACE_RGB
    esp_err_t ret = esp_lcd_touch_new_i2c_gt1151(io, &touch_config,
                    out_touch);
#elif CONFIG_EXAMPLE_LCD_INTERFACE_QSPI_CO5300
    esp_err_t ret = esp_lcd_touch_new_i2c_cst9217(io, &touch_config,
                    out_touch);
#else
    esp_err_t ret = esp_lcd_touch_new_i2c_cst816s(io, &touch_config,
                    out_touch);
#endif
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "%s not responding; running without touch",
                 TOUCH_NAME);
        esp_lcd_panel_io_del(io);
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

#else

esp_err_t hw_touch_init(esp_lcd_touch_handle_t *out_touch)
{
    (void)out_touch;
    return ESP_ERR_NOT_SUPPORTED;
}

#endif /* HW_USE_TOUCH */
