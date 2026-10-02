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
#include <compare>
#include <cstddef>
#include <iosfwd>
#include <type_traits>

namespace EnergyPlus {

// Array-like type usable with Plane's array interface: has a value_type convertible to double, operator[] and size()
// (e.g. std::array< double, 4 >, std::vector< double >)
template <typename A>
concept Array4Like = requires(A const &a) {
    typename A::value_type;
    a.size();
    a[0];
} && std::is_assignable_v<double &, typename A::value_type>;

// Plane: an infinite plane in 3D space.  The equation of a plane is
//  a*x + b*y + c*z + d = 0, any point that satisfies this equation is on the plane.
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
// . Use std::array< double, 4 > instead in array/vectorization context
class Plane
{

public: // Types
    using value_type = double;
    using reference = double &;
    using const_reference = double const &;
    using pointer = double *;
    using const_pointer = double const *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

public: // Creation
    // Default Constructor: Zero-Initializes All Elements
    Plane() = default;

    // Value Constructor
    Plane(double x_, double y_, double z_, double w_);

public: // Array Interface
    // Any type with operator[] and size() == 4 (e.g. std::array< double, 4 >)

    // Array Constructor Template
    template <Array4Like A> Plane(A const &a) : x(a[0]), y(a[1]), z(a[2]), w(a[3])
    {
        assert(a.size() == 4);
    }

    // = Array
    template <Array4Like A> Plane &operator=(A const &a)
    {
        assert(a.size() == 4);
        x = a[0];
        y = a[1];
        z = a[2];
        w = a[3];
        return *this;
    }

    // Dot Product with an Array
    template <Array4Like A> double dot(A const &a) const
    {
        assert(a.size() == 4);
        return (x * a[0]) + (y * a[1]) + (z * a[2]) + (w * a[3]);
    }

public: // Subscript
    // Plane[ i ] const: 0-Based Index
    double operator[](size_type i) const;

    // Plane[ i ]: 0-Based Index
    double &operator[](size_type i);

public: // Properties: General
    // Size
    size_type size() const;

public: // Modifiers
    // Normalize: Scale All Four Coefficients So the Normal (x, y, z) Has Unit Length (w Then Is the Distance to the Origin)
    Plane &normalize();

public: // Generators
    // -Plane (Negated)
    Plane operator-() const;

    // Normalized: Copy with the Normal (x, y, z) Scaled to Unit Length
    Plane normalized() const;

public: // Comparison
    // Lexicographic on (x, y, z, w)
    auto operator<=>(Plane const &) const = default;

public: // Data Elements
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double w = 0.0;
}; // Plane

// Stream << Plane output operator
std::ostream &operator<<(std::ostream &stream, Plane const &v);

} // namespace EnergyPlus

#endif // ENERGYPLUS_GEOMETRY_PLANE_INCLUDED
