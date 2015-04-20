#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstdlib>

#include "../../include/i2cbus.h"
#include "../../include/chips.h"
#include "../../include/errors.h"

int busno = 0;

class I2CBusTest: public CppUnit::TestFixture {
  void testCreateBus() {
    CPPUNIT_ASSERT_NO_THROW(I2CBus bus(busno));
    CPPUNIT_ASSERT_THROW(I2CBus bus(999), Error);
  }
  void testScanBus() {
    I2CBus bus(busno);
    Ints addrs = bus.scan();
    CPPUNIT_ASSERT_EQUAL(3, (int)addrs.size());
    CPPUNIT_ASSERT_EQUAL(compass_address, addrs[0]);
    CPPUNIT_ASSERT_EQUAL(acceleration_address, addrs[1]);
    CPPUNIT_ASSERT_EQUAL(gyro_address, addrs[2]);
  }
  void testDeviceRead() {
    I2CBus bus(busno);
    I2CDevice(bus, acceleration_address);
  }
  void testDeviceWrite() {
    I2CBus bus(busno);
    I2CDevice(bus, acceleration_address);
  }
public:
  CPPUNIT_TEST_SUITE(I2CBusTest);
  CPPUNIT_TEST(testCreateBus);
  CPPUNIT_TEST(testScanBus);
  CPPUNIT_TEST(testDeviceRead);
  CPPUNIT_TEST(testDeviceWrite);
  CPPUNIT_TEST_SUITE_END();
};

int main()
{
  char* sbusno = getenv("NINEDOF_BUSNO");
  if (sbusno != 0) {
    busno = atoi(sbusno);
  }
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(I2CBusTest::suite());
  if (runner.run())
    return 0;
  else
    return 1;
}

