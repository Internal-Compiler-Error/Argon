find_package(GTest MODULE REQUIRED)

if (GTest_FOUND)
    set_target_properties(GTest::Main PROPERTIES IMPORTED_GLOBAL TRUE)
    set_target_properties(GTest::GTest PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()
