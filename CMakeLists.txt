cmake_minimum_required(VERSION 3.26)
string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
project(${CURRENT_FOLDER} LANGUAGES CXX)

FIND_PACKAGE(OpenMP REQUIRED)

if(OpenMP_FOUND)
    message(STATUS "found openmp")
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS})
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS})
else()
    message(FATAL_ERROR "openmp not found!")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# file
file(GLOB_RECURSE CPP_FILES "src/*.cpp")

# include
include_directories(${PROJECT_SOURCE_DIR}/deps/Eigen/eigen-3.4.0)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the executable
add_executable(${CURRENT_FOLDER} ${CPP_FILES})

# set executable path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)