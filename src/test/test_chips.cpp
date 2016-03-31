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

using namespace ninedof;

struct I2CDeviceMock {
  typedef int Bus_type;
  I2CDeviceMock(Bus_type& bus, int address, bool little_endian=true): words(256), bytes(256) {}
  I2CDeviceMock(const I2CDeviceMock& device): words(256), bytes(256) {}
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
    words[offset] = value;
  }
  void write_words(const int offset, const Words& values) {
    int index = offset;
    for(Word_ci i = values.begin(); i != values.end(); ++i) {
      words[index++] = *i;
    }
  }
  Word read_word(const int offset) const {
    return words[offset];
  }
  Words read_words(const int offset, const int count) const {
    Words result(count);
    int index = offset;
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

class BMP085Test : public CppUnit::TestFixture {
  typedef BMP085T<I2CDeviceMock> BMP085;
  I2CDeviceMock::Bus_type* bus;
  BMP085* ps;
  void test_evaluation() {
  }
public:
  virtual void setUp() {
    bus = new I2CDeviceMock::Bus_type;
    ps = new BMP085(*bus);
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

