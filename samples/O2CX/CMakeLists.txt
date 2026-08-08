add_library(O2CX SHARED
    O2CX.cpp
    Ui.cpp
    Extensions/AccuracyMeterExtension.cpp
    Extensions/ChatExtension.cpp
    Extensions/MusicSearchExtension.cpp
    Extensions/TetrisExtension.cpp
    Extensions/WelcomeExtension.cpp
)

target_include_directories(O2CX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(O2CX PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(O2CX PRIVATE Genode)
target_link_libraries(O2CX PRIVATE magic_enum::magic_enum)
target_link_libraries(O2CX PRIVATE ced)

if(WIN32)
    set(O2CX_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/windows/${CMAKE_BUILD_TYPE}/extensions")
elseif(APPLE)
    set(O2CX_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/osx/${CMAKE_OSX_ARCHITECTURES}/${CMAKE_BUILD_TYPE}/extensions")
else()
    set(O2CX_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/linux/${CMAKE_BUILD_TYPE}/extensions")
endif()

file(MAKE_DIRECTORY ${O2CX_OUTPUT_DIR})
set_target_properties(O2CX PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${O2CX_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${O2CX_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${O2CX_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${O2CX_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${O2CX_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${O2CX_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${O2CX_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${O2CX_OUTPUT_DIR}
)
