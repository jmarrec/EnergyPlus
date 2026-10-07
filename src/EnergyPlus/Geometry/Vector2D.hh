// EnergyPlus, Copyright (c) 1996-present, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Energy Innovation, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef ENERGYPLUS_GEOMETRY_VECTOR2D_INCLUDED
#define ENERGYPLUS_GEOMETRY_VECTOR2D_INCLUDED

// C++ Headers
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <numbers>
#include <ostream>

namespace EnergyPlus {

// Vector2D: Fast 2-Element Vector
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
// . Use std::array< double, 2 > instead in array/vectorization context
class Vector2D
{

public: // Types
    // STL Style
    using value_type = double;
    using reference = double &;
    using const_reference = double const &;
    using pointer = double *;
    using const_pointer = double const *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

public: // Creation
    // Default Constructor
    Vector2D() = default;

    // Uniform Value Constructor
    explicit constexpr Vector2D(double t) : x(t), y(t)
    {
    }

    // Value Constructor
    constexpr Vector2D(double x_, double y_) : x(x_), y(y_)
    {
    }

public: // Scalar multiplication and division
    /// Scalar Compound Assignment
    // *= Scalar
    constexpr Vector2D &operator*=(double t)
    {
        x *= t;
        y *= t;
        return *this;
    }

    // /= Scalar
    constexpr Vector2D &operator/=(double const u)
    {
        assert(u != 0.0);
        double const inv_u(1.0 / u);
        x *= inv_u;
        y *= inv_u;
        return *this;
    }

    /// Scalar Binary Operators
    // Vector2D * Scalar
    friend constexpr Vector2D operator*(Vector2D const &v, double t)
    {
        return {v.x * t, v.y * t};
    }

    // Scalar * Vector2D
    friend constexpr Vector2D operator*(double t, Vector2D const &v)
    {
        return {t * v.x, t * v.y};
    }

    // Vector2D / Scalar
    friend constexpr Vector2D operator/(Vector2D const &v, double u)
    {
        assert(u != 0.0);
        double const inv_u(1.0 / u);
        return {v.x * inv_u, v.y * inv_u};
    }

public: // Vector2D Addition and Subtraction
    /// Compound Assignment and Unary negation
    // += Vector2D
    constexpr Vector2D &operator+=(Vector2D const &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // -= Vector2D
    constexpr Vector2D &operator-=(Vector2D const &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    // -Vector2D (Negated)
    constexpr Vector2D operator-() const
    {
        return {-x, -y};
    }

    /// Addition and Subtraction: Binary Operators
    // Vector2D + Vector2D
    friend constexpr Vector2D operator+(Vector2D const &a, Vector2D const &b)
    {
        return {a.x + b.x, a.y + b.y};
    }

    // Vector2D - Vector2D
    friend constexpr Vector2D operator-(Vector2D const &a, Vector2D const &b)
    {
        return {a.x - b.x, a.y - b.y};
    }

public: // Comparison
    friend constexpr bool operator==(Vector2D const &, Vector2D const &) = default;

public: // Subscript
    // Vector2D[ i ] const: 0-Based Index
    constexpr double operator[](size_type const i) const
    {
        assert(i <= 1);
        return (i == 0 ? x : y);
    }

    // Vector2D[ i ]: 0-Based Index
    constexpr double &operator[](size_type const i)
    {
        assert(i <= 1);
        return (i == 0 ? x : y);
    }

public: // Properties: Predicates
    // Is Zero Vector?
    constexpr bool is_zero() const
    {
        return (x == 0.0) && (y == 0.0);
    }

    // Is Unit Vector?
    constexpr bool is_unit() const
    {
        return (length_squared() == 1.0);
    }

public: // Properties: General
    // Size
    constexpr size_type size() const
    {
        return 2u;
    }

    // Length (L2 norm)
    double length() const
    {
        return std::sqrt(length_squared());
    }

    // Length Squared
    constexpr double length_squared() const
    {
        return (x * x) + (y * y);
    }

    // L1 Norm
    double norm_L1() const
    {
        return std::abs(x) + std::abs(y);
    }

    // L-infinity Norm
    double norm_Linf() const
    {
        return std::max(std::abs(x), std::abs(y));
    }

    // Distance to a Vector2D
    double distance(Vector2D const &v) const
    {
        return (v - *this).length();
    }

    // Distance Squared to a Vector2D
    constexpr double distance_squared(Vector2D const &v) const
    {
        return (v - *this).length_squared();
    }

    // Dot Product with a Vector2D
    constexpr double dot(Vector2D const &v) const
    {
        return (x * v.x) + (y * v.y);
    }

    // Cross Product with a Vector2D
    constexpr double cross(Vector2D const &v) const
    {
        return (x * v.y) - (y * v.x);
    }

    // Angle Between this and a Vector2D (in Radians on [0,pi])
    double angle(Vector2D const &v) const
    {
        double const axb(std::abs(cross(v)));
        double const adb(dot(v));
        return ((axb != 0.0) || (adb != 0.0) ? bump_up_angle(std::atan2(axb, adb)) : 0.0); // More accurate than dot-based for angles near 0 and Pi
    }

    // Cosine of Angle Between this and a Vector2D
    double cos(Vector2D const &v) const
    {
        double const mag(std::sqrt(length_squared() * v.length_squared()));
        return (mag > 0.0 ? std::clamp(dot(v) / mag, -1.0, 1.0) : 1.0);
    }

    // Sine of Angle Between this and a Vector2D
    double sin(Vector2D const &v) const
    {
        double const mag(std::sqrt(length_squared() * v.length_squared()));
        return (mag > 0.0 ? std::abs(std::clamp(cross(v) / mag, -1.0, 1.0)) : 0.0);
    }

    // Directed Angle from this to a Vector2D (in Radians on [0,2*pi])
    double dir_angle(Vector2D const &v) const
    {
        double const axb(cross(v));
        double const adb(dot(v));
        return ((axb != 0.0) || (adb != 0.0) ? bump_up_angle(std::atan2(axb, adb)) : 0.0);
    }

    // Cosine of Directed Angle from this to a Vector2D
    double dir_cos(Vector2D const &v) const
    {
        double const mag(std::sqrt(length_squared() * v.length_squared()));
        return (mag > 0.0 ? std::clamp(dot(v) / mag, -1.0, 1.0) : 1.0);
    }

    // Sine of Directed Angle from this to a Vector2D
    double dir_sin(Vector2D const &v) const
    {
        double const mag(std::sqrt(length_squared() * v.length_squared()));
        return (mag > 0.0 ? std::clamp(cross(v) / mag, -1.0, 1.0) : 0.0);
    }

public: // Modifiers
    // Normalize to a Length
    Vector2D &normalize(double tar_length = 1.0)
    {
        double const cur_length(length());
        assert(cur_length != double(0));
        double const dilation(tar_length / cur_length);
        x *= dilation;
        y *= dilation;
        return *this;
    }

    // Normalize to a Length: Zero Vector2D if Length is Zero
    Vector2D &normalize_zero(double tar_length = 1.0)
    {
        double const cur_length(length());
        if (cur_length > 0.0) {
            double const dilation(tar_length / cur_length);
            x *= dilation;
            y *= dilation;
        } else { // Set zero vector
            x = y = 0.0;
        }
        return *this;
    }

    // Project Normal to a Vector2D
    constexpr Vector2D &project_normal(Vector2D const &v)
    {
        assert(v.length_squared() != 0.0);
        double const c(dot(v) / v.length_squared());
        x -= c * v.x;
        y -= c * v.y;
        return *this;
    }

    // Project onto a Vector2D
    constexpr Vector2D &project_parallel(Vector2D const &v)
    {
        assert(v.length_squared() != 0.0);
        double const c(dot(v) / v.length_squared());
        x = c * v.x;
        y = c * v.y;
        return *this;
    }

public: // Generators
    // Normalized to a Length
    Vector2D normalized(double tar_length = 1.0) const
    {
        double const cur_length(length());
        assert(cur_length != double(0));
        double const dilation(tar_length / cur_length);
        return {x * dilation, y * dilation};
    }

    // Normalized to a Length: Zero Vector2D if Length is Zero
    Vector2D normalized_zero(double tar_length = 1.0) const
    {
        double const cur_length(length());
        if (cur_length > 0.0) {
            double const dilation(tar_length / cur_length);
            return {x * dilation, y * dilation};
        } else { // Return zero vector
            return {0.0, 0.0};
        }
    }

    // Projected Normal to a Vector2D
    constexpr Vector2D projected_normal(Vector2D const &v) const
    {
        assert(v.length_squared() != 0.0);
        double const c(dot(v) / v.length_squared());
        return {x - (c * v.x), y - (c * v.y)};
    }

    // Projected onto a Vector2D
    constexpr Vector2D projected_parallel(Vector2D const &v) const
    {
        assert(v.length_squared() != 0.0);
        double const c(dot(v) / v.length_squared());
        return {c * v.x, c * v.y};
    }

private: // Static Methods
    // Add 2*Pi to a Negative Value
    static constexpr double bump_up_angle(double t)
    {
        constexpr double Two_Pi = 2.0 * std::numbers::pi;
        return (t >= 0.0 ? t : Two_Pi + t);
    }

public: // Data Elements
    double x = 0.0;
    double y = 0.0;

}; // Vector2D

// Stream << Vector2D output operator
inline std::ostream &operator<<(std::ostream &stream, Vector2D const &v)
{
    constexpr std::streamsize precision = 16; // Significant digits
    constexpr int width = 23;                 // Field width

    // Save current stream state and set persistent state
    std::ios_base::fmtflags const old_flags(stream.flags());
    std::streamsize const old_precision(stream.precision(precision));
    stream << std::right << std::showpoint << std::uppercase;

    // Output Vector2D
    stream << std::setw(width) << v.x << ' ' << std::setw(width) << v.y;

    // Restore previous stream state
    stream.precision(old_precision);
    stream.flags(old_flags);

    return stream;
}

} // namespace EnergyPlus

#endif // ENERGYPLUS_GEOMETRY_VECTOR2D_INCLUDED
