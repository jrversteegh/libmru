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
 * Defines some default calibration constants
 */

#ifndef NINEDOF_CALIBRATION_
#define NINEDOF_CALIBRATION_

#define accel_x_fact ( 3.78 * 0.009810)
#define accel_x_offs (-0.15)
#define accel_y_fact (-3.78 * 0.009810)
#define accel_y_offs ( 0.15)
#define accel_z_fact (-3.95 * 0.009810)
#define accel_z_offs (-1.05)

#define comp_x_fact ( 0.085)
#define comp_x_offs ( 0.0)
#define comp_y_fact ( 0.085)
#define comp_y_offs ( 0.0)
#define comp_z_fact ( 0.085)
#define comp_z_offs ( 0.0)

#define gyro_x_fact ( 0.001214)
#define gyro_x_offs ( 0.00)
#define gyro_y_fact (-0.001214)
#define gyro_y_offs ( 0.00)
#define gyro_z_fact (-0.001214)
#define gyro_z_offs ( 0.00)
#define gyro_temp_fact ( 0.003571)
#define gyro_temp_offs ( 81.0 )

#define pressure_fact ( 1.0)
#define pressure_offs ( 0.0)
#define temp_fact ( 1.0)
#define temp_offs ( 0.0)

#define history_item_count (100000)

#endif
