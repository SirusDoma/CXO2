# CMake Project
cmake_minimum_required(VERSION 3.27)
project(O2Jam LANGUAGES C CXX)
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
set(TARGET "OTwo")

# Sources and Headers
file(GLOB_RECURSE SRCS
    src/main.cpp
    src/Genode/*.cpp
    src/OTwo/*.cpp
    extlibs/libs/internal/*.c
    extlibs/libs/internal/*.cc
    extlibs/libs/internal/*.cpp
)

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
    # App Resources (Icon)
    set(APP_RESOURCE ${CMAKE_SOURCE_DIR}/resources/OTwo.rc)
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

    # App Icon
    set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources/)
    set(MACOSX_BUNDLE_ICON_FILE OTwo.icns)
    set(APP_ICON ${RESOURCES_DIR}/${MACOSX_BUNDLE_ICON_FILE})
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    # Output Directory
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/osx/${CMAKE_OSX_ARCHITECTURES}/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

    # Executable
    set(PACKAGE_NAME com.cxo2.game)
    add_executable(${TARGET} MACOSX_BUNDLE ${APP_ICON} ${SRCS} ${HEADERS})
    target_sources(${TARGET} PRIVATE ${APP_ICON})

    # MacOS Bundle flag
    set_target_properties(${TARGET} PROPERTIES
        BUNDLE                                  TRUE
        MACOSX_BUNDLE                           TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER            ${PACKAGE_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME               ${TARGET}
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

    # Freetype
    find_package(Freetype REQUIRED)

    # Fontconfig for font discovery on Linux
    find_package(Fontconfig QUIET)
    if (Fontconfig_FOUND)
        target_link_libraries(${TARGET} PRIVATE Fontconfig::Fontconfig)
        target_compile_definitions(${TARGET} PRIVATE GX_HAS_FONTCONFIG)
    endif()
endif()

# External Libs Sources and Flags
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

# -- SFML
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.1
)
FetchContent_MakeAvailable(SFML)

# -- JSON
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# -- FMT
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG        11.0.2
)
FetchContent_MakeAvailable(fmt)

# -- Magic Enum
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        ecdeb1cea54d87b09bafe9999faf56654646209d
)
FetchContent_MakeAvailable(magic_enum)


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
