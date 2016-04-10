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
 * Defines some default calibration constants
 */

#ifndef NINEDOF_CALIBRATION_
#define NINEDOF_CALIBRATION_

#include <boost/filesystem.hpp>

#include "types.h"

namespace ninedof {

extern Calibration load_calibration(const std::string& filename, const std::string& section);
extern Calibration load_calibration(const boost::filesystem::path& filename, const std::string& section);
extern void save_calibration(const std::string& filename, const std::string& section,
                             const Calibration& calibration);
extern void save_calibration(const boost::filesystem::path& filename, const std::string& section,
                             const Calibration& calibration);

}  // namespace ninedof

#endif
