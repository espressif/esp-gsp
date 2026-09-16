# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
cmake_minimum_required(VERSION 3.20)
project(gsp_sim_bridge LANGUAGES C)
include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/gsp_toolchain.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/gsp_compatibility.cmake")

add_library(gsp_sim_bridge STATIC src/transport.c src/api.c src/media.c src/images.c src/assets.c)
add_library(esp_gsp::sim_bridge ALIAS gsp_sim_bridge)
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
    add_test(NAME bridge_transport
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_transport.py"
            "$<TARGET_FILE:bridge_test_client>")
endif()

# Compile scenes and generated headers together using the component's tool
# selection contract (explicit override, otherwise esp-gsp-tools).
function(gsp_add_backend target)
    cmake_parse_arguments(ARG "" "PROFILE" "SOURCES;INCLUDE_DIRECTORIES;SCENES" ${ARGN})
    if(NOT ARG_SOURCES OR NOT ARG_SCENES)
        message(FATAL_ERROR "gsp_add_backend requires SOURCES and SCENES")
    endif()
    get_filename_component(component_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../.." ABSOLUTE)
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
    # Run on every build: scene JSON may reference images/fonts/imports outside
    # the JSON directory. This avoids silently stale bundle/header pairs.
    add_custom_target(${target}_bundle
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${generated}"
        COMMAND ${gspc_command} pack ${scenes} --deployable
            --profile "${profile}" --gen-dir "${generated}"
            --api-header "${generated}/bundle_gsp.h" --symbol bridge_bundle
            -o "${generated}/app.gspb"
        BYPRODUCTS "${generated}/bundle_gsp.h" "${generated}/app.gspb"
        VERBATIM)
    add_executable(${target} ${ARG_SOURCES}
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/main.c")
    add_dependencies(${target} ${target}_bundle)
    target_link_libraries(${target} PRIVATE esp_gsp::sim_bridge)
    target_include_directories(${target} PRIVATE "${generated}" ${ARG_INCLUDE_DIRECTORIES})
    # Generator expressions handle Ninja/Makefiles and Visual Studio/Xcode.
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/${target}-$<CONFIG>.json"
        CONTENT "{\"version\":1,\"component_root\":\"${component_root}\",\"executable\":\"$<TARGET_FILE:${target}>\",\"bundle\":\"${generated}/app.gspb\"}\n")
endfunction()
