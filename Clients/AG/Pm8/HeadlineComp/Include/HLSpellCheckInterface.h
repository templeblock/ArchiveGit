// ****************************************************************************
//
//  File Name:			HLSpellCheckInterface.h
//
//  Project:			Renaissance Headline Component
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RHLSpellCheckInterface class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HLSpellCheckInterface.h              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HLSPELLCHECKINTERFACE_H_
#define		_HLSPELLCHECKINTERFACE_H_

#ifndef	_SPELLCHECKINTERFACE_H_
#include	"SpellCheckInterface.h"
#endif

//	Forward References
class RHeadlineView;
class RHeadlineDocument;

// ****************************************************************************
//
//  Class Name:	RHLSpellCheckInterface
//
//  Description:	A pure virtual class interfacing with text for spell checking
//
// ****************************************************************************
//
class RHLSpellCheckInterface : public RSpellCheckInterface 
	{
	//	Construction & Destruction
	public :
													RHLSpellCheckInterface( const RHeadlineView* pView );
		static RInterface*					CreateInterface( const RComponentView* );

	//	Operations
	public :
		virtual void							Startup( );
		virtual void							Shutdown( );
		virtual BOOLEAN						FindNextWord( RMBCString& word );
		virtual void							GetContext( RMBCString& context, int& nWordStartPos );
		virtual BOOLEAN						ReplaceWord( const RMBCString& word, const RMBCString& replacement );

	//	Members
	private :
		RHeadlineDocument*					m_pDocument;
		RMBCString								m_Text;
		int										m_nWordStart;
		int										m_nWordEnd;
	} ;


#endif	//	_HLSPELLCHECKINTERFACE_H_
