/*
// $Workfile: CORNWMF.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CORNWMF.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:48   Fred
// Initial revision.
// 
//    Rev 1.1   29 Jul 1996 18:14:10   Fred
//  
// 
//    Rev 1.0   14 Mar 1996 13:36:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:44   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:35:54   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:53:32   FRED
//  
// 
//    Rev 1.3   12 Oct 1994 13:01:30   JAY
// Integration of Commit and Update at CPropertyObject level
// 
//    Rev 1.2   19 Sep 1994 15:47:12   FRED
// Added some checks to prevent divide-by-zero
// and other errors.
// 
// 
//    Rev 1.1   19 Sep 1994 11:54:00   FRED
// Some enhancements.
// 
// 
//    Rev 1.0   16 Sep 1994 10:48:40   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "mlsprop.h"
#include "cprop.h"
#include "cwmf.h"
#include "cornment.h"
#include "cornwmf.h"


// Properties

BEGIN_OBJECT_PROPERTIES(COrnamentWMFState)
	OBJECT_PROPERTY_INHERIT(COrnamentState)
END_OBJECT_PROPERTIES()


// Construct a COrnamentWMF object.

COrnamentWMF::COrnamentWMF()
{
}


// Destroy a COrnamentWMF object.

COrnamentWMF::~COrnamentWMF()
{
} 


// Initialize the COrnamentWMF object properties to their default values.

void COrnamentWMF::Init(CDrawContext* pContext, LPCSTR pszFileName, BOOL fReset /*=TRUE*/)
{
	// Initialize the current properties.

	COrnament::Init(pContext, fReset);
	
	// Initialize the meta file.
	
	m_MetaFile.Init(pszFileName);
	
	// Finish the initialization.
	
	CommonInit();
}

void COrnamentWMF::Init(CDrawContext* pContext, HINSTANCE hInstance, LPCSTR pszResource, BOOL fReset /*=TRUE*/)
{
	// Initialize the current properties.

	COrnament::Init(pContext, fReset);
	
	// Initialize the meta file.
	
	m_MetaFile.Init(hInstance, pszResource, "METAFILE");
	
	// Finish the initialization.
	
	CommonInit();
}

void COrnamentWMF::CommonInit(void)
{
	// Initialize the Aspect property. It is constant for a given meta file.
	
	const ALDUS_WMF_HEADER* pAldusHeader = m_MetaFile.AldusHeader();
	
	if (pAldusHeader != NULL)
	{
		CurrentState()->m_czAspect.cx = pAldusHeader->bbox.right-pAldusHeader->bbox.left;
		CurrentState()->m_czAspect.cy = pAldusHeader->bbox.bottom-pAldusHeader->bbox.top;
	}
}


void COrnamentWMF::SizeContent(void)
{
	// Compute the origin and offset based on the extent of
	// the ornament.
	
	// NOTE: The Bounds property is not up to date at this
	// point and should not be used is the SizeContent() method.
	// Furthermore, SizeContent() should not set the Bounds
	// rectangle. Its definition is fixed: Extent+Position-Origin.
	// The Update() method is responsible for setting the bounds
	// after SizeContent() has returned. The bounds rectangle is
	// used as the position where the actual image will be drawn.
	
	CSize czFullExtent = CurrentState()->m_czAspect;
	CSize czNewExtent = CurrentState()->m_czExtent;
	CPoint cpOrigin = GetOrigin();
	CSize czOffset = GetOffset();
	
	CurrentState()->m_cpOrigin = CPoint(
												MulDiv(cpOrigin.x, czNewExtent.cx, czFullExtent.cx),
												MulDiv(cpOrigin.y, czNewExtent.cy, czFullExtent.cy));
												
	CurrentState()->m_czOffset = CSize(
												MulDiv(czOffset.cx, czNewExtent.cx, czFullExtent.cx),
												MulDiv(czOffset.cy, czNewExtent.cy, czFullExtent.cy));
}

int COrnamentWMF::SizeForRepetitionsX(int nRepetitions)
{
	ASSERT(nRepetitions >= 0);
	return ((CurrentState()->m_czExtent.cx+CurrentState()->m_czOffset.cx)*nRepetitions)-CurrentState()->m_czOffset.cx;
}

int COrnamentWMF::SizeForRepetitionsY(int nRepetitions)
{
	ASSERT(nRepetitions >= 0);
	return ((CurrentState()->m_czExtent.cy+CurrentState()->m_czOffset.cy)*nRepetitions)-CurrentState()->m_czOffset.cy;
}

int COrnamentWMF::RepetitionsForSizeX(int nSize)
{
	if (nSize <= 0)
	{
		return 0;
	}
	
	return (nSize+CurrentState()->m_czOffset.cx)/(CurrentState()->m_czExtent.cx+CurrentState()->m_czOffset.cx);
}

int COrnamentWMF::RepetitionsForSizeY(int nSize)
{
	if (nSize <= 0)
	{
		return 0;
	}
	
	return (nSize+CurrentState()->m_czOffset.cy)/(CurrentState()->m_czExtent.cy+CurrentState()->m_czOffset.cy);
}

int COrnamentWMF::UnitSizeForRepetitionsInSizeX(int nRepetitions, int nSize)
{
	ASSERT(nRepetitions > 0);
	
	long dwS = (long)nSize;
	long dwR = (long)nRepetitions;
	long dwA = (long)CurrentState()->m_czAspect.cx;
	long dwO = (long)GetOffset().cx;
	long dwE = (dwS*dwA)/(dwR*dwA+dwO*(dwR-1));
	
	if (dwE > 0)
	{
		long dwP = (long)MulDiv((int)dwE, (int)dwO, (int)dwA);
		long dwQ = (dwS+dwP)/(dwE+dwP);
			
		if (dwQ < dwR)
		{
			dwE--;
		}
	}
	
	return (int)dwE;
}

int COrnamentWMF::UnitSizeForRepetitionsInSizeY(int nRepetitions, int nSize)
{
	ASSERT(nRepetitions > 0);
	
	long dwS = (long)nSize;
	long dwR = (long)nRepetitions;
	long dwA = (long)CurrentState()->m_czAspect.cy;
	long dwO = (long)GetOffset().cy;
	long dwE = (dwS*dwA)/(dwR*dwA+dwO*(dwR-1));
	
	if (dwE > 0)
	{
		long dwP = (long)MulDiv((int)dwE, (int)dwO, (int)dwA);
		if (dwE+dwP > 0)
		{
			long dwQ = (dwS+dwP)/(dwE+dwP);
				
			if (dwQ < dwR)
			{
				dwE--;
			}
		}
	}
	
	return (int)dwE;
}


// Draw a ornament's mask into a DC.

void COrnamentWMF::DrawMask(CDC* pDC)
{
	ASSERT(pDC != NULL);
	
	// Create the mask image. Pixels that should be masked out should
	// be set to zero (black). Pixels that should be transparent should
	// be set to one (white).
	
	CRect crBounds(0, 0, CurrentState()->m_czExtent.cx, CurrentState()->m_czExtent.cy);
	if (!crBounds.IsRectEmpty())
	{
		// Set the viewport to be our rectangle.

		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetViewportOrg(crBounds.left, crBounds.top);
		pDC->SetViewportExt(crBounds.Width(), crBounds.Height());
		m_MetaFile.Play(pDC, TRUE);
	}
}


// Draw a ornament's image into a DC.

void COrnamentWMF::DrawImage(CDC* pDC)
{
	ASSERT(pDC != NULL);
	
	CRect crBounds(0, 0, CurrentState()->m_czExtent.cx, CurrentState()->m_czExtent.cy);
	if (!crBounds.IsRectEmpty())
	{
		// Set the viewport to be our rectangle.

		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetViewportOrg(crBounds.left, crBounds.top);
		pDC->SetViewportExt(crBounds.Width(), crBounds.Height());
		m_MetaFile.Play(pDC, FALSE);
	}
}


BOOL COrnamentWMF::AppearanceChanged(CPropertyState* pState) const
{
	COrnamentWMFState* pCurrent = CurrentState();
	COrnamentWMFState* pCompare = StateType(pState);
	
	return
		COrnament::AppearanceChanged(pState);
}

BOOL COrnamentWMF::StateChanged(CPropertyState* pState) const
{
	COrnamentWMFState* pCurrent = CurrentState();
	COrnamentWMFState* pCompare = StateType(pState);
	
	return
		COrnament::StateChanged(pState);
}


// COrnamentWMFState
//
//		This object records the state of the COrnamentWMF object.

COrnamentWMFState::COrnamentWMFState()
{
}

COrnamentWMFState::~COrnamentWMFState()
{
}
	
void COrnamentWMFState::Init(void)
{
	COrnamentState::Init();
}
