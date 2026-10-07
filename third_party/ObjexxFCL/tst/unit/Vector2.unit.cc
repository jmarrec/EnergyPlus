// ObjexxFCL::Vector2 Unit Tests
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
#include <ObjexxFCL/Vector2.hh>
#include <ObjexxFCL/Array1D.hh>
#include "ObjexxFCL.unit.hh"

// C++ Headers
#include <array>
#include <cmath>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace ObjexxFCL;

TEST( Vector2Test, Basic )
{
	Vector2 v( 15.0 ); // Uniform value construction
	EXPECT_EQ( 15.0f, v.x );
	EXPECT_EQ( 15.0f, v.y );
	v.normalize();
	EXPECT_FLOAT_EQ( 1.0f, v.length() );
	v.normalize( 5.0f );
	EXPECT_FLOAT_EQ( 5.0f, v.length() );
	v = { 0.0, 0.0 };
	EXPECT_EQ( 0.0f, v.x );
	EXPECT_EQ( 0.0f, v.y );
	EXPECT_EQ( 0.0f, v.length() );
	v.normalize_zero();
	EXPECT_EQ( 0.0f, v.x );
	EXPECT_EQ( 0.0f, v.y );
}

TEST( Vector2Test, BraceInit )
{
	Vector2 v{ 33, 52 };
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 33, v[ 0 ] );
	EXPECT_EQ( 52, v[ 1 ] );
	v = { 44, 55 };
	EXPECT_EQ( 44, v.x );
	EXPECT_EQ( 55, v.y );
	v *= 2;
	EXPECT_EQ( 88, v.x );
	EXPECT_EQ( 110, v.y );
	v /= 2.0;
	EXPECT_EQ( 44, v.x );
	EXPECT_EQ( 55, v.y );
	v = -v;
	EXPECT_EQ( -44, v.x );
	EXPECT_EQ( -55, v.y );
}

TEST( Vector2Test, Comparisons )
{
	Vector2 v( 1.0, 2.0 );
	Vector2 w( 1.0, 2.0 );

	EXPECT_EQ( v, w );

	// Reduce v and test inequality
	v -= Vector2( 0.5 );
	EXPECT_TRUE( v != w );
	EXPECT_TRUE( ! ( v == w ) );
	EXPECT_TRUE( v < w );
	EXPECT_TRUE( v <= w );

	// Increase v and test inequality
	v += Vector2( 1.0 );
	EXPECT_TRUE( v != w );
	EXPECT_TRUE( ! ( v == w ) );
	EXPECT_TRUE( v > w );
	EXPECT_TRUE( v >= w );

	// Set v.x to 0: v and w now differ
	v.x = 0.0;
	EXPECT_TRUE( v != w );
	EXPECT_TRUE( ! ( v == w ) );
}

TEST( Vector2Test, Generators )
{
	Vector2 v( 1.0, 12.0 );
	Vector2 w( 2.0, 6.0 );
	EXPECT_EQ( Vector2( 3.0, 18.0 ), v + w );
	EXPECT_EQ( Vector2( -1.0, 6.0 ), v - w );
	EXPECT_EQ( Vector2( 2.0, 24.0 ), v * 2.0 );
	EXPECT_EQ( Vector2( 2.0, 24.0 ), 2.0 * v );
	EXPECT_EQ( Vector2( 0.5, 6.0 ), v / 2.0 );
}

TEST( Vector2Test, Distance )
{
	Vector2 v( 3.0, 3.0 );
	Vector2 w( 3.0, 2.0 );
	EXPECT_DOUBLE_EQ( 1.0, v.distance( w ) );
	EXPECT_DOUBLE_EQ( 1.0, v.distance_squared( w ) );
}

TEST( Vector2Test, Dot )
{
	Vector2 x( 3.0, 0.0 );
	Vector2 y( 0.0, 2.0 );
	EXPECT_EQ( 0.0, x.dot( y ) );
}

TEST( Vector2Test, Cross )
{
	Vector2 x( 3.0, 0.0 );
	Vector2 y( 0.0, 2.0 );
	EXPECT_EQ( 6.0, x.cross( y ) );
}

TEST( Vector2Test, Center )
{
	Vector2 x( 4.0, 0.0 );
	Vector2 y( 0.0, 4.0 );
	EXPECT_EQ( Vector2( 2.0, 2.0 ), cen( x, y ) );
}

TEST( Vector2Test, Angle )
{
	double const Pi( std::acos( -1.0 ) );
	double const Pi_2( std::asin( 1.0 ) );
	{
		Vector2 a( 4.0, 0.0 );
		Vector2 b( 0.0, 4.0 );
		EXPECT_DOUBLE_EQ( Pi_2, angle( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, cos( a, b ) );
		EXPECT_DOUBLE_EQ( 1.0, sin( a, b ) );
		EXPECT_DOUBLE_EQ( Pi_2, dir_angle( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, dir_cos( a, b ) );
		EXPECT_DOUBLE_EQ( 1.0, dir_sin( a, b ) );
	}
	{
		Vector2 a( 4.0, 0.0 );
		Vector2 b( 0.0, -4.0 );
		EXPECT_DOUBLE_EQ( Pi_2, angle( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, cos( a, b ) );
		EXPECT_DOUBLE_EQ( 1.0, sin( a, b ) );
		EXPECT_DOUBLE_EQ( 3.0 * Pi_2, dir_angle( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, dir_cos( a, b ) );
		EXPECT_DOUBLE_EQ( -1.0, dir_sin( a, b ) );
	}
	{
		Vector2 a( 4.0, 0.0 );
		Vector2 b( -1.0, 0.0 );
		EXPECT_DOUBLE_EQ( Pi, angle( a, b ) );
		EXPECT_DOUBLE_EQ( -1.0, cos( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, sin( a, b ) );
		EXPECT_DOUBLE_EQ( Pi, dir_angle( a, b ) );
		EXPECT_DOUBLE_EQ( -1.0, dir_cos( a, b ) );
		EXPECT_DOUBLE_EQ( 0.0, dir_sin( a, b ) );
	}
}

TEST( Vector2Test, BinaryOperations )
{
	Vector2 v( 1.0, 2.0 );
	Vector2 w( 1.0, 2.0 );
	Vector2 const original( v );

	// Check dot product of equal vectors
	EXPECT_DOUBLE_EQ( v.length_squared(), v.dot( w ) ); // v == w here

	// Check midpoint (should match original vector)
	v += Vector2( 1.0 ); w -= Vector2( 1.0 );
	Vector2 const midpoint( cen( v, w ) );
	EXPECT_DOUBLE_EQ( original.x, midpoint.x );
	EXPECT_DOUBLE_EQ( original.y, midpoint.y );
}
