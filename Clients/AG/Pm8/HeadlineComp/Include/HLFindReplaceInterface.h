// ****************************************************************************
//
//  File Name:			HLFindReplaceInterface.h
//
//  Project:			Renaissance Headline Component
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RHLFindReplaceInterface class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HLFindReplaceInterface.h              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HLFINDREPLACEINTERFACE_H_
#define		_HLFINDREPLACEINTERFACE_H_

#ifndef	_FindReplaceINTERFACE_H_
#include	"FindReplaceInterface.h"
#endif

//	Forward References
class RHeadlineView;

// ****************************************************************************
//
//  Class Name:	RHLFindReplaceInterface
//
//  Description:	A pure virtual class interfacing with text for spell checking
//
// ****************************************************************************
//
class RHLFindReplaceInterface : public RFindReplaceInterface 
	{
	//	Construction & Destruction
	public :
													RHLFindReplaceInterface( const RComponentView* pView );
		static RInterface*					CreateInterface( const RComponentView* );

	//	Operations
	public :
		virtual void							Startup( );
		virtual void							Shutdown( );
		virtual BOOLEAN						FindNextWord( EFindReplaceFlags flags, RMBCString& word );
		virtual void							GetContext( RMBCString& context, int& nWordStartPos );
		virtual BOOLEAN						ReplaceWord( const RMBCString& word, const RMBCString& replacement );

	//	Members
	private :
		RHeadlineDocument*					m_pDocument;
		RMBCString								m_Text;
		int										m_nWordStart;
		int										m_nWordEnd;
	} ;


#endif	//	_HLFINDREPLACEINTERFACE_H_
