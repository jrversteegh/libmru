#include "../include/chips.h"
#include "../include/calibration.h"

#include <chrono>
#include <thread>

namespace ninedof {

const int compass_address = 0x1e;
const int acceleration_address = 0x53;
const int gyro_address = 0x68;

void ADXL345::initialize()
{
  // Clear the sleep bit (when it was set)
  device_.write_byte(0x2D, 0x00);
  // Enable measure bit (get out of standby)
  device_.write_byte(0x2D, 0x08);
  // Set FULL_RES bit for full resolution on all g scales
  // and the maximum g scale -> +-16g (why would you want to reduce this??)
  // scale is 4mg/bit
  device_.write_byte(0x31, 0x0B);
}

void ADXL345::poll()
{
  Words words = device_.read_words(0x32, 3);
  raw_ = Vector(
      static_cast<Number_type>(static_cast<int16_t>(words[0])) * accel_x_fact + accel_x_offs,
      static_cast<Number_type>(static_cast<int16_t>(words[1])) * accel_y_fact + accel_y_offs,
      static_cast<Number_type>(static_cast<int16_t>(words[2])) * accel_z_fact + accel_z_offs 
  );
}

void ADXL345::finalize()
{
  // Disable measure bit (set to standby)
  device_.write_byte(0x2D, 0x00);
  // Put the device to sleep
  device_.write_byte(0x2D, 0x07);
}

void HMC5843::initialize()
{
  // 20Hz output, no bias
  device_.write_byte(0x00, 0x14);
  // 1 Gauss range
  device_.write_byte(0x01, 0x20);
  // Continuous data aquisition
  device_.write_byte(0x02, 0x00);

}

void HMC5843::poll()
{
  Byte ready = device_.read_byte(0x09) & 0x01;
  if (ready) {
    Words words = device_.read_words(0x03, 3);
    raw_ = Vector(
        static_cast<Number_type>(static_cast<int16_t>(words[1])) * comp_x_fact + comp_x_offs,
        static_cast<Number_type>(static_cast<int16_t>(words[0])) * comp_y_fact + comp_y_offs,
        static_cast<Number_type>(static_cast<int16_t>(words[2])) * comp_z_fact + comp_z_offs
    );
  }
}

void HMC5843::finalize()
{
  // Put device to sleep
  device_.write_byte(0x02, 0x03);
}

void ITG3200::initialize()
{
  // First reset the chip
  device_.write_byte(0x3E, 0x80);
  // Wait a little for it to come back up
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // Sample at 20Hz: 1kHz / 50 (49 + 1)
  device_.write_byte(0x15, 0x31);
  // Select range: 0x03 << 3 and low pass filter of 10Hz: 0x05
  device_.write_byte(0x16, 0x1D);
  // Get out of sleep and select PLL with X Gyro reference as clock
  device_.write_byte(0x3E, 0x01);
}

void ITG3200::poll()
{
  Words words = device_.read_words(0x1B, 4);
  temp_ = static_cast<Number_type>(words[0]);
  raw_ = Vector(
      static_cast<Number_type>(static_cast<int16_t>(words[1])) * gyro_x_fact + gyro_x_offs,
      static_cast<Number_type>(static_cast<int16_t>(words[2])) * gyro_y_fact + gyro_y_offs,
      static_cast<Number_type>(static_cast<int16_t>(words[3])) * gyro_z_fact + gyro_z_offs
  ); 
}

void ITG3200::finalize()
{
  // Put to sleep and select internal oscillator as clock
  device_.write_byte(0x3E, 0x40);
}

}  // namespace ninedof
