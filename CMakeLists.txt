# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

set(gsp_srcs
    "src/config/gsp_build_caps.c"
    "src/config/gsp_config_desc.c"
    "src/config/gsp_config.c"
    "src/codec/gsp_codec.c"
    "src/codec/gsp_codec_qoi.c"
    "src/codec/gsp_codec_rle16.c"
    "src/codec/gsp_codec_rle16_a8.c"
    "src/codec/gsp_codec_rle32.c"
    "src/codec/gsp_codec_png.c"
    "src/runtime/gsp_container.c"
    "src/runtime/gsp_decode_task.c"
    "src/runtime/gsp_gfb.c"
    "src/runtime/gsp_grb.c"
    "src/runtime/gsp_bundle.c"
    "src/runtime/gsp_gsb.c"
    "src/runtime/gsp_image_cache.c"
    "src/runtime/gsp_input.c"
    "src/runtime/gsp_pointer.c"
    "src/runtime/gsp_multitouch.c"
    "src/runtime/gsp_image_fit.c"
    "src/runtime/gsp_swipe_kinetics.c"
    "src/runtime/gsp_instance.c"
    "src/runtime/gsp_scroll_phys.c"
    "src/runtime/gsp_text_edit.c"
    "src/runtime/gsp_value_drag.c"
    "src/runtime/gsp_overlay.c"
    "src/font/gsp_text.c"
    "src/runtime/gsp_repeater.c"
    "src/runtime/gsp_update.c"
    "src/runtime/gsp_context.c"
    "src/runtime/gsp_font_catalog.c"
    "src/runtime/gsp_anim.c"
    "src/codec/eaf/gfx_eaf_dec.c"
    "src/widget/gsp_widget.c"
    "src/present/gsp_dirty.c"
    "src/present/gsp_region_cost.c"
    "src/present/gsp_scroll_blit_gate.c"
    "src/present/gsp_frame_loop.c"
    "src/present/gsp_present_compose.c"
    "src/present/gsp_transition_present.c"
    "src/backend/esp_lcd/gsp_esp_lcd_canvas.c"
    "src/backend/esp_lcd/gsp_esp_lcd_display.c"
    "src/effect/gsp_transition.c"
    "src/effect/gsp_transition_policy.c"
    "src/effect/gsp_snapshot_motion.c"
    "src/effect/gsp_viewport_snap.c"
    "src/platform/esp_idf/gsp_esp_accel.c"
    "src/platform/esp_idf/accel/gsp_esp_accel_ppa.c"
    "src/platform/esp_idf/accel/gsp_esp_accel_dma2d.c"
    "src/platform/esp_idf/gsp_esp_runtime.c"
    "src/platform/esp_idf/esp_gsp_esp_lcd.c"
    "src/ui/esp_gsp_api.c"
    "src/ui/esp_gsp_deployable.c"
    "src/ui/gsp_app.c"
    "src/ui/esp_gsp_ui.c"
    "src/ui/esp_gsp_list.c"
    "src/ui/esp_gsp_widget.c"
    "src/ui/esp_gsp_anim.c"
    "src/ui/esp_gsp_text.c"
    "src/ui/esp_gsp_canvas.c"
    "src/ui/esp_gsp_overlay.c"
    "src/component/gsp_component.c"
    "src/component/gsp_component_compat.c"
    "src/component/gsp_component_manager.c"
    "src/component/gsp_viewport_track.c"
    "src/component/gsp_component_route.c"
    "src/component/driver/gsp_boolean.c"
    "src/component/driver/gsp_range.c"
    "src/component/driver/gsp_list.c"
    "src/component/driver/gsp_page_flow.c"
    "src/component/driver/gsp_stack_view.c"
    "src/component/driver/gsp_drawer.c"
    "src/ui/esp_gsp_input.c"
    "src/effect/gsp_transition_compose.c"
    "src/accel/gsp_accel.c"
    "src/renderer/gsp_renderer.c"
    "src/renderer/gsp_render_effect.c"
    "src/runtime/gsp_transform.c"
    "src/renderer/kernel_rgb565/gsp_kernel_rgb565.c"
    "src/renderer/kernel_rgb888/gsp_kernel_rgb888.c"
    "src/renderer/kernel_argb8888/gsp_kernel_argb8888.c"
    "src/renderer/reference/gsp_reference.c")
set(gsp_priv_include_dirs
    "priv_include"
    "src/ui"
    "src/accel"
    "src/renderer"
    "src/codec"
    "src/codec/eaf"
    "src/runtime"
    "src/port"
    "src/effect"
    "src/backend/esp_lcd"
    "src/platform/esp_idf")

# esp_driver_ppa registers as an empty component on SoCs without a PPA,
# so the dependency is unconditional; the backend source is gated by
# the SoC capabilities internally.
set(gsp_priv_requires log esp_driver_ppa esp_driver_dma esp_mm esp_timer esp_lcd esp_partition)
list(APPEND gsp_srcs "src/codec/gsp_codec_jpeg.c")
if(CONFIG_SOC_CPU_HAS_PIE)
    list(APPEND gsp_srcs
        "src/renderer/kernel_rgb565/gsp_kernel_rgb565_pie.S"
        "src/renderer/kernel_rgb888/gsp_kernel_rgb888_pie.S")
endif()
if(CONFIG_SOC_SIMD_INSTRUCTION_SUPPORTED AND CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND gsp_srcs
        "src/renderer/kernel_rgb565/gsp_kernel_rgb565_ee.S")
endif()

set(GSP_PREBUILT_LIB
    "${CMAKE_CURRENT_LIST_DIR}/prebuilt/${IDF_TARGET}/libesp-gsp.a")
if(ESP_GSP_BUILD_PREBUILT)
    # Release archives must be self-contained: consumption-side packages do
    # not ship gsp_freetype.c and therefore cannot add the provider later.
    # This branch deliberately takes precedence over an existing archive so a
    # release build can never accidentally recycle stale CI artifacts.
    list(APPEND gsp_srcs "src/runtime/gsp_freetype.c")
    list(APPEND gsp_priv_requires espressif__freetype)
elseif(EXISTS "${GSP_PREBUILT_LIB}")
    # Keep COMPONENT_LIB as a real STATIC target. Registering with no sources
    # makes ESP-IDF create an INTERFACE target, which application helpers cannot
    # safely configure. Generate the anchor in the build tree so the published
    # component remains source-free.
    set(gsp_prebuilt_anchor
        "${CMAKE_CURRENT_BINARY_DIR}/esp_gsp_prebuilt_anchor.c")
    file(WRITE "${gsp_prebuilt_anchor}"
        "/* Generated prebuilt component anchor. */\n"
        "void esp_gsp_prebuilt_component_anchor(void) {}\n")
    set(gsp_srcs "${gsp_prebuilt_anchor}")
    set(gsp_priv_include_dirs "")
    set(gsp_use_prebuilt ON)
    list(APPEND gsp_priv_requires espressif__esp_new_jpeg esp_driver_jpeg)
endif()
if(NOT ESP_GSP_BUILD_PREBUILT AND NOT gsp_use_prebuilt)
    list(APPEND gsp_srcs "adapters/esp_idf/esp_gsp_idf_touch.c")
endif()
idf_component_register(
    SRCS ${gsp_srcs}
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS ${gsp_priv_include_dirs}
    REQUIRES esp_display_present
    PRIV_REQUIRES ${gsp_priv_requires}
)

if(ESP_GSP_BUILD_PREBUILT OR gsp_use_prebuilt)
    # Keep the public touch adapter outside the release core archive while
    # linking it directly into the final firmware. An INTERFACE source avoids
    # static archive ordering problems without forcing the entire core archive
    # into the executable.
    target_sources(${COMPONENT_LIB} INTERFACE
        "${CMAKE_CURRENT_LIST_DIR}/adapters/esp_idf/esp_gsp_idf_touch.c")
endif()

# The project configuration is intentionally not part of libesp-gsp.a.  It is
# compiled as an object target in the consuming build, so sdkconfig belongs to
# the final firmware rather than to the release archive.
set(gsp_project_config_source
    "${CMAKE_CURRENT_BINARY_DIR}/esp_gsp_project_config.c")
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/esp_gsp_project_config.c.in"
    "${gsp_project_config_source}" COPYONLY)
add_library(esp_gsp_project_config OBJECT "${gsp_project_config_source}")
target_include_directories(esp_gsp_project_config PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_BINARY_DIR}/config")
target_sources(${COMPONENT_LIB} INTERFACE
    $<TARGET_OBJECTS:esp_gsp_project_config>)
# Export the actual target name for project helpers. Managed components are
# namespace-qualified by the Registry, so helpers must not reconstruct it.
set_property(GLOBAL PROPERTY ESP_GSP_COMPONENT_LIB "${COMPONENT_LIB}")
set_property(GLOBAL PROPERTY ESP_GSP_COMPONENT_DIR "${CMAKE_CURRENT_LIST_DIR}")

if(gsp_use_prebuilt)
    add_prebuilt_library(gsp_prebuilt "${GSP_PREBUILT_LIB}"
        REQUIRES ${gsp_priv_requires}
                 espressif__esp_new_jpeg
                 esp_driver_jpeg espressif__freetype)
    # idf_component_register() has already resolved public requirements to
    # their real component targets (including Registry namespace prefixes).
    # Reuse those links instead of looking up an unqualified component name
    # again in add_prebuilt_library().
    get_target_property(gsp_public_links ${COMPONENT_LIB}
        INTERFACE_LINK_LIBRARIES)
    if(gsp_public_links)
        target_link_libraries(gsp_prebuilt INTERFACE ${gsp_public_links})
    endif()
    target_link_libraries(${COMPONENT_LIB} INTERFACE gsp_prebuilt)
    set_property(TARGET ${COMPONENT_LIB}
        PROPERTY ESP_GSP_PREBUILT TRUE)
else()
    idf_component_optional_requires(PRIVATE espressif__esp_new_jpeg esp_driver_jpeg)
    if(ESP_GSP_BUILD_PREBUILT)
        target_compile_definitions(${COMPONENT_LIB}
            PRIVATE GSP_ENABLE_FREETYPE=1)
        set_property(TARGET ${COMPONENT_LIB}
            PROPERTY ESP_GSP_FREETYPE_ENABLED TRUE)
    endif()
endif()
