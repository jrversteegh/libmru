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

#define adxl345_x_fact ( 3.78 * 0.009810)
#define adxl345_x_offs (-0.15)
#define adxl345_y_fact (-3.78 * 0.009810)
#define adxl345_y_offs ( 0.15)
#define adxl345_z_fact (-3.95 * 0.009810)
#define adxl345_z_offs (-1.05)

// Default range setting is 2g -> .25mg/LSB
#define bma180_x_fact (0.25 * 0.009810)
#define bma180_x_offs (0.0)
#define bma180_y_fact (0.25 * 0.009810)
#define bma180_y_offs (0.0)
#define bma180_z_fact (0.25 * 0.009810)
#define bma180_z_offs (0.0)
#define bma180_temp_fact (1.0)
#define bma180_temp_offs (0.0)

#define hmc5843_x_fact ( 0.085)
#define hmc5843_x_offs ( 0.0)
#define hmc5843_y_fact ( 0.085)
#define hmc5843_y_offs ( 0.0)
#define hmc5843_z_fact ( 0.085)
#define hmc5843_z_offs ( 0.0)

#define itg3200_x_fact ( 0.001214)
#define itg3200_x_offs ( 0.00)
#define itg3200_y_fact (-0.001214)
#define itg3200_y_offs ( 0.00)
#define itg3200_z_fact (-0.001214)
#define itg3200_z_offs ( 0.00)
#define itg3200_temp_fact ( 0.003571)
#define itg3200_temp_offs ( 81.0 )

#define itg3205_x_fact ( 0.001214)
#define itg3205_x_offs ( 0.00)
#define itg3205_y_fact (-0.001214)
#define itg3205_y_offs ( 0.00)
#define itg3205_z_fact (-0.001214)
#define itg3205_z_offs ( 0.00)
#define itg3205_temp_fact ( 0.003571)
#define itg3205_temp_offs ( 81.0 )

#define bmp085_pressure_fact ( 1.0)
#define bmp085_pressure_offs ( 0.0)
#define bmp085_temp_fact ( 1.0)
#define bmp085_temp_offs ( 0.0)

#define history_item_count (100000)

#endif
