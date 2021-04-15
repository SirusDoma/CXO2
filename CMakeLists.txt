# CMake Config
cmake_minimum_required(VERSION 3.16)
project(O2Jam)

# Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources
file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
file(GLOB_RECURSE HEADERS extlibs/headers/*.hpp)
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

# Executable
set(EXECUTABLE_NAME "OTwo")
add_executable(${EXECUTABLE_NAME} ${SRCS} ${HEADERS})

# Compiler Flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fbracket-depth=512") # Required for magic enum

# Specific OS Directories and Flags
if(WIN32)
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    # Windows (Visual Studio) Compiler Flags
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG  ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR}
    )

    # External Libs Compiler Flags
    set(SFML_STATIC_LIBRARIES TRUE)
    set_property(TARGET ${EXECUTABLE_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_property(TARGET clip PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # Set dependencies libs path
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-msvc/${CMAKE_BUILD_TYPE}")
    set(FreeType_LIB ${MODULES_DIR}/freetype.lib)
    set(OpenAL_LIB ${MODULES_DIR}/openal32.lib)
    set(Ogg_LIB ${MODULES_DIR}/ogg.lib)
    set(FLAC_LIB ${MODULES_DIR}/flac.lib)
    set(Vorbis_LIB ${MODULES_DIR}/vorbis.lib)
    set(VorbisEnc_LIB ${MODULES_DIR}/vorbisenc.lib)
    set(VorbisFile_LIB ${MODULES_DIR}/vorbisfile.lib)
elseif(APPLE)
    # Output Directory
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/osx/${CMAKE_BUILD_TYPE}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

    # Set dependencies libs path
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-osx")
    set(FreeType_LIB ${MODULES_DIR}/Framework/freetype.framework)
    set(OpenAL_LIB ${MODULES_DIR}/Framework/openal32.framework)
    set(Ogg_LIB ${MODULES_DIR}/Framework/ogg.framework)
    set(FLAC_LIB ${MODULES_DIR}/Framework/flac.framework)
    set(Vorbis_LIB ${MODULES_DIR}/Framework/vorbis.framework)
    set(VorbisEnc_LIB ${MODULES_DIR}/Framework/vorbisenc.framework)
    set(VorbisFile_LIB ${MODULES_DIR}/Framework/vorbisfile.framework)
else()
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
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

    # Set dependencies libs path
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-mingw")
    set(FreeType_LIB ${MODULES_DIR}/libfreetype.a)
    set(OpenAL_LIB ${MODULES_DIR}/libopenal32.a)
    set(Ogg_LIB ${MODULES_DIR}/libogg.a)
    set(FLAC_LIB ${MODULES_DIR}/libFLAC.a)
    set(Vorbis_LIB ${MODULES_DIR}/libvorbis.a)
    set(VorbisEnc_LIB ${MODULES_DIR}/libvorbisenc.a)
    set(VorbisFile_LIB ${MODULES_DIR}/libvorbisfile.a)

    # FileSystem library
    target_link_libraries(${EXECUTABLE_NAME} stdc++fs)
endif()

# Link Clip
target_link_libraries(${EXECUTABLE_NAME} clip)

# Link Magic Enum
find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} magic_enum::magic_enum)

# Link SFML
find_package(SFML 2.5.1 COMPONENTS audio graphics system window REQUIRED)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} sfml-audio sfml-graphics sfml-system sfml-window)
else()
    message(FATAL_ERROR "SFML not found")
endif()
