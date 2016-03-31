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


namespace ninedof {

const int hmc5843_address = 0x1E;
const int hmc5883_address = 0x1E;
const int adxl345_address = 0x53;
const int hma180_address = 0x40;  // alternative 0x41
const int itg3200_address = 0x68;
const int itg3205_address = 0x68; // alternative 0x69: pin 9 high
const int bmp085_address = 0x77;


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
  int32_t b3 = ((ac1_ * 4 + x3) << 3 + 2) / 4;
  x1 = (ac3_ * b6) >> 13;
  x2 = (b1_ * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) / 4;
  int32_t b4 = ac4_ * (uint32_t)(x3 + 32768) >> 15;
  int32_t b7 = (uint32_t)raw_pressure - b3 * (50000 >> 3);
  if (b7 < 0x80000000) {
    result = (b7 * 2) / b4;
  } 
  else {
    result = (b7 / b4) * 2;
  }
  x1 = (result >> 8) * (result >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * result) >> 16;
  result = result + (x1 + x2 + 3791) >> 4;
  return result;
}

}  // namespace ninedof


/* vim: set sw=2 ts=2 et: */
