#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cmath>

#include "../include/types.h"
#include "../include/i2cbus.h"
#include "../include/chips.h"


using namespace ninedof;
using namespace std;

ostream& operator<<(ostream& o, const Vector_t& v) {
  o << fixed << setprecision(3);
  o << setw(8) << v.x();
  o << setw(8) << v.y();
  o << setw(8) << v.z();
  o  << " :" << setw(8) << sqrt(v.squared_length());
  return o;
}

int main()
{
  cout << "Sparkfun 9 DOF stick" << endl;
  cout << "Time, Compass, Acceleration, Gyro, Gyro Temp." << endl;
  I2CBus bus(0);

  HMC5843 compass(bus);
  ADXL345 acceleration(bus);
  ITG3200 gyro(bus);

  compass.initialize();
  acceleration.initialize();
  gyro.initialize();

  this_thread::sleep_for(chrono::milliseconds(1000));

  for (int i =0; i < 600; ++i) {
    compass.poll();
    acceleration.poll();
    gyro.poll();

    cout << "t,C,A,G,T: " << 
      compass.data().time << " ## " <<
      compass.data().vector << " ## " <<
      acceleration.data().vector << " ## " <<
      gyro.data().vector << " ## " <<
      gyro.temp() <<
      endl;
    this_thread::sleep_for(chrono::milliseconds(97));
  }

  compass.finalize();
  acceleration.finalize();
  gyro.finalize();
  return 0;
}
