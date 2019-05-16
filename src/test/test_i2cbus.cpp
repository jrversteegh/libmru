#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cstdlib>
#include <iostream>

#include "../../include/i2cbus.h"
#include "../../include/chips.h"
#include "../../include/errors.h"

using namespace mru;
using namespace std;

int busno = 0;

class I2CBusTest: public CppUnit::TestFixture {
  void testCreateBus() {
    CPPUNIT_ASSERT_NO_THROW(I2C_bus bus(busno));
    CPPUNIT_ASSERT_THROW(I2C_bus bus(999), Error);
  }
  void testScanBus() {
    I2C_bus bus(busno);
    Ints addrs = bus.scan();
    CPPUNIT_ASSERT_EQUAL(3, (int)addrs.size());
    CPPUNIT_ASSERT_EQUAL(HMC5883::default_address, addrs[0]);
    CPPUNIT_ASSERT_EQUAL(ADXL345::default_address, addrs[1]);
    CPPUNIT_ASSERT_EQUAL(ITG3200::default_address, addrs[2]);
  }
  void testDeviceRead() {
    // This test may fail when the sensor values changes between reads
    const int regaddr = 0x2C;
    I2C_bus bus(busno);
    I2C_device device(bus, ADXL345::default_address);
    Byte b = device.read_byte(regaddr);
    Word w = device.read_word(regaddr);
    Words ws = device.read_words(regaddr, 2);
    Bytes bs = device.read_bytes(regaddr, 4);
    CPPUNIT_ASSERT_EQUAL((int)b, (int)bs[0] );
    CPPUNIT_ASSERT_EQUAL((int)w, (int)ws[0] );
    CPPUNIT_ASSERT_EQUAL((int)bs[0], ((int)ws[0] & 0xFF));
    CPPUNIT_ASSERT_EQUAL((int)bs[1], ((int)ws[0] >> 8));
    CPPUNIT_ASSERT_EQUAL((int)bs[2], ((int)ws[1] & 0xFF));
    CPPUNIT_ASSERT_EQUAL((int)bs[3], ((int)ws[1] >> 8));
    // Test big endian word reading
    I2C_device device2 = I2C_device(bus, ADXL345::default_address, false);
    w = device2.read_word(regaddr);
    ws = device2.read_words(regaddr, 2);
    // This is somewhat arbitrary: we demand the bytes are different so
    // the test of the byte order becomes valid.
    CPPUNIT_ASSERT((int)bs[0] != (int)bs[1]);
    CPPUNIT_ASSERT_EQUAL((int)w, (int)ws[0] );
    CPPUNIT_ASSERT_EQUAL((int)bs[0], ((int)ws[0] >> 8));
    CPPUNIT_ASSERT_EQUAL((int)bs[1], ((int)ws[0] & 0xFF));
  }
  void testDeviceWrite() {
    I2C_bus bus(busno);
    I2C_device device(bus, ADXL345::default_address);
    Bytes bs;
    bs.push_back(0x08);
    bs.push_back(0x80);
    device.write_bytes(0x2D, bs);
    Byte b = device.read_byte(0x2E);
    CPPUNIT_ASSERT_EQUAL(0x80, (int)b);
    device.write_word(0x2D, 0x9008);
    b = device.read_byte(0x2E);
    CPPUNIT_ASSERT_EQUAL(0x90, (int)b);
    I2C_device device2(bus, ADXL345::default_address, false);
    Words ws;
    ws.push_back(0x0880);
    device2.write_words(0x2D, ws);
    b = device2.read_byte(0x2E);
    CPPUNIT_ASSERT_EQUAL(0x80, (int)b);
    device2.write_word(0x2D, 0);
    b = device2.read_byte(0x2E);
    CPPUNIT_ASSERT_EQUAL(0x0, (int)b);
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
  char* sbusno = getenv("MRU_BUSNO");
  if (sbusno != 0) {
    busno = atoi(sbusno);
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(I2CBusTest::suite());
    if (runner.run())
      return 0;
    else
      return 1;
  } 
  else {
    std::cout << "Please set the  MRU_BUSNO environment variable to run this test." << std::endl;
  }
}

