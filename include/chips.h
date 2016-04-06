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

#include <chrono>
#include <thread>

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
  Chip(typename Device::Bus_type& bus, const int address, bool little_endian): 
      device_(bus, address, little_endian), data_(), history_(),
      chip_id_(0), chip_version_(0) {}
protected:
  Chip& push_sample(const Sample_t& sample) {
    data_ = sample;
    history_.push_back(data_);
    trim_history();
    return *this;
  }
  Chip& push_sample(Sample_t&& sample) {
    data_ = sample;
    history_.push_back(data_);
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
struct HMC5843T: public Chip<Device> {
  virtual void initialize() {
    // 20Hz output, no bias
    this->device().write_byte(0x00, 0x14);
    // 1 Gauss range
    this->device().write_byte(0x01, 0x20);
    // Continuous data aquisition
    this->device().write_byte(0x02, 0x00);
  }
  virtual void poll() {
    //Byte ready = this->device().read_byte(0x09) & 0x01;
    //if (ready) {
    Words words = this->device().read_words(0x03, 3);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(static_cast<int16_t>(words[1])) * hmc5843_x_fact + hmc5843_x_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[0])) * hmc5843_y_fact + hmc5843_y_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[2])) * hmc5843_z_fact + hmc5843_z_offs
    ));
    this->push_sample(sample);
    //}
  }
  virtual void finalize() {
    // Put device to sleep
    this->device().write_byte(0x02, 0x03);
  }
  HMC5843T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, false) {}
  HMC5843T(typename Device::Bus_type& bus): Chip<Device>(bus, hmc5843_address, false) {}
};

typedef HMC5843T<I2C_device> HMC5843;

template<class Device>
struct HMC5883T: public HMC5843T<Device> {
  HMC5883T(typename Device::Bus_type& bus, const int address): HMC5843T<Device>(bus, address) {}
  HMC5883T(typename Device::Bus_type& bus): HMC5843T<Device>(bus, hmc5883_address) {}
};

typedef HMC5883T<I2C_device> HMC5883;

template<class Device>
struct ADXL345T: public Chip<Device> {
  virtual void initialize() {
    // Clear the sleep bit (when it was set)
    this->device().write_byte(0x2D, 0x00);
    // Enable measure bit (get out of standby)
    this->device().write_byte(0x2D, 0x08);
    // Set FULL_RES bit for full resolution on all g scales
    // and the maximum g scale -> +-16g (why would you want to reduce this??)
    // scale is 4mg/bit
    this->device().write_byte(0x31, 0x0B);
  }
  virtual void poll() {
    Words words = this->device().read_words(0x32, 3);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(static_cast<int16_t>(words[0])) * adxl345_x_fact + adxl345_x_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[1])) * adxl345_y_fact + adxl345_y_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[2])) * adxl345_z_fact + adxl345_z_offs 
    ));
    this->push_sample(sample);
  }
  virtual void finalize() {
    // Disable measure bit (set to standby)
    this->device().write_byte(0x2D, 0x00);
    // Put thethis->device to sleep
    this->device().write_byte(0x2D, 0x07);
  }
  ADXL345T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, true) {}
  ADXL345T(typename Device::Bus_type& bus): Chip<Device>(bus, adxl345_address, true) {}
};

typedef ADXL345T<I2C_device> ADXL345;

template<class Device>
struct BMA180T: public Chip<Device> {
  virtual void initialize() {

    // Start by soft resetting the device
    this->device().write_byte(0x10, 0xB6);
    // Wait a little bit for the device to come up
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Disable wake up mode (bit 0)
    this->device().write_byte(0x0D, 0x01);
    // ... and bit 0
    this->device().write_byte(0x34, 0x80);

    // Set range to -4/+4g: 0.5mg/bit
    this->device().write_byte(0x35, 0x08);
    // Put in ultra low noise mode
    this->device().write_byte(0x30, 0x01);
    // set filter range to 20Hz (defaults to 1200Hz)
    this->device().write_byte(0x20, 0x10);

    // Get chip information
    this->set_chip_id(this->device().read_byte(0x00));
    this->set_chip_version(this->device().read_byte(0x01));
  }
  virtual void poll() {
    Words xyz = this->device().read_words(0x02, 3);
    Byte temp = this->device().read_byte(0x08);
    int16_t x = static_cast<int16_t>(xyz[0]);
    int16_t y = static_cast<int16_t>(xyz[1]);
    int16_t z = static_cast<int16_t>(xyz[2]);
    // Check the 0 bits of the data for whether the data is new..
    if ((x % 1 == 1) && (y % 1 == 1) && (z % 1 == 1)) {
      // .. if so, add it with the 0 and 1 bits shifted out (the values are only 14 bit)
      Sample_t sample = Sample_t(Vector_t(
          static_cast<Value_t>(x >> 2) * bma180_x_fact + bma180_x_offs,
          static_cast<Value_t>(y >> 2) * bma180_y_fact + bma180_y_offs,
          static_cast<Value_t>(z >> 2) * bma180_z_fact + bma180_z_offs 
      ), static_cast<Value_t>(static_cast<int8_t>(temp)) * bma180_temp_fact + bma180_temp_offs);
      this->push_sample(sample);
    }
  }
  virtual void finalize() {
    // Put the device to sleep
    this->device().write_byte(0x0D, 0x02);
  }
  BMA180T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, true) {}
  BMA180T(typename Device::Bus_type& bus): Chip<Device>(bus, bma180_address, true) {}
};

typedef BMA180T<I2C_device> BMA180;

template<class Device>
struct ITG3200T: public Chip<Device> {
  virtual void initialize() {
    // First reset the chip
    this->device().write_byte(0x3E, 0x80);
    // Wait a little for it to come back up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Sample at 20Hz: 1kHz / 50 (49 + 1)
    this->device().write_byte(0x15, 0x31);
    // Select range: 0x03 << 3 and low pass filter of 10Hz: 0x05
    this->device().write_byte(0x16, 0x1D);
    // Get out of sleep and select PLL with X Gyro reference as clock
    this->device().write_byte(0x3E, 0x01);
  }
  virtual void poll() {
    Words words = this->device().read_words(0x1B, 4);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(static_cast<int16_t>(words[1])) * itg3200_x_fact + itg3200_x_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[2])) * itg3200_y_fact + itg3200_y_offs,
        static_cast<Value_t>(static_cast<int16_t>(words[3])) * itg3200_z_fact + itg3200_z_offs
      ), static_cast<Value_t>(static_cast<int16_t>(words[0])) * itg3200_temp_fact + itg3200_temp_offs
    ); 
    this->push_sample(sample);
  }
  virtual void finalize() {
    // Put to sleep and select internal oscillator as clock
    this->device().write_byte(0x3E, 0x40);
  }
  ITG3200T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, false) {}
  ITG3200T(typename Device::Bus_type& bus): Chip<Device>(bus, itg3200_address, false) {}
};

typedef ITG3200T<I2C_device> ITG3200;

template<class Device>
struct ITG3205T: public ITG3200T<Device> {
  ITG3205T(typename Device::Bus_type& bus, const int address): ITG3200T<Device>(bus, address) {}
  ITG3205T(typename Device::Bus_type& bus): ITG3200T<Device>(bus, itg3205_address) {}
};

typedef ITG3205T<I2C_device> ITG3205;

template<class Device>
struct BMP085T: public Chip<Device> {
  virtual void initialize() {
    // Read calibration data from EEPROM
    Words words = this->device().read_words(0xAA, 11);
    set_calibration_data(words);
  }
  virtual void poll() {
    if (loop_count_ % 120 == 0) {
      loop_count_ = 0;
      // Get temperature
      this->device().write_byte(0xF4, 0x2E);
    } else if (loop_count_ == 1) {
      // Read temperature
      Word raw_temp = this->device().read_word(0xF6);
      temp_ = eval_temp(raw_temp);
    } else if (loop_count_ % 2 == 0) {
      // Get pressure 8 times oversampling: takes 25ms
      this->device().write_byte(0xF4, 0x34 + (oss_ << 6));
    } else {
      Bytes raw_pressure = this->device().read_bytes(0xF6, 3);
      int32_t pressure = (raw_pressure[0] << 16) + (raw_pressure[1] << 8) + raw_pressure[0];
      pressure >>= (8 - oss_);
      pressure = eval_pressure(pressure);
      Sample_t sample = Sample_t(Vector_t(
          0, 0, static_cast<Value_t>(pressure) * bmp085_pressure_fact + bmp085_pressure_offs), 
          static_cast<Value_t>(temp_) * bmp085_temp_fact + bmp085_temp_offs); 
      this->push_sample(sample);
    }
    loop_count_++;
  }
  virtual void finalize() {
  }
  BMP085T(typename Device::Bus_type& bus, const int address, const int oss): 
        Chip<Device>(bus, address, false), loop_count_(0), oss_(oss) {}
  BMP085T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, false), loop_count_(0), oss_(3) {}
  BMP085T(typename Device::Bus_type& bus): Chip<Device>(bus, bmp085_address, false), loop_count_(0), oss_(3) {}
protected:
  int32_t eval_temp(const Word raw_temp);
  int32_t eval_pressure(const int32_t raw_pressure);
  void set_calibration_data(const Words& calibration_data);
private:
  // Oversampling rate
  int oss_;
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

template <class Device>
void BMP085T<Device>::set_calibration_data(const Words& calibration_data)
{
  ac1_ = static_cast<int16_t>(calibration_data[0]);
  ac2_ = static_cast<int16_t>(calibration_data[1]);
  ac3_ = static_cast<int16_t>(calibration_data[2]);
  ac4_ = static_cast<uint16_t>(calibration_data[3]);
  ac5_ = static_cast<uint16_t>(calibration_data[4]);
  ac6_ = static_cast<uint16_t>(calibration_data[5]);
  b1_ = static_cast<int16_t>(calibration_data[6]);
  b2_ = static_cast<int16_t>(calibration_data[7]);
  mb_ = static_cast<int16_t>(calibration_data[8]);
  mc_ = static_cast<int16_t>(calibration_data[9]);
  md_ = static_cast<int16_t>(calibration_data[10]);
}

template <class Device>
int32_t BMP085T<Device>::eval_temp(const Word raw_temp)
{
  int32_t result = static_cast<int32_t>(raw_temp);
  int32_t x1 = ((result - ac6_) * ac5_) >>  15;
  int32_t x2 = (static_cast<int32_t>(mc_) << 11) / (x1 + md_);
  b5_ = x1 + x2;
  result = (b5_ + 8) >> 4;
  return result;
}

template <class Device>
int32_t BMP085T<Device>::eval_pressure(const int32_t raw_pressure)
{
  int32_t result = 0;
  int32_t b6 = b5_ - 4000;
  int32_t x1 = (b2_ * ((b6 * b6) >> 12)) >> 11;
  int32_t x2 = (ac2_ * b6) >> 11;
  int32_t x3 = x1 + x2;
  int32_t b3 = (((ac1_ * 4 + x3) << oss_) + 2) / 4;
  x1 = (ac3_ * b6) >> 13;
  x2 = (b1_ * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) / 4;
  uint32_t b4 = ac4_ * (uint32_t)(x3 + 32768) >> 15;
  uint32_t b7 = ((uint32_t)raw_pressure - b3) * (50000 >> oss_);
  if (b7 < 0x80000000) {
    result = (b7 * 2) / b4;
  } 
  else {
    result = (b7 / b4) * 2;
  }
  x1 = (result >> 8) * (result >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * result) >> 16;
  result = result + ((x1 + x2 + 3791) >> 4);
  return result;
}

typedef BMP085T<I2C_device> BMP085;

} //namespace ninedof

#endif
