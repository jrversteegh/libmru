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

ostream& operator<<(ostream& o, const Vector& v) {
  o << fixed << setprecision(3);
  o << setw(10) << v.x();
  o << setw(10) << v.y();
  o << setw(10) << v.z();
  o << setw(10) << sqrt(v.squared_length());
  return o;
}

int main()
{
  cout << "9 degrees stick" << endl;
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

    cout << "     Compass: " << *compass.raw() << endl;
    cout << "Acceleration: " << *acceleration.raw() << endl;
    cout << "        Gyro: " << *gyro.raw() << endl;
    this_thread::sleep_for(chrono::milliseconds(997));
  }

  compass.finalize();
  acceleration.finalize();
  gyro.finalize();
  return 0;
}
