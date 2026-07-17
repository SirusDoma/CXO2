# CMake Project
cmake_minimum_required(VERSION 3.27)
project(CXO2 LANGUAGES C CXX)
if (APPLE)
    enable_language(OBJCXX)
endif()

# CMake Modules
include(CheckIncludeFiles)
include(FetchContent)

# CMake Config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD OFF)
set(CMAKE_DISABLE_SOURCE_CHANGES OFF)

# Target
set(TARGET "CXO2")

# Executable output name kept as "OTwo" across all platforms (OTwo.exe / OTwo.app / OTwo)
set(OUTPUT_NAME "OTwo")

# Sources
file(GLOB_RECURSE SRCS
    src/main.cpp
    src/CXO2/*.cpp
    extlibs/libs/internal/*.c
    extlibs/libs/internal/*.cc
    extlibs/libs/internal/*.cpp
)

# Platform-specific sources
if(WIN32)
    list(FILTER SRCS EXCLUDE REGEX "/(Unix|macOS)/")
elseif(APPLE)
    list(FILTER SRCS EXCLUDE REGEX "/(Win32)/")
else()
    list(FILTER SRCS EXCLUDE REGEX "/(Win32|macOS)/")
endif()

# Headers
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/headers/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/headers/*.h)
file(GLOB_RECURSE HEADERS extlibs/libs/internal/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/libs/internal/*.h)
file(GLOB_RECURSE HEADERS schemas/headers/*.hpp)

# Default flags
if (NOT CMAKE_BUILD_TYPE)
    message("-- [Warning] CMAKE_BUILD_TYPE is not set. Falling back to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()
message("-- Build type: " ${CMAKE_BUILD_TYPE})

# Specific OS Directories and Flags
if(WIN32)
    # App Resources (Icon) (Uncomment to set icon; you need `OTwo.ico` in `resources` directory)
    # set(APP_RESOURCE ${CMAKE_SOURCE_DIR}/resources/OTwo.rc)
    add_executable(${TARGET} ${APP_RESOURCE} ${SRCS} ${HEADERS})

    # Output directory
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    # Windows (Visual Studio) Compiler Output Directories and Flags
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "Force static link" FORCE)
    set_target_properties(${TARGET} PROPERTIES
        WIN32_EXECUTABLE                        TRUE
        MSVC_RUNTIME_LIBRARY                    ${CMAKE_MSVC_RUNTIME_LIBRARY}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # Allow big obj (for embedded Resource.hpp)
    target_compile_options(${TARGET} PRIVATE /bigobj)

    # Windows API for fetching font
    target_link_libraries(${TARGET} PRIVATE Advapi32)
    target_link_libraries(${TARGET} PRIVATE User32)
elseif(APPLE)
    # App Arch
    if (NOT CMAKE_OSX_ARCHITECTURES)
        message("-- [Warning] CMAKE_OSX_ARCHITECTURES is not set. Falling back to CMAKE_HOST_SYSTEM_PROCESSOR")
        set(CMAKE_OSX_ARCHITECTURES ${CMAKE_HOST_SYSTEM_PROCESSOR})
    endif()

    message("-- Architecture: " ${CMAKE_OSX_ARCHITECTURES})

    # App resources
    set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources/)

    # App icon (Uncomment to set icon; you need `OTwo.icns` in `resources` directory)
    # set(MACOSX_BUNDLE_ICON_FILE OTwo.icns)
    # set(APP_ICON ${RESOURCES_DIR}/${MACOSX_BUNDLE_ICON_FILE})
    # set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    # Output Directory
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/osx/${CMAKE_OSX_ARCHITECTURES}/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

    # Executable
    set(PACKAGE_NAME com.cxo2.game)
    add_executable(${TARGET} MACOSX_BUNDLE ${APP_ICON} ${SRCS} ${HEADERS})

    # App icon
    # target_sources(${TARGET} PRIVATE ${APP_ICON})

    # MacOS Bundle flag
    set_target_properties(${TARGET} PROPERTIES
        BUNDLE                                  TRUE
        MACOSX_BUNDLE                           TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER            ${PACKAGE_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME               ${OUTPUT_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION            "3.10"
        MACOSX_BUNDLE_LONG_VERSION_STRING       "3.10"
        MACOSX_BUNDLE_SHORT_VERSION_STRING      "3.10"
        MACOSX_BUNDLE_COPYRIGHT                 "Copyright O2Media"
        MACOSX_BUNDLE_INFO_PLIST                "${RESOURCES_DIR}/OTwo.info.plist"
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS  "${RESOURCES_DIR}/OTwo.entitlements.plist"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # iconv
    find_library(ICONV_LIB iconv REQUIRED)
    target_link_libraries(${TARGET} PRIVATE ${ICONV_LIB})

    # Set dependencies libs
    target_link_libraries(${TARGET} PRIVATE "-framework Security")
    target_link_libraries(${TARGET} PRIVATE "-framework CoreText" "-framework CoreFoundation")
else()
    # Executable
    add_executable(${TARGET} ${SRCS} ${HEADERS})

    # Compiler Flags
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 8)
        message("-- FileSystem is not supported with your compiler version.")
        message("-- Compiler Version: " ${CMAKE_CXX_COMPILER_VERSION})
    endif()

    # Output Directory
    if(UNIX OR __linux__)
        set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/linux/${CMAKE_BUILD_TYPE}")
    else()
        message(FATAL_ERROR "Platform is not supported.")
    endif()

    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # Compiler flags
    target_compile_options(${TARGET} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wno-non-template-friend -Wno-virtual-move-assign>)

    # Set dependencies libs
    target_link_libraries(${TARGET} PRIVATE stdc++fs)

    # iconv
    find_library(ICONV_LIB iconv REQUIRED)
    target_link_libraries(${TARGET} PRIVATE ${ICONV_LIB})

    # Freetype
    find_package(Freetype REQUIRED)
endif()

# Set executable output name
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})

# External Libs Sources and Flags
set(SFML_USE_STATIC_STD_LIBS TRUE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

# -- Genode
add_subdirectory(modules/Genode)

# -- Magic Enum
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        ecdeb1cea54d87b09bafe9999faf56654646209d
)
FetchContent_MakeAvailable(magic_enum)

# -- Compact Encoding Detector
FetchContent_Declare(
    ced
    GIT_REPOSITORY https://github.com/google/compact_enc_det.git
    GIT_TAG        d127078cedef9c6642cbe592dacdd2292b50bb19
    SOURCE_SUBDIR  disabled
)
FetchContent_MakeAvailable(ced)
add_library(ced STATIC
    ${ced_SOURCE_DIR}/compact_enc_det/compact_enc_det.cc
    ${ced_SOURCE_DIR}/compact_enc_det/compact_enc_det_hint_code.cc
    ${ced_SOURCE_DIR}/util/encodings/encodings.cc
    ${ced_SOURCE_DIR}/util/languages/languages.cc
)
target_include_directories(ced PUBLIC ${ced_SOURCE_DIR})

message("-- Output Directory: " ${OUTPUT_DIR})

# Application includes
target_include_directories(${TARGET} PRIVATE src)
target_include_directories(${TARGET} PRIVATE include)
target_include_directories(${TARGET} PRIVATE extlibs/headers)
target_include_directories(${TARGET} PRIVATE extlibs/libs/internal)

# Link JSON
target_link_libraries(${TARGET} PRIVATE nlohmann_json::nlohmann_json)

# Link fmt
target_link_libraries(${TARGET} PRIVATE fmt::fmt)

# Link Magic Enum
target_link_libraries(${TARGET} PRIVATE magic_enum::magic_enum)

# Link Compact Encoding Detector
target_link_libraries(${TARGET} PRIVATE ced)

# Link Boost PFR
target_link_libraries(${TARGET} PRIVATE Boost::pfr)

# Link Genode
target_link_libraries(${TARGET} PRIVATE Genode)

# Link SFML
target_link_libraries(${TARGET} PRIVATE SFML::Audio SFML::Graphics SFML::System SFML::Window SFML::Network)
if(WIN32)
    target_link_libraries(${TARGET} PRIVATE SFML::Main)
endif()

# Include FreeType
target_link_libraries(${TARGET} PRIVATE Freetype::Freetype)

# Resource Compiler Target (separate from main build)
add_executable(ResourceCompiler
    src/ResourceCompiler/ResourceCompiler.cpp
)

# Resource Compiler includes and dependencies
target_include_directories(ResourceCompiler PRIVATE include)
target_include_directories(ResourceCompiler PRIVATE extlibs/headers)
target_link_libraries(ResourceCompiler PRIVATE Genode)
target_link_libraries(ResourceCompiler PRIVATE nlohmann_json::nlohmann_json)

# Resource Compiler output directory
set(COMPILER_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/compiler/${CMAKE_BUILD_TYPE}")
file(MAKE_DIRECTORY ${COMPILER_OUTPUT_DIR})
set_target_properties(ResourceCompiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${COMPILER_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${COMPILER_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${COMPILER_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${COMPILER_OUTPUT_DIR}
)
