/*
// DLL testing program.
//
// $Workfile: DLLTEST.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/DLLTEST.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:14   Fred
// Initial revision.
// 
//    Rev 1.1   14 Oct 1996 13:34:30   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:28   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:44:38   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:57:58   JAY
// Initial revision.
// 
//    Rev 1.5   30 Nov 1995 10:20:12   FRED
//  
*/

#define STRICT
#include <windows.h>
#include <stdio.h>

#include "iupgrade.h"

#define USE_WINDOW

HINSTANCE m_hInstance = NULL;
HWND m_hWndMain = NULL;

#ifdef USE_WINDOW
void CreateMyWindow(void);
void InitInstance(HINSTANCE hInstance, HINSTANCE hPrevInstance);
#endif

int PASCAL WinMain(HINSTANCE hInstance,
						 HINSTANCE hPrevInstance,
						 LPSTR lpCmdLine,
						 int nCmdShow)
{
	m_hInstance = hInstance;
#ifdef USE_WINDOW
	InitInstance(hInstance, hPrevInstance);
	CreateMyWindow();
#endif

	// Invoke the "Upgrade" function of the DLL.
	int nResult = (int)Upgrade(m_hWndMain, NULL, 2, 123456L);

	char result_buffer[80];
	sprintf(result_buffer, "Result (%d)", nResult);

	char buffer[80];
	sprintf(buffer, "Upgrade returned %s", result_buffer);
	MessageBox(m_hWndMain, buffer, "Upgrade", MB_OK);

#ifdef USE_WINDOW
	if (m_hWndMain != NULL)
	{
		DestroyWindow(m_hWndMain);
	}
#endif

	return 0;
}

#ifdef USE_WINDOW

#ifdef WIN32
LONG FAR PASCAL TestWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#else
LONG FAR PASCAL __export TestWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#endif
{
	switch(msg)
	{
		case WM_DESTROY:
		{
			if (m_hWndMain == hwnd)
			{
				m_hWndMain = NULL;
			}
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	return 0;
}

void InitInstance(HINSTANCE hInstance, HINSTANCE hPrevInstance)
{
	// Register the window class if this is the first instance.
	if (hPrevInstance == NULL)
	{
		WNDCLASS wc;

		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "UpgradeTestApp";
		wc.hInstance        = hInstance;
		wc.hIcon            = NULL;
		wc.hCursor          = NULL;
		wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW+1);
		wc.style            = 0;
		wc.lpfnWndProc      = TestWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = 0;

		RegisterClass(&wc);
	}
}

void CreateMyWindow(void)
{
	if ((m_hWndMain = CreateWindow("UpgradeTestApp",
								 "Upgrade Test Application",
								 WS_OVERLAPPEDWINDOW,
								 CW_USEDEFAULT, 0,
								 CW_USEDEFAULT, CW_USEDEFAULT,
								 NULL, NULL, m_hInstance, NULL)) != NULL)
	{
		ShowWindow(m_hWndMain, SW_SHOW);
		UpdateWindow(m_hWndMain);
	}
}

#endif
