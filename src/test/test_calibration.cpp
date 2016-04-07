#include <iostream>
#include <sstream>

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <boost/filesystem.hpp>

#include "../../include/types.h"
#include "../../include/calibration.h"

using namespace std;
using namespace ninedof;

boost::filesystem::path app_path;

class CalibrationTest: public CppUnit::TestFixture {
  void testLoad() {
    Calibration calibration = load_calibration(app_path / "calibration/test.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Value_t)8.8, calibration.x_factor);
  }
  void testLoadNonExisting() {
  }
  void testSave() {
  }
  void testSaveExisting() {
  }
public:
  CPPUNIT_TEST_SUITE(CalibrationTest);
  CPPUNIT_TEST(testLoad);
  CPPUNIT_TEST(testLoadNonExisting);
  CPPUNIT_TEST(testSave);
  CPPUNIT_TEST(testSaveExisting);
  CPPUNIT_TEST_SUITE_END();
};

int main(int argc, char* argv[])
{
  app_path = argv[0];
  app_path.remove_filename();
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(CalibrationTest::suite());
  if (runner.run())
    return 0;
  else
    return 1;
}

