include(FindPackageHandleStandardArgs)

find_library(DPP_LIBRARY NAMES dpp)

find_package_handle_standard_args(dpp REQUIRED_VARS DPP_LIBRARY)

if (DPP_FOUND)
    mark_as_advanced(DPP_LIBRARY)
endif()

if(DPP_FOUND AND NOT TARGET dpp::dpp)
    add_library(dpp::dpp IMPORTED UNKNOWN)
    set_property(TARGET dpp::dpp PROPERTY IMPORTED_LOCATION ${DPP_LIBRARY})
endif()