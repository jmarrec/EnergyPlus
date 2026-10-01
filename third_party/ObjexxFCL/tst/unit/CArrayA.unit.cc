// ObjexxFCL::CArrayA Unit Tests
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
#include <ObjexxFCL/CArrayA.hh>
#include "ObjexxFCL.unit.hh"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

using namespace ObjexxFCL;

TEST( CArrayATest, SizeConstruction )
{
	CArrayA<int> v( 10u );
	EXPECT_EQ( 10u, v.size() );

	CArrayA<int> e( 0u );
	EXPECT_EQ( 0u, e.size() );
}

TEST( CArrayATest, Subscripting )
{
	CArrayA<int> v( 10u );
	for ( int i = 0; i < 10; ++i ) {
		v[ i ] = 22; // Signed index
	}
	v[ 3u ] = 33; // Unsigned index
	EXPECT_EQ( 22, v[ 0u ] );
	EXPECT_EQ( 33, v[ 3u ] );
	EXPECT_EQ( 33, v[ 3 ] );
	EXPECT_EQ( 22, v[ 9u ] );
}
