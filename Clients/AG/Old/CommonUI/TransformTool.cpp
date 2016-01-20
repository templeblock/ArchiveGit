#include "stdafx.h"
#include "Ctp.h"
#include "TransformTool.h"
#include "AGDC.h"
#include "Grid.h"

//#define NEW_STUFF
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CTransformTool::CTransformTool()
{
	m_pSym = NULL;
	m_hWnd = NULL;
	m_bStarted = false;
}

//////////////////////////////////////////////////////////////////////
CTransformTool::~CTransformTool()
{
}

//////////////////////////////////////////////////////////////////////
void CTransformTool::InitEx(const RECT& PageRect, const CAGMatrix& ViewToDeviceMatrix, const CGrid& Grid)
{
	m_PageRect = PageRect;
	m_ViewToDeviceMatrix = ViewToDeviceMatrix;
	m_Grid = Grid;
}

//////////////////////////////////////////////////////////////////////
void CTransformTool::Init(CAGSym* pSym, HWND hWnd, CAGDC* pDC, DRAWPROC lpDrawProc, void* pData)
{
	m_pSym = pSym;
	m_hWnd = hWnd;

	int iWhatCanDo = TR_EXCLUSIVE | TR_SIZE | (m_pSym->IsLine() ? TR_DRAWASLINE : 0) |
		(!m_pSym->IsImage() && !m_pSym->IsAddAPhoto() && !m_pSym->IsLine() ? TR_ROTATE : 0) |
		(m_pSym->IsImage() ? TR_CONSTRAINASPECT : 0);
	RECT SymbolRect = m_pSym->GetDestRect();
	CAGMatrix SymbolMatrix = m_pSym->GetMatrix();
	m_Track.Init(pDC, iWhatCanDo, lpDrawProc, pData, SymbolRect, SymbolRect, m_PageRect, m_ViewToDeviceMatrix, SymbolMatrix, m_Grid);
}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::Start()
{
	m_bStarted = true;
	m_Track.Draw(true/*bOn*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
void CTransformTool::Paint(HDC hDC)
{
	if (!m_bStarted)
		return;

	m_Track.Paint(hDC);
}

//////////////////////////////////////////////////////////////////////
//bool CTransformTool::ButtonDown(int x, int y)
//{
//	if (!m_bStarted)
//		return true;
//
//	// If no handle/border was grabbed, stop the transform
//	if (!m_Track.Down(x, y))
//	{
//		Stop(true/*bSuccess*/);
//		return false;
//	}
//
//	return true;
//}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::ButtonDown(int x, int y)
{
	if (!m_bStarted)
		return true;

	// If no handle/border was grabbed
	if (!m_Track.Down(x, y))
	{
#ifndef NEW_STUFF
		Stop(true/*Success*/);
#endif NEW_STUFF
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::ButtonUp(int x, int y)
{
	if (!m_bStarted)
		return false;

	m_Track.Up(x, y);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::ButtonDblClick(int x, int y)
{
	if (!m_bStarted)
		return false;

	Stop(true/*bSuccess*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::SetCursor(int x, int y)
{
	if (!m_bStarted)
		return false;

	return m_Track.SetCursor(x, y);
}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::MouseMove(int x, int y)
{
	if (!m_bStarted)
		return false;

	bool bMoved = m_Track.Move(x, y);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::Command(int iCommand)
{
	if (!m_bStarted)
		return false;

	bool bShift = (GetAsyncKeyState(VK_SHIFT)<0);
	switch (iCommand)
	{
		case VK_UP:
			m_Track.Delta(0, (bShift ? -5 : -100));
			return true;
		case VK_DOWN:
			m_Track.Delta(0, (bShift ?  5 :  100));
			return true;
		case VK_LEFT:
			m_Track.Delta((bShift ? -5 : -100), 0);
			return true;
		case VK_RIGHT:
			m_Track.Delta((bShift ?  5 :  100), 0);
			return true;
		case VK_RETURN:
		case VK_F12:
			Stop(true/*bSuccess*/);
			return true;
		case VK_ESCAPE:
			Stop(false/*bSuccess*/);
			return true;
		case VK_DELETE:
			::PostMessage(m_hWnd, WM_COMMAND, IDC_DOC_DELETE, 0L);
			Stop(false/*bSuccess*/);
			return true;
		default:
			break;
		}

	return false; /*bHandled*/
}

//////////////////////////////////////////////////////////////////////
bool CTransformTool::Stop(bool bSuccess)
{
	if (!m_bStarted)
		return false;

	m_bStarted = false;
	bool bMoveOnly = m_Track.Done(bSuccess);

	return true;
}
