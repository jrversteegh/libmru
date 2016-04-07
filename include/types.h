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

typedef boost::posix_time::ptime Time_t;
inline Time_t utc_now() {
  return boost::posix_time::microsec_clock::universal_time();
}

typedef float Value_t;
typedef std::deque<Value_t> Values_t;
typedef Values_t::iterator Value_i;

typedef CGAL::Simple_cartesian<Value_t>::Vector_3 Vector_t;
typedef std::deque<Vector_t> Vectors_t;
typedef Vectors_t::iterator Vector_i;

typedef struct Calibration {
  Value_t x_factor;
  Value_t x_offset;
  Value_t y_factor;
  Value_t y_offset;
  Value_t z_factor;
  Value_t z_offset;
  Value_t v_factor;
  Value_t v_offset;
} Calibration_t;

typedef struct Sample {
  Time_t time;
  Vector_t vector;
  Value_t value;
  Sample(): time(), vector(), value() {}
  Sample(const Sample& s): 
      time(s.time), vector(s.vector), value(s.value) {}
  Sample(const Time_t& t, const Vector_t& vr): 
      time(t), vector(vr), value() {}
  Sample(const Time_t& t, const Vector_t& vr, const Value_t& v): 
      time(t), vector(vr), value(v) {}
  Sample(const Vector_t& vr): 
      time(utc_now()), vector(vr), value() {}
  Sample(const Vector_t& vr, const Value_t& v): 
      time(utc_now()), vector(vr), value(v) {}
  Sample(Sample&& s): 
      time(std::move(s.time)), vector(std::move(s.vector)), value(std::move(s.value)) {}
  Sample(const Time_t& t, const Value_t& x, const Value_t& y, const Value_t& z,
         const Value_t& v, const Calibration& calibration):
      time(t) {
    vector = Vector_t(x * calibration.x_factor + calibration.x_offset,
                      y * calibration.y_factor + calibration.y_offset,
                      z * calibration.z_factor + calibration.z_offset);
    value = v * calibration.v_factor + calibration.v_offset;
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
} Sample_t;
typedef std::deque<Sample_t> Samples_t;
typedef Samples_t::iterator Sample_i;


} //namespace ninedof

#endif
