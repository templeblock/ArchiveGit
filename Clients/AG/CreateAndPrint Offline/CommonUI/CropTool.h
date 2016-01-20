#pragma once

#include "MouseTool.h"
#include "Track.h"

class CCropTool : public CMouseTool  
{
public:
	CCropTool();
	virtual ~CCropTool();

	void Init(CAGSym* pSym, HWND hWnd, CAGDC* pDC, DRAWPROC lpDrawProc, void* pData);
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
public:
	void InitEx(const RECT& PageRect, const CAGMatrix& ViewToDeviceMatrix, const CGrid& Grid);
	const CAGMatrix& GetMatrix()
		{ return m_Track.GetMatrix(); }
	CAGMatrix GetChangeMatrix()
		{ return m_Track.GetChangeMatrix(); }

protected:
	CAGSym* m_pSym;
	HWND m_hWnd;
	bool m_bStarted;
	RECT m_PageRect;
	CTrack m_Track;
	CAGMatrix m_ViewToDeviceMatrix;
	CGrid m_Grid;
};
