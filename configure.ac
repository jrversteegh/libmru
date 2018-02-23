AC_INIT([libmru], [0.1.0], [j.r.versteegh@orca-st.com])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_MACRO_DIR([m4])

AM_PROG_AR
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11])

LT_INIT

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
  Makefile
  include/Makefile
  src/Makefile
  src/test/Makefile
  doc/Makefile
])

AC_CHECK_HEADER([linux/i2c-dev.h], [], [AC_MSG_ERROR([Unable to find or use linux/i2c-dev.h. There exist two versions of this file. Need libi2c-dev?])],
  [
   #include <linux/i2c-dev.h>
   #ifndef I2C_SMBUS_READ 
   #error "Failed."
   #endif
  ]
)

AC_LANG_PUSH([C++])
AC_CHECK_HEADER([boost/date_time.hpp], [], [AC_MSG_ERROR([Unable to find or use boost/date_time.hpp. Need libboost-dev?])])
AC_CHECK_HEADER([boost/property_tree/ptree.hpp], [], [AC_MSG_ERROR([Unable to find or use boost/property_tree/ptree.hpp. Need libboost-dev?])])
AC_CHECK_HEADER([boost/filesystem.hpp], [], [AC_MSG_ERROR([Unable to find or use boost/filesystem.hpp]. Need libboost-filesystem-dev?)])
AC_CHECK_HEADER([CGAL/Vector_3.h], [], [AC_MSG_ERROR([Unable to find or use CGAL/Vector_3.h. Need libcgal-dev?])])
AC_LANG_POP
AC_CHECK_LIB([boost_system], [_init], [], [AC_MSG_ERROR([Unable to find boost_system library])])
AC_CHECK_LIB([boost_filesystem], [_init], [], [AC_MSG_ERROR([Unable to find boost_filesystem library])])
AC_CHECK_LIB([CGAL], [_init], [], [AC_MSG_ERROR([Unable to find CGAL library.])])

AC_PATH_PROG([DOXYGEN], [doxygen], [])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], AC_CONFIG_FILES([doc/Doxyfile]))

# For unit tests
m4_include([m4/cppunit.m4])
AM_PATH_CPPUNIT([1.0.0], [], [AC_MSG_NOTICE([CppUnit not found. Unit tests will not be build.])])
AM_CONDITIONAL([HAVE_CPPUNIT], [test "x$CPPUNIT_CONFIG" != "xno"])

AC_OUTPUT
