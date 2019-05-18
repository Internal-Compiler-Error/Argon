hunter_add_package(CppNetlibUri)
find_package(CppNetlibUri CONFIG REQUIRED)

if (network-uri_FOUND)
    set_target_properties(network-uri PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()

