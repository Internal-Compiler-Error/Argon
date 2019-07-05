find_package(OpenSSL REQUIRED)

if (OpenSSL_FOUND)
    set_target_properties(OpenSSL::SSL PROPERTIES IMPORTED_GLOBAL TRUE)
    set_target_properties(OpenSSL::Crypto PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()