#include "stdafx.h"
#include "Ctp.h"
#include "Select.h"
#include "..\\ImageEdit\\MainDlg.h"

static int TIMER_BlinkCaret = 1;
#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)
//////////////////////////////////////////////////////////////////////
CSelect::CSelect(CDocWindow* pDocWnd)
{
	m_pDocWnd = pDocWnd;
	m_uMode = NO_MODE;
	m_uModeSuspended = NO_MODE;
	m_pSelSym = NULL;
	::SetRectEmpty(&m_LastCheckPointCropRect);
}

//////////////////////////////////////////////////////////////////////
CSelect::~CSelect()
{
	SymbolUnselect(true/*bClearPointer*/, false/*SetPanel*/);
}

//////////////////////////////////////////////////////////////////////
void CSelect::SymbolSelect(CAGSym* pSym, POINT* pClickPoint)
{
	if (!pSym)
		return;

	if (pSym->IsHidden(m_pDocWnd->GetHiddenFlag()))
		return;

	if (pSym->IsLocked() && !SpecialsEnabled())
		return;

	if (m_pSelSym == pSym)
	{
		m_pDocWnd->UpdateInfo();
		m_pDocWnd->m_pClientDC->SetSymbolMatrix(m_pSelSym->GetMatrix());

		if (!pClickPoint)
			m_pDocWnd->SymbolInvalidate(pSym);
		return;
	}

	SymbolUnselect(true/*bClearPointer*/, false/*SetPanel*/);

	m_pSelSym = pSym;
	m_pDocWnd->UpdateInfo();

	if (NoSymSelected())
		return;

	m_pDocWnd->m_pClientDC->SetSymbolMatrix(m_pSelSym->GetMatrix());

	if (SymIsText() || (SymIsAddAPhoto() && SpecialsEnabled()))
	{
		SetTextEditMode(true, pClickPoint);
		m_pDocWnd->m_pCtp->SelectPanel(TEXT_PANEL);
		StartTracking(EDIT_MODE);
	}
	else
	if (SymIsAddAPhoto() || SymIsImage())
	{
		m_pDocWnd->m_pCtp->SelectPanel(IMAGE_PANEL);
		StartTracking(TRANS_MODE);
	}
	else
	if (SymIsGraphic())
	{
		m_pDocWnd->m_pCtp->SelectPanel(GRAPHICS_PANEL);
		StartTracking(TRANS_MODE);
	}
	else
	if (SymIsCalendar())
	{
		((CAGSymCalendar*)m_pSelSym)->SetMode(-1);
		m_pDocWnd->m_pCtp->SelectPanel(CALENDAR_PANEL);
		StartTracking(TRANS_MODE);
	}

	// Invalidate the symbol's current location
	m_pDocWnd->SymbolInvalidate(m_pSelSym);
}

//////////////////////////////////////////////////////////////////////
void CSelect::SymbolUnselect(bool bClearPointer, bool fSetPanel)
{
	if (!m_pDocWnd->m_pAGDoc)
		return;

	if (!m_pSelSym)
		return;

	if (SymIsText() || (SymIsAddAPhoto() && SpecialsEnabled()))
		SetTextEditMode(false);

	// Invalidate the symbol's current location
	m_pDocWnd->SymbolInvalidate(m_pSelSym);

	m_pDocWnd->m_pClientDC->SetSymbolMatrix();

	if (InTrackingMode())
		StopTracking(true/*bSuccess*/);

	if (bClearPointer)
	{
		m_pSelSym = NULL;
		m_pDocWnd->UpdateInfo();
	}

	if (fSetPanel)
		m_pDocWnd->m_pCtp->SelectPanel(DEFAULT_PANEL);
}

//////////////////////////////////////////////////////////////////////
void CSelect::SetTextEditMode(bool bOn, POINT* pClickPoint)
{
	CAGSymText* pText = GetTextSym();
	if (!pText)
		return;

	if (bOn)
	{
		m_pDocWnd->GetUndo().CheckPointText(IDC_DOC_TEXT_EDIT_START, pText);
		m_pDocWnd->SetTimer(TIMER_BlinkCaret, 500);
		pText->StartEdit(m_pDocWnd->m_pClientDC, m_pSelSym);
		if (pClickPoint)
		{
			POINT ptm = *pClickPoint;
			m_pDocWnd->m_pClientDC->GetSymbolToDeviceMatrix().Inverse().Transform(ptm);
			pText->OnLButtonDown(ptm);
		}
		else
		{
			RECT DestRect = m_pSelSym->GetDestRect();
			POINT ClickPoint = {DestRect.right, DestRect.bottom};
			pText->OnLButtonDown(ClickPoint);
			pText->OnLButtonUp(ClickPoint);
		}
	}
	else
	{
		m_pDocWnd->KillTimer(TIMER_BlinkCaret);
		pText->EndEdit();
		if (pText->IsDirty())
			m_pDocWnd->m_pAGDoc->SetModified(true);
		m_pDocWnd->GetUndo().CheckPointText(IDC_DOC_TEXT_EDIT_END, pText);
	}

	// Invalidate the symbol's current location
	m_pDocWnd->SymbolInvalidate(m_pSelSym);
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartTracking(SEL_MODE uSymMode)
{
	if (NoSymSelected())
		return;

	if (InTrackingMode())
		return;

	if (uSymMode != TRANS_MODE && uSymMode != EDIT_MODE && uSymMode != CROP_MODE)
		return;

	int iWhatCanDo = TR_EXCLUSIVE | TR_SIZE | (SymIsLine() ? TR_DRAWASLINE : 0);
	DRAWPROC pDrawProc = NULL;

	CAGMatrix SymbolMatrix = m_pSelSym->GetMatrix();
	RECT SymbolRect = m_pSelSym->GetDestRect();

	RECT BoundRect = {0,0,0,0};
	if (uSymMode == TRANS_MODE || uSymMode == EDIT_MODE)
	{
		BoundRect = SymbolRect;
		iWhatCanDo |= (SymIsImage() ? TR_CONSTRAINASPECT : 0) |
			(!SymIsAddAPhoto() && !SymIsLine() && (uSymMode != EDIT_MODE) ? TR_ROTATE : 0);
		pDrawProc = CSelect::TransformDrawProcEx;
	}
	else
	if (uSymMode == CROP_MODE)
	{
		CAGSymImage* pSymImage = (CAGSymImage*)m_pSelSym;
		if (::IsRectEmpty(&pSymImage->GetCropRect()))
			pSymImage->SetCropRect(SymbolRect);

		BoundRect = m_LastCheckPointCropRect = pSymImage->GetCropRect();
		iWhatCanDo |= TR_BOUNDTOSYMBOL;
		pDrawProc = CSelect::CropDrawProcEx;
	}

	m_LastCheckPointMatrix = SymbolMatrix;
	CAGMatrix ViewToDeviceMatrix = m_pDocWnd->m_pClientDC->GetViewToDeviceMatrix();
	m_Track.Init(m_pDocWnd->m_pClientDC, iWhatCanDo, pDrawProc, this, SymbolRect, BoundRect,
		 m_pDocWnd->m_PageRect, ViewToDeviceMatrix, SymbolMatrix, m_pDocWnd->m_Grid);

	m_uMode = uSymMode;
	m_Track.Draw(true/*bOn*/);
}

//////////////////////////////////////////////////////////////////////
void CSelect::StopTracking(bool bSuccess)
{
	if (InTrackingMode())
		m_Track.Done(bSuccess);

	m_uMode = NO_MODE;
}

//////////////////////////////////////////////////////////////////////
void CSelect::SuspendTracking()
{
	if (NoSymSelected())
		return;

	if (!InTrackingMode())
		return;

	m_uModeSuspended = m_uMode;
	StopTracking(true/*bSuccess*/);
}

//////////////////////////////////////////////////////////////////////
void CSelect::ResumeTracking()
{
	if (InTrackingMode())
		return;

	StartTracking(m_uModeSuspended);
	m_uModeSuspended = NO_MODE;
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartTextRotate()
{
	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (bTextSymbolInEditMode)
	{
		StopTracking(true/*bSuccess*/);
		StartTransform();
    }
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartTransform()
{
	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (bTextSymbolInEditMode)
	{
		SetTextEditMode(false);
		StartTracking(TRANS_MODE);
		return;
	}

	if (InCropMode())
	{
		StopTracking(true/*bSuccess*/);
		StartTracking(TRANS_MODE);
	}
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartCrop()
{
	if (!SymIsImage())
		return;

	if (InTransformMode())
	{
		StopTracking(true/*bSuccess*/);
		StartTracking(CROP_MODE);
	}
}

//////////////////////////////////////////////////////////////////////
bool CSelect::LaunchImageEditor(UINT iTabID)
{
	if (!m_pDocWnd->m_pCtp->IsAmericanGreetings())
		return false;

	CWaitCursor Wait;
	CLSID clsid2;
	HRESULT hr2 = CLSIDFromProgID(CComBSTR("ImageControl.ImageControl.1"), &clsid2);
	CLSID clsid;
	HRESULT hr = CLSIDFromString(CComBSTR("{266D1DBF-1107-4529-A48F-0FFBAE7351D1}"), &clsid);
	if (FAILED(hr))
	{
		CMessageBox::Message(String("Sorry.  The image control is not installed on this machine."));
		return false;
	}

	CAGPage* pPage = m_pDocWnd->m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return false;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return false;

	CAGSymImage* pSelSymImage = (CAGSymImage*)m_pSelSym;
	if (!pSelSymImage)
		return false;

	CAGMatrix AGMatrix = m_pSelSym->GetMatrix();
	CMatrix Matrix;
	Matrix.m_ax = AGMatrix.m_ax;
	Matrix.m_ay = AGMatrix.m_ay;
	Matrix.m_bx = AGMatrix.m_bx;
	Matrix.m_by = AGMatrix.m_by;
	Matrix.m_cx = AGMatrix.m_cx;
	Matrix.m_cy = AGMatrix.m_cy;
	BITMAPINFOHEADER* pDIB = pSelSymImage->GetDib();

	// Only supports 24 bit Dib's
	if (DibBitCount(pDIB) != 24)
	{
		CMessageBox::Message(String("The image editor can only operate on full color images."));
#ifndef _DEBUG
		return false;
#endif _DEBUG
	}
	
	CRect OldDibRect(0, 0, DibWidth(pDIB), DibHeight(pDIB));

	COLORREF TransparentColor = pSelSymImage->GetTransparentColor();
	if (TransparentColor != CLR_NONE)
		pDIB->biClrImportant  = 0xFF000000 | TransparentColor;

	// Bring up the image editor
	CImageEdit dlg(pDIB, Matrix, iTabID);
	dlg.RegisterArtSetupCallback(CCtp::MyArtSetupCallback, (LPARAM)m_pDocWnd->m_pCtp);
	bool bIsClipart = (m_pSelSym->GetID() == ID_CLIPART);
	if (bIsClipart)
		dlg.SetClipArtInfo(m_pSelSym->GetProductInfo(), true/*bInitialState*/);

	INT_PTR idReturn = dlg.DoModal();
	pDIB->biClrImportant = 0;
	if (idReturn != IDOK)
		return false;

	// Get the new DIB
	bool bOK = dlg.GetImage(pDIB, Matrix);
	if (!bOK || !pDIB)
		return false;

	// See if it has a transparent color
	bool bHasTransparentColor = ((pDIB->biClrImportant & 0xFF000000) == 0xFF000000);
	TransparentColor = (bHasTransparentColor ? pDIB->biClrImportant & 0x00FFFFFF : CLR_NONE);
	pDIB->biClrImportant = 0;

	// Create the new symbol
	CAGSym* pNewSym = pSelSymImage->Duplicate(false/*bCopyImageData*/);
	CAGSymImage* pNewSymImage = (CAGSymImage*)pNewSym;
	pNewSymImage->SetDib(pDIB);
	pNewSymImage->SetTransparentColor(TransparentColor);

	// If the image in the Image Editor has clip art or was originally clipart
	// then identify new image as clip art and save product info with 
	// new symbol - used for usage reporting.
	CString strClipArtInfo = dlg.GetClipArtInfo();
	if (!strClipArtInfo.IsEmpty())
	{
		pNewSymImage->SetID(ID_CLIPART);
		pNewSymImage->SetProductInfo(strClipArtInfo);
	}

	// Find the symbol center
	CRect DestRect = pNewSymImage->GetDestRectTransformed();
	double fcx = (DestRect.right + DestRect.left) / 2;
	double fcy = (DestRect.bottom + DestRect.top) / 2;

	// See if the image was rotated during editing
	double fOldAngle = AGMatrix.GetAngle();
	double fNewAngle = Matrix.GetAngle();
	if (fOldAngle != fNewAngle)
	{
		double fAngle = fNewAngle - fOldAngle;
		AGMatrix.Rotate(fAngle, fAngle, fcx, fcy);
	}

	// See if the image's aspect ratio changed during editing
	CRect NewDibRect(0, 0, DibWidth(pDIB), DibHeight(pDIB));
	if (OldDibRect.Width() != NewDibRect.Width() || OldDibRect.Height() != NewDibRect.Height())
	{
		double fxScale = (double)NewDibRect.Width() / OldDibRect.Width();
		double fyScale = (double)NewDibRect.Height() / OldDibRect.Height();
		
		// Anchor the larger scale factor at 1.0
		double fScale = max(fxScale, fyScale);
		fyScale /= fScale;
		fxScale /= fScale;
		
		AGMatrix.Scale(fxScale, fyScale, fcx, fcy);
	}

	// Apply any Matrix changes to the symbol
	pNewSymImage->SetMatrix(AGMatrix);

	// Change the symbol
	m_pDocWnd->GetUndo().CheckPoint(IDC_CONVERT_TO_JPG, m_pSelSym, (long)pNewSym);
	m_pDocWnd->m_pAGDoc->SetModified(true);
	int iOffset = pActiveLayer->FindSymbol(m_pSelSym);
	m_pSelSym->SetDeleted(true); // The actual delete happens when the undo list is cleared
	pActiveLayer->AddSymbol(pNewSym);
	pActiveLayer->OrderSymbol(pNewSym, iOffset);

	// Select the newly modified symbol
	SymbolSelect(pNewSym);
	m_pDocWnd->SetFocus();
	return true;
}

//////////////////////////////////////////////////////////////////////
void CSelect::Paint(HDC pDC)
{
	if (NoSymSelected())
		return;

	if (InTrackingMode())
		m_Track.Paint(pDC);
}

//////////////////////////////////////////////////////////////////////
bool CSelect::KeyDown(int nChar)
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (bTextSymbolInEditMode)
	{
		if (GetTextSym()->OnKeyDown(nChar))
		{
			CToolsPanelContainer* pCtlPanel = m_pDocWnd->m_pCtp->GetCtlPanel(SELECTED_PANEL);
			if (pCtlPanel)
				pCtlPanel->UpdateControls();
		}

		return true;
	}

	if (InTrackingMode())
	{
		#define SHIFT (GetAsyncKeyState(VK_SHIFT)<0)
		switch (nChar)
		{
			case VK_UP:
				m_Track.Delta(0, (SHIFT ? -5 : -100));
				return true;
			case VK_DOWN:
				m_Track.Delta(0, (SHIFT ?  5 :  100));
				return true;
			case VK_LEFT:
				m_Track.Delta((SHIFT ? -5 : -100), 0);
				return true;
			case VK_RIGHT:
				m_Track.Delta((SHIFT ?  5 :  100), 0);
				return true;
			default:
				break;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::KeyUp(int nChar)
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (bTextSymbolInEditMode)
	{
		GetTextSym()->OnKeyUp(nChar);
		return true;
	}

	if (InTrackingMode())
	{
		switch (nChar)
		{
			case VK_RETURN:
				StopTracking(true/*bSuccess*/);
				SymbolUnselect(true/*bClearPointer*/, true/*SetPanel*/);
				return true;
			case VK_ESCAPE:
				StopTracking(false/*bSuccess*/);
				SymbolUnselect(true/*bClearPointer*/, true/*SetPanel*/);
				return true;
			case VK_DELETE:
				if (!CONTROL)
					::PostMessage(m_pDocWnd->m_hWnd, WM_COMMAND, IDC_DOC_DELETE, 0L);
				return true;
			default:
				break;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::KeyChar(int nChar)
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (bTextSymbolInEditMode)
	{
		CAGSymText* pText = GetTextSym();
		bool bSizeChange = pText->OnChar(nChar);
		if (bSizeChange)
		{
			CRect DestRect = pText->GetDestRect();
			SuspendTracking();
			pText->SetDestRect(DestRect);
			ResumeTracking();
		}

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::ButtonDown(int x, int y, CAGSym* pHitSym)
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	POINT pt = {x, y};
	m_pDocWnd->m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);
	bool bPtInSymbol = SymIsEqual(pHitSym) && m_pSelSym->HitTest(pt, m_pDocWnd->GetHiddenFlag());

	if (InTrackingMode())
	{
		bool bInsideMoveAllowed = !bTextSymbolInEditMode;
		if (m_Track.Down(x, y, bInsideMoveAllowed))
		{
			bool bPtInBorder = m_Track.PtInBorder();
			bool bPtInHandle = m_Track.PtInHandle();

			if (bPtInBorder || bPtInHandle || bPtInSymbol)
				return true;
		}
	}

	if (bTextSymbolInEditMode && bPtInSymbol)
	{
		POINT ptm = {x, y};
		m_pDocWnd->m_pClientDC->GetSymbolToDeviceMatrix().Inverse().Transform(ptm);
		GetTextSym()->OnLButtonDown(ptm);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::ButtonUp(int x, int y)
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();

	if (InTrackingMode())
	{
		if (m_Track.Up(x, y))
			return true;
	}

	if (bTextSymbolInEditMode)
	{
		POINT ptm = {x, y};
		m_pDocWnd->m_pClientDC->GetSymbolToDeviceMatrix().Inverse().Transform(ptm);
		if (GetTextSym()->OnLButtonUp(ptm))
		{
			CToolsPanelContainer* pCtlPanel = m_pDocWnd->m_pCtp->GetCtlPanel(SELECTED_PANEL);
			if (pCtlPanel)
				pCtlPanel->UpdateControls();
		}

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////
bool CSelect::ButtonDblClick(int x, int y)
{
	if (NoSymSelected())
		return false;

	if (SymIsText())
	{
		if (InTrackingMode())
			StopTracking(true/*bSuccess*/);

		if (GetTextSym()->InEditMode())
		{
			if (CONTROL)
				::PostMessage(m_pDocWnd->m_hWnd, WM_COMMAND, IDC_DOC_SELECTALL, 0);
            else
            {
				SetTextEditMode(false);
			    StartTracking(TRANS_MODE);
            }
		}
		else
		{
			POINT ClickPoint = {x, y};
			SetTextEditMode(true, &ClickPoint);
			StartTracking(EDIT_MODE);
		}

		return true;
	}

	if (SymIsImage())
		return LaunchImageEditor();

	if (SymIsAddAPhoto())
	{
		if (!m_pDocWnd->m_pAGDoc)
			return false;

		CAGPage* pPage = m_pDocWnd->m_pAGDoc->GetCurrentPage();
		if (!pPage)
			return false;

		CAGLayer* pActiveLayer = pPage->GetActiveLayer();
		if (!pActiveLayer)
			return false;

		int iOffset = pActiveLayer->FindSymbol(m_pSelSym);
		return ((m_pDocWnd->AddImage(iOffset) == S_OK) ? true : false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SetCursor(int x, int y, CAGSym* pHitSym)
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	POINT pt = {x, y};
	m_pDocWnd->m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);
	bool bPtInSymbol = SymIsEqual(pHitSym) && m_pSelSym->HitTest(pt, m_pDocWnd->GetHiddenFlag());

	if (InTrackingMode())
	{
		bool bPtInHandle = false;
		bool bPtInBorder = false;
		bool bInsideMoveAllowed = !bTextSymbolInEditMode;
		if (m_Track.SetCursor(x, y, bInsideMoveAllowed, bPtInBorder, bPtInHandle))
		{
			if (bPtInBorder || bPtInHandle || bPtInSymbol)
				return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::MouseMove(int x, int y)
{
	if (NoSymSelected())
		return false;

	if (InTrackingMode())
	{
		if (m_Track.Move(x, y))
			return true;
	}

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (bTextSymbolInEditMode)
	{
		POINT ptm = {x, y};
		m_pDocWnd->m_pClientDC->GetSymbolToDeviceMatrix().Inverse().Transform(ptm);
		GetTextSym()->OnMouseMove(ptm);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::Timer(WPARAM wParam)
{
	if (wParam != TIMER_BlinkCaret)
		return false;

	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (!bTextSymbolInEditMode)
		return false;

	GetTextSym()->BlinkCaret();
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SetFocus()
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (!bTextSymbolInEditMode)
		return false;

	ShowTextSelection(NULL, true/*bCaretOnly*/, true);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::KillFocus()
{
	if (NoSymSelected())
		return false;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (!bTextSymbolInEditMode)
		return false;

	ShowTextSelection(NULL, true/*bCaretOnly*/, false);
	return true;
}

//////////////////////////////////////////////////////////////////////
void CSelect::ShowTextSelection(HDC hDC, bool bCaretOnly, bool bShow)
{
	if (NoSymSelected())
		return;

	bool bTextSymbolInEditMode = IsTextSymbolInEditMode();
	if (!bTextSymbolInEditMode)
		return;

	if (!bShow)
		m_pDocWnd->KillTimer(TIMER_BlinkCaret);

	GetTextSym()->ShowSelectionEx(hDC, bCaretOnly, bShow);

	if (bShow)
		m_pDocWnd->SetTimer(TIMER_BlinkCaret, 500);
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsEqual(CAGSym* pSym)
{
	if (!m_pSelSym || !pSym)
		return false;

	return (m_pSelSym == pSym);
}

//////////////////////////////////////////////////////////////////////
bool CSelect::NoSymSelected()
{
	return !SymSelected();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymSelected()
{
	return (m_pSelSym && !m_pSelSym->IsHidden(m_pDocWnd->GetHiddenFlag()));
}

//////////////////////////////////////////////////////////////////////
void CSelect::ResumeDrawing()
{
	if (m_pSelSym && !m_pSelSym->IsHidden(m_pDocWnd->GetHiddenFlag()))
		m_Track.ResumeDrawing();
}

//////////////////////////////////////////////////////////////////////
void CSelect::PauseDrawing()
{
	if (m_pSelSym && !m_pSelSym->IsHidden(m_pDocWnd->GetHiddenFlag()))
		m_Track.PauseDrawing();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsAddAPhoto()
{
	if (!m_pSelSym)
		return false;

	return m_pSelSym->IsAddAPhoto();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsCalendar()
{
	if (!m_pSelSym)
		return false;

	return m_pSelSym->IsCalendar();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsGraphic()
{
	if (!m_pSelSym)
		return false;

	return m_pSelSym->IsGraphic();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsShape()
{
	if (!m_pSelSym)
		return false;

	return m_pSelSym->IsShape();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsClipArt()
{
	if (!m_pSelSym)
		return false;

	return (m_pSelSym->GetID() == ID_CLIPART);
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsImage()
{
	if (!m_pSelSym)
		return false;

	return m_pSelSym->IsImage();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsText()
{
	if (!m_pSelSym)
		return false;

	return m_pSelSym->IsText();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsLine()
{
	if (!m_pSelSym)
		return false;

	return m_pSelSym->IsLine();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SymIsLocked()
{
	if (!m_pSelSym)
		return false;

	if (!m_pSelSym->IsImage())
		return false;

	return ((CAGSymImage*)m_pSelSym)->IsLocked();
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SpecialsEnabled()
{ 
	return m_pDocWnd->SpecialsEnabled(); 
}

//////////////////////////////////////////////////////////////////////
const CAGMatrix& CSelect::GetMatrix()
{
	ATLASSERT(m_pSelSym);

	return m_pSelSym->GetMatrix();
}

//////////////////////////////////////////////////////////////////////
const RECT& CSelect::GetDestRect()
{
	ATLASSERT(m_pSelSym);

	return m_pSelSym->GetDestRect();
}

//////////////////////////////////////////////////////////////////////
CString& CSelect::GetImgNativeType()
{
	ATLASSERT(m_pSelSym);

	CAGSymImage* pImgSym = dynamic_cast<CAGSymImage*>(m_pSelSym);
	ATLASSERT(pImgSym);

	return pImgSym->GetSourceType();
}

//////////////////////////////////////////////////////////////////////
CAGSymImage* CSelect::GetImageSym()
{
	ATLASSERT(m_pSelSym);
	if (!m_pSelSym)
		return NULL;

	return dynamic_cast<CAGSymImage*>(m_pSelSym);
}

//////////////////////////////////////////////////////////////////////
CAGSymGraphic* CSelect::GetGraphicSym()
{
	ATLASSERT(m_pSelSym);
	if (!m_pSelSym)
		return NULL;

	return dynamic_cast<CAGSymGraphic*>(m_pSelSym);
}

//////////////////////////////////////////////////////////////////////
CAGSymCalendar* CSelect::GetCalendarSym()
{
	ATLASSERT(m_pSelSym);
	if (!m_pSelSym)
		return NULL;

	return dynamic_cast<CAGSymCalendar*>(m_pSelSym);
}

//////////////////////////////////////////////////////////////////////
CAGSymText* CSelect::GetTextSym()
{
	ATLASSERT(m_pSelSym);
	if (!m_pSelSym)
		return NULL;

	return dynamic_cast<CAGSymText*>(m_pSelSym);
}

//////////////////////////////////////////////////////////////////////
CAGSymLine* CSelect::GetLineSym()
{
	ATLASSERT(m_pSelSym);
	if (!m_pSelSym)
		return NULL;

	return dynamic_cast<CAGSymLine*>(m_pSelSym);
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::TransformDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData)
{
	if (!pData)
		return;

	CSelect* pSelect = (CSelect*)pData;
	if (!pSelect)
		return;

	pSelect->TransformDrawProc(hDC, bOn, iTrackingCode);
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::TransformDrawProc(HDC hDC, bool bOn, int iTrackingCode)
{
	if (!m_pDocWnd)
		return;

	if (!InTrackingMode())
		return;

	CAGSym* pSym = m_pSelSym;
	if (!pSym)
		return;

	// iTrackingCode == TOOLCODE_ABORT when the user aborts the tool changes (i.e., presses Escape)
	// iTrackingCode == TOOLCODE_DONE when the user confirms the tool changes (i.e., presses Enter or DoubleClicks)
	// iTrackingCode == TOOLCODE_UPDATE when the tool changes (i.e., ButtonUp)

	// Invalidate the symbol's current location
	m_pDocWnd->SymbolInvalidate(pSym);

	CAGMatrix Matrix = m_Track.GetMatrix();
	bool bNonRotatedText = ((pSym->IsText() || pSym->IsAddAPhoto()) && !Matrix.IsRotated());
	bool bDoSetRect = (pSym->IsLine() || bNonRotatedText);
	if (iTrackingCode == TOOLCODE_ABORT)
	{
//j		pSym->SetMatrix(m_LastCheckPointMatrix);
	}
	else
	{
		// Change the symbol, either by applying the matrix, or by changing the rectangle
		if (bDoSetRect)
		{
			RECT DestRect = m_Track.GetRectOrig();
			Matrix.Transform(DestRect);

			bool bModified = !::EqualRect(&DestRect, &pSym->GetDestRectTransformed());
			if (bModified)
			{
				// Don't let the rect be empty in either dimension
				::InflateRect(&DestRect, !WIDTH(DestRect), !HEIGHT(DestRect));
				pSym->SetDestRect(DestRect);
				pSym->SetMatrix(CAGMatrix());
				m_pDocWnd->m_pAGDoc->SetModified(true);
				m_pDocWnd->UpdateInfo();
			}

			if (iTrackingCode == TOOLCODE_DONE)
			{
				bool bSetCheckPoint = !!(Matrix != m_LastCheckPointMatrix);
				if (bSetCheckPoint)
				{
					RECT CheckPointRect = m_Track.GetRectOrig();
					m_LastCheckPointMatrix.Transform(CheckPointRect);
					m_pDocWnd->GetUndo().CheckPoint(IDC_DOC_DESTRECTCHANGE, pSym, CheckPointRect);
					m_LastCheckPointMatrix = Matrix;
				}
			}
		}
		else
		{
			bool bModified = !!(Matrix != pSym->GetMatrix());
			if (bModified)
			{
				pSym->SetMatrix(Matrix);
				m_pDocWnd->m_pAGDoc->SetModified(true);
				m_pDocWnd->UpdateInfo();
			}

			if (iTrackingCode == TOOLCODE_DONE)
			{
				bool bSetCheckPoint = !!(Matrix != m_LastCheckPointMatrix);
				if (bSetCheckPoint)
				{
					m_pDocWnd->GetUndo().CheckPoint(IDC_DOC_TRANSFORM, pSym, m_LastCheckPointMatrix);
					m_LastCheckPointMatrix = Matrix;
				}
			}
		}
	}

	// Invalidate the rotate handle, in case it hangs outside the symbol
	RECT rect = {0,0,0,0};
	m_Track.GetExcessRect(&rect);
	m_pDocWnd->Repaint(rect);

	// Invalidate the newly modified symbol
	m_pDocWnd->SymbolInvalidate(pSym);
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::CropDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData)
{
	if (!pData)
		return;

	CSelect* pSelect = (CSelect*)pData;
	if (!pSelect)
		return;

	pSelect->CropDrawProc(hDC, bOn, iTrackingCode);
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::CropDrawProc(HDC hDC, bool bOn, int iTrackingCode)
{
	if (!m_pDocWnd)
		return;

	if (!InCropMode())
		return;

	CAGSym* pSym = m_pSelSym;
	if (!pSym)
		return;

	CAGSymImage* pSymImage = (CAGSymImage*)pSym;

	// iTrackingCode == TOOLCODE_ABORT when the user aborts the tool changes (i.e., presses Escape)
	// iTrackingCode == TOOLCODE_DONE when the user confirms the tool changes (i.e., presses Enter or DoubleClicks)
	// iTrackingCode == TOOLCODE_UPDATE when the tool changes (i.e., ButtonUp)

	// Invalidate the symbol's current location
	m_pDocWnd->SymbolInvalidate(pSym);

	if (iTrackingCode == TOOLCODE_ABORT)
	{
//j		pSymImage->SetCropRect(m_LastCheckPointCropRect);
	}
	else
	{
		// Change the symbol by changing the crop rectangle
		CAGMatrix ChangeMatrix = m_Track.GetChangeMatrix();
		RECT CropRect = m_LastCheckPointCropRect;
		ChangeMatrix.Transform(CropRect);

		bool bModified = !::EqualRect(&CropRect, &pSymImage->GetCropRect());
		if (bModified)
		{
			// Don't let the rect be empty in either dimension
			::InflateRect(&CropRect, !WIDTH(CropRect), !HEIGHT(CropRect));
			pSymImage->SetCropRect(CropRect);
			m_pDocWnd->m_pAGDoc->SetModified(true);
			m_pDocWnd->UpdateInfo();
		}

		if (iTrackingCode == TOOLCODE_DONE)
		{
			bool bSetCheckPoint = !::EqualRect(&CropRect, &m_LastCheckPointCropRect);
			if (bSetCheckPoint)
			{
				m_pDocWnd->GetUndo().CheckPoint(IDC_DOC_CROP, pSym, m_LastCheckPointCropRect);
				m_LastCheckPointCropRect = CropRect;
			}
		}
	}

	// Invalidate the newly modified symbol
	m_pDocWnd->SymbolInvalidate(pSym);
}
