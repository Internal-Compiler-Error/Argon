add_executable(network_test network_test.cpp)
target_link_libraries(network_test PRIVATE GTest::GTest)
target_link_libraries(network_test PRIVATE network)

add_test(NAME google_test
        COMMAND $<TARGET_FILE:network_test>)