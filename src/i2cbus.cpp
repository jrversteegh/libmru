extern "C" {
#include <linux/i2c-dev.h>
#include <fcntl.h>
}

#include <cerrno>
#include <string>
#include <cstring>
#include <cstdio>

#include "../include/errors.h"
#include "../include/i2cbus.h"

const int compass_address = 0x1e;
const int acceleration_address = 0x53;
const int gyro_address = 0x68;

void I2CBus::open_bus_()
{
  char filename[16];
  
  snprintf(filename, 15, "/dev/i2c-%d", busno_);
  file_ = open(filename, O_RDWR);
  if (file_ < 0) {
    throw Error("Failed to open I2C bus.", errno);
  }
}

void I2CBus::close_bus_()
{
  if (file_ > 0) {
    close(file_);
  }
}

