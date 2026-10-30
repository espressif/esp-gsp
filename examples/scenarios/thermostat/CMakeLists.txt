# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "components")
set(ESP_GSP_EXAMPLE_ASSETS_SUBTREE "scenarios/thermostat")
set(ESP_GSP_EXAMPLE_ASSETS_SENTINEL "scenes/assets/climate.svg")
set(ESP_GSP_EXAMPLE_ASSETS_FONTS "Ubuntu-L.ttf;Ubuntu-R.ttf")
set(SUPPORTED_TARGETS esp32c3)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gsp_thermostat)
