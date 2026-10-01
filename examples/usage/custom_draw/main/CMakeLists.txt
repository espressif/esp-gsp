# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

set(gsp_component esp-gsp)
get_filename_component(gsp_source_root "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
if(EXISTS "${gsp_source_root}/tools/gspc/Cargo.toml")
    get_filename_component(gsp_component "${gsp_source_root}" NAME)
endif()

idf_component_register(
    SRCS "app_main.c" "custom_draw.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES ${gsp_component} esp_timer hw_init)

if(CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI)
    set(custom_draw_screen_w 1024)
    set(custom_draw_screen_h 600)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_RGB OR
       CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
    set(custom_draw_screen_w 800)
    set(custom_draw_screen_h 480)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM)
    set(custom_draw_screen_w 320)
    set(custom_draw_screen_h 240)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM)
    set(custom_draw_screen_w 240)
    set(custom_draw_screen_h 240)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_QSPI)
    set(custom_draw_screen_w 360)
    set(custom_draw_screen_h 360)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_QSPI_CO5300)
    set(custom_draw_screen_w 480)
    set(custom_draw_screen_h 480)
else()
    message(FATAL_ERROR
        "custom_draw: no supported EXAMPLE_LCD_INTERFACE profile selected")
endif()

if(CONFIG_EXAMPLE_DISPLAY_ROTATION_90 OR
   CONFIG_EXAMPLE_DISPLAY_ROTATION_270)
    set(custom_draw_rotated_w "${custom_draw_screen_w}")
    set(custom_draw_screen_w "${custom_draw_screen_h}")
    set(custom_draw_screen_h "${custom_draw_rotated_w}")
endif()

set(custom_draw_pixel_format rgb565)
if(GSP_CUSTOM_DRAW_RGB888)
    if(NOT CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI AND
       NOT CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
        message(FATAL_ERROR
            "GSP_CUSTOM_DRAW_RGB888 requires MIPI-DSI or 24-bit RGB")
    endif()
    set(custom_draw_pixel_format rgb888)
    target_compile_definitions(${COMPONENT_LIB}
        PRIVATE GSP_CUSTOM_DRAW_RGB888=1)
endif()

math(EXPR custom_draw_canvas_x
    "(${custom_draw_screen_w} - 200) / 2")
math(EXPR custom_draw_canvas_y
    "(${custom_draw_screen_h} - 100) / 2")
file(TO_CMAKE_PATH "${CMAKE_CURRENT_LIST_DIR}/../assets/placeholder.png"
     custom_draw_placeholder_json)
set(CUSTOM_DRAW_SCREEN_W "${custom_draw_screen_w}")
set(CUSTOM_DRAW_SCREEN_H "${custom_draw_screen_h}")
set(CUSTOM_DRAW_CANVAS_X "${custom_draw_canvas_x}")
set(CUSTOM_DRAW_CANVAS_Y "${custom_draw_canvas_y}")
set(CUSTOM_DRAW_PLACEHOLDER "${custom_draw_placeholder_json}")
set(custom_draw_scene
    "${CMAKE_CURRENT_BINARY_DIR}/custom_draw_scene.json")
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/../scenes/custom_draw.json.in"
    "${custom_draw_scene}" @ONLY)

gsp_add_bundle(${COMPONENT_LIB}
    SCENES "${custom_draw_scene}"
    PIXEL_FORMAT ${custom_draw_pixel_format})

# Compile hooks with this component's configuration and generated-header
# dependencies; the helper adds independent strong retention anchors.
gsp_add_hooks(${COMPONENT_LIB} SOURCES "custom_draw_hooks.c")
