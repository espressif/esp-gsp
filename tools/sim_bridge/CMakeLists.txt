# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
cmake_minimum_required(VERSION 3.20)
project(gsp_sim_bridge LANGUAGES C)
include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/gsp_toolchain.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/gsp_compatibility.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/gsp_context.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/gsp_generated.cmake")

add_library(gsp_sim_bridge STATIC src/transport.c src/api.c src/media.c src/images.c src/assets.c)
add_library(esp_gsp::sim_bridge ALIAS gsp_sim_bridge)
set_target_properties(gsp_sim_bridge PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden)
target_compile_features(gsp_sim_bridge PUBLIC c_std_11)
target_include_directories(gsp_sim_bridge PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
if(WIN32)
    target_link_libraries(gsp_sim_bridge PUBLIC ws2_32)
endif()
if(MSVC)
    target_compile_definitions(gsp_sim_bridge PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(gsp_sim_bridge PRIVATE /W4)
    target_compile_options(gsp_sim_bridge PUBLIC /utf-8)
else()
    target_compile_options(gsp_sim_bridge PRIVATE -Wall -Wextra -Wpedantic)
endif()

option(GSP_SIM_BRIDGE_BUILD_TESTS "Build native bridge transport tests" OFF)
if(GSP_SIM_BRIDGE_BUILD_TESTS)
    enable_testing()
    find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
    add_executable(bridge_test_client tests/client.c)
    target_link_libraries(bridge_test_client PRIVATE esp_gsp::sim_bridge)
    target_include_directories(bridge_test_client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../ci/host")
    add_executable(bridge_inproc_close_test tests/inproc_close.c)
    target_link_libraries(bridge_inproc_close_test PRIVATE esp_gsp::sim_bridge)
    target_include_directories(bridge_inproc_close_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_test(NAME bridge_transport
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_transport.py"
            "$<TARGET_FILE:bridge_test_client>")
    add_test(NAME bridge_inproc_close COMMAND bridge_inproc_close_test)
    add_executable(bridge_assets_publication_test tests/assets_publication.c src/assets.c)
    target_include_directories(bridge_assets_publication_test PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../include")
    add_test(NAME bridge_assets_publication COMMAND bridge_assets_publication_test
        "${CMAKE_CURRENT_BINARY_DIR}/asset-publication.gspb")
    add_test(NAME bridge_rpc_client
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_rpc_client.py")
    add_test(NAME bridge_scenario
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_scenario.py")
    add_test(NAME bridge_interaction
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_interaction.py")
endif()

# Compile scenes and generated headers together using the component's tool
# selection contract (explicit override, otherwise esp-gsp-tools).
function(gsp_add_backend target)
    cmake_parse_arguments(ARG "" "PROFILE;COMPONENTS_DIR" "SOURCES;INCLUDE_DIRECTORIES;SCENES" ${ARGN})
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "gsp_add_backend: unknown arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if("COMPONENTS_DIR" IN_LIST ARG_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR "gsp_add_backend: missing values for: COMPONENTS_DIR")
    endif()
    if(NOT ARG_SOURCES OR NOT ARG_SCENES)
        message(FATAL_ERROR "gsp_add_backend requires SOURCES and SCENES")
    endif()
    esp_gsp_component_inputs(component_args component_deps "${ARG_COMPONENTS_DIR}")
    get_filename_component(component_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../.." ABSOLUTE)
    include("${component_root}/cmake/gsp_example_assets.cmake")
    gsp_example_assets_prepare("${component_root}" gsp_example_assets_target)
    # gsp_add_backend is called from the consumer's directory scope, not this
    # add_subdirectory scope. Load the values here for every backend target.
    include("${component_root}/cmake/gsp_compatibility_contract.cmake")
    esp_gsp_resolve_gspc("${component_root}" gspc_command gspc_dependencies gspc_source)
    esp_gsp_check_gspc_command(gspc_version ${gspc_command})
    message(STATUS "sim_bridge: using ${gspc_version} from ${gspc_source}")
    if(NOT ARG_PROFILE)
        set(ARG_PROFILE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/profiles/rgb565.yaml")
    endif()
    get_filename_component(profile "${ARG_PROFILE}" ABSOLUTE)
    set(scenes "")
    foreach(scene IN LISTS ARG_SCENES)
        get_filename_component(scene "${scene}" ABSOLUTE)
        list(APPEND scenes "${scene}")
    endforeach()
    set(generated "${CMAKE_CURRENT_BINARY_DIR}/${target}_assets")
    set(context_outputs "")
    set(context_manifest "")
    get_property(context_schema GLOBAL PROPERTY ESP_GSPC_CONTEXT_SCHEMA)
    if(context_schema STREQUAL "1")
        set(context_outputs "${generated}/bundle_gsp.context.json")
        set(context_manifest ",\"context\":\"${context_outputs}\"")
        esp_gsp_register_context("${context_outputs}")
    endif()
    if(gspc_command STREQUAL "${component_root}/ci/gspc-dev")
        file(GLOB_RECURSE compiler_sources CONFIGURE_DEPENDS
            "${component_root}/tools/gspc/*.rs" "${component_root}/tools/gspc/*/Cargo.toml")
        list(APPEND gspc_dependencies ${compiler_sources}
            "${component_root}/tools/gspc/Cargo.toml" "${component_root}/tools/gspc/Cargo.lock"
            "${component_root}/formats/gsp_format.yaml" "${component_root}/config/esp_gsp_config_schema.yml")
    endif()
    esp_gsp_scene_outputs(scene_headers scene_byproducts "${generated}" TRUE NINJA_NAMES_ONLY ${scenes})
    set(depfile "${generated}/app.d")
    # GSPC's depfile includes referenced fonts, images and component inputs.
    add_custom_command(OUTPUT "${generated}/app.gspb" "${generated}/bundle_gsp.h"
                              "${generated}/bundle_gsp.api.json" ${context_outputs} ${scene_headers}
        BYPRODUCTS ${scene_byproducts}
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${generated}"
        COMMAND ${gspc_command} pack ${scenes} --deployable
            ${component_args} --profile "${profile}" --gen-dir "${generated}"
            --depfile "${depfile}"
            --api-header "${generated}/bundle_gsp.h" --symbol bridge_bundle
            -o "${generated}/app.gspb"
        DEPENDS ${scenes} ${component_deps} "${profile}" ${gspc_dependencies} ${gsp_example_assets_target}
        DEPFILE "${depfile}"
        VERBATIM)
    add_custom_target(${target}_bundle DEPENDS "${generated}/app.gspb" "${generated}/bundle_gsp.h"
                                             "${generated}/bundle_gsp.api.json" ${context_outputs} ${scene_headers})
    add_executable(${target} ${ARG_SOURCES}
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/main.c")
    add_library(${target}_module SHARED ${ARG_SOURCES}
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/library.c")
    add_dependencies(${target} ${target}_bundle)
    add_dependencies(${target}_module ${target}_bundle)
    target_link_libraries(${target} PRIVATE esp_gsp::sim_bridge)
    target_link_libraries(${target}_module PRIVATE esp_gsp::sim_bridge)
    target_include_directories(${target} PRIVATE "${generated}" ${ARG_INCLUDE_DIRECTORIES})
    target_include_directories(${target}_module PRIVATE "${generated}" ${ARG_INCLUDE_DIRECTORIES})
    set_target_properties(${target}_module PROPERTIES
        OUTPUT_NAME "${target}_backend"
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES)
    # Generator expressions handle Ninja/Makefiles and Visual Studio/Xcode.
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/${target}-$<CONFIG>.json"
        CONTENT "{\"version\":2,\"component_root\":\"${component_root}\",\"executable\":\"$<TARGET_FILE:${target}>\",\"backend_library\":\"$<TARGET_FILE:${target}_module>\",\"bundle\":\"${generated}/app.gspb\",\"api_json\":\"${generated}/bundle_gsp.api.json\"${context_manifest}}\n")
endfunction()
