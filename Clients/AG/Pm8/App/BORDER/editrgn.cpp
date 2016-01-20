// editrgn.cpp : Implements 'CEditRegion' class and subclasses
//

#include "stdafx.h"
#include "border.h"
#include "editrgn.h"

#include <math.h>

// Statics
int CEditRegion::m_nRefLimit;

/////////////////////////////////////////////////////////////////////////////
// CEditRegion - base class which contains generic edit region members.
// Construction/destruction

CEditRegion::CEditRegion(RECTAliasT& rctBounds, void* pDatabase)
{
	m_rctBounds.SetRECTAlias(rctBounds.left, rctBounds.top, rctBounds.right, rctBounds.bottom);
	m_dwID = 0;
	m_bSelected = FALSE;
	m_bSelectBoxDrawState = FALSE;
	m_pDatabase = pDatabase;

	// Initialize transforms
	m_Transforms.bXFlipped = FALSE;
	m_Transforms.bYFlipped = FALSE;
	m_Transforms.dAngle = 0.;
}

// Attributes
void CEditRegion::Rotate(double dAngle)
{
	// Compute new rotations angle
	if ((m_Transforms.dAngle += dAngle) >= _2PI)
	{
		m_Transforms.dAngle -= _2PI;
	}

	// Don't allow negatives
	if (m_Transforms.dAngle < 0.)
	{
		m_Transforms.dAngle += _2PI;
	}

	// Round off
	m_Transforms.dAngle = ROUND(m_Transforms.dAngle, ROUND_PLACE);
}

// Implementation
void CEditRegion::SetDrawInfo(CBEDrawInfo* pDrawInfo)
{
	CRect rctBounds;
	GetBounds(rctBounds);

	pDrawInfo->m_Bounds = rctBounds;
	pDrawInfo->m_fXFlipped ^= m_Transforms.bXFlipped;
	pDrawInfo->m_fYFlipped ^= m_Transforms.bYFlipped;
}

void CEditRegion::RestoreDrawInfo(CBEDrawInfo* pDrawInfo)
{
	// Restore original flipped states
	pDrawInfo->m_fXFlipped ^= m_Transforms.bXFlipped;
	pDrawInfo->m_fYFlipped ^= m_Transforms.bYFlipped;
}

void CEditRegion::RotateBounds(CBEDrawInfo* pDrawInfo, CPoint& ptOrigin)
{
	// Find center of bound
	CRect rctBounds(&pDrawInfo->m_Bounds);
	CPoint ptCenter = rctBounds.CenterPoint();
	
	// Rotate center point around origin
	// Compute sin & cosine of the given angle
	double dSin, dCos;
	dSin = sin(pDrawInfo->m_dAngle);
	dCos = cos(pDrawInfo->m_dAngle);
	
	// Make point relative to origin
	CPoint ptRelative = ptCenter - ptOrigin;
	CPoint ptRotated, ptNewCenter;

	// Rotate point
	ptRotated.x = (LONG)ROUND((ptRelative.x*dCos + ptRelative.y*dSin), 1);
	ptRotated.y = (LONG)ROUND((-ptRelative.x*dSin + ptRelative.y*dCos), 1);

	// Bring it on home
	ptNewCenter = ptOrigin + ptRotated;

	// Reconstruct bounds
	rctBounds.OffsetRect(ptNewCenter - ptCenter);
	rctBounds.InflateRect(
		(int)(rctBounds.Width() * .016),
		(int)(rctBounds.Height() * .016));	// inflate by 1.6% of size
	pDrawInfo->m_Bounds = rctBounds;
}

void CEditRegion::ApplyLocalRotation(CBEDrawInfo* pDrawInfo)
{
	// Add the angle
	// (subtract to get desired rotation effect)
	pDrawInfo->m_dAngle -= m_Transforms.dAngle;

	// Rotate the bounds
	// (Only 90 degree multiples are supported)
	if (IS_90_ROT(m_Transforms.dAngle))
	{
		RECT* rct = &pDrawInfo->m_Bounds;
		LONG nSizeDiff = (rct->right - rct->left) - (rct->bottom - rct->top);
		LONG nDiff = nSizeDiff / 2;
		
		// Assign all in one step		
		rct->left += nDiff;
		rct->top -= nDiff;
		rct->right -= nDiff;
		rct->bottom += nDiff;
	}
}
		
BOOL CEditRegion::InSelectRegion(CPoint& point)
{
	CRect rctBounds;
	GetBounds(rctBounds);

	// Simple point in bounds check
	return (rctBounds.PtInRect(point));
}

void CEditRegion::DrawSelectionBox(CDC* pDC, CPoint ptOffset, BOOL bDrawOver)
{
	CRect rctBounds;
	GetBounds(rctBounds);

	if ((!bDrawOver && (m_bSelected ^ m_bSelectBoxDrawState)) || 
		(bDrawOver && m_bSelected))
	{
		// XOR a box
		CPen BorderPen(PS_SOLID, 2, RGB(255, 255, 255));
		CPen* pOldPen = pDC->SelectObject(&BorderPen);
		pDC->SelectStockObject(HOLLOW_BRUSH);
		int nOldMode = pDC->SetROP2(R2_XORPEN);
		rctBounds.DeflateRect(1, 1);
		rctBounds.OffsetRect(ptOffset);
		pDC->Rectangle(rctBounds);
		pDC->SetROP2(nOldMode);
		pDC->SelectObject(pOldPen);
	}

	m_bSelectBoxDrawState = m_bSelected;
}

void CEditRegion::MoveRectToOrigin(RECT* rct)
{
	rct->right -= rct->left;
	rct->bottom -= rct->top;
	rct->left = 0;
	rct->top = 0;
}

void CEditRegion::SetGraphicID(CBECallbackInterface* pCBECallbacks, DWORD dwID, BOOL bUpdateList)
{
	ASSERT(m_pDatabase != NULL);
	if (dwID) {
		if (bUpdateList)
		{
			// Add the new reference to the release list if valid
			AddReleaseRef(pCBECallbacks, dwID, m_pDatabase);
		}
		else
		{
			// Make sure current IDs node is at head
			if (!AdjustHeadRef(dwID))
			{
				AddRefHead(dwID, m_pDatabase);
			}
		}
	}
	
	m_dwID = dwID;
}

void CEditRegion::DuplicateGraphicID(CBECallbackInterface* pCBECallbacks, void* pDupDatabase, CEditRegion* pRegion)
{
	// Make the duplication
	if (pRegion->m_dwID)
	{
		pCBECallbacks->Duplicate(m_dwID, pDupDatabase, pRegion->m_dwID, pRegion->m_pDatabase);
		m_pDatabase = pDupDatabase;
	}
}

// Ref list operators
void CEditRegion::AddReleaseRef(CBECallbackInterface* pCBECallbacks, DWORD dwID, void* pDatabase)
{
	RefDataT stRelease;

	// Add a reference
	pCBECallbacks->AddRef(dwID, pDatabase);

	// Add the new reference to the head. Remove the reference
	// from the tail if there is more than m_nRefLimit references.
	AddRefHead(dwID, pDatabase);
	if (m_ReleaseList.GetCount() > m_nRefLimit)
	{
		stRelease = m_ReleaseList.GetTail();
		pCBECallbacks->RemoveRef(stRelease.dwID, stRelease.pDatabase);
		m_ReleaseList.RemoveTail();
	}

}

BOOL CEditRegion::AdjustHeadRef(DWORD dwID)
{
	RefDataT stRefData;
	POSITION pos = m_ReleaseList.GetTailPosition(), oldPos;
	BOOL bFoundNode = FALSE;
	
	// Scan for the correct node from the tail of the list
	while (pos != NULL)
	{
		oldPos = pos;
		stRefData = m_ReleaseList.GetPrev(pos);
		if (dwID == stRefData.dwID)
		{
			// Remove and place at head
			m_ReleaseList.RemoveAt(oldPos);
			m_ReleaseList.AddHead(stRefData);
			bFoundNode = TRUE;
			break;
		}
	}

	return (bFoundNode);
}

void CEditRegion::AddRefHead(DWORD dwID, void* pDatabase)
{
	RefDataT stRefData = {dwID, pDatabase};
	m_ReleaseList.AddHead(stRefData);
}

void CEditRegion::RemoveRefList(CBECallbackInterface* pCBECallbacks)
{
	POSITION pos = m_ReleaseList.GetHeadPosition();
	RefDataT stRefData;

	// Release all references in the list
	while (pos != NULL)
	{
		stRefData = m_ReleaseList.GetNext(pos);
		pCBECallbacks->RemoveRef(stRefData.dwID, stRefData.pDatabase);
	}

}

/////////////////////////////////////////////////////////////////////////////
// CCornerRegion - specifically handles corner selection regions
// Construction/destruction

CCornerRegion::CCornerRegion(RECTAliasT& rctBounds, void* pDatabase) :
	CEditRegion(rctBounds, pDatabase)
{

}

// Attributes

// Implementation
int CCornerRegion::DrawRegion(CBECallbackInterface* pCBECallbacks, CBEDrawInfo* pDrawInfo, CPoint* ptOrigin, BOOL bRelative)
{
	// Draw the graphic if valid
	if (m_dwID || pDrawInfo->m_clBackground != -1L)
	{
		SetDrawInfo(pDrawInfo);

		// Look for refresh info
		if (pDrawInfo->m_pDrawState != NULL)
		{
			SRedrawStateT* pRedrawState = (SRedrawStateT*)pDrawInfo->m_pDrawState;
			pDrawInfo->m_pDrawState = pRedrawState->pDrawState;
			delete pRedrawState;
		}

		CRect* pRect = (CRect*)&pDrawInfo->m_Bounds;
		if (pRect->Width() > 1 && pRect->Height() > 1)
		{
			// If drawing is relative, bring bounds to the origin
			if (bRelative)
			{
				MoveRectToOrigin(&pDrawInfo->m_Bounds);
			}
			
			// Rotate bounds
			if (pDrawInfo->m_dAngle != 0.)
			{
				CEditRegion::RotateBounds(pDrawInfo, CPoint(*ptOrigin));
			}

			// apply extra rotation now
			double dAngle = pDrawInfo->m_dAngle;
			CEditRegion::ApplyLocalRotation(pDrawInfo);

			// Draw & restore angle
			if (pCBECallbacks->DrawGraphic(m_dwID, m_pDatabase, pDrawInfo))
				return (-1);
			
			// Check for interrupt
			if (pDrawInfo->m_pDrawState != NULL)
			{
				SRedrawStateT* pRedrawState = new SRedrawStateT;
				pRedrawState->pDrawState = pDrawInfo->m_pDrawState;
				pDrawInfo->m_pDrawState = (void*)pRedrawState;
			}

			// Restore saved info
			RestoreDrawInfo(pDrawInfo);
			pDrawInfo->m_dAngle = dAngle;
		}
	}

	return (0);
}

/////////////////////////////////////////////////////////////////////////////
// CEdgeRegion - specifically handles edge selection regions
// Construction/destruction

CEdgeRegion::CEdgeRegion(RECTAliasT& rctBounds, enum SideTypeE side, void* pDatabase) : 
	CEditRegion(rctBounds, pDatabase)
{
	m_Side = side;
	m_bForceTile = FALSE;
	m_bUseLastTiles = FALSE;
	m_nTiles = 0;
	m_nTilesDrawn = -1;
	m_nMaxTiles = -1;
}	
	

// Attributes
void CEdgeRegion::SetSizing(double dSize, double aXOffsets[], double aYOffsets[])
{
	// Set the size (depends on m_Side)
	switch (m_Side)
	{
	case LEFT_SIDE:
		aXOffsets[0] = dSize;
		if (aXOffsets[0] > aXOffsets[1])
		{
			aXOffsets[0] = aXOffsets[1];
		}
		break;
	case TOP_SIDE:
		aYOffsets[0] = dSize;
		if (aYOffsets[0] > aYOffsets[1])
		{
			aYOffsets[0] = aYOffsets[1];
		}
		break;
	case RIGHT_SIDE:
		aXOffsets[1] = aXOffsets[2] - dSize;
		if (aXOffsets[1] < aXOffsets[0])
		{
			aXOffsets[1] = aXOffsets[0];
		}
		break;
	case BOTTOM_SIDE:
		aYOffsets[1] = aYOffsets[2] - dSize;
		if (aYOffsets[1] < aYOffsets[0])
		{
			aYOffsets[1] = aYOffsets[0];
		}
		break;
	}

}

void CEdgeRegion::GetSizing(double& dSize, double aXOffsets[], double aYOffsets[])
{
	// Get the size (depends on m_Side)
	switch (m_Side)
	{
	case LEFT_SIDE:
		dSize = aXOffsets[0];
		break;
	case TOP_SIDE:
		dSize = aYOffsets[0];
		break;
	case RIGHT_SIDE:
		dSize = aXOffsets[2] - aXOffsets[1];
		break;
	case BOTTOM_SIDE:
		dSize = aYOffsets[2] - aYOffsets[1];
		break;
	}

}

void CEdgeRegion::IntelligentRotate(CBECallbackInterface* pCBECallbacks)
{
	// -Intelligent rotation-
	// If no transforms have been applied to the region, and the new
	// graphic has an aspect ration which differs from the region's
	// aspect ration of a factor higher than AUTOMATIC_ROTATE_THRESHOLD,
	// rotate the graphic.
	const double AUTOMATIC_ROTATE_THRESHOLD = 3.5;

	if (m_dwID &&
		 m_Transforms.dAngle == 0. &&
		 !m_Transforms.bXFlipped &&
		 !m_Transforms.bYFlipped)
	{
		// Get region bounds
		CRect rctBounds;
		GetBounds(rctBounds);

		// Determine dimensions
		int nRctHeight = rctBounds.Height();
		int nRctWidth = rctBounds.Width();

		// Get graphic size
		CBEGraphicInfo stGraphicInfo;
		if (pCBECallbacks->GetGraphicInfo(m_dwID, m_pDatabase, stGraphicInfo))
			return;

		// If aspect ratios are drastically different, then rotate
		if (m_Side == LEFT_SIDE || m_Side == RIGHT_SIDE)
		{
			if ((double)nRctHeight / nRctWidth > AUTOMATIC_ROTATE_THRESHOLD &&
				(double)stGraphicInfo.m_dXSize / stGraphicInfo.m_dYSize > AUTOMATIC_ROTATE_THRESHOLD)
			{
				// Apply a rotation
				Rotate(RAD_90);
			}
		}
		else
		{
			if ((double)nRctWidth / nRctHeight > AUTOMATIC_ROTATE_THRESHOLD &&
				(double)stGraphicInfo.m_dYSize / stGraphicInfo.m_dXSize > AUTOMATIC_ROTATE_THRESHOLD)
			{
				// Apply a rotation
				Rotate(RAD_90);
			}
		}
	}

}

// Implementation
int CEdgeRegion::DrawRegion(CBECallbackInterface* pCBECallbacks, CBEDrawInfo* pDrawInfo, CPoint* ptOrigin, BOOL bRelative)
{
	// Draw the graphic if valid
	if (m_dwID || pDrawInfo->m_clBackground != -1L)
	{
		// Set up basic info
		SetDrawInfo(pDrawInfo);
		CRect rctBounds = pDrawInfo->m_Bounds;

		// Need to compute the number of tiles to draw
		int nDrawTiles;
		int nXAdd, nYAdd;
		int nXMod = 0, nYMod = 0;
		
		// If drawing is relative, bring bounds to the origin
		if (bRelative)
		{
			MoveRectToOrigin(&rctBounds);
		}

		// Determine dimensions
		int nRctHeight = rctBounds.Height();
		int nRctWidth = rctBounds.Width();
		if (m_dwID)
		{
			// Get graphic size
			CBEGraphicInfo stGraphicInfo;
			if (pCBECallbacks->GetGraphicInfo(m_dwID, m_pDatabase, stGraphicInfo))
				return (-1);

			// Swap graphic aspect ratio on certain rotations
			if (IS_90_ROT(m_Transforms.dAngle))
			{
				double dSwap = stGraphicInfo.m_dXSize;
				stGraphicInfo.m_dXSize = stGraphicInfo.m_dYSize;
				stGraphicInfo.m_dYSize = dSwap;
			}

			if (m_bForceTile)
			{
				nDrawTiles = m_nTiles;
			}
			else if (m_bUseLastTiles && m_nTilesDrawn != -1)
			{
				nDrawTiles = m_nTilesDrawn;
			}
			else	// Rudimentary intelligent redraw
			{
				// Number of tiles to draw is equal to the aspect
				// ratio of the edit region divided by the aspect
				// ratio of the graphic to be drawn.
				if (m_Side == LEFT_SIDE || m_Side == RIGHT_SIDE)
				{
					nDrawTiles = (int)(ceil(
						((double)nRctHeight / nRctWidth) /
						(stGraphicInfo.m_dYSize / stGraphicInfo.m_dXSize)));
				}
				else
				{
					nDrawTiles = (int)(ceil(
						((double)nRctWidth / nRctHeight) /
						(stGraphicInfo.m_dXSize / stGraphicInfo.m_dYSize)));
				}

				// Can't have 0
				if (nDrawTiles == 0) nDrawTiles = 1;

				// Check for max
				if (m_nMaxTiles != -1 && nDrawTiles > m_nMaxTiles) nDrawTiles = m_nMaxTiles;
			}
		}
		else	// just drawing background now
		{
			nDrawTiles = 1;
		}
		m_nTilesDrawn = nDrawTiles;
			
		// Compute bounds of each graphic
		if (m_Side == LEFT_SIDE || m_Side == RIGHT_SIDE)
		{
			int nHeight = nRctHeight / nDrawTiles;
			nYMod = nRctHeight % nDrawTiles;
				
			// Offsets
			nXAdd = 0;
			nYAdd = nHeight;

			// Compute new initial bounds
			rctBounds.bottom = rctBounds.top + nHeight;
		}
		else
		{
			int nWidth = nRctWidth / nDrawTiles;
			nXMod = nRctWidth % nDrawTiles;
	
			// Offsets
			nXAdd = nWidth;
			nYAdd = 0;

			// Compute new initial bounds
			rctBounds.right = rctBounds.left + nWidth;
		}

		// Look for refresh info
		int nFirstTile = 0;
		if (pDrawInfo->m_pDrawState != NULL)
		{
			SRedrawStateT* pRedrawState = (SRedrawStateT*)pDrawInfo->m_pDrawState;
			nFirstTile = pRedrawState->nTileIndex;
			pDrawInfo->m_pDrawState = pRedrawState->pDrawState;
			delete pRedrawState;
		}

		// Draw the graphics
		if (nRctWidth > 1 && nRctHeight > 1)
		{
			int i;
			int nXExtra, nYExtra;

			for (i = 0; i < nDrawTiles; i++)
			{
				// Check on remainder
				nXExtra = 0, nYExtra = 0;
				if (nXMod)
				{
					nXExtra = 1;
					nXMod--;
				}
				if (nYMod)
				{
					nYExtra = 1;
					nYMod--;
				}

				// Transform and draw
				if (i >= nFirstTile)
				{					
					// Rotate bounds & draw
					pDrawInfo->m_Bounds = rctBounds;
					if (pDrawInfo->m_dAngle != 0.)
					{
						CEditRegion::RotateBounds(pDrawInfo, CPoint(*ptOrigin));
					}

					// apply extra rotation now
					double dAngle = pDrawInfo->m_dAngle;	
					CEditRegion::ApplyLocalRotation(pDrawInfo);

					// Final computation on bounds size
					pDrawInfo->m_Bounds.right += nXExtra;
					pDrawInfo->m_Bounds.bottom += nYExtra;

					if (pCBECallbacks->DrawGraphic(m_dwID, m_pDatabase, pDrawInfo))
						return (-1);

					// Restore angle
					pDrawInfo->m_dAngle = dAngle;

					// Check for interrupt
					if (pDrawInfo->m_pDrawState != NULL)
					{
						SRedrawStateT* pRedrawState = new SRedrawStateT;
						pRedrawState->pDrawState = pDrawInfo->m_pDrawState;
						pRedrawState->nTileIndex = i;
						pDrawInfo->m_pDrawState = (void*)pRedrawState;
						break;
					}
				}

				// Translate bounds for next draw operation
				rctBounds.OffsetRect(nXAdd + nXExtra, nYAdd + nYExtra);
			}
		}

		// Restore saved info
		RestoreDrawInfo(pDrawInfo);
	}

	return (0);
}
