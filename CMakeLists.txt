cmake_minimum_required(VERSION 2.8)

project(libmru)

# use C++11
set(CMAKE_CXX_FLAGS "-std=c++11")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(CGAL REQUIRED)

include_directories(PRIVATE include)
set(SOURCES
  src/calibration.cc
  src/i2cbus.cc
  src/chips.cc
)
add_library(mru ${SOURCES})

# Application
add_executable(ninedof tools/ninedof.cpp)
target_link_libraries(ninedof mru)

# Install
install(TARGETS mru DESTINATION lib)

# Testing
enable_testing()
add_executable(test_calibration src/test/test_calibration.cpp)
target_link_libraries(test_calibration mru) 
add_test(NAME Calibration COMMAND test_calibration)

