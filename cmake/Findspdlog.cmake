include(FindPackageHandleStandardArgs)

find_library(SPDLOG_LIBRARY NAMES spdlog)

find_package_handle_standard_args(spdlog REQUIRED_VARS SPDLOG_LIBRARY)

if (SPDLOG_FOUND)
    mark_as_advanced(SPDLOG_LIBRARY)
endif()

if(SPDLOG_FOUND AND NOT TARGET spdlog::spdlog)
    add_library(spdlog::spdlog IMPORTED UNKNOWN)
    set_property(TARGET spdlog::spdlog PROPERTY IMPORTED_LOCATION ${SPDLOG_LIBRARY})
endif()