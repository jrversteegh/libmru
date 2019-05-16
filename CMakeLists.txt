cmake_minimum_required(VERSION 2.8)

project(libmru)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

#set(Boost_DEBUG ON)
set(BUILD_SHARED_LIBS ON)
find_package(Boost 1.67 REQUIRED COMPONENTS system date_time filesystem)

find_package(CGAL REQUIRED)

include_directories(
  PRIVATE include
  ${Boost_INCLUDE_DIR}
)
link_directories(
  ${Boost_LIBRARY_DIR}
)
include(${CGAL_USE_FILE})
link_libraries(
  i2c 
  boost_system 
  boost_filesystem 
  boost_date_time
)

# use latest C++
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(cppunit)
if (CPPUNIT_FOUND)
  enable_testing()
endif()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(doc)


