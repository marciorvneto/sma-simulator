cmake_minimum_required(VERSION 3.10)

add_subdirectory(vendor/CoolProp)

add_library(pnp
  src/BlackLiquor.cpp
  src/Evaporator.cpp
)

target_include_directories(pnp
  PUBLIC include
)

target_link_libraries(pnp
  PRIVATE core
  PRIVATE CoolProp
)

