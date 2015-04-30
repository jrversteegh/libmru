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
 * Declares types for error handling
 */
#ifndef NINEDOF_ERRORS_
#define NINEDOF_ERRORS_

#include <string> 
#include <sstream> 

namespace ninedof {

struct Error { 
  Error(const std::string message, int error = 0): message_(message), error_(error) {}; 
  std::string get_message() const { 
    std::ostringstream os(message_);
    os << " Error: " << error_; 
    return os.str();
  }
private: 
  std::string message_; 
  int error_;
};

}  // namespace ninedof

#endif
