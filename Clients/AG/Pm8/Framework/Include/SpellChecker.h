//****************************************************************************
//
// File Name:		SpellChecker.h
//
// Project:			Renaissance Framework Component
//
// Author:			Mike Heydlauf
//
// Description:	Spell checker interface to Inso libraries
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/SpellChecker.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//*****************************************************************************

#ifndef		_SPELLCHECKER_H_
#define		_SPELLCHECKER_H_

#ifndef		_CORRECTSPELL_H_
#include		"CorrectSpell.h"
#endif

#ifndef		_VIEW_H_
#include		"View.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class FrameworkLinkage RSpellChecker 
{
	// Construction & destruction
	public :
													RSpellChecker( YInterfaceCollection& collection );
													~RSpellChecker( );

	//	Operations
	public :
		void										StartSpellCheck( );
		BOOLEAN									FindMisspelledWord( );
		void										Replace( const RMBCString& rReplacement );
		void										AddToDictionary( );
		void										Done( );

		const RMBCString&						GetMisspelledWord( ) const;
		void										GetContext( RMBCString& context, int& nWordStartPos );
		BOOLEAN									GetAlternateSpelling( RMBCString& alternate );
		void										AddWordToPersonalDict( RMBCString& rWord );

	//	Data members
	private :
		BOOLEAN									m_fStarted;
		RMBCString								m_MisspelledWord;
		YInterfaceCollection&				m_Collection;
		YInterfaceIterator					m_Iterator;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_SPELLCHECKER_H_