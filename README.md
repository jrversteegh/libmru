# ninedof
C++ code for Sparkfun 9 degrees of freedom sensor stick SEN-10321-9DOF. Should also work with newer SEN-10724 with no or minor modifications. The newer stick has an HMC5883L compass instead of an HMC5843. The ADXL345 and ITG3200 are the same.

i2c-dev.h is required for this package, which can be found in package libi2c-dev on debian based systems.

`make`  
`make run`  
`make test`  

