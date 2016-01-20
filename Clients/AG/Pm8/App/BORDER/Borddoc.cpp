// Borddoc.cpp : implementation file
//

#include "stdafx.h"
#include "mainfrm.h"
#include "border.h"
#include "Borddoc.h"
#include "Bordview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UNDO_LIMIT = 10;

const MIN_SPACE_W = 20;
const MIN_SPACE_H = 20;

static CColorPalette BackgroundColorWindow;

BOOL PopupIsActive(void)
{
   return BackgroundColorWindow.GetSafeHwnd() != NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CBorddoc

IMPLEMENT_DYNCREATE(CBorddoc, CDocument)

CBorddoc::CBorddoc()
{
	m_pCurrBEObj = NULL;

	// Set up undo
	m_CommandList.SetUndoLimit(UNDO_LIMIT);
	CEditRegion::SetRefLimit(UNDO_LIMIT+1);

	// Drawing
	m_bFirstDraw = TRUE;


}

BOOL CBorddoc::OnNewDocument()
{	
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CBorddoc::~CBorddoc()
{
	// Deselect all edit regions
	if (m_pCurrBEObj != NULL)
	{
		m_pCurrBEObj->SelectAllRegions(FALSE);
	}

}


BEGIN_MESSAGE_MAP(CBorddoc, CDocument)
	//{{AFX_MSG_MAP(CBorddoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, OnUpdateZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateZoomOut)
	ON_COMMAND(ID_ROTATE_90, OnRotate90)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_90, OnUpdateTransforms)
	ON_COMMAND(ID_FLIP_HORIZONTAL, OnFlipHorizontal)
	ON_COMMAND(ID_FLIP_VERTICAL, OnFlipVertical)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnUpdateSettings)
	ON_COMMAND(ID_CHOOSE_GRAPHIC, OnChooseGraphic)
	ON_UPDATE_COMMAND_UI(ID_CHOOSE_GRAPHIC, OnUpdateChooseGraphic)
	ON_COMMAND(ID_SELECT_CORNERS, OnSelectCorners)
	ON_COMMAND(ID_SELECT_EDGES, OnSelectEdges)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_COPY_EDGES, OnCopyEdges)
	ON_UPDATE_COMMAND_UI(ID_COPY_EDGES, OnUpdateCopyEdges)
	ON_COMMAND(ID_COPY_CORNERS, OnCopyCorners)
	ON_UPDATE_COMMAND_UI(ID_COPY_CORNERS, OnUpdateCopyCorners)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCutCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_SELECT_H_EDGES, OnSelectHEdges)
	ON_COMMAND(ID_SELECT_V_EDGES, OnSelectVEdges)
	ON_UPDATE_COMMAND_UI(ID_SELECT_ALL, OnUpdateSelectAll)
	ON_UPDATE_COMMAND_UI(ID_SELECT_CORNERS, OnUpdateSelectCorners)
	ON_UPDATE_COMMAND_UI(ID_SELECT_EDGES, OnUpdateSelectEdges)
	ON_UPDATE_COMMAND_UI(ID_SELECT_H_EDGES, OnUpdateSelectHEdges)
	ON_UPDATE_COMMAND_UI(ID_SELECT_V_EDGES, OnUpdateSelectVEdges)
	ON_COMMAND(ID_CHOOSE_BG_COLOR, OnChooseBgColor)
	ON_UPDATE_COMMAND_UI(ID_CHOOSE_BG_COLOR, OnUpdateChooseBgColor)
	ON_COMMAND(ID_BACKGROUND_COLOR_PALETTE, OnBackgroundColorPalette)
	ON_COMMAND(ID_COPY_ACROSS, OnCopyAcross)
	ON_UPDATE_COMMAND_UI(ID_COPY_ACROSS, OnUpdateCopyAcross)
	ON_COMMAND(ID_REVERT, OnRevert)
	ON_UPDATE_COMMAND_UI(ID_REVERT, OnUpdateRevert)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCutCopy)
	ON_UPDATE_COMMAND_UI(ID_FLIP_HORIZONTAL, OnUpdateTransforms)
	ON_UPDATE_COMMAND_UI(ID_FLIP_VERTICAL, OnUpdateTransforms)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBorddoc diagnostics

#ifdef _DEBUG
void CBorddoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBorddoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBorddoc serialization

void CBorddoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBorddoc commands

// Attributes
void CBorddoc::SetCurrBEObj(CBEObjectD* pBEObject)
{
	m_pCurrBEObj = pBEObject;
	UpdateAllViews(NULL, CBordview::SetHandleLimits, this);

	// Clear selection
	m_bSelectAll = TRUE;
	m_bSelectCorners = TRUE;
	m_bSelectEdges = TRUE;
	m_bSelectHEdges = TRUE;
	m_bSelectVEdges = TRUE; 
}

BOOL CBorddoc::AddCommand(CBECommand* pCommand, BOOL bDoIt /*= TRUE*/)
{
// Actually do the command if requested.
	if (bDoIt)
	{
		// Clear change
		m_pCurrBEObj->ClearChangedRect();

		// Do the command
		SetModifiedFlag();
		if (!pCommand->DoIt())
		{
			return (FALSE);
		}

		// Notify the client of a change
		m_pCallbackInterface->Notify(CBECallbackInterface::BENotifyBorderChange, m_pCurrBEObj->GetDatabase(), (void*)m_pCurrBEObj->GetChangedRect());
	}
// Add the command to the undo list.
	m_CommandList.AddCommand(pCommand);
	return (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CBorddoc Operations
void CBorddoc::Resize(RECT& rNewSpaceBound, SIZE& sMinBorder)
{
	if (m_pCurrBEObj != NULL)
	{
		RECT rctNewBound = rNewSpaceBound;
		int nMinWidth = sMinBorder.cx * 2 + MIN_SPACE_W;
		int nMinHeight = sMinBorder.cy * 2 + MIN_SPACE_H;

		// Check for minimum bound
		if (rctNewBound.right - rctNewBound.left < nMinWidth)
		{
			rctNewBound.right = rctNewBound.left + nMinWidth;
		}
		if (rctNewBound.bottom - rctNewBound.top < nMinHeight)
		{
			rctNewBound.bottom = rctNewBound.top + nMinHeight;
		}
		m_bFirstDraw = TRUE;
		m_pCurrBEObj->Resize(rctNewBound, sMinBorder);
		UpdateAllViews(NULL, CBordview::AdjustGridHandles | CBordview::ComputeMinRect, this);
	}
}

BOOL CBorddoc::DrawBEGrid(CDC* pDC, CDC* pMemDC, CWnd* pDrawWnd, BOOL bDrawBackground)
{
	CBEDrawInfo stDrawInfo;

	// Set up skeleton draw info struct
	stDrawInfo.m_hdc = pDC->m_hDC;
	stDrawInfo.m_hWnd = pDrawWnd->m_hWnd;
	stDrawInfo.m_dAngle = 0.;
	stDrawInfo.m_fXFlipped = FALSE;
	stDrawInfo.m_fYFlipped = FALSE;
	stDrawInfo.m_pClientData = NULL;
	stDrawInfo.m_pDrawState = NULL;
	stDrawInfo.m_fAllowInterrupt = FALSE;

	// Draw graphics in the grid, with selection boxes on top
	m_pCurrBEObj->DrawGrid(&stDrawInfo, pMemDC, bDrawBackground, m_bFirstDraw);
	m_bFirstDraw = FALSE;

	return (TRUE);
}

BOOL CBorddoc::DrawMinBEGrid(CDC* pDC)
{
	// Wipe the grid background clear
	m_pCurrBEObj->DrawGridRect(pDC, TRUE, FALSE);

	return (TRUE);
}

BOOL CBorddoc::SelectBEEditRegion(BOOL bKeep, BOOL bToggle, CPoint point)
{
	// Select regions, and build up area of changed rectangles
	BOOL r;
	r = m_pCurrBEObj->SelectEditRegion(bKeep, bToggle, point);
	UpdateAllViews(NULL, CBordview::DrawChangedRects | CBordview::SelectionsOnly, m_pCurrBEObj->GetRectList());
	if (r)
	{
		// Possibly begin drag-drop
		COleDataSource* pData = new COleDataSource; 
		CopyOleDataObject(pData);
		if (pData->DoDragDrop(DROPEFFECT_COPY) == DROPEFFECT_NONE)
		{
			m_pCurrBEObj->SetDragRegion(-1);
		}
		delete pData;
	}

	return (r);
}

BOOL CBorddoc::SelectBEDragRegion(CPoint* point)
{
	// Select regions, and build up area of changed rectangles
	BOOL r = m_pCurrBEObj->SelectDragRegion(point);
	if (point == NULL)
	{
		m_pCurrBEObj->ReselectRegions();
	}
	UpdateAllViews(NULL, CBordview::DrawChangedRects | CBordview::SelectionsOnly, m_pCurrBEObj->GetRectList());
	return (r);
}

BOOL CBorddoc::PermSelectBEDragRegion()
{
	// Permanently select the drag region
	m_pCurrBEObj->PermSelectDragRegion();
	UpdateAllViews(NULL, CBordview::DrawChangedRects, m_pCurrBEObj->GetRectList());
	return (TRUE);
}

int CBorddoc::PasteFromOleDataObject(UINT uFormat, COleDataObject* pDataObject, RefDataT* pRefData)
{

	HGLOBAL hData;
	int error = 0;

	if ((hData = pDataObject->GetGlobalData(uFormat)) != NULL)
	{
		LPVOID pData;
		BOOL fLockData = (uFormat != CF_BITMAP);

		if (fLockData && (pData = ::GlobalLock(hData)) == NULL)
		{
			error = -1;
		}
		else
		{
			/* We got the data. */
			if (uFormat == CBordview::m_cfDataExchangeName)
			{
				CBrowserDataExchange* pExchange = (CBrowserDataExchange*)pData;
				CString csFilePath = ((LPCSTR)pExchange) + (int)pExchange->m_dwFilePathOffset;
				CString csFriendlyName = ((LPCSTR)pExchange) + (int)pExchange->m_dwFriendlyNameOffset;
				BYTE fCropCopyright = *(((LPCSTR)pExchange) + (int)pExchange->m_dwCropCopyrightOffset);

				// Create the graphic and "choose" it
				pRefData->dwID = m_pCurrBEObj->ChooseGraphic(csFilePath, csFriendlyName, fCropCopyright);
				pRefData->pDatabase = m_pCurrBEObj->GetDatabase();
			}
			else if (uFormat == CBordview::m_cfDataExchangeID)
			{
				pRefData->dwID = ((RefDataT*)pData)->dwID;
				pRefData->pDatabase = ((RefDataT*)pData)->pDatabase;
			}

			if (fLockData)
			{
				::GlobalUnlock(hData);
			}
		}
	}

	return (error);
}

void CBorddoc::CopyGraphic()
{
	COleDataSource* pData = new COleDataSource; 
	CopyOleDataObject(pData);
	pData->SetClipboard();
}

int CBorddoc::CopyOleDataObject(COleDataSource* pData)
{
	int error = 0;
	
	// Create a structure with the data and cache it to the object
	HGLOBAL hData;
	if ((hData = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(RefDataT))) != NULL)
	{
		RefDataT* pRefData;

		if ((pRefData = (RefDataT*)::GlobalLock(hData)) == NULL)
		{
			::GlobalFree(hData);
			error = -1;
		}
		else
		{
			m_pCurrBEObj->GetSelectionData(pRefData);
			::GlobalUnlock(hData);

			// Transfer data to the clipboard
			pData->CacheGlobalData(CBordview::m_cfDataExchangeID, hData);
		}
	}
	
	return (error);
}

/////////////////////////////////////////////////////////////////////////////
// CBorddoc menu commands

void CBorddoc::SetGridCoords(int nXIndex1, int nYIndex1, LONG nNewXValue1, LONG nNewYValue1, int nXIndex2, int nYIndex2, LONG nNewXValue2, LONG nNewYValue2)
{
	// Create and initialize a command, then add it to the list
	CCmdSetGridCoords* pCommand = new CCmdSetGridCoords(this, m_pCurrBEObj);
	pCommand->SetGridCoords(nXIndex1, nYIndex1, nNewXValue1, nNewYValue1, nXIndex2, nYIndex2, nNewXValue2, nNewYValue2);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
}
		
void CBorddoc::PasteGraphic(COleDataObject* pDataObject)
{
	// Create and initialize a command, then add it to the list
	CCmdPasteGraphic* pCommand = new CCmdPasteGraphic(this, m_pCurrBEObj);
	pCommand->InitPasteGraphic(pDataObject);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
}

void CBorddoc::FlipRegion(enum FlipTypesE flip)
{
	// Create and initialize a command, then add it to the list
	CCmdFlipRegion* pCommand = new CCmdFlipRegion(this, m_pCurrBEObj);
	pCommand->SetType(flip);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}

}

void CBorddoc::OnFileSaveAs() 
{
	// Notify client to save the current object
	m_pCallbackInterface->Notify(CBECallbackInterface::BESaveBorder, m_pCurrBEObj->GetDatabase(), NULL);
}

void CBorddoc::OnRevert() 
{
	// Put up warning, then revert
	CString szRevert;
	szRevert.LoadString(IDS_REVERT);
	if (AfxMessageBox(szRevert, MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
	{
		m_pCurrBEObj->Revert();
		m_CommandList.ClearAll();
		UpdateAllViews(NULL, CBordview::Revert | CBordview::Redraw);

		// Notify the client of a change
		m_pCallbackInterface->Notify(CBECallbackInterface::BENotifyBorderChange, m_pCurrBEObj->GetDatabase(), (void*)m_pCurrBEObj->GetChangedRect());
	}
}

void CBorddoc::OnUpdateRevert(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsModified());	
	
}

void CBorddoc::OnEditUndo() 
{
	// Clear change
	m_pCurrBEObj->ClearChangedRect();
	
	// Undo the last action
	m_CommandList.Undo();

	// Notify the client of a change
	m_pCallbackInterface->Notify(CBECallbackInterface::BENotifyBorderChange, m_pCurrBEObj->GetDatabase(), (void*)m_pCurrBEObj->GetChangedRect());
}

void CBorddoc::OnEditRedo() 
{
	// Clear change
	m_pCurrBEObj->ClearChangedRect();
	
	// Redo the last action
	m_CommandList.Redo();
	
	// Notify the client of a change
	m_pCallbackInterface->Notify(CBECallbackInterface::BENotifyBorderChange, m_pCurrBEObj->GetDatabase(), (void*)m_pCurrBEObj->GetChangedRect());
}

void CBorddoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_CommandList.CanUndo());
}

void CBorddoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_CommandList.CanRedo());	
}

void CBorddoc::OnZoomIn() 
{
	if (m_pCurrBEObj != NULL)
	{
		// Create and initialize a command, then add it to the list
		CCmdZoom* pCommand = new CCmdZoom(this, m_pCurrBEObj);
		pCommand->SetZoom(CBEObjectD::ZoomIn);

		if (!AddCommand(pCommand))
		{
			delete pCommand;
		}
	}
		
}

void CBorddoc::OnZoomOut() 
{
	if (m_pCurrBEObj != NULL)
	{
		// Create and initialize a command, then add it to the list
		CCmdZoom* pCommand = new CCmdZoom(this, m_pCurrBEObj);
		pCommand->SetZoom(CBEObjectD::ZoomOut);

		if (!AddCommand(pCommand))
		{
			delete pCommand;
		}
	}
	
}

void CBorddoc::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && !m_pCurrBEObj->AtZoomLimit(CBEObjectD::ZoomIn));
}

void CBorddoc::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && !m_pCurrBEObj->AtZoomLimit(CBEObjectD::ZoomOut));	
}

void CBorddoc::OnRotate90() 
{
	// Create and initialize a command, then add it to the list
	CCmdRotateRegion* pCommand = new CCmdRotateRegion(this, m_pCurrBEObj);
	pCommand->SetMembers();

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	
}

void CBorddoc::OnUpdateTransforms(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->CanDoTransforms());
	
}

void CBorddoc::OnFlipHorizontal() 
{
	FlipRegion(H_FLIP);
	
}

void CBorddoc::OnFlipVertical() 
{
	FlipRegion(V_FLIP);
	
}

void CBorddoc::OnSettings() 
{
	// Bring up the dialog
	CSettingsDlg SettingsDlg(m_pCurrBEObj);

	// Wait for completion
	if (SettingsDlg.DoModal() == IDOK)
	{
		// Create and initialize a command, then add it to the list
		CCmdChangeSettings* pCommand = new CCmdChangeSettings(this, m_pCurrBEObj);
		pCommand->ChangeSettings(SettingsDlg);

		if (!AddCommand(pCommand))
		{
			delete pCommand;
		}		
	}
}

void CBorddoc::OnUpdateSettings(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->AreRegionsSelected(EDGE_R));
	
}

void CBorddoc::OnChooseBgColor() 
{
#if 1
	COLORREF colBGColor;
	BOOL bUseBGColor = m_pCurrBEObj->GetBGColor(colBGColor);
	BackgroundColorWindow.SetColor(bUseBGColor ? colBGColor : TRANSPARENT_COLOR);
   OnModifyPalette(ID_CHOOSE_BG_COLOR, ID_BACKGROUND_COLOR_PALETTE, BackgroundColorWindow);
#else
	// Bring up the dialog
	CBGColorDlg BGColorDlg(m_pCurrBEObj, m_pCallbackInterface);

	// Wait for completion
	if (BGColorDlg.DoModal() == IDOK)
	{
		// Create and initialize a command, then add it to the list
		CCmdChangeBGColor* pCommand = new CCmdChangeBGColor(this, m_pCurrBEObj);
		pCommand->ChangeBGColor(BGColorDlg);

		if (!AddCommand(pCommand))
		{
			delete pCommand;
		}		
	}
#endif
}

void CBorddoc::OnUpdateChooseBgColor(CCmdUI* pCmdUI) 
{
	if (m_pCurrBEObj == NULL)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(BackgroundColorWindow.GetSafeHwnd() != NULL);
	}
}

void CBorddoc::OnBackgroundColorPalette()
{
   COLORREF Color = TRANSPARENT_COLOR;
   if (!BackgroundColorWindow.IsNone())
   {
      Color = BackgroundColorWindow.GetColor();
	}

	// Create and initialize a command, then add it to the list
	CCmdChangeBGColor* pCommand = new CCmdChangeBGColor(this, m_pCurrBEObj);
	pCommand->ChangeBGColor(Color != TRANSPARENT_COLOR, Color);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}		
}

void CBorddoc::OnChooseGraphic() 
{
	ChooseGraphicWArtGallery();
}

void CBorddoc::ChooseGraphicWArtGallery()
{
	// Bring up dialog to choose the graphic, and transfer it
	DWORD dwID = m_pCurrBEObj->ChooseGraphic(theBorderApp.m_pMainWnd->m_hWnd);
	
	if (dwID)
	{		
		// Create a command to assign the graphic
		ChooseGraphic(dwID);
	}
}

void CBorddoc::ChooseGraphic(DWORD dwID)
{
	// Create and initialize a command, then add it to the list
	CCmdChooseGraphic* pCommand = new CCmdChooseGraphic(this, m_pCurrBEObj);
	pCommand->ChooseGraphic(dwID);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}

	// Remove the extra reference now
	m_pCurrBEObj->RemoveRef(dwID);
}	

void CBorddoc::OnUpdateChooseGraphic(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->Selected());	
}

void CBorddoc::OnSelectAll() 
{
	m_pCurrBEObj->SelectAllRegions(m_bSelectAll);
	UpdateAllViews(NULL, CBordview::SelectionsOnly | CBordview::Redraw);	
}

void CBorddoc::OnSelectCorners() 
{
	m_pCurrBEObj->SelectCornerRegions(m_bSelectCorners);
	UpdateAllViews(NULL, CBordview::SelectionsOnly | CBordview::Redraw);
}

void CBorddoc::OnSelectEdges() 
{
	m_pCurrBEObj->SelectEdgeRegions(m_bSelectEdges);
	UpdateAllViews(NULL, CBordview::SelectionsOnly | CBordview::Redraw);
	
}

void CBorddoc::OnSelectHEdges() 
{
	m_pCurrBEObj->SelectEdgeRegions(m_bSelectHEdges, H_FLIP);
	UpdateAllViews(NULL, CBordview::SelectionsOnly | CBordview::Redraw);
	
}

void CBorddoc::OnSelectVEdges() 
{
	m_pCurrBEObj->SelectEdgeRegions(m_bSelectVEdges, V_FLIP);
	UpdateAllViews(NULL, CBordview::SelectionsOnly | CBordview::Redraw);
	
}

void CBorddoc::OnUpdateSelectAll(CCmdUI* pCmdUI) 
{
	m_bSelectAll = OnUpdateSelectHelper(pCmdUI, ALL_R, IDS_SELECT_ALL, IDS_DESELECT_ALL);
	
}

void CBorddoc::OnUpdateSelectCorners(CCmdUI* pCmdUI) 
{
	m_bSelectCorners = OnUpdateSelectHelper(pCmdUI, CORNERS_R, IDS_SELECT_CORNERS, IDS_DESELECT_CORNERS);
	
}

void CBorddoc::OnUpdateSelectEdges(CCmdUI* pCmdUI) 
{
	m_bSelectEdges = OnUpdateSelectHelper(pCmdUI, EDGES_R, IDS_SELECT_EDGES, IDS_DESELECT_EDGES);
	
}

void CBorddoc::OnUpdateSelectHEdges(CCmdUI* pCmdUI) 
{
	m_bSelectHEdges = OnUpdateSelectHelper(pCmdUI, EDGES_H_R, 0, 0);
	
}

void CBorddoc::OnUpdateSelectVEdges(CCmdUI* pCmdUI) 
{
	m_bSelectVEdges = OnUpdateSelectHelper(pCmdUI, EDGES_V_R, 0, 0);
	
}

BOOL CBorddoc::OnUpdateSelectHelper(CCmdUI* pCmdUI, enum RegionGroupsE group, UINT nOnID, UINT nOffID)
{
	BOOL bTurnOn = !(m_pCurrBEObj != NULL && m_pCurrBEObj->AreRegionsSelected(group));

	if (pCmdUI->m_pMenu != NULL)
	{
		CString szSelectAll;
		szSelectAll.LoadString((bTurnOn) ? nOnID : nOffID);
		pCmdUI->SetText(szSelectAll);
	}
	else
	{
		pCmdUI->SetCheck(!bTurnOn);
	}

	return (bTurnOn);
}

void CBorddoc::OnEditClear() 
{
	// Create and initialize a command, then add it to the list
	CCmdChooseGraphic* pCommand = new CCmdChooseGraphic(this, m_pCurrBEObj);
	pCommand->ChooseGraphic(0);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	
}

void CBorddoc::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->CanDoTransforms());	
	
}

void CBorddoc::OnCopyAcross() 
{
	// Create and initialize a command, then add it to the list
	CCmdCopyOpposite* pCommand = new CCmdCopyOpposite(this, m_pCurrBEObj);
	pCommand->CopyOpposite();

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	
}

void CBorddoc::OnUpdateCopyAcross(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->EdgeCopyAllowed(HV_FLIP));	
	
}

void CBorddoc::OnCopyEdges() 
{
	// Create and initialize a command, then add it to the list
	CCmdCopyEdges* pCommand = new CCmdCopyEdges(this, m_pCurrBEObj);
	pCommand->CopyEdges();

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	
}

void CBorddoc::OnUpdateCopyEdges(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->EdgeCopyAllowed());	
	
}

void CBorddoc::OnCopyCorners() 
{
	// Create and initialize a command, then add it to the list
	CCmdCopyCorners* pCommand = new CCmdCopyCorners(this, m_pCurrBEObj);
	pCommand->CopyCorners();

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	
}

void CBorddoc::OnUpdateCopyCorners(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->CornerCopyAllowed());	
	
}


void CBorddoc::OnEditCopy() 
{
	// Create and initialize a command, then add it to the list
	CCmdCopyGraphic* pCommand = new CCmdCopyGraphic(this, m_pCurrBEObj);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	
}

void CBorddoc::OnUpdateEditCutCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pCurrBEObj != NULL && m_pCurrBEObj->CanCutCopy());	
	
}

void CBorddoc::OnEditCut() 
{
	// Create and initialize a command, then add it to the list
	CCmdCutGraphic* pCommand = new CCmdCutGraphic(this, m_pCurrBEObj);
	pCommand->CutGraphic();

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	
}

void CBorddoc::OnEditPaste() 
{
	COleDataObject Data;

	Data.AttachClipboard();
	CCmdPasteGraphic* pCommand = new CCmdPasteGraphic(this, m_pCurrBEObj);
	pCommand->InitPasteGraphic(&Data);

	if (!AddCommand(pCommand))
	{
		delete pCommand;
	}
	Data.Detach();	
}



void CBorddoc::OnModifyPalette(UINT uID, UINT uPaletteID, CPopupPalette& Window)
{
   if (Window.GetSafeHwnd() == NULL)
   {
		CWnd* pParent = AfxGetMainWnd();

      // Create the window.
      Window.SetOwner(pParent);
      Window.Create(pParent, uPaletteID);

		// Set the palette.

		Window.SetPalette(((CBorderApp*)AfxGetApp())->GetPalette());

      // Get the window rect so we can know how big this guy is.
      CRect crWindow;
      Window.GetWindowRect(crWindow);

      int nWidth = crWindow.Width();
      int nHeight = crWindow.Height();

      // Determine the position for the window.

      CPoint p;
      CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CToolBar& ToolBar = pMainFrame->ToolBar();
		if (ToolBar.IsWindowVisible())
		{
         int nIndex = ToolBar.CommandToIndex(uID);
         ASSERT(nIndex != -1);
         if (nIndex != -1)
         {
            CRect cr;
            ToolBar.GetItemRect(nIndex, cr);
            ToolBar.ClientToScreen(cr);
            pMainFrame->ScreenToClient(cr);
            p.x = (cr.left + cr.right - nWidth)/2;
            p.y = cr.bottom;
         }
      }
      else
      {
         CRect cr;
         pMainFrame->GetClientRect(cr);
         p.x = (cr.left + cr.right - nWidth)/2;
         p.y = (cr.top + cr.bottom - nHeight)/2;
      }

      pMainFrame->ClientToScreen(&p);

      // Make sure palette window is on the screen.
      CRect crScreen;
#ifdef WIN32
      if (SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)crScreen, FALSE) == 0)
      {
         crScreen.SetRectEmpty();
      }
#else
      crScreen.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#endif

      if (!crScreen.IsRectEmpty())
      {
         if (p.x+nWidth > crScreen.right) 
         {
            p.x = crScreen.right-nWidth;
         }
         if (p.y+nHeight > crScreen.bottom) 
         {
            p.y = crScreen.bottom-nHeight;
         }
         if (p.x < crScreen.left)
         {
            p.x = crScreen.left;
         }
         if (p.y < crScreen.top)
         {
            p.y = crScreen.top;
         }
      }

      // Position the window.
      Window.SetWindowPos(NULL, p.x, p.y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

      // Show the window.
      Window.ShowWindow(SW_SHOW);
   }
   else
   {
      // Destroy the window.
      Window.DestroyWindow();
   }
}
