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

#ifndef ENERGYPLUS_GEOMETRY_PLANE_INCLUDED
#define ENERGYPLUS_GEOMETRY_PLANE_INCLUDED

// C++ Headers
#include <cassert>
#include <cstddef>
#include <iosfwd>

// ObjexxFCL Headers
#include <ObjexxFCL/Array1D.fwd.hh>

// EnergyPlus Headers
#include <EnergyPlus/Geometry/Vector3D.hh>
#include <EnergyPlus/api/TypeDefs.h>

namespace EnergyPlus {

/// @brief Plane: an infinite plane in 3D space.
///
/// The equation of a plane is a*x + b*y + c*z + d = 0, any point that satisfies this equation is on the plane.
class Plane
{
public:
    /// @name Data Elements
    //@{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double w = 0.0;
    //@}

    /// @name Creation
    //@{
    /// Default Constructor: Zero-Initializes All Elements
    constexpr Plane() = default;

    /// Value Constructor
    constexpr Plane(double x_, double y_, double z_, double w_) : x(x_), y(y_), z(z_), w(w_)
    {
    }

    /// @brief Plane of a polygon by Newell's method (robust for nonplanar and nonconvex polygons)
    ///
    ///  - Normal (x, y, z) = Newell area vector: oriented by the vertex order (counterclockwise seen from the front),
    ///    with a length of twice the polygon area, so the plane is not normalized
    ///  - w is set so the plane passes through the vertex average (not the area centroid)
    ///  - A degenerate polygon (e.g. collinear vertices) gives a zero normal: check normal() before calling
    ///    normalize(), normalized() or signedDistance(), which assert on it
    ///  - Requires at least 3 vertices
    static Plane fromVertices(ObjexxFCL::Array1D<Vector3D> const &vertices);
    //@}

    /// @name Unary Operators
    //@{
    /// -Plane (Negated)
    constexpr Plane operator-() const
    {
        return {-x, -y, -z, -w};
    }
    //@}

    /// @name Comparison
    //@{
    /// @brief Exact comparison of the four coefficients (also provides !=)
    ///
    /// Compares the representation: (1, 0, 0, 0) and (2, 0, 0, 0) describe the same plane but compare unequal.
    constexpr bool operator==(Plane const &) const = default;

    /// @brief Is this plane equal to the other plane (same position and same orientation)?
    ///
    /// Both planes are normalized before comparing, so any scaling of the coefficients is accepted.
    /// tol is used for two separate tests, and both must pass:
    ///  - Angle: dot product of the unit normals >= (1 - tol)
    ///    (tol is on the cosine, not in radians: 0.001 allows about 2.6 degrees of tilt)
    ///  - Distance: abs(w1 - w2) <= tol, the difference in distance to the origin
    ///    (in length units: 0.001 is 1 mm when working in meters)
    bool equal(const Plane &other, double tol = 0.001) const;

    /// Is this plane reverse equal to the other plane
    bool reverseEqual(const Plane &other, double tol = 0.001) const;
    //@}

    /// @name Subscript
    //@{
    /// Plane[ i ] const: 0-Based Index
    constexpr double operator[](std::size_t i) const
    {
        assert(i <= 3);
        return (i < 2 ? (i == 0 ? x : y) : (i == 2 ? z : w));
    }

    /// Plane[ i ]: 0-Based Index
    constexpr double &operator[](std::size_t i)
    {
        assert(i <= 3);
        return (i < 2 ? (i == 0 ? x : y) : (i == 2 ? z : w));
    }
    //@}

    /// @name Queries
    //@{
    /// @brief Outward Normal vector (x, y, z)
    ///
    /// Not unit length unless the plane was normalized
    constexpr Vector3D normal() const
    {
        return {x, y, z};
    }

    /// @brief Degenerate plane: zero normal (e.g. fromVertices() of collinear or coincident vertices)
    ///
    /// normalize(), normalized() and signedDistance() must not be called on a degenerate plane
    constexpr bool isDegenerate() const
    {
        return normal().length_squared() == 0.0; // Same test as the normal_length != 0.0 asserts
    }

    /// @brief Signed distance from a point to the plane: (a*x + b*y + c*z + d) / |(a, b, c)|
    ///
    ///  - Positive on the side the normal points to (outside), negative behind it, zero on the plane
    ///  - A true distance whether or not the plane is normalized (it divides by the normal's length)
    ///  - The plane must not be degenerate: its normal must be nonzero
    double signedDistance(Vector3D const &point) const
    {
        double const normal_length = normal().length();
        assert(normal_length != 0.0);
        return ((x * point.x) + (y * point.y) + (z * point.z) + w) / normal_length;
    }
    //@}

    /// @name Normalization
    //@{
    /// @brief Normalize to a Length (in-place)
    ///
    /// Scale All Four Coefficients So the Normal (x, y, z) Has Unit Length (w Then Is the Distance to the Origin)
    Plane &normalize()
    {
        double const normal_length = normal().length();
        assert(normal_length != 0.0);
        double const inv_length = 1.0 / normal_length;
        x *= inv_length;
        y *= inv_length;
        z *= inv_length;
        w *= inv_length;
        return *this;
    }

    /// @brief Normalized to a Length (return a new vector)
    ///
    /// Copy with the Normal (x, y, z) Scaled to Unit Length
    Plane normalized() const
    {
        double const normal_length = normal().length();
        assert(normal_length != 0.0);
        double const inv_length = 1.0 / normal_length;
        return {x * inv_length, y * inv_length, z * inv_length, w * inv_length};
    }
    //@}

}; // Plane

/// Stream << Plane output operator
std::ostream &operator<<(std::ostream &stream, Plane const &v);

} // namespace EnergyPlus

#endif // ENERGYPLUS_GEOMETRY_PLANE_INCLUDED
