# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

include_guard(GLOBAL)

# Register application-owned hook sources after the consumer target exists.
# Each source is wrapped and becomes an ordinary source of that consumer, so
# later target compile requirements, conditional dependencies and generated
# header ordering are handled by native CMake target semantics.
function(gsp_add_hooks target)
    if(NOT target OR NOT TARGET "${target}")
        message(FATAL_ERROR "gsp_add_hooks: target does not exist: ${target}")
    endif()
    if(MSVC OR WIN32 OR APPLE OR EMSCRIPTEN OR
       CMAKE_SYSTEM_NAME STREQUAL "WASI")
        message(FATAL_ERROR
            "gsp_add_hooks: core weak overrides require an ELF target with "
            "a GNU-compatible linker; use instance observers on this target")
    endif()

    # Match the capability guard used by esp_gsp_hooks.c, including custom
    # toolchains whose CMake system name does not identify the object format.
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    set(gsp_hook_capability_source "
#if !defined(__ELF__) || (!defined(__GNUC__) && !defined(__clang__))
#error Global hook overrides require ELF GCC/Clang
#endif
int gsp_hook_capability(void) { return 0; }
")
    if(CMAKE_C_COMPILER_LOADED)
        include(CheckCSourceCompiles)
        check_c_source_compiles("${gsp_hook_capability_source}"
            GSP_HOOKS_ELF_WEAK_SUPPORTED)
    elseif(CMAKE_CXX_COMPILER_LOADED)
        include(CheckCXXSourceCompiles)
        check_cxx_source_compiles("${gsp_hook_capability_source}"
            GSP_HOOKS_ELF_WEAK_SUPPORTED)
    endif()
    if(NOT GSP_HOOKS_ELF_WEAK_SUPPORTED)
        message(FATAL_ERROR
            "gsp_add_hooks: compiler does not support ELF weak overrides; "
            "use instance observers on this target")
    endif()

    cmake_parse_arguments(ARG "" "" "SOURCES" ${ARGN})
    if(ARG_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR
            "gsp_add_hooks(${target}): missing value for "
            "${ARG_KEYWORDS_MISSING_VALUES}")
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "gsp_add_hooks(${target}): unexpected arguments: "
            "${ARG_UNPARSED_ARGUMENTS}; use SOURCES <...>")
    endif()
    if(NOT ARG_SOURCES)
        message(FATAL_ERROR
            "gsp_add_hooks(${target}): SOURCES must contain at least one source")
    endif()

    get_target_property(gsp_alias_target "${target}" ALIASED_TARGET)
    if(gsp_alias_target)
        set(gsp_hook_target "${gsp_alias_target}")
    else()
        set(gsp_hook_target "${target}")
    endif()
    get_target_property(gsp_imported "${gsp_hook_target}" IMPORTED)
    if(gsp_imported)
        message(FATAL_ERROR
            "gsp_add_hooks(${target}): imported targets are not supported")
    endif()
    get_target_property(gsp_target_type "${gsp_hook_target}" TYPE)
    if(NOT gsp_target_type STREQUAL "STATIC_LIBRARY" AND
       NOT gsp_target_type STREQUAL "EXECUTABLE")
        message(FATAL_ERROR
            "gsp_add_hooks(${target}): target must be a non-imported "
            "STATIC_LIBRARY or EXECUTABLE (got ${gsp_target_type})")
    endif()

    get_target_property(gsp_source_dir "${gsp_hook_target}" SOURCE_DIR)
    get_property(gsp_registered_sources TARGET "${gsp_hook_target}"
        PROPERTY GSP_HOOK_SOURCE_KEYS)
    if(NOT gsp_registered_sources)
        set(gsp_registered_sources "")
    endif()
    get_target_property(gsp_target_sources "${gsp_hook_target}" SOURCES)
    if(NOT gsp_target_sources OR gsp_target_sources STREQUAL
       "gsp_target_sources-NOTFOUND")
        set(gsp_target_sources "")
    endif()

    set(gsp_source_keys "")
    foreach(gsp_source IN LISTS ARG_SOURCES)
        if(gsp_source MATCHES "\\$<")
            message(FATAL_ERROR
                "gsp_add_hooks(${target}): generator expressions are not "
                "valid hook sources: ${gsp_source}")
        endif()
        get_filename_component(gsp_source_abs "${gsp_source}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        if(NOT EXISTS "${gsp_source_abs}" OR
           IS_DIRECTORY "${gsp_source_abs}")
            message(FATAL_ERROR
                "gsp_add_hooks(${target}): hook source is not a file: "
                "${gsp_source_abs}")
        endif()
        get_filename_component(gsp_source_key "${gsp_source_abs}" REALPATH)
        list(FIND gsp_source_keys "${gsp_source_key}" gsp_duplicate_index)
        if(NOT gsp_duplicate_index EQUAL -1)
            message(FATAL_ERROR
                "gsp_add_hooks(${target}): duplicate source in one call: "
                "${gsp_source_abs}")
        endif()
        list(FIND gsp_registered_sources "${gsp_source_key}"
            gsp_registered_index)
        if(NOT gsp_registered_index EQUAL -1)
            message(FATAL_ERROR
                "gsp_add_hooks(${target}): source was already registered: "
                "${gsp_source_abs}")
        endif()
        foreach(gsp_existing_source IN LISTS gsp_target_sources)
            if(gsp_existing_source MATCHES "^\\$<")
                continue()
            endif()
            get_filename_component(gsp_existing_abs "${gsp_existing_source}"
                ABSOLUTE BASE_DIR "${gsp_source_dir}")
            if(NOT EXISTS "${gsp_existing_abs}")
                continue()
            endif()
            get_filename_component(gsp_existing_key "${gsp_existing_abs}"
                REALPATH)
            if(gsp_existing_key STREQUAL gsp_source_key)
                message(FATAL_ERROR
                    "gsp_add_hooks(${target}): source is already part of "
                    "the target: ${gsp_existing_abs}")
            endif()
        endforeach()
        list(APPEND gsp_source_keys "${gsp_source_key}")
    endforeach()

    get_filename_component(gsp_wrapper_dir
        "${CMAKE_CURRENT_BINARY_DIR}/gsp_hooks" ABSOLUTE)
    file(MAKE_DIRECTORY "${gsp_wrapper_dir}")

    set(gsp_wrapper_sources "")
    set(gsp_anchor_names "")
    foreach(gsp_source IN LISTS ARG_SOURCES)
        get_filename_component(gsp_source_abs "${gsp_source}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        get_filename_component(gsp_source_key "${gsp_source_abs}" REALPATH)
        get_filename_component(gsp_extension "${gsp_source_abs}" EXT)
        string(TOLOWER "${gsp_extension}" gsp_extension_lower)
        if(gsp_extension STREQUAL ".C")
            set(gsp_wrapper_extension ".cpp")
        elseif(gsp_extension_lower STREQUAL ".c")
            set(gsp_wrapper_extension ".c")
        elseif(gsp_extension_lower STREQUAL ".cc" OR
               gsp_extension_lower STREQUAL ".cpp" OR
               gsp_extension_lower STREQUAL ".cxx")
            set(gsp_wrapper_extension ".cpp")
        else()
            message(FATAL_ERROR
                "gsp_add_hooks(${target}): only C and C++ hook sources are "
                "supported: ${gsp_source_abs}")
        endif()
        string(SHA256 gsp_source_hash
            "${gsp_hook_target}|${gsp_source_key}")
        string(SUBSTRING "${gsp_source_hash}" 0 20 gsp_source_hash_short)
        set(gsp_anchor "gsp_hook_anchor_${gsp_source_hash_short}")
        set(gsp_wrapper
            "${gsp_wrapper_dir}/${gsp_anchor}${gsp_wrapper_extension}")
        file(TO_CMAKE_PATH "${gsp_source_abs}" gsp_include_path)
        if(gsp_include_path MATCHES "[\"\r\n]")
            message(FATAL_ERROR
                "gsp_add_hooks: quoted/newline source paths cannot be included: ${gsp_source_abs}")
        endif()
        # LANGUAGE can explicitly make a .c source C++. Determine linkage in
        # the compiler, not from its filename. Keep timestamps on no-op reruns.
        set(gsp_wrapper_content "/* Generated by gsp_add_hooks; do not edit. */\n#ifdef __cplusplus\nextern \"C\"\n#endif\nvoid ${gsp_anchor}(void) {}\n#include \"${gsp_include_path}\"\n")
        set(gsp_previous_content "")
        if(EXISTS "${gsp_wrapper}")
            file(READ "${gsp_wrapper}" gsp_previous_content)
        endif()
        if(NOT gsp_previous_content STREQUAL gsp_wrapper_content)
            file(WRITE "${gsp_wrapper}" "${gsp_wrapper_content}")
        endif()
        set_source_files_properties("${gsp_wrapper}" PROPERTIES
            SKIP_UNITY_BUILD_INCLUSION TRUE)

        # Preserve source-specific settings when the caller attached them
        # before registration. Target-wide settings remain native because the
        # wrapper is a normal source of the consumer target.
        foreach(gsp_source_property IN ITEMS
                COMPILE_OPTIONS COMPILE_DEFINITIONS COMPILE_FLAGS
                INCLUDE_DIRECTORIES LANGUAGE OBJECT_DEPENDS)
            get_source_file_property(gsp_source_value "${gsp_source_abs}"
                "${gsp_source_property}")
            if(NOT gsp_source_value STREQUAL "NOTFOUND" AND
               NOT gsp_source_value STREQUAL "gsp_source_value-NOTFOUND" AND
               NOT gsp_source_value STREQUAL "")
                set_source_files_properties("${gsp_wrapper}" PROPERTIES
                    "${gsp_source_property}" "${gsp_source_value}")
            endif()
        endforeach()
        list(APPEND gsp_wrapper_sources "${gsp_wrapper}")
        list(APPEND gsp_anchor_names "${gsp_anchor}")
    endforeach()

    target_sources(${gsp_hook_target} PRIVATE ${gsp_wrapper_sources})
    set(gsp_anchor_options)
    foreach(gsp_anchor IN LISTS gsp_anchor_names)
        list(APPEND gsp_anchor_options "-Wl,-u,${gsp_anchor}")
    endforeach()
    if(gsp_target_type STREQUAL "EXECUTABLE")
        target_link_options(${gsp_hook_target} PRIVATE ${gsp_anchor_options})
    else()
        target_link_options(${gsp_hook_target} INTERFACE ${gsp_anchor_options})
    endif()
    set_property(TARGET ${gsp_hook_target} APPEND PROPERTY
        GSP_HOOK_SOURCE_KEYS ${gsp_source_keys})
endfunction()
