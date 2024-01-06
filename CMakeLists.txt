cmake_minimum_required(VERSION 3.10)
project(cupot C)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_executable(cupot ${SOURCES})

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# target_include_directories(my_c_executable PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)