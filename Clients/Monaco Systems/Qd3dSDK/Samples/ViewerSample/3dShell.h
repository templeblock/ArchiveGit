#ifndef __3DSHELL__
#define __3DSHELL__

#include "QD3D.h"
#include "QD3DWinViewer.h"
#include <windows.h>

extern BOOL InitApplication(HANDLE);
extern BOOL InitInstance(HANDLE, int);
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK About  (HWND, UINT, WPARAM, LPARAM);

extern HINSTANCE hInst;
extern TQ3ViewerObject gViewer;
extern HWND gHwnd;
#endif
