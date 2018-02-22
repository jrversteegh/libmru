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

#include "utils.h"
#include "errors.h"

namespace mru {
 
// Default floating point type
using DefaultFT = float;
using Time = boost::posix_time::ptime;
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


template <typename> struct UnitQuaternion;

template <typename FT=DefaultFT>
struct Quaternion
{
  Quaternion() : real_(0), vector_(0, 0, 0) {}
  Quaternion(const Scalar<FT> real, const Vector<FT>& vector) : real_(real_), vector_(vector) {}
  Quaternion(const Quaternion<FT>& quaternion): 
      real_(quaternion.real_), vector_(quaternion.vector_) {}

  Quaternion conjugate() {
    return Quaternion(real_, -vector_);
  }

  Scalar<FT> squared_length() const {
    return real_ * real_ + vector_ * vector_;
  }

  template <typename> friend Quaternion operator*(const Quaternion&, const Quaternion&);
  friend UnitQuaternion<FT>;
  FT qr() { 
    return real_;
  }
  FT qi() { 
    return vector_[0];
  }
  FT qj() { 
    return vector_[1];
  }
  FT qk() { 
    return vector_[2];
  }
private:
  Scalar<FT> real_;
  Vector<FT> vector_;
};

template <typename FT>
Quaternion<FT> operator*(const Quaternion<FT>& q, const Quaternion<FT>& r)
{
  return Quaternion<FT>(q.real_ * r.real_ - q.vector_ * r.vector_,
      q.real_ * r.vector_ + r.scalar_ * q.vector_ +
      CGAL::cross_product(q.vector_, r.vector_));
}

template <typename FT=DefaultFT>
struct UnitQuaternion: public Quaternion<FT> {
  UnitQuaternion() : Quaternion<FT>(0, Vector<FT>(1, 0, 0)) {}
  UnitQuaternion(const Quaternion<FT>& quaterion): Quaternion<FT>(quaterion) {
    normalize_();
  }
  UnitQuaternion(const Scalar<FT> real, const Vector<FT>& vector): Quaternion<FT>(real, vector) {
    normalize_();
  }

  Vector<FT> rotate(const Vector<FT>& vector) {
    // This can be optimized: there are quite a few zeroes in there
    return (*this * Quaternion<FT>(0, vector) * this->conjugate()).vector_;
  }
  Transformation<FT> transformation() {
    FT qii = sqr(this->qi());
    FT qjj = sqr(this->qj());
    FT qkk = sqr(this->qk());
    FT qij = this->qi() * this->qj();
    FT qik = this->qi() * this->qk();
    FT qir = this->qi() * this->qr();
    FT qjk = this->qj() * this->qk();
    FT qjr = this->qj() * this->qr();
    FT qkr = this->qk() * this->qr();

    return Transformation<FT>(
        1 - 2 * (qjj + qkk), 2 * (qij - qkr), 2 * (qik + qjr),
        2 * (qij + qkr), 1 - 2 * (qii + qkk), 2 * (qjk - qir),
        2 * (qik - qjr), 2 * (qjk + qir), 1 - 2 * (qii + qjj));
  }
private:
  void normalize_() {
    Scalar<FT> sql = this->squared_lenght();
    if (std::fabs(sql) < std::numeric_limits<FT>::epsilon()) {
      Scalar<FT> invl = 1 / sqrt(sql);
      this->real_ *= invl;
      this->vector_ *= invl;
    }
    else {
      throw Error("Can't normalize 0 quaterion");
    }
  }
};



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
    return *this;
  }
  RotScalar& operator=(const Scalar<FT> s) {
    set_value(s);
    return *this;
  }
  RotScalar& operator+= (const RotScalar& rs) {
    set_value(get_value() + rs.get_value());
    return *this;
  }
  RotScalar& operator-= (const RotScalar& rs) {
    set_value(get_value() - rs.get_value());
    return *this;
  }
  RotScalar& operator+= (const Scalar<FT>& s) {
    set_value(get_value() + s);
    return *this;
  }
  RotScalar& operator-= (const Scalar<FT>& s) {
    set_value(get_value() - s);
    return *this;
  }
  RotScalar& operator*= (const Scalar<FT>& s) {
    set_value(get_value() * s);
    return *this;
  }
  RotScalar& operator/= (const Scalar<FT>& s) {
    set_value(get_value() / s);
    return *this;
  }
private:
  Scalar<FT> value_;
};

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<MinQ, MaxQ, FT> >::type 
operator*(const RotScalar<MinQ, MaxQ, FT>& rs, const O& s) {
  return RotScalar<MinQ, MaxQ, FT>(rs.get_value() * s);
}

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<MinQ, MaxQ, FT> >::type 
operator*(const O& s, const RotScalar<MinQ, MaxQ, FT>& rs) {
  return rs * s;
}

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<MinQ, MaxQ, FT> >::type 
operator/(const RotScalar<MinQ, MaxQ, FT>& rs, const O& s) {
  return RotScalar<MinQ, MaxQ, FT>(rs.get_value() / s);
}

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<MinQ, MaxQ, FT> >::type 
operator+(const RotScalar<MinQ, MaxQ, FT>& rs, const O& s) {
  return RotScalar<MinQ, MaxQ, FT>(rs.get_value() + s);
}

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<MinQ, MaxQ, FT> >::type 
operator+(const O s, const RotScalar<MinQ, MaxQ, FT>& rs) {
  return rs + s;
}

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<-2, 2, FT> >::type 
operator-(const RotScalar<MinQ, MaxQ, FT>& rs, const O& s) {
  return RotScalar<-2, 2, FT>(rs.get_value() - s);
}

template<int MinQ, int MaxQ, typename FT, typename O> inline 
typename std::enable_if<std::is_arithmetic<O>::value, RotScalar<-2, 2, FT> >::type 
operator-(const O& s, const RotScalar<MinQ, MaxQ, FT>& rs) {
  return rs - s;
}

template<int MinQ1, int MaxQ1, typename FT1, int MinQ2, int MaxQ2, typename FT2> inline 
RotScalar<-2, 2, FT1>
operator-(const RotScalar<MinQ1, MaxQ1, FT1>& rs1, const RotScalar<MinQ2, MaxQ2, FT2>& rs2) {
  return RotScalar<-2, 2, FT1>(rs1.get_value() - rs2.get_value());
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
  Rotation,
};

static constexpr Quantity Pressure = Quantity::Pressure;
static constexpr Quantity Temperature = Quantity::Temperature;
static constexpr Quantity Acceleration = Quantity::Acceleration;
static constexpr Quantity AngularVelocity = Quantity::AngularVelocity;
static constexpr Quantity MagneticFlux = Quantity::MagneticFlux;
static constexpr Quantity Heading = Quantity::Heading;
static constexpr Quantity Pitch = Quantity::Pitch;
static constexpr Quantity Roll = Quantity::Roll;
static constexpr Quantity Rotation = Quantity::Rotation;


template<Quantity Q, typename FT=DefaultFT>
struct Quantity_type {};
template<typename FT>
struct Quantity_type<Pressure, FT> {
  typedef Scalar<FT> type;
};
template<typename FT>
struct Quantity_type<Temperature, FT> {
  typedef Scalar<FT> type;
};
template<typename FT>
struct Quantity_type<Acceleration, FT> {
  typedef Vector<FT> type;
};
template<typename FT>
struct Quantity_type<AngularVelocity, FT> {
  typedef Vector<FT> type;
};
template<typename FT>
struct Quantity_type<MagneticFlux, FT> {
  typedef Vector<FT> type;
};
template<typename FT>
struct Quantity_type<Heading, FT> {
  typedef RotScalar<0, 4, FT> type;
};
template<typename FT>
struct Quantity_type<Pitch, FT> {
  typedef RotScalar<-1, 1, FT> type;
};
template<typename FT>
struct Quantity_type<Roll, FT> {
  typedef RotScalar<-2, 2, FT> type;
};
template<typename FT>
struct Quantity_type<Rotation, FT> {
  typedef UnitQuaternion<FT> type;
};


template <typename FT, Quantity... Qs>
struct Sample {};

template <typename FT, Quantity Q, Quantity... Qs>
struct Sample<FT, Q, Qs...>: Sample<FT, Qs...> {
  Sample<FT, Q, Qs...>(typename Quantity_type<Q>::type q, typename Quantity_type<Qs>::type... qs):
    Sample<FT, Qs...>(qs...), value(q) {}
  typename Quantity_type<Q, FT>::type value;
};

template <typename FT, Quantity GQ, Quantity Q, Quantity... Qs>
inline typename std::enable_if<GQ == Q, typename Quantity_type<GQ>::type>::type
get(const Sample<FT, Q, Qs...>& sample) {
  typename Quantity_type<GQ>::type value = sample.value;
  return value;
}

template <typename FT, Quantity GQ, Quantity Q, Quantity... Qs>
inline typename std::enable_if<GQ != Q, typename Quantity_type<GQ>::type>::type
get(const Sample<FT, Q, Qs...>& sample) {
  return get<FT, GQ>(static_cast<Sample<FT, Qs...> >(sample));
}

/*
template <typename FT>
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
  Sample(const Point<FT>& p,
         const Scalar<FT>& v, const Calibration<FT>& calibration):
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
*/

template <typename FT, Quantity... Qs>
using Samples = std::deque<Sample<FT, Qs...> >;

} //namespace mru

#endif

// vim: syntax=cpp : shiftwidth=2 : tabstop=2 : expandtab :
