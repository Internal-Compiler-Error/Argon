set(BOOST_VERSION 1.70.0)

# Settings for finding the correct Boost libraries
set(Boost_USE_STATIC_LIBS FLASE)
set(Boost_USE_MULTITHREADED TRUE)
set(Boost_USE_STATIC_RUNTIME FALSE)
set(Boost_ADDITIONAL_VERSIONS ${BOOST_VERSION})


# Search for Boost Libraries
find_package(Boost 1.70 REQUIRED COMPONENTS system)

# Make Boost globally available
if (Boost_FOUND)
    set_target_properties(Boost::system PROPERTIES IMPORTED_GLOBAL TRUE)
endif ()