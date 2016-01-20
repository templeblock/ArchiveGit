#include "stdafx.h"
#include "Select.h"
#include "Utility.h"
#include "DocWindow.h"

#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)

//////////////////////////////////////////////////////////////////////
CSelect::CSelect(CDocWindow* pDocWnd)
{
	m_pDocWnd = pDocWnd;
	m_uMode = NO_MODE;
	m_uModeSuspended = NO_MODE;
	m_pSelObject = NULL;
	SetRect(&m_LastCheckPointCropRect, 0, 0, 0, 0);
}

//////////////////////////////////////////////////////////////////////
CSelect::~CSelect()
{
	Unselect(false/*bUpdateUI*/);
}

//////////////////////////////////////////////////////////////////////
void CSelect::Select(CImageObject* pObject, CPoint* pClickPoint)
{
	if (!pObject)
		return;

	if (pObject->IsHidden())
		return;

	if (m_pSelObject == pObject)
	{
//j		m_pDocWnd->UpdateInfo();
		m_pDocWnd->GetClientDC()->SetSymbolMatrix(m_pSelObject->GetMatrix());

		if (!pClickPoint)
			m_pDocWnd->InvalidateImage(m_pSelObject);
		return;
	}

	Unselect(false/*bUpdateUI*/);

	m_pSelObject = pObject;
//j	m_pDocWnd->UpdateInfo();

	m_pDocWnd->GetClientDC()->SetSymbolMatrix(m_pSelObject->GetMatrix());

//j	m_pDocWnd->m_pCtp->SelectPanel(IMAGE_PANEL);
	StartTracking(NORMAL_MODE);

	// Invalidate the object's current location
	m_pDocWnd->InvalidateImage(m_pSelObject);
}

//////////////////////////////////////////////////////////////////////
void CSelect::Unselect(bool bUpdateUI)
{
	if (!m_pSelObject)
		return;

	// Invalidate the object's current location
	m_pDocWnd->InvalidateImage(m_pSelObject);

	m_pDocWnd->GetClientDC()->SetSymbolMatrix();

	if (InTrackingMode())
		StopTracking(true/*bSuccess*/);

	m_pSelObject = NULL;
//j	m_pDocWnd->UpdateInfo();

//j	if (bUpdateUI)
//j		m_pDocWnd->m_pCtp->SelectPanel(DEFAULT_PANEL);
}

//////////////////////////////////////////////////////////////////////
void CSelect::Paint(HDC pDC)
{
	if (!GetSelected())
		return;

	if (InTrackingMode())
		m_Track.Paint(pDC);
}

//////////////////////////////////////////////////////////////////////
bool CSelect::KeyDown(int nChar)
{
	if (!GetSelected())
		return false;

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
	if (!GetSelected())
		return false;

	if (InTrackingMode())
	{
		switch (nChar)
		{
			case VK_RETURN:
				StopTracking(true/*bSuccess*/);
				return true;
			case VK_ESCAPE:
				StopTracking(false/*bSuccess*/);
				return true;
			case VK_DELETE:
//j				::PostMessage(m_pDocWnd->m_hWnd, WM_COMMAND, IDC_DOC_DELETE, 0L);
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
	if (!GetSelected())
		return false;

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::ButtonDown(int x, int y, CImageObject* pHitObject)
{
	if (!GetSelected())
		return false;

	CPoint pt(x, y);
	m_pDocWnd->GetClientDC()->GetViewToDeviceMatrix().Inverse().Transform(pt);
	bool bPtInObject = IsSelected(pHitObject) && m_pSelObject->HitTest(pt, 0/*bHiddenflag*/);

	if (InTrackingMode())
	{
		if (m_Track.Down(x, y, true/*bInsideMoveAllowed*/))
		{
			bool bPtInBorder = m_Track.PtInBorder();
			bool bPtInHandle = m_Track.PtInHandle();
			if (bPtInBorder || bPtInHandle || bPtInObject)
				return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::ButtonUp(int x, int y)
{
	if (!GetSelected())
		return false;

	if (InTrackingMode())
	{
		if (m_Track.Up(x, y))
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////
bool CSelect::DoubleClick(int x, int y)
{
	if (!GetSelected())
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////
bool CSelect::RightClick(int x, int y)
{
	if (!GetSelected())
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SetCursor(int x, int y, CImageObject* pHitObject)
{
	if (!GetSelected())
		return false;

	CPoint pt(x, y);
	m_pDocWnd->GetClientDC()->GetViewToDeviceMatrix().Inverse().Transform(pt);
	bool bPtInObject = IsSelected(pHitObject) && m_pSelObject->HitTest(pt, 0/*bHiddenflag*/);

	if (InTrackingMode())
	{
		bool bPtInBorder = false;
		bool bPtInHandle = false;
		if (m_Track.SetCursor(x, y, true/*bInsideMoveAllowed*/, bPtInBorder, bPtInHandle))
		{
			if (bPtInBorder || bPtInHandle || bPtInObject)
				return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::MouseMove(int x, int y)
{
	if (!GetSelected())
		return false;

	if (InTrackingMode())
	{
		if (m_Track.Move(x, y))
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::SetFocus()
{
	if (!GetSelected())
		return false;

	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelect::KillFocus()
{
	if (!GetSelected())
		return false;

	return false;
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartTracking(SEL_MODE uObjectMode)
{
	if (!GetSelected())
		return;

	if (InTrackingMode())
		return;

	// See if the mode is valid
	if (uObjectMode <= 0)
		return;

	CAGMatrix Matrix = m_pSelObject->GetMatrix();
	CRect Rect = m_pSelObject->GetDestRect();

	CRect BoundRect;
	int iWhatCanDo = 0;
	DRAWPROC pDrawProc = NULL;

	if (uObjectMode == NORMAL_MODE)
	{
		BoundRect = Rect;
		iWhatCanDo = 0;
		pDrawProc = CSelect::NormalDrawProcEx;
	}
	else
	if (uObjectMode == TRANS_MODE)
	{
		BoundRect = Rect;
		iWhatCanDo = TR_EXCLUSIVE | TR_SIZE | TR_CONSTRAINASPECT;
		pDrawProc = CSelect::TransformDrawProcEx;
	}
	else
	if (uObjectMode == CROP_MODE)
	{
		if (m_pSelObject->GetCropRect().IsRectEmpty())
			m_pSelObject->SetCropRect(Rect);

		BoundRect = m_LastCheckPointCropRect = m_pSelObject->GetCropRect();
		iWhatCanDo = TR_EXCLUSIVE | TR_SIZE | TR_BOUNDTOSYMBOL| TR_DRAWBARS;
		pDrawProc = CSelect::CropDrawProcEx;
	}

	m_LastCheckPointMatrix = Matrix;
	CAGMatrix ViewToDeviceMatrix = m_pDocWnd->GetClientDC()->GetViewToDeviceMatrix();
	m_Track.Init(m_pDocWnd->GetClientDC(), iWhatCanDo, pDrawProc, this, Rect, BoundRect,
		 m_pDocWnd->GetPageViewRect(), ViewToDeviceMatrix, Matrix, m_pDocWnd->GetGrid());

	m_uMode = uObjectMode;
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
	if (!GetSelected())
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
void CSelect::StartNormal()
{
	if (InNormalMode())
		return;

	if (InTrackingMode())
		StopTracking(true/*bSuccess*/);

	StartTracking(NORMAL_MODE);
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartTransform()
{
	if (InTransformMode())
		return;

	if (InTrackingMode())
		StopTracking(true/*bSuccess*/);

	StartTracking(TRANS_MODE);
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartCrop()
{
	if (InCropMode())
		return;

	if (InTrackingMode())
		StopTracking(true/*bSuccess*/);

	StartTracking(CROP_MODE);
}

//////////////////////////////////////////////////////////////////////
bool CSelect::IsSelected(CImageObject* pObject)
{
	return pObject == GetSelected();
}

//////////////////////////////////////////////////////////////////////
CImageObject* CSelect::GetSelected()
{
	if (m_pSelObject && !m_pSelObject->IsHidden(0/*bHiddenflag*/))
		return m_pSelObject;

	return NULL;
}

//////////////////////////////////////////////////////////////////////
const CAGMatrix& CSelect::GetMatrix()
{
	ATLASSERT(m_pSelObject);

	return m_pSelObject->GetMatrix();
}

//////////////////////////////////////////////////////////////////////
const CRect& CSelect::GetDestRect()
{
	ATLASSERT(m_pSelObject);

	return m_pSelObject->GetDestRect();
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::NormalDrawProcEx(HDC hDC, bool bOn, int iTrackingCode, void* pData)
{
	if (!pData)
		return;

	CSelect* pSelect = (CSelect*)pData;
	if (!pSelect)
		return;

	pSelect->NormalDrawProc(hDC, bOn, iTrackingCode);
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::NormalDrawProc(HDC hDC, bool bOn, int iTrackingCode)
{
	if (!m_pDocWnd)
		return;

	if (!InTrackingMode())
		return;

	CImageObject* pObject = m_pSelObject;
	if (!pObject)
		return;

	// iTrackingCode == TOOLCODE_ABORT when the user aborts the tool changes (i.e., presses Escape)
	// iTrackingCode == TOOLCODE_DONE when the user confirms the tool changes (i.e., presses Enter or DoubleClicks)
	// iTrackingCode == TOOLCODE_UPDATE when the tool changes (i.e., ButtonUp)

	// Invalidate the Object's current location
	m_pDocWnd->InvalidateImage(pObject);
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

	CImageObject* pObject = m_pSelObject;
	if (!pObject)
		return;

	// iTrackingCode == TOOLCODE_ABORT when the user aborts the tool changes (i.e., presses Escape)
	// iTrackingCode == TOOLCODE_DONE when the user confirms the tool changes (i.e., presses Enter or DoubleClicks)
	// iTrackingCode == TOOLCODE_UPDATE when the tool changes (i.e., ButtonUp)

	// Invalidate the Object's current location
	m_pDocWnd->InvalidateImage(pObject);

	CAGMatrix Matrix = m_Track.GetMatrix();
	if (iTrackingCode == TOOLCODE_ABORT)
	{
		pObject->SetMatrix(m_LastCheckPointMatrix);
	}
	else
	{
		bool bModified = !!(Matrix != pObject->GetMatrix());
		if (bModified)
		{
			pObject->SetMatrix(Matrix);
			pObject->SetModified(true);
//j			m_pDocWnd->UpdateInfo();
		}

		if (iTrackingCode == TOOLCODE_DONE)
		{
			bool bSetCheckPoint = !!(Matrix != m_LastCheckPointMatrix);
			if (bSetCheckPoint)
			{
//j				m_pDocWnd->GetUndo().CheckPoint(IDC_DOC_TRANSFORM, pObject, m_LastCheckPointMatrix);
				m_LastCheckPointMatrix = Matrix;
			}
		}
	}

	// Invalidate the rotate handle, in case it hangs outside the Object
	CRect rect;
	m_Track.GetExcessRect(&rect);
	m_pDocWnd->Repaint(rect);

	// Invalidate the newly modified Object
	m_pDocWnd->InvalidateImage(pObject);
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

	CImageObject* pObject = m_pSelObject;
	if (!pObject)
		return;

	// iTrackingCode == TOOLCODE_ABORT when the user aborts the tool changes (i.e., presses Escape)
	// iTrackingCode == TOOLCODE_DONE when the user confirms the tool changes (i.e., presses Enter or DoubleClicks)
	// iTrackingCode == TOOLCODE_UPDATE when the tool changes (i.e., ButtonUp)

	// Invalidate the Object's current location
	m_pDocWnd->InvalidateImage(pObject);

	if (iTrackingCode == TOOLCODE_ABORT)
	{
		pObject->SetCropRect(m_LastCheckPointCropRect);
	}
	else
	{
		// Change the Object by changing the crop rectangle
		CAGMatrix ChangeMatrix = m_Track.GetChangeMatrix();
		CRect CropRect = m_LastCheckPointCropRect;
		ChangeMatrix.Transform(CropRect);

		bool bModified = !::EqualRect(&CropRect, &pObject->GetCropRect());
		if (bModified)
		{
			// Don't let the rect be empty in either dimension
			CropRect.InflateRect(!CropRect.Width(), !CropRect.Height());
			pObject->SetCropRect(CropRect);
			pObject->SetModified(true);
//j			m_pDocWnd->UpdateInfo();
		}

		if (iTrackingCode == TOOLCODE_DONE)
		{
			bool bSetCheckPoint = !::EqualRect(&CropRect, &m_LastCheckPointCropRect);
			if (bSetCheckPoint)
			{
//j				m_pDocWnd->GetUndo().CheckPoint(IDC_DOC_CROP, pObject, m_LastCheckPointCropRect);
				m_LastCheckPointCropRect = CropRect;
			}
		}
	}

	// Invalidate the newly modified Object
	m_pDocWnd->InvalidateImage(pObject);
}
