# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "components")
set(SUPPORTED_TARGETS esp32p4 esp32s31)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gsp_showcase)

option(GSP_SHOWCASE_RGB888
       "Compile the showcase bundle and panel as native RGB888" OFF)

if(GSP_SHOWCASE_RGB888)
    if(NOT CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI AND
       NOT CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
        message(FATAL_ERROR
            "GSP_SHOWCASE_RGB888 needs MIPI-DPI or 24-bit RGB")
    endif()
    idf_component_get_property(showcase_main main COMPONENT_LIB)
    target_compile_definitions(${showcase_main} PRIVATE
        GSP_SHOWCASE_RGB888=1)
endif()
