#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../../include/chips.h"

using namespace ninedof;

class ADXL345Test: public CppUnit::TestFixture {
  void testDummy() {
  }
public:
  CPPUNIT_TEST_SUITE(ADXL345Test);
  CPPUNIT_TEST(testDummy);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(ADXL345Test::suite());
  if (runner.run())
    return 0;
  else
    return 1;
}

