cmake_minimum_required(VERSION 3.15)
project(Morpheus VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SDL3
if(WIN32)
    # Windows: Use the downloaded SDL3 from libs directory
    set(SDL3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/include" CACHE PATH "SDL3 include directory")
    set(SDL3_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/lib/x64" CACHE PATH "SDL3 library directory")
    
    if(EXISTS "${SDL3_INCLUDE_DIR}/SDL3/SDL.h")
        message(STATUS "Found SDL3 in libs directory")
        add_library(SDL3::SDL3 SHARED IMPORTED)
        set_target_properties(SDL3::SDL3 PROPERTIES
            IMPORTED_LOCATION "${SDL3_LIBRARY_DIR}/SDL3.dll"
            IMPORTED_IMPLIB "${SDL3_LIBRARY_DIR}/SDL3.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIR};${SDL3_INCLUDE_DIR}/SDL3"
        )
    else()
        message(FATAL_ERROR "SDL3 not found in libs/SDL3. Please download SDL3-devel-*-VC.zip and extract to libs/SDL3")
    endif()
else()
    # Unix-like systems: Use find_package
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/libs")
    set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL3/cmake" CACHE PATH "Path to SDL3 CMake config")
    find_package(SDL3 3.4.2 REQUIRED CONFIG)
endif()

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

# Copy SDL3.dll to output directory on Windows
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_LIBRARY_DIR}/SDL3.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
