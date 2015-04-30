#ifndef NINEDOF_TYPES_
#define NINEDOF_TYPES_

#include <utility>
#include <deque>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Vector_3.h>

namespace ninedof {

typedef boost::posix_time::ptime Time_t;
typedef float Value_t;
typedef CGAL::Simple_cartesian<Value_t>::Vector_3 Vector_t;
typedef std::deque<Vector_t> Vectors_t;
typedef Vectors_t::iterator Vector_i;
typedef std::deque<Value_t> Values_t;
typedef Values_t::iterator Value_i;
typedef struct Sample {
  Time_t time;
  Vector_t vector;
  Value_t value;
  Sample(): time(boost::posix_time::microsec_clock::universal_time()), vector(), value() {}
  Sample(const Sample& s): time(s.time), vector(s.vector), value(s.value) {}
  Sample(const Time_t& t, const Vector_t& vr): time(t), vector(vr), value() {}
  Sample(const Time_t& t, const Vector_t& vr, const Value_t& v): time(t), vector(vr), value(v) {}
  Sample(const Vector_t& vr): time(boost::posix_time::microsec_clock::universal_time()), vector(vr), value() {}
  Sample(const Vector_t& vr, const Value_t& v): time(boost::posix_time::microsec_clock::universal_time()), vector(vr), value(v) {}
  Sample(Sample&& s): time(std::move(s.time)), vector(std::move(s.vector)), value(std::move(s.value)) {}
  Sample& operator=(const Sample& s) {
    time = s.time;
    vector = s.vector;
    value = s.value;
  }
  Sample& operator=(Sample&& s) {
    time = std::move(s.time);
    vector = std::move(s.vector);
    value = std::move(s.value);
  }
} Sample_t;
typedef std::deque<Sample_t> Samples_t;
typedef Samples_t::iterator Sample_i;

} //namespace ninedof

#endif
