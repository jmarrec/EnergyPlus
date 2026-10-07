#ifndef ObjexxFCL_Vector3_hh_INCLUDED
#define ObjexxFCL_Vector3_hh_INCLUDED

// Vector3: Fast 3-Element Vector
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

// ObjexxFCL Headers
#include <ObjexxFCL/Vector3.fwd.hh>

// C++ Headers
#include <cassert>
#include <cmath>
#include <cstddef>
#include <ostream>

namespace ObjexxFCL {

// Vector3: Fast 3-Element Vector of doubles
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
class Vector3
{
public: // Data Elements
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

public: // Creation
    Vector3() = default;

    // Uniform Value Constructor
    explicit constexpr Vector3(double t) : x(t), y(t), z(t)
    {
    }

    // Value Constructor
    constexpr Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_)
    {
    }

public: // Scalar multiplication and division
    /// Scalar Compound Assignment
    constexpr Vector3 &operator*=(double t)
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    constexpr Vector3 &operator/=(double u)
    {
        assert(u != 0.0);
        double const inv_u(1.0 / u);
        x *= inv_u;
        y *= inv_u;
        z *= inv_u;
        return *this;
    }

    /// Scalar Binary Operators
    friend constexpr Vector3 operator*(Vector3 const &v, double t)
    {
        return {v.x * t, v.y * t, v.z * t};
    }

    friend constexpr Vector3 operator*(double t, Vector3 const &v)
    {
        return {t * v.x, t * v.y, t * v.z};
    }

    friend constexpr Vector3 operator/(Vector3 const &v, double u)
    {
        assert(u != 0.0);
        double const inv_u(1.0 / u);
        return {v.x * inv_u, v.y * inv_u, v.z * inv_u};
    }

public: // Vector3 Addition and Subtraction
    /// Compound Assignment and Unary negation
    constexpr Vector3 &operator+=(Vector3 const &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    constexpr Vector3 &operator-=(Vector3 const &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    constexpr Vector3 operator-() const
    {
        return {-x, -y, -z};
    }

    /// Binary Operators
    friend constexpr Vector3 operator+(Vector3 const &a, Vector3 const &b)
    {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    friend constexpr Vector3 operator-(Vector3 const &a, Vector3 const &b)
    {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

public: // Comparison
    friend constexpr bool operator==(Vector3 const &, Vector3 const &) = default;

public: // Subscript
    // Vector3[ i ]: 0-Based Index
    constexpr double operator[](std::size_t i) const
    {
        assert(i <= 2);
        return (i == 0 ? x : (i == 1 ? y : z));
    }

    constexpr double &operator[](std::size_t i)
    {
        assert(i <= 2);
        return (i == 0 ? x : (i == 1 ? y : z));
    }

public: // Properties
    // Length (L2 norm)
    double length() const
    {
        return std::sqrt(length_squared());
    }

    constexpr double length_squared() const
    {
        return (x * x) + (y * y) + (z * z);
    }

    // Distance to a Vector3
    double distance(Vector3 const &v) const
    {
        return (v - *this).length();
    }

    constexpr double distance_squared(Vector3 const &v) const
    {
        return (v - *this).length_squared();
    }

    // Dot Product with a Vector3
    constexpr double dot(Vector3 const &v) const
    {
        return (x * v.x) + (y * v.y) + (z * v.z);
    }

    // Cross Product with a Vector3
    constexpr Vector3 cross(Vector3 const &v) const
    {
        return {(y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x)};
    }

public: // Normalization
    // Normalize to a Length
    Vector3 &normalize(double tar_length = 1.0)
    {
        double const cur_length(length());
        assert(cur_length != 0.0);
        *this *= tar_length / cur_length;
        return *this;
    }

    // Normalized to a Length
    Vector3 normalized(double tar_length = 1.0) const
    {
        double const cur_length(length());
        assert(cur_length != 0.0);
        return *this * (tar_length / cur_length);
    }

}; // Vector3

// Stream << Vector3 output operator
inline std::ostream &operator<<(std::ostream &os, Vector3 const &v)
{
    os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return os;
}

} // namespace ObjexxFCL

#endif // ObjexxFCL_Vector3_hh_INCLUDED
