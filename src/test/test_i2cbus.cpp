#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../../include/i2cbus.h"

class I2CBusTest: public CppUnit::TestFixture {
  void testDummy() {
  }
public:
  CPPUNIT_TEST_SUITE(I2CBusTest);
  CPPUNIT_TEST(testDummy);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(I2CBusTest::suite());
  if (runner.run())
    return 0;
  else
    return 1;
}

