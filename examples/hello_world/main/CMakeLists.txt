# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

idf_component_register(
    SRCS "app_main.c" "hello_ui.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp-gsp esp_timer esp_lcd hw_init
)

# The scene matching the selected panel; gsp_add_bundle compiles it
# with gspc at build time, embeds the bundle (bundle_start/_end) and
# exposes the generated headers — no committed artifacts, no manual
# regenerate step.
if(CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI)
    set(hello_scene ../scenes/hello_1024.json)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_RGB OR CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
    if(CONFIG_EXAMPLE_DISPLAY_ROTATION_90 OR
       CONFIG_EXAMPLE_DISPLAY_ROTATION_270)
        set(hello_scene ../scenes/hello_480.json)
    else()
        set(hello_scene ../scenes/hello_800.json)
    endif()
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM)
    set(hello_scene ../scenes/hello_320.json)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM)
    set(hello_scene ../scenes/hello_240.json)
else()
    set(hello_scene ../scenes/hello_360.json)
endif()

set(hello_pixel_format rgb565)
if(GSP_HELLO_RGB888)
    if(NOT CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI AND
       NOT CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
        message(FATAL_ERROR
            "GSP_HELLO_RGB888 requires MIPI-DPI or 24-bit RGB")
    endif()
    set(hello_pixel_format rgb888)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE GSP_HELLO_RGB888=1)
endif()

gsp_add_bundle(${COMPONENT_LIB}
    SCENES ${hello_scene}
    PIXEL_FORMAT ${hello_pixel_format})
