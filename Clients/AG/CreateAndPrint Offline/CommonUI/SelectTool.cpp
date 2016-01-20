#include "stdafx.h"
#include "ctp.h"
#include "SelectTool.h"
#include "Selection.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CSelectTool::CSelectTool()
{
	m_hWnd = NULL;
	m_hDC = NULL;
	m_bStarted = false;
	m_AspectX = 1;
	m_AspectY = 1;
	m_StartPoint.x = 0;
	m_StartPoint.y = 0;
	::SetRectEmpty(&m_SelectRect);
}

//////////////////////////////////////////////////////////////////////
CSelectTool::~CSelectTool()
{
}

//////////////////////////////////////////////////////////////////////
void CSelectTool::Init(CAGSym* pSym, HWND hWnd, HDC hDC, DRAWPROC lpDrawProc, void* pData)
{
	m_pSym = pSym;
	m_hWnd = hWnd;
	m_hDC = hDC;
	m_lpDrawProc = lpDrawProc;
	m_pData = pData;
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::Start()
{
	m_bStarted = true;
	return true;
}

//////////////////////////////////////////////////////////////////////
void CSelectTool::Paint(HDC hDC)
{
	if (!m_bStarted)
		return;

	if (!hDC)
		return;

	HDC hSaveDC = m_hDC;
	m_hDC = hDC;
	CSelection::InvertSelection(m_hWnd, m_hDC, &m_SelectRect, SL_BOXHANDLES);
	m_hDC = hSaveDC;
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::ButtonDown(int x, int y)
{
	if (!m_bStarted)
	{
		Start();
		if (!m_bStarted)
			return false;
	}

	m_StartPoint.x = x;
	m_StartPoint.y = y;

	::SetRectEmpty(&m_SelectRect);
	CSelection::StartSelection(m_hWnd, m_hDC, &m_SelectRect, SL_BOXHANDLES | SL_SPECIAL, m_StartPoint, 0/*fStatusScale*/);

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::ButtonUp(int x, int y)
{
	if (!m_bStarted)
		return false;

	Stop(true/*bSuccess*/);

	// Bound the point to the window
	RECT ClientRect;
	GetClientRect(m_hWnd, &ClientRect);
	x = bound(x, ClientRect.left, ClientRect.right);
	y = bound(y, ClientRect.top, ClientRect.bottom);

	bool bConstrain = (GetAsyncKeyState(VK_SHIFT)<0);
	if (bConstrain)
	{ // Constrain it
		int dx = abs(m_StartPoint.x - x);
		int dy = abs(m_StartPoint.y - y);
		if (m_AspectY * dx < m_AspectX * dy)
		{
			y = m_StartPoint.y + (m_AspectY * ((y > m_StartPoint.y) ? dx : -dx)) / m_AspectX;
		}
		else
		{
			x = m_StartPoint.x + (m_AspectX * ((x > m_StartPoint.x) ? dy : -dy)) / m_AspectY;
		}
	}

	CSelection::EndSelection(m_hWnd, m_hDC, &m_SelectRect, SL_BOXHANDLES, true/*bRemove*/);
	CSelection::InvertSelection(m_hWnd, m_hDC, &m_SelectRect, SL_BOXHANDLES);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::ButtonDblClick(int x, int y)
{
	if (!m_bStarted)
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::SetCursor(int x, int y)
{
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::MouseMove(int x, int y)
{
	if (!m_bStarted)
		return false;

	// Bound the point to the window
	RECT ClientRect;
	GetClientRect(m_hWnd, &ClientRect);
	x = bound(x, ClientRect.left, ClientRect.right);
	y = bound(y, ClientRect.top, ClientRect.bottom);

	bool bConstrain = (GetAsyncKeyState(VK_SHIFT)<0);
	if (bConstrain)
	{ // Constrain it
		int dx = abs(m_StartPoint.x - x);
		int dy = abs(m_StartPoint.y - y);
		if (m_AspectY * dx < m_AspectX * dy)
		{
			y = m_StartPoint.y + (m_AspectY * ((y > m_StartPoint.y) ? dx : -dx)) / m_AspectX;
		}
		else
		{
			x = m_StartPoint.x + (m_AspectX * ((x > m_StartPoint.x) ? dy : -dy)) / m_AspectY;
		}
	}

	POINT Point;
	Point.x = x;
	Point.y = y;
	CSelection::UpdateSelection(m_hWnd, m_hDC, &m_SelectRect, SL_BOXHANDLES, Point, bConstrain, m_AspectX, m_AspectY, false/*bMove*/, false/*bFromCenter*/);
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::Command(int iCommand)
{
	if (!m_bStarted)
		return false;

	return false; /*bHandled*/
}

//////////////////////////////////////////////////////////////////////
bool CSelectTool::Stop(bool bSuccess)
{
	if (!m_bStarted)
		return false;

	CSelection::InvertSelection(m_hWnd, m_hDC, &m_SelectRect, SL_BOXHANDLES);
	m_bStarted = false;
	return true;
}
