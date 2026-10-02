// Licensing is available from Objexx Engineering, Inc.:  http://objexx.com

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244) // Suppress conversion warnings: Intentional narrowing assignments present
#endif

// Google Test Headers
#include <gtest/gtest.h>

// ObjexxFCL Headers
#include <ObjexxFCL/Vector3.hh>
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

TEST( Vector3Test, Basic )
{
	Vector3<float> v( 15.0 ); // Uniform value construction
	EXPECT_EQ( 15.0f, v.x );
	EXPECT_EQ( 15.0f, v.y );
	EXPECT_EQ( 15.0f, v.z );
	v.normalize();
	EXPECT_FLOAT_EQ( 1.0f, v.length() );
	v.normalize( 5.0f );
	EXPECT_FLOAT_EQ( 5.0f, v.length() );
}

TEST( Vector3Test, InitializerList )
{
	Vector3<int> v( { 33, 52, 17 } );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 17, v.z );
	EXPECT_EQ( 33, v[ 0 ] );
	EXPECT_EQ( 52, v[ 1 ] );
	EXPECT_EQ( 17, v[ 2 ] );
	EXPECT_EQ( 33, v( 1 ) );
	EXPECT_EQ( 52, v( 2 ) );
	EXPECT_EQ( 17, v( 3 ) );
	v = { 44, 55, 66 };
	EXPECT_EQ( 44, v.x );
	EXPECT_EQ( 55, v.y );
	EXPECT_EQ( 66, v.z );
	v *= 2;
	EXPECT_EQ( 88, v.x );
	EXPECT_EQ( 110, v.y );
	EXPECT_EQ( 132, v.z );
	v /= 2.0; // May cause conversion warning
	EXPECT_EQ( 44, v.x );
	EXPECT_EQ( 55, v.y );
	EXPECT_EQ( 66, v.z );
	v.negate();
	EXPECT_EQ( -44, v.x );
	EXPECT_EQ( -55, v.y );
	EXPECT_EQ( -66, v.z );
}

TEST( Vector3Test, StdArray )
{
	std::array< int, 3 > arr = {{ 33, 52, 17 }};
	Vector3<int> v( arr );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 17, v.z );
	arr = {{ 133, 152, 117 }};
	v = arr;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
}

TEST( Vector3Test, StdVector )
{
	std::vector< int > vec( { 33, 52, 17 } );
	Vector3<int> v( vec );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 17, v.z );
	vec = { 133, 152, 117 };
	v = vec;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
}

TEST( Vector3Test, Array )
{
	Array1D_int a( 3, { 33, 52, 17 } );
	Vector3<int> v( a );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 17, v.z );
	a = { 133, 152, 117 };
	v = a;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
	EXPECT_EQ( 117, v.z );
}

TEST( Vector3Test, MinMax )
{
	Vector3<double> v( 1.0, 5.0, 3.0 );
	Vector3<double> w( 3.0, 2.0, 7.0 );
	v.max( w );
	EXPECT_EQ( 3.0, v.x );
	EXPECT_EQ( 5.0, v.y );
	EXPECT_EQ( 7.0, v.z );
	w.max( v );
	EXPECT_EQ( v, w );
}

TEST( Vector3Test, Comparisons )
{
	Vector3<double> v( 1.0, 2.0, 3.0 );
	Vector3<double> w( 1.0, 2.0, 3.0 );

	EXPECT_EQ( v, w );

	v.x = 0.0;
	EXPECT_TRUE( v != w );
	EXPECT_TRUE( ! ( v == w ) );
}

TEST( Vector3Test, Generators )
{
	Vector3<double> v( 1.0, 12.0, 21.0 );
	Vector3<double> w( 2.0, 6.0, 7.0 );
	EXPECT_EQ( Vector3<double>( 3.0, 18.0, 28.0 ), v + w );
	EXPECT_EQ( Vector3<double>( -1.0, 6.0, 14.0 ), v - w );
}

TEST( Vector3Test, Distance )
{
	Vector3<double> v( 3.0, 3.0, 0.0 );
	Vector3<double> w( 3.0, 2.0, 0.0 );
	EXPECT_DOUBLE_EQ( 1.0, distance( v, w ) );
	EXPECT_DOUBLE_EQ( 1.0, distance_squared( v, w ) );
}

TEST( Vector3Test, Dot )
{
	Vector3<double> x( 3.0, 0.0, 0.0 );
	Vector3<double> y( 0.0, 2.0, 0.0 );
	EXPECT_EQ( 0.0, dot( x, y ) );
}

TEST( Vector3Test, Cross )
{
	Vector3<double> x( 3.0, 0.0, 0.0 );
	Vector3<double> y( 0.0, 2.0, 0.0 );
	EXPECT_EQ( Vector3<double>( 0.0, 0.0, 6.0 ), cross( x, y ) );
}

TEST( Vector3Test, Center )
{
	Vector3<double> x( 4.0, 0.0, 77.0 );
	Vector3<double> y( 0.0, 4.0, 77.0 );
	EXPECT_EQ( Vector3<double>( 2.0, 2.0, 77.0 ), cen( x, y ) );
}

TEST( Vector3Test, BinaryOperations )
{
	Vector3<double> v( 1.0, 2.0, 3.0 );
	Vector3<double> w( 1.0, 2.0, 3.0 );
	Vector3<double> const original( v );

	// Tweak the vectors and compute cross product
	Vector3<double> const c( cross( v, w ) );
	EXPECT_DOUBLE_EQ( dot( c, v ), 0.0 ); // t and v are orthogonal
	EXPECT_DOUBLE_EQ( dot( c, w ), 0.0 ); // t and w are orthogonal

	// Check midpoint (should match original vector)
	Vector3<double> const midpoint( mid( v, w ) );
	EXPECT_DOUBLE_EQ( original.x, midpoint.x );
	EXPECT_DOUBLE_EQ( original.y, midpoint.y );
	EXPECT_DOUBLE_EQ( original.z, midpoint.z );
}
