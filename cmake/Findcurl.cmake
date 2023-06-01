include(FindPackageHandleStandardArgs)

find_library(CURL_LIBRARY NAMES curl)

find_package_handle_standard_args(curl REQUIRED_VARS CURL_LIBRARY)

if (CURL_FOUND)
    mark_as_advanced(CURL_LIBRARY)
endif()

if(CURL_FOUND AND NOT TARGET curl::curl)
    add_library(CURL::libcurl IMPORTED UNKNOWN)
    set_property(TARGET CURL::libcurl PROPERTY IMPORTED_LOCATION ${CURL_LIBRARY})
endif()