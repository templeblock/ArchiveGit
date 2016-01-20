//****************************************************************************
//
// File Name:		PushlikeRadioButton.cpp
//
// Project:			Renaissance application framework
//
// Author:			Mike Heydlauf
//
// Description:	Manages RPushlikeRadioButton class which uses CButtons to simulate
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

#include "HeadlineIncludes.h"
ASSERTNAME

#include "PushlikeRadioButton.h"

/////////////////////////////////////////////////////////////////////////////
// RPushlikeRadioButton

RPushlikeRadioButton::RPushlikeRadioButton()
{
}

//*****************************************************************************
// Function Name: RPushlikeRadioButton::RPushlikeRadioButton
//
// Description:   Constructor which accepts a pointer to a RPushlikeRadioButton
//				  pointer.  This pointer is used to maintain which button is
//				  currently selected.
//
// Returns:       Nothing
//
// Exceptions:    None
//
//*****************************************************************************
RPushlikeRadioButton::RPushlikeRadioButton( RPushlikeRadioButton** ppCurrentRadioButton )
{
	m_ppCurrentRadioButton = ppCurrentRadioButton;
	m_fMouseButtonDown = FALSE;
}

RPushlikeRadioButton::~RPushlikeRadioButton()
{
}


BEGIN_MESSAGE_MAP(RPushlikeRadioButton, CBitmapButton)
	//{{AFX_MSG_MAP(RPushlikeRadioButton)
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RPushlikeRadioButton message handlers

//*****************************************************************************
// Function Name: RPushlikeRadioButton::SimulateRadioClick
//
// Description:   Simulates a radio button being clicked.  The function
//				  depresses the button clicked, and releases the previously
//				  selected button.
//
// Returns:       VOID
//
// Exceptions:    None
//
//*****************************************************************************
void RPushlikeRadioButton::SimulateRadioClick() 
{
	//
	//if *m_ppCurrentRadioButton == NULL  then this is the first time
	//SimulateRadioClick has been called for this group of buttons, so I set
	//this instance of the button as the current selection...
	if ( *m_ppCurrentRadioButton == NULL )
	{
		*m_ppCurrentRadioButton = this ;
		SetState( TRUE );
	}

	//
	//If a button other than the currently selected one has been clicked
	//release the old selection, and depress the current selection..
	if ( this != NULL && *m_ppCurrentRadioButton != NULL && this != *m_ppCurrentRadioButton )
	{
		(*m_ppCurrentRadioButton)->SetState( FALSE );
		SetState( TRUE );
		*m_ppCurrentRadioButton = this;
	}
	else if ( this == *m_ppCurrentRadioButton )
	{
		//
		//The currently selected button has been clicked, so I have to 
		//call "SetState( TRUE )" to force it to stay depressed.  Otherwise
		//the default button behavior would be to release.
		SetState( TRUE );
	}
	
}

void RPushlikeRadioButton::OnKillFocus(CWnd* pNewWnd) 
{
	//
	//I have to disable the call to OnKillFocus to keep any depressed buttons
	//from being released when the button group looses focus...
	//	CBitmapButton::OnKillFocus(pNewWnd);

	//Get rid of unreferenced formal parameter warning (on warning level 4 )...
	pNewWnd = pNewWnd;
}

void RPushlikeRadioButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_fMouseButtonDown = TRUE;
	CBitmapButton::OnLButtonDown(nFlags, point);
}

void RPushlikeRadioButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_fMouseButtonDown = FALSE;
	CBitmapButton::OnLButtonUp(nFlags, point);
}

void RPushlikeRadioButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	//If the mouse button is down, I need to disable the call to
	//"CBitmapButton::OnMouseMove" to prevent the button from popping up
	//when the user drags off of the button.
	if ( !m_fMouseButtonDown )
	{
		CBitmapButton::OnMouseMove(nFlags, point);
	}
}
