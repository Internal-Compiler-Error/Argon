find_package(nlohmann_json CONFIG REQUIRED)

if (nlohmann_json_FOUND)
    set_target_properties(nlohmann_json::nlohmann_json PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()