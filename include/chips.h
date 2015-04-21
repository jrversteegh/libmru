#ifndef NINEDOF_CHIPS_
#define NINEDOF_CHIPS_

namespace ninedof {

extern const int compass_address;
extern const int acceleration_address;
extern const int gyro_address;

struct Chip {
  virtual void initialize() = 0;
  virtual void poll() = 0;
  float x() const { return 0; }
  float y() const { return 0; }
  float z() const { return 0; }
};

struct ADXL345: public Chip {
  virtual void initialize();
  virtual void poll();
};

struct HMC5843: public Chip {
  virtual void initialize();
  virtual void poll();
};

struct ITG3200: public Chip {
  virtual void initialize();
  virtual void poll();
};

} //namespace ninedof

#endif
