cmake_minimum_required(VERSION 3.13)

include("cmake-scripts/HunterGate.cmake")
huntergate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.165.tar.gz"
        SHA1 "5a73f91df5f6109c0bb1104d0c0ee423f7bece79"
)

project(Argon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(external)

enable_testing()
add_subdirectory(test)
