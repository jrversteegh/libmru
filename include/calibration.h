#ifndef NINEDOF_CALIBRATION_
#define NINEDOF_CALIBRATION_

#define accel_x_fact ( 3.78 * 0.00981)
#define accel_x_offs (-0.15)
#define accel_y_fact (-3.78 * 0.009810)
#define accel_y_offs ( 0.15)
#define accel_z_fact (-3.92 * 0.009810)
#define accel_z_offs (-1.22)

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

#define history_item_count (100000)

#endif
