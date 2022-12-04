cmake_minimum_required(VERSION 3.16)
project(DiffMerge3)

# Options
#option(DIFFMERGE_GENERATE_HEADER_ONLY "Generate header only version?" FALSE)
#option(DIFFMERGE_GENERATE_C_API "Generate header with C compliant API format?" FALSE)
option(DIFFMERGE_BUILD_TESTS "Build tests?" FALSE)
option(DIFFMERGE_BUILD_SHARED_LIBRARY "Build as shared library?" FALSE)

# Settings
set(CMAKE_CXX_STANDARD 17)

# Includes
include(GenerateExportHeader)

# Internal variables
if (WIN32)
    set(WINDOWS TRUE)
    set(PLATFORM_NAME Windows)
    set(PLATFORM_EXE_EXTENSION .exe)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX TRUE)
    set(PLATFORM_NAME Linux)
    set(PLATFORM_EXE_EXTENSION "")
endif()

# DiffMerge3 Library
file(GLOB LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
if (DIFFMERGE_BUILD_SHARED_LIBRARY)
    add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
else()
    add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE "-D_CRT_SECURE_NO_WARNINGS")
set(ADDITIONAL_EXPORT_CONTENT "
#ifndef DIFFMERGE_INLINE
    #define DIFFMERGE_INLINE inline
#endif
")
generate_export_header(${PROJECT_NAME}
    EXPORT_FILE_NAME ${CMAKE_SOURCE_DIR}/include/DiffMerge3/DiffMergeExport.h
    CUSTOM_CONTENT_FROM_VARIABLE ADDITIONAL_EXPORT_CONTENT
)

# Tests
if (DIFFMERGE_BUILD_TESTS)
    add_executable(${PROJECT_NAME}_test tests/main.cpp)
    target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME})
    target_compile_definitions(${PROJECT_NAME}_test PRIVATE "-D_CRT_SECURE_NO_WARNINGS")
endif()
