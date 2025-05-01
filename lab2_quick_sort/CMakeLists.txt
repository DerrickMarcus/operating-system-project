# set the minimum required version of CMake
cmake_minimum_required(VERSION 3.14)

# set the project name as a variable
set(PROJECT_NAME quick_sort)
project(${PROJECT_NAME})

# set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# include header files from the include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# find all source files in the src directory
file(GLOB SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
