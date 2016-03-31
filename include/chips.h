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

template<class Device>
struct Chip {
  virtual void initialize() = 0;
  virtual void poll() = 0;
  virtual void finalize() = 0;
  const Sample_t& data() const { return data_; }
  const Samples_t& history() const { return history_; }
  int chip_id() { return chip_id_; }
  int chip_version() { return chip_version_; }
  Chip(I2CBus& bus, const int address, bool little_endian): 
      device_(bus, address, little_endian), data_(), history_(),
      chip_id_(0), chip_version_(0) {}
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
  void set_chip_id(const int value) { chip_id_ = value; }
  void set_chip_version(const int value) { chip_version_ = value; }
  Device& device() { return device_; }
private:
  Device device_;
  Sample_t data_;
  Samples_t history_;
  int chip_id_;
  int chip_version_;
  void trim_history() {
    while (history_.size() > history_item_count) {
      history_.pop_front();
    }
  }
};

template<class Device>
struct HMC5843: public Chip<Device> {
  virtual void initialize() {
    // 20Hz output, no bias
    device().write_byte(0x00, 0x14);
    // 1 Gauss range
    device().write_byte(0x01, 0x20);
    // Continuous data aquisition
    device().write_byte(0x02, 0x00);
  }
  virtual void poll() {
    //Byte ready = device().read_byte(0x09) & 0x01;
    //if (ready) {
    Words words = device().read_words(0x03, 3);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(static_cast<int16_t>(words[1])) * comp_x_fact + comp_x_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[0])) * comp_y_fact + comp_y_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[2])) * comp_z_fact + comp_z_offs
    ));
    push_sample(sample);
    //}
  }
  virtual void finalize() {
    // Put device to sleep
    device().write_byte(0x02, 0x03);
  }
  HMC5843(I2CBus& bus, const int address): Chip<Device>(bus, address, false) {}
  HMC5843(I2CBus& bus): Chip<Device>(bus, hmc5843_address, false) {}
};

template<class Device>
struct HMC5883: public HMC5843<Device> {
  HMC5883(I2CBus& bus, const int address): HMC5843<Device>(bus, address) {}
  HMC5883(I2CBus& bus): HMC5843<Device>(bus, hmc5883_address) {}
};

template<class Device>
struct ADXL345: public Chip<Device> {
  virtual void initialize() {
    // Clear the sleep bit (when it was set)
    device().write_byte(0x2D, 0x00);
    // Enable measure bit (get out of standby)
    device().write_byte(0x2D, 0x08);
    // Set FULL_RES bit for full resolution on all g scales
    // and the maximum g scale -> +-16g (why would you want to reduce this??)
    // scale is 4mg/bit
    device().write_byte(0x31, 0x0B);
  }
  virtual void poll() {
    Words words = device().read_words(0x32, 3);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(static_cast<int16_t>(words[0])) * accel_x_fact + accel_x_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[1])) * accel_y_fact + accel_y_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[2])) * accel_z_fact + accel_z_offs 
    ));
    push_sample(sample);
  }
  virtual void finalize() {
    // Disable measure bit (set to standby)
    device().write_byte(0x2D, 0x00);
    // Put the device to sleep
    device().write_byte(0x2D, 0x07);
  }
  ADXL345(I2CBus& bus, const int address): Chip<Device>(bus, address, true) {}
  ADXL345(I2CBus& bus): Chip<Device>(bus, adxl345_address, true) {}
};

template<class Device>
struct BMA180: public Chip<Device> {
  virtual void initialize() {
    // Get chip information
    set_chip_id(device().read_byte(0x00));
    set_chip_version(device().read_byte(0x01));

    // Disable wake up mode (bit 0): never sleep
    device().write_byte(0x0D, 0x01);
    // Put in ultra low noise mode
    device().write_byte(0x30, 0x01);
    // Set range to -2/+2g: 0.25mg/bit
    device().write_byte(0x35, 0x04);
    // set filter range to 20Hz (defaults to 1200Hz)
    device().write_byte(0x20, 0x10);
  }
  virtual void poll() {
    Words xyz = device().read_words(0x02, 3);
    Byte temp = device().read_byte(0x08);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(static_cast<int16_t>(xyz[0] >> 2)) * accel_x_fact + accel_x_offs,
        static_cast<Value_t>(static_cast<int16_t>(xyz[1] >> 2)) * accel_y_fact + accel_y_offs,
        static_cast<Value_t>(static_cast<int16_t>(xyz[2] >> 2)) * accel_z_fact + accel_z_offs 
    ), static_cast<Value_t>(static_cast<int8_t>(temp)) * gyro_temp_fact + gyro_temp_offs);
    push_sample(sample);
  }
  virtual void finalize() {
    // Put the device to sleep
    device().write_byte(0x0D, 0x02);
  }
  BMA180(I2CBus& bus, const int address): Chip<Device>(bus, address, true) {}
  BMA180(I2CBus& bus): Chip<Device>(bus, bma180_address, true) {}
};

template<class Device>
struct ITG3200: public Chip<Device> {
  virtual void initialize() {
    // First reset the chip
    device().write_byte(0x3E, 0x80);
    // Wait a little for it to come back up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Sample at 20Hz: 1kHz / 50 (49 + 1)
    device().write_byte(0x15, 0x31);
    // Select range: 0x03 << 3 and low pass filter of 10Hz: 0x05
    device().write_byte(0x16, 0x1D);
    // Get out of sleep and select PLL with X Gyro reference as clock
    device().write_byte(0x3E, 0x01);
  }
  virtual void poll() {
    Words words = device().read_words(0x1B, 4);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(static_cast<int16_t>(words[1])) * gyro_x_fact + gyro_x_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[2])) * gyro_y_fact + gyro_y_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[3])) * gyro_z_fact + gyro_z_offs
      ), static_cast<Value_t>(static_cast<int16_t>(words[0])) * gyro_temp_fact + gyro_temp_offs
    ); 
    push_sample(sample);
  }
  virtual void finalize() {
    // Put to sleep and select internal oscillator as clock
    device().write_byte(0x3E, 0x40);
  }
  ITG3200(I2CBus& bus, const int address): Chip<Device>(bus, address, false) {}
  ITG3200(I2CBus& bus): Chip<Device>(bus, itg3200_address, false) {}
  const Value_t temp() const { return Chip<Device>::data().value; }
};

template<class Device>
struct ITG3205: public ITG3200<Device> {
  ITG3205(I2CBus& bus, const int address): ITG3200<Device>(bus, address) {}
  ITG3205(I2CBus& bus): ITG3200<Device>(bus, itg3205_address) {}
};

template<class Device>
struct BMP085: public Chip<Device> {
  virtual void initialize() {
    // Read calibration data from EEPROM
    Words words = device().read_words(0xAA, 11);
  }
  virtual void poll() {
    if (loop_count_ % 120 == 0) {
      loop_count_ = 0;
      // Get temperature
      device().write_byte(0xF4, 0x2E);
    } else if (loop_count_ == 1) {
      // Read temperature
      Byte raw_temp = device().read_byte(0xF6);
      raw_temp = eval_temp(raw_temp);
    } else if (loop_count_ % 2 == 0) {
      // Read pressure 8 times oversampling: takes 25ms
      device().write_byte(0xF4, 0xF4);
    } else {
      Word raw_pressure = device().read_word(0xF6);
      int32_t pressure = eval_pressure(raw_pressure);
      Sample_t sample = Sample_t(Vector_t(
          static_cast<Value_t>(pressure) * pressure_fact + pressure_offs, 0, 0), 
          static_cast<Value_t>(temp_) * temp_fact + temp_offs); 
      push_sample(sample);
    }
    loop_count_++;
  }
  virtual void finalize() {
  }
  BMP085(I2CBus& bus, const int address): Chip<Device>(bus, address, false), loop_count_(0) {}
  BMP085(I2CBus& bus): Chip<Device>(bus, bmp085_address, false), loop_count_(0) {}
protected:
  int32_t eval_temp(const Word raw_temp);
  int32_t eval_pressure(const int32_t raw_pressure);
  void set_calibration_data(const Words& calibration_data);
private:
  // Calibration parameters
  int16_t ac1_;
  int16_t ac2_;
  int16_t ac3_;
  uint16_t ac4_;
  uint16_t ac5_;
  uint16_t ac6_;
  int16_t b1_;
  int16_t b2_;
  int32_t b5_;
  int16_t mb_;
  int16_t mc_;
  int16_t md_;
  // State machine position indicator
  int loop_count_;
  int32_t temp_;
};

} //namespace ninedof

#endif
