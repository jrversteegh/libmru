cmake_minimum_required(VERSION 2.8)

project(libmru)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

find_package(CGAL REQUIRED)
set(Boost_USE_MULTITHREADED ON) 
find_package(Boost REQUIRED COMPONENTS filesystem date_time)

include_directories(PRIVATE include)
include(${CGAL_USE_FILE})
link_libraries(${Boost_LIBRARIES})

# use C++11
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(cppunit)
if (CPPUNIT_FOUND)
  enable_testing()
endif()

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(doc)


