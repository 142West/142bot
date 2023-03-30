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
    
FetchContent_MakeAvailable(clog dpp)

include(cmake/FindPCRE.cmake)

find_package(fmt)

string(ASCII 27 Esc)

aux_source_directory("src" coresrc)
add_executable(142bot ${coresrc})

include_directories("include")
target_link_libraries(142bot clog dpp fmt spdlog pcre)

set (modules_dir "modules")
file(GLOB subdirlist ${modules_dir}/*)
foreach (fullmodname ${subdirlist})
    get_filename_component(modname ${fullmodname} NAME)
    message(STATUS "Found module '${Esc}[1;34mmodule_${modname}${Esc}[m'")
    set (modsrc "")
    aux_source_directory(${modules_dir}/${modname} modsrc)
    add_library(module_${modname} SHARED ${modsrc})
    set_target_properties(module_${modname} PROPERTIES PREFIX "")
endforeach(fullmodname)
