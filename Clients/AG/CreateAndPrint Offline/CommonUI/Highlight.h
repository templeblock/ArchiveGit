#pragma once

#include "Track.h"

class CDocWindow;
class CAGSym;

class CHighlight
{
public:
	CHighlight(CDocWindow* pDocWnd);
	~CHighlight();
	void DoHighlight(int x, int y, CAGSym* pDragSym, bool bDrop);
	void Paint(HDC pDC);
	void Highlight();

	static void CALLBACK SelFlashProc_Wrapper(HWND hWnd, UINT message, UINT idTimer, DWORD dwTime);
	static void * pObject;
	static bool g_bOn;

	int m_uiTimer;

protected:
	void StartHighlight();
	void StopHighlight();

protected:

    CDocWindow* m_pDocWnd;
	CAGSym* m_pHlSym;
	CTrack m_Track;
};

