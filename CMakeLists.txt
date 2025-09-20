cmake_minimum_required(VERSION 3.10)
project(simulation-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(core)
add_subdirectory(pulp-and-paper)
add_subdirectory(sandbox)
