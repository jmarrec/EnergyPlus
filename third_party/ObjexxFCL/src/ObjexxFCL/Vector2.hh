#ifndef ObjexxFCL_Vector2_hh_INCLUDED
#define ObjexxFCL_Vector2_hh_INCLUDED

// Vector2: Fast 2-Element Vector
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
#include <ObjexxFCL/Vector2.fwd.hh>
#include <ObjexxFCL/Fmath.hh>
#include <ObjexxFCL/TypeTraits.hh>

// C++ Headers
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace ObjexxFCL {

// Vector2: Fast 2-Element Vector
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
// . Use std::array< T, 2 > instead in array/vectorization context
template< typename T >
class Vector2
{

private: // Friends

	template< typename > friend class Vector2;

public: // Types

	typedef  TypeTraits< T >  Traits;
	typedef  typename std::conditional< std::is_scalar< T >::value, T const, T const & >::type  Tc;
	typedef  typename std::conditional< std::is_scalar< T >::value, typename std::remove_const< T >::type, T const & >::type  Tr;

	// STL Style
	typedef  T  value_type;
	typedef  T &  reference;
	typedef  T const &  const_reference;
	typedef  T *  pointer;
	typedef  T const *  const_pointer;
	typedef  std::size_t  size_type;
	typedef  std::ptrdiff_t  difference_type;

	// C++ Style
	typedef  T  Value;
	typedef  T &  Reference;
	typedef  T const &  ConstReference;
	typedef  T *  Pointer;
	typedef  T const *  ConstPointer;
	typedef  std::size_t  Size;
	typedef  std::ptrdiff_t  Difference;

public: // Creation

	// Default Constructor
	Vector2()
#if defined(OBJEXXFCL_ARRAY_INIT) || defined(OBJEXXFCL_ARRAY_INIT_DEBUG)
	 :
	 x( Traits::initial_array_value() ),
	 y( Traits::initial_array_value() )
#endif
	{}

	// Copy Constructor
	Vector2( Vector2 const & v ) :
	 x( v.x ),
	 y( v.y )
	{}

	// Uniform Value Constructor
	explicit
	Vector2( Tc t ) :
	 x( t ),
	 y( t )
	{}

	// Value Constructor
	Vector2(
	 Tc x_,
	 Tc y_
	) :
	 x( x_ ),
	 y( y_ )
	{}

	// Array Constructor Template
	template< typename A, class = typename std::enable_if< std::is_constructible< T, typename A::value_type >::value >::type >
	Vector2( A const & a ) :
	 x( a[ 0 ] ),
	 y( a[ 1 ] )
	{
		assert( a.size() == 2 );
	}

	// Destructor
	~Vector2()
	{}

public: // Assignment

	// Copy Assignment
	Vector2 &
	operator =( Vector2 const & v )
	{
		if ( this != &v ) {
			x = v.x;
			y = v.y;
		}
		return *this;
	}

public: // Assignment: Scaled

public: // Subscript

	// Vector2[ i ] const: 0-Based Index
	Tr
	operator []( size_type const i ) const
	{
		assert( i <= 1 );
		return ( i == 0 ? x : y );
	}

public: // Properties: Predicates

public: // Properties: General

	// Length
	T
	length() const
	{
		return std::sqrt( ( x * x ) + ( y * y ) );
	}

	// Magnitude Squared
	T
	magnitude_squared() const
	{
		return ( x * x ) + ( y * y );
	}

	// Dot Product with a Vector2
	T
	dot( Vector2 const & v ) const
	{
		return ( x * v.x ) + ( y * v.y );
	}

	// Cross Product with a Vector2
	T
	cross( Vector2 const & v ) const
	{
		return ( x * v.y ) - ( y * v.x );
	}


public: // Data

	T x, y; // Elements

}; // Vector2

// Vector2 - Vector2
template< typename T >
inline
Vector2< T >
operator -( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >( a.x - b.x, a.y - b.y );
}

} // ObjexxFCL

#endif // ObjexxFCL_Vector2_hh_INCLUDED
