# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
# Source checkouts may have a worktree-specific directory name. Registry
# packages omit the compiler source and use the canonical component name.
set(gsp_component esp-gsp)
get_filename_component(gsp_source_root "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
if(EXISTS "${gsp_source_root}/tools/gspc/Cargo.toml")
    get_filename_component(gsp_component "${gsp_source_root}" NAME)
endif()

idf_component_register(SRCS "app_main.c" REQUIRES ${gsp_component} hw_init fatfs esp_driver_sdspi esp_driver_spi esp_timer)
gsp_add_bundle(${COMPONENT_LIB} PIXEL_FORMAT rgb565 IMAGE_CACHE_BYTES 262144)
gsp_add_assets(${COMPONENT_LIB} MANIFEST "../assets/assets.json" SYMBOL media)
