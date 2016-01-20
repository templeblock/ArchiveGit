//****************************************************************************
//
// File Name:  InsertSymbolDialog.h
//
// Project:    Renaissance Text Component
//
// Author:     Park Row -this file was yanked directly out of BroderBund's ParkRow 
//								 source code at their request.  The only things that have 
//								 changed are resource names and the file name (formerly InsertSymbol.h). -MWH
//
// Description: Definition of Insert symbol dialog Class
//
// Portability: Windows Specific
//
// Adapted by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/InsertSymbolDialog.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
/////////////////////////////////////////////////////////////////////////////
// InsertSymbol.H : Header file
//

/////////////////////////////////////////////////////////////////////////////
// CBInsertSymbol dialog


#ifndef _INSERTSYMBOLDIALOG_H
#define _INSERTSYMBOLDIALOG_H

#include "resource.h"

//#include "controls.h"
#include "SymbolGridCtrl.h"


class CBInsertSymbol : public CDialog
{
    unsigned char     m_curSymbolChar;
    CString m_fontName;

    CRect   m_previewRect;
    CFont   m_previewFont;

// Construction
public:
	CBInsertSymbol(CWnd* pParent = NULL, unsigned char c = 32, int titleID = IDS_INSERT_SYMBOL);

// Dialog Data
	//{{AFX_DATA(CBInsertSymbol)
	enum { IDD = DIALOG_INSERT_SYMBOL };
//	CBMagicStatic	m_sidebar;
//	CStatic			m_sidebar;
//	CBitmap			m_sidebarbmp;
	RSymbolGridCtrl	m_symbolGridCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBInsertSymbol)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation


protected:

	// Generated message map functions
	//{{AFX_MSG(CBInsertSymbol)
	virtual BOOL OnInitDialog();
	afx_msg void OnInsertSymbol();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg LRESULT	OnSymbolChange( WPARAM wParam, LPARAM lParam ) ;
	//afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

    CString			GetFont()   { return m_fontName;   }
    unsigned char	GetChar()  { return m_curSymbolChar; }

private:
	int			m_nTitleID;
};


#endif
