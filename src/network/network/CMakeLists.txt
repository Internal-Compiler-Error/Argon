add_library(network STATIC "")
target_sources(network
        PUBLIC
        network.hpp
        PRIVATE
        network.cpp)
target_include_directories(network PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(network PRIVATE pthread)
target_link_libraries(network PRIVATE OpenSSL::SSL)