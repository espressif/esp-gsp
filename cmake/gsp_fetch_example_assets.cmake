# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
# CMake-only downloader/extractor so Linux, macOS and Windows use the same
# implementation and do not require curl, tar, zstd or Python packages.

cmake_minimum_required(VERSION 3.20)

set(required_arguments ASSET_URL ASSET_SHA256 ASSET_ARCHIVE ASSET_CACHE_ROOT)
if(NOT ASSET_DOWNLOAD_ONLY)
    list(APPEND required_arguments ASSET_DESTINATION ASSET_SENTINEL ASSET_STAMP)
endif()
foreach(required IN LISTS required_arguments)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "Missing -D${required} for example asset fetch")
    endif()
endforeach()

if(NOT DEFINED ASSET_OFFLINE OR "${ASSET_OFFLINE}" STREQUAL "")
    set(ASSET_OFFLINE OFF)
endif()

string(TOLOWER "${ASSET_SHA256}" expected_sha256)
file(MAKE_DIRECTORY "${ASSET_CACHE_ROOT}")
set(download_part "${ASSET_ARCHIVE}.part")

set(archive_valid OFF)
if(EXISTS "${ASSET_ARCHIVE}")
    file(SHA256 "${ASSET_ARCHIVE}" existing_sha256)
    if(existing_sha256 STREQUAL expected_sha256)
        set(archive_valid ON)
    else()
        file(REMOVE "${ASSET_ARCHIVE}")
    endif()
endif()

if(NOT archive_valid)
    if(ASSET_OFFLINE)
        message(FATAL_ERROR
            "Example asset archive is not cached and offline mode is enabled: "
            "${ASSET_ARCHIVE}")
    endif()
    foreach(attempt RANGE 1 3)
        file(DOWNLOAD "${ASSET_URL}" "${download_part}"
            TLS_VERIFY ON
            TIMEOUT 120
            INACTIVITY_TIMEOUT 30
            STATUS download_status)
        list(GET download_status 0 download_code)
        if(download_code EQUAL 0)
            break()
        endif()
        file(REMOVE "${download_part}")
        list(GET download_status 1 download_message)
        if(attempt LESS 3)
            message(STATUS "Example asset download attempt ${attempt}/3 failed: ${download_message}; retrying")
            execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep 2)
        endif()
    endforeach()
    if(NOT download_code EQUAL 0)
        message(FATAL_ERROR
            "Cannot download example assets from ${ASSET_URL}: "
            "${download_message}")
    endif()
    file(SHA256 "${download_part}" downloaded_sha256)
    if(NOT downloaded_sha256 STREQUAL expected_sha256)
        file(REMOVE "${download_part}")
        message(FATAL_ERROR
            "Example asset SHA256 mismatch: expected ${ASSET_SHA256}, "
            "got ${downloaded_sha256}")
    endif()
    file(RENAME "${download_part}" "${ASSET_ARCHIVE}")
endif()

if(ASSET_DOWNLOAD_ONLY)
    return()
endif()

if(EXISTS "${ASSET_STAMP}" AND EXISTS "${ASSET_SENTINEL}")
    return()
endif()

set(stage "${ASSET_CACHE_ROOT}/extract-${ASSET_SHA256}")
file(REMOVE_RECURSE "${stage}")
file(MAKE_DIRECTORY "${stage}")
file(ARCHIVE_EXTRACT INPUT "${ASSET_ARCHIVE}" DESTINATION "${stage}")
set(asset_source "${stage}/examples")
if(NOT "${ASSET_SUBTREE}" STREQUAL "")
    set(asset_source "${asset_source}/${ASSET_SUBTREE}")
endif()
if(NOT EXISTS "${asset_source}")
    message(FATAL_ERROR
        "Example asset archive has no requested example subtree: "
        "${ASSET_ARCHIVE}")
endif()

set(asset_widgets "${ASSET_WIDGETS}")
if("${ASSET_SUBTREE}" STREQUAL "performance/benchmark" OR
        "${ASSET_SUBTREE}" STREQUAL "usage/effects")
    list(APPEND asset_widgets image carousel)
    list(REMOVE_DUPLICATES asset_widgets)
endif()
if(NOT "${asset_widgets}" STREQUAL "")
    foreach(widget IN LISTS asset_widgets)
        set(widget_source "${stage}/examples/usage/widgets/${widget}")
        if(NOT EXISTS "${widget_source}")
            message(FATAL_ERROR
                "Example asset archive has no requested widget subtree: "
                "${widget}")
        endif()
        file(COPY "${widget_source}/"
             DESTINATION "${ASSET_DESTINATION}/assets/widgets/${widget}")
    endforeach()
endif()

# Copy only after the archive has downloaded and extracted successfully. A
# failed build leaves no success stamp, so the next build retries safely.
file(COPY "${asset_source}/" DESTINATION "${ASSET_DESTINATION}")
if(NOT EXISTS "${ASSET_SENTINEL}")
    message(FATAL_ERROR
        "Example asset archive did not restore the requested sentinel")
endif()
file(WRITE "${ASSET_STAMP}"
    "sha256=${ASSET_SHA256}\nurl=${ASSET_URL}\n")
