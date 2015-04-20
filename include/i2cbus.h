#include <cstdint>
#include <vector>

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
  const int get_file() const { return file_; }
  const int get_selected_address() const { return selected_address_; }
  void select_address(const int address);
private:
  int file_;
  int busno_;
  static int refcount_;
  static int selected_address_;
  void open_bus_();
  void close_bus_();
};

typedef uint8_t Byte;
typedef uint16_t Word;
typedef std::vector<Byte> Bytes;
typedef std::vector<Word> Words;
typedef Bytes::iterator Byte_i;
typedef Words::iterator Word_i;

struct I2CDevice {
  I2CDevice(I2CBus& bus, int address, bool little_endian=true): 
      bus_(bus), address_(address), little_endian_(little_endian) {}
  I2CDevice(const I2CDevice& device): 
      bus_(device.bus_), address_(device.address_), little_endian_(device.little_endian_) {}
  ~I2CDevice() {}
  void write_byte(const int offset, const Byte value) const;
  void write_bytes(const int offset, const Bytes& values) const;
  Byte read_byte(const int offset) const;
  Bytes read_bytes(const int offset, const int count) const;
  void write_word(const int offset, const Word value) const;
  void write_words(const int offset, const Words& values) const;
  Word read_word(const int offset) const;
  Words read_words(const int offset, const int count) const;
private:
  int address_;
  bool little_endian_;
  I2CBus& bus_;
  void select_() const {
    if (bus_.get_selected_address() != address_) {
      bus_.select_address(address_);
    }
  }
};
