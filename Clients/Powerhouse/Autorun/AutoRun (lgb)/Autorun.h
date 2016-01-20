#define ID_CMDLINE			1
#define ID_ININAME          2
#define ID_SECTION          3

#define IDM_EXIT           101
#define IDM_CONTINUE       102
#define IDM_INSTALL		   103

BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
