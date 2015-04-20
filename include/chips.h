#ifndef NINEDOF_CHIPS_
#define NINEDOF_CHIPS_

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

struct ADXL345: Chip {
  virtual void initialize();
  virtual void poll();
};

struct HMC5843: Chip {
  virtual void initialize();
  virtual void poll();
};

struct ITG3200: Chip {
  virtual void initialize();
  virtual void poll();
};

#endif
