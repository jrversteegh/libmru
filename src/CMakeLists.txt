
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCES calibration.cc i2cbus.cc chips.cc)
add_library(mru ${SOURCES})

# Install
install(TARGETS mru DESTINATION lib)

add_subdirectory(test)
