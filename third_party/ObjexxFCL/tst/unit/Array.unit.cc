// ObjexxFCL::Array Unit Tests
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

// Google Test Headers
#include <gtest/gtest.h>

// ObjexxFCL Headers
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244) // Suppress conversion warnings: Intentional narrowing assignments present
#endif
#include <ObjexxFCL/Array.all.hh>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include "ObjexxFCL.unit.hh"

// C++ Headers
#include <array>

using namespace ObjexxFCL;
typedef  IndexRange  IR;

TEST( ArrayTest, DefaultConstruction )
{
	Array2D_int A, B;
	EXPECT_EQ( 0u, A.size() );
}

TEST( ArrayTest, Construction2DIndexRangeInitializerList )
{
	Array2D_int r( IR( -1, 1 ), IR( -1, 1 ), { 1, 2, 3, 4, 5, 6, 7, 8, 9 } );
	EXPECT_EQ( -1, r.l1() );
	EXPECT_EQ( 1, r.u1() );
	EXPECT_EQ( -1, r.l2() );
	EXPECT_EQ( 1, r.u2() );
	for ( int i = -1, k = 1; i <= 1; ++i ) {
		for ( int j = -1; j <= 1; ++j, ++k ) {
			EXPECT_EQ( k, r( i, j ) );
		}
	}
	int v( 0 );
	for ( auto const e : r ) {
		EXPECT_EQ( ++v, e );
	}
	v = 10;
	for ( auto & e : r ) {
		e = ++v;
		EXPECT_EQ( v, e );
	}
}

TEST( ArrayTest, Construction2DDifferentValueType )
{
	Array2D_int A( 3, 3, 33 );
	Array2D_double B( A );
	EXPECT_TRUE( eq( Array2D_double( 3, 3, 33.0 ), B ) );
}

TEST( ArrayTest, Assignment2D )
{
	Array2D_int A( 3, 3, 33 );
	Array2D_int B( IR( 0, 4 ), IR( 0, 4 ), 44 );
	A = B;
	EXPECT_TRUE( eq( B, A ) );
}

TEST( ArrayTest, Assignment2DRedimensionDifferentValueType )
{
	Array2D_int A( 3, 3, 33 );
	Array2D_double B( IR( 0, 4 ), IR( 0, 4 ), 4.4 );
	A = B; // Causes VC++ C4244 warning
	EXPECT_TRUE( eq( Array2D_int( IR( 0, 4 ), IR( 0, 4 ), 4 ), A ) );
}

TEST( ArrayTest, Assignment2DNoOverlapProxy )
{
	Array2D_int A( 3, 3, 33 );
	Array2A_int B( A( 2, 2 ), 2, 2 );
	B = 44;
	Array2A_int C( A( 1, 1 ), 2, 2 );
	C = 55;
	EXPECT_TRUE( A.overlap( B ) );
	EXPECT_TRUE( A.overlap( C ) );
	EXPECT_TRUE( B.overlap( A ) );
	EXPECT_TRUE( C.overlap( A ) );
	EXPECT_FALSE( B.overlap( C ) );
	EXPECT_FALSE( C.overlap( B ) );
	B = C;
	EXPECT_TRUE( eq( C, B ) );
	Array2A_int D( A( 3, 3 ), 1, 1 );
	EXPECT_FALSE( C.overlap( D ) );
	EXPECT_FALSE( D.overlap( C ) );
	EXPECT_FALSE( B.overlap( D ) );
	EXPECT_FALSE( D.overlap( B ) );
	EXPECT_EQ( 55, A( 1, 1 ) );
	EXPECT_EQ( 55, A( 1, 2 ) );
	EXPECT_EQ( 55, A( 1, 3 ) );
	EXPECT_EQ( 55, A( 2, 1 ) );
	EXPECT_EQ( 55, A( 2, 2 ) );
	EXPECT_EQ( 55, A( 2, 3 ) );
	EXPECT_EQ( 55, A( 3, 1 ) );
	EXPECT_EQ( 55, A( 3, 2 ) );
	EXPECT_EQ( 33, A( 3, 3 ) );
}

TEST( ArrayTest, Assignment2DOverlapProxy )
{
	Array2D_int A( 3, 3, 33 );
	Array2A_int B( A( 1, 1 ), 2, 3 ); // Rows 1-2 of A
	B = 44;
	EXPECT_EQ( 44, A( 1, 1 ) );
	EXPECT_EQ( 44, A( 1, 2 ) );
	EXPECT_EQ( 44, A( 1, 3 ) );
	EXPECT_EQ( 44, A( 2, 1 ) );
	EXPECT_EQ( 44, A( 2, 2 ) );
	EXPECT_EQ( 44, A( 2, 3 ) );
	EXPECT_EQ( 33, A( 3, 1 ) );
	EXPECT_EQ( 33, A( 3, 2 ) );
	EXPECT_EQ( 33, A( 3, 3 ) );
	Array2A_int C( A( 2, 1 ), 2, 3 ); // Rows 2-3 of A
	C = 55;
	EXPECT_TRUE( A.overlap( B ) );
	EXPECT_TRUE( A.overlap( C ) );
	EXPECT_TRUE( B.overlap( A ) );
	EXPECT_TRUE( C.overlap( A ) );
	EXPECT_TRUE( B.overlap( C ) );
	EXPECT_TRUE( C.overlap( B ) );
	B = C;
	EXPECT_TRUE( eq( C, B ) );
	EXPECT_TRUE( eq( 55, A ) );
}

TEST( ArrayTest, Assignment2DOverlapProxyVarying )
{
	Array2D_int A( 2, 2, 1 );
	Array1A_int B( A( 1, 1 ), 3 ); // First 3 elements of A
	B( 1 ) = 1;
	B( 2 ) = 2;
	B( 3 ) = 3;
	EXPECT_EQ( 1, A( 1, 1 ) );
	EXPECT_EQ( 2, A( 1, 2 ) );
	EXPECT_EQ( 3, A( 2, 1 ) );
	EXPECT_EQ( 1, A( 2, 2 ) );
	Array1A_int C( A( 1, 2 ), 3 ); // Last 3 elements of A
	EXPECT_TRUE( A.overlap( B ) );
	EXPECT_TRUE( A.overlap( C ) );
	EXPECT_TRUE( B.overlap( A ) );
	EXPECT_TRUE( C.overlap( A ) );
	EXPECT_TRUE( B.overlap( C ) );
	EXPECT_TRUE( C.overlap( B ) );
	C = B; // Need overlap-safe copy
	EXPECT_FALSE( eq( C, B ) ); // Differ after overlapping assignment
	EXPECT_FALSE( eq( B, C ) ); // Differ after overlapping assignment
	EXPECT_EQ( 1, A( 1, 1 ) );
	EXPECT_EQ( 1, A( 1, 2 ) );
	EXPECT_EQ( 2, A( 2, 1 ) );
	EXPECT_EQ( 3, A( 2, 2 ) );
}

TEST( ArrayTest, Swap3D )
{
	Array3D_int A( 4, 4, 4, 44 );
	Array3D_int( 5, 5, 5, 55 ).swap( A );
	EXPECT_EQ( IR( 1, 5 ), A.I1() );
	EXPECT_EQ( IR( 1, 5 ), A.I2() );
	EXPECT_EQ( IR( 1, 5 ), A.I3() );
	EXPECT_EQ( 5u, A.size1() );
	EXPECT_EQ( 5u, A.size2() );
	EXPECT_EQ( 5u, A.size3() );
	EXPECT_EQ( 5u * 5u * 5u, A.size() );
	for ( std::size_t i = 0; i < A.size(); ++i ) {
		EXPECT_EQ( 55, A[ i ] );
	}
}

TEST( ArrayTest, Cross1D )
{
	Array1D_int A( 3, 33 ), B( 3, 11 );
	EXPECT_TRUE( eq( cross( A, B ), cross_product( A, B ) ) );
}

TEST( ArrayTest, LogicalNegation )
{
	Array2D_bool const F( 2, 2, false );
	EXPECT_FALSE( F( 1, 1 ) );
	EXPECT_FALSE( F( 1, 2 ) );
	EXPECT_FALSE( F( 2, 1 ) );
	EXPECT_FALSE( F( 2, 2 ) );
}

TEST( ArrayTest, EmptyComparisonElemental )
{
	Array1D_int a( 0 ), b( 0 ); // Empty
	EXPECT_EQ( 0u, a.size() );
	EXPECT_EQ( 0u, b.size() );
}

TEST( ArrayTest, Unallocated )
{
	Array1D_int a; // Empty
	EXPECT_FALSE( a.allocated() );
	EXPECT_FALSE( allocated( a ) );
	EXPECT_DEBUG_DEATH( a( 1 ), ".*Assertion.*" );
}

TEST( ArrayTest, AnyOp2D )
{
	Array2D_int const A( 3, 3, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } );
	EXPECT_TRUE( any_ne( A, 6 ) );
}

TEST( ArrayTest, CountOp2D )
{
	Array2D_int const A( 3, 3, { 1, 2, 2, 3, 3, 3, 7, 8, 9 } );
	EXPECT_EQ( 0u, count_eq( A, 0 ) );
	EXPECT_EQ( 1u, count_eq( A, 1 ) );
	EXPECT_EQ( 2u, count_eq( A, 2 ) );
	EXPECT_EQ( 3u, count_eq( A, 3 ) );
}

TEST( ArrayTest, Functions1D )
{
	Array1D_int u{ 1, 2, 3 };
	Array1D_int v{ 2, 3, 4 };
	EXPECT_EQ( 14, magnitude_squared( u ) );
	EXPECT_EQ( 3, distance_squared( u, v ) );
	EXPECT_EQ( 20, dot( u, v ) );
}
