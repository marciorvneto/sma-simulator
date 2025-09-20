cmake_minimum_required(VERSION 3.10)

add_library(core
  src/CalculationBlock.cpp
  src/Simulator.cpp
  src/LinearRunner.cpp
)

target_include_directories(core
  PUBLIC include
)

