// ****************************************************************************
//
//  File Name:			ThesaurusDialog.h
//
//  Project:			Renaissance Application
//
//  Author:				Mike Heydlauf
//
//  Description:		Definition of the RThesaurusDialog class
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
//  $Logfile:: /PM8/Framework/Include/ThesaurusDialog.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// RThesaurusDialog dialog
								  
#ifndef _THESAURUSDIALOG_H_
#define _THESAURUSDIALOG_H_

#include "FrameworkResourceIds.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// Forward declarations

class FrameworkLinkage RThesaurusDialog : public CDialog
{
// Construction
public:
	RThesaurusDialog( RMBCString& rLookupWord, CWnd* pParent = NULL);   // standard constructor

	void							FillData( RMBCString& rString );

protected:
// Dialog Data
	//{{AFX_DATA(RThesaurusDialog)
	enum { IDD = DIALOG_THESAURUS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RThesaurusDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	RMBCString					m_rLookupWord;
	RMBCString					m_rDefinition;
	CListBox						m_cSynonymListBox;
	CListBox						m_cDefinitionListBox;

	void							LookupWord();
	void							LoadSynonyms();
	void							LoadDefinitions();
	void							MaintainHorizScrollLenForListBox( int& nCurrMaxLen, CListBox& cLBox, const RMBCString& rNewString );

	// Generated message map functions
	//{{AFX_MSG(RThesaurusDialog)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnEditThesaurusReplace();
	afx_msg void OnEditThesaurusDefine();
	afx_msg void OnDblclkListboxSpellSynonyms();
	afx_msg void OnSelchangeListboxSynonyms();
	//afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_THESAURUSDIALOG_H_