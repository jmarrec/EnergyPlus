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

// C++ Headers
#include <cassert>
#include <cmath>
#include <iomanip>
#include <ostream>

// EnergyPlus Headers
#include <EnergyPlus/Geometry/Plane.hh>

namespace EnergyPlus {

// Value Constructor
Plane::Plane(double x_, double y_, double z_, double w_) : x(x_), y(y_), z(z_), w(w_)
{
}

// Plane[ i ] const: 0-Based Index
double Plane::operator[](size_type i) const
{
    assert(i <= 3);
    return (i < 2 ? (i == 0 ? x : y) : (i == 2 ? z : w));
}

// Plane[ i ]: 0-Based Index
double &Plane::operator[](size_type i)
{
    assert(i <= 3);
    return (i < 2 ? (i == 0 ? x : y) : (i == 2 ? z : w));
}

// Size
Plane::size_type Plane::size() const
{
    return 4u;
}

// Normalize: Scale All Four Coefficients So the Normal (x, y, z) Has Unit Length
Plane &Plane::normalize()
{
    double const normal_length(std::sqrt((x * x) + (y * y) + (z * z)));
    assert(normal_length != 0.0);
    x /= normal_length;
    y /= normal_length;
    z /= normal_length;
    w /= normal_length;
    return *this;
}

// -Plane (Negated)
Plane Plane::operator-() const
{
    return {-x, -y, -z, -w};
}

Plane Plane::reversedPlane() const
{
    return {-x, -y, -z, -w};
}

// Normalized: Copy with the Normal (x, y, z) Scaled to Unit Length
Plane Plane::normalized() const
{
    double const normal_length(std::sqrt((x * x) + (y * y) + (z * z)));
    assert(normal_length != 0.0);
    return {x / normal_length, y / normal_length, z / normal_length, w / normal_length};
}

// Stream << Plane output operator
std::ostream &operator<<(std::ostream &stream, Plane const &v)
{
    constexpr std::streamsize precision = 16; // Significant digits
    constexpr int width = 23;                 // Field width

    // Save current stream state and set persistent state
    std::ios_base::fmtflags const old_flags(stream.flags());
    std::streamsize const old_precision(stream.precision(precision));
    stream << std::right << std::showpoint << std::uppercase;

    // Output Plane
    stream << std::setw(width) << v.x << ' ' << std::setw(width) << v.y << ' ' << std::setw(width) << v.z << ' ' << std::setw(width) << v.w;

    // Restore previous stream state
    stream.precision(old_precision);
    stream.flags(old_flags);

    return stream;
}

} // namespace EnergyPlus
