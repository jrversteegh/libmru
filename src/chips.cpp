#include "../include/chips.h"

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
      static_cast<Number_type>(words[0]),
      static_cast<Number_type>(words[1]),
      static_cast<Number_type>(words[2])
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
}

void HMC5843::poll()
{
}

void HMC5843::finalize()
{
}

void ITG3200::initialize()
{
}

void ITG3200::poll()
{
}

void ITG3200::finalize()
{
}

}  // namespace ninedof
