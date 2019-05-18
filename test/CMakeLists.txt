add_executable(cpp_test "")
target_sources(cpp_test
               PRIVATE
               gtest.cpp
        ../src/network/download/download.cpp
        ../src/network/download/download.hpp
               )

target_link_libraries(cpp_test PUBLIC gtest)
target_link_libraries(cpp_test PUBLIC pthread)
target_link_libraries(cpp_test PUBLIC nlohmann_json::nlohmann_json)
add_test(
        NAME google_test
        COMMAND $<TARGET_FILE:cpp_test>
)
