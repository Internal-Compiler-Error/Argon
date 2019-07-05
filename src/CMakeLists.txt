add_subdirectory(gui)
add_subdirectory(network)
add_executable(Argon main.cpp)

target_link_libraries(Argon PRIVATE argon_gui)
