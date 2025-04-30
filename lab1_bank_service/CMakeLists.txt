# set the minimum required version of CMake
cmake_minimum_required(VERSION 3.14)

# set the project name as a variable
set(PROJECT_NAME bank_service)
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

# find and link the Threads library
find_package(Threads REQUIRED)

# set the output directory for the compiled binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# define the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# link external libraries to the target
target_link_libraries(${PROJECT_NAME} Threads::Threads)

# optional: install the executable
# install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin)

# optional: enable testing
# enable_testing()
# add_subdirectory(tests)
