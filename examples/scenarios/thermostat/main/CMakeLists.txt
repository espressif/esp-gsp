# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
set(gsp_component esp-gsp)
get_filename_component(gsp_source_root "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
if(EXISTS "${gsp_source_root}/tools/gspc/Cargo.toml")
    get_filename_component(gsp_component "${gsp_source_root}" NAME)
endif()
idf_component_register(SRCS "app_main.c" "thermostat_ui.c" "knob_input.c"
    INCLUDE_DIRS "." PRIV_REQUIRES ${gsp_component} esp_timer esp_lcd hw_init esp_driver_gpio)
if(NOT CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM OR NOT CONFIG_EXAMPLE_DISPLAY_ROTATION_0)
    message(FATAL_ERROR "thermostat requires 240x240 SPI_WITHOUT_PSRAM, rotation 0")
endif()
gsp_add_bundle(${COMPONENT_LIB} SCENES ../scenes/thermostat.json PIXEL_FORMAT rgb565)
