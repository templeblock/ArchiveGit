// ****************************************************************************
//
//  File Name:			MacMBCString.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				R. Hood
//
//  Description:		Definition of the RMBCString class
//
//  Portability:		Mac specific
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
//  $Logfile:: /PM8/Framework/Source/MacMBCString.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "MBCString.h"


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
RMBCString::RMBCString( )	: m_cString( NULL )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RMBCString::RMBCString( )
//
//  Description:		Copy Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::RMBCString( const RMBCString& rhs ) 
{
	if ( rhs.m_cString == NULL )
		m_cString = NULL;
	else
	{
		m_cString = new sBYTE[ strlen( rhs.m_cString ) + 1 ];
		strcpy( m_cString, rhs.m_cString );
	}
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
RMBCString::RMBCString( LPCSZ lpsz ) 
{
	if ( lpsz == NULL )
		m_cString = NULL;
	else
	{
		m_cString = new sBYTE[ strlen( lpsz ) + 1 ];
		strcpy( m_cString, lpsz );
	}
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
	m_cString = new sBYTE[ 3 ];
	m_cString[0] = rhs.GetLeadByte();
	m_cString[1] = rhs.GetTrailByte();	//	This is 0 on a single byte character
	m_cString[2] = 0;
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
	delete [] m_cString;
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
	if ( m_cString == NULL )
		return 0;
	return (YCounter)strlen( (const char*)m_cString );
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
	if ( m_cString == NULL )
		return 0;
	return (YCounter)strlen( (const char*)m_cString ) + 1;
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
	if ( m_cString == NULL )
		return TRUE;
	return( *m_cString == 0 );
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
	delete [] m_cString;
	m_cString = NULL;
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
	int	character = m_cString[ nIndex ];
	TpsAssert( ( nIndex >= 0 ), "Bad character index." )
	TpsAssert( ( nIndex < GetDataLength() ), "Bad character index." )
	return RCharacter( character );
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
	if ( this == &rhs )
		return *this;
	delete [] m_cString;
	if ( rhs.m_cString == NULL )
		m_cString = NULL;
	else
	{
		m_cString = new sBYTE[ strlen( rhs.m_cString ) + 1 ];
		strcpy( m_cString, rhs.m_cString );
	}
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
	delete [] m_cString;
	m_cString = new sBYTE[ 3 ];
	m_cString[0] = rhs.GetLeadByte();
	m_cString[1] = rhs.GetTrailByte();
	m_cString[2] = 0;
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
	delete [] m_cString;
	if ( lpsz == NULL )
		m_cString = NULL;
	else
	{
		m_cString = new sBYTE[ strlen( lpsz ) + 1 ];
		strcpy( m_cString, lpsz );
	}
	return *this;
}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator LPSZ( )
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
	LPCSZ	lpcsz = (LPCSZ)m_cString;
	return lpcsz;
}

// ****************************************************************************
//
//  Function Name:	RMBCString::operator LPSZ( )
//
//  Description:		Set the contents of this string to have the given value
//	
//  Returns:			the LPSZ stored in the CString
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMBCString::operator LPCUBYTE() const
{
	LPCUBYTE	lpcub = (LPCUBYTE)m_cString;
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
int		size = 1;
LPSZ		string = NULL;

	if ( m_cString != NULL )
		size += strlen( m_cString );
	if ( rhs.m_cString != NULL )
		size += strlen( rhs.m_cString );
	if ( size > 1 )
	{
		string = new sBYTE[ size ];
		*string = 0;
		if ( m_cString != NULL )
			strcpy( string, (LPCSZ)m_cString );
		if ( rhs.m_cString != NULL )
			strcat( string, (LPCSZ)rhs.m_cString );
	}
	else
		string = NULL;
	delete [] m_cString;
	m_cString = string;
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
int		length = 0;
LPSZ		string = NULL;

	if ( m_cString != NULL )
		length = strlen( m_cString );
	string = new sBYTE[ length + 3 ];
	if ( m_cString != NULL )
		strcpy( string, (LPCSZ)m_cString );
	string[length] = rhs.GetLeadByte();
	string[length + 1] = rhs.GetTrailByte();	//	This is 0 on a single byte character
	string[length + 2] = 0;
	delete [] m_cString;
	m_cString = string;
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
int		size = 1;
LPSZ		string = NULL;

	if ( m_cString != NULL )
		size += strlen( m_cString );
	if ( lpsz != NULL )
		size += strlen( lpsz );
	if ( size > 1 )
	{
		string = new sBYTE[ size ];
		*string = 0;
		if ( m_cString != NULL )
			strcpy( string, (LPCSZ)m_cString );
		if ( lpsz != NULL )
			strcat( string, lpsz );
	}
	else
		string = NULL;
	delete [] m_cString;
	m_cString = string;
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
	return Compare( rhs ) == 0;
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
	return Compare( rhs ) != 0;
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
	return Compare( rhs ) < 0;
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
	return Compare( rhs ) <= 0;
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
	return Compare( rhs ) > 0;
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
	return Compare( rhs ) >= 0;
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
	return ::strcmp( m_cString, compString.m_cString );
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
	return ( m_pPosition == rhs.m_pPosition );
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
	return ( m_pPosition != rhs.m_pPosition );
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
	m_pPosition	+= 1;	//	Mac does not do MBCS or Unicode for now...
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
	m_pPosition	-= 1;	//	Mac doesn't support MBCS or Unicode for now.
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
	return RCharacter( *m_pPosition );
}


