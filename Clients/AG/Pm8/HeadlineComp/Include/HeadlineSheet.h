//****************************************************************************
//
// File Name:  HeadlineSheet.h
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description:	Definition of CHeadlineSheet
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineSheet.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _HEADLINESHEET_H_
#define _HEADLINESHEET_H_

class RHeadlineDocument;

const int kTextCtrlId        = 732; //No real reason for this particular no.
const int kTextCaptionCtrlId = 733;

#define kHeadlineCaption  "Headline Text:"
#define kHeadlineDlgTitle "Headline"

class REditHeadlineDlg;
class CHeadlineSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CHeadlineSheet)

// Construction
 public:
	CHeadlineSheet( CString* psText );
	void FillData( CString* pGetText );
	void GetPreviewRect( RRealRect* rPrevRect );
	void SetPointerToOwner( REditHeadlineDlg* pParentPropSheet );

	//{{AFX_DATA(CShapeTab)
	enum { IDD =  kTextCtrlId };
	//}}AFX_DATA

// Data
private :
	RHeadlineDocument* m_pPreviewHeadline;

// Attributes
public:

// Operations
public:
	void UpdatePreviewHeadline( HeadlineDataStruct* pData );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeadlineSheet)
	public:
	virtual BOOL OnInitDialog();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHeadlineSheet();

	// Generated message map functions
protected:
	CEdit	m_ctrlText;
	CStatic m_ctrlTextCaption;
	CString m_sHeadlineText;

	CRect m_cPrevRect;

	REditHeadlineDlg* m_pParentPropSheet;
	//{{AFX_MSG(CHeadlineSheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint( );
	//}}AFX_MSG
	afx_msg void OnTextEditCtrlUpdate();
	DECLARE_MESSAGE_MAP()
};
#endif //_HEADLINESHEET_H_
/////////////////////////////////////////////////////////////////////////////
