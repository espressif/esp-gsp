# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
include_guard(GLOBAL)

# Register after every component has contributed bundles and external assets.
function(_esp_gsp_register_feature_report)
    get_property(registered GLOBAL PROPERTY ESP_GSP_FEATURE_REPORT_REGISTERED)
    if(NOT registered)
        set_property(GLOBAL PROPERTY ESP_GSP_FEATURE_REPORT_REGISTERED TRUE)
        cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL _esp_gsp_finalize_feature_report)
    endif()
endfunction()

function(_esp_gsp_finalize_feature_report)
    get_property(bundles GLOBAL PROPERTY ESP_GSP_RUNTIME_BUNDLES)
    if(NOT bundles)
        return()
    endif()
    get_property(gspc_command GLOBAL PROPERTY ESP_GSPC_COMMAND)
    if(NOT gspc_command)
        get_filename_component(gsp_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
        include("${gsp_root}/cmake/gsp_toolchain.cmake")
        include("${gsp_root}/cmake/gsp_compatibility.cmake")
        esp_gsp_resolve_gspc("${gsp_root}" gspc_command dependencies source)
        esp_gsp_check_gspc_command(version ${gspc_command})
        set_property(GLOBAL PROPERTY ESP_GSPC_COMMAND "${gspc_command}")
        set_property(GLOBAL PROPERTY ESP_GSPC_DEPENDENCIES "${dependencies}")
    endif()
    get_property(supported GLOBAL PROPERTY ESP_GSPC_FEATURE_REPORT)
    # Version 2 reads final bundles; legacy sidecar reports use another contract.
    if(NOT supported STREQUAL "2")
        return()
    endif()
    list(REMOVE_DUPLICATES bundles)
    list(SORT bundles)
    get_property(bundle_targets GLOBAL PROPERTY ESP_GSP_RUNTIME_BUNDLE_TARGETS)
    get_property(dynamic_features GLOBAL PROPERTY ESP_GSP_RUNTIME_DYNAMIC_FEATURES)
    if(NOT dynamic_features)
        set(dynamic_features auto)
    endif()
    set(arguments features ${bundles}
        --sdkconfig "${CMAKE_BINARY_DIR}/config/sdkconfig.json"
        --runtime "${dynamic_features}")
    add_custom_target(gsp_feature_report
        COMMAND ${gspc_command} ${arguments}
        DEPENDS ${bundles}
        USES_TERMINAL VERBATIM)
    if(bundle_targets)
        add_dependencies(gsp_feature_report ${bundle_targets})
    endif()
    get_property(gsp_lib GLOBAL PROPERTY ESP_GSP_COMPONENT_LIB)
    if(gsp_lib AND TARGET ${gsp_lib})
        get_target_property(disabled ${gsp_lib} ESP_GSP_RUNTIME_DISABLED_FEATURES)
        # Check capability ceilings against final resources, then reuse the
        # result until the bundles, compiler, arguments or configuration change.
        if(disabled)
            set(check_stamp "${CMAKE_BINARY_DIR}/esp_gsp/feature_check.stamp")
            set(check_inputs "${CMAKE_BINARY_DIR}/esp_gsp/feature_check.inputs")
            file(GENERATE OUTPUT "${check_inputs}" CONTENT "${arguments}\n")
            get_property(gspc_dependencies GLOBAL PROPERTY ESP_GSPC_DEPENDENCIES)
            add_custom_command(OUTPUT "${check_stamp}"
                COMMAND ${gspc_command} ${arguments} --check
                COMMAND "${CMAKE_COMMAND}" -E touch "${check_stamp}"
                DEPENDS ${bundles} ${gspc_dependencies} "${check_inputs}"
                    "${CMAKE_BINARY_DIR}/config/sdkconfig.json"
                COMMENT "Checking GSP link capabilities" VERBATIM)
            add_custom_target(gsp_feature_check DEPENDS "${check_stamp}")
            if(bundle_targets)
                add_dependencies(gsp_feature_check ${bundle_targets})
            endif()
            add_dependencies(${gsp_lib} gsp_feature_check)
        endif()
    endif()
endfunction()
