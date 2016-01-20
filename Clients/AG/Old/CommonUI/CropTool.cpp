#include "stdafx.h"
#include "Ctp.h"
#include "CropTool.h"
#include "AGDC.h"
#include "Grid.h"

//#define NEW_STUFF
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CCropTool::CCropTool()
{
	m_pSym = NULL;
	m_hWnd = NULL;
	m_bStarted = false;

}

//////////////////////////////////////////////////////////////////////
CCropTool::~CCropTool()
{
}

//////////////////////////////////////////////////////////////////////
void CCropTool::InitEx(const RECT& PageRect, const CAGMatrix& ViewToDeviceMatrix, const CGrid& Grid)
{
	m_PageRect = PageRect;
	m_ViewToDeviceMatrix = ViewToDeviceMatrix;
	m_Grid = Grid;
}

//////////////////////////////////////////////////////////////////////
void CCropTool::Init(CAGSym* pSym, HWND hWnd, CAGDC* pDC, DRAWPROC lpDrawProc, void* pData)
{
	m_pSym = pSym;
	m_hWnd = hWnd;

	int iWhatCanDo = TR_EXCLUSIVE | TR_SIZE | TR_BOUNDTOSYMBOL | (m_pSym->IsLine() ? TR_DRAWASLINE : 0);
	RECT SymbolRect = ((CAGSymImage*)m_pSym)->GetDestRect();
	RECT CropRect = ((CAGSymImage*)m_pSym)->GetCropRect();
	CAGMatrix SymbolMatrix = m_pSym->GetMatrix();
	m_Track.Init(pDC, iWhatCanDo, lpDrawProc, pData, CropRect, SymbolRect, m_PageRect, m_ViewToDeviceMatrix, SymbolMatrix, m_Grid);
}

//////////////////////////////////////////////////////////////////////
bool CCropTool::Start()
{
	m_bStarted = true;
	m_Track.Draw(true/*bOn*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
void CCropTool::Paint(HDC hDC)
{
	if (!m_bStarted)
		return;

	m_Track.Paint(hDC);
}

////////////////////////////////////////////////////////////////////////
//bool CCropTool::ButtonDown(int x, int y)
//{
//	if (!m_bStarted)
//		return true;
//
//	// If no handle was grabbed, stop the Crop
//	if (!m_Track.Down(x, y))
//	{
//		Stop(true/*bSuccess*/);
//		return false;
//	}
//
//	return true;
//}
//
//////////////////////////////////////////////////////////////////////
bool CCropTool::ButtonDown(int x, int y)
{
	if (!m_bStarted)
		return true;

	// If no handle was grabbed, stop the Crop
	if (!m_Track.Down(x, y))
	{
#ifndef NEW_STUFF
		Stop(true/*Success*/);
#endif
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CCropTool::ButtonUp(int x, int y)
{
	if (!m_bStarted)
		return false;

	m_Track.Up(x, y);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CCropTool::ButtonDblClick(int x, int y)
{
	if (!m_bStarted)
		return false;

	Stop(true/*bSuccess*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CCropTool::SetCursor(int x, int y)
{
	if (!m_bStarted)
		return false;

	return m_Track.SetCursor(x, y);
}

//////////////////////////////////////////////////////////////////////
bool CCropTool::MouseMove(int x, int y)
{
	if (!m_bStarted)
		return false;

	bool bMoved = m_Track.Move(x, y);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CCropTool::Command(int iCommand)
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
bool CCropTool::Stop(bool bSuccess)
{
	if (!m_bStarted)
		return false;

	m_bStarted = false;
	bool bMoveOnly = m_Track.Done(bSuccess);

	return true;
}
