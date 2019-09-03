# CMake Config
cmake_minimum_required(VERSION 3.14)
project(O2Jam)

# Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specific OS Directories and Flags
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin")
if(WIN32)
    # Windows Compiler Flags
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

    # Set output and module directory for windows
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}")
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-msvc")

    # Set dependencies libs path
    set(FreeType_LIB ${MODULES_DIR}/freetype.lib)
    set(OpenAL_LIB ${MODULES_DIR}/openal32.lib)
    set(Ogg_LIB ${MODULES_DIR}/ogg.lib)
    set(FLAC_LIB ${MODULES_DIR}/flac.lib)
    set(Vorbis_LIB ${MODULES_DIR}/vorbis.lib)
    set(VorbisEnc_LIB ${MODULES_DIR}/vorbisenc.lib)
    set(VorbisFile_LIB ${MODULES_DIR}/vorbisfile.lib)
elseif(APPLE)
    # Set output and module directory for osx
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/osx/${CMAKE_BUILD_TYPE}")
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-osx")

    # Set dependencies libs path
    set(FreeType_LIB ${MODULES_DIR}/Framework/freetype.framework)
    set(OpenAL_LIB ${MODULES_DIR}/Framework/openal32.framework)
    set(Ogg_LIB ${MODULES_DIR}/Framework/ogg.framework)
    set(FLAC_LIB ${MODULES_DIR}/Framework/flac.framework)
    set(Vorbis_LIB ${MODULES_DIR}/Framework/vorbis.framework)
    set(VorbisEnc_LIB ${MODULES_DIR}/Framework/vorbisenc.framework)
    set(VorbisFile_LIB ${MODULES_DIR}/Framework/vorbisfile.framework)
else()
    # Compiler Flags
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_C_FLAGS -m32)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_FLAGS -m32)

    # Set output and module directory for unix
    set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/linux/${CMAKE_BUILD_TYPE}")
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-mingw")

    # Set dependencies libs path
    set(FreeType_LIB ${MODULES_DIR}/libfreetype.a)
    set(OpenAL_LIB ${MODULES_DIR}/libopenal32.a)
    set(Ogg_LIB ${MODULES_DIR}/libogg.a)
    set(FLAC_LIB ${MODULES_DIR}/libFLAC.a)
    set(Vorbis_LIB ${MODULES_DIR}/libvorbis.a)
    set(VorbisEnc_LIB ${MODULES_DIR}/libvorbisenc.a)
    set(VorbisFile_LIB ${MODULES_DIR}/libvorbisfile.a)
endif()

# Output Directory
file(MAKE_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Sources
file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)
file(GLOB_RECURSE HEADERS include/*.hpp)
include_directories(include)
include_directories(src)

# Executable
set(EXECUTABLE_NAME "OTwo")
add_executable(${EXECUTABLE_NAME} ${SRCS} ${HEADERS})

# SFML Flags (TODO: Should have no dependencies with ENV)
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "${SFML_ROOT}/cmake/SFML")
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include ")

# Link SFML
find_package(SFML 2.5 COMPONENTS main audio graphics system window REQUIRED)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} sfml-audio sfml-graphics sfml-system sfml-window)
else()
    message(FATAL_ERROR "SFML not found")
endif()
