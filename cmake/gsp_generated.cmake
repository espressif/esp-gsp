# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
include_guard(GLOBAL)

# Component definitions are optional and shared by device and native builds.
# GSPC's depfile tracks each definition; the directory also tracks additions
# and removals without making every content edit reconfigure the project.
function(esp_gsp_component_inputs out_args out_deps directory)
    set(args "")
    set(deps "")
    if(NOT "${directory}" STREQUAL "")
        get_filename_component(directory "${directory}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if(NOT IS_DIRECTORY "${directory}")
            message(FATAL_ERROR "esp-gsp: COMPONENTS_DIR is not a directory: ${directory}")
        endif()
        set(args --components-dir "${directory}")
        set(deps "${directory}")
    endif()
    set(${out_args} "${args}" PARENT_SCOPE)
    set(${out_deps} "${deps}" PARENT_SCOPE)
endfunction()

function(_esp_gsp_scene_stem scene out)
    set(scene_error "")
    if(NOT EXISTS "${scene}" OR IS_DIRECTORY "${scene}")
        set(scene_error "esp-gsp: scene JSON does not exist: ${scene}")
    else()
        file(READ "${scene}" scene_json)
        string(JSON scene_screen ERROR_VARIABLE scene_json_error
            GET "${scene_json}" screen)
        if(scene_json_error)
            set(scene_error "esp-gsp: cannot read screen from ${scene}: ${scene_json_error}")
        endif()
    endif()
    if(scene_error)
        if(CMAKE_SCRIPT_MODE_FILE)
            # The owning CMakeLists may have removed this old input. Let its
            # reconfigure decide the current registration before diagnosing it.
            set(${out} "!invalid:${scene}" PARENT_SCOPE)
            return()
        endif()
        message(FATAL_ERROR "${scene_error}")
    endif()
    string(REGEX REPLACE "[^A-Za-z0-9_]+" "_" stem "${scene_screen}")
    string(REGEX REPLACE "^_+" "" stem "${stem}")
    string(REGEX REPLACE "_+$" "" stem "${stem}")
    string(TOLOWER "${stem}" stem)
    if(NOT stem)
        set(stem scene)
    endif()
    set(${out} "${stem}" PARENT_SCOPE)
endfunction()

function(_esp_gsp_write_scene_names path content)
    if(EXISTS "${path}")
        file(READ "${path}" previous)
    endif()
    if(NOT EXISTS "${path}" OR NOT "${previous}" STREQUAL "${content}")
        get_filename_component(directory "${path}" DIRECTORY)
        file(MAKE_DIRECTORY "${directory}")
        file(WRITE "${path}" "${content}")
    endif()
endfunction()

# Required headers are OUTPUTs and target dependencies, so both Make and Ninja
# can repair a partial generated tree. Other scene reports remain byproducts.
function(esp_gsp_scene_outputs out_headers out_byproducts gen_dir deployable)
    cmake_parse_arguments(SCENE "NINJA_NAMES_ONLY" "" "" ${ARGN})
    set(filter_names FALSE)
    if(SCENE_NINJA_NAMES_ONLY AND CMAKE_GENERATOR MATCHES "^Ninja")
        set(filter_names TRUE)
    endif()
    set(headers "")
    set(byproducts "")
    set(scene_index 0)
    set(scene_names "")
    foreach(abs IN LISTS SCENE_UNPARSED_ARGUMENTS)
        _esp_gsp_scene_stem("${abs}" scene_stem)
        string(APPEND scene_names "${scene_stem}\n")
        if(NOT filter_names)
            # Other callers and generators keep normal configure dependencies.
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${abs}")
        endif()
        set(scene_dir "${gen_dir}/scene${scene_index}")
        list(APPEND headers
            "${scene_dir}/${scene_stem}_binds.h"
            "${scene_dir}/${scene_stem}_actions.h"
            "${scene_dir}/${scene_stem}_objects.h"
            "${scene_dir}/${scene_stem}_templates.h")
        list(APPEND byproducts
            "${scene_dir}/${scene_stem}.gsb"
            "${scene_dir}/${scene_stem}.api.json"
            "${scene_dir}/${scene_stem}.execution.json"
            "${scene_dir}/${scene_stem}.manifest.md")
        if(deployable)
            list(APPEND byproducts
                "${scene_dir}/${scene_stem}.gmd")
        endif()
        math(EXPR scene_index "${scene_index} + 1")
    endforeach()
    if(filter_names)
        # Ninja updates generated configure inputs before checking build.ninja.
        # Only filename changes refresh the graph. Check before reconfigure,
        # including when the owning CMakeLists has removed an old scene file.
        set(names_path "${gen_dir}/scene_names.stamp")
        _esp_gsp_write_scene_names("${names_path}" "${scene_names}")
        string(SHA256 names_key "${names_path}")
        string(SUBSTRING "${names_key}" 0 16 names_key)
        add_custom_target(gsp_scene_names_${names_key}
            COMMAND "${CMAKE_COMMAND}" "-DGSP_SCENE_NAMES_FILE=${names_path}"
                "-DGSP_SCENE_SOURCES=${SCENE_UNPARSED_ARGUMENTS}" -P "${CMAKE_CURRENT_FUNCTION_LIST_FILE}"
            BYPRODUCTS "${names_path}"
            COMMENT "Checking GSP scene names" VERBATIM)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${names_path}")
    endif()
    set(${out_headers} "${headers}" PARENT_SCOPE)
    set(${out_byproducts} "${byproducts}" PARENT_SCOPE)
endfunction()

if(CMAKE_SCRIPT_MODE_FILE AND DEFINED GSP_SCENE_NAMES_FILE)
    set(scene_names "")
    foreach(scene IN LISTS GSP_SCENE_SOURCES)
        _esp_gsp_scene_stem("${scene}" scene_stem)
        string(APPEND scene_names "${scene_stem}\n")
    endforeach()
    _esp_gsp_write_scene_names("${GSP_SCENE_NAMES_FILE}" "${scene_names}")
endif()
