# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
# Source checkouts may have a worktree-specific directory name. Registry
# packages omit the compiler source and use the canonical component name.
set(gsp_component esp-gsp)
get_filename_component(gsp_source_root "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
if(EXISTS "${gsp_source_root}/tools/gspc/Cargo.toml")
    get_filename_component(gsp_component "${gsp_source_root}" NAME)
endif()

idf_component_register(SRCS "app_main.c" "coffee_ui.c" INCLUDE_DIRS "."
    PRIV_REQUIRES ${gsp_component} esp_lcd hw_init freertos)
if((NOT CONFIG_EXAMPLE_LCD_INTERFACE_RGB) AND
   (NOT CONFIG_EXAMPLE_LCD_INTERFACE_RGB24))
    message(FATAL_ERROR
        "Coffee machine requires an 800x480 RGB or RGB24 panel")
endif()
if(CONFIG_EXAMPLE_DISPLAY_ROTATION_90 OR
   CONFIG_EXAMPLE_DISPLAY_ROTATION_180 OR
   CONFIG_EXAMPLE_DISPLAY_ROTATION_270)
    message(FATAL_ERROR
        "Coffee machine requires the authored 800x480 rotation-0 layout")
endif()
if(CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
    set(coffee_pixel_format rgb888)
else()
    set(coffee_pixel_format rgb565)
endif()
gsp_add_bundle(${COMPONENT_LIB} SCENES ../scenes/coffee.json
    PIXEL_FORMAT ${coffee_pixel_format})
option(COFFEE_DEMO_PROBE "Exercise the gallery on the display board" OFF)
if(COFFEE_DEMO_PROBE)
    target_sources(${COMPONENT_LIB} PRIVATE "coffee_probe.c")
    target_compile_definitions(${COMPONENT_LIB} PRIVATE COFFEE_DEMO_PROBE=1)
endif()
