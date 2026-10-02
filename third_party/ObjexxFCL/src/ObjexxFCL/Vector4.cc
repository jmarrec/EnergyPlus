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
#include <ObjexxFCL/Vector4.hh>

// C++ Headers
#include <cassert>
#include <cmath>
#include <iomanip>
#include <ostream>

namespace ObjexxFCL {

// Uniform Value Constructor
Vector4::Vector4(double t) : x(t), y(t), z(t), w(t) {}

// Value Constructor
Vector4::Vector4(double x_, double y_, double z_, double w_) : x(x_), y(y_), z(z_), w(w_) {}

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
  assert(v.x != 0.0);
  assert(v.y != 0.0);
  assert(v.z != 0.0);
  assert(v.w != 0.0);
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

// Size
Vector4::size_type Vector4::size() const {
  return 4u;
}

// Length (L2 norm)
double Vector4::length() const {
  return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

// Length Squared
double Vector4::length_squared() const {
  return (x * x) + (y * y) + (z * z) + (w * w);
}

// L1 Norm
double Vector4::norm_L1() const {
  return std::abs(x) + std::abs(y) + std::abs(z) + std::abs(w);
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

// Normalize to a Length
Vector4& Vector4::normalize(double tar_length) {
  double const cur_length(length());
  assert(cur_length != 0.0);
  double const dilation(tar_length / cur_length);
  x *= dilation;
  y *= dilation;
  z *= dilation;
  w *= dilation;
  return *this;
}

// Project Normal to a Vector4
Vector4& Vector4::project_normal(Vector4 const& v) {
  assert(v.length_squared() != 0.0);
  double const c(dot(v) / v.length_squared());
  x -= c * v.x;
  y -= c * v.y;
  z -= c * v.z;
  w -= c * v.w;
  return *this;
}

// Project onto a Vector4
Vector4& Vector4::project_parallel(Vector4 const& v) {
  assert(v.length_squared() != 0.0);
  double const c(dot(v) / v.length_squared());
  x = c * v.x;
  y = c * v.y;
  z = c * v.z;
  w = c * v.w;
  return *this;
}

// -Vector4 (Negated)
Vector4 Vector4::operator-() const {
  return {-x, -y, -z, -w};
}

// Normalized to a Length
Vector4 Vector4::normalized(double tar_length) const {
  double const cur_length(length());
  assert(cur_length != 0.0);
  double const dilation(tar_length / cur_length);
  return {x * dilation, y * dilation, z * dilation, w * dilation};
}

// Projected Normal to a Vector4
Vector4 Vector4::projected_normal(Vector4 const& v) const {
  assert(v.length_squared() != 0.0);
  double const c(dot(v) / v.length_squared());
  return {x - (c * v.x), y - (c * v.y), z - (c * v.z), w - (c * v.w)};
}

// Projected onto a Vector4
Vector4 Vector4::projected_parallel(Vector4 const& v) const {
  assert(v.length_squared() != 0.0);
  double const c(dot(v) / v.length_squared());
  return {c * v.x, c * v.y, c * v.z, c * v.w};
}

// Square of a value
double Vector4::square(double t) {
  return t * t;
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

// Vector4 + Vector4
Vector4 operator+(Vector4 const& a, Vector4 const& b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

// Vector4 + Value
Vector4 operator+(Vector4 const& v, double t) {
  return {v.x + t, v.y + t, v.z + t, v.w + t};
}

// Value + Vector4
Vector4 operator+(double t, Vector4 const& v) {
  return {t + v.x, t + v.y, t + v.z, t + v.w};
}

// Vector4 - Vector4
Vector4 operator-(Vector4 const& a, Vector4 const& b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

// Vector4 - Value
Vector4 operator-(Vector4 const& v, double t) {
  return {v.x - t, v.y - t, v.z - t, v.w - t};
}

// Value - Vector4
Vector4 operator-(double t, Vector4 const& v) {
  return {t - v.x, t - v.y, t - v.z, t - v.w};
}

// Vector4 * Vector4
Vector4 operator*(Vector4 const& a, Vector4 const& b) {
  return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

// Vector4 * Value
Vector4 operator*(Vector4 const& v, double t) {
  return {v.x * t, v.y * t, v.z * t, v.w * t};
}

// Value * Vector4
Vector4 operator*(double t, Vector4 const& v) {
  return {t * v.x, t * v.y, t * v.z, t * v.w};
}

// Vector4 / Vector4
Vector4 operator/(Vector4 const& a, Vector4 const& b) {
  assert(b.x != 0.0);
  assert(b.y != 0.0);
  assert(b.z != 0.0);
  assert(b.w != 0.0);
  return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

// Vector4 / Value
Vector4 operator/(Vector4 const& v, double const u) {
  assert(u != 0.0);
  double const inv_u(1.0 / u);
  return {v.x * inv_u, v.y * inv_u, v.z * inv_u, v.w * inv_u};
}

// Value / Vector4
Vector4 operator/(double t, Vector4 const& v) {
  assert(v.x != 0.0);
  assert(v.y != 0.0);
  assert(v.z != 0.0);
  assert(v.w != 0.0);
  return {t / v.x, t / v.y, t / v.z, t / v.w};
}

// Stream << Vector4 output operator
std::ostream& operator<<(std::ostream& stream, Vector4 const& v) {
  constexpr std::streamsize precision = 16;  // Significant digits
  constexpr int width = 23;                  // Field width

  // Save current stream state and set persistent state
  std::ios_base::fmtflags const old_flags(stream.flags());
  std::streamsize const old_precision(stream.precision(precision));
  stream << std::right << std::showpoint << std::uppercase;

  // Output Vector4
  stream << std::setw(width) << v.x << ' ' << std::setw(width) << v.y << ' ' << std::setw(width) << v.z << ' ' << std::setw(width) << v.w;

  // Restore previous stream state
  stream.precision(old_precision);
  stream.flags(old_flags);

  return stream;
}

}  // namespace ObjexxFCL
