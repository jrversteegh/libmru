if HAVE_CPPUNIT

AM_CXXFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include $(CPPUNIT_FLAGS)
SRCS = ../calibration.cc ../chips.cc ../i2cbus.cc

check_PROGRAMS = test_types test_cgal test_calibration test_chips test_i2cbus
TESTS = $(check_PROGRAMS)

test_types_SOURCES = test_types.cpp 
test_types_LDADD = $(CPPUNIT_LIBS)

test_cgal_SOURCES = test_cgal.cpp
test_cgal_LDADD = $(CPPUNIT_LIBS)

test_calibration_SOURCES = test_calibration.cpp $(SRCS)
test_calibration_LDADD = $(CPPUNIT_LIBS)

test_chips_SOURCES = test_chips.cpp $(SRCS)
test_chips_LDADD = $(CPPUNIT_LIBS)

test_i2cbus_SOURCES = test_i2cbus.cpp $(SRCS)
test_i2cbus_LDADD = $(CPPUNIT_LIBS)

.PHONY: test

test: check

endif

