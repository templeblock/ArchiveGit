//****************************************************************************
//
// File Name:		HLSpellCheckInterface.cpp
//
// Project:			Renaissance Headline Component
//
// Author:			Mike Houle
//
// Description:	Manages Headlines Spell Check Interface.
//
// Portability:	Platform Independant
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
//  $Logfile:: /PM8/HeadlineComp/Source/HLSpellCheckInterface.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "HeadlineIncludes.h"

ASSERTNAME

#include "HeadlineView.h"
#include "HeadlineDocument.h"
#include "HLSpellCheckInterface.h"
// #include "RenaissanceResource.h"

// ****************************************************************************
//
//  Function Name:	RCharacterSettingsInterface::RCharacterSettingsInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RHLSpellCheckInterface::RHLSpellCheckInterface( const RHeadlineView* pView )
	: RSpellCheckInterface( pView ),
	  m_pDocument( pView->GetHeadlineDocument( ) )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RTextSpellCheckInterface::RTextSpellCheckInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RInterface* RHLSpellCheckInterface::CreateInterface( const RComponentView* pView )
{
	return new RHLSpellCheckInterface( static_cast< const RHeadlineView* >( pView ) );
}

// ****************************************************************************
//
//  Function Name:	RHLSpellCheckInterface::Startup()
//
//  Description:		Prepare to Spell Check this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RHLSpellCheckInterface::Startup( )
{
	RHeadlinePersistantObject&	headlineObject	= m_pDocument->HeadlineObject( );
	headlineObject.GetText( m_Text );
	m_nWordStart	= 0;
	m_nWordEnd		= 0;
}

// ****************************************************************************
//
//  Function Name:	RHLSpellCheckInterface::Shutdown()
//
//  Description:		Done spell checking this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RHLSpellCheckInterface::Shutdown( )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RHLSpellCheckInterface::FindNextWord()
//
//  Description:		Return the next word in the component
//
//  Returns:			TRUE if word found, FALSE if no more words
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RHLSpellCheckInterface::FindNextWord( RMBCString& word )
{
	//	Empty the word
	word.Empty( );

	RMBCStringIterator	iterator		= m_Text.Start( );
	RMBCStringIterator	iteratorEnd	= m_Text.End( );

	//	Advance to current word position
	iterator			+= m_nWordEnd;
	m_nWordStart	= m_nWordEnd;

	//	Try to find a word start.
	while ( iterator != iteratorEnd )
	{
		if ( !(*iterator).IsSpace( ) )
			break;
		++m_nWordStart;
		++iterator;
	}

	if ( iterator == iteratorEnd )
		return FALSE;

	//	Find the word end
	m_nWordEnd	= m_nWordStart;
	while ( iterator != iteratorEnd )
	{
		++m_nWordEnd;
		if ( (*iterator).IsSpace( ) )
			break;
		word	+=	(*iterator);
		++iterator;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RHLSpellCheckInterface::RHLSpellCheckInterface()
//
//  Description:		Replace the current word with the given replacement
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RHLSpellCheckInterface::ReplaceWord( const RMBCString& original, const RMBCString& replacement )
{
	RMBCString				newText;
	RMBCStringIterator	iterator		= m_Text.Start( );
	RMBCStringIterator	iteratorEnd	= m_Text.End( );

	//	Empty the new string ( just to be paranoid );
	newText.Empty( );

	//	Copy the text before the word
	int	nLength = 0;
	while ( nLength++ < m_nWordStart )
	{
		newText	+= (*iterator);
		++iterator;
	}

	//	add the replacement word
	newText	+= replacement;
	//	skip over the original word
	iterator	+= original.GetStringLength( );

	//	add the remaining string
	while ( iterator != iteratorEnd )
	{
		newText	+= (*iterator);
		++iterator;
	}

	m_pDocument->SetHeadlineText( newText );
	m_Text			=	newText;
	m_nWordEnd		=	m_nWordStart + replacement.GetStringLength();

	//	Invalidate the render cache
	TpsAssertIsObject( RComponentView, m_pDocument->GetActiveView( ) );
	RComponentView* pView = static_cast< RComponentView* >( m_pDocument->GetActiveView( ) );
	pView->InvalidateRenderCache( );
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RHLSpellCheckInterface::GetContext()
//
//  Description:		Return the context around the last word returned
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RHLSpellCheckInterface::GetContext( RMBCString& context, int& nWordStart )
{
	//	Empty the context
	context.Empty( );

	RMBCStringIterator	iterator		= m_Text.Start( );
	RMBCStringIterator	iteratorEnd	= m_Text.End( );

	nWordStart			= m_nWordStart;
	int nStartContext	= (m_nWordStart>kContextSize/2)? m_nWordStart-(kContextSize/2) : 0;
	iterator	+= nStartContext;

	//	Fill in at least kContextSize characters
	int	nLength = 0;
	while ( iterator != iteratorEnd )
	{
		context += (*iterator);
		++iterator;
		if ( ++nLength == kContextSize )
			break;
	}
}
