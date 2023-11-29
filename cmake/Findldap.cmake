include(FindPackageHandleStandardArgs)

find_library(LDAP_LIBRARY NAMES ldap)

find_package_handle_standard_args(ldap REQUIRED_VARS LDAP_LIBRARY)

if (LDAP_FOUND)
    mark_as_advanced(LDAP_LIBRARY)
endif()

if(LDAP_FOUND AND NOT TARGET ldap::ldap)
    add_library(ldap::ldap IMPORTED UNKNOWN)
    set_property(TARGET ldap::ldap PROPERTY IMPORTED_LOCATION ${LDAP_LIBRARY})
endif()