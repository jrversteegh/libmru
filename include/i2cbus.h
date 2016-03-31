/* 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/

/** \file
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 * Declares types for accessing devices on the I2C bus
 */

#ifndef NINEDOF_I2CBUS_
#define NINEDOF_I2CBUS_

#include <cstdint>
#include <vector>

#include "errors.h"

namespace ninedof {

typedef uint8_t Byte;
typedef uint16_t Word;
typedef std::vector<Byte> Bytes;
typedef std::vector<Word> Words;
typedef std::vector<int> Ints;
typedef Bytes::iterator Byte_i;
typedef Words::iterator Word_i;
typedef Ints::iterator Int_i ;
typedef Bytes::const_iterator Byte_ci;
typedef Words::const_iterator Word_ci;
typedef Ints::const_iterator Int_ci ;

struct I2C_bus {
  I2C_bus(int busno): file_(0), busno_(busno) {
    open_bus_();
  }
  I2C_bus(const I2C_bus& bus): file_(bus.file_), busno_(bus.busno_) {
    open_bus_();
  }
  ~I2C_bus() {
    close_bus_();
  }
  const int get_selected_address() const;
  const int get_bus() const { return busno_; }
  const int get_file() const { return file_; }
  void select_address(const int address);
  Ints scan(); 
private:
  int file_;
  int busno_;
  void open_bus_();
  void close_bus_();
};

struct I2C_device {
  typedef I2C_bus Bus_type;
  I2C_device(Bus_type& bus, int address, bool little_endian=true): 
      bus_(bus), address_(address), little_endian_(little_endian) {}
  I2C_device(const I2C_device& device): 
      bus_(device.bus_), address_(device.address_), little_endian_(device.little_endian_) {}
  void write_byte(const int offset, const Byte value) const;
  void write_bytes(const int offset, const Bytes& values) const;
  Byte read_byte(const int offset) const;
  Bytes read_bytes(const int offset, const int count) const;
  void write_word(const int offset, const Word value) const;
  void write_words(const int offset, const Words& values) const;
  Word read_word(const int offset) const;
  Words read_words(const int offset, const int count) const;
private:
  Bus_type& bus_;
  int address_;
  bool little_endian_;
  void select_() const {
    if (bus_.get_selected_address() != address_) {
      bus_.select_address(address_);
    }
  }
};
 
}  // namespace ninedof

#endif
