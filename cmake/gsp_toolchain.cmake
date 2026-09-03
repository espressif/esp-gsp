# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

include_guard(GLOBAL)

function(esp_gsp_resolve_gspc gsp_root output_command output_dependencies output_source)
    # An explicit executable is an escape hatch for CI, offline builds, and
    # users who intentionally pin a binary outside esp-gsp-tools.
    if(DEFINED GSPC_EXECUTABLE AND
            NOT "${GSPC_EXECUTABLE}" STREQUAL "")
        set(gspc_command "${GSPC_EXECUTABLE}")
        set(gspc_dependencies "${GSPC_EXECUTABLE}")
        set(source "GSPC_EXECUTABLE CMake variable")
    elseif(DEFINED ENV{GSPC_EXECUTABLE} AND
            NOT "$ENV{GSPC_EXECUTABLE}" STREQUAL "")
        set(gspc_command "$ENV{GSPC_EXECUTABLE}")
        set(gspc_dependencies "$ENV{GSPC_EXECUTABLE}")
        set(source "GSPC_EXECUTABLE environment variable")
    else()
        if(DEFINED PROJECT_DIR AND NOT "${PROJECT_DIR}" STREQUAL "")
            get_filename_component(project_root "${PROJECT_DIR}" ABSOLUTE)
        else()
            get_filename_component(project_root "${CMAKE_SOURCE_DIR}" ABSOLUTE)
        endif()

        # The consuming IDF project may deliberately select a different
        # compiler than the one used to build the component archive.
        set(version_file "")
        if(EXISTS "${project_root}/.gspc_version")
            set(version_file "${project_root}/.gspc_version")
        elseif(EXISTS "${gsp_root}/.gspc_version")
            set(version_file "${gsp_root}/.gspc_version")
        endif()

        if(NOT version_file)
            message(FATAL_ERROR
                "gsp_add_bundle: no usable .gspc_version found. The marker "
                "is generated in CI component packages; for a project "
                "override, create ${project_root}/.gspc_version, or download "
                "a compatible GSPC release and set GSPC_EXECUTABLE manually: "
                "https://github.com/espressif/esp-gsp/releases")
        endif()

        file(READ "${version_file}" version_contents)
        string(STRIP "${version_contents}" gspc_version)
        if("${gspc_version}" STREQUAL "")
            message(FATAL_ERROR
                "gsp_add_bundle: ${version_file} is empty. Put the exact "
                "GSPC version in .gspc_version, or set GSPC_EXECUTABLE to a "
                "downloaded compatible compiler")
        endif()

        find_program(ESP_GSP_PYTHON_EXECUTABLE NAMES python3 python)
        if(NOT ESP_GSP_PYTHON_EXECUTABLE)
            message(FATAL_ERROR
                "gsp_add_bundle: Python is required to resolve GSPC "
                "automatically from ${version_file}. Install "
                "esp-gsp-tools with 'python -m pip install -U "
                "esp-gsp-tools', or download GSPC from "
                "https://github.com/espressif/esp-gsp/releases and set "
                "GSPC_EXECUTABLE manually")
        endif()

        set(gspc_command "${ESP_GSP_PYTHON_EXECUTABLE}" -m gsp.execute
            --version "${gspc_version}" gspc)
        # This is deliberately a product invocation rather than a direct
        # manager install. gsp.execute owns the implicit cache/download path,
        # including its signature verification and cache locking.
        execute_process(
            COMMAND ${gspc_command} --version
            RESULT_VARIABLE manager_result
            OUTPUT_VARIABLE manager_output
            ERROR_VARIABLE manager_error
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_STRIP_TRAILING_WHITESPACE)
        if(NOT manager_result EQUAL 0)
            string(REPLACE ";" " " manager_diagnostic
                "${manager_error} ${manager_output}")
            message(FATAL_ERROR
                "gsp_add_bundle: esp-gsp-tools could not execute GSPC "
                "${gspc_version} from ${version_file}: ${manager_diagnostic}. "
                "Install it with 'python -m pip install -U esp-gsp-tools', "
                "or download a compatible release and set GSPC_EXECUTABLE "
                "manually: https://github.com/espressif/esp-gsp/releases")
        endif()
        set(gspc_dependencies "${ESP_GSP_PYTHON_EXECUTABLE}" "${version_file}")
        set(source "esp-gsp-tools ${gspc_version} (${version_file})")
    endif()

    if("${gspc_command}" STREQUAL "")
        message(FATAL_ERROR "gsp_add_bundle: GSPC command is empty")
    endif()

    set(${output_command} "${gspc_command}" PARENT_SCOPE)
    set(${output_dependencies} "${gspc_dependencies}" PARENT_SCOPE)
    set(${output_source} "${source}" PARENT_SCOPE)
endfunction()
