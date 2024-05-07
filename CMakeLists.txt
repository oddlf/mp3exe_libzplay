cmake_minimum_required(VERSION 3.6)

project(mp3exe
    DESCRIPTION "Mp3exe audio player based on libZPlay library"
    LANGUAGES CXX
)

option(MP3EXE_STATIC_CRT "Use static CRT library"       FALSE)
option(MP3EXE_PACK_WITH_UPX "Pack program with UPX"     FALSE)

set(MP3EXE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(MP3EXE_SRC ${MP3EXE_ROOT}/src)

set(MP3EXE_SOURCE_FILES
    ${MP3EXE_SRC}/Main.cpp
    ${MP3EXE_SRC}/Main.h
    ${MP3EXE_SRC}/MainForm.cpp
    ${MP3EXE_SRC}/MainForm.h
    ${MP3EXE_SRC}/resource.h
    ${MP3EXE_SRC}/resource.rc
    ${MP3EXE_SRC}/WApp.cpp
    ${MP3EXE_SRC}/WApp.h
    ${MP3EXE_SRC}/WBmpButton.cpp
    ${MP3EXE_SRC}/WBmpButton.h
    ${MP3EXE_SRC}/WBmpFont.cpp
    ${MP3EXE_SRC}/WBmpFont.h
    ${MP3EXE_SRC}/WBmpTextBox.cpp
    ${MP3EXE_SRC}/WBmpTextBox.h
    ${MP3EXE_SRC}/WControl.cpp
    ${MP3EXE_SRC}/WControl.h
    ${MP3EXE_SRC}/WFile.cpp
    ${MP3EXE_SRC}/WFile.h
    ${MP3EXE_SRC}/WFont.cpp
    ${MP3EXE_SRC}/WFont.h
    ${MP3EXE_SRC}/WForm.cpp
    ${MP3EXE_SRC}/WForm.h
    ${MP3EXE_SRC}/WIniKey.cpp
    ${MP3EXE_SRC}/WIniKey.h
    ${MP3EXE_SRC}/WMem.cpp
    ${MP3EXE_SRC}/WMem.h
    ${MP3EXE_SRC}/WMixer.cpp
    ${MP3EXE_SRC}/WMixer.h
    ${MP3EXE_SRC}/WOpenDialog.cpp
    ${MP3EXE_SRC}/WOpenDialog.h
    ${MP3EXE_SRC}/WSaveDialog.cpp
    ${MP3EXE_SRC}/WSaveDialog.h
    ${MP3EXE_SRC}/WScrollbar.cpp
    ${MP3EXE_SRC}/WScrollbar.h
    ${MP3EXE_SRC}/WTooltip.cpp
    ${MP3EXE_SRC}/WTooltip.h
    ${MP3EXE_SRC}/WVUMeter.cpp
    ${MP3EXE_SRC}/WVUMeter.h
)

find_package(libzplay REQUIRED)

add_executable(${PROJECT_NAME} WIN32 ${MP3EXE_SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_include_directories(${PROJECT_NAME} PRIVATE ${MP3EXE_SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE comctl32 winmm libzplay::libzplay)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wpedantic -O3)
    if(WIN32)
        target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
    endif()
endif()

if(MP3EXE_STATIC_CRT AND WIN32)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

if(MP3EXE_PACK_WITH_UPX)
    include(FindSelfPackers)
    if(SELF_PACKER_FOR_EXECUTABLE)
        ADD_CUSTOM_COMMAND(
            COMMAND ${SELF_PACKER_FOR_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
            ARGS ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} -9q
            TARGET ${PROJECT_NAME}
        )
    endif()
endif()
