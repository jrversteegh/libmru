/* 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/** \file
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 * Declares types for interfacing with the sensor ASICS
 */

#ifndef NINEDOF_CHIPS_
#define NINEDOF_CHIPS_

#include "types.h"
#include "i2cbus.h"
#include "calibration.h"

namespace ninedof {

extern const int hmc5843_address;
extern const int hmc5883_address;
extern const int adxl345_address;
extern const int bma180_address;
extern const int itg3200_address;
extern const int itg3205_address;
extern const int bmp085_address;

struct Chip {
  virtual void initialize() = 0;
  virtual void poll() = 0;
  virtual void finalize() = 0;
  const Sample_t& data() const { return data_; }
  const Samples_t& history() const { return history_; }
  Chip(I2CBus& bus, const int address, bool little_endian): 
      device_(bus, address, little_endian), data_(), history_() {}
protected:
  Chip& push_sample(const Sample_t& sample) {
    history_.push_back(sample);
    data_ = history_.back();
    trim_history();
    return *this;
  }
  Chip& push_sample(Sample_t&& sample) {
    history_.push_back(sample);
    data_ = history_.back();
    trim_history();
    return *this;
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
  HMC5843(I2CBus& bus, const int address): Chip(bus, address, false) {}
  HMC5843(I2CBus& bus): Chip(bus, hmc5843_address, false) {}
};

struct HMC5883: public HMC5843 {
  HMC5883(I2CBus& bus, const int address): HMC5843(bus, address) {}
  HMC5883(I2CBus& bus): HMC5843(bus, hmc5883_address) {}
};

struct ADXL345: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  ADXL345(I2CBus& bus, const int address): Chip(bus, address, true) {}
  ADXL345(I2CBus& bus): Chip(bus, adxl345_address, true) {}
};

struct BMA180: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  BMA180(I2CBus& bus, const int address): Chip(bus, address, false) {}
  BMA180(I2CBus& bus): Chip(bus, bma180_address, false) {}
};

struct ITG3200: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  ITG3200(I2CBus& bus, const int address): Chip(bus, address, false) {}
  ITG3200(I2CBus& bus): Chip(bus, itg3200_address, false) {}
  const Value_t temp() const { return data().value; }
};

struct ITG3205: public ITG3200 {
  ITG3205(I2CBus& bus, const int address): ITG3200(bus, address) {}
  ITG3205(I2CBus& bus): ITG3200(bus, itg3205_address) {}
  const Value_t temp() const { return data().value; }
};

struct BMP085: public Chip {
  virtual void initialize();
  virtual void poll();
  virtual void finalize();
  BMP085(I2CBus& bus, const int address): Chip(bus, address, false) {}
  BMP085(I2CBus& bus): Chip(bus, bmp085_address, false) {}
};

} //namespace ninedof

#endif
