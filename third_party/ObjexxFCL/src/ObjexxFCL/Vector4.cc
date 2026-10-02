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
#include <algorithm>
#include <cassert>
#include <cmath>
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

// Value Assignment
Vector4& Vector4::assign(double x_, double y_, double z_, double w_) {
  x = x_;
  y = y_;
  z = z_;
  w = w_;
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
Vector4::Size Vector4::size() const {
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

// Midpoint of Two Vector4s
Vector4 mid(Vector4 const& a, Vector4 const& b) {
  return {0.5 * (a.x + b.x), 0.5 * (a.y + b.y), 0.5 * (a.z + b.z), 0.5 * (a.w + b.w)};
}

// Center of Two Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b) {
  return {0.5 * (a.x + b.x), 0.5 * (a.y + b.y), 0.5 * (a.z + b.z), 0.5 * (a.w + b.w)};
}

// Center of Three Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  constexpr double third(1.0 / 3.0);
  return {third * (a.x + b.x + c.x), third * (a.y + b.y + c.y), third * (a.z + b.z + c.z),
                 third * (a.w + b.w + c.w)};
}

// Center of Four Vector4s
Vector4 cen(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d) {
  return {0.25 * (a.x + b.x + c.x + d.x), 0.25 * (a.y + b.y + c.y + d.y), 0.25 * (a.z + b.z + c.z + d.z),
                 0.25 * (a.w + b.w + c.w + d.w)};
}

// Angle Between Two Vector4s (in Radians on [0,pi])
double angle(Vector4 const& a, Vector4 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > 0.0 ? std::acos(std::clamp(a.dot(b) / mag, -1.0, 1.0)) : 0.0);
}

// Angle abc Formed by Three Vector4s (in Radians on [0,pi])
double angle(Vector4 const& a, Vector4 const& b, Vector4 const& c) {
  return angle(a - b, c - b);
}

// Cosine of Angle Between Two Vector4s
double cos(Vector4 const& a, Vector4 const& b) {
  double const mag(std::sqrt(a.length_squared() * b.length_squared()));
  return (mag > 0.0 ? std::clamp(a.dot(b) / mag, -1.0, 1.0) : 1.0);
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
