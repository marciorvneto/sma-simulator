cmake_minimum_required(VERSION 3.10)

set(IF97_HEADER_MODULE ON CACHE BOOL "Build IF97 header library" FORCE)
add_subdirectory(vendor/IF97)

get_property(targets DIRECTORY vendor/IF97 PROPERTY BUILDSYSTEM_TARGETS)
message("Available IF97 targets: ${targets}")

add_library(pnp
  src/BlackLiquor.cpp
  src/Evaporator.cpp
  src/PulpAndPaperCalculationSettings.cpp
)

target_include_directories(pnp
  PUBLIC include
)

target_link_libraries(pnp
  PRIVATE core
  PRIVATE IF97
)

