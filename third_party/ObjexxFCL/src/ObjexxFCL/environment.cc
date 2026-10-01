// System Environment Functions
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

// ObjexxFCL Headers
#include <ObjexxFCL/environment.hh>
#include <ObjexxFCL/string.functions.hh>

// C++ Headers
#include <cstdlib>

namespace ObjexxFCL {

// Get Environment Variable
void
get_environment_variable(
 std::string const & name,
 Optional< std::string > value,
 Optional< int > length,
 Optional< int > status,
 Optional< bool const > trim_name
)
{
	char const * const cval( std::getenv( name.c_str() ) );
	std::string val( cval != nullptr ? cval : "" );
	if ( ( ! trim_name.present() ) || ( trim_name() ) ) rstrip( val ); // Strip any trailing spaces
	if ( value.present() ) value = val;
	if ( length.present() ) length = static_cast< int >( val.length() );
	if ( status.present() ) {
		if ( cval == nullptr ) { // Env var does not exist
			status = 1;
		} else { // Env var exists
			if ( value.present() ) {
				status = ( value().length() >= val.length() ? 0 : -1 );
			} else {
				status = 0;
			}
		}
	}
}

} // ObjexxFCL
