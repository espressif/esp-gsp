# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
cmake_minimum_required(VERSION 3.20)
project(hello_backend LANGUAGES C)
# The example can live inside the component or be copied into an IDF project.
# The runner passes this cache variable; direct CMake also accepts the env var.
if(NOT ESP_GSP_COMPONENT_DIR AND DEFINED ENV{ESP_GSP_COMPONENT_DIR})
    set(ESP_GSP_COMPONENT_DIR "$ENV{ESP_GSP_COMPONENT_DIR}")
endif()
if(NOT ESP_GSP_COMPONENT_DIR)
    foreach(candidate IN ITEMS
            "../managed_components/espressif__esp-gsp"
            "../components/espressif__esp-gsp"
            "../../..")
        get_filename_component(candidate "${candidate}" ABSOLUTE)
        if(EXISTS "${candidate}/tools/sim_bridge/CMakeLists.txt")
            set(ESP_GSP_COMPONENT_DIR "${candidate}")
            break()
        endif()
    endforeach()
endif()
if(NOT EXISTS "${ESP_GSP_COMPONENT_DIR}/tools/sim_bridge/CMakeLists.txt")
    message(FATAL_ERROR "Install espressif/esp-gsp first, or set ESP_GSP_COMPONENT_DIR to its component directory")
endif()
add_subdirectory("${ESP_GSP_COMPONENT_DIR}/tools/sim_bridge" sim_bridge)
gsp_add_backend(hello_backend
    SOURCES ../main/hello_ui.c platform_pc.c
    INCLUDE_DIRECTORIES ../main
    SCENES ../scenes/hello_800.json)
