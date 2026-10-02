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

// Google Test Headers
#include <gtest/gtest.h>

#include "GeometryFixture.hh"

// EnergyPlus Headers
#include <EnergyPlus/Geometry/Plane.hh>

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4244) // Suppress conversion warnings: Intentional narrowing assignments present
#endif

// ObjexxFCL Headers
#include <ObjexxFCL/Array1D.hh>

// C++ Headers
#include <array>
#include <cmath>
#include <sstream>
#include <vector>

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

using namespace EnergyPlus;

TEST_F(GeometryFixture, Basic)
{
    Plane v(15.0); // Uniform value construction
    EXPECT_EQ(15.0, v.x);
    EXPECT_EQ(15.0, v.y);
    EXPECT_EQ(15.0, v.z);
    EXPECT_EQ(15.0, v.w);
    v = 0.0;
    EXPECT_EQ(0.0, v.x);
    EXPECT_EQ(0.0, v.y);
    EXPECT_EQ(0.0, v.z);
    EXPECT_EQ(0.0, v.w);
}

TEST_F(GeometryFixture, Normalize)
{
    // Normalizing a plane scales the normal (x, y, z) to unit length; w (the offset) scales with it, not into the length
    Plane p(3.0, 0.0, 4.0, 10.0); // |normal| = 5
    Plane const q(p.normalized());
    EXPECT_DOUBLE_EQ(0.6, q.x);
    EXPECT_DOUBLE_EQ(0.0, q.y);
    EXPECT_DOUBLE_EQ(0.8, q.z);
    EXPECT_DOUBLE_EQ(2.0, q.w);
    EXPECT_DOUBLE_EQ(1.0, std::sqrt((q.x * q.x) + (q.y * q.y) + (q.z * q.z)));
    EXPECT_DOUBLE_EQ(3.0, p.x); // normalized() leaves the original alone

    p.normalize();
    EXPECT_EQ(q, p);

    // The plane x = 3 stays x = 3 however it is scaled: (2, 0, 0, -6) -> (1, 0, 0, -3)
    Plane s(2.0, 0.0, 0.0, -6.0);
    s.normalize();
    EXPECT_DOUBLE_EQ(1.0, s.x);
    EXPECT_DOUBLE_EQ(0.0, s.y);
    EXPECT_DOUBLE_EQ(0.0, s.z);
    EXPECT_DOUBLE_EQ(-3.0, s.w);
}

TEST_F(GeometryFixture, StdArray)
{
    std::array<int, 4> arr = {{33, 52, 17, 42}};
    Plane v(arr);
    EXPECT_EQ(33.0, v.x);
    EXPECT_EQ(52.0, v.y);
    EXPECT_EQ(17.0, v.z);
    EXPECT_EQ(42.0, v.w);
    arr = {{133, 152, 117, 123}};
    v = arr;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
    v += arr;
    EXPECT_EQ(266.0, v.x);
    EXPECT_EQ(304.0, v.y);
    EXPECT_EQ(234.0, v.z);
    EXPECT_EQ(246.0, v.w);
    v -= arr;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
    arr = {{3, 2, 2, 3}};
    v *= arr;
    EXPECT_EQ(399.0, v.x);
    EXPECT_EQ(304.0, v.y);
    EXPECT_EQ(234.0, v.z);
    EXPECT_EQ(369.0, v.w);
    v /= arr;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
}

TEST_F(GeometryFixture, StdVector)
{
    std::vector<int> vec({33, 52, 17, 42});
    Plane v(vec);
    EXPECT_EQ(33.0, v.x);
    EXPECT_EQ(52.0, v.y);
    EXPECT_EQ(17.0, v.z);
    EXPECT_EQ(42.0, v.w);
    vec = {133, 152, 117, 123};
    v = vec;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
    v += vec;
    EXPECT_EQ(266.0, v.x);
    EXPECT_EQ(304.0, v.y);
    EXPECT_EQ(234.0, v.z);
    EXPECT_EQ(246.0, v.w);
    v -= vec;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
    vec = {3, 2, 2, 3};
    v *= vec;
    EXPECT_EQ(399.0, v.x);
    EXPECT_EQ(304.0, v.y);
    EXPECT_EQ(234.0, v.z);
    EXPECT_EQ(369.0, v.w);
    v /= vec;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
}

TEST_F(GeometryFixture, Array)
{
    Array1D<int> a(4, {33, 52, 17, 42});
    Plane v(a);
    EXPECT_EQ(33.0, v.x);
    EXPECT_EQ(52.0, v.y);
    EXPECT_EQ(17.0, v.z);
    EXPECT_EQ(42.0, v.w);
    a = {133, 152, 117, 123};
    v = a;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
    v += a;
    EXPECT_EQ(266.0, v.x);
    EXPECT_EQ(304.0, v.y);
    EXPECT_EQ(234.0, v.z);
    EXPECT_EQ(246.0, v.w);
    v -= a;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
    a = {3, 2, 2, 3};
    v *= a;
    EXPECT_EQ(399.0, v.x);
    EXPECT_EQ(304.0, v.y);
    EXPECT_EQ(234.0, v.z);
    EXPECT_EQ(369.0, v.w);
    v /= a;
    EXPECT_EQ(133.0, v.x);
    EXPECT_EQ(152.0, v.y);
    EXPECT_EQ(117.0, v.z);
    EXPECT_EQ(123.0, v.w);
}

TEST_F(GeometryFixture, Comparisons)
{
    Plane v(1.0, 2.0, 3.0, 4.0);
    Plane w(1.0, 2.0, 3.0, 4.0);

    EXPECT_EQ(v, w);

    // Reduce v and test inequality
    v -= 0.5;
    EXPECT_TRUE(v != w);
    EXPECT_TRUE(!(v == w));
    EXPECT_TRUE(v < w);
    EXPECT_TRUE(v <= w);

    // Increase v and test inequality
    v += 1.0;
    EXPECT_TRUE(v != w);
    EXPECT_TRUE(!(v == w));
    EXPECT_TRUE(v > w);
    EXPECT_TRUE(v >= w);

    // Set v.x to 0 but leave the other components larger than w's
    v.x = 0.0;
    EXPECT_TRUE(v != w);
    EXPECT_TRUE(!(v == w));
}

TEST_F(GeometryFixture, Generators)
{
    Plane v(1.0, 12.0, 21.0, 18.0);
    Plane w(2.0, 6.0, 7.0, 9.0);
    EXPECT_EQ(Plane(3.0, 18.0, 28.0, 27.0), v + w);
    EXPECT_EQ(Plane(-1.0, 6.0, 14.0, 9.0), v - w);
    EXPECT_EQ(Plane(2.0, 72.0, 147.0, 162.0), v * w);
    EXPECT_EQ(Plane(0.5, 2.0, 3.0, 2.0), v / w);
}

TEST_F(GeometryFixture, StreamOutput)
{
    // Pins the current output format: right-aligned fixed-width fields, 16 significant digits, showpoint, uppercase exponent
    {
        std::ostringstream os;
        os << Plane(1.0, -2.5, 1234567.891, 1.0e-7);
        EXPECT_EQ("      1.000000000000000      -2.500000000000000       1234567.891000000   1.000000000000000E-07", os.str());
    }
    {
        std::ostringstream os;
        os << Plane();
        EXPECT_EQ("      0.000000000000000       0.000000000000000       0.000000000000000       0.000000000000000", os.str());
    }
    {
        std::ostringstream os;
        os << Plane(1.0 / 3.0);
        EXPECT_EQ("     0.3333333333333333      0.3333333333333333      0.3333333333333333      0.3333333333333333", os.str());
    }
}

TEST_F(GeometryFixture, StreamOutputRestoresStreamState)
{
    std::ostringstream os;
    os.precision(3);
    os << std::fixed;
    std::ios_base::fmtflags const flags(os.flags());
    std::streamsize const precision(os.precision());
    os << Plane(1.0, 2.0, 3.0, 4.0);
    EXPECT_EQ(flags, os.flags());
    EXPECT_EQ(precision, os.precision());
    os.str("");
    os << 3.14159265;
    EXPECT_EQ("3.142", os.str()); // Fixed with precision 3, as set before streaming the Plane
}
