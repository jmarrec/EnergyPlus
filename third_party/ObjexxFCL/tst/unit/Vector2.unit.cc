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
	Vector2<float> v( 15.0 ); // Uniform value construction
	EXPECT_EQ( 15.0f, v.x );
	EXPECT_EQ( 15.0f, v.y );
}

TEST( Vector2Test, InitializerList )
{
	Vector2<int> v( { 33, 52 } );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	EXPECT_EQ( 33, v[ 0 ] );
	EXPECT_EQ( 52, v[ 1 ] );
	v = { 44, 55 };
	EXPECT_EQ( 44, v.x );
	EXPECT_EQ( 55, v.y );
	EXPECT_EQ( 44, v.x );
	EXPECT_EQ( 55, v.y );
}

TEST( Vector2Test, StdArray )
{
	std::array< int, 2 > arr = {{ 33, 52 }};
	Vector2<int> v( arr );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	arr = {{ 133, 152 }};
	v = arr;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
}

TEST( Vector2Test, StdVector )
{
	std::vector< int > vec( { 33, 52 } );
	Vector2<int> v( vec );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	vec = { 133, 152 };
	v = vec;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
}

TEST( Vector2Test, Array )
{
	Array1D_int a( 2, { 33, 52 } );
	Vector2<int> v( a );
	EXPECT_EQ( 33, v.x );
	EXPECT_EQ( 52, v.y );
	a = { 133, 152 };
	v = a;
	EXPECT_EQ( 133, v.x );
	EXPECT_EQ( 152, v.y );
}

TEST( Vector2Test, MinMax )
{
	Vector2<double> v( 1.0, 5.0 );
	Vector2<double> w( 3.0, 2.0 );
	EXPECT_EQ( 5.0, v.y );
}

TEST( Vector2Test, Comparisons )
{
	Vector2<double> v( 1.0, 2.0 );
	Vector2<double> w( 1.0, 2.0 );


	v.x = 0.0;
}

TEST( Vector2Test, Generators )
{
	Vector2<double> v( 1.0, 12.0 );
	Vector2<double> w( 2.0, 6.0 );
}

TEST( Vector2Test, String )
{
	std::string const X( "X" );
	Vector2<std::string> v( X );
	EXPECT_EQ( X, v.x );
	EXPECT_EQ( X, v.y );
	Vector2<std::string> w;
	w = v;
	EXPECT_EQ( X, w.x );
	EXPECT_EQ( X, w.y );
}
