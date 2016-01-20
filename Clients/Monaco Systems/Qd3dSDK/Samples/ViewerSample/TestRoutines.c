#include "TestRoutines.h"
#include "3DShell.h"
#include "resource.h"

BOOL gSetWindowRegistered = FALSE;
BOOL gGetBitmapWindowRegistered = FALSE;

LRESULT CALLBACK SetWindowProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam)   
{
    int wmId, wmEvent;

    switch (message) 
	{
        case WM_COMMAND:

            wmId    = LOWORD(uParam);
            wmEvent = HIWORD(uParam);

            switch (wmId) 
			{
				default:
                    return (DefWindowProc(hWnd, message, uParam, lParam));
            }
            break;

		case WM_DESTROY:
			Q3WinViewerSetWindow (gViewer, gHwnd);
			return 0;
			break;

        default:          // Passes it on if unproccessed
			return (DefWindowProc(hWnd, message, uParam, lParam));
    }
    return (0);
}

void DoSetWindowTest ()
{
	HWND		window;
	WNDCLASS	wc;
    ATOM		atom;
    
	if (gSetWindowRegistered == FALSE)
	{
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = (WNDPROC)SetWindowProc;       
		wc.cbClsExtra    = 0;                      
		wc.cbWndExtra    = 0;                     
		wc.hInstance     = hInst;             
		wc.hIcon         = LoadIcon (hInst, MAKEINTRESOURCE(IDI_APP)); 
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL; 
		wc.lpszClassName = "SetWindowTestWindowClass";              
			
		atom = RegisterClass(&wc);

		if (atom == 0)
		{
			MessageBox (gHwnd, "SetWindowTest: RegisterClass failed", "SetWindowTest error", MB_OK);
			return;
		}

		gSetWindowRegistered = TRUE;
	}

	window =  CreateWindowEx(WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,	// extended window style
					"SetWindowTestWindowClass",	// pointer to registered class name
					"Set Window Test",	// pointer to window name
					WS_OVERLAPPED | WS_CAPTION |WS_THICKFRAME | WS_SYSMENU,	// window style
					450, 25, 400, 450, 	 // fixed size windows
					NULL,	// handle to parent or owner window
					NULL,	// handle to menu, or child-window identifier
					hInst,	// handle to application instance
					NULL 	// pointer to window-creation data
		);

	if (window == NULL)
	{
		MessageBox (gHwnd, "SetWindowTest: CreateWindowEx failed", "SetWindowTest error", MB_OK);
		return;
	}

	ShowWindow (window, SW_SHOW);
	UpdateWindow (window);
	if (Q3WinViewerSetWindow (gViewer, window) == kQ3Failure)
	{
		MessageBox (gHwnd, "SetWindowTest: Q3WinViewerSetWindow Failed", "SetWindowTest error", MB_OK);
		DestroyWindow (window);
		return;
	}
}

void DoTestMinDimension ()
{
	unsigned long width, height;
	char string[255];

	if (Q3WinViewerGetMinimumDimension (gViewer, &width, &height) == kQ3Failure)
	{
		MessageBox (gHwnd, "TestMinDimension: Q3WinViewerGetMinimum failed", "TestMinDimension error", MB_OK);
		return;
	}
	sprintf (string, "Minimum Dimension - Height: %d Width: %d", height, width); 
	MessageBox (gHwnd, string, "Q3WinViewerGetMinimumDimension test", MB_OK);
}

void DoTestGetButtonRect ()
{
	char string[500];
	RECT rect;
	int index = 0;
	int count;

	if (Q3WinViewerGetButtonRect (gViewer, kQ3ViewerButtonCamera, &rect) == kQ3Success)
		count = sprintf (&(string[index]), "Camera Button - top: %d left: %d bottom: %d right: %d\n", rect.top, rect.left, rect.bottom, rect.right);
	else
		count = sprintf (&(string[index]), "Camera Button - failed\n");

	index += count;

	if (Q3WinViewerGetButtonRect (gViewer, kQ3ViewerButtonTruck, &rect) == kQ3Success)
		count = sprintf (&(string[index]), "Truck Button - top: %d left: %d bottom: %d right: %d\n", rect.top, rect.left, rect.bottom, rect.right);
	else
		count = sprintf (&(string[index]), "Truck Button - failed\n");

	index += count;	

	if (Q3WinViewerGetButtonRect (gViewer, kQ3ViewerButtonOrbit, &rect) == kQ3Success)
		count = sprintf (&(string[index]), "Rotate Button - top: %d left: %d bottom: %d right: %d\n", rect.top, rect.left, rect.bottom, rect.right);
	else
		count = sprintf (&(string[index]), "Rotate Button - failed\n");

	index += count;	

	if (Q3WinViewerGetButtonRect (gViewer, kQ3ViewerButtonZoom, &rect) == kQ3Success)
		count = sprintf (&(string[index]), "Zoom Button - top: %d left: %d bottom: %d right: %d\n", rect.top, rect.left, rect.bottom, rect.right);
	else
		count = sprintf (&(string[index]), "Zoom Button - failed\n");

	index += count;	

	if (Q3WinViewerGetButtonRect (gViewer, kQ3ViewerButtonDolly, &rect) == kQ3Success)
		count = sprintf (&(string[index]), "Dolly Button - top: %d left: %d bottom: %d right: %d\n", rect.top, rect.left, rect.bottom, rect.right);
	else
		count = sprintf (&(string[index]), "Dolly Button - failed\n");

	index += count;	

	if (Q3WinViewerGetButtonRect (gViewer, kQ3ViewerButtonReset, &rect) == kQ3Success)
		count = sprintf (&(string[index]), "Reset Button - top: %d left: %d bottom: %d right: %d\n", rect.top, rect.left, rect.bottom, rect.right);
	else
		count = sprintf (&(string[index]), "Reset Button - failed\n");

	index += count;	

	MessageBox (gHwnd, string, "Q3WinViewerGetButtonRect test", MB_OK);
}


void DoTestSetCurrentButton (int id)
{
	switch (id)
	{
		case IDM_TESTSETCAMERABUTTON: 
			if (Q3WinViewerSetCurrentButton (gViewer, kQ3ViewerButtonCamera) == kQ3Failure)
				MessageBox (gHwnd, "SetCurrentButtonTest: Q3WinViewerSetCurrentButton (kQ3ViewerButtonCamera) failed", "SetCurrentButtonTest error", MB_OK);
			break;
		case IDM_TESTSETTRUCKBUTTON:
			if (Q3WinViewerSetCurrentButton (gViewer, kQ3ViewerButtonTruck) == kQ3Failure)
				MessageBox (gHwnd, "SetCurrentButtonTest: Q3WinViewerSetCurrentButton (kQ3ViewerButtonTruck) failed", "SetCurrentButtonTest error", MB_OK);
			break;
		case IDM_TESTSETROTATEBUTTON:
			if (Q3WinViewerSetCurrentButton (gViewer, kQ3ViewerButtonOrbit) == kQ3Failure)
				MessageBox (gHwnd, "SetCurrentButtonTest: Q3WinViewerSetCurrentButton (kQ3ViewerButtonOrbit) failed", "SetCurrentButtonTest error", MB_OK);
			break;
		case IDM_TESTSETZOOMBUTTON:
			if (Q3WinViewerSetCurrentButton (gViewer, kQ3ViewerButtonZoom) == kQ3Failure)
				MessageBox (gHwnd, "SetCurrentButtonTest: Q3WinViewerSetCurrentButton (kQ3ViewerButtonZoom) failed", "SetCurrentButtonTest error", MB_OK);
			break;
		case IDM_TESTSETDOLLYBUTTON:
			if (Q3WinViewerSetCurrentButton (gViewer, kQ3ViewerButtonDolly) == kQ3Failure)
				MessageBox (gHwnd, "SetCurrentButtonTest: Q3WinViewerSetCurrentButton (kQ3ViewerButtonDolly) failed", "SetCurrentButtonTest error", MB_OK);
			break;
	}
}

LRESULT CALLBACK GetBitmapProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam)   
{
    int wmId, wmEvent;

    switch (message) 
	{
        case WM_COMMAND:

            wmId    = LOWORD(uParam);
            wmEvent = HIWORD(uParam);

            switch (wmId) 
			{
				default:
                    return (DefWindowProc(hWnd, message, uParam, lParam));
            }
            break;

		case WM_DESTROY:
		{
			HBITMAP bitmap = (HBITMAP) GetWindowLong (hWnd, GWL_USERDATA);

			if (bitmap != NULL)
				DeleteObject (bitmap);

			return 0;
		}
			break;

		case WM_PAINT:
		{
			HBITMAP bitmap = (HBITMAP) GetWindowLong (hWnd, GWL_USERDATA);

			if (bitmap != NULL)
			{
				HDC fromDC, toDC;
				PAINTSTRUCT paintStruct;

				toDC = GetDC (hWnd);
				
				if (toDC == NULL)
				{
					MessageBox (gHwnd, "GetButtonTest: GetDC failed", "GetBitmapTest error", MB_OK);
					return 0;
				}

				fromDC = CreateCompatibleDC (toDC);

				if (fromDC == NULL)
				{
					MessageBox (gHwnd, "GetButtonTest: CreateCompatibleDC failed", "GetBitmapTest error", MB_OK);
					return 0;
				}

				SelectObject (fromDC, bitmap);

				if (BeginPaint (hWnd, &paintStruct) != NULL)
				{
					BITMAP map;
					int		bytes;

					bytes = GetObject (bitmap, sizeof (BITMAP), &map);

					if (bytes == 0)	
						MessageBox (gHwnd, "GetButtonTest: GetObject failed", "GetBitmapTest error", MB_OK);
					else
						BitBlt (toDC, 0, 0, map.bmWidth, map.bmHeight, fromDC, 0, 0, SRCCOPY);
					EndPaint (hWnd, &paintStruct);
				}

				DeleteDC (fromDC);


			}
			else
				MessageBox (gHwnd, "GetButtonTest: GetWindowLong failed", "GetBitmapTest error", MB_OK);

			return 0;

		}
			break;

        default:          // Passes it on if unproccessed
			return (DefWindowProc(hWnd, message, uParam, lParam));
    }
    return (0);
}

void DoTestGetBitmap ()
{
	HBITMAP		bitmap;
	HWND		window;
	WNDCLASS	wc;
    ATOM		atom;
    RECT		rect;

	if (gGetBitmapWindowRegistered == FALSE)
	{
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = (WNDPROC)GetBitmapProc;       
		wc.cbClsExtra    = 0;                      
		wc.cbWndExtra    = 0;                     
		wc.hInstance     = hInst;             
		wc.hIcon         = LoadIcon (hInst, MAKEINTRESOURCE(IDI_APP)); 
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wc.lpszMenuName  = NULL; 
		wc.lpszClassName = "GetBitmapTestWindowClass";              
			
		atom = RegisterClass(&wc);

		if (atom == 0)
		{
			MessageBox (gHwnd, "GetBitmapTest: RegisterClass failed", "GetBitmapTest error", MB_OK);
			return;
		}

		gGetBitmapWindowRegistered = TRUE;
	}


	bitmap = Q3WinViewerGetBitmap (gViewer);

	if (bitmap == NULL)
	{
		MessageBox (gHwnd, "GetButtonTest: Q3WinViewerGetBitmap failed", "GetBitmapTest error", MB_OK);
		return;
	}

	if (GetWindowRect (gHwnd, &rect) == FALSE)
	{
		MessageBox (gHwnd, "GetButtonTest: GetWindowRect failed", "GetBitmapTest error", MB_OK);
		DeleteObject (bitmap);
		return;
	}

	window =  CreateWindowEx(WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,	// extended window style
				"GetBitmapTestWindowClass",	// pointer to registered class name
				"Q3WinViewerGetBitmap Test",	// pointer to window name
				WS_OVERLAPPED | WS_CAPTION |WS_THICKFRAME | WS_SYSMENU,	// window style
				450, 25, rect.right-rect.left, rect.bottom-rect.top, 	 // fixed size windows
				NULL,	// handle to parent or owner window
				NULL,	// handle to menu, or child-window identifier
				hInst,	// handle to application instance
				NULL 	// pointer to window-creation data
	);

	if (window == NULL)
	{
		MessageBox (gHwnd, "GetButtonTest: CreateWindowEx failed", "GetBitmapTest error", MB_OK);
		DeleteObject (bitmap);
		return;
	}

	(void) SetWindowLong (window, GWL_USERDATA, (long) bitmap);

	ShowWindow (window, SW_SHOW);
	UpdateWindow (window);

}

void DoTestWriteData ()
{
	long size, actualSize;
	void *data;

	if (Q3WinViewerWriteData (gViewer, NULL, 0, &actualSize) == kQ3Failure)
	{
		MessageBox (gHwnd, "WriteDataTest: 1st Q3WinViewerWriteData failed", "WriteDataTest error", MB_OK);
		return;
	}

	data = malloc (actualSize);

	if (data == NULL)
	{
		MessageBox (gHwnd, "WriteDataTest: malloc failed", "WriteDataTest error", MB_OK);
		return;
	}

	size = actualSize;

	if (Q3WinViewerWriteData (gViewer, data, size, &actualSize) == kQ3Failure)
	{
		MessageBox (gHwnd, "WriteDataTest: 2nd Q3WinViewerWriteData failed", "WriteDataTest error", MB_OK);
		free (data);
		return;
	}

	if (size != actualSize)
	{
		char string[255];

		sprintf (string, "WriteDataTest: Sizes are different.  Size: %d ActualSize: %d", size, actualSize);
		MessageBox (gHwnd, string, "WriteDataTest error", MB_OK);
	}

	free (data);

}

