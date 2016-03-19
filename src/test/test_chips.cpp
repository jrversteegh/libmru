/** \file
 * Test chip communications
 *
 * \author J.R. Versteegh
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../../include/chips.h"

using namespace ninedof;

class ADXL345Test: public CppUnit::TestFixture {
  void test_dummy() {
  }
public:
  CPPUNIT_TEST_SUITE(ADXL345Test);
  CPPUNIT_TEST(test_dummy);
  CPPUNIT_TEST_SUITE_END();
};

class BMP085Test : public CppUnit::TestFixture {
  void test_evaluation() {
  }
public:
  CPPUNIT_TEST_SUITE(BMP085Test);
  CPPUNIT_TEST(test_evaluation);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(ADXL345Test::suite());
  runner.addTest(BMP085Test::suite());
  if (runner.run())
    return 0;
  else
    return 1;
}

