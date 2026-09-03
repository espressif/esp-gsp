# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/gsp_compatibility_contract.cmake")

function(_esp_gsp_json_get output json)
    string(JSON value ERROR_VARIABLE json_error GET "${json}" ${ARGN})
    if(json_error)
        string(JOIN "." json_path ${ARGN})
        message(FATAL_ERROR
            "esp-gsp: invalid GSPC compatibility response at ${json_path}: "
            "${json_error}")
    endif()
    set(${output} "${value}" PARENT_SCOPE)
endfunction()

function(_esp_gsp_require_equal label actual expected)
    if(NOT "${actual}" STREQUAL "${expected}")
        message(FATAL_ERROR
            "esp-gsp: incompatible GSPC ${label}: compiler=${actual}, "
            "component=${expected}. Select the recommended GSPC release or "
            "update the ESP-GSP component.")
    endif()
endfunction()

function(esp_gsp_validate_gspc_compatibility compatibility_json)
    _esp_gsp_json_get(contract_schema "${compatibility_json}"
        schema_version)
    _esp_gsp_require_equal("contract schema" "${contract_schema}"
        "${ESP_GSP_COMPATIBILITY_SCHEMA_VERSION}")

    foreach(format_name IN LISTS ESP_GSP_COMPATIBILITY_FORMATS)
        string(TOUPPER "${format_name}" format_upper)
        _esp_gsp_json_get(actual_major "${compatibility_json}"
            formats "${format_name}" major)
        _esp_gsp_json_get(actual_minor "${compatibility_json}"
            formats "${format_name}" minor)
        _esp_gsp_require_equal("${format_upper} major" "${actual_major}"
            "${ESP_GSP_COMPATIBILITY_${format_upper}_MAJOR}")
        _esp_gsp_require_equal("${format_upper} minor" "${actual_minor}"
            "${ESP_GSP_COMPATIBILITY_${format_upper}_MINOR}")
    endforeach()

    foreach(part IN ITEMS major minor)
        string(TOUPPER "${part}" part_upper)
        _esp_gsp_json_get(actual "${compatibility_json}"
            formats gspb_deployable "${part}")
        _esp_gsp_require_equal("GSPB deployable ${part}" "${actual}"
            "${ESP_GSP_COMPATIBILITY_GSPB_DEPLOYABLE_${part_upper}}")
        _esp_gsp_json_get(actual "${compatibility_json}"
            gmd_runtime_abi "${part}")
        _esp_gsp_require_equal("GMD runtime ABI ${part}" "${actual}"
            "${ESP_GSP_COMPATIBILITY_GMD_RUNTIME_ABI_${part_upper}}")
    endforeach()

    _esp_gsp_json_get(actual "${compatibility_json}" requirements_version)
    _esp_gsp_require_equal("requirements record" "${actual}"
        "${ESP_GSP_COMPATIBILITY_REQUIREMENTS_VERSION}")
    _esp_gsp_json_get(actual "${compatibility_json}" config schema_version)
    _esp_gsp_require_equal("configuration schema" "${actual}"
        "${ESP_GSP_COMPATIBILITY_CONFIG_SCHEMA}")
    _esp_gsp_json_get(actual "${compatibility_json}" config abi_version)
    _esp_gsp_require_equal("configuration ABI" "${actual}"
        "${ESP_GSP_COMPATIBILITY_CONFIG_ABI}")
endfunction()

function(esp_gsp_check_gspc executable output_version)
    execute_process(
        COMMAND "${executable}" compatibility
        RESULT_VARIABLE compatibility_result
        OUTPUT_VARIABLE compatibility_json
        ERROR_VARIABLE compatibility_error
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT compatibility_result EQUAL 0)
        message(FATAL_ERROR
            "esp-gsp: ${executable} does not provide a compatible machine-"
            "readable contract: ${compatibility_error}")
    endif()
    esp_gsp_validate_gspc_compatibility("${compatibility_json}")
    _esp_gsp_json_get(gspc_version "${compatibility_json}" gspc_version)
    set(${output_version} "gspc ${gspc_version}" PARENT_SCOPE)
endfunction()

function(esp_gsp_check_gspc_command output_version)
    execute_process(
        COMMAND ${ARGN} compatibility
        RESULT_VARIABLE compatibility_result
        OUTPUT_VARIABLE compatibility_json
        ERROR_VARIABLE compatibility_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE)
    if(NOT compatibility_result EQUAL 0)
        string(REPLACE ";" " " command_text "${ARGN}")
        message(FATAL_ERROR
            "esp-gsp: ${command_text} does not provide a compatible machine-"
            "readable contract: ${compatibility_error}")
    endif()
    esp_gsp_validate_gspc_compatibility("${compatibility_json}")
    _esp_gsp_json_get(gspc_version "${compatibility_json}" gspc_version)
    set(${output_version} "gspc ${gspc_version}" PARENT_SCOPE)
endfunction()
