/* SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT */
#include "esp_gsp_esp_lcd.h"
#include "esp_gsp_assets.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "hw_init.h"
#include "bundle_gsp.h"
#include "media.h"
static const char *TAG = "asset_example";

static esp_err_t mount_storage(void)
{
#if CONFIG_GSP_ASSET_USE_EXISTING_MOUNT
    /* The application's BSP mounts NAND/SDMMC at GSP_ASSET_MOUNT_PATH. */
    return ESP_OK;
#else
    if (CONFIG_GSP_ASSET_SD_MOSI < 0 || CONFIG_GSP_ASSET_SD_MISO < 0 ||
            CONFIG_GSP_ASSET_SD_CLK < 0 || CONFIG_GSP_ASSET_SD_CS < 0) {
        ESP_LOGW(TAG, "Configure SD GPIOs in menuconfig -> External asset example; keeping built-in images");
        return ESP_ERR_INVALID_STATE;
    }
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
#if CONFIG_GSP_ASSET_SD_SPI3
    host.slot = SPI3_HOST;
#endif
    spi_bus_config_t bus = {
        .mosi_io_num = CONFIG_GSP_ASSET_SD_MOSI, .miso_io_num = CONFIG_GSP_ASSET_SD_MISO,
        .sclk_io_num = CONFIG_GSP_ASSET_SD_CLK, .quadwp_io_num = -1, .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    esp_err_t ret = spi_bus_initialize(host.slot, &bus, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        return ret;
    }
    sdspi_device_config_t device = SDSPI_DEVICE_CONFIG_DEFAULT();
    device.host_id = host.slot; device.gpio_cs = CONFIG_GSP_ASSET_SD_CS;
    esp_vfs_fat_sdmmc_mount_config_t mount = { .format_if_mount_failed = false, .max_files = 4 };
    sdmmc_card_t *card;
    ret = esp_vfs_fat_sdspi_mount(CONFIG_GSP_ASSET_MOUNT_PATH, &host, &device, &mount, &card);
    if (ret != ESP_OK) {
        spi_bus_free(host.slot);
    }
    return ret;
#endif
}

void app_main(void)
{
    esp_display_present_target_config_t display;
    ESP_ERROR_CHECK(hw_lcd_init(&display));
    esp_gsp_config_t app = gsp_bundle_config();
    esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
    lcd.display = display;
    esp_gsp_handle_t ui;
    ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
    esp_err_t ret = mount_storage();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Storage unavailable: %s; built-in images remain", esp_err_to_name(ret));
        return;
    }
    esp_gsp_assets_t *assets;
    ret = gsp_media_open(CONFIG_GSP_ASSET_MOUNT_PATH "/media.gspb", NULL, &assets);
    if (ret != ESP_GSP_OK) {
        ESP_LOGW(TAG, "Copy the generated media.gspb to the mounted filesystem (error=%d)", ret);
        return;
    }
    ESP_ERROR_CHECK(gsp_gallery_spinner_set_asset(ui, assets, gsp_media_spinner, false));
    ESP_ERROR_CHECK(gsp_gallery_fade_set_asset(ui, assets, gsp_media_fade, false));
    /* This example keeps both services alive. Before UI shutdown or storage
     * unmount in an application, require esp_gsp_assets_close(assets) to return
     * ESP_GSP_OK while the UI still services commands/decodes. On timeout keep
     * the handle and retry after progress resumes; do not unmount or stop UI. */
}
