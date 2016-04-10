#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cmath>

#include <signal.h>

#include "../include/types.h"
#include "../include/i2cbus.h"
#include "../include/chips.h"
#include "../include/errors.h"


using namespace ninedof;
using namespace std;
using namespace std::chrono;

ostream& operator<<(ostream& o, const Vector& v) {
  o << fixed << setprecision(3);
  o << setw(8) << v.x();
  o << setw(8) << v.y();
  o << setw(8) << v.z();
  o  << " :" << setw(8) << sqrt(v.squared_length());
  return o;
}

bool volatile quit = false;

void signal_handler(int sig) {
  quit = true;
}

int main(int argc, char* argv[])
{
  signal(SIGINT, &signal_handler);
  cout << "Sparkfun 9 DOF stick" << endl;
  cout << "Press 'CTRL-C' to quit." << endl;
  cout << "Set \"NINEDOF_SAMPLE_RATE\" for other rates than 1Hz." << endl;
  cout << "Set \"NINEDOF_I2C_BUS\" for i2c bus other than 0." << endl;
  cout << "Time, Compass, Acceleration, Gyro, Gyro Temp." << endl;

  char *i2c_bus = getenv("NINEDOF_I2C_BUS");
  int busno = 0;
  if (i2c_bus != 0) {
    busno = atoi(i2c_bus);
  }
  try {
    I2C_bus bus(busno);

    boost::filesystem::path calibration_file = argv[0];
    calibration_file.remove_filename();
    calibration_file /= "calibration/9dof.ini";

    HMC5843 compass(bus);
    ADXL345 acceleration(bus);
    ITG3200 gyro(bus);

    compass.initialize(calibration_file);
    acceleration.initialize(calibration_file);
    gyro.initialize(calibration_file);

    int wait = 1000;
    char *sample_rate = getenv("NINEDOF_SAMPLE_RATE");
    if (sample_rate != 0) {
      wait = 1000 / atoi(sample_rate);
    }
    system_clock::time_point next_time = high_resolution_clock::now();

    while (!quit) {
      next_time += milliseconds(wait);
      this_thread::sleep_until(next_time);

      compass.poll();
      acceleration.poll();
      gyro.poll();

      cout << 
        compass.data().time << " ## " <<
        compass.data().vector << " ## " <<
        acceleration.data().vector << " ## " <<
        gyro.data().vector << " ## " <<
        setw(8) << gyro.data().value <<
        endl;
    }

    compass.finalize();
    acceleration.finalize();
    gyro.finalize();
    cout << "Finalized." << endl;
    return 0;
  } catch (const Error& e) {
    cerr << "=========================" << endl;
    cerr << e.get_message() << endl;
    cerr << "=========================" << endl;
    return 1;
  }
}
