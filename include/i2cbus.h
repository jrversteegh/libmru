struct I2CBus {
  I2CBus(int busno): file_(0), busno_(busno) {
    open_bus_();
    ++refcount_;
  }
  I2CBus(const I2CBus& bus): file_(bus.file_), busno_(bus.busno_) {
    ++refcount_;
  }
  ~I2CBus() {
    --refcount_;
    if (refcount_ == 0) {
      close_bus_();
    }
  }
  const int get_bus() const { return busno_; }
private:
  int file_;
  int busno_;
  int refcount_;
  void open_bus_();
  void close_bus_();
};

struct I2CDevice {
  I2CDevice(const I2CBus& bus, int address) {}
  I2CDevice(const I2CDevice& device) {}
  ~I2CDevice() {}
};
