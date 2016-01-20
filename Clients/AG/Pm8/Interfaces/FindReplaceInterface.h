// ****************************************************************************
//
//  File Name:			FindReplaceInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RFindReplaceInterface class
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
//  $Logfile:: /PM8/Interfaces/FindReplaceInterface.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FINDREPLACEINTERFACE_H_
#define		_FINDREPLACEINTERFACE_H_

#ifndef	_INTERFACE_H_
#include	"Interface.h"
#endif

const	YInterfaceId	kFindReplaceInterfaceId				= 10;
const	YInterfaceId	kCalendarFindReplaceInterfaceId	= 11;

//	Amount of letters around word to display for context
const int	kFindReplaceContextSize		= 128;

typedef int EFindReplaceFlags;
const EFindReplaceFlags				kFindWholeWord			= 0x01;
const EFindReplaceFlags				kFindCaseSensitive	= 0x02;

// ****************************************************************************
//
//  Class Name:	RFindReplaceInterface
//
//  Description:	A pure virtual class interfacing with text for spell checking
//
// ****************************************************************************
//
class RFindReplaceInterface : public RInterface
	{
	//	Construction & Destruction
	public :
													RFindReplaceInterface ( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Operations
	public :
		virtual void							Startup( ) = 0;
		virtual void							Shutdown( ) = 0;
		virtual BOOLEAN						FindNextWord( EFindReplaceFlags flags, RMBCString& word ) = 0;
		virtual void							GetContext( RMBCString& context, int& nWordStartPos ) = 0;
		virtual BOOLEAN						ReplaceWord( const RMBCString& word, const RMBCString& replacement ) = 0;
	} ;


// ****************************************************************************
//						INLINES
// ****************************************************************************
//

// ****************************************************************************
//
//  Function Name:	MapCharacterToSearchFor()
//
//  Description:		Take the iterator and return the character to scan for
//							depending on the flags
//
//  Returns:			The character
//
//  Exceptions:		None
//
// ****************************************************************************
inline RCharacter	MapCharacterToSearchFor( RMBCStringIterator& iter, EFindReplaceFlags flags )
{
	RCharacter	ch	= *iter;
	if ( !(flags & kFindCaseSensitive) )
		ch	= RCharacter( MacWinDos( xxx, _tolower((int)ch), xxx ) );
	return ch;
}

#endif	//	_FINDREPLACEINTERFACE_H_
