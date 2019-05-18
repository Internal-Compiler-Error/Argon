hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

if (GTest_FOUND)
    set_target_properties(GTest::main PROPERTIES IMPORTED_GLOBAL TRUE)
    set_target_properties(GTest::gtest PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()
