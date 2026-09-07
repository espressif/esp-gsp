# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "components")
set(SUPPORTED_TARGETS esp32c3 esp32s3 esp32p4 esp32s31)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gsp_benchmark)

option(GSP_BENCH_FULL_REPAINT
       "Continuously repaint every scene; OFF measures native dirty updates" ON)
idf_component_get_property(benchmark_main main COMPONENT_LIB)
target_compile_definitions(${benchmark_main} PRIVATE
    GSP_BENCH_FULL_REPAINT=$<BOOL:${GSP_BENCH_FULL_REPAINT}>)
option(GSP_BENCH_TOUCH_INPUT
       "Enable physical touch for diagnostics (touch invalidates measurement)" OFF)
target_compile_definitions(${benchmark_main} PRIVATE
    GSP_BENCH_TOUCH_INPUT=$<BOOL:${GSP_BENCH_TOUCH_INPUT}>)

set(GSP_BENCH_SOAK_CASE "" CACHE STRING
    "Run one stable benchmark case ID continuously (for example P_STORM)")
set(GSP_BENCH_SOAK_DWELL_MS "60000" CACHE STRING
    "Measured duration of each soak cycle in milliseconds")
if(GSP_BENCH_SOAK_CASE)
    if(NOT GSP_BENCH_SOAK_DWELL_MS MATCHES "^[1-9][0-9]*$" OR
       GSP_BENCH_SOAK_DWELL_MS GREATER 86400000)
        message(FATAL_ERROR
            "GSP_BENCH_SOAK_DWELL_MS must be 1..86400000 milliseconds")
    endif()
    target_compile_definitions(${benchmark_main} PRIVATE
        GSP_BENCH_SOAK_CASE="${GSP_BENCH_SOAK_CASE}"
        GSP_BENCH_SOAK_DWELL_MS=${GSP_BENCH_SOAK_DWELL_MS}U)
endif()

# One timer interval per service loop plus exact drained-command counts.
# This is the minimum instrumentation required for auditable svc/cmd columns
# and is distinct from the much heavier per-opcode profiler below.
get_property(gsp_framework GLOBAL PROPERTY ESP_GSP_COMPONENT_LIB)
target_compile_definitions(${gsp_framework} PRIVATE GSP_PROFILE_SERVICE=1)

# Per-opcode profiling adds two timer reads around every command and therefore
# is deliberately excluded from the official throughput score. Enable it only
# for diagnostic runs; the mode is fingerprinted in the benchmark log.
option(GSP_BENCH_DIAGNOSTICS
       "Enable intrusive per-opcode render profiling" OFF)
if(GSP_BENCH_DIAGNOSTICS)
    target_compile_definitions(${gsp_framework} PRIVATE GSP_PROFILE_RENDER=1)
    idf_component_get_property(benchmark_main main COMPONENT_LIB)
    target_compile_definitions(${benchmark_main} PRIVATE
        GSP_BENCH_DIAGNOSTICS=1)
endif()

option(GSP_BENCH_RGB888
       "Run a native RGB888 benchmark bundle and panel" OFF)
if(GSP_BENCH_RGB888)
    if(NOT CONFIG_EXAMPLE_LCD_INTERFACE_MIPI_DSI AND
       NOT CONFIG_EXAMPLE_LCD_INTERFACE_RGB24)
        message(FATAL_ERROR "GSP_BENCH_RGB888 needs MIPI-DPI or 24-bit RGB")
    endif()
    idf_component_get_property(benchmark_main main COMPONENT_LIB)
    target_compile_definitions(${benchmark_main} PRIVATE GSP_BENCH_RGB888=1)
endif()

option(GSP_BENCH_DEPLOYABLE
       "Load self-describing bundle metadata instead of generated directories" OFF)
if(GSP_BENCH_DEPLOYABLE)
    idf_component_get_property(benchmark_main main COMPONENT_LIB)
    target_compile_definitions(${benchmark_main} PRIVATE
        GSP_BENCH_DEPLOYABLE=1)
endif()

option(GSP_BENCH_DISABLE_TRANSITION_SNAPSHOTS
       "Measure zero-snapshot surface transitions" OFF)
if(GSP_BENCH_DISABLE_TRANSITION_SNAPSHOTS)
    idf_component_get_property(benchmark_main main COMPONENT_LIB)
    target_compile_definitions(${benchmark_main} PRIVATE
        GSP_BENCH_DISABLE_TRANSITION_SNAPSHOTS=1)
endif()

# Benchmark-only A/B switch.  This is deliberately a CMake option, not
# a product Kconfig: shipped firmware should always select the fastest
# supported decoder automatically.
option(GSP_BENCH_FORCE_JPEG_SOFTWARE
       "Disable hardware JPEG for same-board benchmark A/B" OFF)
if(GSP_BENCH_FORCE_JPEG_SOFTWARE)
    target_compile_definitions(${gsp_framework} PRIVATE
        GSP_CODEC_JPEG_FORCE_SOFTWARE=1)
endif()
