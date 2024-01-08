cmake_minimum_required(VERSION 3.26)
string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
project(${CURRENT_FOLDER} LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# file
file(GLOB_RECURSE CPP_FILES "src/*.cpp")

# include
include_directories("D:/Code/Eigen/eigen-3.4.0")
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the executable
add_executable(${CURRENT_FOLDER} ${CPP_FILES})

# set executable path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)