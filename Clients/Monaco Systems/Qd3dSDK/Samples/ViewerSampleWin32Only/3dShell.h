#ifndef __3DSHELL__
#define __3DSHELL__

#include <windows.h>

extern BOOL InitApplication(HANDLE);
extern BOOL InitInstance(HANDLE, int);
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

extern HINSTANCE hInst;
extern HWND gViewer;
extern HWND gHwnd;
#endif
