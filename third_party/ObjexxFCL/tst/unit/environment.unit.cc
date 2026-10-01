// ObjexxFCL::environment Unit Tests
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
#include <ObjexxFCL/environment.hh>
#include "ObjexxFCL.unit.hh"

// C++ Headers
#include <cstdlib>

using namespace ObjexxFCL;

namespace {

bool set_env( char const * name, char const * value )
{
#ifdef _WIN32
	return ( _putenv_s( name, value ) == 0 );
#else
	return ( setenv( name, value, 1 ) == 0 );
#endif
}

}

TEST( EnvironmentTest, GetEnvironmentVariable )
{
	EXPECT_TRUE( set_env( "ObjexxFCL_Test_Env_Var", "get_environment_variable" ) );
	std::string val;
	get_environment_variable( "ObjexxFCL_Test_Env_Var", val );
	EXPECT_EQ( "get_environment_variable", val );
}
