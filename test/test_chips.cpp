/** \file
 * Test chip communications
 *
 * \author J.R. Versteegh
 */

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>


#include "../include/chips.h"

using namespace std;
using namespace ninedof;

class ChipsTest : public CppUnit::TestFixture {
  void test_BMP085_evaluation() {
  }
public:
  CPPUNIT_TEST_SUITE(ChipsTest);
  CPPUNIT_TEST(test_BMP085_evaluation);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(ChipsTest::suite());
  if (runner.run())
    return 0;
  else
    return 1;
}
