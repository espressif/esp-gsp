# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "components")
set(ESP_GSP_EXAMPLE_ASSETS_SUBTREE "scenarios/industrial_panel")
set(ESP_GSP_EXAMPLE_ASSETS_SENTINEL "scenes/assets/production_cell.png")
set(SUPPORTED_TARGETS esp32p4)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gsp_industrial)
