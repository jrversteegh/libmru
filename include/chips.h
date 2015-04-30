#ifndef NINEDOF_CHIPS_
#define NINEDOF_CHIPS_

#include "types.h"
#include "i2cbus.h"
#include "calibration.h"

namespace ninedof {

extern const int compass_address;
extern const int acceleration_address;
extern const int gyro_address;

struct Chip {
  virtual void initialize() = 0;
  virtual void poll() = 0;
  virtual void finalize() = 0;
  const Sample_t& data() const { return data_; }
  const Samples_t& history() const { return history_; }
  Chip(I2CBus& bus, const int address, bool little_endian): 
      device_(bus, address, little_endian), data_(), history_() {}
protected:
  const Chip& push_sample(const Sample_t& sample) {
    history_.push_back(sample);
    data_ = history_.back();
    trim_history();
  }
  const Chip& push_sample(Sample_t&& sample) {
    history_.push_back(sample);
    data_ = history_.back();
    trim_history();
  }
  I2CDevice& device() { return device_; }
private:
  I2CDevice device_;
  Sample_t data_;
  Samples_t history_;
  void trim_history() {
    while (history_.size() > history_item_count) {
      history_.pop_front();
    }
  }
};

struct HMC5843: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  HMC5843(I2CBus& bus): Chip(bus, compass_address, false) {}
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
  ITG3200(I2CBus& bus): Chip(bus, gyro_address, false) {}
  const Value_t temp() const { return data().value; }
};

} //namespace ninedof

#endif
