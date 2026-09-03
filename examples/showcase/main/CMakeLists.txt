# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp-gsp esp_timer esp_lcd hw_init
)

if(CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI)
    set(showcase_suffix 1024)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_RGB OR
       CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
    if(CONFIG_EXAMPLE_DISPLAY_ROTATION_90 OR
       CONFIG_EXAMPLE_DISPLAY_ROTATION_270)
        message(FATAL_ERROR
            "GSP Showcase currently supports landscape RGB panels only")
    endif()
    set(showcase_suffix 800)
else()
    message(FATAL_ERROR
        "GSP Showcase supports 1024x600 MIPI and 800x480 RGB panels")
endif()

set(showcase_pixel_format rgb565)
if(GSP_SHOWCASE_RGB888)
    set(showcase_pixel_format rgb888)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE
        GSP_SHOWCASE_RGB888=1)
endif()

gsp_add_bundle(${COMPONENT_LIB}
    SCENES
        ../scenes/showcase_command_${showcase_suffix}.json
        ../scenes/showcase_gallery_${showcase_suffix}.json
        ../scenes/showcase_lab_${showcase_suffix}.json
    PIXEL_FORMAT ${showcase_pixel_format})
