cmake_minimum_required(VERSION 3.10)

add_executable(sandbox
  src/main.cpp
)

target_link_libraries(sandbox
  PRIVATE core
  PRIVATE pnp
)

