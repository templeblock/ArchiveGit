#pragma once

#include "BaseTool.h"
#include "Track.h"
#include "DocWindow.h"
#include "ImageObject.h"

class CCropTool : public CBaseTool  
{
public:
	CCropTool();
	virtual ~CCropTool();

	void Init(CDocWindow& DocWindow, CImageObject& ImageObject, DRAWPROC lpDrawProc, void* pData) {}
	void Init(CAGDC* pDC, DRAWPROC lpDrawProc, void* pData);
	bool Start();
	bool Command(int iCommand);
	bool Stop(bool bSuccess);
	bool IsStarted()
		{ return m_bStarted; };

	void Paint(HDC hDC);
	bool MouseMove(int x, int y);
	bool SetCursor(int x, int y);
	bool ButtonDown(int x, int y);
	bool ButtonUp(int x, int y);
	bool DoubleClick(int x, int y);
	bool RightClick(int x, int y) { return false; }
	bool Char(UINT nChar) { return false; }
	bool KeyDown(int nChar) { return false; }
	bool KeyUp(int nChar) { return false; }
public:
	void InitEx(const CRect& PageRect, const CAGMatrix& ViewToDeviceMatrix, const CGrid& Grid);
	const CAGMatrix& GetMatrix()
		{ return m_Track.GetMatrix(); }
	CAGMatrix GetChangeMatrix()
		{ return m_Track.GetChangeMatrix(); }

protected:
	bool m_bStarted;
	CRect m_PageRect;
	CTrack m_Track;
	CAGMatrix m_ViewToDeviceMatrix;
	CGrid m_Grid;
};
