# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
# CMake-only downloader/extractor so Linux, macOS and Windows use the same
# implementation and do not require curl, tar, zstd or Python packages.

cmake_minimum_required(VERSION 3.20)

foreach(required IN ITEMS ASSET_URL ASSET_SHA256 ASSET_ARCHIVE
        ASSET_DESTINATION ASSET_SENTINEL ASSET_STAMP ASSET_CACHE_ROOT)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "Missing -D${required} for example asset fetch")
    endif()
endforeach()

if(NOT DEFINED ASSET_OFFLINE OR "${ASSET_OFFLINE}" STREQUAL "")
    set(ASSET_OFFLINE OFF)
endif()

file(MAKE_DIRECTORY "${ASSET_CACHE_ROOT}")
set(download_part "${ASSET_ARCHIVE}.part")

set(archive_valid OFF)
if(EXISTS "${ASSET_ARCHIVE}")
    file(SHA256 "${ASSET_ARCHIVE}" existing_sha256)
    if(existing_sha256 STREQUAL ASSET_SHA256)
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
    file(DOWNLOAD "${ASSET_URL}" "${download_part}"
        EXPECTED_HASH "SHA256=${ASSET_SHA256}"
        TLS_VERIFY ON
        TIMEOUT 120
        INACTIVITY_TIMEOUT 30
        STATUS download_status)
    list(GET download_status 0 download_code)
    if(NOT download_code EQUAL 0)
        list(GET download_status 1 download_message)
        message(FATAL_ERROR
            "Cannot download example assets from ${ASSET_URL}: "
            "${download_message}")
    endif()
    file(RENAME "${download_part}" "${ASSET_ARCHIVE}")
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
