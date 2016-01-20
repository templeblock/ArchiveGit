//****************************************************************************
//
// File Name:		CorrectSpell.h
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
//  $Logfile:: /PM8/Framework/Include/CorrectSpell.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//*****************************************************************************

#ifndef		_CORRECTSPELL_H_
#define		_CORRECTSPELL_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

BOOLEAN	LocateFile( RMBCString& rFile, RMBCString& rPath );

extern "C"
{
	//
	// Correct spell API includes
	#include "ICFUN.H"
	#include "SLLIBHDR.H"
	#include "ICSAPI.H"
	#include "ICSEXT.H"
}


class FrameworkLinkage RCorrectSpell 
{
	// Construction & destruction
	public :
					RCorrectSpell( );
		virtual	~RCorrectSpell( );

		//
		// Initialization methods
	public:
		BOOLEAN	Initialize( BOOLEAN fInitICDatabase = TRUE, BOOLEAN fInitPersonalDict = TRUE );
		BOOLEAN	Terminate();

		enum EAlternatives { kNoAlternatives, kAltAvail, kMoreAltAvail };

	protected:
		int		InitICDatabase();
		int		InitPersonalDictionary();
		void		ProcessICInitFailure();
		void		ProcessPDInitFailure();

		int		CloseICDatabase();
		int		SavePersonalDictionary();
		void		ProcessICCloseFailure();
		void		ProcessPDCloseFailure();

		//
		// Spell checking methods
	public:
		int		OpenPersonalDictionary();
		int		ClosePersonalDictionary();

		BOOLEAN			IsInitialized( );
		BOOLEAN			IsHyphenation( ) const;

		BOOLEAN			CheckSpelling( const RMBCString& rWord );
		BOOLEAN			GetNextSuggestedSpelling( RMBCString& rSuggestion );

		BOOLEAN			GetHyphenation( const RMBCString& rWord, uLONG ulMap[] );
		void				AddWordToPersonalDict( const RMBCString& rWord );
		static BOOLEAN	FormatWord(_TUCHAR* pFormattedWord, _TUCHAR nLang);

	protected:
		void		GetAltSpellings(_TUCHAR* pWord);
		ICBUFF*	GetICBuff();
		void		ShowAddToDictResults( int nResult );

	// Member vars
	protected:
		BOOLEAN				m_fInitialized;		//Is Correct Spell Initialized
		BOOLEAN				m_fHyphenation;		//Is Hyphenation available
		ICBUFF				m_ICBuff;				//International Correctspell data buffer
		PD_IC_IO				m_PDBuff;				//Personal dictionary data buffer
		PFICPD_RET			m_pPDBuffReturns;
		int					m_nAlternativeCount;	//Count of how many alternatives have been researched for current word
		BOOLEAN				m_fICDBOpen;			//Flag to see if CorrectSpell Database has been opened
		BOOLEAN				m_fPDDBOpen;			//Flag to see if Personal Database has been opened
		EAlternatives		m_eAltAvailable;		//Flag to see if Alternative spellings are available for current word
		RMBCString			m_rCurrentWord;		//Keep track of current word being looked up
		BOOLEAN				m_fSpellChecking;		//Flag to distinguish the last task being CheckSpelling (as opposed to Hyphenate)
		BOOLEAN				m_fAttemptedToInit;	//Flag to see if anyone has tryed to initialize correctspell yet
		TCHAR					m_sTempPath[MAX_PATH + 1];
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_CORRECTSPELL_H_
