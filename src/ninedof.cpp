#include <iostream>

#include "../include/i2cbus.h"

int main()
{
  std::cout << "9 degrees stick" << std::endl;
  I2CBus bus(0);
  return 0;
}
