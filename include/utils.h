/**
 * \file
 * \author Jaap Versteegh <j.r.versteegh@gmail.com>
 * \brief Defines some general utility functions
 * \license
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#ifndef MRU_UTILS_H
#define MRU_UTILS_H


namespace mru {

template<typename FT> 
inline
typename std::enable_if<std::is_floating_point<FT>::value, FT>::type 
d2r(const FT degs) {
  static const FT rpd = 0.017453292519943295769236907684886;
  return rpd * degs;
}

template<typename FT> 
inline 
typename std::enable_if<std::is_floating_point<FT>::value, FT>::type 
r2d(const FT rads) {
  static const FT dpr = 57.295779513082320876798154814105;
  return dpr * rads;
}

template<typename FT> 
inline 
typename std::enable_if<std::is_floating_point<FT>::value, FT>::type 
sqr(const FT value) {
  return value * value;
}

}  // namespace mru 

#endif
