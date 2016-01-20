// ****************************************************************************
//
//  File Name:			SelectGraphicDialog.h
//
//  Project:			Renaissance Graphic Imager
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the CSelectGraphicDialog class
//
//  Portability:		MFC dependent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Include/SelectGraphicDialog.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SELECTGRAPHICDIALOG_H_
#define		_SELECTGRAPHICDIALOG_H_

// ****************************************************************************
//
// Class Name:			CSelectGraphicDialog
//
// Description:		Encapsulates the Select Graphic dialog. This dialog is
//							invoked when the File: Open Graphic menu item is chosen,
//							after a library has been selected.
//
// ****************************************************************************
//
class CSelectGraphicDialog : public CDialog
	{
	// Construction, destruction & initialization
	public :
												CSelectGraphicDialog( char** pGraphicNames, short numGraphics );
	virtual BOOL							OnInitDialog( );

	// Operations
	public :
		void									DoDataExchange( CDataExchange* pDX );

	// Implementation
	private :
		afx_msg void						OnGraphicListBoxDoubleClick( );

	// Member data
	public :
		int									m_nGraphic;

	private :
		CListBox								m_wndGraphicListBox;
		char**								m_pGraphicNames;
		short									m_numGraphics;

	DECLARE_MESSAGE_MAP( )
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif		// _SELECTGRAPHICDIALOG_H_
