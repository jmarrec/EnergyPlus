// Vector4: Fast 4-Element Vector (Implementation)
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
#include <ObjexxFCL/Fmath.hh>
#include <ObjexxFCL/Vector4.hh>

// C++ Headers
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace ObjexxFCL {

// Default Constructor
Vector4::Vector4()
#if defined(OBJEXXFCL_ARRAY_INIT) || defined(OBJEXXFCL_ARRAY_INIT_DEBUG)
  : x(Traits::initial_array_value()),
    y(Traits::initial_array_value()),
    z(Traits::initial_array_value()),
    w(Traits::initial_array_value())
#endif
{
}

// Uniform Value Constructor
Vector4::Vector4(double t) : x(t), y(t), z(t), w(t) {}

// Value Constructor
Vector4::Vector4(double x_, double y_, double z_, double w_) : x(x_), y(y_), z(z_), w(w_) {}

// Default Vector Named Constructor
Vector4 Vector4::default_vector() {
  return Vector4(double());
}

// Zero Vector Named Constructor
Vector4 Vector4::zero_vector() {
  return Vector4(double(0));
}

// x Vector of Specified Length Named Constructor
Vector4 Vector4::x_vector(double tar_length) {
  return Vector4(tar_length, double(0), double(0), double(0));
}

// y Vector of Specified Length Named Constructor
Vector4 Vector4::y_vector(double tar_length) {
  return Vector4(double(0), tar_length, double(0), double(0));
}

// z Vector of Specified Length Named Constructor
Vector4 Vector4::z_vector(double tar_length) {
  return Vector4(double(0), double(0), tar_length, double(0));
}

// W Vector of Specified Length Named Constructor
Vector4 Vector4::W_vector(double tar_length) {
  return Vector4(double(0), double(0), double(0), tar_length);
}

// Uniform Vector of Specified Length Named Constructor
Vector4 Vector4::uniform_vector(double tar_length) {
  return Vector4(tar_length / double(2));
}

// += Vector4
Vector4& Vector4::operator+=(Vector4 const& v) {
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}

// -= Vector4
Vector4& Vector4::operator-=(Vector4 const& v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

// *= Vector4
Vector4& Vector4::operator*=(Vector4 const& v) {
  x *= v.x;
  y *= v.y;
  z *= v.z;
  w *= v.w;
  return *this;
}

// /= Vector4
Vector4& Vector4::operator/=(Vector4 const& v) {
  assert(v.x != double(0));
  assert(v.y != double(0));
  assert(v.z != double(0));
  assert(v.w != double(0));
  x /= v.x;
  y /= v.y;
  z /= v.z;
  w /= v.w;
  return *this;
}

// = Value
Vector4& Vector4::operator=(double t) {
  x = y = z = w = t;
  return *this;
}

// += Value
Vector4& Vector4::operator+=(double t) {
  x += t;
  y += t;
  z += t;
  w += t;
  return *this;
}

// -= Value
Vector4& Vector4::operator-=(double t) {
  x -= t;
  y -= t;
  z -= t;
  w -= t;
  return *this;
}

// *= Value
Vector4& Vector4::operator*=(double t) {
  x *= t;
  y *= t;
  z *= t;
  w *= t;
  return *this;
}

// /= Value
Vector4& Vector4::operator/=(double const u) {
  assert(u != 0.0);
  double const inv_u(1.0 / u);
  x *= inv_u;
  y *= inv_u;
  z *= inv_u;
  w *= inv_u;
  return *this;
}

// Value Assignment
Vector4& Vector4::assign(double x_, double y_, double z_, double w_) {
  x = x_;
  y = y_;
  z = z_;
  w = w_;
  return *this;
}

// Assign Value * Vector4
Vector4& Vector4::scaled_assign(double t, Vector4 const& v) {
  x = t * v.x;
  y = t * v.y;
  z = t * v.z;
  w = t * v.w;
  return *this;
}

// Add Value * Vector4
Vector4& Vector4::scaled_add(double t, Vector4 const& v) {
  x += t * v.x;
  y += t * v.y;
  z += t * v.z;
  w += t * v.w;
  return *this;
}

// Subtract Value * Vector4
Vector4& Vector4::scaled_sub(double t, Vector4 const& v) {
  x -= t * v.x;
  y -= t * v.y;
  z -= t * v.z;
  w -= t * v.w;
  return *this;
}

// Multiply by Value * Vector4
Vector4& Vector4::scaled_mul(double t, Vector4 const& v) {
  x *= t * v.x;
  y *= t * v.y;
  z *= t * v.z;
  w *= t * v.w;
  return *this;
}

// Divide by Value * Vector4
Vector4& Vector4::scaled_div(double t, Vector4 const& v) {
  assert(t != double(0));
  assert(v.x != double(0));
  assert(v.y != double(0));
  assert(v.z != double(0));
  assert(v.w != double(0));
  x /= t * v.x;
  y /= t * v.y;
  z /= t * v.z;
  w /= t * v.w;
  return *this;
}

// Vector4[ i ] const: 0-Based Index
double Vector4::operator[](size_type const i) const {
  assert(i <= 3);
  return (i < 2 ? (i == 0 ? x : y) : (i == 2 ? z : w));
}

// Vector4[ i ]: 0-Based Index
double& Vector4::operator[](size_type const i) {
  assert(i <= 3);
  return (i < 2 ? (i == 0 ? x : y) : (i == 2 ? z : w));
}

// Vector4( i ) const: 1-Based Index
double Vector4::operator()(size_type const i) const {
  assert((1 <= i) && (i <= 4));
  return (i <= 2 ? (i == 1 ? x : y) : (i == 3 ? z : w));
}

// Vector4( i ): 1-Based Index
double& Vector4::operator()(size_type const i) {
  assert((1 <= i) && (i <= 4));
  return (i <= 2 ? (i == 1 ? x : y) : (i == 3 ? z : w));
}

// Is Zero Vector?
bool Vector4::is_zero() const {
  static double const ZERO(0);
  return (x == ZERO) && (y == ZERO) && (z == ZERO) && (w == ZERO);
}

// Is Unit Vector?
bool Vector4::is_unit() const {
  return (length_squared() == double(1));
}

// Size
Vector4::Size Vector4::size() const {
  return 4u;
}

// Length
double Vector4::length() const {
  return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Length Squared
double Vector4::length_squared() const {
  return (x * x) + (y * y) + (z * z) + (w * w);
}

// Magnitude
double Vector4::magnitude() const {
  return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Magnitude
double Vector4::mag() const {
  return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Magnitude Squared
double Vector4::magnitude_squared() const {
  return (x * x) + (y * y) + (z * z) + (w * w);
}

// Magnitude Squared
double Vector4::mag_squared() const {
  return (x * x) + (y * y) + (z * z) + (w * w);
}

// L1 Norm
double Vector4::norm_L1() const {
  return std::abs(x) + std::abs(y) + std::abs(z) + std::abs(w);
}

// L2 Norm
double Vector4::norm_L2() const {
  return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// L-infinity Norm
double Vector4::norm_Linf() const {
  return ObjexxFCL::max(std::abs(x), std::abs(y), std::abs(z), std::abs(w));
}

// Distance to a Vector4
double Vector4::distance(Vector4 const& v) const {
  return std::sqrt(square(x - v.x) + square(y - v.y) + square(z - v.z) + square(w - v.w));
}

// Distance Squared to a Vector4
double Vector4::distance_squared(Vector4 const& v) const {
  return square(x - v.x) + square(y - v.y) + square(z - v.z) + square(w - v.w);
}

// Dot Product with a Vector4
double Vector4::dot(Vector4 const& v) const {
  return (x * v.x) + (y * v.y) + (z * v.z) + (w * v.w);
}

// Alias for Element 1
double Vector4::x1() const {
  return x;
}

// Alias for Element 1
double& Vector4::x1() {
  return x;
}

// Alias for Element 2
double Vector4::x2() const {
  return y;
}

// Alias for Element 2
double& Vector4::x2() {
  return y;
}

// Alias for Element 3
double Vector4::x3() const {
  return z;
}

// Alias for Element 3
double& Vector4::x3() {
  return z;
}

// Alias for Element 4
double Vector4::x4() const {
  return w;
}

// Alias for Element 4
double& Vector4::x4() {
  return w;
}

// Zero
Vector4& Vector4::zero() {
  x = y = z = w = double(0);
  return *this;
}

// Negate
Vector4& Vector4::negate() {
  x = -x;
  y = -y;
  z = -z;
  w = -w;
  return *this;
}

// Normalize to a Length
Vector4& Vector4::normalize(double tar_length) {
  double const cur_length(length());
  assert(cur_length != double(0));
  double const dilation(tar_length / cur_length);
  x *= dilation;
  y *= dilation;
  z *= dilation;
  w *= dilation;
  return *this;
}

// Normalize to a Length: Zero Vector4 if Length is Zero
Vector4& Vector4::normalize_zero(double tar_length) {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    x *= dilation;
    y *= dilation;
    z *= dilation;
    w *= dilation;
  } else {  // Set zero vector
    x = y = z = w = double(0);
  }
  return *this;
}

// Normalize to a Length: Uniform Vector4 if Length is Zero
Vector4& Vector4::normalize_uniform(double tar_length) {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    x *= dilation;
    y *= dilation;
    z *= dilation;
    w *= dilation;
  } else {  // Set uniform vector
    operator=(uniform_vector(tar_length));
  }
  return *this;
}

// Normalize to a Length: x Vector4 if Length is Zero
Vector4& Vector4::normalize_x(double tar_length) {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    x *= dilation;
    y *= dilation;
    z *= dilation;
    w *= dilation;
  } else {  // Set x vector
    x = tar_length;
    y = z = w = double(0);
  }
  return *this;
}

// Normalize to a Length: y Vector4 if Length is Zero
Vector4& Vector4::normalize_y(double tar_length) {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    x *= dilation;
    y *= dilation;
    z *= dilation;
    w *= dilation;
  } else {  // Set y vector
    y = tar_length;
    x = z = w = double(0);
  }
  return *this;
}

// Normalize to a Length: z Vector4 if Length is Zero
Vector4& Vector4::normalize_z(double tar_length) {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    x *= dilation;
    y *= dilation;
    z *= dilation;
    w *= dilation;
  } else {  // Set z vector
    z = tar_length;
    x = y = w = double(0);
  }
  return *this;
}

// Normalize to a Length: w Vector4 if Length is Zero
Vector4& Vector4::normalize_w(double tar_length) {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    x *= dilation;
    y *= dilation;
    z *= dilation;
    w *= dilation;
  } else {  // Set z vector
    w = tar_length;
    x = y = z = double(0);
  }
  return *this;
}

// Minimum Coordinates with a Vector4
Vector4& Vector4::min(Vector4 const& v) {
  x = (x <= v.x ? x : v.x);
  y = (y <= v.y ? y : v.y);
  z = (z <= v.z ? z : v.z);
  w = (w <= v.w ? w : v.w);
  return *this;
}

// Maximum Coordinates with a Vector4
Vector4& Vector4::max(Vector4 const& v) {
  x = (x >= v.x ? x : v.x);
  y = (y >= v.y ? y : v.y);
  z = (z >= v.z ? z : v.z);
  w = (w >= v.w ? w : v.w);
  return *this;
}

// Add a Vector4
Vector4& Vector4::add(Vector4 const& v) {
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}

// Sum a Vector4
Vector4& Vector4::sum(Vector4 const& v) {
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}

// Subtract a Vector4
Vector4& Vector4::sub(Vector4 const& v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

// Subtract a Vector4
Vector4& Vector4::subtract(Vector4 const& v) {
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

// Project Normal to a Vector4
Vector4& Vector4::project_normal(Vector4 const& v) {
  assert(v.length_squared() != double(0));
  double const c(dot(v) / v.length_squared());
  x -= c * v.x;
  y -= c * v.y;
  z -= c * v.z;
  w -= c * v.w;
  return *this;
}

// Project onto a Vector4
Vector4& Vector4::project_parallel(Vector4 const& v) {
  assert(v.length_squared() != double(0));
  double const c(dot(v) / v.length_squared());
  x = c * v.x;
  y = c * v.y;
  z = c * v.z;
  w = c * v.w;
  return *this;
}

// -Vector4 (Negated)
Vector4 Vector4::operator-() const {
  return Vector4(-x, -y, -z, -w);
}

// Negated
Vector4 Vector4::negated() const {
  return Vector4(-x, -y, -z, -w);
}

// Normalized to a Length
Vector4 Vector4::normalized(double tar_length) const {
  double const cur_length(length());
  assert(cur_length != double(0));
  double const dilation(tar_length / cur_length);
  return Vector4(x * dilation, y * dilation, z * dilation, w * dilation);
}

// Normalized to a Length: Zero Vector4 if Length is Zero
Vector4 Vector4::normalized_zero(double tar_length) const {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    return Vector4(x * dilation, y * dilation, z * dilation, w * dilation);
  } else {  // Return zero vector
    return Vector4(double(0));
  }
}

// Normalized to a Length: Uniform Vector4 if Length is Zero
Vector4 Vector4::normalized_uniform(double tar_length) const {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    return Vector4(x * dilation, y * dilation, z * dilation, w * dilation);
  } else {  // Return uniform vector
    return uniform_vector(tar_length);
  }
}

// Normalized to a Length: x Vector4 if Length is Zero
Vector4 Vector4::normalized_x(double tar_length) const {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    return Vector4(x * dilation, y * dilation, z * dilation, w * dilation);
  } else {  // Return x vector
    return Vector4(tar_length, double(0), double(0), double(0));
  }
}

// Normalized to a Length: y Vector4 if Length is Zero
Vector4 Vector4::normalized_y(double tar_length) const {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    return Vector4(x * dilation, y * dilation, z * dilation, w * dilation);
  } else {  // Return y vector
    return Vector4(double(0), tar_length, double(0), double(0));
  }
}

// Normalized to a Length: z Vector4 if Length is Zero
Vector4 Vector4::normalized_z(double tar_length) const {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    return Vector4(x * dilation, y * dilation, z * dilation, w * dilation);
  } else {  // Return z vector
    return Vector4(double(0), double(0), tar_length, double(0));
  }
}

// Normalized to a Length: w Vector4 if Length is Zero
Vector4 Vector4::normalized_w(double tar_length) const {
  double const cur_length(length());
  if (cur_length > double(0)) {
    double const dilation(tar_length / cur_length);
    return Vector4(x * dilation, y * dilation, z * dilation, w * dilation);
  } else {  // Return z vector
    return Vector4(double(0), double(0), double(0), tar_length);
  }
}

// Projected Normal to a Vector4
Vector4 Vector4::projected_normal(Vector4 const& v) const {
  assert(v.length_squared() != double(0));
  double const c(dot(v) / v.length_squared());
  return Vector4(x - (c * v.x), y - (c * v.y), z - (c * v.z), w - (c * v.w));
}

// Projected onto a Vector4
Vector4 Vector4::projected_parallel(Vector4 const& v) const {
  assert(v.length_squared() != double(0));
  double const c(dot(v) / v.length_squared());
  return Vector4(c * v.x, c * v.y, c * v.z, c * v.w);
}

// Square of a value
double Vector4::square(double t) {
  return t * t;
}

// Value Clipped to [-1,1]
double Vector4::sin_cos_range(double t) {
  return std::min(std::max(t, double(-1)), double(1));
}

// Add 2*Pi to a Negative Value
double Vector4::bump_up_angle(double t) {
  static double const Two_Pi(double(2) * std::acos(-1.0));
  return (t >= double(0) ? t : Two_Pi + t);
}

// Length
double length(Vector4 const& v) {
  return v.length();
}

// Length Squared
double length_squared(Vector4 const& v) {
  return v.length_squared();
}

// Magnitude
double magnitude(Vector4 const& v) {
  return v.magnitude();
}

// Magnitude
double mag(Vector4 const& v) {
  return v.mag();
}

// Magnitude Squared
double magnitude_squared(Vector4 const& v) {
  return v.magnitude_squared();
}

// Magnitude Squared
double mag_squared(Vector4 const& v) {
  return v.mag_squared();
}

// Vector4 == Vector4
bool operator==(Vector4 const& a, Vector4 const& b) {
  return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

// Vector4 != Vector4
bool operator!=(Vector4 const& a, Vector4 const& b) {
  return (a.x != b.x) || (a.y != b.y) || (a.z != b.z) || (a.w != b.w);
}

// Vector4 < Vector4: Lexicographic
bool operator<(Vector4 const& a, Vector4 const& b) {
  return ((a.x < b.x ? true
                     : (b.x < a.x ? false :  // a.x == b.x
                          (a.y < b.y ? true
                                     : (b.y < a.y ? false :  // a.y == b.y
                                          (a.z < b.z ? true
                                                     : (b.z < a.z ? false :  // a.z == b.z
                                                          (a.w < b.w))))))));
}

// Vector4 <= Vector4: Lexicographic
bool operator<=(Vector4 const& a, Vector4 const& b) {
  return ((a.x < b.x ? true
                     : (b.x < a.x ? false :  // a.x == b.x
                          (a.y < b.y ? true
                                     : (b.y < a.y ? false :  // a.y == b.y
                                          (a.z < b.z ? true
                                                     : (b.z < a.z ? false :  // a.z == b.z
                                                          (a.w <= b.w))))))));
}

// Vector4 >= Vector4: Lexicographic
bool operator>=(Vector4 const& a, Vector4 const& b) {
  return ((a.x > b.x ? true
                     : (b.x > a.x ? false :  // a.x == b.x
                          (a.y > b.y ? true
                                     : (b.y > a.y ? false :  // a.y == b.y
                                          (a.z > b.z ? true
                                                     : (b.z > a.z ? false :  // a.z == b.z
                                                          (a.w >= b.w))))))));
}

// Vector4 > Vector4: Lexicographic
bool operator>(Vector4 const& a, Vector4 const& b) {
  return ((a.x > b.x ? true
                     : (b.x > a.x ? false :  // a.x == b.x
                          (a.y > b.y ? true
                                     : (b.y > a.y ? false :  // a.y == b.y
                                          (a.z > b.z ? true
                                                     : (b.z > a.z ? false :  // a.z == b.z
                                                          (a.w > b.w))))))));
}

// Vector4 < Vector4: Element-wise
bool lt(Vector4 const& a, Vector4 const& b) {
  return (a.x < b.x) && (a.y < b.y) && (a.z < b.z) && (a.w < b.w);
}

// Vector4 <= Vector4: Element-wise
bool le(Vector4 const& a, Vector4 const& b) {
  return (a.x <= b.x) && (a.y <= b.y) && (a.z <= b.z) && (a.w <= b.w);
}

// Vector4 >= Vector4: Element-wise
bool ge(Vector4 const& a, Vector4 const& b) {
  return (a.x >= b.x) && (a.y >= b.y) && (a.z >= b.z) && (a.w >= b.w);
}

// Vector4 > Vector4: Element-wise
bool gt(Vector4 const& a, Vector4 const& b) {
  return (a.x > b.x) && (a.y > b.y) && (a.z > b.z) && (a.w > b.w);
}

// Vector4 == Value
bool operator==(Vector4 const& v, double t) {
  return (v.x == t) && (v.y == t) && (v.z == t) && (v.w == t);
}

// Vector4 != Value
bool operator!=(Vector4 const& v, double t) {
  return (v.x != t) || (v.y != t) || (v.z != t) || (v.w != t);
}

// Vector4 < Value
bool operator<(Vector4 const& v, double t) {
  return (v.x < t) && (v.y < t) && (v.z < t) && (v.w < t);
}

// Vector4 <= Value
bool operator<=(Vector4 const& v, double t) {
  return (v.x <= t) && (v.y <= t) && (v.z <= t) && (v.w <= t);
}

// Vector4 >= Value
bool operator>=(Vector4 const& v, double t) {
  return (v.x >= t) && (v.y >= t) && (v.z >= t) && (v.w >= t);
}

// Vector4 > Value
bool operator>(Vector4 const& v, double t) {
  return (v.x > t) && (v.y > t) && (v.z > t) && (v.w > t);
}

// Value == Vector4
bool operator==(double t, Vector4 const& v) {
  return (t == v.x) && (t == v.y) && (t == v.z) && (t == v.w);
}

// Value != Vector4
bool operator!=(double t, Vector4 const& v) {
  return (t != v.x) || (t != v.y) || (t != v.z) || (t != v.w);
}

// Value < Vector4
bool operator<(double t, Vector4 const& v) {
  return (t < v.x) && (t < v.y) && (t < v.z) && (t < v.w);
}

// Value <= Vector4
bool operator<=(double t, Vector4 const& v) {
  return (t <= v.x) && (t <= v.y) && (t <= v.z) && (t <= v.w);
}

// Value >= Vector4
bool operator>=(double t, Vector4 const& v) {
  return (t >= v.x) && (t >= v.y) && (t >= v.z) && (t >= v.w);
}

// Value > Vector4
bool operator>(double t, Vector4 const& v) {
  return (t > v.x) && (t > v.y) && (t > v.z) && (t > v.w);
}

// Equal Length?
bool equal_length(Vector4 const& a, Vector4 const& b) {
  return (a.length_squared() == b.length_squared());
}

// Not Equal Length?
bool not_equal_length(Vector4 const& a, Vector4 const& b) {
  return (a.length_squared() != b.length_squared());
}

// Vector4 + Vector4
Vector4 operator+(Vector4 const& a, Vector4 const& b) {
  return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

// Vector4 + Value
Vector4 operator+(Vector4 const& v, double t) {
  return Vector4(v.x + t, v.y + t, v.z + t, v.w + t);
}

// Value + Vector4
Vector4 operator+(double t, Vector4 const& v) {
  return Vector4(t + v.x, t + v.y, t + v.z, t + v.w);
}

// Vector4 - Vector4
Vector4 operator-(Vector4 const& a, Vector4 const& b) {
  return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

// Vector4 - Value
Vector4 operator-(Vector4 const& v, double t) {
  return Vector4(v.x - t, v.y - t, v.z - t, v.w - t);
}

// Value - Vector4
Vector4 operator-(double t, Vector4 const& v) {
  return Vector4(t - v.x, t - v.y, t - v.z, t - v.w);
}

// Vector4 * Vector4
Vector4 operator*(Vector4 const& a, Vector4 const& b) {
  return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

// Vector4 * Value
Vector4 operator*(Vector4 const& v, double t) {
  return Vector4(v.x * t, v.y * t, v.z * t, v.w * t);
}

// Value * Vector4
Vector4 operator*(double t, Vector4 const& v) {
  return Vector4(t * v.x, t * v.y, t * v.z, t * v.w);
}

// Vector4 / Vector4
Vector4 operator/(Vector4 const& a, Vector4 const& b) {
  assert(b.x != double(0));
  assert(b.y != double(0));
  assert(b.z != double(0));
  assert(b.w != double(0));
  return Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

// Vector4 / Value
Vector4 operator/(Vector4 const& v, double const u) {
  assert(u != 0.0);
  double const inv_u(1.0 / u);
  return Vector4(v.x * inv_u, v.y * inv_u, v.z * inv_u, v.w * inv_u);
}

// Value / Vector4
Vector4 operator/(double t, Vector4 const& v) {
  assert(v.x != double(0));
  assert(v.y != double(0));
  assert(v.z != double(0));
  assert(v.w != double(0));
  return Vector4(t / v.x, t / v.y, t / v.z, t / v.w);
}

// Minimum of Two Vector4s
Vector4 min(Vector4 const& a, Vector4 const& b) {
  return Vector4((a.x <= b.x ? a.x : b.x), (a.y <= b.y ? a.y : b.y), (a.z <= b.z ? a.z : b.z), (a.w <= b.w ? a.w : b.w));
}

// Minimum of Three Vector4s
Vector4 min(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  return Vector4(ObjexxFCL::min(a.x, b.x, c.x), ObjexxFCL::min(a.y, b.y, c.y), ObjexxFCL::min(a.z, b.z, c.z), ObjexxFCL::min(a.w, b.w, c.w));
}

// Minimum of Four Vector4s
Vector4 min(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d) {
  return Vector4(ObjexxFCL::min(a.x, b.x, c.x, d.x), ObjexxFCL::min(a.y, b.y, c.y, d.y), ObjexxFCL::min(a.z, b.z, c.z, d.z),
                 ObjexxFCL::min(a.w, b.w, c.w, d.w));
}

// Maximum of Two Vector4s
Vector4 max(Vector4 const& a, Vector4 const& b) {
  return Vector4((a.x >= b.x ? a.x : b.x), (a.y >= b.y ? a.y : b.y), (a.z >= b.z ? a.z : b.z), (a.w >= b.w ? a.w : b.w));
}

// Maximum of Three Vector4s
Vector4 max(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  return Vector4(ObjexxFCL::max(a.x, b.x, c.x), ObjexxFCL::max(a.y, b.y, c.y), ObjexxFCL::max(a.z, b.z, c.z), ObjexxFCL::max(a.w, b.w, c.w));
}

// Maximum of Four Vector4s
Vector4 max(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d) {
  return Vector4(ObjexxFCL::max(a.x, b.x, c.x, d.x), ObjexxFCL::max(a.y, b.y, c.y, d.y), ObjexxFCL::max(a.z, b.z, c.z, d.z),
                 ObjexxFCL::max(a.w, b.w, c.w, d.w));
}

// Sum of Two Vector4s
Vector4 sum(Vector4 const& a, Vector4 const& b) {
  return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

// Sum of Three Vector4s
Vector4 sum(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  return Vector4(a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z, a.w + b.w + c.w);
}

// Sum of Four Vector4s
Vector4 sum(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d) {
  return Vector4(a.x + b.x + c.x + d.x, a.y + b.y + c.y + d.y, a.z + b.z + c.z + d.z, a.w + b.w + c.w + d.w);
}

// Subtract of Two Vector4s
Vector4 sub(Vector4 const& a, Vector4 const& b) {
  return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

// Subtract of Two Vector4s
Vector4 subtract(Vector4 const& a, Vector4 const& b) {
  return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

// Midpoint of Two Vector4s
Vector4 mid(Vector4 const& a, Vector4 const& b) {
  return Vector4(double(0.5 * (a.x + b.x)), double(0.5 * (a.y + b.y)), double(0.5 * (a.z + b.z)), double(0.5 * (a.w + b.w)));
}

// Center of Two Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b) {
  return Vector4(double(0.5 * (a.x + b.x)), double(0.5 * (a.y + b.y)), double(0.5 * (a.z + b.z)), double(0.5 * (a.w + b.w)));
}

// Center of Three Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  static long double const third(1.0 / 3.0);
  return Vector4(double(third * (a.x + b.x + c.x)), double(third * (a.y + b.y + c.y)), double(third * (a.z + b.z + c.z)),
                 double(third * (a.w + b.w + c.w)));
}

// Center of Four Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d) {
  return Vector4(double(0.25 * (a.x + b.x + c.x + d.x)), double(0.25 * (a.y + b.y + c.y + d.y)), double(0.25 * (a.z + b.z + c.z + d.z)),
                 double(0.25 * (a.w + b.w + c.w + d.w)));
}

// Distance
double distance(Vector4 const& a, Vector4 const& b) {
  return std::sqrt(Vector4::square(a.x - b.x) + Vector4::square(a.y - b.y) + Vector4::square(a.z - b.z) + Vector4::square(a.w - b.w));
}

// Distance Squared
double distance_squared(Vector4 const& a, Vector4 const& b) {
  return Vector4::square(a.x - b.x) + Vector4::square(a.y - b.y) + Vector4::square(a.z - b.z) + Vector4::square(a.w - b.w);
}

// Dot Product
double dot(Vector4 const& a, Vector4 const& b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

// Angle Between Two Vector4s (in Radians on [0,pi])
double angle(Vector4 const& a, Vector4 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > double(0) ? std::acos(Vector4::sin_cos_range(a.dot(b) / mag)) : double(0));
}

// Angle abc Formed by Three Vector4s (in Radians on [0,pi])
double angle(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  return angle(a - b, c - b);
}

// Cosine of Angle Between Two Vector4s
double cos(Vector4 const& a, Vector4 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > double(0) ? Vector4::sin_cos_range(a.dot(b) / mag) : double(1));
}

// Cosine of Angle abc Formed by Three Vector4s
double cos(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  return cos(a - b, c - b);
}

// Sine of Angle Between Two Vector4s
double sin(Vector4 const& a, Vector4 const& b) {
  return std::sin(angle(a, b));
}

// Sine of Angle abc Formed by Three Vector4s
double sin(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  return sin(a - b, c - b);
}

// Stream << Vector4 output operator
std::ostream& operator<<(std::ostream& stream, Vector4 const& v) {
  // Types
  typedef TypeTraits<double> Traits;

  // Save current stream state and set persistent state
  std::ios_base::fmtflags const old_flags(stream.flags());
  std::streamsize const old_precision(stream.precision(Traits::precision));
  stream << std::right << std::showpoint << std::uppercase;

  // Output Vector4
  std::size_t const w(Traits::width);
  stream << std::setw(w) << v.x << ' ' << std::setw(w) << v.y << ' ' << std::setw(w) << v.z << ' ' << std::setw(w) << v.w;

  // Restore previous stream state
  stream.precision(old_precision);
  stream.flags(old_flags);

  return stream;
}

// Stream >> Vector4 input operator
//  Supports whitespace-separated values with optional commas between values as long as whitespace is also present
//  String or char values containing whitespace or commas or enclosed in quotes are not supported
//  Vector can optionally be enclosed in parentheses () or square brackets []
std::istream& operator>>(std::istream& stream, Vector4& v) {
  bool parens(false);    // Opening ( present?
  bool brackets(false);  // Opening [ present?

  {  // x
    std::string input_string;
    stream >> input_string;
    if (input_string == "(") {  // Skip opening (
      stream >> input_string;
      parens = true;
    } else if (input_string[0] == '(') {  // Skip opening (
      input_string.erase(0, 1);
      brackets = true;
    } else if (input_string == "[") {  // Skip opening [
      stream >> input_string;
      brackets = true;
    } else if (input_string[0] == '[') {  // Skip opening [
      input_string.erase(0, 1);
      brackets = true;
    }
    std::string::size_type const input_size(input_string.size());
    if ((input_size > 0) && (input_string[input_size - 1] == ',')) {
      input_string.erase(input_size - 1);  // Remove trailing ,
    }
    std::istringstream num_stream(input_string);
    num_stream >> v.x;
  }

  {  // y
    std::string input_string;
    stream >> input_string;
    if (input_string == ",") {  // Skip ,
      stream >> input_string;
    } else if (input_string[0] == ',') {  // Skip leading ,
      input_string.erase(0, 1);
    }
    std::string::size_type const input_size(input_string.size());
    if ((input_size > 0) && (input_string[input_size - 1] == ',')) {
      input_string.erase(input_size - 1);  // Remove trailing ,
    }
    std::istringstream num_stream(input_string);
    num_stream >> v.y;
  }

  {  // z
    std::string input_string;
    stream >> input_string;
    if (input_string == ",") {  // Skip ,
      stream >> input_string;
    } else if (input_string[0] == ',') {  // Skip leading ,
      input_string.erase(0, 1);
    }
    std::string::size_type const input_size(input_string.size());
    if ((input_size > 0) && (input_string[input_size - 1] == ',')) {
      input_string.erase(input_size - 1);  // Remove trailing ,
    }
    std::istringstream num_stream(input_string);
    num_stream >> v.z;
  }

  {  // w
    std::string input_string;
    stream >> input_string;
    if (input_string == ",") {  // Skip ,
      stream >> input_string;
    } else if (input_string[0] == ',') {  // Skip leading ,
      input_string.erase(0, 1);
    }
    std::string::size_type input_size(input_string.size());
    if (parens || brackets) {  // Remove closing ) or ]
      if (input_size > 0) {
        if (parens) {
          if (input_string[input_size - 1] == ')') {  // Remove closing )
            input_string.erase(input_size - 1);
            --input_size;
          }
        } else if (brackets) {
          if (input_string[input_size - 1] == ']') {  // Remove closing ]
            input_string.erase(input_size - 1);
            --input_size;
          }
        }
      }
    }
    if ((input_size > 0) && (input_string[input_size - 1] == ',')) {
      input_string.erase(input_size - 1);  // Remove trailing ,
    }
    std::istringstream num_stream(input_string);
    num_stream >> v.w;
  }

  // Remove closing ) or ] if opening ( or [ present
  if (parens || brackets) {  // Remove closing ) or ]
    while ((stream.peek() == ' ') || (stream.peek() == '\t')) {
      stream.ignore();
    }
    if (parens) {  // Remove closing ) if present
      if (stream.peek() == ')') stream.ignore();
    } else if (brackets) {  // Remove closing ] if present
      if (stream.peek() == ']') stream.ignore();
    }
  }

  return stream;
}

}  // namespace ObjexxFCL
