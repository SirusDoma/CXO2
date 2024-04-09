# CMake Config
cmake_minimum_required(VERSION 3.27)
project(O2Jam LANGUAGES C CXX OBJCXX)

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
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
include(FetchContent)
# -- SFML
fetchcontent_declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG feature/miniaudio
)
fetchcontent_makeavailable(SFML)
# -- Clip Flags
set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples")
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests")
fetchcontent_declare(
    clip
    GIT_REPOSITORY https://github.com/dacap/clip.git
    GIT_TAG main
)
fetchcontent_makeavailable(clip)
# -- Boxer Flags
set(BOXER_BUILD_EXAMPLES OFF CACHE BOOL "Compile boxer examples")
fetchcontent_declare(
    Boxer
    GIT_REPOSITORY https://github.com/aaronmjacobs/Boxer.git
    GIT_TAG master
)
fetchcontent_makeavailable(Boxer)

# Default flags
if (NOT CMAKE_BUILD_TYPE)
    message("[Warning] CMAKE_BUILD_TYPE is not set. Falling back to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})

# Aliases
set(EXECUTABLE_NAME "OTwo")

# Specific OS Directories and Flags
if(WIN32)
    # App Resources (Icon)
    set(APP_RESOURCE ${CMAKE_SOURCE_DIR}/resources/OTwo.o)
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

    message("Architecture: " ${CMAKE_OSX_ARCHITECTURES})

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
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS  "${RESOURCES_DIR}/OTwo.entitlements.plist"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # Set dependencies libs
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework CoreAudio")
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework AudioUnit")
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework Security")
else()
    # Executable
    add_executable(${EXECUTABLE_NAME} ${SRCS} ${HEADERS})

    # Compiler Flags
    # TODO: Not to assume the compiler to be GCC/G++
    if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 8)
        message("FileSystem is not supported with your compiler version.")
        message("Compiler Version: " ${CMAKE_CXX_COMPILER_VERSION})
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
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIR}
    )

    # Set dependencies libs
    set(MODULES_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib-gcc")

    # Libraries
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE stdc++fs)
endif()

message("Output Directory: " ${OUTPUT_DIR})
message("")

# Link JSON
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Link Clip
include_directories(${FETCHCONTENT_BASE_DIR}/clip-src)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE clip)

# Link Boxer
# TODO: Fork Boxer and migrate Gtk to X11
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Boxer)

# Link Magic Enum
find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE magic_enum::magic_enum)

# Link SFML
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-audio sfml-graphics sfml-system sfml-window)
if (WIN32)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-main)
endif()
