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

boost::filesystem::path app_path;

using Calibration = mru::Calibration<float>;
using Scalar = mru::Scalar<float>;

class CalibrationTest: public CppUnit::TestFixture {
  void testLoad() {
    Calibration calibration = mru::load_calibration(app_path/"calibration/test.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Scalar)8.8, calibration.x_factor());
    CPPUNIT_ASSERT_EQUAL((Scalar)9.9, calibration.x_offset());
    CPPUNIT_ASSERT_EQUAL((Scalar)88, calibration.y_factor());
    CPPUNIT_ASSERT_EQUAL((Scalar)99, calibration.y_offset());
    CPPUNIT_ASSERT_EQUAL((Scalar)880, calibration.z_factor());
    CPPUNIT_ASSERT_EQUAL((Scalar)990, calibration.z_offset());
    CPPUNIT_ASSERT_EQUAL((Scalar)8800, calibration.v_factor());
    CPPUNIT_ASSERT_EQUAL((Scalar)9900, calibration.v_offset());
  }
  void testLoadNonExisting() {
    Calibration calibration = mru::load_calibration(app_path/"calibration/nonexisting.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Scalar)1.0, calibration.x_factor());
  }
  void testSave() {
    Calibration calibration(1.1, 0, 1, 0, 1, 0, 1, 0);
    save_calibration(app_path / "calibration/temp.ini", "test", calibration);
    calibration = mru::load_calibration(app_path/"calibration/temp.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Scalar)1.1, calibration.x_factor());
  }
  void testSaveExisting() {
    boost::filesystem::copy_file(app_path/"calibration/test.ini", app_path/"calibration/copy.ini");
    Calibration calibration(2.2, 0, 1, 0, 1, 0, 1, 0);
    mru::save_calibration(app_path / "calibration/copy.ini", "test_copy", calibration);
    calibration = mru::load_calibration(app_path/"calibration/copy.ini", "test");
    CPPUNIT_ASSERT_EQUAL((Scalar)8.8, calibration.x_factor());
    calibration = mru::load_calibration(app_path/"calibration/copy.ini", "test_copy");
    CPPUNIT_ASSERT_EQUAL((Scalar)2.2, calibration.x_factor());
  }
public:
  virtual void setUp() {
    boost::filesystem::remove(app_path/"calibration/copy.ini");
    boost::filesystem::remove(app_path/"calibration/temp.ini");
  }
  virtual void tearDown() {
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

