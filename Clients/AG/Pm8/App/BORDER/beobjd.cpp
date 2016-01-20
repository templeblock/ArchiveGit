// beobjd.cpp : Implements 'CBEObjectD' class
//

#include "stdafx.h"
#include "border.h"
#include "beobjd.h"
#include "bordview.h"

#include <math.h>

// Default border grid
const double DEFAULT_W = 4.0;
const double DEFAULT_H = 5.0;

// CBEObjectD statics
BOOL CBEObjectD::m_bCopyCompleted = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CBEObjectD - interface definition for border object
//             These objects are created and destroyed by the border DLL in
//             response to requests from the client (via NewObject()).
// Construction/destruction
CBEObjectD::CBEObjectD(void* pDatabase, int nLength, void* pData)
{
	// Init data members
	Init(pDatabase);
	
	// Create the editing regions
	CreateEditRegions();
	
	// Call 'SetData' to transfer serializeable data to object data
	if (pData) {
		SetData(nLength, pData);
	}
}

CBEObjectD::CBEObjectD(CBEObjectD* pBEObjCopy, void* pDatabase)
{
	int i;
	CBECallbackInterface* pCBECallbacks = theBorderApp.GetCallbacks();

	// Init as failure
	m_bCopyCompleted = FALSE;

	// Init (copy) data members
	Init((pDatabase == NULL) ? pBEObjCopy->m_pDatabase : pDatabase);
	
	// Create the editing regions
	CreateEditRegions();

	// Copy edit region data
	for (i = 0; i < 8; i++)
	{
		// Duplicate a reference
		m_apEditRegions[i]->DuplicateGraphicID(pCBECallbacks, m_pDatabase, pBEObjCopy->m_apEditRegions[i]);

		// Transforms
		CEditRegion::TransformsT transforms;
		pBEObjCopy->m_apEditRegions[i]->GetTransforms(transforms);
		m_apEditRegions[i]->SetTransforms(transforms);

		// Edges
		if (i >= 4)
		{
			BOOL bForce;
			int nTiles, nMaxTiles;

			((CEdgeRegion*)pBEObjCopy->m_apEditRegions[i])->GetTiling(bForce, nTiles, nMaxTiles);			
			((CEdgeRegion*)m_apEditRegions[i])->SetTiling(bForce, nTiles, nMaxTiles);
		}
	}
	

	// Initialize the serializable data
	// Init bounds
	SetSize(pBEObjCopy->m_dXSize, pBEObjCopy->m_dYSize);

	// Copy grid offsets
	for (i = 0; i < 3; i++)
	{
		m_aXOffsets[i] = pBEObjCopy->m_aXOffsets[i];
		m_aYOffsets[i] = pBEObjCopy->m_aYOffsets[i];
	}

	// Background color
	m_bUseBGColor = pBEObjCopy->m_bUseBGColor;
	m_colBGColor = pBEObjCopy->m_colBGColor;
	
	// Finished
	m_bCopyCompleted = TRUE;
}

void CBEObjectD::Init(void* pDatabase)
{
	m_pDatabase = pDatabase;
	m_bInteriorsInitialized = FALSE;
	m_bSelected = FALSE;

	// Default zoom position
	m_nZoomPosition = 0;
	m_dZoomScale = 1.0;

	// Background color
	m_bUseBGColor = FALSE;
	m_colBGColor = RGB(255, 255, 255);
}

void CBEObjectD::CreateEditRegions()
{
	RECTAliasT rctBounds;	// Rectangle bounds of the region

	// corners
	rctBounds.SetRECTAlias(&m_aXCoords[0], &m_aYCoords[0], &m_aXCoords[1], &m_aYCoords[1]);
	m_apEditRegions[0] = new CCornerRegion(rctBounds, m_pDatabase);		// Top-left corner
	rctBounds.SetRECTAlias(&m_aXCoords[2], &m_aYCoords[0], &m_aXCoords[3], &m_aYCoords[1]);
	m_apEditRegions[1] = new CCornerRegion(rctBounds, m_pDatabase);		// Top-right corner
	rctBounds.SetRECTAlias(&m_aXCoords[0], &m_aYCoords[2], &m_aXCoords[1], &m_aYCoords[3]);
	m_apEditRegions[2] = new CCornerRegion(rctBounds, m_pDatabase);		// Bottom-left corner
	rctBounds.SetRECTAlias(&m_aXCoords[2], &m_aYCoords[2], &m_aXCoords[3], &m_aYCoords[3]);
	m_apEditRegions[3] = new CCornerRegion(rctBounds, m_pDatabase);		// Bottom-right corner

	// edges
	rctBounds.SetRECTAlias(&m_aXCoords[0], &m_aYCoords[1], &m_aXCoords[1], &m_aYCoords[2]);
	m_apEditRegions[4] = new CEdgeRegion(rctBounds, LEFT_SIDE, m_pDatabase);			// Left edge
	rctBounds.SetRECTAlias(&m_aXCoords[1], &m_aYCoords[0], &m_aXCoords[2], &m_aYCoords[1]);
	m_apEditRegions[5] = new CEdgeRegion(rctBounds, TOP_SIDE, m_pDatabase);			// Top edge
	rctBounds.SetRECTAlias(&m_aXCoords[2], &m_aYCoords[1], &m_aXCoords[3], &m_aYCoords[2]);
	m_apEditRegions[6] = new CEdgeRegion(rctBounds, RIGHT_SIDE, m_pDatabase);			// Right edge
	rctBounds.SetRECTAlias(&m_aXCoords[1], &m_aYCoords[2], &m_aXCoords[2], &m_aYCoords[3]);
	m_apEditRegions[7] = new CEdgeRegion(rctBounds, BOTTOM_SIDE, m_pDatabase);			// Bottom edge

	// Clear selection array
	int i;
	for (i = 0; i < 8; m_aSelectedRegions[i++] = FALSE);
	m_nDragRegion = -1;

	// Initialize array of rects
	m_aRectArray.SetSize(8);
}
	
// Interface
int CBEObjectD::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Free edit regions
	int i;
	for (i = 0; i < 8; i++)
	{
		delete m_apEditRegions[i];
	}

	// Free the object itself
	delete this;

	return (0);
}

int CBEObjectD::Destroy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Remove graphic references
	int i;
	for (i = 0; i < 8; i++)
	{
		m_apEditRegions[i]->RemoveRefList(theBorderApp.GetCallbacks());
	}

	// Delete the object
	Release();

	return (0);
}

int CBEObjectD::GetData(int& nLength, void* pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Store serialize data in pData
	SRawDataT SerializeData;

	nLength = sizeof(SRawDataT);
	if (pData != NULL)
	{
		// version
		SerializeData.version = BE_VERSION;

		// Transfer data from object to serialize
		SerializeData.dXSize = m_dXSize;
		SerializeData.dYSize = m_dYSize;
		memcpy(SerializeData.aXCoords, m_aXOffsets, sizeof(double)*3);
		memcpy(SerializeData.aYCoords, m_aYOffsets, sizeof(double)*3);

		// Background color
		SerializeData.bUseBGColor = m_bUseBGColor;
		SerializeData.colBGColor = m_colBGColor;

		// Inner rectangle
		SerializeData.rctInner = m_rctInner;

		int i;
		for (i = 0; i < 8; i++)
		{
			SerializeData.dwIDs[i] = m_apEditRegions[i]->GetGraphicID();
			m_apEditRegions[i]->GetTransforms(SerializeData.transforms[i]);
		}

		// Edges
		GetAllTiling(SerializeData.bForceTile, SerializeData.nTiles, SerializeData.nMaxTiles);

		// Copy the whole
		*(SRawDataT*)pData = SerializeData;
	}

	return (0);
}

int CBEObjectD::SetData(int nLength, void* pData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CBECallbackInterface* pCBECallbacks = theBorderApp.GetCallbacks();

	// Transfer passed data to the serialize object, 
	// and into the BE object data
	if (nLength != sizeof(SRawDataT))
	{
		return (-1);
	}

	// transfer serialize
	SRawDataT SerializeData = *(SRawDataT*)pData;

	// Set size & offsets
	SetSize(SerializeData.dXSize, SerializeData.dYSize);
	memcpy(m_aXOffsets, SerializeData.aXCoords, sizeof(double)*3);
	memcpy(m_aYOffsets, SerializeData.aYCoords, sizeof(double)*3);

	// Background color
	m_bUseBGColor = SerializeData.bUseBGColor;
	m_colBGColor = SerializeData.colBGColor;

	// Inner rectangle
	m_rctInner = SerializeData.rctInner;

	// Reconstruct edit regions
	int i;
	for (i = 0; i < 8; i++)
	{
		// Make sure the record referenced is of the correct type or have it
		// converted and the new ID set back for us.
		pCBECallbacks->MatchupRecordType(&SerializeData.dwIDs[i], m_pDatabase);
		// Now set ourselves with the guaranteed correct ID.
		m_apEditRegions[i]->SetGraphicID(pCBECallbacks, SerializeData.dwIDs[i], FALSE);
		m_apEditRegions[i]->SetTransforms(SerializeData.transforms[i]);
	}

	// Edges
	SetAllTiling(SerializeData.bForceTile, SerializeData.nTiles, SerializeData.nMaxTiles);

	return (0);
}

int CBEObjectD::Duplicate(CBEObject*& pObject, void* pDatabase)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Use the copy constructor
	pObject = new CBEObjectD(this, pDatabase);

	return (m_bCopyCompleted ? 0 : -1);
}

int CBEObjectD::SetSize(double dXSize, double dYSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Compute aspect ratio
	ComputeAspectRatio(dXSize, dYSize);

	// Reset outside edges
	m_aXOffsets[2] = dXSize;
	m_aYOffsets[2] = dYSize;

	// Recompute interior
	if (m_bInteriorsInitialized)
	{
		double dInteriorScale = (dXSize < dYSize) ? dXSize / m_dXSize : dYSize / m_dYSize;
		int i;
		for (i = 0; i < 2; i++)
		{
			m_aXOffsets[i] *= dXSize / m_dXSize;
			m_aYOffsets[i] *= dYSize / m_dYSize;
		}
	}
	else
	{
		double dInteriorSize = (dXSize < dYSize) ? dXSize / 5. : dYSize / 5.;

		m_aXOffsets[0] = dInteriorSize;
		m_aXOffsets[1] = dXSize - dInteriorSize;
		m_aYOffsets[0] = dInteriorSize;
		m_aYOffsets[1] = dYSize - dInteriorSize;

		m_bInteriorsInitialized = TRUE;
	}

	// Compute zoom offsets
	ScaleOffsets(m_aXZmOffsets, m_aYZmOffsets, m_aXOffsets, m_aYOffsets, m_dZoomScale);

	// Store data
	m_dXSize = dXSize;
	m_dYSize = dYSize;

	return (0);
}

void CBEObjectD::GetInnerRect(RECT* pRect)
{
	*pRect = m_rctInner;
}

int CBEObjectD::Draw(CBEDrawInfo* pDrawInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	double dOldDPInchRatio = m_dDPInchRatio;
	LONG nXAnchor = m_aXCoords[0];
	LONG nYAnchor = m_aYCoords[0];

	// Resize the grid to fit in the given bounds
	CRect rctDrawBounds(pDrawInfo->m_Bounds);

	// Set anchor point
	m_aXCoords[0] = pDrawInfo->m_Bounds.left;
	m_aYCoords[0] = pDrawInfo->m_Bounds.top;

	// Compute device-to-inch coversion ratio
	m_dDPInchRatio = (m_dXSize != 0.) ?
		(pDrawInfo->m_Bounds.right - pDrawInfo->m_Bounds.left) / m_dXSize :
		(pDrawInfo->m_Bounds.bottom - pDrawInfo->m_Bounds.top) / m_dYSize;

	// Compute device grid coordinates
	ComputeGridCoords(FALSE);

	// Save inner rectangle
	m_rctInner.left = m_aXCoords[1];
	m_rctInner.top = m_aYCoords[1];
	m_rctInner.right = m_aXCoords[2];
	m_rctInner.bottom = m_aYCoords[2];

	// Now draw
	int r = DrawGrid(pDrawInfo);

	// Restore old coordinates
	m_aXCoords[0] = nXAnchor;
	m_aYCoords[0] = nYAnchor;
	m_dDPInchRatio = dOldDPInchRatio;
	ComputeGridCoords();

	return (r);
}

int CBEObjectD::Edit(HWND hParent)
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Clear selection array
	int i;
	for (i = 0; i < 8; m_aSelectedRegions[i++] = FALSE);

	// Default zoom position
	m_nZoomPosition = 0;
	m_dZoomScale = 1.0;

	// Compute zoom offsets
	ScaleOffsets(m_aXZmOffsets, m_aYZmOffsets, m_aXOffsets, m_aYOffsets, m_dZoomScale);

	// Tell the app to initialize itself
	if (theBorderApp.InitApp(this, hParent))
	{
		return (-1);
	}

	// Save BE data, in case of revert
	int nSize = sizeof(SRawDataT);
	GetData(nSize, (void*)&m_OriginalData);

	// Run a modal loop until editing is complete
	int nStatus = theBorderApp.DoModal();
	if (!nStatus)
	{
		// Check for empty border object
		if (IsEmpty())
		{
			nStatus = BorderEmpty;
		}
	}

	return (nStatus);
}

// Attributes
void CBEObjectD::SetXCoord(int nIndex, LONG nValue)
{
	m_aXCoords[nIndex] = nValue;

	// Adjust inches
	if (nIndex != 0)
	{
		m_aXZmOffsets[nIndex-1] = (m_aXCoords[nIndex] - m_aXCoords[0]) / m_dDPInchRatio;
	}
}

void CBEObjectD::SetYCoord(int nIndex, LONG nValue)
{
	m_aYCoords[nIndex] = nValue;

	// Adjust inches
	if (nIndex != 0)
	{
		m_aYZmOffsets[nIndex-1] = (m_aYCoords[nIndex] - m_aYCoords[0]) / m_dDPInchRatio;
	}
}

void CBEObjectD::GetXCoords(LONG aXCoords[])
{
	memcpy(aXCoords, m_aXCoords, sizeof(LONG)*4);
}

void CBEObjectD::GetYCoords(LONG aYCoords[])
{
	memcpy(aYCoords, m_aYCoords, sizeof(LONG)*4);
}

void CBEObjectD::GetXOffsets(double aXOffsets[])
{
	memcpy(aXOffsets, m_aXOffsets, sizeof(double)*3);
}

void CBEObjectD::GetYOffsets(double aYOffsets[])
{
	memcpy(aYOffsets, m_aYOffsets, sizeof(double)*3);
}

void CBEObjectD::Zoom(ZoomActionsE zoom)
{
	m_nZoomPosition += (zoom == ZoomIn) ? 1 : -1;
	m_dZoomScale = (zoom == ZoomIn) ? 2.0 : 0.5;
	ScaleOffsets(m_aXZmOffsets, m_aYZmOffsets, m_aXZmOffsets, m_aYZmOffsets, m_dZoomScale);
	ComputeGridCoords();
	CheckZoomedGrid();

	// Zoom scale should now be absolute
	m_dZoomScale = pow(2., m_nZoomPosition);
}

BOOL CBEObjectD::AtZoomLimit(ZoomActionsE zoom)
{
	BOOL bAtLimit;

	// Basic limit check
	bAtLimit = 
		(zoom == ZoomIn && m_nZoomPosition == 1) ||
		(zoom == ZoomOut && m_nZoomPosition == -1);

	// Don't allow a zoom in if edges will cross
	if (zoom == ZoomIn)
	{
		if (m_aXZmOffsets[0] * 2.0 > m_aXZmOffsets[2] - ((m_aXZmOffsets[2] - m_aXZmOffsets[1]) * 2.0) ||
			m_aYZmOffsets[0] * 2.0 > m_aYZmOffsets[2] - ((m_aYZmOffsets[2] - m_aYZmOffsets[1]) * 2.0))
		{
			bAtLimit = TRUE;
		}
	}

	return (bAtLimit);
}

BOOL CBEObjectD::EdgeCopyAllowed(enum FlipTypesE flip /*= NO_FLIP*/)
{
	// For flip == HV_FLIP, return TRUE if there are any edges selected.
	// For flip == NO_FLIP, return TRUE if there is 1 edge selected
	int i;
	int edge_cnt = 0;

	for (i = 4; i < N_EDIT_REGIONS; i++)
	{
		if (m_aSelectedRegions[i] && m_apEditRegions[i]->GetGraphicID())
		{
			edge_cnt++;
		}
	}

	return ((flip == HV_FLIP && edge_cnt) || edge_cnt == 1);
}

BOOL CBEObjectD::CornerCopyAllowed()
{
	// Return TRUE if 1 and only 1 corner is selected
	int i;
	int corner_cnt = 0;

	for (i = 0; i < 4; i++)
	{
		if (m_aSelectedRegions[i] && m_apEditRegions[i]->GetGraphicID())
		{
			corner_cnt++;
		}
	}

	return (corner_cnt == 1);
}

BOOL CBEObjectD::CanCutCopy()
{
	// Return TRUE if 1 and only 1 region is selected that
	// has a valid graphic
	int i;
	int corner_cnt = 0;

	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if (m_aSelectedRegions[i] && m_apEditRegions[i]->GetGraphicID())
		{
			corner_cnt++;
		}
	}

	return (corner_cnt == 1);
}

BOOL CBEObjectD::CanDoTransforms()
{
	// Return TRUE if any region is selected that has a valid graphic
	int i;

	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if (m_aSelectedRegions[i] && m_apEditRegions[i]->GetGraphicID())
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

BOOL CBEObjectD::IsEmpty()
{
	// Border is empty if all edit regions have no graphics
	// and no background color is selected
	if (m_bUseBGColor)
	{
		return (FALSE);
	}

	int i;
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if (m_apEditRegions[i]->GetGraphicID() != 0)
		{
			return (FALSE);
		}
	}

	return (TRUE);
}

// Edit Region
BOOL CBEObjectD::SelectEditRegion(BOOL bKeep, BOOL bToggle, CPoint point)
{
	int i;
	BOOL bOldState;
	BOOL r = FALSE;
	CPoint pt = point;	// so we can pass a reference
	BOOL nChanged[N_EDIT_REGIONS];

	// Search for selections, unmarking if no keep
	m_bSelected = FALSE;
	m_nDragRegion = -1;
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		// Unselect region if bKeep is FALSE
		bOldState = m_aSelectedRegions[i];
		if (!bKeep)
		{
			m_aSelectedRegions[i] = m_apEditRegions[i]->SetSelection(FALSE);
		}

		// If point is in the region, select or deselect it
		if (m_apEditRegions[i]->InSelectRegion(pt))
		{
			m_nDragRegion = i;
			r = TRUE;
			m_aSelectedRegions[i] = m_apEditRegions[i]->SetSelection(TRUE, bToggle);
		}

		// Keep track if at least one region is selected
		m_bSelected |= m_aSelectedRegions[i];

		// Add the rectangle to the list if it was changed
		nChanged[i] = (m_aSelectedRegions[i] != bOldState);
	}

	// Update rectangles
	SetChangedRects(nChanged);

	return (r);
}

BOOL CBEObjectD::SelectDragRegion(CPoint* point)
{
	int i;
	BOOL nChanged[N_EDIT_REGIONS];
	int nNewDragRegion = -1;
	
	// Search for selections, unmarking if no keep
	if (point != NULL)
	{
		CPoint pt = *point;	// so we can pass a reference
		for (i = 0; i < N_EDIT_REGIONS; i++)
		{
			// If point is in the region, make it m_nDragRegion
			nChanged[i] = FALSE;
			if (m_apEditRegions[i]->InSelectRegion(pt))
			{
				nNewDragRegion = i;
				if (i != m_nDragRegion)
				{
					// Set the "unselecting" state on all other regions
					int j;
					for (j = 0; j < N_EDIT_REGIONS; j++)
					{
						// Add the rectangle to the list if it was changed
						if (j != i)
						{
							nChanged[j] = (
								m_apEditRegions[j]->GetSelection() !=
								m_apEditRegions[j]->SetSelection(FALSE));
						}
					}
					nChanged[i] = m_apEditRegions[i]->SetSelection(TRUE);
					m_nDragRegion = i;
				}
				break;
			}
		}
	}

	// Check for going from selected to unselected
	if (nNewDragRegion == -1 && m_nDragRegion != -1)
	{
		nChanged[m_nDragRegion] = TRUE;
		m_apEditRegions[m_nDragRegion]->SetSelection(FALSE);
		m_nDragRegion = -1;
	}

	// Update rectangles
	SetChangedRects(nChanged);

	return (m_nDragRegion != -1);
}

void CBEObjectD::ReselectRegions()
{
	// Reselect the "real" selected regions
	int i;
	BOOL bChanged[N_EDIT_REGIONS];
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		bChanged[i] = (
			m_apEditRegions[i]->GetSelection() !=
			m_apEditRegions[i]->SetSelection(m_aSelectedRegions[i]));
	}

	// Update rectangles
	SetChangedRects(bChanged);

}

void CBEObjectD::PermSelectDragRegion()
{
	// Deselect old regions, select drag region
	int i;
	BOOL nChanged[N_EDIT_REGIONS];
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		nChanged[i] = (i == m_nDragRegion);
		m_apEditRegions[i]->SetSelection(nChanged[i]);
		m_aSelectedRegions[i] = nChanged[i];
	}
	m_nDragRegion = -1;

	// Update rectangles
	SetChangedRects(nChanged);

}

void CBEObjectD::SelectAllRegions(BOOL bState)
{
	// Clear or set all regions
	int i;
	m_bSelected = bState;
	for (i = 0; i < 8; i++)
	{
		m_apEditRegions[i]->SetSelection(bState);
		m_aSelectedRegions[i] = bState;
	}
}

void CBEObjectD::SelectCornerRegions(BOOL bState)
{
	// Select corners, deselecting edges
	int i;
	BOOL bNewState;
	m_bSelected = FALSE;
	for (i = 0; i < 8; i++)
	{
		if (i < 4 || bState)
		{
			bNewState = (i < 4 && bState);
			m_apEditRegions[i]->SetSelection(bNewState);
			m_aSelectedRegions[i] = bNewState;
		}
		m_bSelected |= m_aSelectedRegions[i];
	}
}

void CBEObjectD::SelectEdgeRegions(BOOL bState, enum FlipTypesE flip /*= NO_FLIP*/)
{
	// Select edges, deselecting corners
	int i;
	BOOL bNewState, bFlipSide;
	enum SideTypeE side;
	m_bSelected = FALSE;
	for (i = 0; i < 8; i++)
	{
		bNewState = FALSE;
		if (i >= 4)
		{
			side = GetRegionSide(i);
			bFlipSide = (flip == NO_FLIP ||
				(flip == H_FLIP && (side == TOP_SIDE || side == BOTTOM_SIDE)) ||
				(flip == V_FLIP && (side == LEFT_SIDE || side == RIGHT_SIDE)));
			bNewState = (bState && bFlipSide);
		}
		if ((i >= 4 && bFlipSide) || bState)
		{
			m_apEditRegions[i]->SetSelection(bNewState);
			m_aSelectedRegions[i] = bNewState;
		}
		m_bSelected |= m_aSelectedRegions[i];
	}
}

void CBEObjectD::GetTiling(BOOL& bForce, int& nTiles, int& nMaxTiles)
{
	// If more than 1 edge is selected and tiling differs, indicate that
	// tiling is off, otherwise return info
	int i;
	int cnt = 0;
	int nAllTiles;
	int nAllMaxTiles;
	BOOL bCurrForce;
	bForce = TRUE;
	for (i = 4; i < 8; i++)
	{
		if (m_aSelectedRegions[i])
		{
			((CEdgeRegion*)m_apEditRegions[i])->GetTiling(bCurrForce, nTiles, nMaxTiles);
			bForce &= bCurrForce;
			if (++cnt >= 2)
			{
				if (nTiles != nAllTiles) 
				{
					bForce = FALSE;
				}
				if (nMaxTiles != nAllMaxTiles)
				{
					nAllMaxTiles = -1;
				}
			}
			nAllTiles = nTiles;
			nAllMaxTiles = nMaxTiles;
		}
	}
}

void CBEObjectD::SetTiling(BOOL bForce, int nTiles, int nMaxTiles)
{
	// Change tiling on all selected edges
	int i;
	for (i = 4; i < 8; i++)
	{
		if (m_aSelectedRegions[i])
		{
			((CEdgeRegion*)m_apEditRegions[i])->SetTiling(bForce, nTiles, nMaxTiles);
		}
	}
}

void CBEObjectD::GetAllTiling(BOOL bForce[], int nTiles[], int nMaxTiles[])
{
	int i;
	for (i = 0; i < 4; i++)
	{
		((CEdgeRegion*)m_apEditRegions[i+4])->GetTiling(bForce[i], nTiles[i], nMaxTiles[i]);
	}
}
	
void CBEObjectD::SetAllTiling(BOOL bForce[], int nTiles[], int nMaxTiles[])
{
	int i;
	for (i = 0; i < 4; i++)
	{
		((CEdgeRegion*)m_apEditRegions[i+4])->SetTiling(bForce[i], nTiles[i], nMaxTiles[i]);
	}
}

BOOL CBEObjectD::GetSizing(double& dEdgeSize)
{
	// If more than 1 edge is selected with different sizes, return FALSE
	// otherwise, return the edge size
	int i;
	int cnt = 0;
	double dAllSizes, dCurrSize;
	BOOL bReturn = FALSE;
	for (i = 4; i < 8; i++)
	{
		if (m_aSelectedRegions[i])
		{
			((CEdgeRegion*)m_apEditRegions[i])->GetSizing(dEdgeSize, m_aXOffsets, m_aYOffsets);

			// Round off dEdgeSize
			dCurrSize = ROUND(dEdgeSize, 1000);
			if (++cnt > 1 && dCurrSize != dAllSizes)
			{
				return (FALSE);
			}
			dAllSizes = dCurrSize;

			dEdgeSize = dCurrSize;
			bReturn = TRUE;
		}
	}

	return (bReturn);
}

void CBEObjectD::SetSizing(double dEdgeSize, BOOL aRegions[])
{
	// Change sizing on all selected edges
	int i;
	for (i = 4; i < 8; i++)
	{
		if (aRegions[i])
		{
			((CEdgeRegion*)m_apEditRegions[i])->SetSizing(dEdgeSize, m_aXOffsets, m_aYOffsets);
		}
	}
}

BOOL CBEObjectD::IsEdgeSelected()
{
	// Return TRUE if at least 1 edge is selected
	int i;
	for (i = 4; i < 8; i++)
	{
		if (m_aSelectedRegions[i])
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

int CBEObjectD::GetOppositeEdge(int nEdgeIndex)
{
	static enum SideTypeE aOpposites[] = {
		RIGHT_SIDE, BOTTOM_SIDE, LEFT_SIDE, TOP_SIDE
	};

	// Retrieve the opposite side
	int nOpposite = aOpposites[((CEdgeRegion*)m_apEditRegions[nEdgeIndex])->GetSide()];

	// add 4 to get into edge territory
	return (nOpposite+4);
}

enum SideTypeE CBEObjectD::GetRegionSide(int nEdgeIndex)
{
	return (((CEdgeRegion*)m_apEditRegions[nEdgeIndex])->GetSide());
}

void CBEObjectD::GetSelectionData(RefDataT* pRefData)
{
	// Fill in the structure with selected data
	// Drag region overrides
	if (m_nDragRegion != -1)
	{
		pRefData->dwID = m_apEditRegions[m_nDragRegion]->GetGraphicID();
		pRefData->pDatabase = m_pDatabase;
		return;
	}

	int i;
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if (m_aSelectedRegions[i])
		{
			pRefData->dwID = m_apEditRegions[i]->GetGraphicID();
			pRefData->pDatabase = m_pDatabase;
			break;
		}
	}
}

BOOL CBEObjectD::AreRegionsSelected(enum RegionGroupsE group)
{
	static int aGroupStates[][N_EDIT_REGIONS] = {
		{1, 1, 1, 1, 1, 1, 1, 1},	// ALL
		{1, 1, 1, 1, 0, 0, 0, 0},	// CORNERS
		{0, 0, 0, 0, 1, 1, 1, 1},	// EDGES
		{0, 0, 0, 0, 0, 1, 0, 1},	// H_EDGES
		{0, 0, 0, 0, 1, 0, 1, 0},	// V_EDGES
		{0, 0, 0, 0, 1, 1, 1, 1},	// EDGE
	};

	int i;
	BOOL bTruth = (group != EDGE_R) ? FALSE : TRUE;
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if (aGroupStates[group][i])
		{
			if (m_aSelectedRegions[i] == bTruth)
			{
				return (bTruth);
			}
		}
	}
	return (!bTruth);
}

// Workspace and grid sizing
void CBEObjectD::Resize(const RECT& rNewSpaceBound, const SIZE& sMinBorder)
{
	LONG ddXSize, ddYSize;		// size of border grid in device coordinates
	CRect rctBound(rNewSpaceBound);

	// Check for valid dimensions
	if (rctBound.Width() == 0 || rctBound.Height() == 0)
	{
		return;	// invalid space
	}

	// Compute device size
	if (m_dAspectRatio > (double)rctBound.Height() / (double)rctBound.Width())
	{
		// Assume height of rctBound is >= sMinBorder.cy * 2
		ddYSize = rctBound.Height() - (sMinBorder.cy << 1);
		ddXSize = (LONG)(ddYSize / m_dAspectRatio);		// ddXSize = dXSize * ddYSize / dYSize

		// Determine the anchor point
		m_aXCoords[0] = (rctBound.Width() - ddXSize) >> 1;
		m_aYCoords[0] = rctBound.TopLeft().y + sMinBorder.cy;
	}
	else
	{
		// Assume width of rctBound is >= sMinBorder.cx * 2
		ddXSize = rctBound.Width() - (sMinBorder.cx << 1);
		ddYSize = (LONG)(ddXSize * m_dAspectRatio);		// ddYSize = dYSize * ddXSize / dXSize

		// Determine the anchor point
		m_aXCoords[0] = rctBound.TopLeft().x + sMinBorder.cx;
		m_aYCoords[0] = (rctBound.Height() - ddYSize) >> 1;
	}

	// Compute device-to-inch coversion ratio
	m_dDPInchRatio = (m_dXSize != 0.) ?
		ddXSize / m_dXSize :
		ddYSize / m_dYSize;

	// Compute device grid coordinates
	ComputeGridCoords();
}

void CBEObjectD::ComputeGridCoords(BOOL bUseZoom/* = TRUE*/)
{
	int i;

	// Compute unrotated points
	for (i = 0; i < 3; i++) {
		if (bUseZoom)
		{
			m_aXCoords[i+1] = m_aXCoords[0] + (LONG)(m_aXZmOffsets[i] * m_dDPInchRatio);
			m_aYCoords[i+1] = m_aYCoords[0] + (LONG)(m_aYZmOffsets[i] * m_dDPInchRatio);
		}
		else
		{
			m_aXCoords[i+1] = m_aXCoords[0] + (LONG)(m_aXOffsets[i] * m_dDPInchRatio);
			m_aYCoords[i+1] = m_aYCoords[0] + (LONG)(m_aYOffsets[i] * m_dDPInchRatio);
		}
	}
}

void CBEObjectD::AdjustOffsets(BOOL bToZoom)
{
	// Scale regular-to-zoom or vice versa
	if (bToZoom)
	{
		ScaleOffsets(m_aXZmOffsets, m_aYZmOffsets, m_aXOffsets, m_aYOffsets, m_dZoomScale);
	}
	else
	{
		ScaleOffsets(m_aXOffsets, m_aYOffsets, m_aXZmOffsets, m_aYZmOffsets, 1.0/m_dZoomScale);
	}
}

void CBEObjectD::ScaleOffsets(double aXDest[], double aYDest[], double aXSrc[], double aYSrc[], double dScale)
{
	// Compute interior zoomed offsets
	aXDest[0] = aXSrc[0] * dScale;
	aYDest[0] = aYSrc[0] * dScale;
	aXDest[1] = aXSrc[2] - ((aXSrc[2] - aXSrc[1]) * dScale);
	aYDest[1] = aYSrc[2] - ((aYSrc[2] - aYSrc[1]) * dScale);
	aXDest[2] = aXSrc[2];
	aYDest[2] = aYSrc[2];
}

void CBEObjectD::CheckZoomedGrid()
{	
	// Check for cross-over
	LONG nHalfway;
	if (m_aXCoords[1] > m_aXCoords[2])
	{
		nHalfway = (m_aXCoords[3] - m_aXCoords[0]) / 2;
		m_aXCoords[1] = m_aXCoords[0] + nHalfway-1;
		m_aXCoords[2] = m_aXCoords[0] + nHalfway+1;
	}
	if (m_aYCoords[1] > m_aYCoords[2])
	{
		nHalfway = (m_aYCoords[3] - m_aYCoords[0]) / 2;
		m_aYCoords[1] = m_aYCoords[0] + nHalfway-1;
		m_aYCoords[2] = m_aYCoords[0] + nHalfway+1;
	}

}

// Drawing
int CBEObjectD::DrawGrid(CBEDrawInfo* pDrawInfo, CDC* pMemDC, BOOL bDrawBackground, BOOL bWipeBackground)
{
	int i;
	CDC* pDestDC = CDC::FromHandle(pDrawInfo->m_hdc);
	CRect rctRegionBounds;
	int nWidth, nHeight;

	// Draw the center rectangle
	if (bDrawBackground)
	{
		DrawGridRect(pDestDC, bWipeBackground);
	}

	// Background color
	pMemDC->SelectStockObject(WHITE_BRUSH);
	pDrawInfo->m_clBackground = (m_bUseBGColor) ? m_colBGColor : (COLORREF)-1L;

	// Draw the edit regions
	pDrawInfo->m_pClip = NULL;
	pDrawInfo->m_pDrawState = NULL;
	pDrawInfo->m_hdc = pMemDC->m_hDC;
	for (i = 0; i < 8; i++)
	{
		// Get bounds of the region, and see if it's not clipped
		m_apEditRegions[i]->GetBounds(rctRegionBounds);
		nWidth = rctRegionBounds.Width();
		nHeight = rctRegionBounds.Height();
		if (pDestDC->RectVisible(rctRegionBounds))
		{
			// Create a bitmap to draw region into
			CBitmap Bitmap;
			Bitmap.CreateCompatibleBitmap(pDestDC, nWidth, nHeight);
			CBitmap* pOldBitmap = pMemDC->SelectObject(&Bitmap);
			if (pOldBitmap != NULL)
			{
				pMemDC->Rectangle(-1, -1, nWidth+2, nHeight+2);

				// Draw the region into the background
				if (m_apEditRegions[i]->DrawRegion(theBorderApp.GetCallbacks(), pDrawInfo, NULL, TRUE))
				{
					// Delete the bitmap
					pMemDC->SelectObject(pOldBitmap);
					Bitmap.DeleteObject();
					return (-1);
				}

				// Blast to the screen
				pDestDC->BitBlt(
					rctRegionBounds.TopLeft().x, rctRegionBounds.TopLeft().y, 
					rctRegionBounds.Width(), rctRegionBounds.Height(),
					pMemDC, 
					0, 0,
					SRCCOPY);

				// Delete the bitmap
				pMemDC->SelectObject(pOldBitmap);
			}
			Bitmap.DeleteObject();
		}
	}

	return (0);
}

void CBEObjectD::DrawGridRect(CDC* pDC, BOOL bWipeBackground /*= TRUE*/, BOOL bWipeInner /*= TRUE*/)
{
	CRect rctBgBounds;
	
	if (bWipeBackground)
	{
		rctBgBounds.SetRect(m_aXCoords[0], m_aYCoords[0], m_aXCoords[3], m_aYCoords[3]);
		pDC->FillSolidRect(&rctBgBounds, RGB(255, 255, 255));
	}

	// Now draw inner
	if (bWipeInner)
	{
		rctBgBounds.SetRect(m_aXCoords[1], m_aYCoords[1], m_aXCoords[2]+1, m_aYCoords[2]+1);
		pDC->FillSolidRect(&rctBgBounds, BACKGROUND_RGB);
	}
}

int CBEObjectD::DrawSelections(CDC* pDC, BOOL bErase)
{
	int i;
	CRect rctRegionBounds;
	static POINT ptOffsets[] = {
		{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0, 1}, {1, 0}, {1, 1}, {1, 1}
	};

	// Draw the edit regions selection boxes
	for (i = 0; i < 8; i++)
	{
		// Get bounds of the region, and see if it's not clipped
		m_apEditRegions[i]->GetBounds(rctRegionBounds);
		if (pDC->RectVisible(rctRegionBounds))
		{
			m_apEditRegions[i]->DrawSelectionBox(pDC, ptOffsets[i], bErase);
		}
	}

	return (0);
}

int CBEObjectD::DrawGrid(CBEDrawInfo* pDrawInfo)
{
	int i;
	int nFirstRegion = 0;
	CRect rctBounds(m_aXCoords[0], m_aYCoords[0], m_aXCoords[3], m_aYCoords[3]);
	
	// Calculate center point
	CPoint ptCenter = rctBounds.CenterPoint();

	// Look for refresh info
	if (pDrawInfo->m_pDrawState != NULL)
	{
		SRedrawStateT* pRedrawState = (SRedrawStateT*)pDrawInfo->m_pDrawState;
		nFirstRegion = pRedrawState->nRegionIndex;
	}
	else
	{
		// Draw background
		DrawBackground(pDrawInfo);
	}

	// Background color (not used)
	pDrawInfo->m_clBackground = -1L;

	// Draw the edit regions
	for (i = nFirstRegion; i < 8; i++)
	{
		// Use last tiles for consistency
		if (i >= 4 && m_nZoomPosition == 0)
		{
			((CEdgeRegion*)m_apEditRegions[i])->UseLastTiles(TRUE);
		}
		
		if (m_apEditRegions[i]->DrawRegion(theBorderApp.GetCallbacks(), pDrawInfo, &ptCenter))
			return (-1);

		// Check for interrupt
		if (pDrawInfo->m_pDrawState != NULL)
		{
			SRedrawStateT* pRedrawState = (SRedrawStateT*)pDrawInfo->m_pDrawState;
			pRedrawState->nRegionIndex = i;
			break;
		}			

		// Clean up
		if (i >= 4)
		{
			((CEdgeRegion*)m_apEditRegions[i])->UseLastTiles(FALSE);
		}
	}

	return (0);
}

void CBEObjectD::DrawBackground(CBEDrawInfo* pDrawInfo)
{
	CBEBackgroundInfo stBgInfo = {
		pDrawInfo->m_hdc,
		{m_aXCoords[0], m_aYCoords[0], m_aXCoords[3], m_aYCoords[3]},
		{m_aXCoords[1], m_aYCoords[1], m_aXCoords[2], m_aYCoords[2]},
		pDrawInfo->m_dAngle,
		m_colBGColor,
		pDrawInfo->m_pClientData
	};

	// Draw it
	if (m_bUseBGColor)
	{
		theBorderApp.GetCallbacks()->DrawBackground(&stBgInfo);
	}
}

// Callback interface wrappers
DWORD CBEObjectD::ChooseGraphic(HWND hParent)
{
	DWORD dwID;

	theBorderApp.GetCallbacks()->ChooseGraphic(hParent, dwID, m_pDatabase);
	return (dwID);
}

DWORD CBEObjectD::ChooseGraphic(CString csFilePath, CString csFriendlyName, BOOL fCropCopyright)
{
	DWORD dwID;

	theBorderApp.GetCallbacks()->NewGraphic(dwID, m_pDatabase, csFilePath, csFriendlyName, fCropCopyright);
	return (dwID);
}

void CBEObjectD::RemoveRef(DWORD dwID)
{
	theBorderApp.GetCallbacks()->RemoveRef(dwID, m_pDatabase);	
}

// Operations
void CBEObjectD::SetRegionGraphics(DWORD dwID, BOOL aRegions[], BOOL bNew)
{
	DWORD dwIDs[1] = {dwID};
	SetAllRegionGraphics(dwIDs, aRegions, FALSE, bNew);
}

void CBEObjectD::SetRegionGraphics(DWORD dwIDs[], BOOL aRegions[], BOOL bNew)
{
	SetAllRegionGraphics(dwIDs, aRegions, TRUE, bNew);
}

void CBEObjectD::SetRegionGraphic(DWORD dwID, int nRegion, BOOL bUpdateRef)
{
	m_apEditRegions[nRegion]->SetGraphicID(theBorderApp.GetCallbacks(), dwID, bUpdateRef);
	m_apEditRegions[nRegion]->IntelligentRotate(theBorderApp.GetCallbacks());
}

void CBEObjectD::SetAllRegionGraphics(DWORD dwIDs[], BOOL aRegions[], BOOL bMulti, BOOL bUpdateRef)
{
	int i;
	CBECallbackInterface* pCBECallbacks = theBorderApp.GetCallbacks();

	for (i = 0; i < 8; i++)
	{
		if (aRegions[i])
		{
			m_apEditRegions[i]->SetGraphicID(pCBECallbacks, dwIDs[(bMulti) ? i : 0], bUpdateRef);
			m_apEditRegions[i]->IntelligentRotate(pCBECallbacks);
		}

	}

	// Update rectangles
	SetChangedRects(aRegions);
}


void CBEObjectD::GetRegionGraphics(DWORD dwIDs[])
{
	int i;
	for (i = 0; i < 8; i++)
	{
		dwIDs[i] = m_apEditRegions[i]->GetGraphicID();
	}
}

void CBEObjectD::SetRegionTransforms(CEditRegion::TransformsT transforms[])
{
	int i;
	for (i = 0; i < 8; i++)
	{
		m_apEditRegions[i]->SetTransforms(transforms[i]);
	}
}

void CBEObjectD::SetRegionTransforms(CEditRegion::TransformsT transforms, BOOL aRegions[])
{
	int i;
	for (i = 0; i < 8; i++)
	{
		if (aRegions[i])
		{
			m_apEditRegions[i]->SetTransforms(transforms);
		}
	}
}

void CBEObjectD::GetRegionTransforms(CEditRegion::TransformsT transforms[])
{
	int i;
	for (i = 0; i < 8; i++)
	{
		m_apEditRegions[i]->GetTransforms(transforms[i]);
	}
}

void CBEObjectD::RotateRegions(double dAngle, BOOL aRegions[])
{
	int i;

	// Rotate each valid region by the angle specified
	for (i = 0; i < 8; i++)
	{
		if (aRegions[i])
		{
			m_apEditRegions[i]->Rotate(dAngle);
		}
	}

	// Update rectangles
	SetChangedRects(aRegions);
}

void CBEObjectD::FlipRegions(enum FlipTypesE flip, BOOL aRegions[])
{
	int i;

	// Flip each valid region
	for (i = 0; i < 8; i++)
	{
		if (aRegions[i])
		{
			if (flip == H_FLIP)
			{
				m_apEditRegions[i]->FlipOverX();
			}
			else
			{
				m_apEditRegions[i]->FlipOverY();
			}
		}
	}

	// Update rectangles
	SetChangedRects(aRegions);
}

void CBEObjectD::ApplyRegionFlip(int nSrcRegion, BOOL aChangeRegions[])
{
	// Either do edge or corner flip, depending on source
	if (nSrcRegion < 4)
	{
		ApplyCornerFlip(nSrcRegion, aChangeRegions);
	}
	else
	{
		ApplyEdgeFlip(nSrcRegion, aChangeRegions);
	}
}

void CBEObjectD::ApplyEdgeFlip(int nSrcEdge, BOOL aChangeRegions[])
{
	static enum SideTypeE aFlips[][2] = {
		{RIGHT_SIDE, BOTTOM_SIDE},	// left side
		{RIGHT_SIDE, BOTTOM_SIDE},	// top side
		{LEFT_SIDE, TOP_SIDE},		// right side
		{LEFT_SIDE, TOP_SIDE},		// bottom side
	};

	// Apply a specified flip to the destination regions
	int i;
	enum SideTypeE srcSide = GetRegionSide(nSrcEdge);
	BOOL bXFlip = m_apEditRegions[nSrcEdge]->GetXFlip();
	BOOL bYFlip = m_apEditRegions[nSrcEdge]->GetYFlip();
	double dAngle = m_apEditRegions[nSrcEdge]->GetRotation();

	// Copy tiling info
	BOOL bForce;
	int nTiles, nMaxTiles;
	((CEdgeRegion*)m_apEditRegions[nSrcEdge])->GetTiling(bForce, nTiles, nMaxTiles);

	for (i = 4; i < N_EDIT_REGIONS; i++)
	{
		if (aChangeRegions[i])
		{
			// Apply rotation first
			m_apEditRegions[i]->SetRotation(dAngle);
			
			// Check for H-flip
			m_apEditRegions[i]->SetXFlip(
				(GetRegionSide(i) == aFlips[srcSide][H_FLIP]) ? !bXFlip : bXFlip);

			// Check for V-flip
			m_apEditRegions[i]->SetYFlip(
				(GetRegionSide(i) == aFlips[srcSide][V_FLIP]) ? !bYFlip : bYFlip);

			// Apply tiling
			((CEdgeRegion*)m_apEditRegions[i])->SetTiling(bForce, nTiles, nMaxTiles);

		}
	}
}

void CBEObjectD::ApplyCornerFlip(int nSrcCorner, BOOL aChangeRegions[])
{
	static int aFlipSequences[][3] = {
		{1, 2, 3},
		{0, 3, 2},
		{3, 0, 1},
		{2, 1, 0}
	};

	// Apply a specified flip to the destination regions
	int i;
	BOOL bXFlip = m_apEditRegions[nSrcCorner]->GetXFlip();
	BOOL bYFlip = m_apEditRegions[nSrcCorner]->GetYFlip();
	double dAngle = m_apEditRegions[nSrcCorner]->GetRotation();

	for (i = 0; i < 4; i++)
	{
		if (aChangeRegions[i])
		{
			// Apply rotation first
			m_apEditRegions[i]->SetRotation(dAngle);
			
			// Copy flips
			m_apEditRegions[i]->SetXFlip(bXFlip);
			m_apEditRegions[i]->SetYFlip(bYFlip);

			// Do H-flip
			if (i == aFlipSequences[nSrcCorner][H_FLIP] || i == aFlipSequences[nSrcCorner][HV_FLIP])
			{
				m_apEditRegions[i]->SetXFlip(!bXFlip);
			}

			// Do V-flip
			if (i == aFlipSequences[nSrcCorner][V_FLIP] || i == aFlipSequences[nSrcCorner][HV_FLIP])
			{
				m_apEditRegions[i]->SetYFlip(!bYFlip);
			}
		}
	}
}

void CBEObjectD::ApplyEdgeCopy(int nSrcEdge, BOOL aChangeRegions[])
{
	// Apply a specified rotation to the destination regions
	int i;
	int nDestRegion;
	double dAngle = m_apEditRegions[nSrcEdge]->GetRotation();

	// Copy tiling info
	BOOL bForce;
	int nTiles, nMaxTiles;
	((CEdgeRegion*)m_apEditRegions[nSrcEdge])->GetTiling(bForce, nTiles, nMaxTiles);

	// Do this for 3 edges
	for (i = 0, nDestRegion = nSrcEdge + 1; i < 3; i++, nDestRegion++)
	{
		// Compute valid destination edge
		if (nDestRegion == N_EDIT_REGIONS) nDestRegion = 4;

		if (aChangeRegions[nDestRegion])
		{
			// Apply rotation first
			if ((dAngle += RAD_90) >= _2PI)
			{
				dAngle -= _2PI;
			}
			m_apEditRegions[nDestRegion]->SetRotation(dAngle);
			
			// Apply tiling
			((CEdgeRegion*)m_apEditRegions[nDestRegion])->SetTiling(bForce, nTiles, nMaxTiles);

		}
	}
}

void CBEObjectD::ClearChangedRect()
{
	// Set to size of whole border
	m_adChangedRect[0] = 0.;
	m_adChangedRect[1] = 0.;
	m_adChangedRect[2] = m_dXSize;
	m_adChangedRect[3] = m_dYSize;
}

void CBEObjectD::SetChangedRects(BOOL nChanged[])
{
	// Set up changed rects
	int i;
	int nIndex = 0;
	CRect rctBounds;
	double dLeft, dTop, dRight, dBottom;
	static int aLeftIndex[] = {-1, 1, -1, 1, -1, 0, 1, 0};
	static int aTopIndex[] = {-1, -1, 1, 1, 0, -1, 0, 1};

	// Minimize normalized changed rect
	m_adChangedRect[0] = m_dXSize;
	m_adChangedRect[1] = m_dYSize;
	m_adChangedRect[2] = 0.;
	m_adChangedRect[3] = 0.;

	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if (nChanged[i])
		{
			m_apEditRegions[i]->GetBounds(rctBounds);
			m_aRectArray[nIndex++] = rctBounds;

			// Possibly expand changed rect
			dLeft = (aLeftIndex[i] != -1) ? m_aXOffsets[aLeftIndex[i]] : 0.;
			if (dLeft < m_adChangedRect[0]) m_adChangedRect[0] = dLeft;
			dTop = (aTopIndex[i] != -1) ? m_aYOffsets[aTopIndex[i]] : 0.;
			if (dTop < m_adChangedRect[1]) m_adChangedRect[1] = dTop;
			dRight = m_aXOffsets[aLeftIndex[i]+1];
			if (dRight > m_adChangedRect[2]) m_adChangedRect[2] = dRight;
			dBottom = m_aXOffsets[aTopIndex[i]+1];
			if (dBottom > m_adChangedRect[3]) m_adChangedRect[3] = dBottom;
		}
	}

	// Set # of valid rects
	m_aRectArray.SetNValidItems(nIndex);
}

void CBEObjectD::Revert()
{
	// Restore original data
	SetData(sizeof(SRawDataT), (void*)&m_OriginalData);	

	// Default zoom position
	m_nZoomPosition = 0;
	m_dZoomScale = 1.0;

	// Compute zoom offsets
	ScaleOffsets(m_aXZmOffsets, m_aYZmOffsets, m_aXOffsets, m_aYOffsets, m_dZoomScale);

}