include(FindPackageHandleStandardArgs)

find_library(SENTRY_LIBRARY NAMES sentry)

find_package_handle_standard_args(sentry REQUIRED_VARS SENTRY_LIBRARY)

if (SENTRY_FOUND)
    mark_as_advanced(SENTRY_LIBRARY)
endif()

if(SENTRY_FOUND AND NOT TARGET sentry::sentry)
    add_library(sentry::sentry IMPORTED UNKNOWN)
    set_property(TARGET sentry::sentry PROPERTY IMPORTED_LOCATION ${SENTRY_LIBRARY})
endif()