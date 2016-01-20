//****************************************************************************
//
// File Name:		PushlikeRadioButton.h
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Defines RPushlikeRadioButton class which uses CButtons to simulate
//				Radio Buttons with the "Pushlike" style set (this was necessary 
//				for non-Win95 machines).
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
#ifndef _PUSHLIKERADIOBUTTON_H_
#define _PUSHLIKERADIOBUTTON_H_

class RPushlikeRadioButton : public CBitmapButton
{
// Construction
public:
	RPushlikeRadioButton();
	RPushlikeRadioButton( RPushlikeRadioButton** ppCurrentRadioButton );

// Attributes
public:

// Operations
public:
	void RPushlikeRadioButton::SimulateRadioClick();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RPushlikeRadioButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~RPushlikeRadioButton();

	// Generated message map functions
protected:
	RPushlikeRadioButton** m_ppCurrentRadioButton;
	BOOL   m_fMouseButtonDown;
	//{{AFX_MSG(RPushlikeRadioButton)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
#endif //_PUSHLIKERADIOBUTTON_H_
/////////////////////////////////////////////////////////////////////////////
