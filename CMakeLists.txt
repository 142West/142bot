cmake_minimum_required(VERSION 3.0)
project(142-bot VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11 REQUIRED)

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

add_subdirectory(src)
