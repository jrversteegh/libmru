#include <iostream>
#include <sstream>

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <boost/filesystem.hpp>

#include "boost_copy_file.h"

#include "../../include/types.h"
#include "../../include/calibration.h"

using namespace std;
using namespace ninedof;

boost::filesystem::path app_path;

class CalibrationTest: public CppUnit::TestFixture {
  void testLoad() {
    Calibration calibration = load_calibration(app_path/"calibration/test.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Value)8.8, calibration.x_factor());
  }
  void testLoadNonExisting() {
    Calibration calibration = load_calibration(app_path/"calibration/nonexisting.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Value)1.0, calibration.x_factor());
  }
  void testSave() {
    Calibration calibration(1.1, 0, 1, 0, 1, 0, 1, 0);
    save_calibration(app_path / "calibration/temp.ini", "test", calibration);
    calibration = load_calibration(app_path/"calibration/temp.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Value)1.1, calibration.x_factor());
  }
  void testSaveExisting() {
    test_utils::copy_file(app_path/"calibration/test.ini", app_path/"calibration/copy.ini");
    Calibration calibration(2.2, 0, 1, 0, 1, 0, 1, 0);
    save_calibration(app_path / "calibration/copy.ini", "test_copy", calibration);
    calibration = load_calibration(app_path/"calibration/copy.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Value)8.8, calibration.x_factor());
    calibration = load_calibration(app_path/"calibration/copy.ini", "test_copy");
    CPPUNIT_ASSERT_EQUAL((Value)2.2, calibration.x_factor());
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

