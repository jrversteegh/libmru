
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
