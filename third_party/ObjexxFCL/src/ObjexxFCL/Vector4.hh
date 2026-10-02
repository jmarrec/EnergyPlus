#ifndef ObjexxFCL_Vector4_hh_INCLUDED
#define ObjexxFCL_Vector4_hh_INCLUDED

// Vector4: Fast 4-Element Vector
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
#include <cassert>
#include <compare>
#include <cstddef>
#include <iosfwd>
#include <type_traits>

namespace ObjexxFCL {

// Array-like type usable with Vector4's array interface: has a value_type convertible to double, operator[] and size()
// (e.g. std::array< double, 4 >, std::vector< double >)
template <typename A>
concept Array4Like = requires(A const& a) {
  typename A::value_type;
  a.size();
  a[0];
} && std::is_assignable_v<double&, typename A::value_type>;

// Vector4: Fast 4-Element Vector
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
// . Use std::array< double, 4 > instead in array/vectorization context
class Vector4
{

 public:  // Types
  using value_type = double;
  using reference = double&;
  using const_reference = double const&;
  using pointer = double*;
  using const_pointer = double const*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

 public:  // Creation
  // Default Constructor: Zero-Initializes All Elements
  Vector4() = default;

  // Uniform Value Constructor
  explicit Vector4(double t);

  // Value Constructor
  Vector4(double x_, double y_, double z_, double w_);

 public:  // Assignment
  // = Value
  Vector4& operator=(double t);

  // += Vector4
  Vector4& operator+=(Vector4 const& v);

  // -= Vector4
  Vector4& operator-=(Vector4 const& v);

  // *= Vector4
  Vector4& operator*=(Vector4 const& v);

  // /= Vector4
  Vector4& operator/=(Vector4 const& v);

  // += Value
  Vector4& operator+=(double t);

  // -= Value
  Vector4& operator-=(double t);

  // *= Value
  Vector4& operator*=(double t);

  // /= Value
  Vector4& operator/=(double u);

 public:  // Array Interface
  // Any type with operator[] and size() == 4 (e.g. std::array< double, 4 >)

  // Array Constructor Template
  template <Array4Like A>
  Vector4(A const& a) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {
    assert(a.size() == 4);
  }

  // = Array
  template <Array4Like A>
  Vector4& operator=(A const& a) {
    assert(a.size() == 4);
    x = a[0];
    y = a[1];
    z = a[2];
    w = a[3];
    return *this;
  }

  // += Array
  template <Array4Like A>
  Vector4& operator+=(A const& a) {
    assert(a.size() == 4);
    x += a[0];
    y += a[1];
    z += a[2];
    w += a[3];
    return *this;
  }

  // -= Array
  template <Array4Like A>
  Vector4& operator-=(A const& a) {
    assert(a.size() == 4);
    x -= a[0];
    y -= a[1];
    z -= a[2];
    w -= a[3];
    return *this;
  }

  // *= Array
  template <Array4Like A>
  Vector4& operator*=(A const& a) {
    assert(a.size() == 4);
    x *= a[0];
    y *= a[1];
    z *= a[2];
    w *= a[3];
    return *this;
  }

  // /= Array
  template <Array4Like A>
  Vector4& operator/=(A const& a) {
    assert(a.size() == 4);
    assert(a[0] != 0.0);
    assert(a[1] != 0.0);
    assert(a[2] != 0.0);
    assert(a[3] != 0.0);
    x /= a[0];
    y /= a[1];
    z /= a[2];
    w /= a[3];
    return *this;
  }

  // Dot Product with an Array
  template <Array4Like A>
  double dot(A const& a) const {
    assert(a.size() == 4);
    return (x * a[0]) + (y * a[1]) + (z * a[2]) + (w * a[3]);
  }

 public:  // Subscript
  // Vector4[ i ] const: 0-Based Index
  double operator[](size_type i) const;

  // Vector4[ i ]: 0-Based Index
  double& operator[](size_type i);

 public:  // Properties: General
  // Size
  size_type size() const;

  // Length (the L2 norm, i.e. the Euclidean norm: sqrt(x^2 + y^2 + z^2 + w^2))
  double length() const;

  // Length Squared
  double length_squared() const;

  // L1 Norm (Manhattan norm or taxicab norm)
  double norm_L1() const;

  // Distance to a Vector4
  double distance(Vector4 const& v) const;

  // Distance Squared to a Vector4
  double distance_squared(Vector4 const& v) const;

  // Dot Product with a Vector4
  double dot(Vector4 const& v) const;

 public:  // Modifiers
  // Normalize to a Length
  Vector4& normalize(double tar_length = 1.0);

  // Project Normal to a Vector4
  Vector4& project_normal(Vector4 const& v);

  // Project onto a Vector4
  Vector4& project_parallel(Vector4 const& v);

 public:  // Generators
  // -Vector4 (Negated)
  Vector4 operator-() const;

  // Normalized to a Length
  Vector4 normalized(double tar_length = 1.0) const;

  // Projected Normal to a Vector4
  Vector4 projected_normal(Vector4 const& v) const;

  // Projected onto a Vector4
  Vector4 projected_parallel(Vector4 const& v) const;

 public:  // Static Methods
  // Square of a value
  static double square(double t);

 public:  // Comparison
  // Lexicographic on (x, y, z, w)
  auto operator<=>(Vector4 const&) const = default;

 public:  // Data Elements
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double w = 0.0;
};  // Vector4

// Vector4 op Vector4
Vector4 operator+(Vector4 const& a, Vector4 const& b);
Vector4 operator-(Vector4 const& a, Vector4 const& b);
Vector4 operator*(Vector4 const& a, Vector4 const& b);
Vector4 operator/(Vector4 const& a, Vector4 const& b);

// Vector4 op Value
bool operator==(Vector4 const& v, double t);
bool operator!=(Vector4 const& v, double t);
bool operator<(Vector4 const& v, double t);
bool operator<=(Vector4 const& v, double t);
bool operator>=(Vector4 const& v, double t);
bool operator>(Vector4 const& v, double t);
Vector4 operator+(Vector4 const& v, double t);
Vector4 operator-(Vector4 const& v, double t);
Vector4 operator*(Vector4 const& v, double t);
Vector4 operator/(Vector4 const& v, double t);

// Value op Vector4
bool operator==(double t, Vector4 const& v);
bool operator!=(double t, Vector4 const& v);
bool operator<(double t, Vector4 const& v);
bool operator<=(double t, Vector4 const& v);
bool operator>=(double t, Vector4 const& v);
bool operator>(double t, Vector4 const& v);
Vector4 operator+(double t, Vector4 const& v);
Vector4 operator-(double t, Vector4 const& v);
Vector4 operator*(double t, Vector4 const& v);
Vector4 operator/(double t, Vector4 const& v);

// Stream << Vector4 output operator
std::ostream& operator<<(std::ostream& stream, Vector4 const& v);

}  // namespace ObjexxFCL

#endif  // ObjexxFCL_Vector4_hh_INCLUDED
