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

#ifndef MRU_TYPES_H
#define MRU_TYPES_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <utility>
#include <deque>
#include <set>
#include <stdexcept>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Vector_3.h>

namespace mru {
 
// Default floating point type
typedef float DefaultFT;

typedef boost::posix_time::ptime Time;
inline Time utc_now() {
  return boost::posix_time::microsec_clock::universal_time();
}

template<typename FT=DefaultFT>
using Scalar = FT;
template<typename FT=DefaultFT>
using Vector = typename CGAL::Simple_cartesian<FT>::Vector_3;
template<typename FT=DefaultFT>
using Point = typename CGAL::Simple_cartesian<FT>::Point_3;
template<typename FT=DefaultFT>
using Transformation = typename CGAL::Simple_cartesian<FT>::Aff_transformation_3;

template<typename FT> inline
typename std::enable_if<std::is_floating_point<FT>::value, FT>::type d2r(FT degs) {
  static const FT rpd = 0.017453292519943295769236907684886;
  return rpd * degs;
}

template<typename FT> inline 
typename std::enable_if<std::is_floating_point<FT>::value, FT>::type r2d(FT rads) {
  static const FT dpr = 57.295779513082320876798154814105;
  return dpr * rads;
}

template <int MinQ=0, int MaxQ=4, typename FT=DefaultFT>
struct RotScalar {
  static constexpr FT two_pi = 6.283185307179586476925286766559;
  static constexpr FT pi = 3.1415926535897932384626433832795;
  static constexpr FT half_pi = 1.5707963267948966192313216916398;
  static constexpr int Quarters = MaxQ - MinQ;
  static constexpr FT min = half_pi * MinQ;
  static constexpr FT max = half_pi * MaxQ;
  static constexpr FT range = max - min;
  RotScalar(): value_(0) {}
  RotScalar(const Scalar<FT>& s) {
    set_value(s);
  }
  RotScalar(const Scalar<FT>& s, bool from_degrees) {
    if (from_degrees)
      set_value(d2r(s));
    else
      set_value(s);
  }
  RotScalar(const RotScalar& rs) {
    set_value(rs.get_value());
  }
  Scalar<FT> get_value() const {
    return value_;
  }
  Scalar<FT> to_degrees() const {
    return r2d(value_);
  }
  template <int Quarters = Quarters>
  typename std::enable_if<Quarters == 4, Scalar<FT> >::type set_value(Scalar<FT> value) {
    if (value >= max || value < min) {
      Scalar<FT> r = std::fmod(value - min, range);
      if (r < 0)
        value_ = r + max;
      else
        value_ = r + min;
    }
    else
      value_ = value;
    return value_;
  }
  template <int Quarters = Quarters>
  typename std::enable_if<Quarters == 2 || Quarters == 1, Scalar<FT> >::type set_value(Scalar<FT> value) {
    if (value > max || value < min) {
      Scalar<FT> d = (value - min) / range;
      Scalar<FT> i, r = std::fabs(std::modf(d, &i));
      if ((static_cast<int>(i) % 2 == 0)) {
        value_ = r * range + min;
      }
      else {
        value_ = max - r * range;
      }
    }
    else
      value_ = value;
    return value_;
  }
  Scalar<FT> operator[](int index) const {
    if (index != 0)
      throw std::out_of_range("Index should be 0");
    return get_value();
  }
  operator Scalar<FT>() const {
    auto v = get_value();
    return v;
  }
  template <class RS>
  RotScalar& operator=(const RS& rs) {
    set_value(rs.get_value());
  }
  RotScalar& operator=(const Scalar<FT> s) {
    set_value(s);
  }
  RotScalar& operator+= (const RotScalar& rs) {
    set_value(get_value() + rs.get_value());
  }
  RotScalar& operator-= (const RotScalar& rs) {
    set_value(get_value() - rs.get_value());
  }
  RotScalar& operator+= (const Scalar<FT>& s) {
    set_value(get_value() + s);
  }
  RotScalar& operator-= (const Scalar<FT>& s) {
    set_value(get_value() - s);
  }
  RotScalar& operator*= (const Scalar<FT>& s) {
    set_value(get_value() * s);
  }
  RotScalar& operator/= (const Scalar<FT>& s) {
    set_value(get_value() / s);
  }
private:
  Scalar<FT> value_;
};

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<MinQ, MaxQ, FT> >&& 
operator*(RotScalar<MinQ, MaxQ, FT> rs, O s) {
  auto v = rs.get_value();
  return std::move(RotScalar<MinQ, MaxQ, FT>(v * s));
}



// Define quantities provided by the sensors
enum class Quantity {
  Pressure,
  Temperature,
  Acceleration,
  AngularVelocity,
  MagneticFlux,
  Heading,
  Pitch,
  Roll,
};


template<Quantity Q, typename FT=DefaultFT>
struct Quantity_type {};
template<typename FT>
struct Quantity_type<Quantity::Pressure, FT> {
  typedef Scalar<FT> type;
};
template<typename FT>
struct Quantity_type<Quantity::Temperature, FT> {
  typedef Scalar<FT> type;
};
template<typename FT>
struct Quantity_type<Quantity::Acceleration, FT> {
  typedef Vector<FT> type;
};
template<typename FT>
struct Quantity_type<Quantity::AngularVelocity, FT> {
  typedef Vector<FT> type;
};
template<typename FT>
struct Quantity_type<Quantity::MagneticFlux, FT> {
  typedef Vector<FT> type;
};
template<typename FT>
struct Quantity_type<Quantity::Heading, FT> {
  typedef RotScalar<0, 4, FT> type;
};
template<typename FT>
struct Quantity_type<Quantity::Pitch, FT> {
  typedef RotScalar<-1, 1, FT> type;
};
template<typename FT>
struct Quantity_type<Quantity::Roll, FT> {
  typedef RotScalar<-2, 2, FT> type;
};


template <typename FT=DefaultFT>
struct Sample {
  Time time;
  Vector<FT> vector;
  Scalar<FT> value;
  Sample(): time(), vector(), value() {}
  Sample(const Sample& s):
      time(s.time), vector(s.vector), value(s.value) {}
  Sample(const Time& t, const Vector<FT>& vr):
      time(t), vector(vr), value() {}
  Sample(const Time& t, const Vector<FT>& vr, const Scalar<FT>& v):
      time(t), vector(vr), value(v) {}
  Sample(const Vector<FT>& vr):
      time(utc_now()), vector(vr), value() {}
  Sample(const Vector<FT>& vr, const Scalar<FT>& v):
      time(utc_now()), vector(vr), value(v) {}
  Sample(Sample&& s):
      time(std::move(s.time)), vector(std::move(s.vector)), value(std::move(s.value)) {}
  /*
  Sample(const Point<FT>& p,
         const Scalar<FT>& v, const Calibration<FT>& calibration):
      time(utc_now()) {
    vector = calibration.correction(p) - CGAL::ORIGIN;
    value = v * calibration.value_factor + calibration.value_offset;
  }
  */
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

template <typename FT=DefaultFT>
using Samples = std::deque<Sample<FT> >;

} //namespace mru

#endif
