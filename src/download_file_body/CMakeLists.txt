# THIS MODULE IS NOW DEPRECARED
# IT NEVER WORKED IN THE FIRST PLACE ANYWAY
# The build logic is preserved for now
add_library(download_file_body "")
target_sources(download_file_body
        PRIVATE
        download_file_body.cpp
        PUBLIC
        download_file_body.hpp)
set_target_properties(download_file_body PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(download_file_body PUBLIC boost_beast)
target_include_directories(download_file_body INTERFACE ${CMAKE_CURRENT_LIST_DIR})