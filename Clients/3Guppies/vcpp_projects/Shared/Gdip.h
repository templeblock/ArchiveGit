#pragma once

#include "GdiPlusDup.h"
using namespace Gdiplus;
using namespace DllExports;
// Do not staticly link GdiPlus.dll as in: #pragma comment(lib, "gdiplus.lib")

#define CheckOk(expr) if(status = expr) throw status;

class CGdip
{
public:
	CGdip();
	~CGdip();
	static bool IsOK();

private:
	bool UsingList();

private:
	static HMODULE m_hGdiPlusLib;
	static bool m_bInitFailed;
	static ULONG_PTR m_dwToken;
	static int m_iRefCount;
};
