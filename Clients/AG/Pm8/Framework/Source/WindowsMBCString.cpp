// ****************************************************************************
//
//  File Name:			WindowsMBCString.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RMBCString class
//
//  Portability:		Windows specific
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
//  $Logfile:: /PM8/Framework/Source/WindowsMBCString.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "MBCString.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DataTransfer.h"

// ****************************************************************************
//
//  Function Name:	RMBCString::RMBCString( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::RMBCString( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::RMBCString( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::RMBCString( const RMBCString& rhs )	: m_cString( rhs.m_cString )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::RMBCString( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::RMBCString( LPCSZ lpsz ) : m_cString( lpsz )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::RMBCString( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::RMBCString( LPCSZ lpsz, int nLength ) : m_cString( lpsz, nLength )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::RMBCString( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::RMBCString( const CString& cString ) : m_cString( cString )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::RMBCString( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::RMBCString( const RCharacter& rhs )
	{
	uBYTE		str[3];
	str[0] = static_cast<uBYTE>( rhs.GetLeadByte() );
	str[1] = static_cast<uBYTE>( rhs.GetTrailByte() );	//	This is 0 on a single byte character
	str[2] = 0;
	m_cString = str;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::~RMBCString( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::~RMBCString( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::GetStringLength( ) const
//
//  Description:		Return the number of Characers in the string
//
//  Returns:			The length of the string
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter	RMBCString::GetStringLength( ) const
	{
	return (YCounter)_tcsclen( (const char*)m_cString );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::GetDataLength( ) const
//
//  Description:		Return the number of Bytes in the string
//
//  Returns:			Number of bytes in the string
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter	RMBCString::GetDataLength( ) const
	{
	return m_cString.GetLength();
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::IsEmpty( )
//
//  Description:		Return TRUE of the string is empty
//
//  Returns:			TRUE if the string is empty
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCString::IsEmpty( ) const
	{
	return static_cast<BOOLEAN>( m_cString.IsEmpty( ) );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::Empty( )
//
//  Description:		Empty the string
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMBCString::Empty( )
	{
	m_cString.Empty();
	}


// ****************************************************************************
//
//  Function Name:	RMBCString::operator[]( )
//
//  Description:		Return the character at the given location in the string
//
//		Notes:		Should this return the MBCS as two separate YCharacters or
//						should it combine it into one.
//	
//  Returns:			the Character at the given string location
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCharacter	RMBCString::operator[](int nIndex) const
	{
	return RCharacter( m_cString[ nIndex ] );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator=( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			a Reference to this string
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString&	RMBCString::operator=( const RMBCString& rhs )
	{
	m_cString = rhs.m_cString;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator=( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			a Reference to this string
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString&	RMBCString::operator=( const RCharacter& rhs )
	{
	char str[3];
	str[0] = static_cast<char>( rhs.GetLeadByte() );
	str[1] = static_cast<char>( rhs.GetTrailByte() );
	str[2] = 0;
	m_cString = str;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator=( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			a Reference to this string
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString&	RMBCString::operator=( LPCSZ lpsz )
	{
	m_cString = lpsz;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator=( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			a Reference to this string
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString&	RMBCString::operator=( const CString& cString )
	{
	m_cString = cString;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator CString&( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			The stored CString
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::operator CString() const
	{
	return m_cString;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator LPCSZ( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			the LPSZ stored in the CString
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::operator LPCSZ() const
	{
	LPCSZ	lpcsz = (LPCTSTR)m_cString;
	return lpcsz;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator LPCUBYTE( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			the LPCUBYTE stored in the CString
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::operator LPCUBYTE() const
	{
	LPCUBYTE	lpcub = (LPCUBYTE)(LPCTSTR)m_cString;
	return lpcub;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator+=( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			a Reference to this string
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString&	 RMBCString::operator+=( const RMBCString& rhs )
	{
	m_cString += rhs.m_cString;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator+=( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			a Reference to this string
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString&	 RMBCString::operator+=( const RCharacter& rhs )
	{
	m_cString += (TCHAR)(int)rhs;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator+=( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			a Reference to this string
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString&	 RMBCString::operator+=( LPCSZ lpsz )
	{
	m_cString += lpsz;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator==( )
//
//  Description:		Test the string for equality with this one
//	
//  Returns:			TRUE if the two strings are equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCString::operator==( const RMBCString& rhs ) const
	{
	return static_cast<BOOLEAN>( Compare( rhs ) == 0 );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator!=( )
//
//  Description:		Test the string for inequality with this one
//	
//  Returns:			TRUE if the two strings are notequal
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCString::operator!=( const RMBCString& rhs ) const
	{
	return static_cast<BOOLEAN>( Compare( rhs ) != 0 );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator<( )
//
//  Description:		Test the string to check if this string is less than the given one
//	
//  Returns:			TRUE if the this string is less than the given one
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCString::operator<( const RMBCString& rhs ) const
	{
	return static_cast<BOOLEAN>( Compare( rhs ) < 0 );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator<=( )
//
//  Description:		Test the string to check if this string is less or equal
//							than the given one
//	
//  Returns:			TRUE if the this string is less than or equal to the given one
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCString::operator<=( const RMBCString& rhs ) const
	{
	return static_cast<BOOLEAN>( Compare( rhs ) <= 0 );
	}


// ****************************************************************************
//
//  Function Name:	RMBCString::operator>( )
//
//  Description:		Test the string to check if this string is greater than the given one
//	
//  Returns:			TRUE if the this string is greater than the given one
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCString::operator>( const RMBCString& rhs ) const
	{
	return static_cast<BOOLEAN>( Compare( rhs ) > 0 );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator>=( )
//
//  Description:		Test the string to check if this string is greater or equal
//							than the given one
//	
//  Returns:			TRUE if the this string is greater than or equal to the given one
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCString::operator>=( const RMBCString& rhs ) const
	{
	return static_cast<BOOLEAN>( Compare( rhs ) >= 0 );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::Compare( )
//
//  Description:		Compare the given string with this one and return the relative
//							comparison of the strings.
//	
//  Returns:			< 0 if the this string is less than the given one.
//							= 0 if the strings are equal
//							> 0 if the this string is greater than the given one
//
//  Exceptions:		None
//
// ****************************************************************************
//
int RMBCString::Compare( const RMBCString& compString ) const
	{
	return m_cString.Compare( compString.m_cString );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::Compare( )
//
//  Description:		Compare the given string with this one and return the relative
//							comparison of the strings (independant of case)
//	
//  Returns:			< 0 if the this string is less than the given one.
//							= 0 if the strings are equal
//							> 0 if the this string is greater than the given one
//
//  Exceptions:		None
//
// ****************************************************************************
//
int RMBCString::iCompare( const RMBCString& compString ) const
	{
	return m_cString.CompareNoCase( compString.m_cString );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::ToUpper( )
//
//  Description:		Convert the entire string to upper case
//	
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMBCString::ToUpper( )
	{
	m_cString.MakeUpper( );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::Lower( )
//
//  Description:		Convert the entire string to lower case
//	
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMBCString::ToLower( )
	{
	m_cString.MakeLower( );
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::Head( )
//
//  Description:		Return a String consisting of the first nCount characters
//	
//  Returns:			The new string
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RMBCString RMBCString::Head( int nCount ) const
	{
	RMBCString	headString;
	try
		{
		TpsAssert( nCount <= m_cString.GetLength( ), "Head asking for more characters than in string" );
		headString = m_cString.Left(nCount);
		}
	catch( ... )
		{
		throw kMemory;
		}
	return headString;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::Tail( )
//
//  Description:		Return a String consisting of the Last nCount characters
//	
//  Returns:			The new string
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RMBCString RMBCString::Tail( int nCount ) const
	{
	RMBCString	headString;
	try
		{
		TpsAssert( nCount <= m_cString.GetLength( ), "Tail asking for more characters than in string" );
		headString = m_cString.Right(nCount);
		}
	catch( ... )
		{
		throw kMemory;
		}
	return headString;
	}

// ****************************************************************************
//
//  Function Name:	RMBCString::Reverse( )
//
//  Description:		Reverse the string in place.
//	
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMBCString::Reverse( ) 
{
	UnimplementedCode(); 
}


// ****************************************************************************
//				RMBCStringIterator methos
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::operator==( )
//
//  Description:		Equality operator
//
//  Returns:			TRUE if iterators are equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCStringIterator::operator==( const RMBCStringIterator& rhs )
	{
	TpsAssert( m_pStart == rhs.m_pStart, "Iterators being compares for different strings" );
	TpsAssert( m_pEnd == rhs.m_pEnd, "Iterators being compares for different strings" );
	return static_cast<BOOLEAN>( m_pPosition == rhs.m_pPosition );
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::operator!=( )
//
//  Description:		Inequality operator
//
//  Returns:			TRUE if iterators are NOT equal
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMBCStringIterator::operator!=( const RMBCStringIterator& rhs )
	{
	TpsAssert( m_pStart == rhs.m_pStart, "Iterators being compares for different strings" );
	TpsAssert( m_pEnd == rhs.m_pEnd, "Iterators being compares for different strings" );
	return static_cast<BOOLEAN>( m_pPosition != rhs.m_pPosition );
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator ++( )
//
//  Description:		Increment operator
//
//  Returns:			A reference to this object
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCStringIterator&	RMBCStringIterator::operator++( )
	{
	TpsAssert( m_pPosition != m_pEnd, "Iterating past end of iterator bounds");
#ifdef	_WINDOWS
	m_pPosition += (int)_tclen( m_pPosition );
#else	//	_WINDOWS
	m_pPosition	+= 1;	//	Mac does not do MBCS or Unicode for now...
#endif	//	_WINDOWS
	return *this;
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
RMBCStringIterator&	RMBCStringIterator::operator--( )			//	Prefix notation
	{
	TpsAssert( m_pPosition != m_pStart, "Iterating past start of interator bounds" );
#ifdef	_WINDOWS
	m_pPosition = _tcsdec( m_pStart, m_pPosition );
#else	//	_WINDOWS
	m_pPosition	-= 1;	//	Mac doesn't support MBCS or Unicode for now.
#endif	//	_WINDOWS
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RMBCStringIterator::Operator *( )
//
//  Description:		Dereference operator
//
//  Returns:			The character at the given position
//
//  Exceptions:		None
//
// ****************************************************************************
//
RCharacter	RMBCStringIterator::operator *( ) const
	{
	TpsAssert( m_pPosition != m_pEnd, "Dereferencing past end of iterator bounds" );
#ifdef	_WINDOWS
	uBYTE	 ubLead = *m_pPosition;
	if (_istlead(ubLead))
		{
		TpsAssert( (m_pPosition+1) != m_pEnd, "Last character is lead character, but trail character is not present ");
		return RCharacter( ubLead, *(m_pPosition+1) );
		}
	else
		return RCharacter( ubLead );
#else		//	_WINDOWS
	return RCharacter( *m_pPosition );
#endif	//	_WINDOWS
	}

// ****************************************************************************
//																												 
//  Function Name:	RMBCString::Copy( )
//
//  Description:		Copies this text string to the specified data transfer
//							target in the specified format
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RMBCString::Copy( RDataTransferTarget& dataTarget, YDataFormat dataFormat ) const
	{
	switch( dataFormat )
		{
		case kTextFormat :
			dataTarget.SetData( dataFormat, *this, GetStringLength( ) + 1 );
			break;

		default :
			TpsAssertAlways( "Unsupported data format." );
		}
	}

// ****************************************************************************
//																												 
//  Function Name:	RMBCString::Paste( )
//
//  Description:		Copies data from the specified data transfer source into
//							this string.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMBCString::Paste( const RDataTransferSource& dataSource )
	{
	TpsAssert( dataSource.IsFormatAvailable( kTextFormat ), "No text data available." );

	uBYTE* pData= NULL;
	YDataLength dataLength;

	try
		{
		// Get the data in text format
		dataSource.GetData( kTextFormat, pData, dataLength );

		// Create a new string and initialize it with the data
		RMBCString tempString( pData );

		// Copy the new string into this string
		*this = tempString;
		}

	catch( ... )
		{
		;
		}

	// Clean up
	delete [] pData;
	}

// ****************************************************************************
//
// Function Name:		operator<<
//
// Description:		Insertion operator for strings
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator<<( RArchive& archive, const RMBCString& string )
	{
	//	use +1 for null termination...
	uLONG		ulStringLength	= static_cast<uLONG>( string.GetDataLength() + 1 );
	archive	<< ulStringLength;
	archive.Write( ulStringLength, (LPCUBYTE)string );
	return archive;
	}

// ****************************************************************************
//
// Function Name:		operator>>
//
// Description:		Extraction operator for transforms
//
// Returns:				The archive
//
// Exceptions:			None
//
// ****************************************************************************
//
RArchive& operator>>( RArchive& archive, RMBCString& string )
	{
	uLONG		ulStringLength;
	archive >> ulStringLength;
	if ( ulStringLength > 0 )
		{
		uBYTE*	pBuffer	= new uBYTE[ulStringLength];
		archive.Read( ulStringLength, pBuffer );
		string	= pBuffer;
		delete [] pBuffer;
		}
	else
		string.Empty();
	return archive;
	}
