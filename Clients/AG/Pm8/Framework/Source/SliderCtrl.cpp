//****************************************************************************
//
// File Name:		SliderCtrl.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Daniel Selman
//
// Description:	Implementation of RSliderCtrl - overide for CSliderCtrl
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
//  $Logfile:: /PM8/Framework/Source/SliderCtrl.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "SliderCtrl.h"


#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	_WINDOWS
	#error	This file must be compiled only on Windows
#endif	//	_WINDOWS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ****************************************************************************
//
//  Function Name:	RSliderCtrl( )
//
//  Description:		Constructor
//
//  Returns:			N/A
//
//  Exceptions:		None
//
// ****************************************************************************

RSliderCtrl::RSliderCtrl()
{
}

// ****************************************************************************
//
//  Function Name:	RSliderCtrl( )
//
//  Description:		Destructor
//
//  Returns:			N/A
//
//  Exceptions:		None
//
// ****************************************************************************

RSliderCtrl::~RSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(RSliderCtrl, CSliderCtrl)
	//{{AFX_MSG_MAP(RSliderCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RSliderCtrl message handlers

// ****************************************************************************
//
//  Function Name:	OnLButtonDown(...)
//
//  Description:		If the user has clicked outside of the channel area of the
//							slider control, move the slider by one page in the 
//							appropriate direction.
//							Note: supports both Horizontal and Vertical styles.
//
//  Returns:			N/A
//
//  Exceptions:		None
//
// ****************************************************************************

void RSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT( (nFlags & MK_LBUTTON) == MK_LBUTTON);

	DWORD dwStyle = ::GetWindowLong( GetSafeHwnd(), GWL_STYLE );

	CRect thumb;
	GetThumbRect( thumb );

	CRect channel;
	GetChannelRect( channel );

	CRect window;
	GetWindowRect( window );
	::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT) &window, 2 );

	CRect OffsetLeft;
	CRect OffsetRight;

	if ( (dwStyle & TBS_VERT) != TBS_VERT )
	{
		OffsetLeft = CRect( window.left, thumb.top, channel.left + thumb.Height(), thumb.bottom);
		OffsetRight = CRect( channel.right - thumb.Height(), thumb.top, window.right, thumb.bottom);
	}
	else
	{
		OffsetLeft = CRect( thumb.left, window.top, thumb.right, channel.top + thumb.Height() );
		OffsetRight = CRect( thumb.left, channel.bottom - thumb.Height(), thumb.right, window.bottom);
	}

	// just to be safe.
	OffsetLeft.NormalizeRect();
	OffsetRight.NormalizeRect();

	if ( OffsetLeft.PtInRect( point ) )
	{
		// user clicked to the left of the slider,
		// move the thumb one page to the left.
		int nPos = GetPos() - GetPageSize();

		if ( nPos >= GetRangeMin() )
			SetPos( nPos );
		else
			SetPos( GetRangeMin() );
	}
	else if ( OffsetRight.PtInRect( point ) )
	{
		// user clicked to the right of the slider,
		// move the thumb one page to the right.
		int nPos = GetPos() + GetPageSize();

		if ( nPos <= GetRangeMax() )
			SetPos( nPos );
		else
			SetPos( GetRangeMax() );

		SetPos( nPos );
	}
	
	CSliderCtrl::OnLButtonDown(nFlags, point);
}
