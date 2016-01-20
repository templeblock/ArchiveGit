// commands.cpp : implementation file
//

#include "stdafx.h"
#include "border.h"
#include "Borddoc.h"
#include "Bordview.h"

// Undo-able commands
	
// Slide bars causing grid to change
CCmdSetGridCoords::CCmdSetGridCoords(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{

}	

void CCmdSetGridCoords::SetGridCoords(int nXIndex1, int nYIndex1, LONG nNewXValue1, LONG nNewYValue1, int nXIndex2, int nYIndex2, LONG nNewXValue2, LONG nNewYValue2)
{
	// Save old and new values
	m_nXIndex[0] = nXIndex1;
	m_nXIndex[1] = nXIndex2;
	m_nYIndex[0] = nYIndex1;
	m_nYIndex[1] = nYIndex2;

	m_nNewXValues[0] = nNewXValue1;
	m_nNewXValues[1] = nNewXValue2;
	m_nNewYValues[0] = nNewYValue1;
	m_nNewYValues[1] = nNewYValue2;

	// Retrieve current state
	int i;
	for (i = 0; i < 2; i++)
	{
		if (m_nXIndex[i] != -1)
		{
			m_nOldXValues[i] = m_pBEObj->GetXCoord(m_nXIndex[i]);
		}
		if (m_nYIndex[i] != -1)
		{
			m_nOldYValues[i] = m_pBEObj->GetYCoord(m_nYIndex[i]);
		}
	}
}

// Do this command.
BOOL CCmdSetGridCoords::DoIt()
{
	// Perform the action
	int i;
	for (i = 0; i < 2; i++)
	{
		if (m_nXIndex[i] != -1)
		{
			m_pBEObj->SetXCoord(m_nXIndex[i], m_nNewXValues[i]);
		}
		if (m_nYIndex[i] != -1)
		{
			m_pBEObj->SetYCoord(m_nYIndex[i], m_nNewYValues[i]);
		}
	}
	m_pBEObj->AdjustOffsets(FALSE);
	m_pDoc->UpdateAllViews(NULL, CBordview::AdjustGridHandles, m_pDoc);

	return (TRUE);
}

// Undo this command.
BOOL CCmdSetGridCoords::UndoIt()
{
	// Perform the action
	int i;
	for (i = 0; i < 2; i++)
	{
		if (m_nXIndex[i] != -1)
		{
			m_pBEObj->SetXCoord(m_nXIndex[i], m_nOldXValues[i]);
		}
		if (m_nYIndex[i] != -1)
		{
			m_pBEObj->SetYCoord(m_nYIndex[i], m_nOldYValues[i]);
		}
	}
	m_pBEObj->AdjustOffsets(FALSE);
	m_pDoc->UpdateAllViews(NULL, CBordview::AdjustGridHandles, m_pDoc);

	return (TRUE);
}	

// Rotate a region 90 degrees
CCmdRotateRegion::CCmdRotateRegion(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{
	m_dAngle = RAD_90;
}

void CCmdRotateRegion::SetMembers(double dAngle /*= RAD_90*/)
{
	m_pBEObj->CopySelectionStates(m_aSelectedRegions);
	m_dAngle = dAngle;

}

BOOL CCmdRotateRegion::DoIt()
{
	// Perform the action
	m_pBEObj->RotateRegions(m_dAngle, m_aSelectedRegions);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());

	return (TRUE);
}

BOOL CCmdRotateRegion::UndoIt()
{
	// Perform the action
	m_pBEObj->RotateRegions(-m_dAngle, m_aSelectedRegions);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());

	return (TRUE);
}

// Flip a region horizontally or vertically
CCmdFlipRegion::CCmdFlipRegion(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{
}

BOOL CCmdFlipRegion::DoIt()
{
	// Perform the action
	m_pBEObj->FlipRegions(m_Flip, m_aSelectedRegions);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());

	return (TRUE);
}

BOOL CCmdFlipRegion::UndoIt()
{
	// Undo the action
	m_pBEObj->FlipRegions(m_Flip, m_aSelectedRegions);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());

	return (TRUE);
}


// Changes region settings
CCmdChangeSettings::CCmdChangeSettings(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{
}
	
void CCmdChangeSettings::ChangeSettings(CSettingsDlg& SettingsDlg)
{
	// Retrieve current values
	m_pBEObj->GetAllTiling(m_bForceTiles, m_nTiles, m_nMaxTiles);
	m_pBEObj->GetXOffsets(m_aXOffsets);
	m_pBEObj->GetYOffsets(m_aYOffsets);

	// Save changes
	m_Settings = SettingsDlg.m_Settings;

	// Save selected regions
	m_pBEObj->CopySelectionStates(m_aSelectedRegions);
}

BOOL CCmdChangeSettings::DoIt()
{
	// Perform the action
	m_Settings.TransferSettings(m_aSelectedRegions);
	m_pDoc->UpdateAllViews(NULL, CBordview::AdjustGridHandles, m_pDoc);

	return (TRUE);
}

BOOL CCmdChangeSettings::UndoIt()
{
	// Put back the old settings
	m_pBEObj->SetAllTiling(m_bForceTiles, m_nTiles, m_nMaxTiles);
	int i;
	for (i = 0; i < 2; i++)
	{
		m_pBEObj->SetXOffset(i, m_aXOffsets[i]);
		m_pBEObj->SetYOffset(i, m_aYOffsets[i]);
	}
	m_pBEObj->AdjustOffsets(TRUE);
	m_pBEObj->ComputeGridCoords();
	m_pDoc->UpdateAllViews(NULL, CBordview::AdjustGridHandles, m_pDoc);

	return (TRUE);
}

// Changes background color
CCmdChangeBGColor::CCmdChangeBGColor(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{
}
	
void CCmdChangeBGColor::ChangeBGColor(BOOL bUseBGColor, COLORREF colBGColor)
{
	// Retrieve current values
	 m_bOldUseBGColor = m_pBEObj->GetBGColor(m_colOldBGColor);

	// Save changes
	m_bNewUseBGColor = bUseBGColor;
	m_colNewBGColor = colBGColor;
}

BOOL CCmdChangeBGColor::DoIt()
{
	// Perform the action
	m_pBEObj->SetBGColor(m_bNewUseBGColor, m_colNewBGColor);
	m_pDoc->UpdateAllViews(NULL, CBordview::Redraw);

	return (TRUE);
}

BOOL CCmdChangeBGColor::UndoIt()
{
	// Put back the old settings
	m_pBEObj->SetBGColor(m_bOldUseBGColor, m_colOldBGColor);
	m_pDoc->UpdateAllViews(NULL, CBordview::Redraw);

	return (TRUE);
}
		
// Choose a graphic
CCmdChooseGraphic::CCmdChooseGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{
}

void CCmdChooseGraphic::ChooseGraphic(DWORD dwID)
{
	m_pBEObj->CopySelectionStates(m_aSelectedRegions);
	m_dwNewID = dwID;
	m_bNewCommand = TRUE;
	m_bForceUpdate = FALSE;

	// Check for drag drop
	int nDragRegion = m_pBEObj->GetDragRegion();
	if (nDragRegion != -1)
	{
		int i;
		for (i = 0; i < N_EDIT_REGIONS; i++)
		{
			m_aSelectedRegions[i] = (i == nDragRegion);
		}
	}

	// Retrieve current IDs & transforms
	m_pBEObj->GetRegionGraphics(m_dwIDs);
	m_pBEObj->GetRegionTransforms(m_aTransforms);

}

BOOL CCmdChooseGraphic::DoIt()
{
	CEditRegion::TransformsT newTransforms = {FALSE, FALSE, 0.0};

	// Perform the action
	m_pBEObj->SetRegionTransforms(newTransforms, m_aSelectedRegions);
	m_pBEObj->SetRegionGraphics(m_dwNewID, m_aSelectedRegions, m_bNewCommand);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects | CBordview::NotSelectionsOnly, m_pBEObj->GetRectList());
	m_bNewCommand = FALSE;

	return (TRUE);
}

BOOL CCmdChooseGraphic::UndoIt()
{
	// Undo the action
	m_pBEObj->SetRegionTransforms(m_aTransforms);
	m_pBEObj->SetRegionGraphics(m_dwIDs, m_aSelectedRegions, m_bNewCommand);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());

	return (TRUE);
}

// Abstract edge copy
CCmdCopyRegion::CCmdCopyRegion(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{
}

void CCmdCopyRegion::SaveStates()
{
	// Save region graphics & flip states
	m_pBEObj->GetRegionGraphics(m_dwIDs);
	m_pBEObj->GetRegionTransforms(m_aTransforms);
	if (m_nSrcRegion != -1)
	{
		m_dwSrcID = m_dwIDs[m_nSrcRegion];
	}
}

BOOL CCmdCopyRegion::DoIt()
{
	// Perform the action
	m_pBEObj->ApplyRegionFlip(m_nSrcRegion, m_aDestRegions);
	m_pBEObj->SetRegionGraphics(m_dwSrcID, m_aDestRegions, m_bNewCommand);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());
	m_bNewCommand = FALSE;

	return (TRUE);
}

BOOL CCmdCopyRegion::UndoIt()
{
	// Undo the action
	m_pBEObj->SetRegionTransforms(m_aTransforms);
	m_pBEObj->SetRegionGraphics(m_dwIDs, m_aDestRegions, m_bNewCommand);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());

	return (TRUE);
}

// Copy an edge vertically/horizontal
CCmdCopyOpposite::CCmdCopyOpposite(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CCmdCopyRegion(pDoc, pBEObject)
{
}

void CCmdCopyOpposite::CopyOpposite()
{
	m_nSrcRegion = -1;	// not used
	CCmdCopyRegion::SaveStates();
	
	// Retrieve the currently selected region
	int i;
	BOOL aSelectedRegions[N_EDIT_REGIONS];
	m_pBEObj->CopySelectionStates(aSelectedRegions);
	memset(m_aDestRegions, 0, sizeof(BOOL)*N_EDIT_REGIONS);
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if ((m_aSrcRegions[i] = (aSelectedRegions[i] && i >= 4 && m_dwIDs[i])))
		{
			// Determine the opposite edge
			int nDestEdge = m_pBEObj->GetOppositeEdge(i);
			m_aDestRegions[nDestEdge] = TRUE;
		}
	}

}

BOOL CCmdCopyOpposite::DoIt()
{
	// Perform the action
	int i;
	BOOL aDestRegions[N_EDIT_REGIONS];
	BOOL aChanged[N_EDIT_REGIONS];

	// Clear arrays
	memset(aDestRegions, 0, sizeof(BOOL)*N_EDIT_REGIONS);
	memset(aChanged, 0, sizeof(BOOL)*N_EDIT_REGIONS);

	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		if (m_aSrcRegions[i])
		{
			int nDestEdge = m_pBEObj->GetOppositeEdge(i);
			aDestRegions[nDestEdge] = TRUE;
			m_pBEObj->ApplyRegionFlip(i, aDestRegions);
			m_pBEObj->SetRegionGraphic(m_dwIDs[i], nDestEdge, m_bNewCommand);
			aDestRegions[nDestEdge] = FALSE;
			aChanged[nDestEdge] = TRUE;
		}
	}
	m_pBEObj->SetChangedRects(aChanged);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());
	m_bNewCommand = FALSE;

	return (TRUE);
}

BOOL CCmdCopyOpposite::UndoIt()
{
	// Undo the action
	return (CCmdCopyRegion::UndoIt());
}


// Copy an edge to all edges
CCmdCopyEdges::CCmdCopyEdges(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CCmdCopyRegion(pDoc, pBEObject)
{
}

void CCmdCopyEdges::CopyEdges()
{
	// Retrieve the currently selected region, and 
	// mark all the others
	int i;
	BOOL aSelectedRegions[N_EDIT_REGIONS];
	m_pBEObj->CopySelectionStates(aSelectedRegions);
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		m_aDestRegions[i] = FALSE;
		if (i >= 4)
		{
			m_aDestRegions[i] = !aSelectedRegions[i];
			if (aSelectedRegions[i])
			{
				m_nSrcRegion = i;
			}
		}
	}

	CCmdCopyRegion::SaveStates();
}

BOOL CCmdCopyEdges::DoIt()
{
	// Perform the action
	m_pBEObj->ApplyEdgeCopy(m_nSrcRegion, m_aDestRegions);
	m_pBEObj->SetRegionGraphics(m_dwSrcID, m_aDestRegions, m_bNewCommand);
	m_pDoc->UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pBEObj->GetRectList());
	m_bNewCommand = FALSE;
	
	return (TRUE);
}

BOOL CCmdCopyEdges::UndoIt()
{
	// Undo the action
	return (CCmdCopyRegion::UndoIt());
}

// Copy a corner to all corners
CCmdCopyCorners::CCmdCopyCorners(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CCmdCopyRegion(pDoc, pBEObject)
{
}

void CCmdCopyCorners::CopyCorners()
{
	// Retrieve the currently selected region, and 
	// mark all the others
	int i;
	BOOL aSelectedRegions[N_EDIT_REGIONS];
	m_pBEObj->CopySelectionStates(aSelectedRegions);
	for (i = 0; i < N_EDIT_REGIONS; i++)
	{
		m_aDestRegions[i] = FALSE;
		if (i < 4)
		{
			m_aDestRegions[i] = !aSelectedRegions[i];
			if (aSelectedRegions[i])
			{
				m_nSrcRegion = i;
			}
		}
	}

	CCmdCopyRegion::SaveStates();
}

BOOL CCmdCopyCorners::DoIt()
{
	// Perform the action
	return (CCmdCopyRegion::DoIt());
}

BOOL CCmdCopyCorners::UndoIt()
{
	// Undo the action
	return (CCmdCopyRegion::UndoIt());
}

// Cut/copy/paste graphics
// -Cut-
CCmdCutGraphic::CCmdCutGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CCmdChooseGraphic(pDoc, pBEObject)
{
}

void CCmdCutGraphic::CutGraphic()
{
	// Set up for a clear of selections
	CCmdChooseGraphic::ChooseGraphic(0);
}

BOOL CCmdCutGraphic::DoIt()
{
	// Perform the action	
	// Put graphic on the clipboard
	m_pDoc->CopyGraphic();

	// Choose the graphic
	CCmdChooseGraphic::DoIt();

	return (TRUE);
}

BOOL CCmdCutGraphic::UndoIt()
{
	// Undo the action
	CCmdChooseGraphic::UndoIt();

	return (TRUE);
}

// -Copy-
CCmdCopyGraphic::CCmdCopyGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CCmdChooseGraphic(pDoc, pBEObject)
{
}

BOOL CCmdCopyGraphic::DoIt()
{
	// Perform the action	
	// Put graphic on the clipboard
	m_pDoc->CopyGraphic();

	return (TRUE);
}

BOOL CCmdCopyGraphic::UndoIt()
{
	return (TRUE);
}

// -Paste-
CCmdPasteGraphic::CCmdPasteGraphic(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CCmdChooseGraphic(pDoc, pBEObject)
{
}

void CCmdPasteGraphic::InitPasteGraphic(COleDataObject* pDataObject)
{
	// Get graphic from the clipboard
	RefDataT refData;
	UINT uFormat = 0;

	// Determine which data is available
	if (pDataObject->IsDataAvailable(CBordview::m_cfDataExchangeID))
	{
		// The internal format takes precendence!
		uFormat = CBordview::m_cfDataExchangeID;
	}
	else if (pDataObject->IsDataAvailable(CBordview::m_cfDataExchangeName))
	{
		uFormat = CBordview::m_cfDataExchangeName;
	}
	if (uFormat)
	{
		m_pDoc->PasteFromOleDataObject(uFormat, pDataObject, &refData);
	}
	
	// Set up for a paste
	CCmdChooseGraphic::ChooseGraphic(refData.dwID);
}

BOOL CCmdPasteGraphic::DoIt()
{
	// Perform the action	
	// Choose the graphic
	m_bForceUpdate = TRUE;
	CCmdChooseGraphic::DoIt();

	return (TRUE);
}

BOOL CCmdPasteGraphic::UndoIt()
{
	// Undo the action
	CCmdChooseGraphic::UndoIt();

	return (TRUE);
}

// Zoom operation
CCmdZoom::CCmdZoom(CBorddoc* pDoc, CBEObjectD* pBEObject) :
	CBECommand(pDoc, pBEObject)
{

}

BOOL CCmdZoom::DoIt()
{
	// Perform the action	
	m_pBEObj->Zoom(m_zoom);
	m_pDoc->UpdateAllViews(NULL, CBordview::AdjustGridHandles, m_pDoc);

	return (TRUE);
}

BOOL CCmdZoom::UndoIt()
{
	// Undo the action
	m_pBEObj->Zoom((m_zoom == CBEObjectD::ZoomOut) ? CBEObjectD::ZoomIn : CBEObjectD::ZoomOut);
	m_pDoc->UpdateAllViews(NULL, CBordview::AdjustGridHandles, m_pDoc);

	return (TRUE);
}

