find_package(PkgConfig)
pkg_search_module(GTKMM REQUIRED gtkmm-3.0 IMPORTED_TARGET)

if (GTKMM_FOUND)
    set_target_properties(PkgConfig::GTKMM PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()

