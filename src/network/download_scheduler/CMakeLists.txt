add_library(argon_download_scheduler STATIC "")
target_sources(argon_download_scheduler
        PRIVATE
        download_scheduler.cpp
        PUBLIC
        download_scheduler.hpp
        )

target_link_libraries(argon_download_scheduler
        PRIVATE
        Beast::Beast
        Boost::system
        argon_download
        )
target_include_directories(argon_download_scheduler
        PRIVATE
        argon_download)

target_include_directories(argon_download_scheduler
        PUBLIC
        ${BOOST_ROOT}
        )