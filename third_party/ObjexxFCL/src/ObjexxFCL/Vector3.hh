#ifndef ObjexxFCL_Vector3_hh_INCLUDED
#define ObjexxFCL_Vector3_hh_INCLUDED

// Vector3: Fast 3-Element Vector
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
#include <ObjexxFCL/Vector3.fwd.hh>
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

// Vector3: Fast 3-Element Vector
// . Heap-free and loop-free for speed
// . Provides direct element access via .x style lookup
// . Use std::array< double, 3 > instead in array/vectorization context
class Vector3
{

public: // Types

	typedef  TypeTraits< double >  Traits;

	// STL Style
	typedef  double  value_type;
	typedef  double &  reference;
	typedef  double const &  const_reference;
	typedef  double *  pointer;
	typedef  double const *  const_pointer;
	typedef  std::size_t  size_type;
	typedef  std::ptrdiff_t  difference_type;

	// C++ Style
	typedef  double  Value;
	typedef  double &  Reference;
	typedef  double const &  ConstReference;
	typedef  double *  Pointer;
	typedef  double const *  ConstPointer;
	typedef  std::size_t  Size;
	typedef  std::ptrdiff_t  Difference;

public: // Creation

	// Default Constructor
	Vector3()
#if defined(OBJEXXFCL_ARRAY_INIT) || defined(OBJEXXFCL_ARRAY_INIT_DEBUG)
	 :
	 x( Traits::initial_array_value() ),
	 y( Traits::initial_array_value() ),
	 z( Traits::initial_array_value() )
#endif
	{}

	// Copy Constructor
	Vector3( Vector3 const & v ) :
	 x( v.x ),
	 y( v.y ),
	 z( v.z )
	{}

	// Uniform Value Constructor
	explicit
	Vector3( double t ) :
	 x( t ),
	 y( t ),
	 z( t )
	{}

	// Value Constructor
	Vector3(
	 double x_,
	 double y_,
	 double z_
	) :
	 x( x_ ),
	 y( y_ ),
	 z( z_ )
	{}

	// Initializer List Constructor Template
	template< typename U, class = typename std::enable_if< std::is_constructible< double, U >::value >::type >
	Vector3( std::initializer_list< U > const l ) :
	 x( *l.begin() ),
	 y( *( l.begin() + 1 ) ),
	 z( *( l.begin() + 2 ) )
	{
		assert( l.size() == 3 );
	}

	// Array Constructor Template
	template< typename A, class = typename std::enable_if< std::is_constructible< double, typename A::value_type >::value >::type >
	Vector3( A const & a ) :
	 x( a[ 0 ] ),
	 y( a[ 1 ] ),
	 z( a[ 2 ] )
	{
		assert( a.size() == 3 );
	}

	// Default Vector Named Constructor
	static
	Vector3
	default_vector()
	{
		return Vector3( double() );
	}

	// Zero Vector Named Constructor
	static
	Vector3
	zero_vector()
	{
		return Vector3( double( 0 ) );
	}

	// x Vector of Specified Length Named Constructor
	static
	Vector3
	x_vector( double tar_length = double( 1 ) )
	{
		return Vector3( tar_length, double( 0 ), double( 0 ) );
	}

	// y Vector of Specified Length Named Constructor
	static
	Vector3
	y_vector( double tar_length = double( 1 ) )
	{
		return Vector3( double( 0 ), tar_length, double( 0 ) );
	}

	// z Vector of Specified Length Named Constructor
	static
	Vector3
	z_vector( double tar_length = double( 1 ) )
	{
		return Vector3( double( 0 ), double( 0 ), tar_length );
	}

	// Uniform Vector of Specified Length Named Constructor
	static
	Vector3
	uniform_vector( double tar_length = double( 1 ) )
	{
		return Vector3( tar_length / std::sqrt( double( 3 ) ) );
	}

	// Destructor
	~Vector3()
	{}

public: // Assignment

	// Copy Assignment
	Vector3 &
	operator =( Vector3 const & v )
	{
		if ( this != &v ) {
			x = v.x;
			y = v.y;
			z = v.z;
		}
		return *this;
	}

	// Initializer List Assignment Template
	template< typename U, class = typename std::enable_if< std::is_assignable< double&, U >::value >::type >
	Vector3 &
	operator =( std::initializer_list< U > const l )
	{
		assert( l.size() == 3 );
		auto i( l.begin() );
		x = *i;
		y = *(++i);
		z = *(++i);
		return *this;
	}

	// Array Assignment Template
	template< typename A, class = typename std::enable_if< std::is_assignable< double&, typename A::value_type >::value >::type >
	Vector3 &
	operator =( A const & a )
	{
		assert( a.size() == 3 );
		x = a[ 0 ];
		y = a[ 1 ];
		z = a[ 2 ];
		return *this;
	}

	// += Vector3
	Vector3 &
	operator +=( Vector3 const & v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	// -= Vector3
	Vector3 &
	operator -=( Vector3 const & v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	// *= Vector3
	Vector3 &
	operator *=( Vector3 const & v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	// /= Vector3
	Vector3 &
	operator /=( Vector3 const & v )
	{
		assert( v.x != double( 0 ) );
		assert( v.y != double( 0 ) );
		assert( v.z != double( 0 ) );
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	// += Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< double&, U >::value >::type >
	Vector3 &
	operator +=( std::initializer_list< U > const l )
	{
		assert( l.size() == 3 );
		auto i( l.begin() );
		x += *i;
		y += *(++i);
		z += *(++i);
		return *this;
	}

	// -= Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< double&, U >::value >::type >
	Vector3 &
	operator -=( std::initializer_list< U > const l )
	{
		assert( l.size() == 3 );
		auto i( l.begin() );
		x -= *i;
		y -= *(++i);
		z -= *(++i);
		return *this;
	}

	// *= Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< double&, U >::value >::type >
	Vector3 &
	operator *=( std::initializer_list< U > const l )
	{
		assert( l.size() == 3 );
		auto i( l.begin() );
		x *= *i;
		y *= *(++i);
		z *= *(++i);
		return *this;
	}

	// /= Initializer List
	template< typename U, class = typename std::enable_if< std::is_assignable< double&, U >::value >::type >
	Vector3 &
	operator /=( std::initializer_list< U > const l )
	{
		assert( l.size() == 3 );
		auto i( l.begin() );
		assert( *i != double( 0 ) );
		assert( *(i+1) != double( 0 ) );
		assert( *(i+2) != double( 0 ) );
		x /= *i;
		y /= *(++i);
		z /= *(++i);
		return *this;
	}

	// += Array
	template< typename A, class = typename std::enable_if< std::is_assignable< double&, typename A::value_type >::value >::type >
	Vector3 &
	operator +=( A const & a )
	{
		assert( a.size() == 3 );
		x += a[ 0 ];
		y += a[ 1 ];
		z += a[ 2 ];
		return *this;
	}

	// -= Array
	template< typename A, class = typename std::enable_if< std::is_assignable< double&, typename A::value_type >::value >::type >
	Vector3 &
	operator -=( A const & a )
	{
		assert( a.size() == 3 );
		x -= a[ 0 ];
		y -= a[ 1 ];
		z -= a[ 2 ];
		return *this;
	}

	// *= Array
	template< typename A, class = typename std::enable_if< std::is_assignable< double&, typename A::value_type >::value >::type >
	Vector3 &
	operator *=( A const & a )
	{
		assert( a.size() == 3 );
		x *= a[ 0 ];
		y *= a[ 1 ];
		z *= a[ 2 ];
		return *this;
	}

	// /= Array
	template< typename A, class = typename std::enable_if< std::is_assignable< double&, typename A::value_type >::value >::type >
	Vector3 &
	operator /=( A const & a )
	{
		assert( a.size() == 3 );
		assert( a[ 0 ] != double( 0 ) );
		assert( a[ 1 ] != double( 0 ) );
		assert( a[ 2 ] != double( 0 ) );
		x /= a[ 0 ];
		y /= a[ 1 ];
		z /= a[ 2 ];
		return *this;
	}

	// = Value
	Vector3 &
	operator =( double t )
	{
		x = y = z = t;
		return *this;
	}

	// += Value
	Vector3 &
	operator +=( double t )
	{
		x += t;
		y += t;
		z += t;
		return *this;
	}

	// -= Value
	Vector3 &
	operator -=( double t )
	{
		x -= t;
		y -= t;
		z -= t;
		return *this;
	}

	// *= Value
	Vector3 &
	operator *=( double t )
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	// /= Value
	Vector3 &
	operator /=( double const u )
	{
		assert( u != 0.0 );
		double const inv_u( 1.0 / u );
		x *= inv_u;
		y *= inv_u;
		z *= inv_u;
		return *this;
	}

	// Value Assignment
	Vector3 &
	assign(
	 double x_,
	 double y_,
	 double z_
	)
	{
		x = x_;
		y = y_;
		z = z_;
		return *this;
	}

public: // Assignment: Scaled

	// Assign Value * Vector3
	Vector3 &
	scaled_assign( double t, Vector3 const & v )
	{
		x = t * v.x;
		y = t * v.y;
		z = t * v.z;
		return *this;
	}

	// Add Value * Vector3
	Vector3 &
	scaled_add( double t, Vector3 const & v )
	{
		x += t * v.x;
		y += t * v.y;
		z += t * v.z;
		return *this;
	}

	// Subtract Value * Vector3
	Vector3 &
	scaled_sub( double t, Vector3 const & v )
	{
		x -= t * v.x;
		y -= t * v.y;
		z -= t * v.z;
		return *this;
	}

	// Multiply by Value * Vector3
	Vector3 &
	scaled_mul( double t, Vector3 const & v )
	{
		x *= t * v.x;
		y *= t * v.y;
		z *= t * v.z;
		return *this;
	}

	// Divide by Value * Vector3
	Vector3 &
	scaled_div( double t, Vector3 const & v )
	{
		assert( t != double( 0 ) );
		assert( v.x != double( 0 ) );
		assert( v.y != double( 0 ) );
		assert( v.z != double( 0 ) );
		x /= t * v.x;
		y /= t * v.y;
		z /= t * v.z;
		return *this;
	}

public: // Subscript

	// Vector3[ i ] const: 0-Based Index
	double
	operator []( size_type const i ) const
	{
		assert( i <= 2 );
		return ( i == 0 ? x : ( i == 1 ? y : z ) );
	}

	// Vector3[ i ]: 0-Based Index
	double &
	operator []( size_type const i )
	{
		assert( i <= 2 );
		return ( i == 0 ? x : ( i == 1 ? y : z ) );
	}

	// Vector3( i ) const: 1-Based Index
	double
	operator ()( size_type const i ) const
	{
		assert( ( 1 <= i ) && ( i <= 3 ) );
		return ( i == 1 ? x : ( i == 2 ? y : z ) );
	}

	// Vector3( i ): 1-Based Index
	double &
	operator ()( size_type const i )
	{
		assert( ( 1 <= i ) && ( i <= 3 ) );
		return ( i == 1 ? x : ( i == 2 ? y : z ) );
	}

public: // Properties: Predicates

	// Is Zero Vector?
	bool
	is_zero() const
	{
		static double const ZERO( 0 );
		return ( x == ZERO ) && ( y == ZERO ) && ( z == ZERO );
	}

	// Is Unit Vector?
	bool
	is_unit() const
	{
		return ( length_squared() == double( 1 ) );
	}

public: // Properties: General

	// Size
	Size
	size() const
	{
		return 3u;
	}

	// Length
	double
	length() const
	{
		return std::sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
	}

	// Length Squared
	double
	length_squared() const
	{
		return ( x * x ) + ( y * y ) + ( z * z );
	}

	// Magnitude
	double
	magnitude() const
	{
		return std::sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
	}

	// Magnitude
	double
	mag() const
	{
		return std::sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
	}

	// Magnitude Squared
	double
	magnitude_squared() const
	{
		return ( x * x ) + ( y * y ) + ( z * z );
	}

	// Magnitude Squared
	double
	mag_squared() const
	{
		return ( x * x ) + ( y * y ) + ( z * z );
	}

	// L1 Norm
	double
	norm_L1() const
	{
		return std::abs( x ) + std::abs( y ) + std::abs( z );
	}

	// L2 Norm
	double
	norm_L2() const
	{
		return std::sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
	}

	// L-infinity Norm
	double
	norm_Linf() const
	{
		return ObjexxFCL::max( std::abs( x ), std::abs( y ), std::abs( z ) );
	}

	// Distance to a Vector3
	double
	distance( Vector3 const & v ) const
	{
		return std::sqrt( square( x - v.x ) + square( y - v.y ) + square( z - v.z ) );
	}

	// Distance Squared to a Vector3
	double
	distance_squared( Vector3 const & v ) const
	{
		return square( x - v.x ) + square( y - v.y ) + square( z - v.z );
	}

	// Dot Product with a Vector3
	double
	dot( Vector3 const & v ) const
	{
		return ( x * v.x ) + ( y * v.y ) + ( z * v.z );
	}

	// Dot Product with an Array
	template< typename A, class = typename std::enable_if< std::is_assignable< double&, typename A::value_type >::value >::type >
	double
	dot( A const & a ) const
	{
		assert( a.size() == 3 );
		return ( x * a[ 0 ] ) + ( y * a[ 1 ] ) + ( z * a[ 2 ] );
	}

	// Cross Product with a Vector3
	Vector3
	cross( Vector3 const & v ) const
	{
		return Vector3(
		 ( y * v.z ) - ( z * v.y ),
		 ( z * v.x ) - ( x * v.z ),
		 ( x * v.y ) - ( y * v.x )
		);
	}

	// Cross Product with an Array
	template< typename A, class = typename std::enable_if< std::is_assignable< double&, typename A::value_type >::value >::type >
	Vector3
	cross( A const & a ) const
	{
		assert( a.size() == 3 );
		return Vector3(
		 ( y * a[ 2 ] ) - ( z * a[ 1 ] ),
		 ( z * a[ 0 ] ) - ( x * a[ 2 ] ),
		 ( x * a[ 1 ] ) - ( y * a[ 0 ] )
		);
	}

	// Alias for Element 1
	double
	x1() const
	{
		return x;
	}

	// Alias for Element 1
	double &
	x1()
	{
		return x;
	}

	// Alias for Element 2
	double
	x2() const
	{
		return y;
	}

	// Alias for Element 2
	double &
	x2()
	{
		return y;
	}

	// Alias for Element 3
	double
	x3() const
	{
		return z;
	}

	// Alias for Element 3
	double &
	x3()
	{
		return z;
	}

public: // Modifiers

	// Zero
	Vector3 &
	zero()
	{
		x = y = z = double( 0 );
		return *this;
	}

	// Negate
	Vector3 &
	negate()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	// Normalize to a Length
	Vector3 &
	normalize( double tar_length = double( 1 ) )
	{
		double const cur_length( length() );
		assert( cur_length != double ( 0 ) );
		double const dilation( tar_length / cur_length );
		x *= dilation;
		y *= dilation;
		z *= dilation;
		return *this;
	}

	// Normalize to a Length: Zero Vector3 if Length is Zero
	Vector3 &
	normalize_zero( double tar_length = double( 1 ) )
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			x *= dilation;
			y *= dilation;
			z *= dilation;
		} else { // Set zero vector
			x = y = z = double( 0 );
		}
		return *this;
	}

	// Normalize to a Length: Uniform Vector3 if Length is Zero
	Vector3 &
	normalize_uniform( double tar_length = double( 1 ) )
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			x *= dilation;
			y *= dilation;
			z *= dilation;
		} else { // Set uniform vector
			operator =( uniform_vector( tar_length ) );
		}
		return *this;
	}

	// Normalize to a Length: x Vector3 if Length is Zero
	Vector3 &
	normalize_x( double tar_length = double( 1 ) )
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			x *= dilation;
			y *= dilation;
			z *= dilation;
		} else { // Set x vector
			x = tar_length;
			y = z = double( 0 );
		}
		return *this;
	}

	// Normalize to a Length: y Vector3 if Length is Zero
	Vector3 &
	normalize_y( double tar_length = double( 1 ) )
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			x *= dilation;
			y *= dilation;
			z *= dilation;
		} else { // Set y vector
			y = tar_length;
			x = z = double( 0 );
		}
		return *this;
	}

	// Normalize to a Length: z Vector3 if Length is Zero
	Vector3 &
	normalize_z( double tar_length = double( 1 ) )
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			x *= dilation;
			y *= dilation;
			z *= dilation;
		} else { // Set z vector
			z = tar_length;
			x = y = double( 0 );
		}
		return *this;
	}

	// Minimum Coordinates with a Vector3
	Vector3 &
	min( Vector3 const & v )
	{
		x = ( x <= v.x ? x : v.x );
		y = ( y <= v.y ? y : v.y );
		z = ( z <= v.z ? z : v.z );
		return *this;
	}

	// Maximum Coordinates with a Vector3
	Vector3 &
	max( Vector3 const & v )
	{
		x = ( x >= v.x ? x : v.x );
		y = ( y >= v.y ? y : v.y );
		z = ( z >= v.z ? z : v.z );
		return *this;
	}

	// Add a Vector3
	Vector3 &
	add( Vector3 const & v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	// Sum a Vector3
	Vector3 &
	sum( Vector3 const & v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	// Subtract a Vector3
	Vector3 &
	sub( Vector3 const & v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	// Subtract a Vector3
	Vector3 &
	subtract( Vector3 const & v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	// Project Normal to a Vector3
	Vector3 &
	project_normal( Vector3 const & v )
	{
		assert( v.length_squared() != double( 0 ) );
		double const c( dot( v ) / v.length_squared() );
		x -= c * v.x;
		y -= c * v.y;
		z -= c * v.z;
		return *this;
	}

	// Project onto a Vector3
	Vector3 &
	project_parallel( Vector3 const & v )
	{
		assert( v.length_squared() != double( 0 ) );
		double const c( dot( v ) / v.length_squared() );
		x = c * v.x;
		y = c * v.y;
		z = c * v.z;
		return *this;
	}

public: // Generators

	// -Vector3 (Negated)
	Vector3
	operator -() const
	{
		return Vector3( -x, -y, -z );
	}

	// Negated
	Vector3
	negated() const
	{
		return Vector3( -x, -y, -z );
	}

	// Normalized to a Length
	Vector3
	normalized( double tar_length = double( 1 ) ) const
	{
		double const cur_length( length() );
		assert( cur_length != double ( 0 ) );
		double const dilation( tar_length / cur_length );
		return Vector3(
		 x * dilation,
		 y * dilation,
		 z * dilation
		);
	}

	// Normalized to a Length: Zero Vector3 if Length is Zero
	Vector3
	normalized_zero( double tar_length = double( 1 ) ) const
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			return Vector3(
			 x * dilation,
			 y * dilation,
			 z * dilation
			);
		} else { // Return zero vector
			return Vector3( double( 0 ) );
		}
	}

	// Normalized to a Length: Uniform Vector3 if Length is Zero
	Vector3
	normalized_uniform( double tar_length = double( 1 ) ) const
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			return Vector3(
			 x * dilation,
			 y * dilation,
			 z * dilation
			);
		} else { // Return uniform vector
			return uniform_vector( tar_length );
		}
	}

	// Normalized to a Length: x Vector3 if Length is Zero
	Vector3
	normalized_x( double tar_length = double( 1 ) ) const
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			return Vector3(
			 x * dilation,
			 y * dilation,
			 z * dilation
			);
		} else { // Return x vector
			return Vector3( tar_length, double( 0 ), double( 0 ) );
		}
	}

	// Normalized to a Length: y Vector3 if Length is Zero
	Vector3
	normalized_y( double tar_length = double( 1 ) ) const
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			return Vector3(
			 x * dilation,
			 y * dilation,
			 z * dilation
			);
		} else { // Return y vector
			return Vector3( double( 0 ), tar_length, double( 0 ) );
		}
	}

	// Normalized to a Length: z Vector3 if Length is Zero
	Vector3
	normalized_z( double tar_length = double( 1 ) ) const
	{
		double const cur_length( length() );
		if ( cur_length > double( 0 ) ) {
			double const dilation( tar_length / cur_length );
			return Vector3(
			 x * dilation,
			 y * dilation,
			 z * dilation
			);
		} else { // Return z vector
			return Vector3( double( 0 ), double( 0 ), tar_length );
		}
	}

	// Projected Normal to a Vector3
	Vector3
	projected_normal( Vector3 const & v ) const
	{
		assert( v.length_squared() != double( 0 ) );
		double const c( dot( v ) / v.length_squared() );
		return Vector3( x - ( c * v.x ), y - ( c * v.y ), z - ( c * v.z ) );
	}

	// Projected onto a Vector3
	Vector3
	projected_parallel( Vector3 const & v ) const
	{
		assert( v.length_squared() != double( 0 ) );
		double const c( dot( v ) / v.length_squared() );
		return Vector3( c * v.x, c * v.y, c * v.z );
	}

public: // Static Methods

	// Square of a value
	static
	double
	square( double t )
	{
		return t * t;
	}

	// Value Clipped to [-1,1]
	static
	double
	sin_cos_range( double t )
	{
		return std::min( std::max( t, double( -1 ) ), double( 1 ) );
	}

	// Add 2*Pi to a Negative Value
	static
	double
	bump_up_angle( double t )
	{
		static double const Two_Pi( double( 2 ) * std::acos( -1.0 ) );
		return ( t >= double( 0 ) ? t : Two_Pi + t );
	}

public: // Data

	double x, y, z; // Elements

}; // Vector3

// Length
inline
double
length( Vector3 const & v )
{
	return v.length();
}

// Length Squared
inline
double
length_squared( Vector3 const & v )
{
	return v.length_squared();
}

// Magnitude
inline
double
magnitude( Vector3 const & v )
{
	return v.magnitude();
}

// Magnitude
inline
double
mag( Vector3 const & v )
{
	return v.mag();
}

// Magnitude Squared
inline
double
magnitude_squared( Vector3 const & v )
{
	return v.magnitude_squared();
}

// Magnitude Squared
inline
double
mag_squared( Vector3 const & v )
{
	return v.mag_squared();
}

// Vector3 == Vector3
inline
bool
operator ==( Vector3 const & a, Vector3 const & b )
{
	return ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z );
}

// Vector3 != Vector3
inline
bool
operator !=( Vector3 const & a, Vector3 const & b )
{
	return ( a.x != b.x ) || ( a.y != b.y ) || ( a.z != b.z );
}

// Vector3 < Vector3: Lexicographic
inline
bool
operator <( Vector3 const & a, Vector3 const & b )
{
	return (
	 ( a.x < b.x ? true :
	 ( b.x < a.x ? false : // a.x == b.x
	 ( a.y < b.y ? true :
	 ( b.y < a.y ? false : // a.y == b.y
	 ( a.z < b.z ) ) ) ) )
	);
}

// Vector3 <= Vector3: Lexicographic
inline
bool
operator <=( Vector3 const & a, Vector3 const & b )
{
	return (
	 ( a.x < b.x ? true :
	 ( b.x < a.x ? false : // a.x == b.x
	 ( a.y < b.y ? true :
	 ( b.y < a.y ? false : // a.y == b.y
	 ( a.z <= b.z ) ) ) ) )
	);
}

// Vector3 >= Vector3: Lexicographic
inline
bool
operator >=( Vector3 const & a, Vector3 const & b )
{
	return (
	 ( a.x > b.x ? true :
	 ( b.x > a.x ? false : // a.x == b.x
	 ( a.y > b.y ? true :
	 ( b.y > a.y ? false : // a.y == b.y
	 ( a.z >= b.z ) ) ) ) )
	);
}

// Vector3 > Vector3: Lexicographic
inline
bool
operator >( Vector3 const & a, Vector3 const & b )
{
	return (
	 ( a.x > b.x ? true :
	 ( b.x > a.x ? false : // a.x == b.x
	 ( a.y > b.y ? true :
	 ( b.y > a.y ? false : // a.y == b.y
	 ( a.z > b.z ) ) ) ) )
	);
}

// Vector3 < Vector3: Element-wise
inline
bool
lt( Vector3 const & a, Vector3 const & b )
{
	return ( a.x < b.x ) && ( a.y < b.y ) && ( a.z < b.z );
}

// Vector3 <= Vector3: Element-wise
inline
bool
le( Vector3 const & a, Vector3 const & b )
{
	return ( a.x <= b.x ) && ( a.y <= b.y ) && ( a.z <= b.z );
}

// Vector3 >= Vector3: Element-wise
inline
bool
ge( Vector3 const & a, Vector3 const & b )
{
	return ( a.x >= b.x ) && ( a.y >= b.y ) && ( a.z >= b.z );
}

// Vector3 > Vector3: Element-wise
inline
bool
gt( Vector3 const & a, Vector3 const & b )
{
	return ( a.x > b.x ) && ( a.y > b.y ) && ( a.z > b.z );
}

// Vector3 == Value
inline
bool
operator ==( Vector3 const & v, double t )
{
	return ( v.x == t ) && ( v.y == t ) && ( v.z == t );
}

// Vector3 != Value
inline
bool
operator !=( Vector3 const & v, double t )
{
	return ( v.x != t ) || ( v.y != t ) || ( v.z != t );
}

// Vector3 < Value
inline
bool
operator <( Vector3 const & v, double t )
{
	return ( v.x < t ) && ( v.y < t ) && ( v.z < t );
}

// Vector3 <= Value
inline
bool
operator <=( Vector3 const & v, double t )
{
	return ( v.x <= t ) && ( v.y <= t ) && ( v.z <= t );
}

// Vector3 >= Value
inline
bool
operator >=( Vector3 const & v, double t )
{
	return ( v.x >= t ) && ( v.y >= t ) && ( v.z >= t );
}

// Vector3 > Value
inline
bool
operator >( Vector3 const & v, double t )
{
	return ( v.x > t ) && ( v.y > t ) && ( v.z > t );
}

// Value == Vector3
inline
bool
operator ==( double t, Vector3 const & v )
{
	return ( t == v.x ) && ( t == v.y ) && ( t == v.z );
}

// Value != Vector3
inline
bool
operator !=( double t, Vector3 const & v )
{
	return ( t != v.x ) || ( t != v.y ) || ( t != v.z );
}

// Value < Vector3
inline
bool
operator <( double t, Vector3 const & v )
{
	return ( t < v.x ) && ( t < v.y ) && ( t < v.z );
}

// Value <= Vector3
inline
bool
operator <=( double t, Vector3 const & v )
{
	return ( t <= v.x ) && ( t <= v.y ) && ( t <= v.z );
}

// Value >= Vector3
inline
bool
operator >=( double t, Vector3 const & v )
{
	return ( t >= v.x ) && ( t >= v.y ) && ( t >= v.z );
}

// Value > Vector3
inline
bool
operator >( double t, Vector3 const & v )
{
	return ( t > v.x ) && ( t > v.y ) && ( t > v.z );
}

// Equal Length?
inline
bool
equal_length( Vector3 const & a, Vector3 const & b )
{
	return ( a.length_squared() == b.length_squared() );
}

// Not Equal Length?
inline
bool
not_equal_length( Vector3 const & a, Vector3 const & b )
{
	return ( a.length_squared() != b.length_squared() );
}

// Vector3 + Vector3
inline
Vector3
operator +( Vector3 const & a, Vector3 const & b )
{
	return Vector3( a.x + b.x, a.y + b.y, a.z + b.z );
}

// Vector3 + Value
inline
Vector3
operator +( Vector3 const & v, double t )
{
	return Vector3( v.x + t, v.y + t, v.z + t );
}

// Value + Vector3
inline
Vector3
operator +( double t, Vector3 const & v )
{
	return Vector3( t + v.x, t + v.y, t + v.z );
}

// Vector3 - Vector3
inline
Vector3
operator -( Vector3 const & a, Vector3 const & b )
{
	return Vector3( a.x - b.x, a.y - b.y, a.z - b.z );
}

// Vector3 - Value
inline
Vector3
operator -( Vector3 const & v, double t )
{
	return Vector3( v.x - t, v.y - t, v.z - t );
}

// Value - Vector3
inline
Vector3
operator -( double t, Vector3 const & v )
{
	return Vector3( t - v.x, t - v.y, t - v.z );
}

// Vector3 * Vector3
inline
Vector3
operator *( Vector3 const & a, Vector3 const & b )
{
	return Vector3( a.x * b.x, a.y * b.y, a.z * b.z );
}

// Vector3 * Value
inline
Vector3
operator *( Vector3 const & v, double t )
{
	return Vector3( v.x * t, v.y * t, v.z * t );
}

// Value * Vector3
inline
Vector3
operator *( double t, Vector3 const & v )
{
	return Vector3( t * v.x, t * v.y, t * v.z );
}

// Vector3 / Vector3
inline
Vector3
operator /( Vector3 const & a, Vector3 const & b )
{
	assert( b.x != double( 0 ) );
	assert( b.y != double( 0 ) );
	assert( b.z != double( 0 ) );
	return Vector3( a.x / b.x, a.y / b.y, a.z / b.z );
}

// Vector3 / Value
inline
Vector3
operator /( Vector3 const & v, double const u )
{
	assert( u != 0.0 );
	double const inv_u( 1.0 / u );
	return Vector3( v.x * inv_u, v.y * inv_u, v.z * inv_u );
}

// Value / Vector3
inline
Vector3
operator /( double t, Vector3 const & v )
{
	assert( v.x != double( 0 ) );
	assert( v.y != double( 0 ) );
	assert( v.z != double( 0 ) );
	return Vector3( t / v.x, t / v.y, t / v.z );
}

// Minimum of Two Vector3s
inline
Vector3
min( Vector3 const & a, Vector3 const & b )
{
	return Vector3(
	 ( a.x <= b.x ? a.x : b.x ),
	 ( a.y <= b.y ? a.y : b.y ),
	 ( a.z <= b.z ? a.z : b.z )
	);
}

// Minimum of Three Vector3s
inline
Vector3
min( Vector3 const & a, Vector3 const & b, Vector3 const & c )
{
	return Vector3(
	 ObjexxFCL::min( a.x, b.x, c.x ),
	 ObjexxFCL::min( a.y, b.y, c.y ),
	 ObjexxFCL::min( a.z, b.z, c.z )
	);
}

// Minimum of Four Vector3s
inline
Vector3
min( Vector3 const & a, Vector3 const & b, Vector3 const & c, Vector3 const & d )
{
	return Vector3(
	 ObjexxFCL::min( a.x, b.x, c.x, d.x ),
	 ObjexxFCL::min( a.y, b.y, c.y, d.y ),
	 ObjexxFCL::min( a.z, b.z, c.z, d.z )
	);
}

// Maximum of Two Vector3s
inline
Vector3
max( Vector3 const & a, Vector3 const & b )
{
	return Vector3(
	 ( a.x >= b.x ? a.x : b.x ),
	 ( a.y >= b.y ? a.y : b.y ),
	 ( a.z >= b.z ? a.z : b.z )
	);
}

// Maximum of Three Vector3s
inline
Vector3
max( Vector3 const & a, Vector3 const & b, Vector3 const & c )
{
	return Vector3(
	 ObjexxFCL::max( a.x, b.x, c.x ),
	 ObjexxFCL::max( a.y, b.y, c.y ),
	 ObjexxFCL::max( a.z, b.z, c.z )
	);
}

// Maximum of Four Vector3s
inline
Vector3
max( Vector3 const & a, Vector3 const & b, Vector3 const & c, Vector3 const & d )
{
	return Vector3(
	 ObjexxFCL::max( a.x, b.x, c.x, d.x ),
	 ObjexxFCL::max( a.y, b.y, c.y, d.y ),
	 ObjexxFCL::max( a.z, b.z, c.z, d.z )
	);
}

// Sum of Two Vector3s
inline
Vector3
sum( Vector3 const & a, Vector3 const & b )
{
	return Vector3( a.x + b.x, a.y + b.y, a.z + b.z );
}

// Sum of Three Vector3s
inline
Vector3
sum( Vector3 const & a, Vector3 const & b, Vector3 const & c )
{
	return Vector3( a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z );
}

// Sum of Four Vector3s
inline
Vector3
sum( Vector3 const & a, Vector3 const & b, Vector3 const & c, Vector3 const & d )
{
	return Vector3( a.x + b.x + c.x + d.x, a.y + b.y + c.y + d.y, a.z + b.z + c.z + d.z );
}

// Subtract of Two Vector3s
inline
Vector3
sub( Vector3 const & a, Vector3 const & b )
{
	return Vector3( a.x - b.x, a.y - b.y, a.z - b.z );
}

// Subtract of Two Vector3s
inline
Vector3
subtract( Vector3 const & a, Vector3 const & b )
{
	return Vector3( a.x - b.x, a.y - b.y, a.z - b.z );
}

// Midpoint of Two Vector3s
inline
Vector3
mid( Vector3 const & a, Vector3 const & b )
{
	return Vector3(
	 double( 0.5 * ( a.x + b.x ) ),
	 double( 0.5 * ( a.y + b.y ) ),
	 double( 0.5 * ( a.z + b.z ) )
	);
}

// Center of Two Vector3s
inline
Vector3
cen( Vector3 const & a, Vector3 const & b )
{
	return Vector3(
	 double( 0.5 * ( a.x + b.x ) ),
	 double( 0.5 * ( a.y + b.y ) ),
	 double( 0.5 * ( a.z + b.z ) )
	);
}

// Center of Three Vector3s
inline
Vector3
cen( Vector3 const & a, Vector3 const & b, Vector3 const & c )
{
	static long double const third( 1.0 / 3.0 );
	return Vector3(
	 double( third * ( a.x + b.x + c.x ) ),
	 double( third * ( a.y + b.y + c.y ) ),
	 double( third * ( a.z + b.z + c.z ) )
	);
}

// Center of Four Vector3s
inline
Vector3
cen( Vector3 const & a, Vector3 const & b, Vector3 const & c, Vector3 const & d )
{
	return Vector3(
	 double( 0.25 * ( a.x + b.x + c.x + d.x ) ),
	 double( 0.25 * ( a.y + b.y + c.y + d.y ) ),
	 double( 0.25 * ( a.z + b.z + c.z + d.z ) )
	);
}

// Distance Squared
inline
double
distance_squared( Vector3 const & a, Vector3 const & b )
{
	return Vector3::square( a.x - b.x ) + Vector3::square( a.y - b.y ) + Vector3::square( a.z - b.z );
}

// Dot Product
inline
double
dot( Vector3 const & a, Vector3 const & b )
{
	return ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z );
}

// Cross Product
inline
Vector3
cross( Vector3 const & a, Vector3 const & b )
{
	return Vector3(
	 ( a.y * b.z ) - ( a.z * b.y ),
	 ( a.z * b.x ) - ( a.x * b.z ),
	 ( a.x * b.y ) - ( a.y * b.x )
	);
}

// Angle Between Two Vector3s (in Radians on [0,pi])
inline
double
angle( Vector3 const & a, Vector3 const & b )
{
	double const axb( a.cross( b ).magnitude() );
	double const adb( a.dot( b ) );
	return ( ( axb != double( 0 ) ) || ( adb != double( 0 ) ) ? Vector3::bump_up_angle( std::atan2( axb, adb ) ) : double( 0 ) ); // More accurate than dot-based for angles near 0 and Pi
}

// Angle abc Formed by Three Vector3s (in Radians on [0,pi])
inline
double
angle( Vector3 const & a, Vector3 const & b, Vector3 const & c )
{
	return angle( a - b, c - b );
}

// Cosine of Angle Between Two Vector3s
inline
double
cos( Vector3 const & a, Vector3 const & b )
{
	double const mag( std::sqrt( a.length_squared() * b.length_squared() ) );
	return ( mag > double( 0 ) ? Vector3::sin_cos_range( a.dot( b ) / mag ) : double( 1 ) );
}

// Cosine of Angle abc Formed by Three Vector3s
inline
double
cos( Vector3 const & a, Vector3 const & b, Vector3 const & c )
{
	return cos( a - b, c - b );
}

// Sine of Angle Between Two Vector3s
inline
double
sin( Vector3 const & a, Vector3 const & b )
{
	double const mag( std::sqrt( a.length_squared() * b.length_squared() ) );
	return ( mag > double( 0 ) ? std::abs( Vector3::sin_cos_range( a.cross( b ).magnitude() / mag ) ) : double( 0 ) );
}

// Sine of Angle abc Formed by Three Vector3s
inline
double
sin( Vector3 const & a, Vector3 const & b, Vector3 const & c )
{
	return sin( a - b, c - b );
}

// Stream << Vector3 output operator
inline
std::ostream &
operator <<( std::ostream & stream, Vector3 const & v )
{
	// Types
	typedef  TypeTraits< double >  Traits;

	// Save current stream state and set persistent state
	std::ios_base::fmtflags const old_flags( stream.flags() );
	std::streamsize const old_precision( stream.precision( Traits::precision ) );
	stream << std::right << std::showpoint << std::uppercase;

	// Output Vector3
	std::size_t const w( Traits::width );
	stream << std::setw( w ) << v.x << ' ' << std::setw( w ) << v.y << ' ' << std::setw( w ) << v.z;

	// Restore previous stream state
	stream.precision( old_precision );
	stream.flags( old_flags );

	return stream;
}

// Stream >> Vector3 input operator
//  Supports whitespace-separated values with optional commas between values as long as whitespace is also present
//  String or char values containing whitespace or commas or enclosed in quotes are not supported
//  Vector can optionally be enclosed in parentheses () or square brackets []
inline
std::istream &
operator >>( std::istream & stream, Vector3 & v )
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
		std::string::size_type const input_size( input_string.size() );
		if ( ( input_size > 0 ) && ( input_string[ input_size - 1 ] == ',' ) ) {
			input_string.erase( input_size - 1 ); // Remove trailing ,
		}
		std::istringstream num_stream( input_string );
		num_stream >> v.y;
	}

	{ // z
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
		num_stream >> v.z;
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

#endif // ObjexxFCL_Vector3_hh_INCLUDED
