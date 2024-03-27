# CMake Config
cmake_minimum_required(VERSION 3.27)
project(O2Jam)

# Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/headers/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/headers/*.h)
include_directories(src)
include_directories(include)
include_directories(extlibs/headers)

# External Libs Sources and Flags
# -- SFML
set(SFML_DIR "${SFML_ROOT}/cmake/SFML")
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
# -- Clip Flags
set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")
add_subdirectory(extlibs/headers/clip)
# -- Boxer Flags
set(BOXER_BUILD_EXAMPLES OFF CACHE BOOL "Compile boxer examples")
add_subdirectory(extlibs/headers/boxer)

# Aliases
set(EXECUTABLE_NAME "OTwo")

# Specific OS Directories and Flags
if(WIN32)
    # App Resources (Icon)
    set(APP_RESOURCE ${CMAKE_SOURCE_DIR}/assets/App.o)
    add_executable(${EXECUTABLE_NAME} ${APP_RESOURCE} ${SRCS} ${HEADERS})

    # Output directory
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    # Windows (Visual Studio) Compiler Output Directories and Flags
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        WIN32_EXECUTABLE                        TRUE
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # External Libs Compiler Flags
    set(SFML_STATIC_LIBRARIES TRUE)
    set_property(TARGET ${EXECUTABLE_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET clip PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET Boxer PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # Set dependencies libs
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-msvc/${CMAKE_BUILD_TYPE}")
    set(OpenAL_LIB ${MODULES_DIR}/openal32.lib)
elseif(APPLE)
    # App Arch
    if (NOT CMAKE_OSX_ARCHITECTURES)
        message("[Warning] CMAKE_OSX_ARCHITECTURES is not set. Falling back to CMAKE_HOST_SYSTEM_PROCESSOR")
        set(CMAKE_OSX_ARCHITECTURES ${CMAKE_HOST_SYSTEM_PROCESSOR})
    endif()

    if (NOT CMAKE_BUILD_TYPE)
        message("[Warning] CMAKE_BUILD_TYPE is not set. Falling back to Debug")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()

    message("Build type: " ${CMAKE_BUILD_TYPE})
    message("Architecture: " ${CMAKE_OSX_ARCHITECTURES})

    # App Icon
    set(MACOSX_BUNDLE_ICON_FILE OTwo.icns)
    set(APP_ICON ${CMAKE_SOURCE_DIR}/assets/${MACOSX_BUNDLE_ICON_FILE})
    set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    # Output Directory
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/osx/${CMAKE_OSX_ARCHITECTURES}/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

    # Executable
    set(PACKAGE_NAME com.o2jam.game)
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${APP_ICON} ${SRCS} ${HEADERS})

    # MacOS Bundle flag
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        BUNDLE                                  TRUE
        MACOSX_BUNDLE                           TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER            ${PACKAGE_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME               ${EXECUTABLE_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION            "3.10"
        MACOSX_BUNDLE_LONG_VERSION_STRING       "3.10"
        MACOSX_BUNDLE_SHORT_VERSION_STRING      "3.10"
        MACOSX_BUNDLE_COPYRIGHT                 "Copyright O2Media"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # Set dependencies libs
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework CoreAudio")
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework AudioUnit")
else()
    # Executable
    add_executable(${EXECUTABLE_NAME} ${SRCS} ${HEADERS})

    # Compiler Flags
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
        set(CMAKE_C_COMPILER gcc)
        set(CMAKE_CXX_COMPILER g++)
    else()
        set(CMAKE_C_COMPILER gcc-8)
        set(CMAKE_CXX_COMPILER g++-8)
    endif()
    set(CMAKE_C_FLAGS "-lstdc++fs -std=c++17")
    set(CMAKE_CXX_FLAGS "-lstdc++fs -std=c++17")

    # Output Directory
    if(UNIX)
        set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/linux/${CMAKE_BUILD_TYPE}")
    else()
        set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}")
    endif()
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY                ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY                ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR})

    # Set dependencies libs
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-gcc")
    set(FreeType_LIB ${MODULES_DIR}/libfreetype.a)
    set(OpenAL_LIB ${MODULES_DIR}/libopenal32.a)
    set(Ogg_LIB ${MODULES_DIR}/libogg.a)
    set(FLAC_LIB ${MODULES_DIR}/libFLAC.a)
    set(Vorbis_LIB ${MODULES_DIR}/libvorbis.a)
    set(VorbisEnc_LIB ${MODULES_DIR}/libvorbisenc.a)
    set(VorbisFile_LIB ${MODULES_DIR}/libvorbisfile.a)

    # FileSystem library
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE stdc++fs)
endif()

message("Output Directory: " ${OUTPUT_DIR})
message("")

# Link JSON
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Link Clip
target_link_libraries(${EXECUTABLE_NAME} PRIVATE clip)

# Link Boxer
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Boxer)

# Link Magic Enum
find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE magic_enum::magic_enum)

# Link SFML
find_package(SFML COMPONENTS audio graphics system window REQUIRED)
if (WIN32)
    find_package(SFML COMPONENTS main REQUIRED)
endif()

if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-audio sfml-graphics sfml-system sfml-window)

    if (WIN32)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-main)
    endif()
else()
    message(FATAL_ERROR "SFML not found")
endif()
