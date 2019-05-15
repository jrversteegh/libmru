cmake_minimum_required(VERSION 2.8)

project(libmru)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

#set(Boost_DEBUG ON)
set(BUILD_SHARED_LIBS ON)
find_package(Boost 1.67 REQUIRED COMPONENTS date_time filesystem)

find_package(CGAL REQUIRED)

include_directories(PRIVATE include)
include(${CGAL_USE_FILE})
link_libraries(i2c)

# use C++11
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(cppunit)
if (CPPUNIT_FOUND)
  enable_testing()
endif()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(doc)


