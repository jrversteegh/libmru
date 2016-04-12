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
 * Declares some basic types that are used throughout
 */

#ifndef NINEDOF_TYPES_
#define NINEDOF_TYPES_

#include <utility>
#include <deque>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Vector_3.h>

namespace ninedof {

typedef boost::posix_time::ptime Time;
inline Time utc_now() {
  return boost::posix_time::microsec_clock::universal_time();
}

typedef float Value;
typedef std::deque<Value> Values;
typedef Values::iterator Value_i;

typedef CGAL::Simple_cartesian<Value>::Vector_3 Vector;
typedef CGAL::Simple_cartesian<Value>::Point_3 Point;

typedef CGAL::Simple_cartesian<Value>::Aff_transformation_3 Transformation;

struct Correction: public Transformation {
  using Transformation::Transformation;
  Correction(const Value& x_factor, const Value& x_offset, 
             const Value& y_factor, const Value& y_offset,
             const Value& z_factor, const Value& z_offset):
    Transformation(x_factor, 0, 0, x_offset,
                   0, y_factor, 0, y_offset,
                   0, 0, z_factor, z_offset) {}
  Correction(const Value& xx_factor, const Value& xy_factor, const Value& xz_factor, const Value& x_offset, 
             const Value& yx_factor, const Value& yy_factor, const Value& yz_factor, const Value& y_offset,
             const Value& zx_factor, const Value& zy_factor, const Value& zz_factor, const Value& z_offset):
    Transformation(xx_factor, xy_factor, xz_factor, x_offset,
                   yx_factor, yy_factor, yz_factor, y_offset,
                   zx_factor, zy_factor, zz_factor, z_offset) {}
};

struct Calibration {
  Correction correction;
  Value value_factor;
  Value value_offset;
  Calibration(): 
      correction(1.0, 0.0, 1.0, 0.0, 1.0, 0.0),
      value_factor(1.0), value_offset(0.0) {}
  Calibration(const Calibration& calibration): 
      correction(calibration.correction),
      value_factor(calibration.value_factor), value_offset(calibration.value_offset) {}
  Calibration(const Value& x_factor, const Value& x_offset,
              const Value& y_factor, const Value& y_offset,
              const Value& z_factor, const Value& z_offset,
              const Value& v_factor, const Value& v_offset):
      correction(x_factor, x_offset, y_factor, y_offset, z_factor, z_offset),
      value_factor(v_factor), value_offset(v_offset) {}
  Calibration(const Value& xx_factor, const Value& xy_factor, const Value& xz_factor, const Value& x_offset,
              const Value& yx_factor, const Value& yy_factor, const Value& yz_factor, const Value& y_offset,
              const Value& zx_factor, const Value& zy_factor, const Value& zz_factor, const Value& z_offset,
              const Value& v_factor, const Value& v_offset):
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
  Value x_factor() const { 
    return correction.m(0, 0);
  }
  Value x_offset() const { 
    return correction.m(0, 3);
  }
  Value y_factor() const { 
    return correction.m(1, 1);
  }
  Value y_offset() const { 
    return correction.m(1, 3);
  }
  Value z_factor() const { 
    return correction.m(2, 2);
  }
  Value z_offset() const { 
    return correction.m(2, 3);
  }
  Value v_factor() const { 
    return value_factor;
  }
  Value v_offset() const { 
    return value_offset;
  }
};

struct Sample {
  Time time;
  Vector vector;
  Value value;
  Sample(): time(), vector(), value() {}
  Sample(const Sample& s): 
      time(s.time), vector(s.vector), value(s.value) {}
  Sample(const Time& t, const Vector& vr): 
      time(t), vector(vr), value() {}
  Sample(const Time& t, const Vector& vr, const Value& v): 
      time(t), vector(vr), value(v) {}
  Sample(const Vector& vr): 
      time(utc_now()), vector(vr), value() {}
  Sample(const Vector& vr, const Value& v): 
      time(utc_now()), vector(vr), value(v) {}
  Sample(Sample&& s): 
      time(std::move(s.time)), vector(std::move(s.vector)), value(std::move(s.value)) {}
  Sample(const Point& p,
         const Value& v, const Calibration& calibration):
      time(utc_now()) {
    vector = calibration.correction(p) - CGAL::ORIGIN;
    value = v * calibration.value_factor + calibration.value_offset;
  }
  Sample& operator=(const Sample& s) {
    time = s.time;
    vector = s.vector;
    value = s.value;
    return *this;
  }
  Sample& operator=(Sample&& s) {
    time = std::move(s.time);
    vector = std::move(s.vector);
    value = std::move(s.value);
    return *this;
  }
};
typedef std::deque<Sample> Samples;
typedef Samples::iterator Sample_i;


} //namespace ninedof

#endif
