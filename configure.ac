AC_INIT([142bot], [0.5])
AM_INIT_AUTOMAKE([-Wall -Werror foreign] subdir-objects)
AC_PROG_CXX
AM_PROG_AR

LT_INIT([dlopen])

AC_SEARCH_LIBS([_ZTVN3dpp3banE], [dpp], [], [AC_MSG_ERROR([unable to find dpp])])
AC_SEARCH_LIBS([sentry_init], [sentry], [], [AC_MSG_ERROR([unable to find sentry])])
AC_SEARCH_LIBS([_ZN3cpr8ResponseC1EOS0_], [cpr], [], [AC_MSG_ERROR([unable to find cpr])])
AC_SEARCH_LIBS([curl_global_init], [curl], [], [AC_MSG_ERROR([unable to find curl])])
AC_SEARCH_LIBS([_ZTVN6spdlog6loggerE], [spdlog], [], [AC_MSG_ERROR([unable to find spdlog])])
AC_SEARCH_LIBS([_ZN4pqxx10connectingC1ENS_5zviewE], [pqxx], [], [AC_MSG_ERROR([unable to find pqxx])])
AC_SEARCH_LIBS([PQexec], [pq], [], [AC_MSG_ERROR([unable to find pq])])
AC_SEARCH_LIBS([pcre_exec], [pcre], [], [AC_MSG_ERROR([unable to find pcre])])
AC_SEARCH_LIBS([_ZN3fmt2v94file3dupEi], [fmt], [], [AC_MSG_ERROR([unable to find fmt])])

AC_CONFIG_HEADERS([src/142bot_config.h])

AC_CONFIG_FILES([
                 Makefile
                 modules/Makefile])
AC_OUTPUT
