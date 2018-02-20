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

#ifndef MRU_CALIBRATION_H
#define MRU_CALIBRATION_H

#include <boost/filesystem.hpp>

#include "types.h"

namespace mru {

template<typename FT=DefaultFT>
struct Correction: public Transformation<FT> {
  Correction(const Scalar<FT>& x_factor, const Scalar<FT>& x_offset,
             const Scalar<FT>& y_factor, const Scalar<FT>& y_offset,
             const Scalar<FT>& z_factor, const Scalar<FT>& z_offset):
    Transformation<FT>(
        x_factor, 0, 0, x_offset,
        0, y_factor, 0, y_offset,
        0, 0, z_factor, z_offset) {}
  Correction(const Scalar<FT>& xx_factor, const Scalar<FT>& xy_factor, 
             const Scalar<FT>& xz_factor, const Scalar<FT>& x_offset,
             const Scalar<FT>& yx_factor, const Scalar<FT>& yy_factor, 
             const Scalar<FT>& yz_factor, const Scalar<FT>& y_offset,
             const Scalar<FT>& zx_factor, const Scalar<FT>& zy_factor, 
             const Scalar<FT>& zz_factor, const Scalar<FT>& z_offset):
    Transformation<FT>(
        xx_factor, xy_factor, xz_factor, x_offset,
        yx_factor, yy_factor, yz_factor, y_offset,
        zx_factor, zy_factor, zz_factor, z_offset) {}
};

template<typename FT=DefaultFT>
struct Calibration {
  Correction<FT> correction;
  Scalar<FT> value_factor;
  Scalar<FT> value_offset;
  Calibration():
      correction(1.0, 0.0, 1.0, 0.0, 1.0, 0.0),
      value_factor(1.0), value_offset(0.0) {}
  Calibration(const Calibration& calibration):
      correction(calibration.correction),
      value_factor(calibration.value_factor), value_offset(calibration.value_offset) {}
  Calibration(const Scalar<FT>& x_factor, const Scalar<FT>& x_offset,
              const Scalar<FT>& y_factor, const Scalar<FT>& y_offset,
              const Scalar<FT>& z_factor, const Scalar<FT>& z_offset,
              const Scalar<FT>& v_factor, const Scalar<FT>& v_offset):
      correction(x_factor, x_offset, y_factor, y_offset, z_factor, z_offset),
      value_factor(v_factor), value_offset(v_offset) {}
  Calibration(const Scalar<FT>& xx_factor, const Scalar<FT>& xy_factor, 
              const Scalar<FT>& xz_factor, const Scalar<FT>& x_offset,
              const Scalar<FT>& yx_factor, const Scalar<FT>& yy_factor, 
              const Scalar<FT>& yz_factor, const Scalar<FT>& y_offset,
              const Scalar<FT>& zx_factor, const Scalar<FT>& zy_factor, 
              const Scalar<FT>& zz_factor, const Scalar<FT>& z_offset,
              const Scalar<FT>& v_factor, const Scalar<FT>& v_offset):
      correction(xx_factor, xy_factor, xz_factor, x_offset,
                 yx_factor, yy_factor, yz_factor, y_offset,
                 zx_factor, zy_factor, zz_factor, z_offset),
      value_factor(v_factor), value_offset(v_offset) {}
  Calibration& operator=(const Calibration& calibration) {
    correction = calibration.correction;
    value_factor = calibration.value_factor;
    value_offset = calibration.value_offset;
    return *this;
  }
  Scalar<FT> x_factor() const {
    return correction.m(0, 0);
  }
  Scalar<FT> x_offset() const {
    return correction.m(0, 3);
  }
  Scalar<FT> y_factor() const {
    return correction.m(1, 1);
  }
  Scalar<FT> y_offset() const {
    return correction.m(1, 3);
  }
  Scalar<FT> z_factor() const {
    return correction.m(2, 2);
  }
  Scalar<FT> z_offset() const {
    return correction.m(2, 3);
  }
  Scalar<FT> v_factor() const {
    return value_factor;
  }
  Scalar<FT> v_offset() const {
    return value_offset;
  }
};

template<typename FT=DefaultFT>
extern Calibration<FT> load_calibration(const std::string& filename, const std::string& section);
template<typename FT=DefaultFT>
extern Calibration<FT> load_calibration(const boost::filesystem::path& filename, const std::string& section);
template<typename FT=DefaultFT>
extern void save_calibration(const std::string& filename, const std::string& section,
                             const Calibration<FT>& calibration);
template<typename FT=DefaultFT>
extern void save_calibration(const boost::filesystem::path& filename, const std::string& section,
                             const Calibration<FT>& calibration);

}  // namespace mru

#endif
