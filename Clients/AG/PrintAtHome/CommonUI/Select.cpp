#include "stdafx.h"
#include "Ctp.h"
#include "Select.h"

static int TIMER_BlinkCaret = 1;
#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)
//////////////////////////////////////////////////////////////////////
CSelect::CSelect(CDocWindow* pDocWnd)
{
	m_pDocWnd = pDocWnd;
	m_uMode = NO_MODE;
	m_uModeSuspended = NO_MODE;
	m_pSelSym = NULL;
	SetRect(&m_LastCheckPointCropRect, 0, 0, 0, 0);
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

	if (pSym->IsImage() && ((CAGSymImage*)pSym)->IsLocked() && !SpecialsEnabled())
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
			(!SymIsImage() && !SymIsAddAPhoto() && !SymIsLine() && (uSymMode != EDIT_MODE) ? TR_ROTATE : 0);
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
void CSelect::StartFreeRotate()
{
	if (SymIsText())
	{
		if (InTrackingMode())
			StopTracking(true/*bSuccess*/);

		if (GetTextSym()->InEditMode())
		{
            SetTextEditMode(false);
			StartTracking(TRANS_MODE);
		}
    }
	
    return;
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
				m_Track.Delta(0, (SHIFT ? -5 : -1));
				return true;
			case VK_DOWN:
				m_Track.Delta(0, (SHIFT ?  5 :  1));
				return true;
			case VK_LEFT:
				m_Track.Delta((SHIFT ? -5 : -1), 0);
				return true;
			case VK_RIGHT:
				m_Track.Delta((SHIFT ?  5 :  1), 0);
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
            {
                SetTextEditMode(false);
			    StartTracking(TRANS_MODE);
            }
            else
            {
               ::PostMessage(m_pDocWnd->m_hWnd, WM_COMMAND, IDC_DOC_SELECTALL, 0);
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

	return pImgSym->GetNativeType();
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
