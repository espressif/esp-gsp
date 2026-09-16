# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
idf_component_register(SRCS "app_main.c" REQUIRES esp-gsp hw_init fatfs esp_driver_sdspi esp_driver_spi)
gsp_add_bundle(${COMPONENT_LIB} PIXEL_FORMAT rgb565 IMAGE_CACHE_BYTES 262144)
gsp_add_assets(${COMPONENT_LIB} MANIFEST "../assets/assets.json" SYMBOL media)
