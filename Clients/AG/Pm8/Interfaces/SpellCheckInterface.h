// ****************************************************************************
//
//  File Name:			SpellCheckInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RSpellCheckInterface class
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
//  $Logfile:: /PM8/Interfaces/SpellCheckInterface.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SPELLCHECKINTERFACE_H_
#define		_SPELLCHECKINTERFACE_H_

#ifndef	_INTERFACE_H_
#include	"Interface.h"
#endif

const	YInterfaceId	kSpellCheckInterfaceId	= 3;

//	Amount of letters around word to display for context
const int	kContextSize		= 128;


// ****************************************************************************
//
//  Class Name:	RSpellCheckInterface
//
//  Description:	A pure virtual class interfacing with text for spell checking
//
// ****************************************************************************
//
class RSpellCheckInterface : public RInterface
	{
	//	Construction & Destruction
	public :
													RSpellCheckInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Operations
	public :
		virtual void							Startup( ) = 0;
		virtual void							Shutdown( ) = 0;
		virtual BOOLEAN						FindNextWord( RMBCString& word ) = 0;
		virtual void							GetContext( RMBCString& context, int& nWordStartPos ) = 0;
		virtual BOOLEAN						ReplaceWord( const RMBCString& word, const RMBCString& replacement ) = 0;
	} ;


#endif	//	_SPELLCHECKINTERFACE_H_
