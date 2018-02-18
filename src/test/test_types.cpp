#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstdlib>
#include <iostream>

#include "../../include/types.h"
#include "../../include/errors.h"

using namespace mru;
using namespace std;


class TypesTest: public CppUnit::TestFixture {
  void testRotScalar() {
  }
public:
  CPPUNIT_TEST_SUITE(TypesTest);
  CPPUNIT_TEST(testRotScalar);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(TypesTest::suite());
  if (runner.run())
    return 0;
  else
    return 1;
} 

