/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

/* Private wire v2: five header words (version, count, completion stage,
 * expected scene or UINT32_MAX, timeout_ms), then eight words per command:
 * kind, bind, component, property, value type, value/payload offset,
 * payload length, image cache key. Payload offsets refer to this packet.
 * All words are little endian. No native pointers or ABI structs cross it.
 * Bit 8 of value type requests RGB888-to-native color conversion. */
#define GSP_UPDATE_WIRE_VERSION 2U
#define GSP_UPDATE_WIRE_HEADER 20U
#define GSP_UPDATE_WIRE_COMMAND 32U
#define GSP_UPDATE_WIRE_LIMIT (480U * 1024U)
#define GSP_UPDATE_WIRE_STATUS_WORDS 16U
#define GSP_UPDATE_WIRE_RETAINED 64U

enum {
    GSP_UPDATE_WIRE_QUERY = 0,
    GSP_UPDATE_WIRE_CANCEL,
    GSP_UPDATE_WIRE_RELEASE,
    GSP_UPDATE_WIRE_CAPABILITIES,
};

static inline uint32_t gsp_update_wire_read(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | (uint32_t)bytes[1] << 8 |
           (uint32_t)bytes[2] << 16 | (uint32_t)bytes[3] << 24;
}

static inline void gsp_update_wire_write(uint8_t *bytes, uint32_t value)
{
    for (unsigned index = 0; index < 4; ++index) {
        bytes[index] = (uint8_t)(value >> (8U * index));
    }
}
