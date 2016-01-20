//****************************************************************************
//
// File Name:		SpellChecker.cpp
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
//  $Logfile:: /PM8/Framework/Source/SpellChecker.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//*****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "SpellChecker.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "View.h"
#include "ApplicationGlobals.h"
#include "Alert.h"
#include "SpellCheckInterface.h"
#include "SpellCheckDialog.h"

//*****************************************************************************************************
// Function Name:	RSpellChecker::RSpellChecker
//
// Description:	Constructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RSpellChecker::RSpellChecker( YInterfaceCollection& collection ) :
	m_Collection( collection ),
	m_fStarted( FALSE )
{
	m_Iterator		= collection.Start( );

	// Open the personal dictionary to reload it. We do this on every spell check in case the 
	// personal dictionary is on a network.
	::GetCorrectSpell( ).OpenPersonalDictionary( );
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::~RSpellChecker
//
// Description:	Destructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RSpellChecker::~RSpellChecker() 
{
	// Close the personal dictionary to save it. We do this on every spell check in case the 
	// personal dictionary is on a network.
	::GetCorrectSpell( ).ClosePersonalDictionary( );
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::StartSpellCheck
//
// Description:	Starts to perform the spell checking
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellChecker::StartSpellCheck( )
{
	if ( FindMisspelledWord( ) )
	{
		//	Bring up the dialog and display the misspellings
		RSpellCheckDialog rSpellDlg( this );
		rSpellDlg.DoModal();
	}	

	RAlert( ).InformUser( STRING_MISC_SPELLCHECKDONE );
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::FindMisspelledWord
//
// Description:	Searches through interfaces for a misspelled word
//
// Returns:			TRUE if a misspelled word was found, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN	RSpellChecker::FindMisspelledWord( )
{
	TpsAssert( m_Iterator != m_Collection.End(), "There are no items in the collection" );

	TpsAssertIsObject( RSpellCheckInterface, (*m_Iterator) );
	RSpellCheckInterface*	pInterface	= static_cast<RSpellCheckInterface*>( *m_Iterator );

	if ( !m_fStarted )
		pInterface->Startup( );
	m_fStarted = TRUE;

	RCorrectSpell&	correctSpell	= GetCorrectSpell( );
	BOOLEAN			fContinue		= TRUE;
	while ( fContinue )
	{
		//	If a word was found, check the spelling
		if ( pInterface->FindNextWord( m_MisspelledWord ) )
		{
			//	If it was not spelled correctly, return FALSE
			if ( !correctSpell.CheckSpelling( m_MisspelledWord ) )
				return TRUE;
		}
		else
		{
			//	No word found, go to next interface (if there is one)
			pInterface->Shutdown( );
			++m_Iterator;
			if (m_Iterator == m_Collection.End( ))
				fContinue = FALSE;
			else
			{
				TpsAssertIsObject( RSpellCheckInterface, (*m_Iterator) );
				pInterface	= static_cast<RSpellCheckInterface*>( *m_Iterator );
				pInterface->Startup( );
			}
		}
	}

	//	We are now done...
	m_fStarted = FALSE;
	return FALSE;
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::Replace
//
// Description:	Replaces occurrence of the misspelled word with rReplacement
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellChecker::Replace( const RMBCString& replacement )
{
	TpsAssert( m_fStarted, "How did Replace get called before the SpellChecker was started" );
	TpsAssert( m_Iterator != m_Collection.End(), "There are no items in the collection" );

	TpsAssertIsObject( RSpellCheckInterface, (*m_Iterator) );
	RSpellCheckInterface*	pInterface	= static_cast<RSpellCheckInterface*>( *m_Iterator );
	pInterface->ReplaceWord( m_MisspelledWord, replacement );
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::AddToDictionary
//
// Description:	Adds the supposed Misspelled word to the personal dictionary
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellChecker::AddToDictionary( )
{
	TpsAssert( m_fStarted, "How did AddToDictionary get called before the SpellChecker was started" );

}

//*****************************************************************************************************
// Function Name:	RSpellChecker::Done
//
// Description:	The Spelling Dialog is stopping
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellChecker::Done( )
{
	if ( m_Iterator != m_Collection.End() )
	{
		TpsAssertIsObject( RSpellCheckInterface, (*m_Iterator) );
		RSpellCheckInterface* pInterface	= static_cast<RSpellCheckInterface*>( *m_Iterator );
		pInterface->Shutdown( );
	}
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::GetMisspelledWord
//
// Description:	Return the misspelled word that was found.
//
// Returns:			m_MisspelledWord
//
// Exceptions:		None
//
//*****************************************************************************************************
const RMBCString& RSpellChecker::GetMisspelledWord( ) const
{
	return m_MisspelledWord;
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::GetContext
//
// Description:	Get the context of the misspelled word
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellChecker::GetContext( RMBCString& context, int& nWordStartPos )
{
	TpsAssert( m_fStarted, "How did GetContext get called before the SpellChecker was started" );
	TpsAssert( m_Iterator != m_Collection.End(), "There are no items in the collection" );

	TpsAssertIsObject( RSpellCheckInterface, (*m_Iterator) );
	RSpellCheckInterface*	pInterface	= static_cast<RSpellCheckInterface*>( *m_Iterator );
	pInterface->GetContext( context, nWordStartPos );
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::GetAlternateSpelling
//
// Description:	Get An alternate spelling of the misspelled word
//
// Returns:			TRUE if there are more alternate spellings
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RSpellChecker::GetAlternateSpelling( RMBCString& alternate )
{
	TpsAssert( m_fStarted, "How did GetAlternateSpelling get called before the SpellChecker was started" );
	TpsAssert( m_Iterator != m_Collection.End(), "There are no items in the collection" );

	return GetCorrectSpell( ).GetNextSuggestedSpelling( alternate );
}

//*****************************************************************************************************
// Function Name:	RSpellChecker::GetAlternateSpelling
//
// Description:	Add given word to personal dictionary
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RSpellChecker::AddWordToPersonalDict( RMBCString& rWord )
{
	GetCorrectSpell( ).AddWordToPersonalDict( rWord );
}