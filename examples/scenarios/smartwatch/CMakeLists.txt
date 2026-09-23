# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "components")
set(ESP_GSP_EXAMPLE_ASSETS_SUBTREE "scenarios/smartwatch")
set(ESP_GSP_EXAMPLE_ASSETS_SENTINEL "scenes/assets/ridge.png")
set(SUPPORTED_TARGETS esp32s3)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gsp_watch)
