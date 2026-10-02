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

	// Copy Constructor Template
	template< typename U, class = typename std::enable_if< std::is_constructible< T, U >::value >::type >
	Vector2( Vector2< U > const & v ) :
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

	// Initializer List Constructor Template
	template< typename U, class = typename std::enable_if< std::is_constructible< T, U >::value >::type >
	Vector2( std::initializer_list< U > const l ) :
	 x( *l.begin() ),
	 y( *( l.begin() + 1 ) )
	{
		assert( l.size() == 2 );
	}

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

	// Copy Assignment Template
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator =( Vector2< U > const & v )
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	// Initializer List Assignment Template
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator =( std::initializer_list< U > const l )
	{
		assert( l.size() == 2 );
		auto i( l.begin() );
		x = *i;
		y = *(++i);
		return *this;
	}

	// Array Assignment Template
	template< typename A, class = typename std::enable_if< std::is_assignable< T&, typename A::value_type >::value >::type >
	Vector2 &
	operator =( A const & a )
	{
		assert( a.size() == 2 );
		x = a[ 0 ];
		y = a[ 1 ];
		return *this;
	}

	// += Vector2
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator +=( Vector2< U > const & v )
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	// -= Vector2
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator -=( Vector2< U > const & v )
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	// *= Vector2
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator *=( Vector2< U > const & v )
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	// /= Vector2
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator /=( Vector2< U > const & v )
	{
		assert( v.x != T( 0 ) );
		assert( v.y != T( 0 ) );
		x /= v.x;
		y /= v.y;
		return *this;
	}

	// += Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator +=( std::initializer_list< U > const l )
	{
		assert( l.size() == 2 );
		auto i( l.begin() );
		x += *i;
		y += *(++i);
		return *this;
	}

	// -= Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator -=( std::initializer_list< U > const l )
	{
		assert( l.size() == 2 );
		auto i( l.begin() );
		x -= *i;
		y -= *(++i);
		return *this;
	}

	// *= Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator *=( std::initializer_list< U > const l )
	{
		assert( l.size() == 2 );
		auto i( l.begin() );
		x *= *i;
		y *= *(++i);
		return *this;
	}

	// /= Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< T&, U >::value >::type >
	Vector2 &
	operator /=( std::initializer_list< U > const l )
	{
		assert( l.size() == 2 );
		auto i( l.begin() );
		assert( *i != T( 0 ) );
		assert( *(i+1) != T( 0 ) );
		x /= *i;
		y /= *(++i);
		return *this;
	}

	// += Array
	template< typename A, class = typename std::enable_if< std::is_assignable< T&, typename A::value_type >::value >::type >
	Vector2 &
	operator +=( A const & a )
	{
		assert( a.size() == 2 );
		x += a[ 0 ];
		y += a[ 1 ];
		return *this;
	}

	// -= Array
	template< typename A, class = typename std::enable_if< std::is_assignable< T&, typename A::value_type >::value >::type >
	Vector2 &
	operator -=( A const & a )
	{
		assert( a.size() == 2 );
		x -= a[ 0 ];
		y -= a[ 1 ];
		return *this;
	}

	// *= Array
	template< typename A, class = typename std::enable_if< std::is_assignable< T&, typename A::value_type >::value >::type >
	Vector2 &
	operator *=( A const & a )
	{
		assert( a.size() == 2 );
		x *= a[ 0 ];
		y *= a[ 1 ];
		return *this;
	}

	// /= Array
	template< typename A, class = typename std::enable_if< std::is_assignable< T&, typename A::value_type >::value >::type >
	Vector2 &
	operator /=( A const & a )
	{
		assert( a.size() == 2 );
		assert( a[ 0 ] != T( 0 ) );
		assert( a[ 1 ] != T( 0 ) );
		x /= a[ 0 ];
		y /= a[ 1 ];
		return *this;
	}

	// = Value
	Vector2 &
	operator =( Tc t )
	{
		x = y = t;
		return *this;
	}

	// += Value
	Vector2 &
	operator +=( Tc t )
	{
		x += t;
		y += t;
		return *this;
	}

	// -= Value
	Vector2 &
	operator -=( Tc t )
	{
		x -= t;
		y -= t;
		return *this;
	}

	// *= Value
	Vector2 &
	operator *=( Tc t )
	{
		x *= t;
		y *= t;
		return *this;
	}

	// /= Value
	template< typename U, class = typename std::enable_if< std::is_floating_point< U >::value && std::is_assignable< T&, U >::value >::type, typename = void >
	Vector2 &
	operator /=( U const & u )
	{
		assert( u != U( 0 ) );
		U const inv_u( U( 1 ) / u );
		x *= inv_u;
		y *= inv_u;
		return *this;
	}

	// /= Value
	template< typename U, class = typename std::enable_if< ! std::is_floating_point< U >::value && std::is_assignable< T&, U >::value >::type, typename = void, typename = void >
	Vector2 &
	operator /=( U const & u )
	{
		assert( u != U( 0 ) );
		x /= u;
		y /= u;
		return *this;
	}

	// Value Assignment
	Vector2 &
	assign(
	 Tc x_,
	 Tc y_
	)
	{
		x = x_;
		y = y_;
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

	// Vector2[ i ]: 0-Based Index
	T &
	operator []( size_type const i )
	{
		assert( i <= 1 );
		return ( i == 0 ? x : y );
	}

	// Vector2( i ) const: 1-Based Index
	Tr
	operator ()( size_type const i ) const
	{
		assert( ( 1 <= i ) && ( i <= 2 ) );
		return ( i == 1 ? x : y );
	}

	// Vector2( i ): 1-Based Index
	T &
	operator ()( size_type const i )
	{
		assert( ( 1 <= i ) && ( i <= 2 ) );
		return ( i == 1 ? x : y );
	}

public: // Properties: Predicates

public: // Properties: General

	// Size
	Size
	size() const
	{
		return 2u;
	}

	// Length
	T
	length() const
	{
		return std::sqrt( ( x * x ) + ( y * y ) );
	}

	// Magnitude
	T
	magnitude() const
	{
		return std::sqrt( ( x * x ) + ( y * y ) );
	}

	// Magnitude Squared
	T
	magnitude_squared() const
	{
		return ( x * x ) + ( y * y );
	}

	// Magnitude Squared
	T
	mag_squared() const
	{
		return ( x * x ) + ( y * y );
	}

	// Distance to a Vector2
	T
	distance( Vector2 const & v ) const
	{
		return std::sqrt( square( x - v.x ) + square( y - v.y ) );
	}

	// Distance Squared to a Vector2
	T
	distance_squared( Vector2 const & v ) const
	{
		return square( x - v.x ) + square( y - v.y );
	}

	// Dot Product with a Vector2
	T
	dot( Vector2 const & v ) const
	{
		return ( x * v.x ) + ( y * v.y );
	}

	// Dot Product with an Array
	template< typename A, class = typename std::enable_if< std::is_assignable< T&, typename A::value_type >::value >::type >
	T
	dot( A const & a ) const
	{
		assert( a.size() == 2 );
		return ( x * a[ 0 ] ) + ( y * a[ 1 ] );
	}

	// Cross Product with a Vector2
	T
	cross( Vector2 const & v ) const
	{
		return ( x * v.y ) - ( y * v.x );
	}

	// Cross Product with an Array
	template< typename A, class = typename std::enable_if< std::is_assignable< T&, typename A::value_type >::value >::type >
	T
	cross( A const & a ) const
	{
		assert( a.size() == 2 );
		return ( x * a[ 1 ] ) - ( y * a[ 0 ] );
	}

public: // Modifiers

	// Negate
	Vector2 &
	negate()
	{
		x = -x;
		y = -y;
		return *this;
	}

	// Normalize to a Length
	Vector2 &
	normalize( Tc tar_length = T( 1 ) )
	{
		T const cur_length( length() );
		assert( cur_length != T ( 0 ) );
		T const dilation( tar_length / cur_length );
		x *= dilation;
		y *= dilation;
		return *this;
	}

	// Minimum Coordinates with a Vector2
	Vector2 &
	min( Vector2 const & v )
	{
		x = ( x <= v.x ? x : v.x );
		y = ( y <= v.y ? y : v.y );
		return *this;
	}

	// Maximum Coordinates with a Vector2
	Vector2 &
	max( Vector2 const & v )
	{
		x = ( x >= v.x ? x : v.x );
		y = ( y >= v.y ? y : v.y );
		return *this;
	}

	// Subtract a Vector2
	Vector2 &
	sub( Vector2 const & v )
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

public: // Generators

	// -Vector2 (Negated)
	Vector2
	operator -() const
	{
		return Vector2( -x, -y );
	}

public: // Static Methods

	// Square of a value
	static
	T
	square( Tc t )
	{
		return t * t;
	}

public: // Data

	T x, y; // Elements

}; // Vector2

// Length
template< typename T >
inline
T
length( Vector2< T > const & v )
{
	return v.length();
}

// Magnitude
template< typename T >
inline
T
magnitude( Vector2< T > const & v )
{
	return v.magnitude();
}

// Magnitude Squared
template< typename T >
inline
T
magnitude_squared( Vector2< T > const & v )
{
	return v.magnitude_squared();
}

// Magnitude Squared
template< typename T >
inline
T
mag_squared( Vector2< T > const & v )
{
	return v.mag_squared();
}

// Vector2 == Vector2
template< typename T >
inline
bool
operator ==( Vector2< T > const & a, Vector2< T > const & b )
{
	return ( a.x == b.x ) && ( a.y == b.y );
}

// Vector2 != Vector2
template< typename T >
inline
bool
operator !=( Vector2< T > const & a, Vector2< T > const & b )
{
	return ( a.x != b.x ) || ( a.y != b.y );
}

// Vector2 < Vector2: Lexicographic
template< typename T >
inline
bool
operator <( Vector2< T > const & a, Vector2< T > const & b )
{
	return (
	 ( a.x < b.x ? true :
	 ( b.x < a.x ? false : // a.x == b.x
	 ( a.y < b.y ) ) )
	);
}

// Vector2 <= Vector2: Lexicographic
template< typename T >
inline
bool
operator <=( Vector2< T > const & a, Vector2< T > const & b )
{
	return (
	 ( a.x < b.x ? true :
	 ( b.x < a.x ? false : // a.x == b.x
	 ( a.y <= b.y ) ) )
	);
}

// Vector2 >= Vector2: Lexicographic
template< typename T >
inline
bool
operator >=( Vector2< T > const & a, Vector2< T > const & b )
{
	return (
	 ( a.x > b.x ? true :
	 ( b.x > a.x ? false : // a.x == b.x
	 ( a.y >= b.y ) ) )
	);
}

// Vector2 > Vector2: Lexicographic
template< typename T >
inline
bool
operator >( Vector2< T > const & a, Vector2< T > const & b )
{
	return (
	 ( a.x > b.x ? true :
	 ( b.x > a.x ? false : // a.x == b.x
	 ( a.y > b.y ) ) )
	);
}

// Vector2 == Value
template< typename T >
inline
bool
operator ==( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return ( v.x == t ) && ( v.y == t );
}

// Vector2 != Value
template< typename T >
inline
bool
operator !=( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return ( v.x != t ) || ( v.y != t );
}

// Vector2 < Value
template< typename T >
inline
bool
operator <( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return ( v.x < t ) && ( v.y < t );
}

// Vector2 <= Value
template< typename T >
inline
bool
operator <=( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return ( v.x <= t ) && ( v.y <= t );
}

// Vector2 >= Value
template< typename T >
inline
bool
operator >=( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return ( v.x >= t ) && ( v.y >= t );
}

// Vector2 > Value
template< typename T >
inline
bool
operator >( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return ( v.x > t ) && ( v.y > t );
}

// Value == Vector2
template< typename T >
inline
bool
operator ==( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return ( t == v.x ) && ( t == v.y );
}

// Value != Vector2
template< typename T >
inline
bool
operator !=( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return ( t != v.x ) || ( t != v.y );
}

// Value < Vector2
template< typename T >
inline
bool
operator <( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return ( t < v.x ) && ( t < v.y );
}

// Value <= Vector2
template< typename T >
inline
bool
operator <=( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return ( t <= v.x ) && ( t <= v.y );
}

// Value >= Vector2
template< typename T >
inline
bool
operator >=( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return ( t >= v.x ) && ( t >= v.y );
}

// Value > Vector2
template< typename T >
inline
bool
operator >( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return ( t > v.x ) && ( t > v.y );
}

// Vector2 + Vector2
template< typename T >
inline
Vector2< T >
operator +( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >( a.x + b.x, a.y + b.y );
}

// Vector2 + Value
template< typename T >
inline
Vector2< T >
operator +( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return Vector2< T >( v.x + t, v.y + t );
}

// Value + Vector2
template< typename T >
inline
Vector2< T >
operator +( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return Vector2< T >( t + v.x, t + v.y );
}

// Vector2 - Vector2
template< typename T >
inline
Vector2< T >
operator -( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >( a.x - b.x, a.y - b.y );
}

// Vector2 - Value
template< typename T >
inline
Vector2< T >
operator -( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return Vector2< T >( v.x - t, v.y - t );
}

// Value - Vector2
template< typename T >
inline
Vector2< T >
operator -( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return Vector2< T >( t - v.x, t - v.y );
}

// Vector2 * Vector2
template< typename T >
inline
Vector2< T >
operator *( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >( a.x * b.x, a.y * b.y );
}

// Vector2 * Value
template< typename T >
inline
Vector2< T >
operator *( Vector2< T > const & v, typename Vector2< T >::Tc t )
{
	return Vector2< T >( v.x * t, v.y * t );
}

// Value * Vector2
template< typename T >
inline
Vector2< T >
operator *( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	return Vector2< T >( t * v.x, t * v.y );
}

// Vector2 / Vector2
template< typename T >
inline
Vector2< T >
operator /( Vector2< T > const & a, Vector2< T > const & b )
{
	assert( b.x != T( 0 ) );
	assert( b.y != T( 0 ) );
	return Vector2< T >( a.x / b.x, a.y / b.y );
}

// Vector2 / Value
template< typename T, typename U, class = typename std::enable_if< std::is_floating_point< U >::value && std::is_assignable< T&, U >::value >::type >
inline
Vector2< T >
operator /( Vector2< T > const & v, U const & u )
{
	assert( u != U( 0 ) );
	U const inv_u( U ( 1 ) / u );
	return Vector2< T >( v.x * inv_u, v.y * inv_u );
}

// Vector2 / Value
template< typename T, typename U, class = typename std::enable_if< ! std::is_floating_point< U >::value && std::is_assignable< T&, U >::value >::type, typename = void >
inline
Vector2< T >
operator /( Vector2< T > const & v, U const & u )
{
	assert( u != U( 0 ) );
	return Vector2< T >( v.x / u, v.y / u );
}

// Value / Vector2
template< typename T >
inline
Vector2< T >
operator /( typename Vector2< T >::Tc t, Vector2< T > const & v )
{
	assert( v.x != T( 0 ) );
	assert( v.y != T( 0 ) );
	return Vector2< T >( t / v.x, t / v.y );
}

// Minimum of Two Vector2s
template< typename T >
inline
Vector2< T >
min( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >(
	 ( a.x <= b.x ? a.x : b.x ),
	 ( a.y <= b.y ? a.y : b.y )
	);
}

// Minimum of Three Vector2s
template< typename T >
inline
Vector2< T >
min( Vector2< T > const & a, Vector2< T > const & b, Vector2< T > const & c )
{
	return Vector2< T >(
	 ObjexxFCL::min( a.x, b.x, c.x ),
	 ObjexxFCL::min( a.y, b.y, c.y )
	);
}

// Minimum of Four Vector2s
template< typename T >
inline
Vector2< T >
min( Vector2< T > const & a, Vector2< T > const & b, Vector2< T > const & c, Vector2< T > const & d )
{
	return Vector2< T >(
	 ObjexxFCL::min( a.x, b.x, c.x, d.x ),
	 ObjexxFCL::min( a.y, b.y, c.y, d.y )
	);
}

// Maximum of Two Vector2s
template< typename T >
inline
Vector2< T >
max( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >(
	 ( a.x >= b.x ? a.x : b.x ),
	 ( a.y >= b.y ? a.y : b.y )
	);
}

// Maximum of Three Vector2s
template< typename T >
inline
Vector2< T >
max( Vector2< T > const & a, Vector2< T > const & b, Vector2< T > const & c )
{
	return Vector2< T >(
	 ObjexxFCL::max( a.x, b.x, c.x ),
	 ObjexxFCL::max( a.y, b.y, c.y )
	);
}

// Maximum of Four Vector2s
template< typename T >
inline
Vector2< T >
max( Vector2< T > const & a, Vector2< T > const & b, Vector2< T > const & c, Vector2< T > const & d )
{
	return Vector2< T >(
	 ObjexxFCL::max( a.x, b.x, c.x, d.x ),
	 ObjexxFCL::max( a.y, b.y, c.y, d.y )
	);
}

// Subtract of Two Vector2s
template< typename T >
inline
Vector2< T >
sub( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >( a.x - b.x, a.y - b.y );
}

// Midpoint of Two Vector2s
template< typename T >
inline
Vector2< T >
mid( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >(
	 T( 0.5 * ( a.x + b.x ) ),
	 T( 0.5 * ( a.y + b.y ) )
	);
}

// Center of Two Vector2s
template< typename T >
inline
Vector2< T >
cen( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >(
	 T( 0.5 * ( a.x + b.x ) ),
	 T( 0.5 * ( a.y + b.y ) )
	);
}

// Center of Three Vector2s
template< typename T >
inline
Vector2< T >
cen( Vector2< T > const & a, Vector2< T > const & b, Vector2< T > const & c )
{
	static long double const third( 1.0 / 3.0 );
	return Vector2< T >(
	 T( third * ( a.x + b.x + c.x ) ),
	 T( third * ( a.y + b.y + c.y ) )
	);
}

// Center of Four Vector2s
template< typename T >
inline
Vector2< T >
cen( Vector2< T > const & a, Vector2< T > const & b, Vector2< T > const & c, Vector2< T > const & d )
{
	return Vector2< T >(
	 T( 0.25 * ( a.x + b.x + c.x + d.x ) ),
	 T( 0.25 * ( a.y + b.y + c.y + d.y ) )
	);
}

// Distance
template< typename T >
inline
T
distance( Vector2< T > const & a, Vector2< T > const & b )
{
	return std::sqrt( Vector2< T >::square( a.x - b.x ) + Vector2< T >::square( a.y - b.y ) );
}

// Distance Squared
template< typename T >
inline
T
distance_squared( Vector2< T > const & a, Vector2< T > const & b )
{
	return Vector2< T >::square( a.x - b.x ) + Vector2< T >::square( a.y - b.y );
}

// Dot Product
template< typename T >
inline
T
dot( Vector2< T > const & a, Vector2< T > const & b )
{
	return ( a.x * b.x ) + ( a.y * b.y );
}

// Cross Product
template< typename T >
inline
T
cross( Vector2< T > const & a, Vector2< T > const & b )
{
	return ( a.x * b.y ) - ( a.y * b.x );
}

// Stream << Vector2 output operator
template< typename T >
std::ostream &
operator <<( std::ostream & stream, Vector2< T > const & v )
{
	// Types
	typedef  TypeTraits< T >  Traits;

	// Save current stream state and set persistent state
	std::ios_base::fmtflags const old_flags( stream.flags() );
	std::streamsize const old_precision( stream.precision( Traits::precision ) );
	stream << std::right << std::showpoint << std::uppercase;

	// Output Vector2
	std::size_t const w( Traits::width );
	stream << std::setw( w ) << v.x << ' ' << std::setw( w ) << v.y;

	// Restore previous stream state
	stream.precision( old_precision );
	stream.flags( old_flags );

	return stream;
}

// Stream >> Vector2 input operator
//  Supports whitespace-separated values with optional commas between values as long as whitespace is also present
//  String or char values containing whitespace or commas or enclosed in quotes are not supported
//  Vector can optionally be enclosed in parentheses () or square brackets []
template< typename T >
std::istream &
operator >>( std::istream & stream, Vector2< T > & v )
{
	bool parens( false ); // Opening ( present?
	bool brackets( false ); // Opening [ present?

	{ // x
		std::string input_string;
		stream >> input_string;
		if ( input_string == "(" ) { // Skip opening (
			stream >> input_string;
			parens = true;
		} else if ( input_string[ 0 ] == '(' ) { // Skip opening (
			input_string.erase( 0, 1 );
			brackets = true;
		} else if ( input_string == "[" ) { // Skip opening [
			stream >> input_string;
			brackets = true;
		} else if ( input_string[ 0 ] == '[' ) { // Skip opening [
			input_string.erase( 0, 1 );
			brackets = true;
		}
		std::string::size_type const input_size( input_string.size() );
		if ( ( input_size > 0 ) && ( input_string[ input_size - 1 ] == ',' ) ) {
			input_string.erase( input_size - 1 ); // Remove trailing ,
		}
		std::istringstream num_stream( input_string );
		num_stream >> v.x;
	}

	{ // y
		std::string input_string;
		stream >> input_string;
		if ( input_string == "," ) { // Skip ,
			stream >> input_string;
		} else if ( input_string[ 0 ] == ',' ) { // Skip leading ,
			input_string.erase( 0, 1 );
		}
		std::string::size_type input_size( input_string.size() );
		if ( parens || brackets ) { // Remove closing ) or ]
			if ( input_size > 0 ) {
				if ( parens ) {
					if ( input_string[ input_size - 1 ] == ')' ) { // Remove closing )
						input_string.erase( input_size - 1 );
						--input_size;
					}
				} else if ( brackets ) {
					if ( input_string[ input_size - 1 ] == ']' ) { // Remove closing ]
						input_string.erase( input_size - 1 );
						--input_size;
					}
				}
			}
		}
		if ( ( input_size > 0 ) && ( input_string[ input_size - 1 ] == ',' ) ) {
			input_string.erase( input_size - 1 ); // Remove trailing ,
		}
		std::istringstream num_stream( input_string );
		num_stream >> v.y;
	}

	// Remove closing ) or ] if opening ( or [ present
	if ( parens || brackets ) { // Remove closing ) or ]
		while ( ( stream.peek() == ' ' ) || ( stream.peek() == '\t' ) ) {
			stream.ignore();
		}
		if ( parens ) { // Remove closing ) if present
			if ( stream.peek() == ')' ) stream.ignore();
		} else if ( brackets ) { // Remove closing ] if present
			if ( stream.peek() == ']' ) stream.ignore();
		}
	}

	return stream;
}

} // ObjexxFCL

#endif // ObjexxFCL_Vector2_hh_INCLUDED
