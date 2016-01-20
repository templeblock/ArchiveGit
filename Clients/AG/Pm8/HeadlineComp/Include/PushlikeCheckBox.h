//****************************************************************************
//
// File Name:		PushlikeCheckBox.h
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Defines RPushlikeCheckBox class which uses CButtons to simulate
//				Check Boxes with the "Pushlike" style set (this was necessary 
//				for non-Win95 machines (ie. NT 3.5)).
//
// Portability:	Windows Specific
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
//	$Header:: /PM8/HeadlineComp $
//****************************************************************************
#ifndef _PUSHLIKECHECKBOX_H_
#define _PUSHLIKECHECKBOX_H_

class RPushlikeCheckBox : public CBitmapButton
{
// Construction
public:
	RPushlikeCheckBox();
	
	void RPushlikeCheckBox::SimulateCheck();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RPushlikeCheckBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~RPushlikeCheckBox();

	// Generated message map functions
protected:
	BOOL** m_ppCurrentCheckBox;
	BOOL m_fChecked;
	//{{AFX_MSG(RPushlikeCheckBox)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //_PUSHLIKECHECKBOX_H_
/////////////////////////////////////////////////////////////////////////////
