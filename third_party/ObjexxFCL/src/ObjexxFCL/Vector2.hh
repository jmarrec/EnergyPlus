#ifndef ObjexxFCL_Vector2_hh_INCLUDED
#define ObjexxFCL_Vector2_hh_INCLUDED

// Vector2: Fast 2-Element Vector
//
// Project: Objexx Fortran-C++ Library (ObjexxFCL)
//
// Version: 4.2.0
//
// Language: C++
//
// Copyright (c) 2000-2017 Objexx Engineering, Inc. All Rights Reserved.
// Use of this source code or any derivative of it is restricted by license.
// Licensing is available from Objexx Engineering, Inc.:  http://objexx.com

// C++ Headers
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <ostream>
namespace ObjexxFCL {

// Vector2: Fast 2-Element Vector
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
// . Use std::array< double, 2 > instead in array/vectorization context
class Vector2
{

 public:  // Types
  // STL Style
  using value_type = double;
  using reference = double&;
  using const_reference = double const&;
  using pointer = double*;
  using const_pointer = double const*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

 public:  // Creation
  // Default Constructor
  Vector2() = default;

  // Uniform Value Constructor
  explicit Vector2(double t) : x(t), y(t) {}

  // Value Constructor
  Vector2(double x_, double y_) : x(x_), y(y_) {}

 public:  // Assignment
  /// Scalar multiplication and division: Compound Assignment
  // *= Scalar
  Vector2& operator*=(double t) {
    x *= t;
    y *= t;
    return *this;
  }

  // /= Scalar
  Vector2& operator/=(double const u) {
    assert(u != 0.0);
    double const inv_u(1.0 / u);
    x *= inv_u;
    y *= inv_u;
    return *this;
  }

  /// Addition and Subtraction: Compound Assignment and Unary negation
  // += Vector2
  Vector2& operator+=(Vector2 const& v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  // -= Vector2
  Vector2& operator-=(Vector2 const& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  // -Vector2 (Negated)
  Vector2 operator-() const {
    return {-x, -y};
  }


 public:  // Subscript
  // Vector2[ i ] const: 0-Based Index
  double operator[](size_type const i) const {
    assert(i <= 1);
    return (i == 0 ? x : y);
  }

  // Vector2[ i ]: 0-Based Index
  double& operator[](size_type const i) {
    assert(i <= 1);
    return (i == 0 ? x : y);
  }

 public:  // Properties: Predicates
  // Is Zero Vector?
  constexpr bool is_zero() const {
    return (x == 0.0) && (y == 0.0);
  }

  // Is Unit Vector?
  bool is_unit() const {
    return (length_squared() == 1.0);
  }

 public:  // Properties: General
  // Size
  size_type size() const {
    return 2u;
  }

  // Length (L2 norm)
  double length() const {
    return std::sqrt((x * x) + (y * y));
  }

  // Length Squared
  double length_squared() const {
    return (x * x) + (y * y);
  }

  // L1 Norm
  double norm_L1() const {
    return std::abs(x) + std::abs(y);
  }

  // L-infinity Norm
  double norm_Linf() const {
    return std::max(std::abs(x), std::abs(y));
  }

  // Distance to a Vector2
  double distance(Vector2 const& v) const {
    return std::sqrt(square(x - v.x) + square(y - v.y));
  }

  // Distance Squared to a Vector2
  double distance_squared(Vector2 const& v) const {
    return square(x - v.x) + square(y - v.y);
  }

  // Dot Product with a Vector2
  double dot(Vector2 const& v) const {
    return (x * v.x) + (y * v.y);
  }

  // Cross Product with a Vector2
  double cross(Vector2 const& v) const {
    return (x * v.y) - (y * v.x);
  }

 public:  // Modifiers
  // Normalize to a Length
  Vector2& normalize(double tar_length = 1.0) {
    double const cur_length(length());
    assert(cur_length != double(0));
    double const dilation(tar_length / cur_length);
    x *= dilation;
    y *= dilation;
    return *this;
  }

  // Normalize to a Length: Zero Vector2 if Length is Zero
  Vector2& normalize_zero(double tar_length = 1.0) {
    double const cur_length(length());
    if (cur_length > 0.0) {
      double const dilation(tar_length / cur_length);
      x *= dilation;
      y *= dilation;
    } else {  // Set zero vector
      x = y = 0.0;
    }
    return *this;
  }

  // Minimum Coordinates with a Vector2
  Vector2& min(Vector2 const& v) {
    x = (x <= v.x ? x : v.x);
    y = (y <= v.y ? y : v.y);
    return *this;
  }

  // Maximum Coordinates with a Vector2
  Vector2& max(Vector2 const& v) {
    x = (x >= v.x ? x : v.x);
    y = (y >= v.y ? y : v.y);
    return *this;
  }

  // Project Normal to a Vector2
  Vector2& project_normal(Vector2 const& v) {
    assert(v.length_squared() != 0.0);
    double const c(dot(v) / v.length_squared());
    x -= c * v.x;
    y -= c * v.y;
    return *this;
  }

  // Project onto a Vector2
  Vector2& project_parallel(Vector2 const& v) {
    assert(v.length_squared() != 0.0);
    double const c(dot(v) / v.length_squared());
    x = c * v.x;
    y = c * v.y;
    return *this;
  }

 public:  // Generators

  // Normalized to a Length
  Vector2 normalized(double tar_length = 1.0) const {
    double const cur_length(length());
    assert(cur_length != double(0));
    double const dilation(tar_length / cur_length);
    return Vector2(x * dilation, y * dilation);
  }

  // Normalized to a Length: Zero Vector2 if Length is Zero
  Vector2 normalized_zero(double tar_length = 1.0) const {
    double const cur_length(length());
    if (cur_length > 0.0) {
      double const dilation(tar_length / cur_length);
      return Vector2(x * dilation, y * dilation);
    } else {  // Return zero vector
      return Vector2(0.0);
    }
  }

  // Projected Normal to a Vector2
  Vector2 projected_normal(Vector2 const& v) const {
    assert(v.length_squared() != 0.0);
    double const c(dot(v) / v.length_squared());
    return Vector2(x - (c * v.x), y - (c * v.y));
  }

  // Projected onto a Vector2
  Vector2 projected_parallel(Vector2 const& v) const {
    assert(v.length_squared() != 0.0);
    double const c(dot(v) / v.length_squared());
    return Vector2(c * v.x, c * v.y);
  }

 public:  // Static Methods
  // Square of a value
  static double square(double t) {
    return t * t;
  }

  // Value Clipped to [-1,1]
  static double sin_cos_range(double t) {
    return std::min(std::max(t, -1.0), 1.0);
  }

  // Add 2*Pi to a Negative Value
  static double bump_up_angle(double t) {
    static double const Two_Pi(2.0 * std::acos(-1.0));
    return (t >= 0.0 ? t : Two_Pi + t);
  }

 public:  // Data Elements
  double x = 0.0;
  double y = 0.0;

};  // Vector2

/// Lexicographic comparison (x first, then y)
inline bool operator==(Vector2 const& a, Vector2 const& b) {
  return (a.x == b.x) && (a.y == b.y);
}

inline bool operator!=(Vector2 const& a, Vector2 const& b) {
  return (a.x != b.x) || (a.y != b.y);
}

inline bool operator<(Vector2 const& a, Vector2 const& b) {
  return ((a.x < b.x ? true
                     : (b.x < a.x ? false :  // a.x == b.x
                          (a.y < b.y))));
}

inline bool operator<=(Vector2 const& a, Vector2 const& b) {
  return ((a.x < b.x ? true
                     : (b.x < a.x ? false :  // a.x == b.x
                          (a.y <= b.y))));
}

inline bool operator>=(Vector2 const& a, Vector2 const& b) {
  return ((a.x > b.x ? true
                     : (b.x > a.x ? false :  // a.x == b.x
                          (a.y >= b.y))));
}

inline bool operator>(Vector2 const& a, Vector2 const& b) {
  return ((a.x > b.x ? true
                     : (b.x > a.x ? false :  // a.x == b.x
                          (a.y > b.y))));
}

/// Scalar multiplication and division: Binary Operators
// Vector2 * Scalar
inline Vector2 operator*(Vector2 const& v, double t) {
  return Vector2(v.x * t, v.y * t);
}

// Scalar * Vector2
inline Vector2 operator*(double t, Vector2 const& v) {
  return Vector2(t * v.x, t * v.y);
}

// Vector2 / Scalar
inline Vector2 operator/(Vector2 const& v, double const u) {
  assert(u != 0.0);
  double const inv_u(1.0 / u);
  return Vector2(v.x * inv_u, v.y * inv_u);
}


/// Addition and Subtraction: Binary Operators
// Vector2 + Vector2
inline Vector2 operator+(Vector2 const& a, Vector2 const& b) {
  return Vector2(a.x + b.x, a.y + b.y);
}

// Vector2 - Vector2
inline Vector2 operator-(Vector2 const& a, Vector2 const& b) {
  return Vector2(a.x - b.x, a.y - b.y);
}

/// Free Functions

// Minimum of Two Vector2s
inline Vector2 min(Vector2 const& a, Vector2 const& b) {
  return Vector2((a.x <= b.x ? a.x : b.x), (a.y <= b.y ? a.y : b.y));
}

// Minimum of Three Vector2s
inline Vector2 min(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return Vector2(std::min({a.x, b.x, c.x}), std::min({a.y, b.y, c.y}));
}

// Minimum of Four Vector2s
inline Vector2 min(Vector2 const& a, Vector2 const& b, Vector2 const& c, Vector2 const& d) {
  return Vector2(std::min({a.x, b.x, c.x, d.x}), std::min({a.y, b.y, c.y, d.y}));
}

// Maximum of Two Vector2s
inline Vector2 max(Vector2 const& a, Vector2 const& b) {
  return Vector2((a.x >= b.x ? a.x : b.x), (a.y >= b.y ? a.y : b.y));
}

// Maximum of Three Vector2s
inline Vector2 max(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return Vector2(std::max({a.x, b.x, c.x}), std::max({a.y, b.y, c.y}));
}

// Maximum of Four Vector2s
inline Vector2 max(Vector2 const& a, Vector2 const& b, Vector2 const& c, Vector2 const& d) {
  return Vector2(std::max({a.x, b.x, c.x, d.x}), std::max({a.y, b.y, c.y, d.y}));
}

// Midpoint of Two Vector2s
inline Vector2 mid(Vector2 const& a, Vector2 const& b) {
  return Vector2(double(0.5 * (a.x + b.x)), double(0.5 * (a.y + b.y)));
}

// Center of Two Vector2s
inline Vector2 cen(Vector2 const& a, Vector2 const& b) {
  return Vector2(double(0.5 * (a.x + b.x)), double(0.5 * (a.y + b.y)));
}

// Center of Three Vector2s
inline Vector2 cen(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  static long double const third(1.0 / 3.0);
  return Vector2(double(third * (a.x + b.x + c.x)), double(third * (a.y + b.y + c.y)));
}

// Center of Four Vector2s
inline Vector2 cen(Vector2 const& a, Vector2 const& b, Vector2 const& c, Vector2 const& d) {
  return Vector2(double(0.25 * (a.x + b.x + c.x + d.x)), double(0.25 * (a.y + b.y + c.y + d.y)));
}

// Angle Between Two Vector2s (in Radians on [0,pi])
inline double angle(Vector2 const& a, Vector2 const& b) {
  double const axb(std::abs(a.cross(b)));
  double const adb(a.dot(b));
  return ((axb != 0.0) || (adb != 0.0) ? Vector2::bump_up_angle(std::atan2(axb, adb))
                                       : 0.0);  // More accurate than dot-based for angles near 0 and Pi
}

// Angle abc Formed by Three Vector2s (in Radians on [0,pi])
inline double angle(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return angle(a - b, c - b);
}

// Cosine of Angle Between Two Vector2s
inline double cos(Vector2 const& a, Vector2 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > 0.0 ? Vector2::sin_cos_range(a.dot(b) / mag) : 1.0);
}

// Cosine of Angle abc Formed by Three Vector2s
inline double cos(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return cos(a - b, c - b);
}

// Sine of Angle Between Two Vector2s
inline double sin(Vector2 const& a, Vector2 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > 0.0 ? std::abs(Vector2::sin_cos_range(a.cross(b) / mag)) : 0.0);
}

// Sine of Angle abc Formed by Three Vector2s
inline double sin(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return sin(a - b, c - b);
}

// Directed Angle Between Two Vector2s (in Radians on [0,2*pi])
inline double dir_angle(Vector2 const& a, Vector2 const& b) {
  double const axb(a.cross(b));
  double const adb(a.dot(b));
  return ((axb != 0.0) || (adb != 0.0) ? Vector2::bump_up_angle(std::atan2(axb, adb)) : 0.0);
}

// Directed Angle abc Formed by Three Vector2s (in Radians on [0,2*pi])
inline double dir_angle(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return dir_angle(a - b, c - b);
}

// Cosine of Directed Angle Between Two Vector2s
inline double dir_cos(Vector2 const& a, Vector2 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > 0.0 ? Vector2::sin_cos_range(a.dot(b) / mag) : 1.0);
}

// Cosine of Directed Angle abc Formed by Three Vector2s
inline double dir_cos(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return dir_cos(a - b, c - b);
}

// Sine of Directed Angle Between Two Vector2s
inline double dir_sin(Vector2 const& a, Vector2 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > 0.0 ? Vector2::sin_cos_range(a.cross(b) / mag) : 0.0);
}

// Sine of Directed Angle abc Formed by Three Vector2s
inline double dir_sin(Vector2 const& a, Vector2 const& b, Vector2 const& c) {
  return dir_sin(a - b, c - b);
}

// Stream << Vector2 output operator
inline std::ostream& operator<<(std::ostream& stream, Vector2 const& v) {
  constexpr std::streamsize precision = 16;  // Significant digits
  constexpr int width = 23;                  // Field width

  // Save current stream state and set persistent state
  std::ios_base::fmtflags const old_flags(stream.flags());
  std::streamsize const old_precision(stream.precision(precision));
  stream << std::right << std::showpoint << std::uppercase;

  // Output Vector2
  stream << std::setw(width) << v.x << ' ' << std::setw(width) << v.y;

  // Restore previous stream state
  stream.precision(old_precision);
  stream.flags(old_flags);

  return stream;
}

}  // namespace ObjexxFCL

#endif  // ObjexxFCL_Vector2_hh_INCLUDED
