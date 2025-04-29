cmake_minimum_required(VERSION 3.14)
project(BankSimulation CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 收集所有源文件
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# 生成可执行文件
add_executable(bank_sim ${SOURCES})
