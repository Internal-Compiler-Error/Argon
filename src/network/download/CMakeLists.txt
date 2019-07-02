add_library(argon_download "")
target_sources(argon_download
        PRIVATE
        download.cpp
        PUBLIC
        download.hpp
        )
target_link_libraries(argon_download PRIVATE
        nlohmann_json::nlohmann_json
        #        network-uri
        )
target_include_directories(argon_download
        PUBLIC
        .
        )
