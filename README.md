# libmru

C++ code for interfacing MRU's (Motion reference Units). Currently the following units are supported:
- 9 degrees of freedom sensor stick SEN-10321-9DOF. Should also work with newer SEN-10724 with no or minor modifications. The newer stick has an HMC5883L compass instead of an HMC5843. The ADXL345 and ITG3200 are the same.

The following chips are supported:
HMC5843 magnetic sensor
HMC5883 magnetic sensor
ADXL345 acceleration sensor
BMA180 acceleration sensor
ITG3200 gyroscope
ITG3205 gyroscope
BMP085 pressure sensor

The code should work on any linux system and is tested on Raspberry Pi and BeagleBone.

The I2C dev interface is required for this package: 
https://www.kernel.org/doc/Documentation/i2c/dev-interface
The header for this: i2c-dev.h can be found in package libi2c-dev on debian based systems.

Further more this packages depends on CGAL and several boost libraries. CppUnit is required for building tests.

Dependency installation on debian based systems:
apt install libi2c-dev libcgal-dev libboost-dev libboost-filesystem-dev libboost-date-time-dev libcppunit-dev.


`test -x ./configure || ./bootstrap && mkdir build && cd build && ../configure`  
`make && make test && make install`  

