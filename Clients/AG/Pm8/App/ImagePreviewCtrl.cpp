//****************************************************************************
//
// File Name:			Preview.cpp
//
// Project:				Image Component
//
// Author:				G. Brown
//
// Description:		Definition of RImagePreviewCtrl class
//
//							NOTE SS_NOTIFY must be set on any controls which use 
//							this class.
//
// Portability:		Windows
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, Ma 02158
//							(617)332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/ImagePreviewCtrl.cpp                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:06p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

//	#ifdef WIN		// old Renaissance flag
#ifdef _WINDOWS

#include "ImagePreviewCtrl.h"
#include "BitmapImage.h"
//#include "VectorImage.h"
#include "ScratchBitmapImage.h"
#include "DcDrawingSurface.h"
#include "OffscreenDrawingSurface.h"
//#include "ImageLibrary.h"
//#include "GDIObjects.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ImageInterface.h"

const UINT kActiveTrackerStyle = CRectTracker::hatchedBorder | CRectTracker::resizeOutside;
const UINT kInactiveTrackerStyle = CRectTracker::solidLine | CRectTracker::resizeOutside;

RImagePreviewCtrl::RImagePreviewCtrl()
{
	m_pImage = NULL;
	m_pComponent = NULL;
	m_fEnableCropping = FALSE;
	m_cRectTracker.m_nStyle = kInactiveTrackerStyle;
	m_rCropArea = kDefaultCropArea; 
}

RImagePreviewCtrl::~RImagePreviewCtrl()
{
}

void RImagePreviewCtrl::SetImageOnly(RImage* pImage, BOOLEAN fResetCropArea)
{		
	// this wil prevent any attempt to render an ImageDocument
	m_pComponent = NULL;

	if (pImage)
	{
		m_pImage = pImage;

		// set up for display
		SetDisplayRect( fResetCropArea );
	}

	// and show it
	// suppress flashes - we refill the background in OnPaint() anyway
//	InvalidateRect(NULL, TRUE);
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}


void RImagePreviewCtrl::SetDocument(RComponentDocument* pComponent, BOOLEAN fResetCropArea)
{		
	if (pComponent)
	{
		m_pComponent = pComponent;

		// get the image
		RImageInterface* pImageInterface;
		pImageInterface = dynamic_cast<RImageInterface*>(static_cast<RComponentView*>(pComponent->GetActiveView())->GetInterface(kImageInterfaceId));
		m_pImage = pImageInterface->GetImage();

		// set up for display
		SetDisplayRect( fResetCropArea );

		// clean up the interface
		delete pImageInterface;
	}

	// and show it
	// suppress flashes - we refill the background in OnPaint() anyway
//	InvalidateRect(NULL, TRUE);
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}


void RImagePreviewCtrl::SetDisplayRect( BOOLEAN fResetCropArea )
{		
	if (m_pImage)
	{
		// Determine the device rect into which to render the image
		CRect cClientRect;
		GetClientRect(&cClientRect);
		
		CSize cDisplaySize;

		// Shrink the client rect to accomodate the crop rect
		cClientRect.DeflateRect(m_cRectTracker.m_nHandleSize, m_cRectTracker.m_nHandleSize);
		
		// Fit the image in the preview window
		RRealSize rImageSize = m_pImage->GetSizeInLogicalUnits();
		YFloatType nARi = rImageSize.m_dx / rImageSize.m_dy;		
		YFloatType nARp = (YFloatType)cClientRect.Width() / (YFloatType)cClientRect.Height();
		if (nARi > nARp)
		{
			cDisplaySize.cx = cClientRect.Width();
			cDisplaySize.cy = (YIntDimension)((YFloatType)cClientRect.Width() / nARi);
		}
		else
		{
			cDisplaySize.cx = (YIntDimension)((YFloatType)cClientRect.Height() * nARi);
			cDisplaySize.cy = cClientRect.Height();
		}

		// If it is a bitmap, we don't want to stretch it
		RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(m_pImage);				
		if (pBitmapImage)
		{
			int sWidth = (int)pBitmapImage->GetWidthInPixels();
			int sHeight = (int)pBitmapImage->GetHeightInPixels();
			if (cDisplaySize.cx > sWidth || cDisplaySize.cy > sHeight)
			{
				cDisplaySize.cx = sWidth;
				cDisplaySize.cy = sHeight;
			}
		}

		// to avoid zero sized objects
		cDisplaySize.cx = max( cDisplaySize.cx, 1 );
		cDisplaySize.cy = max( cDisplaySize.cy, 1 );

		m_cImageRect.left = cClientRect.TopLeft().x + cClientRect.Width() / 2 - cDisplaySize.cx / 2;
		m_cImageRect.top = cClientRect.TopLeft().y + cClientRect.Height() / 2 - cDisplaySize.cy / 2;
		m_cImageRect.right = m_cImageRect.left + cDisplaySize.cx;
		m_cImageRect.bottom = m_cImageRect.top + cDisplaySize.cy;
	}

	if (fResetCropArea) m_rCropArea = kDefaultCropArea;
	UpdateRectTracker();	
}

void RImagePreviewCtrl::EnableCropping(BOOLEAN fEnableCropping)
{
	m_fEnableCropping = fEnableCropping;
}

void RImagePreviewCtrl::SetCropArea(const RRealRect& rCropArea)
{
	m_rCropArea = rCropArea;
	if (m_pImage) UpdateRectTracker();
}

const RRealRect& RImagePreviewCtrl::GetCropArea()
{
	return m_rCropArea;
}

BOOL RImagePreviewCtrl::PreTranslateMessage(MSG* pMsg)
{	
	BOOL fResult = FALSE;
	if (pMsg->hwnd == m_hWnd)
	{			
		if (pMsg->message == WM_KEYDOWN)
		{
			UINT nChar = static_cast<UINT>(pMsg->wParam);
			
			const SHORT kShiftDown = 0x0080;
			const SHORT kShiftLock = 0x0001;
			SHORT nKeyState = ::GetKeyState(VK_SHIFT);
			BOOLEAN fShift = static_cast<BOOLEAN>(nKeyState & kShiftDown);
			
			if ((nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN) && m_fEnableCropping)
			{
				switch(nChar)
				{
				case VK_LEFT:
					if (fShift)
						--m_cRectTracker.m_rect.right;
					else
						m_cRectTracker.m_rect.InflateRect(1, 0, -1, 0);
					break;

				case VK_RIGHT:
					if (fShift)
						++m_cRectTracker.m_rect.right;
					else
						m_cRectTracker.m_rect.InflateRect(-1, 0, 1, 0);
					break;

				case VK_UP:
					if (fShift)
						--m_cRectTracker.m_rect.bottom;
					else
						m_cRectTracker.m_rect.InflateRect(0, 1, 0, -1);
					break;

				case VK_DOWN:
					if (fShift)
						++m_cRectTracker.m_rect.bottom;
					else
						m_cRectTracker.m_rect.InflateRect(0, -1, 0, 1);
					break;
				}

				// Ensure that the rect tracker hass an appropriate size/position
				VerifyRectTracker();

				// Update the crop area
				UpdateCropArea();
				
				// Tell the window to repaint itself
				RedrawWindow();

				fResult = TRUE;
			}		
		}
	}

	// We didn't process the message
	if (!fResult) fResult = CStatic::PreTranslateMessage(pMsg);

	return fResult;
}

BEGIN_MESSAGE_MAP(RImagePreviewCtrl, CStatic)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL RImagePreviewCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_fEnableCropping && m_cRectTracker.SetCursor(this, nHitTest))
	{
		return TRUE;
	}
	else
	{
		CStatic::OnSetCursor(pWnd, nHitTest, message);
		return FALSE;
	}		
}

void RImagePreviewCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_fEnableCropping)
	{
		// Set the focus
		if (GetFocus()->m_hWnd != m_hWnd) SetFocus();

		if (m_cRectTracker.HitTest(point) != CRectTracker::hitNothing && m_cRectTracker.Track(this, point, FALSE, this))
		{
			// Ensure that the rect tracker hass an appropriate size/position
			VerifyRectTracker();

			// Update the crop area
			UpdateCropArea();
			
			// Tell the window to repaint itself
			RedrawWindow();
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void RImagePreviewCtrl::OnSetFocus(CWnd*)
{
	// Change the rect tracker to let the user know that the tracker is active
	if (m_fEnableCropping)
	{
		m_cRectTracker.m_nStyle = m_cRectTracker.m_nStyle = kActiveTrackerStyle;
		RedrawWindow();
	}
}

void RImagePreviewCtrl::OnKillFocus(CWnd*)
{
	// Change the rect tracker to let the user know that the tracker is inactive
	if (m_fEnableCropping)
	{
		m_cRectTracker.m_nStyle = kInactiveTrackerStyle;
		RedrawWindow();
	}
}


void RImagePreviewCtrl::OnPaint()
{	
	// Render the image into the device rect:
	CPaintDC cDC(this);

	// Erase the area behind the image
	CRect cRect;
	GetClientRect(&cRect);
	
	// Initialize the drawing surface
	RDcDrawingSurface rDS;
	if (!rDS.Initialize(cDC.m_hDC, cDC.m_hDC)) return;

	HRGN hClip = NULL;
	ROffscreenDrawingSurface rODS;
	try
	{
		// Create an RBitmapImage compatible with the drawing surface
		RScratchBitmapImage rTempBitmap;
		rTempBitmap.Initialize(rDS, cRect.Width(), cRect.Height());		
		
		// Set the bitmap into an offscreen DS		
		rODS.SetImage(&rTempBitmap);		
				
		// Clear the background
		rODS.SetFillColor(RSolidColor(::GetSysColor(COLOR_3DFACE)));
		rODS.FillRectangle(RIntRect(cRect.left, cRect.top, cRect.right, cRect.bottom));

		// If we have an image, render it
		if (m_pImage)
		{
			TpsAssertValid( m_pImage );

			// Set a clip region around the image rect
			hClip = ::CreateRectRgn(m_cImageRect.left, m_cImageRect.top, m_cImageRect.right, m_cImageRect.bottom);
			if (hClip) VERIFY(::SelectClipRgn((HDC)rODS.GetSurface(), hClip));

			// Draw a white rect in case we have a metafile
//			rODS.SetFillColor(RSolidColor(kWhite));
//			rODS.FillRectangle(RIntRect(m_cImageRect));

			// Render the image
			// if rendering an entire RImage (so we include clipping)
			if (m_pComponent)
			{
				// get the view
				RComponentView *pView = (RComponentView *)m_pComponent->GetActiveView();
				if (pView)
				{
					// create a transform to scale image to the display area
					R2dTransform transform;

					RRealSize rImageSize = m_pImage->GetSizeInLogicalUnits();
					YScaleFactor sX = (YFloatType)m_cImageRect.Width() / (YFloatType)rImageSize.m_dx;		
					YScaleFactor sY = (YFloatType)m_cImageRect.Height() / (YFloatType)rImageSize.m_dy;		

					transform.PreScale( sX, sY );
					transform.PostTranslate( (YRealDimension)m_cImageRect.left, (YRealDimension)m_cImageRect.top );

					pView->Render(rODS, transform, RIntRect(cRect));
				}
			}
			else
			{
				// we have only a bitmap (will not display clipping)
				m_pImage->Render(rODS, RIntRect(m_cImageRect));
			}

			// Draw the tracking rect, if cropping is enabled	
			if (m_fEnableCropping) m_cRectTracker.Draw(CDC::FromHandle((HDC)rODS.GetSurface()));	
		}

		// Clean up the clip region
		if (rODS.GetSurface()) ::SelectClipRgn((HDC)rODS.GetSurface(), NULL);
		if (hClip) VERIFY(::DeleteObject(hClip));
		hClip = NULL;

		// Release the offscreen bitmap
		rODS.ReleaseImage();

		// Render the image
		rTempBitmap.Render(rDS, RIntRect(cRect));
	}
	catch(YException)
	{	
		// Clean up the clip region
		if (rODS.GetSurface()) ::SelectClipRgn((HDC)rODS.GetSurface(), NULL);
		if (hClip) VERIFY(::DeleteObject(hClip));
		hClip = NULL;

		// Release the offscreen bitmap
		rODS.ReleaseImage();

		// For some reason, we couldn't use an offscreen, so just draw directly to the screen			
		// Clear the background
		rDS.SetFillColor(RSolidColor(::GetSysColor(COLOR_3DFACE)));
		rDS.FillRectangle(RIntRect(cRect.left, cRect.top, cRect.right + 1, cRect.bottom + 1));
		if (m_pImage)
		{
			// Render the image
			m_pImage->Render(rDS, RIntRect(m_cImageRect));		

			// Draw the tracking rect, if cropping is enabled	
			if (m_fEnableCropping) m_cRectTracker.Draw(CDC::FromHandle((HDC)rDS.GetSurface()));
		}
	}

	rDS.DetachDCs();	
}


void RImagePreviewCtrl::VerifyRectTracker()
{
	// Compare bounds of m_cRectTracker.m_rect to m_cImageRect and adjust if necessary		
	CSize& cMinSize = m_cRectTracker.m_sizeMin;
	if (m_cRectTracker.m_rect.right < m_cImageRect.left) m_cRectTracker.m_rect.OffsetRect(m_cImageRect.left - m_cRectTracker.m_rect.left, 0);
	if (m_cRectTracker.m_rect.bottom < m_cImageRect.top) m_cRectTracker.m_rect.OffsetRect(0, m_cImageRect.top - m_cRectTracker.m_rect.top);
	if (m_cRectTracker.m_rect.left > m_cImageRect.right) m_cRectTracker.m_rect.OffsetRect(m_cImageRect.right - m_cRectTracker.m_rect.right, 0);
	if (m_cRectTracker.m_rect.top > m_cImageRect.bottom) m_cRectTracker.m_rect.OffsetRect(0, m_cImageRect.bottom - m_cRectTracker.m_rect.bottom);

	if (m_cRectTracker.m_rect.left < m_cImageRect.left)
	{
		m_cRectTracker.m_rect.left = m_cImageRect.left;
		if (m_cRectTracker.m_rect.Width() < cMinSize.cx) m_cRectTracker.m_rect.right = m_cRectTracker.m_rect.left + cMinSize.cx;
	}
	if (m_cRectTracker.m_rect.top < m_cImageRect.top)
	{
		m_cRectTracker.m_rect.top = m_cImageRect.top;
		if (m_cRectTracker.m_rect.Height() < cMinSize.cy) m_cRectTracker.m_rect.bottom = m_cRectTracker.m_rect.top + cMinSize.cy;
	}
	if (m_cRectTracker.m_rect.right > m_cImageRect.right)
	{
		m_cRectTracker.m_rect.right = m_cImageRect.right;
		if (m_cRectTracker.m_rect.Width() < cMinSize.cx) m_cRectTracker.m_rect.left = m_cRectTracker.m_rect.right - cMinSize.cx;
	}
	if (m_cRectTracker.m_rect.bottom > m_cImageRect.bottom)
	{
		m_cRectTracker.m_rect.bottom = m_cImageRect.bottom;
		if (m_cRectTracker.m_rect.Height() < cMinSize.cy) m_cRectTracker.m_rect.top = m_cRectTracker.m_rect.bottom - cMinSize.cy;
	}				
}

void RImagePreviewCtrl::UpdateRectTracker()
{
	TpsAssert(m_pImage, "m_pImage is NULL.");
	m_cRectTracker.m_rect.left = m_cImageRect.left + (LONG)((YRealDimension)m_cImageRect.Width() * m_rCropArea.m_Left);
	m_cRectTracker.m_rect.top = m_cImageRect.top + (LONG)((YRealDimension)m_cImageRect.Height() * m_rCropArea.m_Top);
	m_cRectTracker.m_rect.right = m_cRectTracker.m_rect.left + (LONG)((YRealDimension)m_cImageRect.Width() * m_rCropArea.m_Right);
	m_cRectTracker.m_rect.bottom = m_cRectTracker.m_rect.top + (LONG)((YRealDimension)m_cImageRect.Height() * m_rCropArea.m_Bottom);
}

void RImagePreviewCtrl::UpdateCropArea()
{
	TpsAssert(m_pImage, "m_pImage is NULL.");
	m_rCropArea.m_Left = (YRealDimension)(m_cRectTracker.m_rect.left - m_cImageRect.left) / (YRealDimension)m_cImageRect.Width();
	m_rCropArea.m_Top = (YRealDimension)(m_cRectTracker.m_rect.top - m_cImageRect.top) / (YRealDimension)m_cImageRect.Height();
	m_rCropArea.m_Right = (YRealDimension)m_cRectTracker.m_rect.Width() / (YRealDimension)m_cImageRect.Width();
	m_rCropArea.m_Bottom = (YRealDimension)m_cRectTracker.m_rect.Height() / (YRealDimension)m_cImageRect.Height();
}

#endif //WIN
