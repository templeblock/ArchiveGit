// ****************************************************************************
//
//  File Name:			Character.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RCharacter class
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
//  $Logfile:: /PM8/Framework/Source/Character.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "Character.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "MBCString.h"

// ****************************************************************************
//
//  Function Name:	RCharacter::Compare( )
//
//  Description:		Compare the two characters for equality
//
//  Returns:			the Standard ansi comparison values
//
//  Exceptions:		None
//
// ****************************************************************************
//
int	RCharacter::Compare( int ch ) const
	{
	RCharacter compareCharacter(ch);
	return Compare( compareCharacter );
	}

// ****************************************************************************
//
//  Function Name:	RCharacter::Compare( )
//
//  Description:		Compare the two characters for equality
//
//  Returns:			the Standard ansi comparison values
//
//  Exceptions:		None
//
// ****************************************************************************
//
int	RCharacter::Compare( const RCharacter& character ) const
	{
		//
		// Handle easy cases, if possible.
		//

		if (m_uwChar == character.m_uwChar)
			return 0;

		else if (m_uwChar == 0)
			return -1;

		else if (character.m_uwChar == 0)
			return 1;

	#ifdef	_MBCS
		{
			//
			// I don't know of any routine that will just compare two characters.
			// We need to build two strings, and call a string comparison routine.
			//
		#ifdef	_WINDOWS
			RMBCString str1( *this );
			RMBCString str2( character );
			return str1.Compare( str2 );
		#else		//	_WINDOWS
				//	Must use script manager eventually
			return (m_uwChar - character.m_uwChar);
		#endif	//	_WINDOWS

		}
	#else		//	_MBCS
		{
			//
			// REVIEW BOB -- This is bogus, based on knowledge of how
			// the macro _tccmp() is implemented.
			//
		#ifdef	_WINDOWS
			return _tccmp( (LPCTSTR)&m_wch, (LPCTSTR)&bch.m_wch );
		#else		//	_WINDOWS
			return (m_uwChar - character.m_uwChar);
		#endif	//	_WINDOWS
		}
	#endif	//	_MBCS
	}



