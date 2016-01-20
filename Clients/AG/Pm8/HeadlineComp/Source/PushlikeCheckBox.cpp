//****************************************************************************
//
// File Name:		PushlikeCheckBox.cpp
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Manages RPushlikeCheckBox class which uses CButtons to simulate
//				Check Boxes with the "Pushlike" style set (this was necessary 
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

#include "HeadlineIncludes.h"
ASSERTNAME

#include "PushlikeCheckBox.h"

/////////////////////////////////////////////////////////////////////////////
// RPushlikeCheckBox

RPushlikeCheckBox::RPushlikeCheckBox()
{
	m_fChecked = FALSE;

}

RPushlikeCheckBox::~RPushlikeCheckBox()
{
}


BEGIN_MESSAGE_MAP(RPushlikeCheckBox, CBitmapButton)
	//{{AFX_MSG_MAP(RPushlikeCheckBox)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RPushlikeCheckBox message handlers

//*****************************************************************************
// Function Name: RPushlikeCheckBox::SimulateCheck
//
// Description:   Simulates a "push-like" check button being checked by
//				  depressing the button if it is not selected, or releasing
//				  it is selected.
//
// Returns:       VOID
//
// Exceptions:    None
//
//*****************************************************************************
void RPushlikeCheckBox::SimulateCheck()
{
	//
	//Switch check state.
	m_fChecked = !m_fChecked;

	//
	//If the current button is checked, I have to call SetState to keep it
	//depressed.  Otherwise, the default button behavior is to release.
	if ( m_fChecked )
	{
		SetState( TRUE );
	}
}

void RPushlikeCheckBox::OnKillFocus(CWnd* pNewWnd) 
{
	//
	//I have to disable the call to OnKillFocus to keep any depressed buttons
	//from being released when the button group looses focus.
//	CBitmapButton::OnKillFocus(pNewWnd);

	//Get rid of unreferenced formal parameter warning (on warning level 4 )...
	pNewWnd = pNewWnd;
}

