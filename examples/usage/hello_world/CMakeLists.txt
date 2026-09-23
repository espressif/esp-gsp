# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "components")
set(SUPPORTED_TARGETS esp32c3 esp32s3 esp32p4 esp32s31)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gsp_hello_world)

option(GSP_HELLO_RGB888
       "Compile the minimal scene and display path as native RGB888" OFF)
