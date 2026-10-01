// IndexRange: Index Range Class
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
#include <ObjexxFCL/IndexRange.hh>

namespace ObjexxFCL {

	// Intersects Another IndexRange?
	bool
	IndexRange::intersects( IndexRange const & I ) const
	{
		if ( l_ <= u_ ) { // Bounded with positive size
			if ( I.l_ <= I.u_ ) { // I is bounded with positive size
				return ( ( l_ >= I.l_ ? l_ : I.l_ ) <= ( u_ <= I.u_ ? u_ : I.u_ ) );
			} else if ( I.l_ - 1 == I.u_ ) { // I size is zero
				return false;
			} else { // I is unbounded
				return ( I.l_ <= u_ );
			}
		} else if ( l_ - 1 == u_ ) { // Zero size
			return false; // Intersection with anything is empty
		} else { // Unbounded
			if ( I.l_ <= I.u_ ) { // I is bounded with positive size
				return ( l_ <= I.u_ );
			} else if ( I.l_ - 1 == I.u_ ) { // I size is zero
				return false;
			} else { // I is unbounded
				return true;
			}
		}
	}

	// Static Data Member Definitions
	IndexRange::size_type const IndexRange::npos = static_cast< size_type >( -1 );
	int const IndexRange::l_min = -( static_cast< int >( ( static_cast< unsigned int >( -1 ) / 2u ) ) - 1 );
	int const IndexRange::u_max = static_cast< int >( ( static_cast< unsigned int >( -1 ) / 2u ) );

} // ObjexxFCL
