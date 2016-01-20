//****************************************************************************
//
// File Name:  AttribTab.h
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Definition of CAttribTab class
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Include/AttribTab.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _ATTRIBTAB_H_
#define _ATTRIBTAB_H_

#ifndef	_COMPDEGREE_H_
#include "CompDegree.h"
#endif

#ifndef	_PUSHLIKERADIOBUTTON_H_
#include "PushlikeRadioButton.h"
#endif

#ifndef	_PUSHLIKECHECKBOX_H_
#include "PushlikeCheckBox.h"
#endif

//#define kDefaultEscapementSel 0
#define kDefaultFontSel       0 
#define kMaxNumAttribBmpBtns  7

#define kBoldBtnBmpU		IDB_STYLE_BOLDU
#define kBoldBtnBmpD		IDB_STYLE_BOLDD
#define kItalicBtnBmpU		IDB_STYLE_ITALICU
#define kItalicBtnBmpD		IDB_STYLE_ITALICD
#define kUnderlineBtnBmpU   IDB_STYLE_UNDERLINEU
#define kUnderlineBtnBmpD   IDB_STYLE_UNDERLINED

#define kLeftJustBtnBmpU     IDB_JUST_LEFTU
#define kLeftJustBtnBmpD     IDB_JUST_LEFTD
#define kRightJustBtnBmpU    IDB_JUST_RIGHTU
#define kRightJustBtnBmpD    IDB_JUST_RIGHTD
#define kCenterJustBtnBmpU   IDB_JUST_CENTERU
#define kCenterJustBtnBmpD   IDB_JUST_CENTERD
#define kFullJustBtnBmpU     IDB_JUST_FULLU
#define kFullJustBtnBmpD     IDB_JUST_FULLD

class REditHeadlineDlg;
class CAttribTab : public CPropertyPage
{
	DECLARE_DYNCREATE(CAttribTab)

// Construction
public:
	CAttribTab();
	CAttribTab(struct AttribData* pAttribData );
	~CAttribTab();
	void FillData( struct AttribData* );
	void ApplyFollowPathModifications( BOOLEAN fApplying, int m_nSelShapeId );
	void SetPointerToOwner( REditHeadlineDlg* pParentPropSheet );
	void SetData( struct AttribData* pAttribData  );
	virtual BOOL OnKillActive();
// Dialog Data
	//{{AFX_DATA(CAttribTab)
	enum { IDD = DIALOG_PROPERTY_PAGE_ATTRIBUTES_TAB };
	RPushlikeCheckBox		m_btnStyleUnderline;
	RPushlikeCheckBox		m_btnStyleItalic;
	RPushlikeCheckBox		m_btnStyleBold;
	RPushlikeRadioButton	m_btnJustRight;
	RPushlikeRadioButton	m_btnJustLeft;
	RPushlikeRadioButton	m_btnJustFull;
	RPushlikeRadioButton	m_btnJustCenter;
	CComboBox	m_ctrlFont;
	CComboBox	m_ctrlEscapement;
	BYTE	m_uwScale1;
	BYTE	m_uwScale2;
	BYTE	m_uwScale3;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAttribTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	RPushlikeRadioButton*	m_pCurrentRadioButton;
	
	ETextJustification		m_eSelJust;
	ETextCompensation			m_eSelComp;
	ETextEscapement			m_eSelEscape;

	//CArray< CBitmap, CBitmap& > m_arrayButtonBmps;
	
	BOOL			m_fBoldChecked ;
	BOOL			m_fItalicChecked ;
	BOOL			m_fUnderlineChecked ;
	BOOL			m_fLeadingChecked ;
	BOOL			m_fKerningChecked ;

	CCompDegree dlgEditComp;
	float		m_flCompDegree;

	CString		m_sFontName;

	void CAttribTab::LoadAttribButtonBmps();
	void CAttribTab::InitializePushlikeButtons();

	REditHeadlineDlg* m_pParentPropSheet;
	// Generated message map functions
	//{{AFX_MSG(CAttribTab)
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply( );
	afx_msg void OnBtnEditDegree();
	afx_msg void OnChkBold();
	afx_msg void OnChkItalic();
	afx_msg void OnChkKerning();
	afx_msg void OnChkLeading();
	afx_msg void OnChkUnderline();
	afx_msg void OnRdJustCenter();
	afx_msg void OnRdJustFull();
	afx_msg void OnRdJustLeft();
	afx_msg void OnRdJustRight();
	afx_msg void OnRdLoose();
	afx_msg void OnRdRegular();
	afx_msg void OnRdTight();
	afx_msg void OnBtnJustCenter();
	afx_msg void OnBtnJustFull();
	afx_msg void OnBtnJustLeft();
	afx_msg void OnBtnJustRight();
	afx_msg void OnBtnStyleBold();
	afx_msg void OnBtnStyleItalic();
	afx_msg void OnBtnStyleUnderline();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif //_ATTRIBTAB_H_
