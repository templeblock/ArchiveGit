// ****************************************************************************
//
//  File Name:			Character.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RCharacter class
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
//  $Logfile:: /PM8/Framework/Include/Character.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CHARACTER_H_
#define		_CHARACTER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// This is not defined in TCHAR.H, but it probably should be...
//
#ifdef	_WINDOWS
	#if	defined(_UNICODE)
		#define _isttrail(_c)    (0)
	#elif	defined(_MBCS)
		#define _isttrail    _ismbbtrail
	#else
		#define _isttrail(_c)    (0)
	#endif	//	_UNICODE
#endif

#ifdef	_WINDOWS
enum EVirtualCharacter
	{
		kTabCharacter				= VK_TAB,
		kReturnCharacter			= VK_RETURN,
		kBackspaceCharacter		= VK_BACK,
		kDeleteCharacter			= VK_DELETE,
		kEscapeCharacter			= VK_ESCAPE,
		kLeftArrowCharacter		= VK_LEFT,
		kRightArrowCharacter		= VK_RIGHT,
		kUpArrowCharacter			= VK_UP,
		kDownArrowCharacter		= VK_DOWN,
		kHomeCharacter				= VK_HOME,
		kEndCharacter				= VK_END,
		kPageUpCharacter			= VK_PRIOR,
		kPageDownCharacter		= VK_NEXT,
		kShiftCharacter			= VK_SHIFT,
		//	Others may be added here.
		kUnknownCharacterType	= -1		//	This might not be a good value to use...
	} ;
#else		//	_WINDOWS
enum EVirtualCharacter
	{
		kTabCharacter				= 0x09,
		kReturnCharacter			= 0x0D,
		kBackspaceCharacter		= 0x08,
		kDeleteCharacter			= 0x7F,
		kEscapeCharacter			= 0x1B,
		kLeftArrowCharacter		= 0x00,
		kRightArrowCharacter		= 0x00,
		kUpArrowCharacter			= 0x00,
		kDownArrowCharacter		= 0x00,
		kHomeCharacter				= 0x00,
		kEndCharacter				= 0x00,
		kPageUpCharacter			= 0x00,
		kPageDownCharacter		= 0x00,
		kShiftCharacter			= 0x00,
		//	Others may be added here.
		kUnknownCharacterType	= -1		//	This might not be a good value to use...
	} ;
#endif	//	_WINDOWS

// ****************************************************************************
//
//  Class Name:	RCharacter
//
//  Description:	The class for dealing with Characters that may be single-byte
//						or MultiByte.
//
//						The basis for this class was taken from BChar.h, BChar.inl
//						and BChar.cpp
//
//						For simplicity, the only cast operators that are implemented
//						are "cast from int to RCharacter and "cast from RCharacter to int".
//						To convert to/from some other type, use an intermediate cast
//						to "int".
//
//						This class has not been tested with UNICODE or MBCS
//
// ****************************************************************************
//
class FrameworkLinkage RCharacter
	{
	//	Construction & Destruction
	public:
													RCharacter();
													RCharacter( int ch );
													RCharacter( const RCharacter& bch );
													RCharacter( uBYTE ubLead, uBYTE ubTrail );
													~RCharacter( );

	//	Implementation
	public :
													operator int() const;
  		const RCharacter&						operator=( int ch );
		const RCharacter&						operator=( const RCharacter& rhs );

		int										Compare( int ch ) const;
		int										Compare( const RCharacter& character ) const;

		BOOLEAN									operator== ( const RCharacter& character );
		BOOLEAN									operator== ( int ch );
		BOOLEAN									operator!= ( const RCharacter& character );
		BOOLEAN									operator!= ( int ch );

		BOOLEAN									IsLegal() const;
		BOOLEAN									IsAscii() const;
		BOOLEAN									IsCntrl() const;
		BOOLEAN									IsPrint() const;
		BOOLEAN									IsGraph() const;
		BOOLEAN									IsSpace() const;
		BOOLEAN									IsPunct() const;
		BOOLEAN									IsAlphaNum() const;
		BOOLEAN									IsDigit() const;
		BOOLEAN									IsXDigit() const;
		BOOLEAN									IsAlpha() const;
		BOOLEAN									IsUpper() const;
		BOOLEAN									IsLower() const;
		BOOLEAN									IsLead() const;
		BOOLEAN									IsTrail() const;
		BOOLEAN									IsLeadByte() const;
		BOOLEAN									IsValid() const;
		BOOLEAN									IsWideChar() const;
		BOOLEAN									IsMultiByte() const;

		int										GetLeadByte() const;
		int										GetTrailByte() const;
		int										GetSize() const;

	//	Members
	private :
		uWORD										m_uwChar;
	} ;


// ****************************************************************************
//
//					Inlines
//
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	RCharacter::RCharacter( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCharacter::RCharacter() : m_uwChar( 0 )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::RCharacter( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCharacter::RCharacter( int ch ) : m_uwChar( (uWORD)ch )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RCharacter::RCharacter( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCharacter::RCharacter( uBYTE ubLead, uBYTE ubTrail )
	{
#if	defined(_UNICODE)

//	TpsAssertAlways("If compilied as UNICODE, there are no Trail -vs- Lead bytes" );
	m_uwChar = (uWORD)ubLead;

#elif	defined(_MBCS)

	if (ubTrail == 0)
		m_uwChar = (uWORD)ubLead;
	else
	{
	#ifdef	_WINDOWS
		m_uwChar = (uWORD)MAKEWORD(ubTrail, ubLead);
	#else
		m_uwChar = ((uWORD)ubLead << 8) + ubTrail;
	#endif
	}

#else

//	TpsAssert(ubTrail == 0, "Trail byte is not 0 in a single byte world" );
	m_uwChar = (uWORD)ubLead;

#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::RCharacter( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCharacter::RCharacter( const RCharacter& character ) : m_uwChar( character.m_uwChar )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::RCharacter( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCharacter::~RCharacter( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::int( )
//
//  Description:		Cast operator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCharacter::operator int() const
	{
	return (int)m_uwChar;
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::operator=( )
//
//  Description:		Assignment operator
//
//  Returns:			references to this bject
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline   const RCharacter&	RCharacter::operator=( int ch )
	{
	m_uwChar = (uWORD)ch;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::operator=( )
//
//  Description:		Assignment operator
//
//  Returns:			Reference to this object
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline const RCharacter&	RCharacter::operator=( const RCharacter& character )
	{
	m_uwChar = character.m_uwChar;
	return *this;
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::operator==( )
//
//  Description:		Equality operator
//
//  Returns:			True if the values are the same
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::operator==( const RCharacter& rhs )
	{
	return (BOOLEAN)(m_uwChar == rhs.m_uwChar );
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::operator==( )
//
//  Description:		Equality operator
//
//  Returns:			True if the values are the same
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::operator==( int ch )
	{
	return (BOOLEAN)(m_uwChar == (uWORD)ch);
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::operator!=( )
//
//  Description:		Inequality operator
//
//  Returns:			True if the values are the different
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::operator!=( const RCharacter& rhs )
	{
	return (BOOLEAN)(m_uwChar != rhs.m_uwChar);
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::operator!=( )
//
//  Description:		Inequality operator
//
//  Returns:			True if the values are the different
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::operator!=( int ch )
	{
	return (BOOLEAN)(m_uwChar != (uWORD)ch);
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsLegal( )
//
//  Description:		Return TRUE if the character is 'legal'
//
//  Returns:			True if the value is legal
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsLegal() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istlegal( m_uwChar ) );
	#else
		return (BOOLEAN)(m_uwChar<0x100);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsAscii( )
//
//  Description:		Check if the value is ascii
//
//  Returns:			True if the value Is Ascii
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsAscii() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istascii( m_uwChar ) );
	#else
		return (BOOLEAN)(m_uwChar<0x80);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsCntrl( )
//
//  Description:		Check if the character is a control character
//
//  Returns:			True if the value are the different
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsCntrl() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istcntrl( m_uwChar ) );
	#else
		return (BOOLEAN)(FALSE);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsPrint( )
//
//  Description:		Check if the character is printable
//
//  Returns:			True if the value is printable
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsPrint() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istprint( m_uwChar ) );
	#else
		return (BOOLEAN)(TRUE);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsGraph( )
//
//  Description:		Check if the character is a Non-blank printable character
//
//  Returns:			True if the value is a non-blank printable character
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsGraph() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istgraph( m_uwChar ) );
	#else
		return (BOOLEAN)( m_uwChar != (uWORD)' ');
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsSpace( )
//
//  Description:		Inequality operator
//
//  Returns:			True if the value are the different
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsSpace() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istspace( m_uwChar ) );
	#else
		return (BOOLEAN)(m_uwChar == (uWORD)' ');
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsPunct( )
//
//  Description:		Check if the character is a punctuation
//
//  Returns:			True if the value is a punctuation
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsPunct() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istpunct( m_uwChar ) );
	#else
//		UnimplementedCode();							// REVEIW RAH 
		return (BOOLEAN)(FALSE);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsAlpha( )
//
//  Description:		Check if the character is an alpha character
//
//  Returns:			True if the values is an Alpha character
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsAlpha() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istalpha( m_uwChar ) );
	#else
		return (BOOLEAN)((m_uwChar >= (uWORD)'A') && (m_uwChar <= (uWORD)'Z')) ||
							 ((m_uwChar >= (uWORD)'A') && (m_uwChar <= (uWORD)'Z'));
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsDigit( )
//
//  Description:		Check if the value is a digit
//
//  Returns:			True if the value is a digit
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsDigit() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istdigit( m_uwChar ) );
	#else
		return (BOOLEAN)((m_uwChar >= (uWORD)'0') && (m_uwChar <= (uWORD)'9'));
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsAlphaNum( )
//
//  Description:		Check if the character is alphanumeric
//
//  Returns:			True if the value is alphanumeric
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsAlphaNum() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istalnum( m_uwChar ) );
	#else
		return (BOOLEAN)(IsAlpha() || IsDigit());
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsXDigit( )
//
//  Description:		Check if the character is a Hexvalue or digit
//
//  Returns:			True if the value is a HexValue or a digit
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsXDigit() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istxdigit( m_uwChar ) );
	#else
		return (BOOLEAN)((m_uwChar >= (uWORD)'A') && (m_uwChar <= (uWORD)'F')) ||
							 ((m_uwChar >= (uWORD)'a') && (m_uwChar <= (uWORD)'f')) ||
							IsDigit();
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsUpper( )
//
//  Description:		Check if the character is an Uppercase character
//
//  Returns:			True if the value is uppercase
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsUpper() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istupper( m_uwChar ) );
	#else
		return (BOOLEAN)((m_uwChar >= (uWORD)'A') && (m_uwChar <= (uWORD)'Z'));
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsLower( )
//
//  Description:		Check if the character is a lower case character
//
//  Returns:			True if the value is a lower case character
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsLower() const
	{
	#ifdef	_WINDOWS
		return static_cast<BOOLEAN>( _istlower( m_uwChar ) );
	#else
		return (BOOLEAN)((m_uwChar >= (uWORD)'a') && (m_uwChar <= (uWORD)'z'));
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsLead( )
//
//  Description:		Check if the Multi-byte character is a lead character
//
//  Returns:			True if the value is a lead character
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsLead() const
	{
	#ifdef	_WINDOWS
		//
		// REVIEW BOB -- Maybe this should always return 0,
		//               since we should have a complete char.
		//
		return static_cast<BOOLEAN>( _istlead( m_uwChar ) );
	#else
		return (BOOLEAN)(FALSE);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsTrail( )
//
//  Description:		Check if the character is a trail character
//
//  Returns:			True if the value is a trail character
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsTrail() const
	{
	#ifdef	_WINDOWS
		//
		// REVIEW BOB -- Maybe this should always return 0,
		//               since we should have a complete char.
		//
		return static_cast<BOOLEAN>( _isttrail( m_uwChar ) );
	#else
		return (BOOLEAN)(FALSE);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsLeadByte( )
//
//  Description:		Check if the character is a lead byte
//
//  Returns:			True if the value is a lead byte
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsLeadByte() const
	{
	#ifdef	_WINDOWS
		//
		// REVIEW BOB -- Maybe this should always return 0,
		//               since we should have a complete char.
		//
		return static_cast<BOOLEAN>( _istleadbyte( m_uwChar ) );
	#else
		return (BOOLEAN)(FALSE);
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsValid( )
//
//  Description:		Check if the value is valid
//
//  Returns:			TRUE if the value is a valid character
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsValid() const
	{
	#ifdef	_WINDOWS
		uBYTE	ubLead = HIBYTE(m_uwChar);
		if (ubLead)
			return static_cast<BOOLEAN>( ( _istlead(ubLead) && _isttrail(LOBYTE(m_uwChar)) ) );
		else
			//	If no lead byte, must be standard character which is valid
			return TRUE;
	#else		//	_WINDOWS
		return TRUE;
	#endif	//	_WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsWideChar( )
//
//  Description:		Check if the character is a Wide character (2byte)
//
//  Returns:			True if the values are the different
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsWideChar() const
	{
	#ifdef	_UNICODE
		return TRUE;
	#else
		return FALSE;
	#endif
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::IsMultiByte( )
//
//  Description:		Inequality operator
//
//  Returns:			True if the values are the different
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN	RCharacter::IsMultiByte() const
	{
#ifdef	_UNICODE
		return TRUE;
#else		//	_UNICODE
	#ifdef	_WINDOWS
		uBYTE	ubLead	= HIBYTE(m_uwChar);
		return static_cast<BOOLEAN>( (_istlead(ubLead)) );
	#else		//	_WINDOWS
		uBYTE	ubLead	= ((uBYTE*)&m_uwChar)[0];
		return (BOOLEAN)(ubLead != 0);
	#endif	//	_WINDOWS
#endif	//	_UNICODE
	}



// ****************************************************************************
//
//  Function Name:	RCharacter::GetLeadByte( )
//
//  Description:		Return the lead byte of a multi-byte/single-byte/or double-byte
//
//  Returns:			The lead character
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline int	RCharacter::GetLeadByte() const
	{
	#ifdef	_UNICODE
		return (int)m_uwChar;
	#else		//	_UNICODE
#ifdef	_WINDOWS
		uBYTE		ubLead	= HIBYTE(m_uwChar);
		uBYTE		ubTrail	= LOBYTE(m_uwChar);
#else		//	_WINDOWS
		uBYTE*	pChar = (uBYTE*)&m_uwChar;
		uBYTE		ubLead  = pChar[0];
		uBYTE		ubTrail = pChar[1];
#endif	//	_WINDOWS
		if (ubLead)
			return ubLead;
		else
			return ubTrail;
	#endif	//	_UNICODE
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::GetTrailByte( )
//
//  Description:		Return the trail byte of a multi-byte, returns 0 for UNICODE or single byte
//
//  Returns:			The trail byte
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline int	RCharacter::GetTrailByte() const
	{
	#ifdef	_UNICODE
		return 0;
	#else		//	_UNICODE
#ifdef	_WINDOWS
		uBYTE		ubLead	= HIBYTE(m_uwChar);
		uBYTE		ubTrail	= LOBYTE(m_uwChar);
#else		//	_WINDOWS
		uBYTE*	pChar = (uBYTE*)&m_uwChar;
		uBYTE		ubLead  = pChar[0];
		uBYTE		ubTrail = pChar[1];
#endif	//	_WINDOWS
		if (ubLead)
			return ubTrail;
		else
			return 0;
	#endif	//	_UNICODE
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::GetSize( )
//
//  Description:		Return the Size of the character
//
//  Returns:			2 if Multi-Byte, 1 if single-byte or unicode
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline int	RCharacter::GetSize() const
	{
	return (IsMultiByte())? 2 : 1;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		//	_CHARACTER_H_
