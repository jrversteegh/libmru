
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCES calibration.cc i2cbus.cc chips.cc)
add_library(mru SHARED ${SOURCES})
set_target_properties(mru
  PROPERTIES
  SOVERSION 0.1.0
)
add_library(mru_static STATIC ${SOURCES})
set_target_properties(mru_static
  PROPERTIES
  OUTPUT_NAME mru
)

# Install
install(TARGETS mru mru_static DESTINATION lib)

add_subdirectory(test)
