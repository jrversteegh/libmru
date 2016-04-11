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
typedef std::deque<Vector> Vectors;
typedef Vectors::iterator Vector_i;

typedef CGAL::Simple_cartesian<Value>::Aff_transformation_3 Transformation;

struct Vector_factor_offset: public Transformation {
  using Transformation::Transformation;
  Vector_factor_offset(const Value& x_factor, const Value& x_offset, 
                       const Value& y_factor, const Value& y_offset,
                       const Value& z_factor, const Value& z_offset):
    Transformation(x_factor, 0, 0, x_offset,
                   0, y_factor, 0, y_offset,
                   0, 0, z_factor, z_offset) {}
};

struct Calibration {
  Vector_factor_offset vector_transform;
  Value value_factor;
  Value value_offset;
  Calibration(): 
      vector_transform(1.0, 0.0, 1.0, 0.0, 1.0, 0.0),
      value_factor(1.0), value_offset(0.0) {}
  Calibration(const Calibration& calibration): 
      vector_transform(calibration.vector_transform),
      value_factor(calibration.value_factor), value_offset(calibration.value_offset) {}
  Calibration(const Value& x_factor, const Value& x_offset,
              const Value& y_factor, const Value& y_offset,
              const Value& z_factor, const Value& z_offset,
              const Value& v_factor, const Value& v_offset):
      vector_transform(x_factor, x_offset, y_factor, y_offset, z_factor, y_offset),
      value_factor(v_factor), value_offset(v_offset) {}
  Calibration& operator=(const Calibration& calibration) {
    vector_transform = calibration.vector_transform;
    value_factor = calibration.value_factor;
    value_offset = calibration.value_offset;
    return *this;
  }
  Value x_factor() const { 
    return vector_transform.m(0, 0);
  }
  Value y_factor() const { 
    return vector_transform.m(1, 1);
  }
  Value z_factor() const { 
    return vector_transform.m(2, 2);
  }
  Value x_offset() const { 
    return vector_transform.m(0, 3);
  }
  Value y_offset() const { 
    return vector_transform.m(1, 3);
  }
  Value z_offset() const { 
    return vector_transform.m(2, 3);
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
  Sample(const Vector& vr,
         const Value& v, const Calibration& cal):
      time(utc_now()) {
    vector = cal.vector_transform(vr);
    value = v * cal.value_factor + cal.value_offset;
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
