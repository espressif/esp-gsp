# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "components")
set(SUPPORTED_TARGETS esp32p4)
set(ESP_GSP_EXAMPLE_ASSETS_SUBTREE "scenarios/industrial_panel")
set(ESP_GSP_EXAMPLE_ASSETS_SENTINEL "scenes/artwork/cell-studio.png")
set(ESP_GSP_EXAMPLE_ASSETS_FONTS "Ubuntu-L.ttf;Ubuntu-R.ttf")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gsp_industrial)
