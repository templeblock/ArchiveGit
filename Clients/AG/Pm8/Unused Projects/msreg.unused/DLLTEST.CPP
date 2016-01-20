/*
// DLL testing program.
//
// $Workfile: DLLTEST.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/DLLTEST.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.5   30 Nov 1995 10:20:12   FRED
//  
*/

#include "msreg.h"

#include <stdio.h>

#define USE_WINDOW

HINSTANCE m_hInstance = NULL;
HWND m_hWndMain = NULL;

#ifdef USE_WINDOW
void CreateMyWindow(void);
void InitInstance(HINSTANCE hInstance, HINSTANCE hPrevInstance);
#endif

void AppendRegistrationInfo(LPSTR pszBuffer);
void GetRegistrationData(LPCSTR pszField, LPSTR pszBuffer, int nBuffer);

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

	// Invoke the "register" function of the DLL.
	REGRESULT Result = RegSendCommand(m_hWndMain, "Register", 0, NULL);

	char result_buffer[80];
	switch (Result)
	{
		case REGRESULT_CommandError:
		{
			sprintf(result_buffer, "REGRESULT_CommandError (%ld)", Result);
			break;
		}
		case REGRESULT_Error:
		{
			sprintf(result_buffer, "REGRESULT_Error (%ld)", Result);
			break;
		}
		case REGRESULT_NotRegistered:
		{
			sprintf(result_buffer, "REGRESULT_NotRegistered (%ld)", Result);
			break;
		}
		case REGRESULT_AlreadyRegistered:
		{
			sprintf(result_buffer, "REGRESULT_AlreadyRegistered (%ld)", Result);
			AppendRegistrationInfo(result_buffer);
			break;
		}
		case REGRESULT_NowRegistered:
		{
			sprintf(result_buffer, "REGRESULT_NowRegistered (%ld)", Result);
			AppendRegistrationInfo(result_buffer);
			break;
		}
		default:
		{
			sprintf(result_buffer, "Unknown (%ld)", Result);
			break;
		}
	}

	char buffer[80];
	sprintf(buffer, "Register returned %s", result_buffer);
	MessageBox(m_hWndMain, buffer, "Register", MB_OK);

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
		wc.lpszClassName    = "RegisterTestApp";
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
	if ((m_hWndMain = CreateWindow("RegisterTestApp",
								 "Register Test Application",
								 WS_OVERLAPPEDWINDOW,
								 CW_USEDEFAULT, 0,
								 CW_USEDEFAULT, CW_USEDEFAULT,
								 NULL, NULL, m_hInstance, NULL)) != NULL)
	{
		ShowWindow(m_hWndMain, SW_SHOW);
		UpdateWindow(m_hWndMain);
	}
}

void AppendRegistrationInfo(LPSTR pszBuffer)
{
	char szFirstName[128];
	char szLastName[128];

	GetRegistrationData("FirstName", szFirstName, sizeof(szFirstName));
	GetRegistrationData("LastName", szLastName, sizeof(szLastName));

	lstrcat(pszBuffer, " (");
	lstrcat(pszBuffer, szFirstName);
	lstrcat(pszBuffer, " ");
	lstrcat(pszBuffer, szLastName);
	lstrcat(pszBuffer, ")");
}

void GetRegistrationData(LPCSTR pszField, LPSTR pszBuffer, int nBuffer)
{
	REGDATA_GetRegistrationData Data;
	Data.m_pszField = pszField;
	Data.m_pszData = pszBuffer;
	Data.m_nData = nBuffer;

	REGRESULT Result = RegSendCommand(m_hWndMain, "GetRegistrationData", (DWORD)(LPVOID)(&Data), NULL);
	if (Result != REGRESULT_AlreadyRegistered)
	{
		*pszBuffer = '\0';
	}
}

#endif
