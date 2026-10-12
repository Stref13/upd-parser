cmake_minimum_required(VERSION 3.24)
project(upd_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

FetchContent_Declare(pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG        v1.14
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(pugixml)

add_executable(upd_parser src/main.cpp src/upd_pars.cpp)
target_link_libraries(upd_parser PRIVATE pugixml::pugixml)