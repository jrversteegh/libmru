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


namespace mru {

const int hmc5843_address = 0x1E;
const int hmc5883_address = 0x1E;
const int adxl345_address = 0x53;
const int bma180_address = 0x40;  // alternative 0x41
const int itg3200_address = 0x68;
const int itg3205_address = 0x68; // alternative 0x69: pin 9 high
const int bmp085_address = 0x77;
const int bno055_address = 0x28;


}  // namespace mru


/* vim: set sw=2 ts=2 et: */
