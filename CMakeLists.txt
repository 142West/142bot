cmake_minimum_required(VERSION 3.5)
project(onefortytwobot VERSION 0.6 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20 REQUIRED)
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE OPT_IN)

list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake")

add_compile_definitions(LDAP_DEPRECATED)

find_package(spdlog REQUIRED)
find_package(dpp REQUIRED)
find_package(cpr REQUIRED)
find_package(sentry REQUIRED)
find_package(curl)
find_package(ldap REQUIRED)

include(cmake/FindPCRE.cmake)

find_package(fmt)

string(ASCII 27 Esc)

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

# Define configuration file
configure_file(142bot.config.in 142bot_config.h)

include(GNUInstallDirs)
aux_source_directory("src" coresrc)
add_executable(142bot ${coresrc})

include_directories("include" ${PROJECT_BINARY_DIR})
target_link_libraries(142bot PRIVATE clog dpp fmt::fmt spdlog::spdlog pcre
    cpr::cpr ${PQXX_LIB} ${PQ_LIB} sentry ldap::ldap)

install(TARGETS 142bot)

set (modules_dir "modules")
file(GLOB subdirlist ${modules_dir}/*)
foreach (fullmodname ${subdirlist})
    get_filename_component(modname ${fullmodname} NAME)
    message(STATUS "Found module '${Esc}[1;34mmodule_${modname}${Esc}[m'")
    set (modsrc "")
    aux_source_directory(${modules_dir}/${modname} modsrc)
    add_library(module_${modname} SHARED ${modsrc})
    target_link_libraries(module_${modname} dpp cpr sentry)
    set_target_properties(module_${modname} PROPERTIES PREFIX "")
endforeach(fullmodname)
