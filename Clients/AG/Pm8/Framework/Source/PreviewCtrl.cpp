//****************************************************************************
//
// File Name:		RPreviewCtrl.cpp
//
// Project:			Renaissance Component
//
// Author:			Mike Heydlauf
//
// Description:	Methods for RPreviewCtrl. 
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
//  $Logfile:: /PM8/Framework/Source/PreviewCtrl.cpp                   $
//   $Author:: Gbeddow                                                       $
//     $Date:: 3/03/99 6:17p                                                 $
// $Revision:: 1                                                             $
//
//****************************************************************************
#include "FrameworkIncludes.h"

ASSERTNAME

#include "PreviewCtrl.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "BitmapImage.h"
#include "DCDrawingSurface.h"

#ifndef	_WINDOWS
	#error	This file must be compilied only on Windows
#endif	//	_WINDOWS

/////////////////////////////////////////////////////////////////////////////
// RPreviewCtrl

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::RPreviewCtrl
//
// Description:	Constructor
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
RPreviewCtrl::RPreviewCtrl()	: 
	m_cImageRect(0,0,0,0), 
	m_fMirrorImage(FALSE), 
	m_prPreviewBmp(NULL), 
	m_fDrawingDisabled( FALSE ),	
	m_rOffsetSize( 0,0 )
{
	NULL;
}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::~RPreviewCtrl
//
// Description:	Destructor
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
RPreviewCtrl::~RPreviewCtrl()
{
}


BEGIN_MESSAGE_MAP(RPreviewCtrl, CButton)
	//{{AFX_MSG_MAP(RPreviewCtrl)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RPreviewCtrl message handlers



//*****************************************************************************************************
// Function Name:	RPreviewCtrl::EnableDrawing
//
// Description:	allow drawing
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPreviewCtrl::EnableDrawing()
{
	m_fDrawingDisabled = FALSE;
}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::DisableDrawing
//
// Description:	disallow drawing
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPreviewCtrl::DisableDrawing()
{
	m_fDrawingDisabled = TRUE;
}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::DrawCtrl
//
// Description:	Draw preview control
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
//
//virtual
void RPreviewCtrl::DrawCtrl() 
{
	//	We may be asked to draw when we are in the middle of composing the bitmap.
	//	Wait for another draw message.	
	if ( m_fDrawingDisabled )
		return;	

	ASSERT( m_prPreviewBmp );
	CDC* pCtrlDC = GetDC();
	RDcDrawingSurface rDcDrawSurf;

	rDcDrawSurf.Initialize( pCtrlDC->m_hDC, pCtrlDC->m_hDC );

	try
	{
		Render( rDcDrawSurf );
	}	
	catch(...)
	{
		rDcDrawSurf.DetachDCs();
		if ( pCtrlDC ) ReleaseDC( pCtrlDC );
		throw;
	}

	rDcDrawSurf.DetachDCs();
	ReleaseDC( pCtrlDC );
}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::Render
//
// Description:	Draws
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPreviewCtrl::Render( RDrawingSurface& drawingSurface )
	{
	//
	//Get the  control's dimensions...
	CRect cPreviewRect;
	GetClientRect( &cPreviewRect );

	//
	//Create a RIntRect to represent the bitmaps dimensions...
	RIntRect rDestRect;
	rDestRect.m_Left = 0; rDestRect.m_Top = 0; 
	rDestRect.m_Right  = m_prPreviewBmp->GetWidthInPixels();
	rDestRect.m_Bottom = m_prPreviewBmp->GetHeightInPixels();
	//
	//Shrink the the bitmap rect to fit into the display region while
	//maintaining aspect correctness.
	//rDestRect.ShrinkToFit( RIntRect( cPreviewRect ) );
	rDestRect.CenterRectInRect( RIntRect( cPreviewRect ) );

	if ( m_fMirrorImage )
	{
		RIntRect rSourceRect;
		rSourceRect.m_Left = m_prPreviewBmp->GetWidthInPixels(); rSourceRect.m_Top = 0; 
		rSourceRect.m_Right  = 0;
		rSourceRect.m_Bottom = m_prPreviewBmp->GetHeightInPixels();

		m_prPreviewBmp->Render( drawingSurface, rSourceRect, rDestRect );
	}
	else
	{
		m_prPreviewBmp->Render( drawingSurface, rDestRect );
	}

	m_cImageRect.left		= rDestRect.m_Left;
	m_cImageRect.right	= rDestRect.m_Right;
	m_cImageRect.top		= rDestRect.m_Top;
	m_cImageRect.bottom	= rDestRect.m_Bottom;

	m_rOffsetSize.m_dx = rDestRect.m_Left;
	m_rOffsetSize.m_dy = rDestRect.m_Top;
	}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::DrawItem
//
// Description:	Capture message for ownerdraw control
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPreviewCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	switch( lpDrawItemStruct->itemAction )
	{
	case ODA_DRAWENTIRE:
		{
			DrawCtrl();
		}
		break;
	//
	//Do nothing on focus and select so the control does not behave like a button.
	case ODA_FOCUS:
		break;
	case ODA_SELECT:
		break;

	}
}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::SetPreviewBitmap
//
// Description:	Load the bitmap to be displayed in the control
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPreviewCtrl::SetPreviewBitmap( RBitmapImage* prPreviewBmp, BOOL fMirrorImage )
{
	ASSERT(&prPreviewBmp);
	m_prPreviewBmp = prPreviewBmp;
	m_fMirrorImage = fMirrorImage;
	DrawCtrl();
}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::GetPreviewImageRect
//
// Description:	Get the rect of the preview image (not the preview area)
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RPreviewCtrl::GetPreviewImageRect( RECT* PrevRect )
{
	PrevRect->left		= m_cImageRect.left;
	PrevRect->top		= m_cImageRect.top;
	PrevRect->right	= m_cImageRect.right;
	PrevRect->bottom	= m_cImageRect.bottom;

}

//*****************************************************************************************************
// Function Name:	RPreviewCtrl::OnEraseBkgnd
//
// Description:	Erases the backgound. Do nothing
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
afx_msg BOOL RPreviewCtrl::OnEraseBkgnd( CDC*  )
	{
	return TRUE;
	}