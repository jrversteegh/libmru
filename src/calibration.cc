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
 * Defines functions for calibration value handling
 */

#include <fstream>
#include <utility>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "../include/calibration.h"

namespace mru {

Calibration load_calibration(const std::string& filename, const std::string& section)
{
   using namespace boost::property_tree;

   ptree pt;

   std::ifstream i_file(filename);
   if (i_file.good()) {
     ini_parser::read_ini(i_file, pt);
     i_file.close();
   }

   Calibration result;

   if (pt.get(section + ".x_factor", (Scalar)0) == 0 &&
       pt.get(section + ".y_factor", (Scalar)0) == 0 &&
       pt.get(section + ".z_factor", (Scalar)0) == 0) {
     Scalar xx_factor = pt.get(section + ".xx_factor", (Scalar)1.0);
     Scalar xy_factor = pt.get(section + ".xy_factor", (Scalar)0.0);
     Scalar xz_factor = pt.get(section + ".xz_factor", (Scalar)0.0);
     Scalar x_offset = pt.get(section + ".x_offset", (Scalar)0.0);
     Scalar yx_factor = pt.get(section + ".yx_factor", (Scalar)0.0);
     Scalar yy_factor = pt.get(section + ".yy_factor", (Scalar)1.0);
     Scalar yz_factor = pt.get(section + ".yz_factor", (Scalar)0.0);
     Scalar y_offset = pt.get(section + ".y_offset", (Scalar)0.0);
     Scalar zx_factor = pt.get(section + ".zx_factor", (Scalar)0.0);
     Scalar zy_factor = pt.get(section + ".zy_factor", (Scalar)0.0);
     Scalar zz_factor = pt.get(section + ".zz_factor", (Scalar)1.0);
     Scalar z_offset = pt.get(section + ".z_offset", (Scalar)0.0);
     Scalar v_factor = pt.get(section + ".v_factor", (Scalar)1.0);
     Scalar v_offset = pt.get(section + ".v_offset", (Scalar)0.0);
     result = Calibration(xx_factor, xy_factor, xz_factor, x_offset, 
                          yx_factor, yy_factor, yz_factor, y_offset,
                          zx_factor, zy_factor, zz_factor, z_offset,
                          v_factor, v_offset);
   } 
   else {
     Scalar x_factor = pt.get(section + ".x_factor", (Scalar)1.0);
     Scalar x_offset = pt.get(section + ".x_offset", (Scalar)0.0);
     Scalar y_factor = pt.get(section + ".y_factor", (Scalar)1.0);
     Scalar y_offset = pt.get(section + ".y_offset", (Scalar)0.0);
     Scalar z_factor = pt.get(section + ".z_factor", (Scalar)1.0);
     Scalar z_offset = pt.get(section + ".z_offset", (Scalar)0.0);
     Scalar v_factor = pt.get(section + ".v_factor", (Scalar)1.0);
     Scalar v_offset = pt.get(section + ".v_offset", (Scalar)0.0);
     result = Calibration(x_factor, x_offset, 
                          y_factor, y_offset,
                          z_factor, z_offset, 
                          v_factor, v_offset);
   }
   return result;
}

Calibration load_calibration(const boost::filesystem::path& filename, const std::string& section)
{
  return load_calibration(filename.string(), section);
}

void save_calibration(const std::string& filename, const std::string& section, 
                      const Calibration& calibration)
{
   using namespace boost::property_tree;

   ptree pt;

   std::ifstream i_file(filename);
   if (i_file.good()) {
     ini_parser::read_ini(i_file, pt);
     i_file.close();
   }
   
   pt.put(section + ".x_factor", calibration.x_factor());
   pt.put(section + ".x_offset", calibration.x_offset());
   pt.put(section + ".y_factor", calibration.y_factor());
   pt.put(section + ".y_offset", calibration.y_offset());
   pt.put(section + ".z_factor", calibration.z_factor());
   pt.put(section + ".z_offset", calibration.z_offset());
   pt.put(section + ".v_factor", calibration.value_factor);
   pt.put(section + ".v_offset", calibration.value_offset);

   std::ofstream o_file(filename);
   ini_parser::write_ini(filename, pt);
}

void save_calibration(const boost::filesystem::path& filename, const std::string& section,
                      const Calibration& calibration)
{
  save_calibration(filename.string(), section, calibration);
}

}  // namespace mru
