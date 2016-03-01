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
 * Defines types for interfacing with the sensor ASICS
 */

#include "../include/chips.h"
#include "../include/calibration.h"

#include <chrono>
#include <thread>

namespace ninedof {

const int hmc5843_address = 0x1E;
const int hmc5883_address = 0x1E;
const int adxl345_address = 0x53;
const int hma180_address = 0x40;  // alternative 0x41
const int itg3200_address = 0x68;
const int itg3205_address = 0x68; // alternative 0x69: pin 9 high
const int hmp085_address = 0x77;

void HMC5843::initialize()
{
  // 20Hz output, no bias
  device().write_byte(0x00, 0x14);
  // 1 Gauss range
  device().write_byte(0x01, 0x20);
  // Continuous data aquisition
  device().write_byte(0x02, 0x00);
}

void HMC5843::poll()
{
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

void HMC5843::finalize()
{
  // Put device to sleep
  device().write_byte(0x02, 0x03);
}

void ADXL345::initialize()
{
  // Clear the sleep bit (when it was set)
  device().write_byte(0x2D, 0x00);
  // Enable measure bit (get out of standby)
  device().write_byte(0x2D, 0x08);
  // Set FULL_RES bit for full resolution on all g scales
  // and the maximum g scale -> +-16g (why would you want to reduce this??)
  // scale is 4mg/bit
  device().write_byte(0x31, 0x0B);
}

void ADXL345::poll()
{
  Words words = device().read_words(0x32, 3);
  Sample_t sample = Sample_t(Vector_t(
      static_cast<Value_t>(static_cast<int16_t>(words[0])) * accel_x_fact + accel_x_offs,
      static_cast<Value_t>(static_cast<int16_t>(words[1])) * accel_y_fact + accel_y_offs,
      static_cast<Value_t>(static_cast<int16_t>(words[2])) * accel_z_fact + accel_z_offs 
  ));
  push_sample(sample);
}

void ADXL345::finalize()
{
  // Disable measure bit (set to standby)
  device().write_byte(0x2D, 0x00);
  // Put the device to sleep
  device().write_byte(0x2D, 0x07);
}

void BMA180::initialize()
{
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

void BMA180::poll()
{
  Words xyz = device().read_words(0x02, 3);
  Byte temp = device().read_byte(0x08h);
  Sample_t sample = Sample_t(Vector_t(
      static_cast<Value_t>(static_cast<int16_t>(xyz[0] >> 2)) * accel_x_fact + accel_x_offs,
      static_cast<Value_t>(static_cast<int16_t>(xyz[1] >> 2)) * accel_y_fact + accel_y_offs,
      static_cast<Value_t>(static_cast<int16_t>(xyz[2] >> 2)) * accel_z_fact + accel_z_offs 
  ), static_cast<Value_t>(static_cast<int8_t>(temp)) * gyro_temp_fact + gyro_temp_offs);
  push_sample(sample);
}

void BMA180::finalize()
{
  // Put the device to sleep
  device().write_byte(0x0D, 0x02);
}


void ITG3200::initialize()
{
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

void ITG3200::poll()
{
  Words words = device().read_words(0x1B, 4);
  Sample_t sample = Sample_t(Vector_t(
      static_cast<Value_t>(static_cast<int16_t>(words[1])) * gyro_x_fact + gyro_x_offs,
      static_cast<Value_t>(static_cast<int16_t>(words[2])) * gyro_y_fact + gyro_y_offs,
      static_cast<Value_t>(static_cast<int16_t>(words[3])) * gyro_z_fact + gyro_z_offs
    ), static_cast<Value_t>(static_cast<int16_t>(words[0])) * gyro_temp_fact + gyro_temp_offs
  ); 
  push_sample(sample);
}

void ITG3200::finalize()
{
  // Put to sleep and select internal oscillator as clock
  device().write_byte(0x3E, 0x40);
}


void BMP085::initialize()
{
  // Read calibration data from EEPROM
  Words words = device().read_words(0xAA, 11);
  int16_t ac1_ = static_cast<int16_t>words[0];
  int16_t ac2_ = static_cast<int16_t>words[1];
  int16_t ac3_ = static_cast<int16_t>words[2];
  uint16_t ac4_ = static_cast<uint16_t>words[3];
  uint16_t ac5_ = static_cast<uint16_t>words[4];
  uint16_t ac6_ = static_cast<uint16_t>words[5];
  int16_t b1_ = static_cast<int16_t>words[6];
  int16_t b2_ = static_cast<int16_t>words[7];
  int16_t mb_ = static_cast<int16_t>words[8];
  int16_t mc_ = static_cast<int16_t>words[9];
  int16_t md_ = static_cast<int16_t>words[10];
}

void BMP085::poll()
{
  if (loop_counter_ % 120 == 0) {
    loop_counter_ = 0;
    // Get temperature
    device().write_byte(0xF4, 0x2E);
  } else if (loop_counter_ == 1) {
    // Read temperature
    Byte raw_temp = device().read_byte(0xF6);
    _temp = eval_temp(raw_temp);
  } else if (loop_counter_ % 2 = 0) {
    // Read pressure 8 times oversampling: takes 25ms
    device().write_byte(0xF4, 0xF4)
  } else {
    Word raw_pressure = device().read_word(0xF6);
    int32_t pressure = eval_pressure(raw_pressure);
    Sample_t sample = Sample_t(Vector_t(
        static_cast<Value_t>(pressure) * pressure_fact + pressure_offs, 0, 0), 
        static_cast<Value_t>(temp_) * temp_fact + temp_offs); 
    push_sample(sample);
  }
  loop_counter_++;
}

void BMP085::finalize()
{
}

int32_t BMP085::eval_temp(const Word raw_temp)
{
  int32_t result = static_cast<int32_t>(raw_temp);
  int32_t x1 = ((result - ac6_) * ac5_) >>  15;
  int32_t x2 = (static_cast<int32_t>(mc_) << 11) / (x1 + md_);
  int32_t b5 = x1 + x2;
  result = (b5 + 8) >> 4;
  return result;
}

int32_t BMP085::eval_pressure(const int32_t raw_pressure)
{
}

}  // namespace ninedof


/* vim: set sw=2 ts=2 et: */
