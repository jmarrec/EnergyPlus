// ObjexxFCL::Vector3 Unit Tests
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

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4244) // Suppress conversion warnings: Intentional narrowing assignments present
#endif

// Google Test Headers
#include <gtest/gtest.h>

// ObjexxFCL Headers
#include <ObjexxFCL/Vector3.hh>

using namespace ObjexxFCL;

TEST(Vector3Test, Basic)
{
    Vector3 d; // Default construction zero-initializes
    EXPECT_EQ(0.0, d.x);
    EXPECT_EQ(0.0, d.y);
    EXPECT_EQ(0.0, d.z);
    Vector3 v(15.0); // Uniform value construction
    EXPECT_EQ(15.0, v.x);
    EXPECT_EQ(15.0, v.y);
    EXPECT_EQ(15.0, v.z);
    v = {1.0, 2.0, 3.0};
    EXPECT_EQ(1.0, v[0]);
    EXPECT_EQ(2.0, v[1]);
    EXPECT_EQ(3.0, v[2]);
    v[1] = 5.0;
    EXPECT_EQ(5.0, v.y);
    EXPECT_DOUBLE_EQ(5.0, Vector3(3.0, 4.0, 0.0).length());
    EXPECT_DOUBLE_EQ(25.0, Vector3(3.0, 4.0, 0.0).length_squared());
}

TEST(Vector3Test, Comparisons)
{
    Vector3 v(1.0, 2.0, 3.0);
    Vector3 w(1.0, 2.0, 3.0);
    EXPECT_EQ(v, w);
    EXPECT_FALSE(v != w);
    v.z = 0.0;
    EXPECT_TRUE(v != w);
    EXPECT_FALSE(v == w);
}

TEST(Vector3Test, Operators)
{
    Vector3 v(1.0, 12.0, 4.0);
    Vector3 w(2.0, 6.0, 1.0);
    EXPECT_EQ(Vector3(3.0, 18.0, 5.0), v + w);
    EXPECT_EQ(Vector3(-1.0, 6.0, 3.0), v - w);
    EXPECT_EQ(Vector3(-1.0, -12.0, -4.0), -v);
    EXPECT_EQ(Vector3(2.0, 24.0, 8.0), v * 2.0);
    EXPECT_EQ(Vector3(2.0, 24.0, 8.0), 2.0 * v);
    EXPECT_EQ(Vector3(0.5, 6.0, 2.0), v / 2.0);
    Vector3 u(v);
    u += w;
    EXPECT_EQ(Vector3(3.0, 18.0, 5.0), u);
    u -= w;
    EXPECT_EQ(v, u);
    u *= 2.0;
    EXPECT_EQ(Vector3(2.0, 24.0, 8.0), u);
    u /= 2.0;
    EXPECT_EQ(v, u);
    EXPECT_EQ(Vector3(1.5, 9.0, 2.5), 0.5 * (v + w)); // midpoint
}

TEST(Vector3Test, DotCrossDistance)
{
    Vector3 x(3.0, 0.0, 0.0);
    Vector3 y(0.0, 2.0, 0.0);
    EXPECT_EQ(0.0, x.dot(y));
    EXPECT_EQ(Vector3(0.0, 0.0, 6.0), x.cross(y));
    EXPECT_EQ(Vector3(0.0, 0.0, -6.0), y.cross(x));
    Vector3 v(3.0, 3.0, 3.0);
    Vector3 w(3.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(1.0, v.distance(w));
    EXPECT_DOUBLE_EQ(1.0, v.distance_squared(w));
}

TEST(Vector3Test, Normalize)
{
    Vector3 v(0.0, 3.0, 4.0);
    EXPECT_DOUBLE_EQ(1.0, v.normalized().length());
    EXPECT_DOUBLE_EQ(10.0, v.normalized(10.0).length());
    Vector3 n(v);
    n.normalize();
    EXPECT_DOUBLE_EQ(0.6, n.y);
    EXPECT_DOUBLE_EQ(0.8, n.z);
}

TEST(Vector3Test, Constexpr)
{
    constexpr Vector3 a(1.0, 2.0, 3.0);
    constexpr Vector3 b(4.0, 5.0, 6.0);
    static_assert(Vector3() == Vector3(0.0, 0.0, 0.0));
    static_assert(a + b == Vector3(5.0, 7.0, 9.0));
    static_assert(b - a == Vector3(3.0, 3.0, 3.0));
    static_assert(2.0 * a == Vector3(2.0, 4.0, 6.0));
    static_assert(-a == Vector3(-1.0, -2.0, -3.0));
    static_assert(a.dot(b) == 32.0);
    static_assert(a.cross(b) == Vector3(-3.0, 6.0, -3.0));
    static_assert(a.length_squared() == 14.0);
    static_assert(a.distance_squared(b) == 27.0);
    static_assert(a[2] == 3.0);
    static_assert(a != b);
    SUCCEED();
}
