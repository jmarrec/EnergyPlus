#ifndef ObjexxFCL_CArray_hh_INCLUDED
#define ObjexxFCL_CArray_hh_INCLUDED

// CArray: Memory-Managed C Array Wrapper
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
#include <ObjexxFCL/CArray.fwd.hh>

// C++ Headers
#include <cassert>
#include <cstddef>
#include <type_traits>

namespace ObjexxFCL {

// CArray: Memory-Managed C Array Wrapper
//  Reduced to the operations used for the overlap-safe copy buffers in the Array classes
template< typename T >
class CArray
{

public: // Types

	typedef  std::size_t  size_type;

public: // Creation

	// Size Constructor: Built-in types are default, not zero, initialized for performance
	explicit
	CArray( size_type const size ) :
	 size_( size ),
	 data_( size_ > 0u ? new T[ size_ ] : nullptr )
	{}

	// Copy Constructor
	CArray( CArray const & ) = delete;

	// Destructor
	~CArray()
	{
		delete[] data_;
	}

public: // Assignment

	// Copy Assignment
	CArray &
	operator =( CArray const & ) = delete;

public: // Inspector

	// Size
	size_type
	size() const
	{
		return size_;
	}

public: // Subscript

	// CArray[ i ]: 0-Based Indexing
	template< typename I, class = typename std::enable_if< std::is_integral< I >::value && std::is_unsigned< I >::value >::type >
	T &
	operator []( I const i )
	{
		assert( i < size_ );
		return data_[ i ];
	}

	// CArray[ i ]: 0-Based Indexing
	template< typename I, class = typename std::enable_if< std::is_integral< I >::value && std::is_signed< I >::value >::type, typename = void >
	T &
	operator []( I const i )
	{
		assert( ( i >= 0 ) && ( static_cast< size_type >( i ) < size_ ) );
		return data_[ i ];
	}

private: // Data

	size_type size_; // Number of array elements
	T * data_; // C array

}; // CArray

} // ObjexxFCL

#endif // ObjexxFCL_CArray_hh_INCLUDED
