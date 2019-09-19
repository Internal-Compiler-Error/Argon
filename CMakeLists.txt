cmake_minimum_required(VERSION 3.13)

project(Argon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(external)

enable_testing()
add_subdirectory(test)
