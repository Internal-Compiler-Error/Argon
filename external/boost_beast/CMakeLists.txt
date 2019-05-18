hunter_add_package(Beast)
find_package(Beast CONFIG REQUIRED)
hunter_add_package(OpenSSL)
find_package(OpenSSL REQUIRED)

if (Beast_FOUND)
    target_link_libraries(Beast::Beast INTERFACE OpenSSL::SSL OpenSSL::Crypto)
    set_target_properties(Beast::Beast PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()