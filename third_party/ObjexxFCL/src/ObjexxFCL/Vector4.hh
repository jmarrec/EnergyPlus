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

// ObjexxFCL Headers
#include <ObjexxFCL/TypeTraits.hh>
#include <ObjexxFCL/Vector4.fwd.hh>

// C++ Headers
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iosfwd>
#include <type_traits>

namespace ObjexxFCL {

// Vector4: Fast 4-Element Vector
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
// . Use std::array< double, 4 > instead in array/vectorization context
class Vector4
{

 public:  // Types
  using Traits = TypeTraits<double>;

  // STL Style
  using value_type = double;
  using reference = double&;
  using const_reference = double const&;
  using pointer = double*;
  using const_pointer = double const*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  // C++ Style
  using Value = double;
  using Reference = double&;
  using ConstReference = double const&;
  using Pointer = double*;
  using ConstPointer = double const*;
  using Size = std::size_t;
  using Difference = std::ptrdiff_t;

 public:  // Creation
  // Default Constructor
  Vector4();

  // Copy Constructor
  Vector4(Vector4 const& v);

  // Uniform Value Constructor
  explicit Vector4(double t);

  // Value Constructor
  Vector4(double x_, double y_, double z_, double w_);

  // Initializer List Constructor Template
  template <typename U, class = typename std::enable_if<std::is_constructible<double, U>::value>::type>
  Vector4(std::initializer_list<U> const l) : x(*l.begin()), y(*(l.begin() + 1)), z(*(l.begin() + 2)), w(*(l.begin() + 3)) {
    assert(l.size() == 4);
  }

  // Array Constructor Template
  template <typename A, class = typename std::enable_if<std::is_constructible<double, typename A::value_type>::value>::type>
  Vector4(A const& a) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {
    assert(a.size() == 4);
  }

  // Default Vector Named Constructor
  static Vector4 default_vector();

  // Zero Vector Named Constructor
  static Vector4 zero_vector();

  // x Vector of Specified Length Named Constructor
  static Vector4 x_vector(double tar_length = 1.0);

  // y Vector of Specified Length Named Constructor
  static Vector4 y_vector(double tar_length = 1.0);

  // z Vector of Specified Length Named Constructor
  static Vector4 z_vector(double tar_length = 1.0);

  // W Vector of Specified Length Named Constructor
  static Vector4 W_vector(double tar_length = 1.0);

  // Uniform Vector of Specified Length Named Constructor
  static Vector4 uniform_vector(double tar_length = 1.0);

  // Destructor
  ~Vector4();

 public:  // Assignment
  // Copy Assignment
  Vector4& operator=(Vector4 const& v);

  // Initializer List Assignment Template
  template <typename U, class = typename std::enable_if<std::is_assignable<double&, U>::value>::type>
  Vector4& operator=(std::initializer_list<U> const l) {
    assert(l.size() == 4);
    auto i(l.begin());
    x = *i;
    y = *(++i);
    z = *(++i);
    w = *(++i);
    return *this;
  }

  // Array Assignment Template
  template <typename A, class = typename std::enable_if<std::is_assignable<double&, typename A::value_type>::value>::type>
  Vector4& operator=(A const& a) {
    assert(a.size() == 4);
    x = a[0];
    y = a[1];
    z = a[2];
    w = a[3];
    return *this;
  }

  // += Vector4
  Vector4& operator+=(Vector4 const& v);

  // -= Vector4
  Vector4& operator-=(Vector4 const& v);

  // *= Vector4
  Vector4& operator*=(Vector4 const& v);

  // /= Vector4
  Vector4& operator/=(Vector4 const& v);

  // += Initializer List
  template <typename U, class = typename std::enable_if<std::is_assignable<double&, U>::value>::type>
  Vector4& operator+=(std::initializer_list<U> const l) {
    assert(l.size() == 4);
    auto i(l.begin());
    x += *i;
    y += *(++i);
    z += *(++i);
    w += *(++i);
    return *this;
  }

  // -= Initializer List
  template <typename U, class = typename std::enable_if<std::is_assignable<double&, U>::value>::type>
  Vector4& operator-=(std::initializer_list<U> const l) {
    assert(l.size() == 4);
    auto i(l.begin());
    x -= *i;
    y -= *(++i);
    z -= *(++i);
    w -= *(++i);
    return *this;
  }

  // *= Initializer List
  template <typename U, class = typename std::enable_if<std::is_assignable<double&, U>::value>::type>
  Vector4& operator*=(std::initializer_list<U> const l) {
    assert(l.size() == 4);
    auto i(l.begin());
    x *= *i;
    y *= *(++i);
    z *= *(++i);
    w *= *(++i);
    return *this;
  }

  // /= Initializer List
  template <typename U, class = typename std::enable_if<std::is_assignable<double&, U>::value>::type>
  Vector4& operator/=(std::initializer_list<U> const l) {
    assert(l.size() == 4);
    auto i(l.begin());
    assert(*i != 0.0);
    assert(*(i + 1) != 0.0);
    assert(*(i + 2) != 0.0);
    assert(*(i + 3) != 0.0);
    x /= *i;
    y /= *(++i);
    z /= *(++i);
    w /= *(++i);
    return *this;
  }

  // += Array
  template <typename A, class = typename std::enable_if<std::is_assignable<double&, typename A::value_type>::value>::type>
  Vector4& operator+=(A const& a) {
    assert(a.size() == 4);
    x += a[0];
    y += a[1];
    z += a[2];
    w += a[3];
    return *this;
  }

  // -= Array
  template <typename A, class = typename std::enable_if<std::is_assignable<double&, typename A::value_type>::value>::type>
  Vector4& operator-=(A const& a) {
    assert(a.size() == 4);
    x -= a[0];
    y -= a[1];
    z -= a[2];
    w -= a[3];
    return *this;
  }

  // *= Array
  template <typename A, class = typename std::enable_if<std::is_assignable<double&, typename A::value_type>::value>::type>
  Vector4& operator*=(A const& a) {
    assert(a.size() == 4);
    x *= a[0];
    y *= a[1];
    z *= a[2];
    w *= a[3];
    return *this;
  }

  // /= Array
  template <typename A, class = typename std::enable_if<std::is_assignable<double&, typename A::value_type>::value>::type>
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

  // = Value
  Vector4& operator=(double t);

  // += Value
  Vector4& operator+=(double t);

  // -= Value
  Vector4& operator-=(double t);

  // *= Value
  Vector4& operator*=(double t);

  // /= Value
  Vector4& operator/=(double const u);

  // Value Assignment
  Vector4& assign(double x_, double y_, double z_, double w_);

 public:  // Assignment: Scaled
  // Assign Value * Vector4
  Vector4& scaled_assign(double t, Vector4 const& v);

  // Add Value * Vector4
  Vector4& scaled_add(double t, Vector4 const& v);

  // Subtract Value * Vector4
  Vector4& scaled_sub(double t, Vector4 const& v);

  // Multiply by Value * Vector4
  Vector4& scaled_mul(double t, Vector4 const& v);

  // Divide by Value * Vector4
  Vector4& scaled_div(double t, Vector4 const& v);

 public:  // Subscript
  // Vector4[ i ] const: 0-Based Index
  double operator[](size_type const i) const;

  // Vector4[ i ]: 0-Based Index
  double& operator[](size_type const i);

  // Vector4( i ) const: 1-Based Index
  double operator()(size_type const i) const;

  // Vector4( i ): 1-Based Index
  double& operator()(size_type const i);

 public:  // Properties: Predicates
  // Is Zero Vector?
  bool is_zero() const;

  // Is Unit Vector?
  bool is_unit() const;

 public:  // Properties: General
  // Size
  Size size() const;

  // Length
  double length() const;

  // Length Squared
  double length_squared() const;

  // Magnitude
  double magnitude() const;

  // Magnitude
  double mag() const;

  // Magnitude Squared
  double magnitude_squared() const;

  // Magnitude Squared
  double mag_squared() const;

  // L1 Norm
  double norm_L1() const;

  // L2 Norm
  double norm_L2() const;

  // L-infinity Norm
  double norm_Linf() const;

  // Distance to a Vector4
  double distance(Vector4 const& v) const;

  // Distance Squared to a Vector4
  double distance_squared(Vector4 const& v) const;

  // Dot Product with a Vector4
  double dot(Vector4 const& v) const;

  // Dot Product with an Array
  template <typename A, class = typename std::enable_if<std::is_assignable<double&, typename A::value_type>::value>::type>
  double dot(A const& a) const {
    assert(a.size() == 4);
    return (x * a[0]) + (y * a[1]) + (z * a[2]) + (w * a[3]);
  }

  // Alias for Element 1
  double x1() const;

  // Alias for Element 1
  double& x1();

  // Alias for Element 2
  double x2() const;

  // Alias for Element 2
  double& x2();

  // Alias for Element 3
  double x3() const;

  // Alias for Element 3
  double& x3();

  // Alias for Element 4
  double x4() const;

  // Alias for Element 4
  double& x4();

 public:  // Modifiers
  // Zero
  Vector4& zero();

  // Negate
  Vector4& negate();

  // Normalize to a Length
  Vector4& normalize(double tar_length = 1.0);

  // Normalize to a Length: Zero Vector4 if Length is Zero
  Vector4& normalize_zero(double tar_length = 1.0);

  // Normalize to a Length: Uniform Vector4 if Length is Zero
  Vector4& normalize_uniform(double tar_length = 1.0);

  // Normalize to a Length: x Vector4 if Length is Zero
  Vector4& normalize_x(double tar_length = 1.0);

  // Normalize to a Length: y Vector4 if Length is Zero
  Vector4& normalize_y(double tar_length = 1.0);

  // Normalize to a Length: z Vector4 if Length is Zero
  Vector4& normalize_z(double tar_length = 1.0);

  // Normalize to a Length: w Vector4 if Length is Zero
  Vector4& normalize_w(double tar_length = 1.0);

  // Minimum Coordinates with a Vector4
  Vector4& min(Vector4 const& v);

  // Maximum Coordinates with a Vector4
  Vector4& max(Vector4 const& v);

  // Add a Vector4
  Vector4& add(Vector4 const& v);

  // Sum a Vector4
  Vector4& sum(Vector4 const& v);

  // Subtract a Vector4
  Vector4& sub(Vector4 const& v);

  // Subtract a Vector4
  Vector4& subtract(Vector4 const& v);

  // Project Normal to a Vector4
  Vector4& project_normal(Vector4 const& v);

  // Project onto a Vector4
  Vector4& project_parallel(Vector4 const& v);

 public:  // Generators
  // -Vector4 (Negated)
  Vector4 operator-() const;

  // Negated
  Vector4 negated() const;

  // Normalized to a Length
  Vector4 normalized(double tar_length = 1.0) const;

  // Normalized to a Length: Zero Vector4 if Length is Zero
  Vector4 normalized_zero(double tar_length = 1.0) const;

  // Normalized to a Length: Uniform Vector4 if Length is Zero
  Vector4 normalized_uniform(double tar_length = 1.0) const;

  // Normalized to a Length: x Vector4 if Length is Zero
  Vector4 normalized_x(double tar_length = 1.0) const;

  // Normalized to a Length: y Vector4 if Length is Zero
  Vector4 normalized_y(double tar_length = 1.0) const;

  // Normalized to a Length: z Vector4 if Length is Zero
  Vector4 normalized_z(double tar_length = 1.0) const;

  // Normalized to a Length: w Vector4 if Length is Zero
  Vector4 normalized_w(double tar_length = 1.0) const;

  // Projected Normal to a Vector4
  Vector4 projected_normal(Vector4 const& v) const;

  // Projected onto a Vector4
  Vector4 projected_parallel(Vector4 const& v) const;

 public:  // Static Methods
  // Square of a value
  static double square(double t);

  // Value Clipped to [-1,1]
  static double sin_cos_range(double t);

  // Add 2*Pi to a Negative Value
  static double bump_up_angle(double t);

 public:              // Data
  double x, y, z, w;  // Elements

};  // Vector4

// Length
double length(Vector4 const& v);

// Length Squared
double length_squared(Vector4 const& v);

// Magnitude
double magnitude(Vector4 const& v);

// Magnitude
double mag(Vector4 const& v);

// Magnitude Squared
double magnitude_squared(Vector4 const& v);

// Magnitude Squared
double mag_squared(Vector4 const& v);

// Vector4 == Vector4
bool operator==(Vector4 const& a, Vector4 const& b);

// Vector4 != Vector4
bool operator!=(Vector4 const& a, Vector4 const& b);

// Vector4 < Vector4: Lexicographic
bool operator<(Vector4 const& a, Vector4 const& b);

// Vector4 <= Vector4: Lexicographic
bool operator<=(Vector4 const& a, Vector4 const& b);

// Vector4 >= Vector4: Lexicographic
bool operator>=(Vector4 const& a, Vector4 const& b);

// Vector4 > Vector4: Lexicographic
bool operator>(Vector4 const& a, Vector4 const& b);

// Vector4 < Vector4: Element-wise
bool lt(Vector4 const& a, Vector4 const& b);

// Vector4 <= Vector4: Element-wise
bool le(Vector4 const& a, Vector4 const& b);

// Vector4 >= Vector4: Element-wise
bool ge(Vector4 const& a, Vector4 const& b);

// Vector4 > Vector4: Element-wise
bool gt(Vector4 const& a, Vector4 const& b);

// Vector4 == Value
bool operator==(Vector4 const& v, double t);

// Vector4 != Value
bool operator!=(Vector4 const& v, double t);

// Vector4 < Value
bool operator<(Vector4 const& v, double t);

// Vector4 <= Value
bool operator<=(Vector4 const& v, double t);

// Vector4 >= Value
bool operator>=(Vector4 const& v, double t);

// Vector4 > Value
bool operator>(Vector4 const& v, double t);

// Value == Vector4
bool operator==(double t, Vector4 const& v);

// Value != Vector4
bool operator!=(double t, Vector4 const& v);

// Value < Vector4
bool operator<(double t, Vector4 const& v);

// Value <= Vector4
bool operator<=(double t, Vector4 const& v);

// Value >= Vector4
bool operator>=(double t, Vector4 const& v);

// Value > Vector4
bool operator>(double t, Vector4 const& v);

// Equal Length?
bool equal_length(Vector4 const& a, Vector4 const& b);

// Not Equal Length?
bool not_equal_length(Vector4 const& a, Vector4 const& b);

// Vector4 + Vector4
Vector4 operator+(Vector4 const& a, Vector4 const& b);

// Vector4 + Value
Vector4 operator+(Vector4 const& v, double t);

// Value + Vector4
Vector4 operator+(double t, Vector4 const& v);

// Vector4 - Vector4
Vector4 operator-(Vector4 const& a, Vector4 const& b);

// Vector4 - Value
Vector4 operator-(Vector4 const& v, double t);

// Value - Vector4
Vector4 operator-(double t, Vector4 const& v);

// Vector4 * Vector4
Vector4 operator*(Vector4 const& a, Vector4 const& b);

// Vector4 * Value
Vector4 operator*(Vector4 const& v, double t);

// Value * Vector4
Vector4 operator*(double t, Vector4 const& v);

// Vector4 / Vector4
Vector4 operator/(Vector4 const& a, Vector4 const& b);

// Vector4 / Value
Vector4 operator/(Vector4 const& v, double const u);

// Value / Vector4
Vector4 operator/(double t, Vector4 const& v);

// Minimum of Two Vector4s
Vector4 min(Vector4 const& a, Vector4 const& b);

// Minimum of Three Vector4s
Vector4 min(Vector4 const& a, Vector4 const& b, Vector4 const& c);

// Minimum of Four Vector4s
Vector4 min(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d);

// Maximum of Two Vector4s
Vector4 max(Vector4 const& a, Vector4 const& b);

// Maximum of Three Vector4s
Vector4 max(Vector4 const& a, Vector4 const& b, Vector4 const& c);

// Maximum of Four Vector4s
Vector4 max(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d);

// Sum of Two Vector4s
Vector4 sum(Vector4 const& a, Vector4 const& b);

// Sum of Three Vector4s
Vector4 sum(Vector4 const& a, Vector4 const& b, Vector4 const& c);

// Sum of Four Vector4s
Vector4 sum(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d);

// Subtract of Two Vector4s
Vector4 sub(Vector4 const& a, Vector4 const& b);

// Subtract of Two Vector4s
Vector4 subtract(Vector4 const& a, Vector4 const& b);

// Midpoint of Two Vector4s
Vector4 mid(Vector4 const& a, Vector4 const& b);

// Center of Two Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b);

// Center of Three Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b, Vector4 const& c);

// Center of Four Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d);

// Distance
double distance(Vector4 const& a, Vector4 const& b);

// Distance Squared
double distance_squared(Vector4 const& a, Vector4 const& b);

// Dot Product
double dot(Vector4 const& a, Vector4 const& b);

// Angle Between Two Vector4s (in Radians on [0,pi])
double angle(Vector4 const& a, Vector4 const& b);

// Angle abc Formed by Three Vector4s (in Radians on [0,pi])
double angle(Vector4 const& a, Vector4 const& b, Vector4 const& c);

// Cosine of Angle Between Two Vector4s
double cos(Vector4 const& a, Vector4 const& b);

// Cosine of Angle abc Formed by Three Vector4s
double cos(Vector4 const& a, Vector4 const& b, Vector4 const& c);

// Sine of Angle Between Two Vector4s
double sin(Vector4 const& a, Vector4 const& b);

// Sine of Angle abc Formed by Three Vector4s
double sin(Vector4 const& a, Vector4 const& b, Vector4 const& c);

// Stream << Vector4 output operator
std::ostream& operator<<(std::ostream& stream, Vector4 const& v);

// Stream >> Vector4 input operator
//  Supports whitespace-separated values with optional commas between values as long as whitespace is also present
//  String or char values containing whitespace or commas or enclosed in quotes are not supported
//  Vector can optionally be enclosed in parentheses () or square brackets []
std::istream& operator>>(std::istream& stream, Vector4& v);

}  // namespace ObjexxFCL

#endif  // ObjexxFCL_Vector4_hh_INCLUDED
