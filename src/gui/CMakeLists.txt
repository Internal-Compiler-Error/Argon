add_library(argon_gui STATIC "")

target_sources(argon_gui
        PRIVATE
        argon_app/argon_app.cpp
        download_columns/download_columns.cpp
        download_file_chooser/download_file_chooser.cpp
        download_file_input/download_link_input.cpp
        download_not_supported_dialog/download_not_supported_dialog.cpp
        PUBLIC
        argon_app/argon_app.hpp
        download_columns/download_columns.hpp
        download_file_chooser/download_file_chooser.hpp
        download_file_input/download_link_input.hpp
        download_not_supported_dialog/download_not_supported_dialog.hpp)

target_include_directories(argon_gui PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(argon_gui PkgConfig::GTKMM)
target_include_directories(argon_gui PUBLIC PkgConfig::GTKMM)

target_link_libraries(argon_gui network)
target_link_libraries(argon_gui argon_download)
target_link_libraries(argon_gui argon_download_scheduler)

target_compile_options(argon_gui
        INTERFACE
        -Wall
        #        -Werror # treat all warnings as errors
        -Wextra # reasonable and standard
        -Wshadow # warn the user if a variable declaration shadows one from a parent context
        -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
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