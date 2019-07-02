add_subdirectory(download)
add_subdirectory(download_scheduler)

add_library(network STATIC "")
target_sources(network
        PUBLIC
        network.hpp
        PRIVATE
        network.cpp)
target_link_libraries(network Beast::Beast)