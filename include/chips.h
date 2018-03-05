/**
 * \file
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 * \brief Classes for interfacing individual chips
 * \license
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MRU_CHIPS_H
#define MRU_CHIPS_H

#include <chrono>
#include <thread>

#include <boost/filesystem.hpp>

#include "types.h"
#include "i2cbus.h"
#include "calibration.h"

#define history_item_count (100000)

namespace mru {


template<class Device, typename FT=DefaultFT>
struct Chip {
  virtual std::string chip_name() { return "unknown"; }
  virtual void initialize(const std::string& calibration_file="") {
    calibration_ = load_calibration(calibration_file, chip_name());
  }
  void initialize(const boost::filesystem::path& calibration_file) {
    initialize(calibration_file.string());
  }
  virtual void poll() = 0;
  virtual void finalize() = 0;
  const Sample<FT>& data() const { return data_; }
  const Samples<FT>& history() const { return history_; }
  int id() { return id_; }
  int version() { return version_; }
  int status() { return status_; }
  Chip(typename Device::Bus_type& bus, const int address, bool little_endian):
      device_(bus, address, little_endian), calibration_(), data_(), history_(),
      id_(0), version_(0), status_(0) {}
protected:
  Chip& push_sample(const Sample<FT>& sample) {
    data_ = sample;
    history_.push_back(data_);
    trim_history();
    return *this;
  }
  Chip& push_sample(Sample<FT>&& sample) {
    data_ = sample;
    history_.push_back(data_);
    trim_history();
    return *this;
  }
  void set_id(const int value) { id_ = value; }
  void set_version(const int value) { version_ = value; }
  void set_status(const int value) { status_ = value; }
  Device& device() { return device_; }
  Calibration<FT>& calibration() { return calibration_; }
private:
  Device device_;
  Calibration<FT> calibration_;
  Sample<FT> data_;
  Samples<FT> history_;
  int id_;
  int version_;
  int status_;
  void trim_history() {
    while (history_.size() > history_item_count) {
      history_.pop_front();
    }
  }
};

template<class Device, typename FT=DefaultFT>
struct HMC5843T: public Chip<Device> {

  static constexpr int default_address = 0x1E;

  static constexpr uint8_t reg_config_a = 0x00;
  enum Reg_config_a_rate: uint8_t {
    reg_config_a_0_5hz,
    reg_config_a_1hz,
    reg_config_a_2hz,
    reg_config_a_5hz,
    reg_config_a_10hz,
    reg_config_a_20hz,
    reg_config_a_50hz
  };
  static constexpr int reg_config_a_rate_shift = 2;

  static constexpr uint8_t reg_config_a_nobias = 0x00;

  static constexpr uint8_t reg_config_b = 0x01;
  enum Reg_config_b_gain: uint8_t {
    reg_config_b_0_7g,
    reg_config_b_1_0g,
    reg_config_b_1_5g,
    reg_config_b_2_0g,
    reg_config_b_3_2g,
    reg_config_b_3_8g,
    reg_config_b_4_5g
  };
  static constexpr int reg_config_b_gain_shift = 5;
  static constexpr Scalar<FT> gain_values[] = {
    1620,
    1300,
    970,
    780,
    530,
    460,
    390,
    280
  };

  static constexpr uint8_t reg_mode = 0x02;
  enum Reg_mode: uint8_t {
    reg_mode_continuous,
    reg_mode_single,
    reg_mode_idle,
    reg_mode_sleep
  };

  static constexpr uint8_t reg_data = 0x03;
  static constexpr uint8_t reg_status = 0x09;
  static constexpr uint8_t reg_id_a = 0x0A;
  static constexpr uint8_t reg_id_b = 0x0B;
  static constexpr uint8_t reg_id_c = 0x0C;

  virtual std::string chip_name() { return "hmc5843"; }
  virtual void initialize(const std::string& calibration_file="") {
    Chip<Device>::initialize(calibration_file);
    // 10Hz output, no bias
    this->device().write_byte(reg_config_a, reg_config_a_nobias | reg_config_a_10hz);
    // 1 Gauss range
    this->device().write_byte(reg_config_b, 0x20);

    set_id(
        this->device()->read_byte(reg_id_a) << 16 +
        this->device()->read_byte(reg_id_b) << 8 +
        this->device()->read_byte(reg_id_c));

    // Continuous data aquisition
    this->device().write_byte(reg_mode, reg_mode_continuous);
  }
  using Chip<Device>::initialize;
  virtual void poll() {
    //Byte ready = this->device().read_byte(0x09) & 0x01;
    //if (ready) {
    Words words = this->device().read_words(0x03, 3);

    auto point = Point<FT>{
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[0])),
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[1])),
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[2]))};

    //this->push_sample(Sample<FT>(point, 0);
    //}
  }
  virtual void finalize() {
    // Put device to sleep
    this->device().write_byte(reg_mode, reg_mode_sleep);
  }

  void set_output_rate(Reg_config_a_rate rate) {
  }
  HMC5843T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, false) {}
  HMC5843T(typename Device::Bus_type& bus): Chip<Device>(bus, default_address, false) {}
};

typedef HMC5843T<I2C_device> HMC5843;

template<class Device, typename FT=DefaultFT>
struct HMC5883T: public HMC5843T<Device> {
  static constexpr int default_address = 0x1E;
  virtual std::string chip_name() { return "hmc5883"; }
  HMC5883T(typename Device::Bus_type& bus, const int address): HMC5843T<Device>(bus, address) {}
  HMC5883T(typename Device::Bus_type& bus): HMC5843T<Device>(bus, default_address) {}
};

typedef HMC5883T<I2C_device> HMC5883;

template<class Device, typename FT=DefaultFT>
struct ADXL345T: public Chip<Device> {
  static constexpr int default_address = 0x53;
  virtual std::string chip_name() { return "adxl345"; }
  virtual void initialize(const std::string& calibration_file="") {
    Chip<Device>::initialize(calibration_file);
    // Clear the sleep bit (when it was set)
    this->device().write_byte(0x2D, 0x00);
    // Enable measure bit (get out of standby)
    this->device().write_byte(0x2D, 0x08);
    // Set FULL_RES bit for full resolution on all g scales
    // and the maximum g scale -> +-16g (why would you want to reduce this??)
    // scale is 4mg/bit
    this->device().write_byte(0x31, 0x0B);
  }
  using Chip<Device>::initialize;
  virtual void poll() {
    Words words = this->device().read_words(0x32, 3);
    auto point = Point<FT>{
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[0])),
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[1])),
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[2]))};
    //this->push_sample(Sample<FT>
  }
  virtual void finalize() {
    // Disable measure bit (set to standby)
    this->device().write_byte(0x2D, 0x00);
    // Put the device to sleep
    this->device().write_byte(0x2D, 0x07);
  }
  ADXL345T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, true) {}
  ADXL345T(typename Device::Bus_type& bus): Chip<Device>(bus, default_address, true) {}
};

typedef ADXL345T<I2C_device> ADXL345;

template<class Device, typename FT=DefaultFT>
struct BMA180T: public Chip<Device> {
  static constexpr int default_address = 0x40;  // alternative 0x41
  virtual std::string chip_name() { return "bma180"; }
  virtual void initialize(const std::string& calibration_file="") {
    Chip<Device>::initialize(calibration_file);

    // Start by soft resetting the device
    this->device().write_byte(0x10, 0xB6);
    // Wait a little bit for the device to come up
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Disable wake up and sleep mode (bit 0 and 1), enable image write (bit 4)
    this->device().write_byte(0x0D, 0x11);
    // ... more no wake up: bit 0
    this->device().write_byte(0x34, 0x80);

    // Disable offsets
    this->device().write_byte(0x0E, 0x00);

    Byte reg35 = this->device().read_byte(0x35);
    // Set range to -4/+4g: 0.5mg/bit
    this->device().write_byte(0x35, (reg35 & 0xF1) | 0x08);

    // Put in ultra low noise mode
    this->device().write_byte(0x30, 0x01);
    // set filter range to 20Hz (defaults to 1200Hz)
    this->device().write_byte(0x20, 0x10);

    // Disable wake up mode and sleep (bit 0 and 1), disable image write (bit 4)
    this->device().write_byte(0x0D, 0x01);

    // Get chip information
    this->set_id(this->device().read_byte(0x00));
    this->set_version(this->device().read_byte(0x01));
  }
  using Chip<Device>::initialize;
  virtual void poll() {
    Words xyz = this->device().read_words(0x02, 3);
    auto temp = static_cast<int8_t>(this->device().read_byte(0x08));
    // The 0 and 1 bits are shifted out (the values are only 14 bit)
    auto x = static_cast<int16_t>(xyz[0]) >> 2;
    auto y = static_cast<int16_t>(xyz[1]) >> 2;
    auto z = static_cast<int16_t>(xyz[2]) >> 2;
    auto point = Point<FT>{
        static_cast<Scalar<FT> >(x),
        static_cast<Scalar<FT> >(y),
        static_cast<Scalar<FT> >(z)};
    auto tempf = static_cast<Scalar<FT> >(temp);

    //    this->calibration()
    //this->push_sample(sample);
  }
  virtual void finalize() {
    // Put the device to sleep
    this->device().write_byte(0x0D, 0x02);
  }
  BMA180T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, true) {}
  BMA180T(typename Device::Bus_type& bus): Chip<Device>(bus, default_address, true) {}
};

typedef BMA180T<I2C_device> BMA180;

template<class Device, typename FT=DefaultFT>
struct ITG3200T: public Chip<Device> {
  static constexpr int default_address = 0x68;

  virtual std::string chip_name() { return "itg3200"; }
  virtual void initialize(const std::string& calibration_file="") {
    Chip<Device>::initialize(calibration_file);
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
  using Chip<Device>::initialize;
  virtual void poll() {
    Words words = this->device().read_words(0x1B, 4);
    auto gyr = Point<FT>{
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[1])),
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[2])),
        static_cast<Scalar<FT> >(static_cast<int16_t>(words[3]))};
    auto temp = static_cast<Scalar<FT> >(static_cast<int16_t>(words[0]));

    //    this->calibration()
    //this->push_sample(sample);
  }
  virtual void finalize() {
    // Put to sleep and select internal oscillator as clock
    this->device().write_byte(0x3E, 0x40);
  }
  ITG3200T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, false) {}
  ITG3200T(typename Device::Bus_type& bus): Chip<Device>(bus, default_address, false) {}
};

typedef ITG3200T<I2C_device> ITG3200;

template<class Device, typename FT=DefaultFT>
struct ITG3205T: public ITG3200T<Device> {
  static constexpr int default_address = 0x68; // alternative 0x69: pin 9 high
  virtual std::string chip_name() { return "itg3205"; }
  ITG3205T(typename Device::Bus_type& bus, const int address): ITG3200T<Device>(bus, address) {}
  ITG3205T(typename Device::Bus_type& bus): ITG3200T<Device>(bus, default_address) {}
};

typedef ITG3205T<I2C_device> ITG3205;

template<class Device, typename FT=DefaultFT>
struct BMP085T: public Chip<Device, FT> {
  static constexpr int default_address = 0x77;
  virtual std::string chip_name() { return "bmp085"; }
  virtual void initialize(const std::string& calibration_file="") {
    Chip<Device, FT>::initialize(calibration_file);
    // Read calibration data from EEPROM
    Words words = this->device().read_words(0xAA, 11);
    set_calibration_data(words);
  }
  using Chip<Device, FT>::initialize;
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


      //this->push_sample(
      //  Sample<FT>(Point<FT>(
      //      0, 0, static_cast<Scalar<FT> >(pressure)),
      //      static_cast<Scalar<FT> >(temp_)));
    }
    loop_count_++;
  }
  virtual void finalize() {
  }
  BMP085T(typename Device::Bus_type& bus, const int address, const int oss):
        Chip<Device>(bus, address, false), loop_count_(0), oss_(oss) {}
  BMP085T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, false), loop_count_(0), oss_(3) {}
  BMP085T(typename Device::Bus_type& bus): Chip<Device>(bus, default_address, false), loop_count_(0), oss_(3) {}
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

template <class Device, typename FT>
void BMP085T<Device, FT>::set_calibration_data(const Words& calibration_data)
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

template <class Device, typename FT>
int32_t BMP085T<Device, FT>::eval_temp(const Word raw_temp)
{
  int32_t result = static_cast<int32_t>(raw_temp);
  int32_t x1 = ((result - ac6_) * ac5_) >>  15;
  int32_t x2 = (static_cast<int32_t>(mc_) << 11) / (x1 + md_);
  b5_ = x1 + x2;
  result = (b5_ + 8) >> 4;
  return result;
}

template <class Device, typename FT>
int32_t BMP085T<Device, FT>::eval_pressure(const int32_t raw_pressure)
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


template <class Device, typename FT=DefaultFT>
struct BNO055T: public Chip<Device> {
  static constexpr int default_address = 0x28;
  virtual std::string chip_name() { return "bno055"; }
  virtual void initialize(const std::string& calibration_file="") {
    Chip<Device>::initialize(calibration_file);

    // Switch the chip to config mode
    this->device()->write_byte(0x3D, 0x00);

    // The switch takes a little while
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    // Set/switch to normal power mode
    this->device()->write_byte(0x3E, 0x00);

    // Select units: m/s^2, rad/s, rad, celcius
    this->device()->write_byte(0x3B, 0x06);

    // Axes configuration: z axes down
    this->device()->write_byte(0x41, 0x24);
    this->device()->write_byte(0x42, 0x03);

    // Switch chip to fusion mode: NDOF
    this->device()->write_byte(0x3D, 0x0C);

    // The switch takes a little while
    std::this_thread::sleep_for(std::chrono::milliseconds(15));

    // Get chip information
    this->set_id(this->device().read_byte(0x00));
    this->set_version((this->device().read_byte(0x05) << 8) + this->device().read_byte(0x04));
  }
  using Chip<Device>::initialize;
  virtual void poll() {
    int status = this->device()->read_word(0x39);
    // Expected the "sensor fusion algorithm running" bit to be set. Toggle that so status becomes 0
    // when everything is as expected. Any bits set either indicate an unexpected state or an error
    this->set_status(status ^ 0x0020);

    calibration_ = this->device()->read_byte(0x35);
  }
  virtual void finalize() {
    // Switch the chip to config mode
    this->device()->write_byte(0x3D, 0x00);

    // The switch takes a little while
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    // Suspend the chip
    this->device()->write_byte(0x3E, 0x02);
  }
  BNO055T(typename Device::Bus_type& bus, const int address, const int oss):
        Chip<Device>(bus, address, false), calibration_(0) {}
  BNO055T(typename Device::Bus_type& bus, const int address): Chip<Device>(bus, address, false), calibration_(0) {}
  BNO055T(typename Device::Bus_type& bus): Chip<Device>(bus, default_address, false), calibration_(0) {}
  int get_calibrarion() {
    return calibration_;
  }
private:
  int calibration_;
};

typedef BNO055T<I2C_device> BNO055;

} //namespace mru

#endif

// vim: syntax=cpp : shiftwidth=2 : tabstop=2 : expandtab :
