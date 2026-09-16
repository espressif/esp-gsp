# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

include_guard(GLOBAL)

function(esp_gsp_resolve_gspc gsp_root output_command output_dependencies output_source)
    if(EXISTS "${gsp_root}/tools/gspc/Cargo.toml")
        set(compiler_help "Build this source checkout with cargo build --release --manifest-path ${gsp_root}/tools/gspc/Cargo.toml -p gsp-cli, then set GSPC_EXECUTABLE=${gsp_root}/tools/gspc/target/release/gspc.")
    else()
        set(compiler_help "Set GSPC_EXECUTABLE to a compatible compiler supplied with this component release. If that compiler is unavailable, obtain a matched component/compiler package from the distributor.")
    endif()
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

        # Contributors use the source entry point, which rebuilds stale tools.
        # A project version pin still selects the managed compiler instead.
        if(NOT version_file AND EXISTS "${gsp_root}/ci/gspc-dev"
                AND EXISTS "${gsp_root}/tools/gspc/Cargo.toml")
            find_program(gsp_cargo NAMES cargo)
            if(NOT gsp_cargo)
                message(FATAL_ERROR "gsp_add_bundle: Cargo is required for the source compiler. ${compiler_help}")
            endif()
            set(${output_command} "${gsp_root}/ci/gspc-dev" PARENT_SCOPE)
            set(${output_dependencies} "${gsp_root}/ci/gspc-dev" PARENT_SCOPE)
            set(${output_source} "source checkout (${gsp_root})" PARENT_SCOPE)
            return()
        endif()

        if(NOT version_file)
            message(FATAL_ERROR
                "gsp_add_bundle: no usable .gspc_version found. The marker "
                "is generated in CI component packages; for a project "
                "override, create ${project_root}/.gspc_version. ${compiler_help}")
        endif()

        file(READ "${version_file}" version_contents)
        string(STRIP "${version_contents}" gspc_version)
        if("${gspc_version}" STREQUAL "")
            message(FATAL_ERROR
                "gsp_add_bundle: ${version_file} is empty. Put the exact "
                "GSPC version in .gspc_version, or set GSPC_EXECUTABLE to a "
                "downloaded compatible compiler")
        endif()

        # Reuse an installed compiler without invoking a download manager.
        # A mismatched PATH compiler must not override the project's version pin.
        unset(gspc_on_path CACHE)
        find_program(gspc_on_path NAMES gspc)
        if(gspc_on_path)
            execute_process(COMMAND "${gspc_on_path}" --version
                RESULT_VARIABLE path_result OUTPUT_VARIABLE path_version
                ERROR_VARIABLE path_error OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_STRIP_TRAILING_WHITESPACE)
            if(path_result EQUAL 0 AND path_version STREQUAL "gspc ${gspc_version}")
                set(${output_command} "${gspc_on_path}" PARENT_SCOPE)
                set(${output_dependencies} "${gspc_on_path};${version_file}" PARENT_SCOPE)
                set(${output_source} "PATH (${gspc_on_path}; ${version_file})" PARENT_SCOPE)
                return()
            endif()
            message(STATUS
                "esp-gsp: ignoring PATH compiler ${gspc_on_path} ('${path_version}', result=${path_result}, ${path_error}); "
                "${version_file} requires GSPC ${gspc_version}; using the versioned manager")
        endif()

        # Use the consuming IDF environment when available, so manager
        # installation and invocation address the same Python interpreter.
        if(DEFINED PYTHON AND EXISTS "${PYTHON}")
            set(ESP_GSP_PYTHON_EXECUTABLE "${PYTHON}")
        else()
            find_program(ESP_GSP_PYTHON_EXECUTABLE NAMES python3 python)
        endif()
        if(NOT ESP_GSP_PYTHON_EXECUTABLE)
            message(FATAL_ERROR
                "gsp_add_bundle: Python is required to resolve GSPC "
                "automatically from ${version_file}. Install "
                "esp-gsp-tools with 'python -m pip install -U "
                "esp-gsp-tools' after installing Python. ${compiler_help}")
        endif()

        execute_process(
            COMMAND "${ESP_GSP_PYTHON_EXECUTABLE}" -c "import gsp.execute"
            RESULT_VARIABLE import_result ERROR_VARIABLE import_error
            OUTPUT_QUIET ERROR_STRIP_TRAILING_WHITESPACE)
        if(NOT import_result EQUAL 0)
            message(FATAL_ERROR
                "gsp_add_bundle: compiler manager is unavailable in ${ESP_GSP_PYTHON_EXECUTABLE}: ${import_error}. "
                "Install the manager with: \"${ESP_GSP_PYTHON_EXECUTABLE}\" -m pip install -U esp-gsp-tools. "
                "The manager and GSPC have independent versions. ${compiler_help}")
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
                "esp-gsp-tools is the download manager; its version is independent "
                "of GSPC. Installing/upgrading the manager cannot provide a missing "
                "release artifact. ${compiler_help}")
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
