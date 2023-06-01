include(FindPackageHandleStandardArgs)

find_library(CPR_LIBRARY NAMES cpr)

find_package_handle_standard_args(cpr REQUIRED_VARS CPR_LIBRARY)

if (CPR_FOUND)
    mark_as_advanced(CPR_LIBRARY)
endif()

if(CPR_FOUND AND NOT TARGET cpr::cpr)
    add_library(cpr::cpr IMPORTED UNKNOWN)
    set_property(TARGET cpr::cpr PROPERTY IMPORTED_LOCATION ${CPR_LIBRARY})
endif()