/** \file
 * Test chip communications
 *
 * \author J.R. Versteegh
 */

#include <vector>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "../../include/i2cbus.h"
#include "../../include/chips.h"

using namespace mru;

struct I2CDeviceMock {
  typedef int Bus_type;
  I2CDeviceMock(Bus_type& bus, int address, bool little_endian=true): words(256), bytes(256) {}
  I2CDeviceMock(const I2CDeviceMock& device): words(128), bytes(256) {}
  void write_byte(const int offset, const Byte value) {
    bytes[offset] = value;
  }
  void write_bytes(const int offset, const Bytes& values) {
    int index = offset;
    for(Byte_ci i = values.begin(); i != values.end(); ++i) {
      bytes[index++] = *i;
    }
  }
  Byte read_byte(const int offset) const {
    return bytes[offset];
  }
  Bytes read_bytes(const int offset, const int count) const {
    Bytes result(count);
    int index = offset;
    for(Byte_i i = result.begin(); i != result.end(); ++i) {
      *i = bytes[index++];
    }
    return result;
  }
  void write_word(const int offset, const Word value) {
    words[offset >> 1] = value;
  }
  void write_words(const int offset, const Words& values) {
    int index = offset >> 1;
    for(Word_ci i = values.begin(); i != values.end(); ++i) {
      words[index++] = *i;
    }
  }
  Word read_word(const int offset) const {
    return words[offset >> 1];
  }
  Words read_words(const int offset, const int count) const {
    Words result(count);
    int index = offset >> 1;
    for(Word_i i = result.begin(); i != result.end(); ++i) {
      *i = words[index++];
    }
    return result;
  }
  Words words;
  Bytes bytes;
};

class ADXL345Test: public CppUnit::TestFixture {
  void test_dummy() {
  }
public:
  CPPUNIT_TEST_SUITE(ADXL345Test);
  CPPUNIT_TEST(test_dummy);
  CPPUNIT_TEST_SUITE_END();
};

class BMP085ForTest: public BMP085T<I2CDeviceMock, float> {
public:
  using BMP085T<I2CDeviceMock, float>::BMP085T;
  using BMP085T<I2CDeviceMock, float>::eval_temp;
  using BMP085T<I2CDeviceMock, float>::eval_pressure;
  using BMP085T<I2CDeviceMock, float>::device;
};

class BMP085Test : public CppUnit::TestFixture {
  I2CDeviceMock::Bus_type* bus;
  BMP085ForTest* ps;
  void test_evaluation() {
    ps->initialize();
    // Example values from data sheet
    int32_t temp = ps->eval_temp(27898);
    int32_t pressure = ps->eval_pressure(23843);
    CPPUNIT_ASSERT_EQUAL(150, temp);
    CPPUNIT_ASSERT_EQUAL(69964, pressure);
  }
public:
  virtual void setUp() {
    bus = new I2CDeviceMock::Bus_type;
    ps = new BMP085ForTest(*bus, 0, 0);
    // Setup example calibration data from data sheet
    ps->device().words[0xAA >> 1] = 408;
    ps->device().words[0xAC >> 1] = -72;
    ps->device().words[0xAE >> 1] = -14383; 
    ps->device().words[0xB0 >> 1] = 32741; 
    ps->device().words[0xB2 >> 1] = 32757; 
    ps->device().words[0xB4 >> 1] = 23153;
    ps->device().words[0xB6 >> 1] = 6190;
    ps->device().words[0xB8 >> 1] = 4;
    ps->device().words[0xBA >> 1] = -32768;
    ps->device().words[0xBC >> 1] = -8711;
    ps->device().words[0xBE >> 1] = 2868;
  }
  virtual void tearDown() {
    delete ps;
    delete bus;
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

