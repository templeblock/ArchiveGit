// ****************************************************************************
//
//  File Name:			SpellCheckDialog.h
//
//  Project:			Renaissance Application
//
//  Author:				Mike Heydlauf
//
//  Description:		Definition of the RSpellCheckDialog class
//
//  Portability:		Windows
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
//  $Logfile:: /PM8/Framework/Include/SpellCheckDialog.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// RSpellCheckDialog dialog
								  
#ifndef _SPELLCHECKDIALOG_H_
#define _SPELLCHECKDIALOG_H_

#include "FrameworkResourceIds.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// Forward declarations
class RSpellChecker;

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************                            ********************************
//*****************     RContextEditCtrl       ********************************
//*****************                            ********************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

class RContextEditCtrl : public CRichEditCtrl
{
// Construction
public:
	RContextEditCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RContextEditCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~RContextEditCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(RContextEditCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point) ;
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//*****************************************************************************
//*****************************************************************************
//*****************                            ********************************
//*****************     RSpellCheckDialog      ********************************
//*****************                            ********************************
//*****************************************************************************
//*****************************************************************************
class FrameworkLinkage RSpellCheckDialog : public CDialog
{
// Construction
public:
	RSpellCheckDialog( RSpellChecker* pSpellChecker, CWnd* pParent = NULL);   // standard constructor

protected:
// Dialog Data
	//{{AFX_DATA(RSpellCheckDialog)
	enum { IDD = DIALOG_SPELL_CHECK };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RSpellCheckDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	RMBCString					m_rMisspelledWord;
	RMBCString					m_rContext;
	RContextEditCtrl			m_cContextCtrl;
	RMBCString					m_rReplaceWith;
	CListBox						m_cAltListBox;
	CEdit							m_cEditReplace;
	RSpellChecker*				m_pSpellChecker;

	void							LoadAlternatives();
	void							ShowMisspelledWord();
	void							ProcessNextMisspelledWord();


	// Generated message map functions
	//{{AFX_MSG(RSpellCheckDialog)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnButtonSpellAddToDictionary();
	afx_msg void OnEditSpellIgnore();
	afx_msg void OnEditSpellReplace();
	afx_msg void OnDblclkListboxSpellAlternatives();
	afx_msg void OnSelchangeListboxSpellAlternatives();
	//afx_msg void OnHelp();
	//afx_msg void OnEditSpellIgnoreAll();
	//afx_msg void OnEditSpellReplaceAll();
	//afx_msg void OnEditSpellSuggest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_SPELLCHECKDIALOG_H_