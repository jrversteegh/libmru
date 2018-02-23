/** 
 * \file
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 * \brief Defines functions for calibration value handling
 * \license
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

#include <fstream>
#include <utility>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "../include/calibration.h"

namespace mru {

template<typename FT=DefaultFT>
Calibration<FT> load_calibration(const std::string& filename, const std::string& section)
{
   using namespace boost::property_tree;

   ptree pt;

   std::ifstream i_file(filename);
   if (i_file.good()) {
     ini_parser::read_ini(i_file, pt);
     i_file.close();
   }

   Calibration<FT> result;

   if (pt.get(section + ".x_factor", static_cast<Scalar<FT> >(0)) == 0 &&
       pt.get(section + ".y_factor", static_cast<Scalar<FT> >(0)) == 0 &&
       pt.get(section + ".z_factor", static_cast<Scalar<FT> >(0)) == 0) {
     Scalar<FT> xx_factor = pt.get(section + ".xx_factor", static_cast<Scalar<FT> >(1.0));
     Scalar<FT> xy_factor = pt.get(section + ".xy_factor", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> xz_factor = pt.get(section + ".xz_factor", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> x_offset = pt.get(section + ".x_offset", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> yx_factor = pt.get(section + ".yx_factor", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> yy_factor = pt.get(section + ".yy_factor", static_cast<Scalar<FT >>(1.0));
     Scalar<FT> yz_factor = pt.get(section + ".yz_factor", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> y_offset = pt.get(section + ".y_offset", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> zx_factor = pt.get(section + ".zx_factor", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> zy_factor = pt.get(section + ".zy_factor", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> zz_factor = pt.get(section + ".zz_factor", static_cast<Scalar<FT >>(1.0));
     Scalar<FT> z_offset = pt.get(section + ".z_offset", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> v_factor = pt.get(section + ".v_factor", static_cast<Scalar<FT >>(1.0));
     Scalar<FT> v_offset = pt.get(section + ".v_offset", static_cast<Scalar<FT >>(0.0));
     result = Calibration<FT>(
         xx_factor, xy_factor, xz_factor, x_offset, 
         yx_factor, yy_factor, yz_factor, y_offset,
         zx_factor, zy_factor, zz_factor, z_offset,
         v_factor, v_offset
     );
   } 
   else {
     Scalar<FT> x_factor = pt.get(section + ".x_factor", static_cast<Scalar<FT >>(1.0));
     Scalar<FT> x_offset = pt.get(section + ".x_offset", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> y_factor = pt.get(section + ".y_factor", static_cast<Scalar<FT >>(1.0));
     Scalar<FT> y_offset = pt.get(section + ".y_offset", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> z_factor = pt.get(section + ".z_factor", static_cast<Scalar<FT >>(1.0));
     Scalar<FT> z_offset = pt.get(section + ".z_offset", static_cast<Scalar<FT >>(0.0));
     Scalar<FT> v_factor = pt.get(section + ".v_factor", static_cast<Scalar<FT >>(1.0));
     Scalar<FT> v_offset = pt.get(section + ".v_offset", static_cast<Scalar<FT >>(0.0));
     result = Calibration<FT>(
         x_factor, x_offset, 
         y_factor, y_offset,
         z_factor, z_offset, 
         v_factor, v_offset
     );
   }
   return result;
}

template<typename FT=DefaultFT>
Calibration<FT> load_calibration(const boost::filesystem::path& filename, const std::string& section)
{
  return load_calibration<FT>(filename.string(), section);
}

template<typename FT=DefaultFT>
void save_calibration(const std::string& filename, const std::string& section, 
                      const Calibration<FT>& calibration)
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

template<typename FT=DefaultFT>
void save_calibration(const boost::filesystem::path& filename, const std::string& section,
                      const Calibration<FT>& calibration)
{
  save_calibration<FT>(filename.string(), section, calibration);
}

// Explicit instantiations
template Calibration<float> load_calibration(const std::string& filename, const std::string& section);
template Calibration<float> load_calibration(const boost::filesystem::path& filename, const std::string& section);
template void save_calibration(const std::string& filename, const std::string& section,
                               const Calibration<float>& calibration);
template void save_calibration(const boost::filesystem::path& filename, const std::string& section,
                               const Calibration<float>& calibration);

template Calibration<double> load_calibration(const std::string& filename, const std::string& section);
template Calibration<double> load_calibration(const boost::filesystem::path& filename, const std::string& section);
template void save_calibration(const std::string& filename, const std::string& section,
                               const Calibration<double>& calibration);
template void save_calibration(const boost::filesystem::path& filename, const std::string& section,
                               const Calibration<double>& calibration);

template Calibration<long double> load_calibration(const std::string& filename, const std::string& section);
template Calibration<long double> load_calibration(const boost::filesystem::path& filename, const std::string& section);
template void save_calibration(const std::string& filename, const std::string& section,
                               const Calibration<long double>& calibration);
template void save_calibration(const boost::filesystem::path& filename, const std::string& section,
                               const Calibration<long double>& calibration);
}  // namespace mru

// vim: syntax=cpp : shiftwidth=2 : tabstop=2 : expandtab :
