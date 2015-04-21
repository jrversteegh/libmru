#include <iostream>

#include "../include/i2cbus.h"

#ifdef HASMAIN
int main()
{
  std::cout << "9 degrees stick" << std::endl;
  ninedof::I2CBus bus(0);
  return 0;
}
#endif
