#include "stdafx.h"
#include "Select.h"
#include "Utility.h"
#include "DocWindow.h"
#include "ImageControlConstants.h"

#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)

//////////////////////////////////////////////////////////////////////
CSelect::CSelect(CDocWindow* pDocWnd)
{
	m_pDocWnd = pDocWnd;
	m_uMode = NO_MODE;
	m_uModeSuspended = NO_MODE;
	m_pSelObject = NULL;
	m_StartingCropRect.SetRectEmpty();
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

	bool bOverrideHidden = m_pDocWnd->GetOverrideHidden();
	if (pObject->IsHidden(bOverrideHidden))
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
		StopTracking(false/*bSuccess*/);

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

	return false;
}

////////////////////////////////////////////////////////////////////////
bool CSelect::RightClick(int x, int y)
{
	if (!GetSelected())
		return false;

	return false;
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
void CSelect::StartTracking(SEL_MODE uTrackingMode)
{
	if (!GetSelected())
		return;

	if (InTrackingMode())
		return;

	// See if the mode is valid
	if (uTrackingMode <= 0)
		return;

	CMatrix Matrix = m_pSelObject->GetMatrix();
	CRect DestRect = m_pSelObject->GetDestRect();
	CRect CropRect = m_pSelObject->GetCropRect();

	bool bResume = (uTrackingMode == RESUME_MODE);
	m_uMode = (bResume ? m_uModeSuspended : uTrackingMode);
	if (!bResume)
	{
		m_StartingCropRect = CropRect;
		m_StartingMatrix = Matrix;
	}

	int iWhatCanDo = 0;
	DRAWPROC pDrawProc = NULL;

	if (m_uMode == NORMAL_MODE)
	{
		iWhatCanDo = 0;
		pDrawProc = CSelect::NormalDrawProcEx;
	}
	else
	if (m_uMode == TRANS_MODE)
	{
		iWhatCanDo = TR_EXCLUSIVE | TR_SIZE | TR_CONSTRAINASPECT;
		pDrawProc = CSelect::TransformDrawProcEx;
	}
	else
	if (m_uMode == CROP_MODE)
	{
		iWhatCanDo = TR_EXCLUSIVE | TR_SIZE | TR_BOUNDTOSYMBOL | TR_DRAWBARS;
		pDrawProc = CSelect::CropDrawProcEx;
	}

	CMatrix ViewToDeviceMatrix = m_pDocWnd->GetClientDC()->GetViewToDeviceMatrix();
	m_Track.Init(m_pDocWnd->GetClientDC(), iWhatCanDo, pDrawProc, this, DestRect, CropRect,
		 m_pDocWnd->GetPageViewRect(), ViewToDeviceMatrix, Matrix, m_pDocWnd->GetGrid());

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

	StartTracking(RESUME_MODE);
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartNormal()
{
	if (InNormalMode())
		return;

	if (InTrackingMode())
		StopTracking(false/*bSuccess*/);

	StartTracking(NORMAL_MODE);
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartTransform()
{
	if (InTransformMode())
		return;

	if (InTrackingMode())
		StopTracking(false/*bSuccess*/);

	StartTracking(TRANS_MODE);
}

//////////////////////////////////////////////////////////////////////
void CSelect::StartCrop()
{
	if (InCropMode())
		return;

	if (InTrackingMode())
		StopTracking(false/*bSuccess*/);

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
	if (!m_pSelObject)
		return NULL;

	bool bOverrideHidden = m_pDocWnd->GetOverrideHidden();
	if (m_pSelObject->IsHidden(bOverrideHidden))
		return NULL;

	return m_pSelObject;
}

//////////////////////////////////////////////////////////////////////
const CMatrix& CSelect::GetMatrix()
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
	CSelect* pSelect = (CSelect*)pData;
	if (pSelect)
		pSelect->NormalDrawProc(hDC, bOn, iTrackingCode);
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::NormalDrawProc(HDC hDC, bool bOn, int iTrackingCode)
{
	if (!m_pDocWnd)
		return;

	if (!InNormalMode())
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
	CSelect* pSelect = (CSelect*)pData;
	if (pSelect)
		pSelect->TransformDrawProc(hDC, bOn, iTrackingCode);
}

/////////////////////////////////////////////////////////////////////////////
void CSelect::TransformDrawProc(HDC hDC, bool bOn, int iTrackingCode)
{
	if (!m_pDocWnd)
		return;

	if (!InTransformMode())
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
		pObject->SetMatrix(m_StartingMatrix);
	}
	else
	{
		CMatrix Matrix = m_Track.GetMatrix();
		bool bModified = !!(Matrix != pObject->GetMatrix());
		if (bModified)
		{
			pObject->SetMatrix(Matrix);
//j			m_pDocWnd->UpdateInfo();
		}

		if (iTrackingCode == TOOLCODE_DONE)
		{
			bool bSetCheckPoint = !!(Matrix != m_StartingMatrix);
			if (bSetCheckPoint)
			{
//j				m_pDocWnd->GetUndo().CheckPoint(IDC_DOC_TRANSFORM, pObject, m_StartingMatrix);
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
	CSelect* pSelect = (CSelect*)pData;
	if (pSelect)
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
		pObject->SetCropRect(m_StartingCropRect);
	}
	else
	{
		// Change the Object by changing the crop rectangle
		CMatrix ChangeMatrix = m_Track.GetChangeMatrix();
		CRect CropRect = m_StartingCropRect;
		ChangeMatrix.Transform(CropRect);

		bool bModified = !::EqualRect(&CropRect, &pObject->GetCropRect());
		if (bModified)
		{
			// Don't let the rect be empty in either dimension
			CropRect.InflateRect(!CropRect.Width(), !CropRect.Height());
			pObject->SetCropRect(CropRect);
//j			m_pDocWnd->UpdateInfo();
		}

		if (iTrackingCode == TOOLCODE_DONE)
		{
			bool bSetCheckPoint = !::EqualRect(&CropRect, &m_StartingCropRect);
			if (bSetCheckPoint)
			{
//j				m_pDocWnd->GetUndo().CheckPoint(IDC_DOC_CROP, pObject, m_StartingCropRect);
			}
		}
	}

	// Invalidate the newly modified Object
	m_pDocWnd->InvalidateImage(pObject);
}
