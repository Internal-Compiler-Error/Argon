#add_subdirectory(download)
#add_subdirectory(download_scheduler)
#add_subdirectory(network)

add_library(network STATIC "")
target_sources(network
        PUBLIC
        download.hpp
        download_scheduler.hpp
        network.hpp
        download_handler.hpp
        PRIVATE
        download.cpp
        download_scheduler.cpp
        network.cpp
        download_handler.cpp
        )
target_include_directories(network INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(network PUBLIC boost_beast)

target_compile_options(network
        PRIVATE
        -Wall
        # -Werror # treat all warnings as errors
        -Wextra # reasonable and standard
        -Wshadow # warn the user if a variable declaration shadows one from a parent context
        -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps
        # catch hard to track down memory errors
        -Wold-style-cast # warn for c-style casts
        -Wcast-align # warn for potential performance problem casts
        -Wunused # warn on anything being unused
        -Woverloaded-virtual # warn if you overload (not override) a virtual function
        -Wpedantic # warn if non-standard C++ is used
        -Wconversion # warn on type conversions that may lose data
        -Wsign-conversion # warn on sign conversions
        -Wnull-dereference # warn if a null dereference is detected
        -Wdouble-promotion # warn if float is implicit promoted to double
        -Wformat=2 # warn on security issues around functions that format output (ie printf)
        )
