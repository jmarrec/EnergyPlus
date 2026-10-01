#ifndef ObjexxFCL_ArrayS_functions_hh_INCLUDED
#define ObjexxFCL_ArrayS_functions_hh_INCLUDED

// ArrayS Functions
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
#include <ObjexxFCL/Array1S.hh>
#include <ObjexxFCL/Array2S.hh>
#include <ObjexxFCL/Array1D.hh>
#include <ObjexxFCL/Array2D.hh>
#include <ObjexxFCL/Array3D.hh>
#include <ObjexxFCL/Array4D.hh>
#include <ObjexxFCL/Fmath.hh>

// C++ Headers
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <type_traits>

namespace ObjexxFCL {

// pow /////

template< typename T, typename X >
inline
Array1D< T >
pow( Array1S< T > const & a, X const & x )
{
	Array1D< T > r( a );
	for ( BArray::size_type i = 0, e = a.size(); i < e; ++i ) {
		r[ i ] = T( std::pow( r[ i ], x ) );
	}
	return r;
}

// count /////

// size /////

template< typename T >
inline
BArray::size_type
size( Array2S< T > const & a, int const dim )
{
	switch ( dim ) {
	case 1:
		return a.size1();
	case 2:
		return a.size2();
	default:
		assert( false );
		return 0;
	}
}

template< template< typename > class A, typename T, class = typename std::enable_if< std::is_base_of< ArrayS< T >, A< T > >::value >::type >
inline
int
isize( A< T > const & a, int const dim )
{
	return static_cast< int >( size( a, dim ) );
}

// sum /////

template< typename T >
inline
T
sum( Array1S< T > const & a )
{
	T r( 0 );
	for ( int i = 1, e = a.u(); i <= e; ++i ) {
		r += a( i );
	}
	return r;
}

// maxval /////

template< typename T >
inline
T
maxval( Array1S< T > const & a )
{
	T r( a.empty() ? std::numeric_limits< T >::lowest() : a( 1 ) );
	for ( int i = 2, e = a.u(); i <= e; ++i ) {
		r = std::max( r, a( i ) );
	}
	return r;
}

} // ObjexxFCL

#endif // ObjexxFCL_ArrayS_functions_hh_INCLUDED
