// ****************************************************************************
//
//  File Name:			Mainframe.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMainframe class
//
//  Portability:		Windows (MFC) Specific
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
//  $Logfile:: /PM8/Framework/Include/Mainframe.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MAINFRAME_H_
#define		_MAINFRAME_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RStandaloneDocument;
class RView;
class BTwainWrapper;

// ****************************************************************************
//
//  Class Name:		RMainframe
//
//  Description:		The base frame class. This is the MDI frame which
//							contains the documents, toolbar, and status bar. This class
//							in unnecessary on the Mac.
//
// ****************************************************************************
//
class FrameworkLinkage RMainframe : public CMDIFrameWnd
	{
	// Construction & Destruction
	public:
												RMainframe( );
		virtual								~RMainframe( );

	// Operations
	public :
		virtual								BOOL PreTranslateMessage(MSG* pMsg);
		void									ForceIdle( );
		virtual void						OnIdle( );
		RStandaloneDocument*				GetActiveStandaloneDocument( ) const;

	// Implementation
	protected :
		afx_msg int							OnCreate( LPCREATESTRUCT lpCreateStruct );
		afx_msg void						OnMfcCommand( UINT nID );
		afx_msg BOOL						OnCommand( WPARAM wParam, LPARAM lParam );
		afx_msg LRESULT					OnSetText(WPARAM wParam, LPARAM lParam);
		afx_msg void						OnWinIniChange( LPCTSTR lpszSection );
		afx_msg void						OnDevModeChange( LPTSTR lpDeviceName );
		afx_msg void						OnActivateApp( BOOL bActive, HTASK hTask );
		afx_msg void						OnInitMenuPopup( CMenu* pMenu, UINT nIndex , BOOL bSysMenu );
		afx_msg void						OnUpdateAcquire(CCmdUI* pCmdUI);
		afx_msg void						OnAcquire();
		afx_msg void						OnUpdateSelectSource(CCmdUI* pCmdUI);
		afx_msg void						OnSelectSource();
		afx_msg void						OnDestroy();
		afx_msg LONG						OnBTwainImageXfer(  WPARAM wParam, LPARAM lParam );

	// Members
	protected :
		BTwainWrapper*						m_pTwainHandler;

	DECLARE_DYNAMIC( RMainframe )
	DECLARE_MESSAGE_MAP( )
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _RENAISSANCEMAINFRAME_H_
