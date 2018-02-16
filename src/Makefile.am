SUBDIRS = test

AM_CXXFLAGS = -frounding-math -std=c++11 -O2 -DCGAL_NDEBUG
SRCS = calibration.cc chips.cc i2cbus.cc

lib_LTLIBRARIES = libmru.la
libmru_la_SOURCES = ${SRCS}
libmru_la_LDFLAGS = -version-info 0:0:0
