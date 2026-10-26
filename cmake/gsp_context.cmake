# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
include_guard(GLOBAL)

# Recreate only this generated index on every configure, including a configure
# that removes the final bundle. The compiler owns the actual context sidecars.
if(NOT CMAKE_SCRIPT_MODE_FILE)
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/gsp_contexts")
    file(GLOB previous_context_indices "${CMAKE_BINARY_DIR}/gsp_contexts/*.txt")
    if(previous_context_indices)
        file(REMOVE ${previous_context_indices})
    endif()
endif()

function(esp_gsp_register_context context)
    string(SHA256 context_key "${context}")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/gsp_contexts/${context_key}.txt"
        CONTENT "${context}\n")
endfunction()
