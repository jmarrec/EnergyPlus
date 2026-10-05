// ObjexxFCL::ArrayS Unit Tests
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
#include <ObjexxFCL/ArrayS.all.hh>
#include <ObjexxFCL/Array.functions.hh>
#include "ObjexxFCL.unit.hh"

using namespace ObjexxFCL;

TEST( ArraySTest, Array1SBasic )
{
	Array1D_int a( 5, { 1, 2, 3, 4, 5 } );
	Array1S_int s( a( {2,3} ) );
	EXPECT_EQ( 2u, s.size() );
	EXPECT_EQ( 1, s.l() );
	EXPECT_EQ( 2, s.u() );
	EXPECT_EQ( 2, s( 1 ) );
	EXPECT_EQ( 3, s( 2 ) );
}

TEST( ArraySTest, Array1SSingleIndexSlice )
{
	Array1D_int a( 5, { 1, 2, 3, 4, 5 } );
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) // VC++2013 bug work-around
	Array1S_int s( a( {2,_} ) ); // 2 acts as the lower index
#else
	Array1S_int s( a( {2} ) ); // 2 acts as the lower index
#endif
	EXPECT_EQ( 4u, s.size() );
	EXPECT_EQ( 1, s.l() );
	EXPECT_EQ( 4, s.u() );
	EXPECT_EQ( 2, s( 1 ) );
	EXPECT_EQ( 3, s( 2 ) );
	EXPECT_EQ( 4, s( 3 ) );
	EXPECT_EQ( 5, s( 4 ) );
}

TEST( ArraySTest, Array1SEmptySlice )
{
	Array1D_int a( 5, { 1, 2, 3, 4, 5 } );
	Array1S_int s( a( {2,-2} ) ); // Empty slice
	EXPECT_EQ( 0u, s.size() );
	EXPECT_EQ( 1, s.l() );
	EXPECT_EQ( 0, s.u() );
	EXPECT_EQ( 0u, s.size() );
}

TEST( ArraySTest, Array1SEmptySliceOfEmptyArray )
{
	Array1D_int a( {5,-5} ); // Empty array: Can only slice with empty slice
	Array1S_int s( a( {2,-2} ) ); // Empty slice: Legal
	EXPECT_EQ( 0u, s.size() );
	EXPECT_EQ( 1, s.l() );
	EXPECT_EQ( 0, s.u() );
	EXPECT_EQ( 0u, s.size() );
}

TEST( ArraySTest, Array2SSingleIndexSlice )
{
	Array2D_int A( 2, 2, { 11, 12, 21, 22 } );
	Array2S_int S( A( {2,_}, {1,_} ) );
	EXPECT_EQ( 2u, S.size() );
	EXPECT_EQ( 1, S.l1() );
	EXPECT_EQ( 1, S.u1() );
	EXPECT_EQ( 1, S.l2() );
	EXPECT_EQ( 2, S.u2() );
	EXPECT_EQ( 21, S( 1, 1 ) );
	EXPECT_EQ( 22, S( 1, 2 ) );
}

TEST( ArraySTest, Array2D1SSlice )
{
	Array2D_int A( 2, 2, { 11, 12, 21, 22 } );
	Array1S_int S( A( _, 2 ) );
	EXPECT_EQ( 2u, S.size() );
	EXPECT_EQ( 1, S.l() );
	EXPECT_EQ( 2, S.u() );
	EXPECT_EQ( 1, S.l1() );
	EXPECT_EQ( 2, S.u1() );
	EXPECT_EQ( 12, S( 1 ) );
	EXPECT_EQ( 22, S( 2 ) );
}

TEST( ArraySTest, Array2D1SSlice65 )
{
	Array2D_int A( 6, 5, 999 );
	Array1S_int S( A( _, 2 ) );
	EXPECT_EQ( 6u, S.size() );
	EXPECT_EQ( 1, S.l() );
	EXPECT_EQ( 6, S.u() );
	EXPECT_EQ( 1, S.l1() );
	EXPECT_EQ( 6, S.u1() );
}

TEST( ArraySTest, Array2D2SSlice )
{
	Array2D_int A( 3, 3, { 11, 12, 13, 21, 22, 23, 31, 32, 33 } );
	Array2S_int S( A( {1,2}, _ ) );
	EXPECT_EQ( 6u, S.size() );
	EXPECT_EQ( 1, S.l1() );
	EXPECT_EQ( 2, S.u1() );
	EXPECT_EQ( 1, S.l2() );
	EXPECT_EQ( 3, S.u2() );
	EXPECT_EQ( 11, S( 1, 1 ) );
	EXPECT_EQ( 12, S( 1, 2 ) );
	EXPECT_EQ( 13, S( 1, 3 ) );
	EXPECT_EQ( 21, S( 2, 1 ) );
	EXPECT_EQ( 22, S( 2, 2 ) );
	EXPECT_EQ( 23, S( 2, 3 ) );
}

TEST( ArraySTest, Array2D1DOTFSlice )
{
	Array2D_int A( 2, 2, { 11, 12, 21, 22 } );
	Array1D_int B( A( {1,2}, 2 ) ); // Make a real 1D array out of slice on the fly
	EXPECT_EQ( 1, B.l() );
	EXPECT_EQ( 2, B.u() );
	EXPECT_EQ( 12, B( 1 ) );
	EXPECT_EQ( 22, B( 2 ) );
}

TEST( ArraySTest, Array1SWholeArraySlice )
{
	Array1D_int a( {-2,2}, { 1, 2, 3, 4, 5 } );
	Array1S_int s( a );
	EXPECT_EQ( 5u, s.size() );
	EXPECT_EQ( 1, s.l() );
	EXPECT_EQ( 5, s.u() );
	EXPECT_EQ( 1, s( 1 ) );
	EXPECT_EQ( 2, s( 2 ) );
	EXPECT_EQ( 3, s( 3 ) );
	EXPECT_EQ( 4, s( 4 ) );
	EXPECT_EQ( 5, s( 5 ) );
}

TEST( ArraySTest, Array2SWholeArraySlice )
{
	Array2D_int a( {-1,1}, {-1,1}, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } );
	Array2S_int s( a );
	EXPECT_EQ( 9u, s.size() );
	EXPECT_EQ( 1, s.l1() );
	EXPECT_EQ( 1, s.l2() );
	EXPECT_EQ( 3, s.u1() );
	EXPECT_EQ( 3, s.u2() );
	EXPECT_EQ( 1, s( 1, 1 ) );
	EXPECT_EQ( 2, s( 1, 2 ) );
	EXPECT_EQ( 3, s( 1, 3 ) );
	EXPECT_EQ( 4, s( 2, 1 ) );
	EXPECT_EQ( 5, s( 2, 2 ) );
	EXPECT_EQ( 6, s( 2, 3 ) );
	EXPECT_EQ( 7, s( 3, 1 ) );
	EXPECT_EQ( 8, s( 3, 2 ) );
	EXPECT_EQ( 9, s( 3, 3 ) );
}

TEST( ArraySTest, StreamOut )
{
	Array1D_int const A( 3, { 1, 2, 3 } );
	Array1S_int S( A );
	std::ostringstream stream;
	stream << S;
	EXPECT_EQ( "           1            2            3 ", stream.str() );
}

