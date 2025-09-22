cmake_minimum_required(VERSION 3.10)

add_library(core
  src/CalculationBlock.cpp
  src/Connector.cpp
  src/Simulator.cpp
  src/LinearRunner.cpp
  src/WegsteinRunner.cpp
  src/Pin.cpp
  src/Numeric.cpp
  src/Connectivity.cpp
  src/CalculationMethod.cpp
)

target_include_directories(core
  PUBLIC include
)

