/*
// $Workfile: CORNMENT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CORNMENT.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:35:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:42   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:35:52   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:53:30   FRED
//  
// 
//    Rev 1.5   28 Oct 1994 16:30:30   FRED
// ResetSelf() and working CInterfaceObjects
// 
//    Rev 1.4   13 Oct 1994 14:18:32   JAY
// Renamed some update/commit stuff
// 
//    Rev 1.3   12 Oct 1994 13:00:30   JAY
// Integration of Commit and Update at CPropertyObject level
// 
//    Rev 1.2   19 Sep 1994 15:48:12   FRED
// Added some checks to prevent divide-by-zero
// and other errors.
// 
// 
//    Rev 1.1   19 Sep 1994 11:53:58   FRED
// Some enhancements.
// 
// 
//    Rev 1.0   16 Sep 1994 10:48:38   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "cornment.h"
#include "cdrawcon.h"


inline int Max(int a, int b) { return (a >= b) ? a : b; }


// Properties

BEGIN_OBJECT_PROPERTIES(COrnamentState)
	OBJECT_PROPERTY(Style)
	OBJECT_PROPERTY(Position)
	OBJECT_PROPERTY(Origin)
	OBJECT_PROPERTY(Offset)
	OBJECT_PROPERTY(Extent)
	OBJECT_PROPERTY(Aspect)
	OBJECT_PROPERTY(Bounds)
	OBJECT_PROPERTY(Visible)
END_OBJECT_PROPERTIES()

	
// Construct a COrnament object.

COrnament::COrnament()
{
	m_pContext = NULL;
	m_pMaskState = NULL;
	m_pImageState = NULL;
}


// Destroy a COrnament object.

COrnament::~COrnament()
{
	ResetSelf();
} 


// Initialize the COrnament object to a default state.

void COrnament::Init(CDrawContext* pContext, BOOL fReset /*=TRUE*/)
{
	ResetSelf();
	CPropertyObject::Init(fReset);
	
	// Save pointer to the window we are attached to.
	
	ASSERT(pContext != NULL);
	
	m_pContext = pContext;
}				

void COrnament::Reset(void)
{
	ResetSelf();
	CPropertyObject::Reset();
}

void COrnament::ResetSelf(void)
{
	ResetBitmaps();
}

void COrnament::ResetBitmaps(void)
{
	m_bmMask.DeleteObject();
	delete m_pMaskState;
	m_pMaskState = NULL;
	
	m_bmImage.DeleteObject();
	delete m_pImageState;
	m_pImageState = NULL;
}

// Compute the dimensions of the label given the current parameters.
// The content object is called to size its content to the content
// rectangle for the standard label dimensions.

BOOL COrnament::UpdateSelf(void)
{
	ASSERT(m_pContext != NULL);

	if (UpdateNeeded())
	{
		// Size the ornament base on position and extent.
			
		// NOTE: The Bounds property is not up to date at this
		// point and should not be used is the SizeContent() method.
		// Furthermore, SizeContent() should not set the Bounds
		// rectangle. Its definition is fixed: Extent+Position-Origin.
		// The bounds rectangle is used as the position where the
		// actual image will be drawn.
			
		SizeContent();
			
		// Update the bounds from the position and extent.
			
		CurrentState()->m_crBounds = CRect(0, 0, CurrentState()->m_czExtent.cx, CurrentState()->m_czExtent.cx)+CurrentState()->m_cpPosition-CurrentState()->m_cpOrigin;
		
		// The size information is now valid for the current properties.
	}
	return TRUE;
}

// Accept the current size and properties.

BOOL COrnament::CommitSelf(void)
{
	DoUpdate();
	return TRUE;
}


// Draw the ornament into a DC at its current size, position, and state.
// The GetMask() and GetImage() member function are called to retireve
// the bitmaps for the label mask and image.

void COrnament::Draw(void)
{
	ASSERT(m_pContext != NULL);
	ASSERT(!CommitNeeded());

	// Only draw the object if it's visible.
		
	if (CurrentState()->m_fVisible)
	{
		// Make sure the object has been sized.
		
		CDC* pDC = m_pContext->GetDC();
	
		ASSERT(pDC != NULL);
					
		if (pDC != NULL)
		{
			// Save the state of the DC.
					
			int nContext;
					
			if ((nContext = pDC->SaveDC()) != 0)
			{
				// Set MM_TEXT for all the creation and drawing functions.
					
				pDC->SetMapMode(MM_TEXT);
					
				// Get the mask bitmap.
					
				CBitmap* pbmMask;
					
				if ((pbmMask = GetMask()) != NULL)
				{
					CDC MaskDC;
		
					// Create a compatible memory DC and select the mask bitmap into it.
			
					if (MaskDC.CreateCompatibleDC(pDC))
					{
						MaskDC.SetMapMode(MM_TEXT);
										
						CBitmap* pbmOldMask;
				
						if ((pbmOldMask = MaskDC.SelectObject(pbmMask)) != NULL)
						{
							// Get the image bitmap.
								
							CBitmap* pbmImage;
								
							if ((pbmImage = GetImage()) != NULL)
							{
								// Create another compatible memory DC for the image.
					
								CDC ImageDC;
					
								if (ImageDC.CreateCompatibleDC(pDC))
								{
									ImageDC.SetMapMode(MM_TEXT);
										
									CBitmap* pbmOldImage;
										
									if ((pbmOldImage = ImageDC.SelectObject(pbmImage)) != NULL)
									{
										pDC->SetTextColor(RGB(0,0,0));
										pDC->SetBkColor(RGB(255,255,255));
											
										// Compute the drawing position.
											
										CRect crBounds = CurrentState()->m_crBounds;
				
										// Mask out the area where we will be drawing the image.
						
										m_pContext->Toggle(TRUE, &crBounds);
										
										pDC->BitBlt(
											crBounds.left,
											crBounds.top,
											crBounds.Width(),
											crBounds.Height(),
											&MaskDC,
											0,
											0,
											SRCAND);
												
										// Draw the image into the area that was masked out.
						
										pDC->BitBlt(
											crBounds.left,
											crBounds.top,
											crBounds.Width(),
											crBounds.Height(),
											&ImageDC,
											0,
											0,
											SRCPAINT);
								
										m_pContext->Toggle(FALSE, &crBounds);
										ImageDC.SelectObject(pbmOldImage);
									}
								}
							}
								
							MaskDC.SelectObject(pbmOldMask);
						}
					}
				}
						
				pDC->RestoreDC(nContext);
			}
		}
	
		m_pContext->ReleaseDC();
	}
}


// Get the mask for the label shape. The image mask is rebuilt
// from scratch and cached if appropriate.

CBitmap* COrnament::GetMask(void)
{
	CDC *pDC	= m_pContext->GetDC();
	ASSERT(pDC != NULL);

	// If the mask bitmap is not valid give the current state, free the
	// current bitmap and build the new one.
	
	if (MaskNeeded())
	{
		m_bmMask.DeleteObject();
	}

	// Check if we need to build a new bitmap.		
	
	if (m_bmMask.GetSafeHandle() == NULL)
	{
		// Create the bitmap for the mask.

		CRect crMask(0, 0, Max(CurrentState()->m_czExtent.cx, 1), Max(CurrentState()->m_czExtent.cy, 1));
		
		if (m_bmMask.CreateBitmap(crMask.Width(), crMask.Height(), 1, 1, NULL))
		{
			// Create a memory compatible DC and select the new bitmap into it.
			
			CDC MaskDC;
			
			if (MaskDC.CreateCompatibleDC(pDC))
			{
				CBrush cbTransparent(RGB(255,255,255));
				
				int nContext;
					
				if ((nContext = MaskDC.SaveDC()) != 0)
				{
					MaskDC.SetMapMode(MM_TEXT);
					
					if (MaskDC.SelectObject(&m_bmMask) != NULL)
					{
						// Create the mask image. Pixels that should be masked out should
						// be set to zero (black). Pixels that should be transparent should
						// be set to one (white).
						
						MaskDC.FillRect(&crMask, &cbTransparent);
						DrawMask(&MaskDC);
						ClearMaskNeeded();
					}
					
					MaskDC.RestoreDC(nContext);
				}
			}
		}
	}
	
	m_pContext->ReleaseDC();

	ASSERT(m_bmMask.GetSafeHandle() != NULL);
	
	if (m_bmMask.GetSafeHandle() == NULL)
	{
		return NULL;
	}
	
	return &m_bmMask;
}

// Get the image for the label shape. The image bitmap is rebuilt
// from scratch and cached if appropriate. The content object
// is called to draw the label content into the image bitmap.

CBitmap* COrnament::GetImage(void)
{
	CDC *pDC = m_pContext->GetDC();
	ASSERT(pDC != NULL);

	// If the image bitmap is not valid give the current state, free the
	// current bitmap and build the new one.
			
	if (ImageNeeded())
	{
		m_bmImage.DeleteObject();
	}

	// Check if we need to build a new bitmap.		
		
	if (m_bmImage.GetSafeHandle() == NULL)
	{
		// Create the bitmap for the image.
			
		CRect crImage(0, 0, Max(CurrentState()->m_czExtent.cx, 1), Max(CurrentState()->m_czExtent.cy, 1));
		
		if (m_bmImage.CreateCompatibleBitmap(pDC, crImage.Width(), crImage.Height()))
		{
			// Create a memory compatible DC and select the new bitmap into it.
				
			CDC ImageDC;
				
			if (ImageDC.CreateCompatibleDC(pDC))
			{
				CBrush cbTransparent(RGB(0,0,0));
				
				int nContext;
							
				if ((nContext = ImageDC.SaveDC()) != 0)
				{
					ImageDC.SetMapMode(MM_TEXT);
						
					if (ImageDC.SelectObject(&m_bmImage) != NULL)
					{
						// Create the image. Pixels that are transparent should be set to sero (black).
							
						ImageDC.FillRect(&crImage, &cbTransparent);
						DrawImage(&ImageDC);
						ClearImageNeeded();
					}
						
					ImageDC.RestoreDC(nContext);
				}
			}
		}
	}
	
	m_pContext->ReleaseDC();

	ASSERT(m_bmImage.GetSafeHandle() != NULL);
	
	if (m_bmImage.GetSafeHandle() == NULL)
	{
		return NULL;
	}
	
	return &m_bmImage;
}


// Check if a point is contained in the ornament.

BOOL COrnament::Contains(const CPoint& cpLocation)
{
	BOOL fContains = FALSE;
	
	// Make sure the object has been initialized.
	
	if (!CommitNeeded())
	{
		if (CurrentState()->m_crBounds.PtInRect(cpLocation))
		{
			fContains = TRUE;
		}
	}
	
	return fContains;
}

BOOL COrnament::SizeChanged(CPropertyState* pState) const
{
	COrnamentState* pCurrent = CurrentState();
	COrnamentState* pCompare = StateType(pState);
	
	if (!PropertySet(pState))
	{
		return FALSE;
	}

	return
		(pState == NULL)
	|| (pCurrent->m_dwStyle						!= pCompare->m_dwStyle)
	|| (pCurrent->m_czExtent					!= pCompare->m_czExtent);
}

BOOL COrnament::AppearanceChanged(CPropertyState* pState) const
{
	COrnamentState* pCurrent = CurrentState();
	COrnamentState* pCompare = StateType(pState);
	
	if (!PropertySet(pState))
	{
		return FALSE;
	}

	return
		(pState == NULL)
	|| (pCurrent->m_dwStyle						!= pCompare->m_dwStyle)
	|| (pCurrent->m_czExtent					!= pCompare->m_czExtent)
	|| (pCurrent->m_fVisible					!= pCompare->m_fVisible);
}

BOOL COrnament::PositionChanged(CPropertyState* pState) const
{
	COrnamentState* pCurrent = CurrentState();
	COrnamentState* pCompare = StateType(pState);
	
	if (!PropertySet(pState))
	{
		return FALSE;
	}

	return
		(pState == NULL)
	|| (pCurrent->m_cpPosition != pCompare->m_cpPosition);
}

BOOL COrnament::StateChanged(CPropertyState* pState) const
{
	COrnamentState* pCurrent = CurrentState();
	COrnamentState* pCompare = StateType(pState);
	
	if (!PropertySet(pState))
	{
		return FALSE;
	}

	return
		(pState == NULL)
	|| (pCurrent->m_dwStyle						!= pCompare->m_dwStyle)
	|| (pCurrent->m_cpPosition					!= pCompare->m_cpPosition)
	|| (pCurrent->m_czExtent					!= pCompare->m_czExtent)
	|| (pCurrent->m_fVisible					!= pCompare->m_fVisible);
}

BOOL COrnament::UpdateNeeded(void)
{
	CPropertyState* pState = UpdateState();

	return
		CPropertyObject::UpdateNeeded()
	|| SizeChanged(pState)
	|| PositionChanged(pState);
}

BOOL COrnament::CommitNeeded(void)
{
	return
		CPropertyObject::CommitNeeded()
	|| StateChanged(CommitState());
}

BOOL COrnament::MaskNeeded(void) const
{
	return MaskNeeded(m_pMaskState);
}

BOOL COrnament::MaskNeeded(CPropertyState* pState) const
{
	return SizeChanged(pState) || AppearanceChanged(pState);
}

void COrnament::ClearMaskNeeded(void)
{
	SaveState(m_pMaskState);
}

BOOL COrnament::ImageNeeded(void) const
{
	return ImageNeeded(m_pImageState);
}

BOOL COrnament::ImageNeeded(CPropertyState* pState) const
{
	return SizeChanged(pState) || AppearanceChanged(pState);
}

void COrnament::ClearImageNeeded(void)
{
	SaveState(m_pImageState);
}


// COrnamentState
//
//		This object records the state of the COrnament object.

COrnamentState::COrnamentState()
{
}

COrnamentState::~COrnamentState()
{
}
	
void COrnamentState::Init(void)
{
	m_dwStyle = 0;
	m_cpPosition = CPoint(0,0);
	m_cpOrigin = CPoint(0,0);
	m_czOffset = CSize(0,0);
	m_czExtent = CSize(0,0);
	m_czAspect = CSize(1,1);
	m_crBounds = CRect(0,0,0,0);
	m_fVisible = FALSE;
}
