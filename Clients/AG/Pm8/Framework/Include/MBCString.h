// ****************************************************************************
//
//  File Name:			MBCSString.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMBCString class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/MBCString.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MBCSTRING_H_
#define		_MBCSTRING_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RMBCStringIterator;
class RDataTransferSource;
class RDataTransferTarget;

// ****************************************************************************
//
//  Class Name:	RMBCString
//
//  Description:	Class for handling MultiByte characters in a string.
//						It will also handle single byte.
//
//						The MultiByte character implementation will not be fully
//						tested inside of Renaissance but the code was taken from
//						a full featured MultiByte implementation.
//
//						To work with Unicode, a RUnicodeString class should
//						probably be made and used in a similar fashion.  Also,
//						To make the implementation of either string seamless,
//						routines should most likely take String Iterators. OR
//						Both The RMBCString and RUnicodeString and their iterators
//						should be derived from base classes of say RString and RString
//						iterator.  This specific implementation is not being persued
//						because of time and schedule pressure. However, nothing will
//						intentionally be codded to break the above paridium.
//
// ****************************************************************************
//

class FrameworkLinkage RMBCString
	{
	//	Constructors & Destructors.
	public :
													RMBCString( );
													RMBCString( const RMBCString& rhs );
													RMBCString( LPCSZ lpcsz );
													RMBCString( LPCSZ lpcsz, int nLength );
													RMBCString( const RCharacter& yChar);
													~RMBCString( );

#ifdef	_WINDOWS
													RMBCString( const CString& cstr );
#endif	//	_WINDOWS

	//	Implementation
	public :
		YCounter									GetStringLength( ) const;
		YCounter									GetDataLength( ) const;
		BOOLEAN									IsEmpty( ) const;
		void										Empty( );

		RCharacter								operator[](int nIndex) const;

		const RMBCString&						operator=( const RMBCString& rhs );
		const RMBCString&						operator=( const RCharacter& rhs );
		const RMBCString&						operator=( LPCSZ lpsz );
#ifdef	_WINDOWS
		const RMBCString&						operator=( const CString& cString );
													operator CString() const;
#endif
													operator LPCSZ() const;
													operator LPCUBYTE() const;

		const RMBCString&						operator+=( const RMBCString& rhs );
		const RMBCString&						operator+=( const RCharacter& rhs );
		const RMBCString&						operator+=( LPCSZ lpsz );
		friend RMBCString						operator+(const RMBCString& string1, const RMBCString& string2);
		friend RMBCString						operator+(const RMBCString& string, LPCSZ lpsz);
		friend RMBCString						operator+(LPCSZ lpsz, const RMBCString& string);		

		BOOLEAN									operator==( const RMBCString& rhs ) const;
		BOOLEAN									operator!=( const RMBCString& rhs ) const;
		BOOLEAN									operator<( const RMBCString& rhs ) const;
		BOOLEAN									operator<=( const RMBCString& rhs ) const;
		BOOLEAN									operator>( const RMBCString& rhs ) const;
		BOOLEAN									operator>=( const RMBCString& rhs ) const;

		//	Various Other implementation
		int										Compare( const RMBCString& compString ) const;
		int										iCompare( const RMBCString& compString ) const;

		void										ToUpper( );
		void										ToLower( );
		void										Reverse( );

		RMBCString								Head( int nCount ) const;
		RMBCString								Tail( int nCount ) const;

	// Data transfer functions
	public :
		void										Copy( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const;
		void										Paste( const RDataTransferSource& dataSource );

	//	Functions that work with/on Iterators
	public :
		typedef	RMBCStringIterator YIterator;

		YIterator								Start( ) const;
		YIterator								End( ) const;

	//	Private members
	private :
#ifdef	_WINDOWS
		CString									m_cString;
#endif	//	_WINDOWS
#ifdef	MAC
		LPSZ										m_cString;
#endif	//	MAC
	} ;

FrameworkLinkage RArchive& operator<<( RArchive& archive, const RMBCString& string );
FrameworkLinkage RArchive& operator>>( RArchive& archive, RMBCString& string );

// ****************************************************************************
// 					Helper Classes
// ****************************************************************************
//
class FrameworkLinkage RMBCStringIterator
	{
		//	Member Data
		private :
			LPCSZ									m_pStart;
			LPCSZ									m_pPosition;
			LPCSZ									m_pEnd;	//	+1 past physical end of data

		//	Construction & Destruction
		public :
													RMBCStringIterator( );
													RMBCStringIterator( const RMBCStringIterator& rhs );
													RMBCStringIterator( const RMBCString& string );
													~RMBCStringIterator( );

		//	Typedefs and Structs
		public :
			typedef	RMBCString YContainerType;

		//	Operator overloads
		public :
			RMBCStringIterator&				operator=( const RMBCStringIterator& rhs );
			BOOLEAN								operator==( const RMBCStringIterator& rhs );
			BOOLEAN								operator!=( const RMBCStringIterator& rhs );
			RCharacter							operator *( ) const;
			RCharacter							operator [](int index ) const;

			//	Increment, decrement operators
			RMBCStringIterator&				operator++( );			//	Prefix notation
			RMBCStringIterator				operator++( int );	//	Postfix notation
			RMBCStringIterator&				operator--( );			//	Prefix notation
			RMBCStringIterator				operator--( int );	//	Postfix notation

			RMBCStringIterator&				operator+=( int n );
			RMBCStringIterator&				operator-=( int n );
			RMBCStringIterator				operator+( int n );
			RMBCStringIterator				operator-( int n );
	} ;

// ****************************************************************************
// 					Inlines
//
//			The String Iterators are defined first because Metrowerks does not
//			like inlines being defined after they are called....
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::RMBCStringIterator( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator::RMBCStringIterator( )
	: m_pStart( NULL ),
	  m_pPosition( NULL ),
	  m_pEnd( NULL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::RMBCStringIterator( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator::RMBCStringIterator( const RMBCStringIterator& rhs )
	: m_pStart( rhs.m_pStart ),
	  m_pPosition( rhs.m_pPosition ),
	  m_pEnd( rhs.m_pEnd )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::RMBCStringIterator( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator::RMBCStringIterator( const RMBCString& string )
	: m_pStart( (LPCSZ)string ),
	  m_pPosition( m_pStart ),
	  m_pEnd( m_pStart + string.GetDataLength() ) 
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::~RMBCStringIterator( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator::~RMBCStringIterator( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::operator=( )
//
//  Description:		Assignment operator
//
//  Returns:			Reference to this;
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator&	RMBCStringIterator::operator=( const RMBCStringIterator& rhs )
	{
	m_pStart		= rhs.m_pStart;
	m_pPosition	= rhs.m_pPosition;
	m_pEnd		= rhs.m_pEnd;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator ++( )
//
//  Description:		Increment operator
//
//  Returns:			A Copy of the old iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator	RMBCStringIterator::operator++( int )	//	Postfix notation
	{
	RMBCStringIterator iter = *this;
	++*this;
	return iter;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator --( )
//
//  Description:		Decrement operator
//
//  Returns:			A Reference to This
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator	RMBCStringIterator::operator--( int )	//	Postfix notation
	{
	RMBCStringIterator	iter = *this;
	--*this;
	return iter;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator +=( )
//
//  Description:		Addition operator
//
//  Returns:			A Reference to This
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator&	RMBCStringIterator::operator+=( int n )
	{
	if (n < 0)
		{
		n = (-n);
		while (n-- > 0)
			--*this;
		}
	else
		{
		while (n-- > 0)
			++*this;
		}
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator -=( )
//
//  Description:		Subtraction operator
//
//  Returns:			A Reference to This
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator&	RMBCStringIterator::operator-=( int n )
	{
	if (n < 0)
		{
		n = (-n);
		while (n-- > 0)
			++*this;
		}
	else
		{
		while (n-- > 0)
			--*this;
		}
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator +( )
//
//  Description:		Addition operator
//
//  Returns:			a copy of a new iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator RMBCStringIterator::operator+( int n )
	{
	RMBCStringIterator	iter = *this;
	iter += n;
	return iter;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator -( )
//
//  Description:		Subtraction operator
//
//  Returns:			a copy of a new iterator
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator RMBCStringIterator::operator-( int n )
	{
	RMBCStringIterator	iter = *this;
	iter -= n;
	return iter;
	}

// ****************************************************************************
// 					Inlines
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RMBCString::Start( )
//
//  Description:		Return an Iterator to the beginning of the string
//
//  Returns:			Iterator pointing to the start of the string
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator RMBCString::Start( ) const
	{
	return RMBCStringIterator( *this );
	}


// ****************************************************************************
//
//  Function Name:	RMBCString::End( )
//
//  Description:		Return an Iterator to the beginning of the string
//
//  Returns:			Iterator pointing to the start of the string
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RMBCStringIterator RMBCString::End( ) const
	{
	RMBCStringIterator iter( *this );
	iter += GetStringLength();
	return iter;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator []( )
//
//  Description:		Dereference operator
//						Metrowerks requires this to be defined out of order
//						since it uses +=
//
//  Returns:			The character at the given position
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCharacter	RMBCStringIterator::operator [](int n ) const
	{
	RMBCStringIterator	iter = *this;
	iter += n;
	return *iter;
	}

// ****************************************************************************
//
// Function Name:		operator+
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RMBCString operator+(const RMBCString& string1, const RMBCString& string2)
{
#ifdef _WINDOWS
	return RMBCString(string1.m_cString + string2.m_cString);
#endif
#ifdef MAC
	RMBCString	string0( string1 );
	string0 += string2;
	return string0;
#endif
}

// ****************************************************************************
//
// Function Name:		operator+
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RMBCString operator+(const RMBCString& string, LPCSZ lpsz)
{
#ifdef _WINDOWS
	return RMBCString(string.m_cString + lpsz);
#endif
#ifdef MAC
	RMBCString	string0( string );
	string0 += lpsz;
	return string0;
#endif
}

// ****************************************************************************
//
// Function Name:		operator+
//
// Description:		
//
// Returns:				
//
// Exceptions:			None
//
// ****************************************************************************
//
inline RMBCString operator+(LPCSZ lpsz, const RMBCString& string)
{
#ifdef _WINDOWS
	return RMBCString(lpsz + string.m_cString);
#endif
#ifdef MAC
	RMBCString	string0( lpsz );
	string0 += string;
	return string0;
#endif
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		//	_MBCSTRING_H_
