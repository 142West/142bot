cmake_minimum_required(VERSION 3.0)
project(142-bot VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20 REQUIRED)

include(FetchContent)
FetchContent_Declare(clog
    GIT_REPOSITORY https://git.devcara.com/clog
    GIT_TAG 0.1.0
)

FetchContent_Declare(dpp
    GIT_REPOSITORY https://github.com/brainboxdotcc/DPP
    GIT_TAG v10.0.23
)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 871ed52d350214a034f6ef8a3b8f51c5ce1bd400) # The commit hash for 1.9.0. Replace with the latest from: https://github.com/libcpr/cpr/releases

FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt
                        GIT_TAG 9.1.0)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.1.0
    )
    
FetchContent_MakeAvailable(clog dpp cpr fmt spdlog)

include(cmake/FindPCRE.cmake)

find_package(fmt)

string(ASCII 27 Esc)

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

include(GNUInstallDirs)
aux_source_directory("src" coresrc)
add_executable(142bot ${coresrc})

include_directories("include")
target_link_libraries(142bot PRIVATE clog dpp fmt::fmt spdlog::spdlog pcre
    cpr::cpr ${PQXX_LIB} ${PQ_LIB})
add_dependencies(142bot clog dpp cpr fmt spdlog)

install(TARGETS 142bot)

set (modules_dir "modules")
file(GLOB subdirlist ${modules_dir}/*)
foreach (fullmodname ${subdirlist})
    get_filename_component(modname ${fullmodname} NAME)
    message(STATUS "Found module '${Esc}[1;34mmodule_${modname}${Esc}[m'")
    set (modsrc "")
    aux_source_directory(${modules_dir}/${modname} modsrc)
    add_library(module_${modname} SHARED ${modsrc})
    add_dependencies(module_${modname} dpp cpr spdlog fmt)
    target_link_libraries(module_${modname} dpp cpr)
    install(TARGETS module_${modname} RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/142bot/")
    set_target_properties(module_${modname} PROPERTIES PREFIX "")
endforeach(fullmodname)
