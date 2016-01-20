#pragma once

#define TOOLCODE_ABORT	-1
#define TOOLCODE_UNUSED	0
#define TOOLCODE_DONE	1
#define TOOLCODE_UPDATE	2

typedef void (*DRAWPROC)(HDC hDC, bool bOn, int iToolCode, void* pData);

class CBaseTool  
{
public:
//j	virtual void Init(CDocWindow& DocWindow, CImageObject& ImageObject, DRAWPROC lpDrawProc, void* pData) {}
	virtual void Init1(...) {};
	virtual bool Start() = NULL;
	virtual bool Command(int iCommand) = NULL;
	virtual bool Stop(bool bSuccess) = NULL;
	virtual bool IsStarted() = NULL;

	virtual void Paint(HDC hDC) = NULL;
	virtual bool MouseMove(int x, int y) = NULL;
	virtual bool SetCursor(int x, int y) = NULL;
	virtual bool ButtonDown(int x, int y) = NULL;
	virtual bool ButtonUp(int x, int y) = NULL;
	virtual bool DoubleClick(int x, int y) = NULL;
	virtual bool RightClick(int x, int y) = NULL;

	virtual bool Char(UINT nChar) = NULL;
	virtual bool KeyDown(int nChar) = NULL;
	virtual bool KeyUp(int nChar) = NULL;

private:
//j	CImageObject& m_ImageObject;
//j	CDocWindow& m_DocWindow;
};
