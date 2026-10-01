#ifndef ObjexxFCL_CArrayA_hh_INCLUDED
#define ObjexxFCL_CArrayA_hh_INCLUDED

// CArrayA: Memory-Managed C Array Wrapper with Alignment Support
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
#include <ObjexxFCL/CArrayA.fwd.hh>
#include <ObjexxFCL/AlignedAllocator.hh>

// C++ Headers
#include <cassert>
#include <cstddef>
#include <new>
#include <type_traits>

namespace ObjexxFCL {

// CArrayA: Memory-Managed C Array Wrapper with Alignment Support
//  Reduced to the operations used for the overlap-safe copy buffers in the Array classes
template< typename T >
class CArrayA
{

public: // Types

	typedef  AlignedAllocator< T >  Aligned;
	typedef  std::size_t  size_type;

public: // Creation

	// Size Constructor: Built-in types are default, not zero, initialized for performance
	explicit
	CArrayA( size_type const size ) :
	 size_( size ),
	 mem_( Aligned::allocate( size_ ) ),
	 data_( Aligned::data( mem_ ) )
	{
		for ( size_type i = 0; i < size_; ++i ) {
			new ( data_ + i ) T;
		}
	}

	// Copy Constructor
	CArrayA( CArrayA const & ) = delete;

	// Destructor
	~CArrayA()
	{
		destroy();
	}

public: // Assignment

	// Copy Assignment
	CArrayA &
	operator =( CArrayA const & ) = delete;

public: // Inspector

	// Size
	size_type
	size() const
	{
		return size_;
	}

public: // Subscript

	// CArrayA[ i ]: 0-Based Indexing
	template< typename I, class = typename std::enable_if< std::is_integral< I >::value && std::is_unsigned< I >::value >::type >
	T &
	operator []( I const i )
	{
		assert( i < size_ );
		return data_[ i ];
	}

	// CArrayA[ i ]: 0-Based Indexing
	template< typename I, class = typename std::enable_if< std::is_integral< I >::value && std::is_signed< I >::value >::type, typename = void >
	T &
	operator []( I const i )
	{
		assert( ( i >= 0 ) && ( static_cast< size_type >( i ) < size_ ) );
		return data_[ i ];
	}

private: // Methods

	// Destruct Elements and Delete Array Memory (Doesn't Nullify Pointers)
	void
	destroy()
	{
		size_type i( size_ );
		while ( i ) data_[ --i ].~T();
		::operator delete( mem_ );
	}

private: // Data

	size_type size_; // Number of array elements
	void * mem_; // Pointer to raw memory
	T * data_; // Pointer to data

}; // CArrayA

} // ObjexxFCL

#endif // ObjexxFCL_CArrayA_hh_INCLUDED
