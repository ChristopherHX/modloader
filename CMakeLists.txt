cmake_minimum_required (VERSION 2.6)

project(modloader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_library(log-lib log)

add_library(modloader SHARED src/main.cpp)
target_include_directories(modloader PUBLIC include)
target_link_libraries(modloader PUBLIC ${log-lib})
