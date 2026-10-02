// ObjexxFCL::Vector4 Unit Tests
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
#pragma warning(push)
#pragma warning(disable:4244) // Suppress conversion warnings: Intentional narrowing assignments present
#endif

// Google Test Headers
#include <gtest/gtest.h>

// ObjexxFCL Headers
#include <ObjexxFCL/Vector4.hh>
#include <ObjexxFCL/Array1D.hh>
#include "ObjexxFCL.unit.hh"

// C++ Headers
#include <array>
#include <cmath>
#include <vector>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace ObjexxFCL;

TEST( Vector4Test, Basic )
{
	Vector4 v( 15.0 ); // Uniform value construction
	EXPECT_EQ( 15.0f, v.x );
	EXPECT_EQ( 15.0f, v.y );
	EXPECT_EQ( 15.0f, v.z );
	EXPECT_EQ( 15.0f, v.w );
	v.normalize();
	EXPECT_FLOAT_EQ( 1.0f, v.length() );
	v.normalize( 5.0f );
	EXPECT_FLOAT_EQ( 5.0f, v.length() );
	v = 0.0;
	EXPECT_EQ( 0.0f, v.x );
	EXPECT_EQ( 0.0f, v.y );
	EXPECT_EQ( 0.0f, v.z );
	EXPECT_EQ( 0.0f, v.w );
	EXPECT_EQ( 0.0f, v.length() );
}

TEST( Vector4Test, StdArray )
{
	std::array< int, 4 > arr = {{ 33, 52, 17, 42 }};
	Vector4 v( arr );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 17, v.z );
	EXPECT_EQ( 42, v.w );
	arr = {{ 133, 152, 117, 123 }};
	v = arr;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
	v += arr;
	EXPECT_EQ( 266, v.x );
	EXPECT_EQ( 304, v.y );
	EXPECT_EQ( 234, v.z );
	EXPECT_EQ( 246, v.w );
	v -= arr;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
	arr = {{ 3, 2, 2, 3 }};
	v *= arr;
	EXPECT_EQ( 399, v.x );
	EXPECT_EQ( 304, v.y );
	EXPECT_EQ( 234, v.z );
	EXPECT_EQ( 369, v.w );
	v /= arr;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
}

TEST( Vector4Test, StdVector )
{
	std::vector< int > vec( { 33, 52, 17, 42 } );
	Vector4 v( vec );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 17, v.z );
	EXPECT_EQ( 42, v.w );
	vec = { 133, 152, 117, 123 };
	v = vec;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
	v += vec;
	EXPECT_EQ( 266, v.x );
	EXPECT_EQ( 304, v.y );
	EXPECT_EQ( 234, v.z );
	EXPECT_EQ( 246, v.w );
	v -= vec;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
	vec = { 3, 2, 2, 3 };
	v *= vec;
	EXPECT_EQ( 399, v.x );
	EXPECT_EQ( 304, v.y );
	EXPECT_EQ( 234, v.z );
	EXPECT_EQ( 369, v.w );
	v /= vec;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
}

TEST( Vector4Test, Array )
{
	Array1D_int a( 4, { 33, 52, 17, 42 } );
	Vector4 v( a );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 17, v.z );
	EXPECT_EQ( 42, v.w );
	a = { 133, 152, 117, 123 };
	v = a;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
	v += a;
	EXPECT_EQ( 266, v.x );
	EXPECT_EQ( 304, v.y );
	EXPECT_EQ( 234, v.z );
	EXPECT_EQ( 246, v.w );
	v -= a;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
	a = { 3, 2, 2, 3 };
	v *= a;
	EXPECT_EQ( 399, v.x );
	EXPECT_EQ( 304, v.y );
	EXPECT_EQ( 234, v.z );
	EXPECT_EQ( 369, v.w );
	v /= a;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
	EXPECT_EQ( 123, v.w );
}

TEST( Vector4Test, Comparisons )
{
	Vector4 v( 1.0, 2.0, 3.0, 4.0 );
	Vector4 w( 1.0, 2.0, 3.0, 4.0 );

	EXPECT_EQ( v, w );

	// Reduce v and test inequality
	v -= 0.5;
	EXPECT_TRUE( v != w );
	EXPECT_TRUE( ! ( v == w ) );
	EXPECT_TRUE( v < w );
	EXPECT_TRUE( v <= w );

	// Increase v and test inequality
	v += 1.0;
	EXPECT_TRUE( v != w );
	EXPECT_TRUE( ! ( v == w ) );
	EXPECT_TRUE( v > w );
	EXPECT_TRUE( v >= w );

	// Set v.x to 0 but leave the other components larger than w's
	v.x = 0.0;
	EXPECT_TRUE( v != w );
	EXPECT_TRUE( ! ( v == w ) );
}

TEST( Vector4Test, Generators )
{
	Vector4 v( 1.0, 12.0, 21.0, 18.0 );
	Vector4 w( 2.0, 6.0, 7.0, 9.0 );
	EXPECT_EQ( Vector4( 3.0, 18.0, 28.0, 27.0 ), v + w );
	EXPECT_EQ( Vector4( -1.0, 6.0, 14.0, 9.0 ), v - w );
	EXPECT_EQ( Vector4( 2.0, 72.0, 147.0, 162.0 ), v * w );
	EXPECT_EQ( Vector4( 0.5, 2.0, 3.0, 2.0 ), v / w );
}

TEST( Vector4Test, Distance )
{
	Vector4 v( 3.0, 3.0, 0.0, 1.0 );
	Vector4 w( 3.0, 2.0, 0.0, 1.0 );
	EXPECT_DOUBLE_EQ( 1.0, v.distance( w ) );
	EXPECT_DOUBLE_EQ( 1.0, v.distance_squared( w ) );
}

TEST( Vector4Test, Dot )
{
	Vector4 x( 3.0, 0.0, 0.0, 5.0 );
	Vector4 y( 0.0, 2.0, 0.0, 0.0 );
	EXPECT_EQ( 0.0, x.dot( y ) );
}

TEST( Vector4Test, Center )
{
	Vector4 x( 4.0, 0.0, 77.0, 42.0 );
	Vector4 y( 0.0, 4.0, 77.0, 42.0 );
	EXPECT_EQ( Vector4( 2.0, 2.0, 77.0, 42.0 ), cen( x, y ) );
}

TEST( Vector4Test, Angle )
{
	double const Pi( std::acos( -1.0 ) );
	double const Pi_2( std::asin( 1.0 ) );
	{
		Vector4 a( 4.0, 0.0, 0.0, 0.0 );
		Vector4 b( 0.0, 4.0, 0.0, 0.0 );
		EXPECT_DOUBLE_EQ( Pi_2, angle( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, cos( a, b ) );
		EXPECT_DOUBLE_EQ( 1.0, sin( a, b ) );
	}
	{
		Vector4 a( 4.0, 0.0, 0.0, 0.0 );
		Vector4 b( 0.0, 0.0, -6.0, 0.0 );
		EXPECT_DOUBLE_EQ( Pi_2, angle( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, cos( a, b ) );
		EXPECT_DOUBLE_EQ( 1.0, sin( a, b ) );
	}
	{
		Vector4 a( 0.0, 4.0, 0.0, 0.0 );
		Vector4 b( 0.0, -1.0, 0.0, 0.0 );
		EXPECT_DOUBLE_EQ( Pi, angle( a, b ) );
		EXPECT_DOUBLE_EQ( -1.0, cos( a, b ) );
		EXPECT_NEAR( 0.0, sin( a, b ), 1.0E-15 ); // EXPECT_DOUBLE_EQ tolerance is too small
	}
}

TEST( Vector4Test, BinaryOperations )
{
	Vector4 v( 1.0, 2.0, 3.0, 4.0 );
	Vector4 w( 1.0, 2.0, 3.0, 4.0 );
	Vector4 const original( v );

	// Check dot product of equal vectors
	EXPECT_DOUBLE_EQ( v.length_squared(), v.dot( w ) ); // v == w here

	// Tweak the vectors
	v += 1.0; w -= 1.0;

	// Check midpoint (should match original vector)
	Vector4 const midpoint( mid( v, w ) );
	EXPECT_DOUBLE_EQ( original.x, midpoint.x );
	EXPECT_DOUBLE_EQ( original.y, midpoint.y );
	EXPECT_DOUBLE_EQ( original.z, midpoint.z );
}
