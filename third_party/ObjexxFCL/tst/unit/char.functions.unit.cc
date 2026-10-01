// ObjexxFCL::char.functions Unit Tests
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
#include <ObjexxFCL/char.functions.hh>
#include "ObjexxFCL.unit.hh"

using namespace ObjexxFCL;

TEST( charFunctionsTest, Predicate )
{
	EXPECT_TRUE( is_blank( ' ' ) );
	EXPECT_FALSE( is_blank( 'x' ) );
	EXPECT_TRUE( not_blank( 'x' ) );
	EXPECT_TRUE( is_any_of( 'x', "xyz" ) );
	EXPECT_TRUE( is_any_of( 'x', std::string( "xyz" ) ) );
	EXPECT_FALSE( is_any_of( 'b', "xyz" ) );
}

TEST( charFunctionsTest, Comparison )
{
	EXPECT_TRUE( equali( 'a', 'A' ) );
	EXPECT_FALSE( equali( 'a', 'X' ) );
	EXPECT_TRUE( lessthani( 'a', 'b' ) );
	EXPECT_TRUE( lessthani( 'a', 'B' ) );
	EXPECT_TRUE( lessthani( 'A', 'b' ) );
}

TEST( charFunctionsTest, Modifier )
{
	char s( 'f' );
	EXPECT_EQ( 'f', s );
	uppercase( s );
	EXPECT_EQ( 'F', s );
	EXPECT_TRUE( equali( s, 'F' ) );
	EXPECT_TRUE( equali( s, 'f' ) );
	EXPECT_EQ( 'F', uppercase( s ) );
}

TEST( charFunctionsTest, Generator )
{
	char const s( 'F' );
	EXPECT_EQ( 'f', to_lower( s ) );
	EXPECT_EQ( 'F', to_upper( s ) );
}
