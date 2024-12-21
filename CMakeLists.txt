# CMake Project
cmake_minimum_required(VERSION 3.27)
project(O2Jam LANGUAGES C CXX)
if (APPLE)
    enable_language(OBJCXX)
endif()

# CMake Modules
include(CheckIncludeFiles)
include(FetchContent)
include(schemas/generator.cmake)

# CMake Config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD OFF)
set(CMAKE_DISABLE_SOURCE_CHANGES OFF)

# Target
set(TARGET "OTwo")

# Sources and Headers
file(GLOB_RECURSE SRCS src/*.cpp extlibs/headers/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/headers/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/headers/*.h)
file(GLOB_RECURSE HEADERS schemas/headers/*.hpp)

# Schema
file(GLOB_RECURSE SCHEMA_FILES schemas/**/*.bop)
if (SCHEMA_FILES)
    message("-- Configuring schema generator --")
    set(SCHEMA_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/schemas/headers/OTwo/Serializable.g.hpp)
    Bebop_Generate(
        Application-Schema
        VERSION "v3.0.14"
        LANGUAGE cpp
        OUTPUT ${SCHEMA_OUTPUT}
        BOPS ${SCHEMA_FILES}
    )
endif()

# Default flags
if (NOT CMAKE_BUILD_TYPE)
    message("-- [Warning] CMAKE_BUILD_TYPE is not set. Falling back to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()
message("-- Build type: " ${CMAKE_BUILD_TYPE})

# External Libs Sources and Flags
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

# -- SFML
set(SFML_STATIC_LIBRARIES TRUE)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.0-rc.1
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
    GIT_TAG        v0.9.5
)
FetchContent_MakeAvailable(magic_enum)

# -- MessageBox
Check_Include_Files("gtk/gtk.h" GTK_AVAILABLE)
if(WIN32 OR APPLE OR GTK_AVAILABLE)
    # -- Boxer Flags
    message("-- System message box: Boxer")
    set(USE_BOXER TRUE)
    add_compile_definitions(USE_BOXER)
    set(BOXER_BUILD_EXAMPLES OFF CACHE BOOL "Compile boxer examples")
    FetchContent_Declare(
        Boxer
        GIT_REPOSITORY https://github.com/aaronmjacobs/Boxer.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(Boxer)
else()
    message("-- System message box: MessageBox-X11")
endif()

# Specific OS Directories and Flags
if(WIN32)
    # App Resources (Icon)
    set(APP_RESOURCE ${CMAKE_SOURCE_DIR}/resources/OTwo.o)
    add_executable(${TARGET} ${APP_RESOURCE} ${SRCS} ${HEADERS})

    # Output directory
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    # Windows (Visual Studio) Compiler Output Directories and Flags
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set_target_properties(${TARGET} PROPERTIES
        WIN32_EXECUTABLE                        TRUE
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # External Libs Compiler Flags
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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
    set(PACKAGE_NAME com.o2jam.game)
    add_executable(${TARGET} MACOSX_BUNDLE ${APP_ICON} ${SRCS} ${HEADERS})

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
        set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}")
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

    # Libraries
    target_link_libraries(${TARGET} PRIVATE stdc++fs)
endif()

message("-- Output Directory: " ${OUTPUT_DIR})

# Application includes
target_include_directories(${TARGET} PRIVATE src)
target_include_directories(${TARGET} PRIVATE include)
target_include_directories(${TARGET} PRIVATE extlibs/headers)
target_include_directories(${TARGET} PRIVATE schemas/headers)

# Link JSON
target_link_libraries(${TARGET} PRIVATE nlohmann_json::nlohmann_json)

# Link fmt
target_link_libraries(${TARGET} PRIVATE fmt::fmt)

# Link Magic Enum
target_link_libraries(${TARGET} PRIVATE magic_enum::magic_enum)

# Link Boxer
if(USE_BOXER)
    target_link_libraries(${TARGET} PRIVATE Boxer)
endif()

# Link SFML
target_link_libraries(${TARGET} PRIVATE SFML::Audio SFML::Graphics SFML::System SFML::Window)
if(WIN32)
    target_link_libraries(${TARGET} PRIVATE SFML::Main)
endif()

# Include FreeType
find_package(Freetype REQUIRED)
target_link_libraries(${TARGET} PRIVATE Freetype::Freetype)
