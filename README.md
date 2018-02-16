# libmru

C++ code for Sparkfun 9 degrees of freedom sensor stick SEN-10321-9DOF. Should also work with newer SEN-10724 with no or minor modifications. The newer stick has an HMC5883L compass instead of an HMC5843. The ADXL345 and ITG3200 are the same.

The I2C dev interface is required for this package: 
https://www.kernel.org/doc/Documentation/i2c/dev-interface
The header for this: i2c-dev.h can be found in package libi2c-dev on debian based systems.

Further more this packages depends on CGAL. Install libcgal-dev on debian based systems.

`test -x ./configure || ./bootstrap && mkdir build && cd build && ../configure`  
`make && make test && make install`  

