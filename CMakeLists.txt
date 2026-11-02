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

FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.12
    GIT_SHALLOW    TRUE)

FetchContent_MakeAvailable(pugixml doctest)

add_library(upd_core src/money.cpp src/upd_pars.cpp)

target_include_directories(upd_core PUBLIC src)
target_link_libraries(upd_core PUBLIC pugixml::pugixml)

add_executable(upd_parser src/main.cpp)
target_link_libraries(upd_parser PRIVATE upd_core)

enable_testing()
add_executable(upd_tests tests/test_money.cpp)
target_link_libraries(upd_tests PRIVATE upd_core doctest::doctest)
add_test(NAME upd_tests COMMAND upd_tests)