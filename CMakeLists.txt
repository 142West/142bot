cmake_minimum_required(VERSION 3.0)
project(onefortytwobot VERSION 0.4 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20 REQUIRED)
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE OPT_IN)

find_package(spdlog)
find_package(dpp)
#find_package(cpr REQUIRED)

include(FetchContent)
FetchContent_Declare(clog
    GIT_REPOSITORY https://git.devcara.com/clog
    GIT_TAG 0.1.0
)

FetchContent_Declare(dpp
    GIT_REPOSITORY https://github.com/brainboxdotcc/DPP
    GIT_TAG v10.0.23
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400
                         FIND_PACKAGE_ARGS) # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases

FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt
                        GIT_TAG 9.1.0)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.1.0
    FIND_PACKAGE_ARGS NAMES spdlog
    )

FetchContent_Declare(sentry
    GIT_REPOSITORY https://github.com/getsentry/sentry-native
    GIT_TAG 0.6.1
)
    
FetchContent_MakeAvailable(clog dpp cpr fmt spdlog sentry)

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
    cpr::cpr ${PQXX_LIB} ${PQ_LIB} sentry)
add_dependencies(142bot clog cpr fmt)

install(TARGETS 142bot)

set (modules_dir "modules")
file(GLOB subdirlist ${modules_dir}/*)
foreach (fullmodname ${subdirlist})
    get_filename_component(modname ${fullmodname} NAME)
    message(STATUS "Found module '${Esc}[1;34mmodule_${modname}${Esc}[m'")
    set (modsrc "")
    aux_source_directory(${modules_dir}/${modname} modsrc)
    add_library(module_${modname} SHARED ${modsrc})
    add_dependencies(module_${modname} cpr fmt)
    target_link_libraries(module_${modname} dpp cpr sentry)
    set_target_properties(module_${modname} PROPERTIES PREFIX "")
endforeach(fullmodname)
