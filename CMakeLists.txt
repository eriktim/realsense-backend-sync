cmake_minimum_required (VERSION 3.1.0)

project (backendSync)

find_package (Threads REQUIRED)

if(WIN32)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x64")
        set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x64")
    else()
        set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x86")
        set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x86")
    endif()
    set(PROJECT_BINARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/\$\(Configuration\)")
    set(ADDITIONAL_INCLUDE_DIRS "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\include")
endif()

find_library(REALSENSE2_FOUND realsense2 HINTS ${LIBRARY_DIR} REQUIRED)
if(NOT REALSENSE2_FOUND)
    SET(REALSENSE2_FOUND "realsense2")
    message(WARN "Failed to find_library(realsense2)")
endif()

add_executable(backendSync main.cpp)

target_link_libraries (backendSync ${CMAKE_THREAD_LIBS_INIT})

include_directories(include ${realsense_INCLUDE_DIR})
include_directories(backendSync ${ADDITIONAL_INCLUDE_DIRS})
target_link_libraries(backendSync ${REALSENSE2_FOUND})
target_link_libraries(backendSync ${realsense2_LIBRARY})

if(WIN32)
    message(STATUS "Adding Post build script to copy realsense2.dll to project's binary folder")
    message(STATUS "Will try to copy from ${DLL_DIR} to ${PROJECT_BINARY_OUTPUT_PATH}")
    add_custom_command(TARGET backendSync POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_DIR}/realsense2.dll"
            ${PROJECT_BINARY_OUTPUT_PATH})
endif()
