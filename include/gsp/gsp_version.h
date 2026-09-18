/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */

#pragma once

#define GSP_VERSION_MAJOR 1
#define GSP_VERSION_MINOR 4
#define GSP_VERSION_PATCH 0
#define GSP_VERSION_STRING "1.4.0"

#define GSP_VERSION_VAL(major, minor, patch) (((major) << 16) | ((minor) << 8) | (patch))
#define GSP_VERSION GSP_VERSION_VAL(GSP_VERSION_MAJOR, GSP_VERSION_MINOR, GSP_VERSION_PATCH)
