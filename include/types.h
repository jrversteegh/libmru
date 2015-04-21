#ifndef NINEDOF_TYPES_
#define NINEDOF_TYPES_

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Vector_3.h>

namespace ninedof {

typedef float Number_type;
typedef CGAL::Simple_cartesian<Number_type>::Vector_3 Vector;

} //namespace ninedof

#endif
