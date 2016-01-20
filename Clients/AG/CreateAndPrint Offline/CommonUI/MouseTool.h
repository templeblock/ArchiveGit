#pragma once

#define TOOLCODE_ABORT	-1
#define TOOLCODE_UNUSED	0
#define TOOLCODE_DONE	1
#define TOOLCODE_UPDATE	2

typedef void (*DRAWPROC)(HDC hDC, bool bOn, int iToolCode, void* pData);

class CAGSym;
class CAGDC;

class CMouseTool  
{
public:
	CMouseTool() {};
	virtual ~CMouseTool() {};

	virtual void Init(CAGSym* pSym, HWND hWnd, CAGDC* pDC, DRAWPROC lpDrawProc, void* pData) = NULL;
	virtual void Paint(HDC hDC) = NULL;
	virtual bool Start() = NULL;
	virtual bool ButtonDown(int x, int y) = NULL;
	virtual bool ButtonUp(int x, int y) = NULL;
	virtual bool ButtonDblClick(int x, int y) = NULL;
	virtual bool SetCursor(int x, int y) = NULL;
	virtual bool MouseMove(int x, int y) = NULL;
	virtual bool Command(int iCommand) = NULL;
	virtual bool Stop(bool bSuccess) = NULL;
	virtual bool IsStarted() = NULL;
	virtual CAGSym* GetSymbol() = NULL;
};
