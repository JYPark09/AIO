# CMake version
cmake_minimum_required(VERSION 3.8)

# Delcare project
project(AIO)

# C++ standard
set(CMAKE_CXX_STANDARD 17)

# Options
option(BUILD_WITH_TRT "Build with TensorRT" OFF)

# Set output directories
set(DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# 3rd parties
include_directories(3rdparty/json/include)
include_directories(3rdparty/random/include)
include_directories(3rdparty/spdlog/include)

find_package(Threads REQUIRED)

if (BUILD_WITH_TRT)
    include(CMake/FindTensorRT.cmake)

    if (TensorRT_FOUND)
        message("use TensorRT")
        add_compile_definitions(BUILD_WITH_TRT)

        find_package(CUDA REQUIRED)
    endif()
endif()

# Include directories
include_directories(Includes)

# Project module
add_subdirectory(Sources/AIO)

add_subdirectory(Extensions/NetTest)
add_subdirectory(Extensions/Play)
add_subdirectory(Extensions/Selfplay)
