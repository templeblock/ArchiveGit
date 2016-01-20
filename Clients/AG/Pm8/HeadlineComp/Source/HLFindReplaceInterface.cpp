//****************************************************************************
//
// File Name:		HLFindReplaceInterface.cpp
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
//  $Logfile:: /PM8/HeadlineComp/Source/HLFindReplaceInterface.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "HeadlineIncludes.h"

ASSERTNAME

#include "HeadlineView.h"
#include "HLFindReplaceInterface.h"
// #include "RenaissanceResource.h"

// ****************************************************************************
//
//  Function Name:	RHLFindReplaceInterface::RHLFindReplaceInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RHLFindReplaceInterface::RHLFindReplaceInterface( const RComponentView* pView )
	: RFindReplaceInterface( pView ),
	  m_pDocument( NULL )
{
	TpsAssertIsObject( RHeadlineView, pView );
	m_pDocument	= (static_cast< RHeadlineView* >( const_cast<RComponentView*>(pView) ))->GetHeadlineDocument( );
}

// ****************************************************************************
//
//  Function Name:	RHLFindReplaceInterface::RHLFindReplaceInterface()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RInterface* RHLFindReplaceInterface::CreateInterface( const RComponentView* pView )
{
	return new RHLFindReplaceInterface( pView );
}

// ****************************************************************************
//
//  Function Name:	RHLFindReplaceInterface::Startup()
//
//  Description:		Prepare to Spell Check this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RHLFindReplaceInterface::Startup( )
{
	RHeadlinePersistantObject&	headlineObject	= m_pDocument->HeadlineObject( );
	headlineObject.GetText( m_Text );
	m_nWordStart	= 0;
	m_nWordEnd		= 0;
}

// ****************************************************************************
//
//  Function Name:	RHLFindReplaceInterface::Shutdown()
//
//  Description:		Done spell checking this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RHLFindReplaceInterface::Shutdown( )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RHLFindReplaceInterface::FindNextWord()
//
//  Description:		Return the next word in the component
//
//  Returns:			TRUE if word found, FALSE if no more words
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RHLFindReplaceInterface::FindNextWord( EFindReplaceFlags eFlags, RMBCString& findWord )
{
	//	Since this function is not speed critical as yet, I am not going to put
	//		in a fast find function.
	//		This can be done if a) time permits or b) profile says so...

	//	Get iterators for the data in the headline
	RMBCStringIterator	scanIterStart	= m_Text.Start( );
	RMBCStringIterator	scanIterEnd		= m_Text.End( );
	RMBCStringIterator	findIterStart	= findWord.Start( );
	RMBCStringIterator	findIterEnd		= findWord.End( );

	//	Advance to current word position
	scanIterStart	+= m_nWordEnd;
	//	word end is word start
	m_nWordStart	= m_nWordEnd;

	RMBCStringIterator	scanIter	= scanIterStart;
	RMBCStringIterator	findIter	= findIterStart;

	//	Search until I get a hit or reach the end
	while ( scanIter != scanIterEnd )
	{
		//	Retrieve the characters to scan for
		RCharacter	scanChar	= MapCharacterToSearchFor( scanIter, eFlags );
		RCharacter	findChar	= MapCharacterToSearchFor( findIter, eFlags );

		//	If the first letter matches, 
		if ( findChar == scanChar )
		{
			//	Advance the points
			++findIter;
			++scanIter;
			//	If I am at the end of the find string, I may be done....
			if ( findIter == findIterEnd )
			{
				//	If not whole word searching, we are done.  Otherwise, check that the
				//		next character is either the end OR a space
				if ( !(eFlags&kFindWholeWord) )
					goto foundMatch;
				if ( scanIter == scanIterEnd )
					goto foundMatch;
				else if ( (*scanIter).IsSpace() )
					goto foundMatch;
				//
				//	Word found, but not the whole word fall out...
				else
				{
					//	Reset string iterators, advance word start,
					//		and prepare to start searching again
					scanIter = ++scanIterStart;
					findIter	= findIterStart;
					++m_nWordStart;
				}
			}
		}
		else
		{
			//	No match
			//	Reset string iterators, advance word start,
			//		and prepare to start searching again
			scanIter = ++scanIterStart;
			findIter	= findIterStart;
			++m_nWordStart;
		}
	}

	return FALSE;

foundMatch:

	m_nWordEnd	= m_nWordStart	+ findWord.GetStringLength( );
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RHLFindReplaceInterface::RTextFindReplaceInterface()
//
//  Description:		Replace the current word with the given replacement
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RHLFindReplaceInterface::ReplaceWord( const RMBCString& original, const RMBCString& replacement )
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

	//
	//	This does not use an action to be able to set the text....
	//		If we need to use actions here, just Add a flag and 
	//		use m_pDocument->GetHeadlineText( newText )
	m_pDocument->HeadlineObject().SetText( newText, TRUE );
	m_pDocument->GetActiveView()->Invalidate( TRUE );
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
//  Function Name:	RHLFindReplaceInterface::GetContext()
//
//  Description:		Return the context around the last word returned
//
//  Returns:			Nothing
//
//  Exceptions:		None
//																						  
// ****************************************************************************
void RHLFindReplaceInterface::GetContext( RMBCString& context, int& nWordStart )
{
	//	Empty the context
	context.Empty( );

	RMBCStringIterator	iterator		= m_Text.Start( );
	RMBCStringIterator	iteratorEnd	= m_Text.End( );

	nWordStart			= m_nWordStart;
	int nStartContext	= (m_nWordStart>kFindReplaceContextSize/2)? m_nWordStart-(kFindReplaceContextSize/2) : 0;
	iterator	+= nStartContext;

	//	Fill in at least kContextSize characters
	int	nLength = 0;
	while ( iterator != iteratorEnd )
	{
		context += (*iterator);
		++iterator;
		if ( ++nLength == kFindReplaceContextSize )
			break;
	}
}
