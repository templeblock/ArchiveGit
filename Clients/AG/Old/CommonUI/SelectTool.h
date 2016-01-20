#pragma once

#include "MouseTool.h"

class CSelectTool : public CMouseTool  
{
public:
	CSelectTool();
	virtual ~CSelectTool();

	void Init(CAGSym* pSym, HWND hWnd, HDC hDC, DRAWPROC lpDrawProc, void* pData);
	void Paint(HDC hDC);
	bool Start();
	bool ButtonDown(int x, int y);
	bool ButtonUp(int x, int y);
	bool ButtonDblClick(int x, int y);
	bool SetCursor(int x, int y);
	bool MouseMove(int x, int y);
	bool Command(int iCommand);
	bool Stop(bool bSuccess);
	bool IsStarted()
		{ return m_bStarted; };
	CAGSym* GetSymbol()
		{ return m_pSym; }

protected:
	CAGSym* m_pSym;
	HWND m_hWnd;
	HDC m_hDC;
	DRAWPROC m_lpDrawProc;
	void* m_pData;
	bool m_bStarted;
	int m_AspectX;
	int m_AspectY;
	POINT m_StartPoint;
	RECT m_SelectRect;
};
