# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

# gsp_add_bundle(<component_lib>
#                [SCENES <scene0.json> [scene1.json ...]]
#                [PIXEL_FORMAT rgb565|rgb888]
#                [IMAGE_CACHE_BYTES <bytes>]
#                [DYNAMIC_FONT <font.ttf>]
#                [PROFILE <expert-override.yaml>]
#                [DEPLOYABLE]
#                [SYMBOL <name>])
#
# With no SCENES argument, JSON files are discovered in the project-level
# scenes/ directory. Explicit SCENES paths are the advanced mode for projects
# that keep JSON in multiple locations.
#
# Compiles the scene JSONs with gspc at BUILD time (scene ids by
# position), packs one .gspb, embeds it as <SYMBOL>_start/_end
# (default: bundle_start/bundle_end, 64-byte aligned rodata) and adds
# one <SYMBOL>_gsp.h application header plus the per-scene advanced
# headers (binds/actions/objects/templates) to the component's include
# path. Scene edits rebuild automatically; no committed artifacts, no
# manual regenerate step.
# DEPLOYABLE additionally stores self-describing runtime metadata in
# the GSPB. The generated-header path remains available and is the default.
#
# Advanced applications that provide TTF/OTF data at runtime can call
# gsp_enable_freetype() directly instead of using DYNAMIC_FONT.
function(gsp_enable_freetype)
    get_filename_component(gsp_root
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    get_property(gsp_lib GLOBAL PROPERTY ESP_GSP_COMPONENT_LIB)
    if(NOT gsp_lib OR NOT TARGET ${gsp_lib})
        message(FATAL_ERROR
            "gsp_enable_freetype: esp-gsp component target is unavailable")
    endif()
    idf_build_get_property(build_components BUILD_COMPONENTS)
    get_target_property(gsp_freetype_enabled
        ${gsp_lib} ESP_GSP_FREETYPE_ENABLED)
    if(gsp_freetype_enabled)
        return()
    endif()

    set(freetype_component "")
    foreach(candidate IN ITEMS espressif__freetype freetype)
        if(candidate IN_LIST build_components)
            set(freetype_component "${candidate}")
            break()
        endif()
    endforeach()
    if(NOT freetype_component)
        message(FATAL_ERROR
            "gsp_enable_freetype: managed component espressif/freetype "
            "is unavailable; check esp-gsp's idf_component.yml dependencies")
    endif()
    idf_component_get_property(freetype_lib
        ${freetype_component} COMPONENT_LIB)
    get_target_property(gsp_is_prebuilt ${gsp_lib} ESP_GSP_PREBUILT)
    if(gsp_is_prebuilt)
        # Release archives are compiled with FreeType support already. Keep the
        # dependency explicit but never try to add excluded implementation
        # source to a source-free managed component.
        target_link_libraries(${gsp_lib} PRIVATE ${freetype_lib})
        set_property(TARGET ${gsp_lib}
            PROPERTY ESP_GSP_FREETYPE_ENABLED TRUE)
        message(STATUS "esp-gsp: FreeType enabled in prebuilt library")
        return()
    endif()
    target_sources(${gsp_lib} PRIVATE
        "${gsp_root}/src/runtime/gsp_freetype.c")
    target_compile_definitions(${gsp_lib} PRIVATE GSP_ENABLE_FREETYPE=1)
    target_link_libraries(${gsp_lib} PRIVATE ${freetype_lib})
    set_property(TARGET ${gsp_lib}
        PROPERTY ESP_GSP_FREETYPE_ENABLED TRUE)
    message(STATUS "esp-gsp: FreeType enabled by application")
endfunction()

function(gsp_add_bundle target)
    cmake_parse_arguments(
        ARG "DEPLOYABLE"
        "PROFILE;SYMBOL;PIXEL_FORMAT;IMAGE_CACHE_BYTES;DYNAMIC_FONT"
        "SCENES" ${ARGN})
    if(NOT ARG_SCENES)
        set(gsp_default_scene_dir "${PROJECT_DIR}/scenes")
        file(GLOB ARG_SCENES CONFIGURE_DEPENDS
            "${gsp_default_scene_dir}/*.json")
        list(SORT ARG_SCENES)
        if(NOT ARG_SCENES)
            message(FATAL_ERROR
                "gsp_add_bundle: no JSON scenes found in "
                "${gsp_default_scene_dir}; create project/scenes/*.json or "
                "pass SCENES with explicit paths")
        endif()
        message(STATUS
            "gsp_add_bundle: using project scenes directory "
            "${gsp_default_scene_dir}")
    endif()
    if(NOT ARG_SYMBOL)
        set(ARG_SYMBOL bundle)
    endif()
    if(NOT ARG_SYMBOL MATCHES "^[A-Za-z_][A-Za-z0-9_]*$")
        message(FATAL_ERROR
            "gsp_add_bundle: SYMBOL must be a C identifier: ${ARG_SYMBOL}")
    endif()

    get_filename_component(gsp_root
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
    include("${gsp_root}/cmake/gsp_build_caps.cmake")
    include("${gsp_root}/cmake/gsp_compatibility.cmake")
    include("${gsp_root}/cmake/gsp_toolchain.cmake")
    get_property(gspc_command GLOBAL PROPERTY ESP_GSPC_COMMAND)
    if(NOT gspc_command)
        esp_gsp_resolve_gspc("${gsp_root}" gspc_command gspc_dependencies
            gspc_source)
        esp_gsp_check_gspc_command(gspc_version ${gspc_command})
        set_property(GLOBAL PROPERTY ESP_GSPC_COMMAND
            "${gspc_command}")
        set_property(GLOBAL PROPERTY ESP_GSPC_DEPENDENCIES
            "${gspc_dependencies}")
        message(STATUS "esp-gsp: using ${gspc_version} from ${gspc_source} "
            "(${gspc_command})")
    else()
        get_property(gspc_dependencies GLOBAL PROPERTY ESP_GSPC_DEPENDENCIES)
    endif()
    set(gen_dir "${CMAKE_CURRENT_BINARY_DIR}/gsp_gen_${ARG_SYMBOL}")
    set(bundle_path "${gen_dir}/${ARG_SYMBOL}.gspb")
    set(bundle_depfile "${gen_dir}/${ARG_SYMBOL}.d")
    set(api_header "${gen_dir}/${ARG_SYMBOL}_gsp.h")
    set(dynamic_font_api_args "")
    set(dynamic_font_deps "")
    set(dynamic_font_embed_s "")
    set(bundle_config_args "")
    set(gspc_source_deps "")
    if(gspc_command STREQUAL "${gsp_root}/ci/gspc-dev")
        file(GLOB_RECURSE gspc_source_deps CONFIGURE_DEPENDS
            "${gsp_root}/tools/gspc/*.rs"
            "${gsp_root}/tools/gspc/Cargo.toml"
            "${gsp_root}/tools/gspc/Cargo.lock"
            "${gsp_root}/tools/gspc/*/Cargo.toml"
            "${gsp_root}/tools/gspc/*/build.rs")
        list(APPEND gspc_source_deps
            "${gsp_root}/formats/gsp_format.yaml")
    endif()
    # AUTO is a runtime/project value, not a compiler ceiling. gspc must be
    # allowed to derive the exact requirement up to the prebuilt capability.
    set(gsp_list_max_slots "${CONFIG_ESP_GSP_LIST_MAX_SLOTS}")
    if(NOT gsp_list_max_slots)
        set(gsp_list_max_slots 64)
    endif()
    set(gsp_list_text_slots "${CONFIG_ESP_GSP_LIST_TEXT_SLOTS}")
    if(NOT gsp_list_text_slots)
        set(gsp_list_text_slots 16)
    endif()
    if(NOT CONFIG_ESP_GSP_INSTANCE_STATES_PER_SLOT)
        set(CONFIG_ESP_GSP_INSTANCE_STATES_PER_SLOT 8)
    endif()
    foreach(gsp_project_var IN ITEMS
            CONFIG_ESP_GSP_MAX_SCENES CONFIG_ESP_GSP_MAX_LISTS
            CONFIG_ESP_GSP_LIST_MAX_SLOTS CONFIG_ESP_GSP_LIST_TEXT_SLOTS
            CONFIG_ESP_GSP_TEXT_SLOTS
            CONFIG_ESP_GSP_DEFAULT_DYNAMIC_IMAGE_SLOTS
            CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES
            CONFIG_ESP_GSP_CONTEXT_DEFAULT_GLYPH_RUNS
            CONFIG_ESP_GSP_STACK_VIEW_MAX_DEPTH
            CONFIG_ESP_GSP_INSTANCE_STATES_PER_SLOT)
        if(NOT DEFINED ${gsp_project_var} OR "${${gsp_project_var}}" STREQUAL "")
            set(${gsp_project_var} 0)
        endif()
    endforeach()
    set(slot_limit_args
        --list-max-slots "${gsp_list_max_slots}"
        --list-text-slots "${gsp_list_text_slots}"
        --instance-states-per-slot
            "${CONFIG_ESP_GSP_INSTANCE_STATES_PER_SLOT}"
        --max-dynamic-image-targets
            "${ESP_GSP_BUILD_CAP_MAX_DYNAMIC_IMAGE_TARGETS}"
        --max-font-packs "${ESP_GSP_BUILD_CAP_MAX_FONTS_PER_SCENE}"
        --project-max-scenes "${CONFIG_ESP_GSP_MAX_SCENES}"
        --project-max-lists "${CONFIG_ESP_GSP_MAX_LISTS}"
        --project-list-max-slots "${CONFIG_ESP_GSP_LIST_MAX_SLOTS}"
        --project-list-text-slots "${CONFIG_ESP_GSP_LIST_TEXT_SLOTS}"
        --project-text-slots "${CONFIG_ESP_GSP_TEXT_SLOTS}"
        --project-dynamic-image-slots
            "${CONFIG_ESP_GSP_DEFAULT_DYNAMIC_IMAGE_SLOTS}"
        --project-component-instances
            "${CONFIG_ESP_GSP_CONTEXT_DEFAULT_INSTANCES}"
        --project-stack-view-max-depth
            "${CONFIG_ESP_GSP_STACK_VIEW_MAX_DEPTH}"
        --project-instance-states-per-slot
            "${CONFIG_ESP_GSP_INSTANCE_STATES_PER_SLOT}"
        --project-glyph-run-slots
            "${CONFIG_ESP_GSP_CONTEXT_DEFAULT_GLYPH_RUNS}")
    if(NOT CONFIG_SPIRAM)
        list(APPEND bundle_config_args --default-disable-image-cache)
    endif()
    if(ARG_DYNAMIC_FONT)
        get_filename_component(dynamic_font_abs "${ARG_DYNAMIC_FONT}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if(NOT EXISTS "${dynamic_font_abs}")
            message(FATAL_ERROR
                "gsp_add_bundle: dynamic font does not exist: "
                "${dynamic_font_abs}")
        endif()
        gsp_enable_freetype()
        set(dynamic_font_symbol "${ARG_SYMBOL}_dynamic_font")
        set(dynamic_font_api_args
            --dynamic-font-symbol "${dynamic_font_symbol}")
        set(dynamic_font_deps "${dynamic_font_abs}")
    endif()

    set(scene_paths "")
    set(scene_api_byproducts "")
    set(scene_index 0)
    foreach(scene IN LISTS ARG_SCENES)
        get_filename_component(abs "${scene}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if(NOT EXISTS "${abs}")
            message(FATAL_ERROR
                "gsp_add_bundle: scene JSON does not exist: ${abs}")
        endif()
        list(APPEND scene_paths "${abs}")
        # The screen name determines generated byproduct filenames, so a JSON
        # edit must refresh the CMake graph before the pack command runs.
        set_property(DIRECTORY APPEND PROPERTY
            CMAKE_CONFIGURE_DEPENDS "${abs}")
        file(READ "${abs}" scene_json)
        string(JSON scene_screen ERROR_VARIABLE scene_json_error
            GET "${scene_json}" screen)
        if(scene_json_error)
            message(FATAL_ERROR
                "gsp_add_bundle: cannot read screen from ${abs}: "
                "${scene_json_error}")
        endif()
        string(REGEX REPLACE "[^A-Za-z0-9_]+" "_"
            scene_stem "${scene_screen}")
        string(REGEX REPLACE "^_+" "" scene_stem "${scene_stem}")
        string(REGEX REPLACE "_+$" "" scene_stem "${scene_stem}")
        string(TOLOWER "${scene_stem}" scene_stem)
        if(NOT scene_stem)
            set(scene_stem scene)
        endif()
        set(scene_dir "${gen_dir}/scene${scene_index}")
        list(APPEND scene_api_byproducts
            "${scene_dir}/${scene_stem}.gsb"
            "${scene_dir}/${scene_stem}_binds.h"
            "${scene_dir}/${scene_stem}_actions.h"
            "${scene_dir}/${scene_stem}_objects.h"
            "${scene_dir}/${scene_stem}_templates.h"
            "${scene_dir}/${scene_stem}.execution.json"
            "${scene_dir}/${scene_stem}.manifest.md")
        if(ARG_DEPLOYABLE)
            list(APPEND scene_api_byproducts
                "${scene_dir}/${scene_stem}.gmd")
        endif()
        math(EXPR scene_index "${scene_index} + 1")
    endforeach()

    set(deployable_args "")
    if(ARG_DEPLOYABLE)
        list(APPEND deployable_args --deployable)
    endif()

    set(profile_args "")
    set(profile_deps "")
    if(ARG_PROFILE)
        get_filename_component(profile_abs "${ARG_PROFILE}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        set(profile_args --profile "${profile_abs}")
        set(profile_deps "${profile_abs}")
    else()
        if(NOT ARG_PIXEL_FORMAT)
            set(ARG_PIXEL_FORMAT rgb565)
        endif()
        if(NOT ARG_PIXEL_FORMAT STREQUAL "rgb565" AND
                NOT ARG_PIXEL_FORMAT STREQUAL "rgb888")
            message(FATAL_ERROR
                "gsp_add_bundle: PIXEL_FORMAT must be rgb565 or rgb888")
        endif()
        set(profile_args
            --platform-soc "${IDF_TARGET}"
            --pixel-format "${ARG_PIXEL_FORMAT}")
        if(CONFIG_SPIRAM)
            list(APPEND profile_args --psram)
        endif()
        # JPEG/JPEG_A8 requires a decoded-image cache. On no-PSRAM targets
        # the default cache is deliberately disabled, so advertising the
        # hardware codec would let `codec: auto` emit assets the runtime
        # cannot display. An explicit internal-RAM cache opts back in.
        if(CONFIG_SOC_JPEG_CODEC_SUPPORTED AND
                (CONFIG_SPIRAM OR ARG_IMAGE_CACHE_BYTES))
            list(APPEND profile_args --hardware-jpeg)
        endif()
        if(CONFIG_SOC_PPA_SUPPORTED)
            list(APPEND profile_args --ppa)
        endif()
        if(CONFIG_SOC_DMA2D_SUPPORTED)
            list(APPEND profile_args --dma2d)
        endif()
        if(ARG_IMAGE_CACHE_BYTES)
            list(APPEND profile_args
                --image-cache-bytes "${ARG_IMAGE_CACHE_BYTES}")
        endif()
    endif()

    add_custom_command(
        OUTPUT "${bundle_path}" "${api_header}"
        BYPRODUCTS ${scene_api_byproducts}
        COMMAND ${gspc_command} pack ${scene_paths}
                ${profile_args} -o "${bundle_path}"
                --gen-dir "${gen_dir}" --depfile "${bundle_depfile}"
                --api-header "${api_header}" --symbol "${ARG_SYMBOL}"
                ${deployable_args}
                ${dynamic_font_api_args} ${bundle_config_args}
                ${slot_limit_args}
        DEPENDS ${scene_paths} ${profile_deps} ${dynamic_font_deps}
                ${gspc_dependencies} ${gspc_source_deps}
        DEPFILE "${bundle_depfile}"
        COMMENT "gspc: packing ${ARG_SYMBOL}.gspb"
        VERBATIM)
    list(LENGTH scene_paths scene_count)
    message(STATUS
        "gsp_add_bundle: ${scene_count} JSON scene(s) -> ${bundle_path}")
    message(STATUS
        "gsp_add_bundle: application header -> ${api_header}")

    set(embed_s "${gen_dir}/${ARG_SYMBOL}_embed.S")
    set(GSP_BUNDLE_SYMBOL "${ARG_SYMBOL}")
    set(GSP_BUNDLE_PATH "${bundle_path}")
    configure_file("${gsp_root}/cmake/gsp_embed.S.in"
                   "${embed_s}" @ONLY)
    set_property(SOURCE "${embed_s}" APPEND PROPERTY
        OBJECT_DEPENDS "${bundle_path}")
    if(ARG_DYNAMIC_FONT)
        set(dynamic_font_embed_s
            "${gen_dir}/${ARG_SYMBOL}_dynamic_font_embed.S")
        set(GSP_BUNDLE_SYMBOL "${dynamic_font_symbol}")
        set(GSP_BUNDLE_PATH "${dynamic_font_abs}")
        configure_file("${gsp_root}/cmake/gsp_embed.S.in"
                       "${dynamic_font_embed_s}" @ONLY)
        set_property(SOURCE "${dynamic_font_embed_s}" APPEND PROPERTY
            OBJECT_DEPENDS "${dynamic_font_abs}")
    endif()

    # Sources including the generated headers must build after the
    # pack step (parallel-build ordering).
    add_custom_target(${target}_gsp_${ARG_SYMBOL}
        DEPENDS "${bundle_path}" "${api_header}")
    add_dependencies(${target} ${target}_gsp_${ARG_SYMBOL})

    target_sources(${target} PRIVATE "${embed_s}" ${dynamic_font_embed_s})
    # Generated headers land in the per-scene gen dirs.
    set(include_dirs "${gen_dir}")
    math(EXPR last "${scene_count} - 1")
    foreach(index RANGE ${last})
        list(APPEND include_dirs "${gen_dir}/scene${index}")
    endforeach()
    target_include_directories(${target} PRIVATE ${include_dirs})
endfunction()
