#ifndef NINEDOF_CHIPS_
#define NINEDOF_CHIPS_

#include "types.h"
#include "i2cbus.h"

namespace ninedof {

extern const int compass_address;
extern const int acceleration_address;
extern const int gyro_address;

struct Chip {
  virtual void initialize() = 0;
  virtual void poll() = 0;
  virtual void finalize() = 0;
  const Vector* const raw() const { return &raw_; }
  const Vector* const data() const { return &data_; }
  Chip(I2CBus& bus, const int address, bool little_endian): 
      device_(bus, address, little_endian), raw_(), data_() {}
protected:
  I2CDevice device_;
  Vector raw_;
  Vector data_;
};

struct HMC5843: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  HMC5843(I2CBus& bus): Chip(bus, compass_address, true) {}
};

struct ADXL345: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  ADXL345(I2CBus& bus): Chip(bus, acceleration_address, true) {}
};


struct ITG3200: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  ITG3200(I2CBus& bus): Chip(bus, gyro_address, true) {}
};

} //namespace ninedof

#endif
