# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

idf_build_get_property(bench_board_profile GSP_BENCH_BOARD_PROFILE)
idf_build_get_property(bench_board_idf_target IDF_TARGET)

if(bench_board_profile STREQUAL "C5_CO5300")
    set(board_target esp32c5)
    set(board_source hw_lcd_c5_co5300.c)
    set(board_private_requires esp_lcd_co5300 esp_psram)
elseif(bench_board_profile STREQUAL "P4_EYE_SPI")
    set(board_target esp32p4)
    set(board_source hw_lcd_p4_eye_spi.c)
    set(board_private_requires esp_driver_ledc)
elseif(bench_board_profile STREQUAL "S31_MOSAICO")
    set(board_target esp32s31)
    set(board_source hw_lcd_s31_mosaico.c)
    set(board_private_requires esp_lcd_co5300 efuse)
elseif(bench_board_profile STREQUAL "S31_GC9503")
    set(board_target esp32s31)
    set(board_source hw_lcd_s31_gc9503.c)
    set(board_private_requires esp_lcd_gc9503 esp_lcd_panel_io_additions)
else()
    message(FATAL_ERROR "benchmark-local hw_init requires GSP_BENCH_BOARD_PROFILE")
endif()
if(NOT bench_board_idf_target STREQUAL board_target)
    message(FATAL_ERROR "${bench_board_profile} requires target ${board_target}")
endif()

idf_component_register(
    SRCS ${board_source} hw_touch.c
    INCLUDE_DIRS include
    REQUIRES esp_display_present esp_lcd_touch
    PRIV_REQUIRES esp_lcd esp_driver_gpio esp_driver_spi ${board_private_requires}
)
target_compile_definitions(${COMPONENT_LIB} PUBLIC
    GSP_BENCH_BOARD_${bench_board_profile}=1)
