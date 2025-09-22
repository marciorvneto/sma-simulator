cmake_minimum_required(VERSION 3.10)
project(simulation-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set debug build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  message("Setting build type to Debug")
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_subdirectory(core)
add_subdirectory(pulp-and-paper)
add_subdirectory(sandbox)
