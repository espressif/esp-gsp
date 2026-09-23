# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
#
# Scenario media is restored from the pinned archive when it is absent from a
# source checkout or an extracted example package.

set(ESP_GSP_EXAMPLE_ASSETS_DEFAULT_URL
    "https://dl.espressif.com/AE/esp-dev-kits/esp-gsp-scenario-assets-0c56282d434d.tar.gz")
set(ESP_GSP_EXAMPLE_ASSETS_DEFAULT_SHA256
    "0c56282d434d24e8e6997e5d98db95b907c13b381ab8aba6db2ad124dc4ec0a0")

# Standalone source-preview preparation uses the same archive and extractor as
# device builds. Including this module from a project only defines the helper.
if(CMAKE_SCRIPT_MODE_FILE STREQUAL CMAKE_CURRENT_LIST_FILE)
    get_filename_component(asset_component_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
    if(NOT DEFINED ASSET_CACHE_ROOT)
        set(ASSET_CACHE_ROOT "${asset_component_root}/.ci-build/example-assets")
    endif()
    set(ASSET_URL "${ESP_GSP_EXAMPLE_ASSETS_DEFAULT_URL}")
    set(ASSET_SHA256 "${ESP_GSP_EXAMPLE_ASSETS_DEFAULT_SHA256}")
    set(ASSET_ARCHIVE "${ASSET_CACHE_ROOT}/assets-${ASSET_SHA256}.tar.gz")
    set(ASSET_DESTINATION "${asset_component_root}/examples")
    set(ASSET_SENTINEL "${ASSET_DESTINATION}/scenarios/coffee_machine/scenes/assets/coffee_hero.png")
    set(ASSET_STAMP "${ASSET_CACHE_ROOT}/assets-${ASSET_SHA256}-preview.stamp")
    set(ASSET_SUBTREE "")
    set(ASSET_WIDGETS "")
    include("${CMAKE_CURRENT_LIST_DIR}/gsp_fetch_example_assets.cmake")
    return()
endif()

function(gsp_example_assets_prepare component_root out_target)
    set(${out_target} "" PARENT_SCOPE)
    if(NOT EXISTS "${component_root}/cmake/gsp_fetch_example_assets.cmake")
        return()
    endif()

    set(asset_subtree "")
    set(asset_widgets "")
    set(asset_destination "${component_root}/examples")
    set(asset_sentinel
        "${component_root}/examples/scenarios/coffee_machine/scenes/assets/coffee_hero.png")
    if(DEFINED ESP_GSP_EXAMPLE_ASSETS_SUBTREE AND
            NOT "${ESP_GSP_EXAMPLE_ASSETS_SUBTREE}" STREQUAL "")
        if(NOT DEFINED ESP_GSP_EXAMPLE_ASSETS_SENTINEL OR
                "${ESP_GSP_EXAMPLE_ASSETS_SENTINEL}" STREQUAL "")
            message(FATAL_ERROR
                "ESP_GSP_EXAMPLE_ASSETS_SENTINEL is required with "
                "ESP_GSP_EXAMPLE_ASSETS_SUBTREE")
        endif()
        # Release examples are detached from the component root. Their
        # CMakeLists.txt declares the archive subtree and local sentinel so
        # the same pinned archive can restore only that example in place.
        set(asset_subtree "${ESP_GSP_EXAMPLE_ASSETS_SUBTREE}")
        if(DEFINED ESP_GSP_EXAMPLE_ASSETS_WIDGETS)
            set(asset_widgets "${ESP_GSP_EXAMPLE_ASSETS_WIDGETS}")
        endif()
        if("${asset_subtree}" STREQUAL "performance/benchmark" OR
                "${asset_subtree}" STREQUAL "usage/effects")
            list(APPEND asset_widgets image carousel)
            list(REMOVE_DUPLICATES asset_widgets)
        endif()
        if(DEFINED ESP_GSP_EXAMPLE_ASSETS_DESTINATION AND
                NOT "${ESP_GSP_EXAMPLE_ASSETS_DESTINATION}" STREQUAL "")
            get_filename_component(asset_destination
                "${ESP_GSP_EXAMPLE_ASSETS_DESTINATION}" ABSOLUTE
                BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        elseif(DEFINED PROJECT_DIR AND IS_DIRECTORY "${PROJECT_DIR}")
            set(asset_destination "${PROJECT_DIR}")
        else()
            set(asset_destination "${CMAKE_CURRENT_SOURCE_DIR}")
        endif()
        set(asset_sentinel "${asset_destination}/${ESP_GSP_EXAMPLE_ASSETS_SENTINEL}")
    else()
        # Only source-checkout scenario examples need the whole archive.
        # Published scenarios pass a subtree and local sentinel above.
        file(RELATIVE_PATH project_relative "${component_root}"
             "${CMAKE_CURRENT_SOURCE_DIR}")
        file(TO_CMAKE_PATH "${project_relative}" project_relative)
        if(NOT project_relative MATCHES "^examples/scenarios/[^/]+(/|$)")
            return()
        endif()
    endif()

    set(force_fetch OFF)
    if(DEFINED ESP_GSP_EXAMPLE_ASSETS_FETCH AND ESP_GSP_EXAMPLE_ASSETS_FETCH)
        set(force_fetch ON)
    endif()
    if(NOT force_fetch AND EXISTS "${asset_sentinel}")
        set(asset_media_ready ON)
        foreach(widget IN LISTS asset_widgets)
            if("${widget}" STREQUAL "image")
                set(widget_sentinel
                    "${asset_destination}/assets/widgets/image/vector_planet.svg")
            elseif("${widget}" STREQUAL "carousel")
                set(widget_sentinel
                    "${asset_destination}/assets/widgets/carousel/home.svg")
            else()
                set(widget_sentinel "")
            endif()
            if(NOT "${widget_sentinel}" STREQUAL "" AND
                    NOT EXISTS "${widget_sentinel}")
                set(asset_media_ready OFF)
            endif()
        endforeach()
        if(asset_media_ready)
            return()
        endif()
    endif()

    set(asset_url "${ESP_GSP_EXAMPLE_ASSETS_DEFAULT_URL}")
    if(DEFINED ESP_GSP_EXAMPLE_ASSETS_URL AND
            NOT "${ESP_GSP_EXAMPLE_ASSETS_URL}" STREQUAL "")
        set(asset_url "${ESP_GSP_EXAMPLE_ASSETS_URL}")
    endif()
    set(asset_sha256 "${ESP_GSP_EXAMPLE_ASSETS_DEFAULT_SHA256}")
    if(DEFINED ESP_GSP_EXAMPLE_ASSETS_SHA256 AND
            NOT "${ESP_GSP_EXAMPLE_ASSETS_SHA256}" STREQUAL "")
        set(asset_sha256 "${ESP_GSP_EXAMPLE_ASSETS_SHA256}")
    endif()
    string(LENGTH "${asset_sha256}" asset_sha256_length)
    if(NOT asset_sha256_length EQUAL 64 OR
            NOT asset_sha256 MATCHES "^[0-9a-fA-F]+$")
        message(FATAL_ERROR
            "ESP_GSP_EXAMPLE_ASSETS_SHA256 must be a 64-character SHA256")
    endif()

    set(cache_root "${CMAKE_BINARY_DIR}/gsp_example_assets_cache")
    if(DEFINED ESP_GSP_EXAMPLE_ASSETS_CACHE_DIR AND
            NOT "${ESP_GSP_EXAMPLE_ASSETS_CACHE_DIR}" STREQUAL "")
        set(cache_root "${ESP_GSP_EXAMPLE_ASSETS_CACHE_DIR}")
    endif()
    set(archive "${cache_root}/assets-${asset_sha256}.tar.gz")
    string(MD5 subtree_hash "${asset_subtree}")
    set(stamp "${cache_root}/assets-${asset_sha256}-${subtree_hash}.stamp")
    string(SUBSTRING "${subtree_hash}" 0 12 target_suffix)
    set(target "gsp_example_assets_fetch_${target_suffix}")
    if(NOT TARGET ${target})
        add_custom_command(
            OUTPUT "${stamp}"
            COMMAND "${CMAKE_COMMAND}"
                "-DASSET_URL=${asset_url}"
                "-DASSET_SHA256=${asset_sha256}"
                "-DASSET_ARCHIVE=${archive}"
                "-DASSET_DESTINATION=${asset_destination}"
                "-DASSET_SENTINEL=${asset_sentinel}"
                "-DASSET_SUBTREE=${asset_subtree}"
                "-DASSET_WIDGETS=${asset_widgets}"
                "-DASSET_STAMP=${stamp}"
                "-DASSET_CACHE_ROOT=${cache_root}"
                "-DASSET_OFFLINE=${ESP_GSP_EXAMPLE_ASSETS_OFFLINE}"
                -P "${component_root}/cmake/gsp_fetch_example_assets.cmake"
            DEPENDS "${component_root}/cmake/gsp_fetch_example_assets.cmake"
            COMMENT "esp-gsp: preparing example assets"
            VERBATIM)
        add_custom_target(${target} DEPENDS "${stamp}")
    endif()
    set(${out_target} "${target}" PARENT_SCOPE)
endfunction()
