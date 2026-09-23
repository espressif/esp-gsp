# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

# Source checkouts may have a worktree-specific directory name. Registry
# packages omit the compiler source and use the canonical component name.
set(gsp_component esp-gsp)
get_filename_component(gsp_source_root "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
if(EXISTS "${gsp_source_root}/tools/gspc/Cargo.toml")
    get_filename_component(gsp_component "${gsp_source_root}" NAME)
endif()

idf_component_register(
    SRCS "app_main.c" "bench_present_modes.c" "bench_workload.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES ${gsp_component} esp_timer esp_lcd hw_init esp_display_present
)

idf_build_get_property(build_components BUILD_COMPONENTS)
if("espressif__esp_display_present" IN_LIST build_components)
    idf_component_get_property(present_dir espressif__esp_display_present COMPONENT_DIR)
else()
    idf_component_get_property(present_dir esp_display_present COMPONENT_DIR)
endif()
target_include_directories(${COMPONENT_LIB} PRIVATE
    "${present_dir}/private_include")

set(bench_pixel_format rgb565)
if(GSP_BENCH_RGB888)
    set(bench_pixel_format rgb888)
    if(CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI)
        set(bench_scene ../scenes/bench_rgb888_1024.json)
        set(bench_alt_scene ../scenes/bench_alt_1024.json)
    elseif(CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
        set(bench_scene ../scenes/bench_rgb888_800.json)
        set(bench_alt_scene ../scenes/bench_alt_800.json)
    else()
        message(FATAL_ERROR
            "GSP_BENCH_RGB888 needs MIPI-DPI or 24-bit RGB")
    endif()
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI)
    set(bench_scene ../scenes/bench_1024.json)
    set(bench_alt_scene ../scenes/bench_alt_1024.json)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_RGB OR
       CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
    if(CONFIG_EXAMPLE_DISPLAY_ROTATION_90 OR
       CONFIG_EXAMPLE_DISPLAY_ROTATION_270)
        set(bench_scene ../scenes/bench_480.json)
        set(bench_alt_scene ../scenes/bench_alt_480.json)
    else()
        set(bench_scene ../scenes/bench_800.json)
        set(bench_alt_scene ../scenes/bench_alt_800.json)
    endif()
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITH_PSRAM)
    set(bench_scene ../scenes/bench_320.json)
    set(bench_alt_scene ../scenes/bench_alt_320.json)
elseif(CONFIG_EXAMPLE_LCD_INTERFACE_SPI_WITHOUT_PSRAM)
    set(bench_scene ../scenes/bench_240.json)
    set(bench_alt_scene ../scenes/bench_alt_240.json)
else()
    set(bench_scene ../scenes/bench_360.json)
    set(bench_alt_scene ../scenes/bench_alt_360.json)
endif()

# Keep C and the bundle embed in lockstep with the project option.
if(GSP_BENCH_RGB888)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE GSP_BENCH_RGB888=1)
endif()

set(bench_bundle_options "")
if(GSP_BENCH_DEPLOYABLE)
    list(APPEND bench_bundle_options DEPLOYABLE)
endif()
gsp_add_bundle(${COMPONENT_LIB}
    SCENES ${bench_scene} ${bench_alt_scene}
    PIXEL_FORMAT ${bench_pixel_format}
    ${bench_bundle_options})
