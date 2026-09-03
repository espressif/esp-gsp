# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

# Makes gsp_add_bundle() available to every component in projects
# that depend on esp-gsp.
include("${CMAKE_CURRENT_LIST_DIR}/cmake/gsp_bundle.cmake")
