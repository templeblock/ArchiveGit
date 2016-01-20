#include <windows.h>		// required for all Windows applications
#include "resource.h"		// Windows resource IDs
#include "3dshell.h"		// specific to this program

#include <stdio.h>
#include "QD3DWinViewer.h"

HINSTANCE hInst;          	// current instance
HWND gHwnd;					// main hwnd

HWND		gViewer;

char szAppName[] = "QuickDraw 3D Viewer Demo";		// The name of this application
char szTitle[]   = "QuickDraw 3D Viewer Demo";		// The title bar text

/* define NOLINKVIEWER if you want to dynamically link to 3DViewer.LIB */
#if defined (NOLINKVIEWER) 

#if defined( _DEBUG )
#define VIEWERDLL "3DViewer_D.DLL"
#else
#define VIEWERDLL "3DViewer.DLL"
#endif 

#endif /* NOLINKVIEWER */

int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{

        MSG msg;
        HANDLE hAccelTable;
		BOOL aStatus = FALSE;

#if !defined (NOLINKVIEWER) /* dummy call to Q3Viewer to force link */
		Q3WinViewerGetState( NULL );
#endif /* !NOLINKVIEWER */

		
		if (!InitApplication(hInstance)) 
		{
			return (FALSE);    
		}

        if (!InitInstance(hInstance, nCmdShow)) 
		{
			return (FALSE);
        }

        hAccelTable = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_GENERIC));
		
		(void) InvalidateRect(gHwnd, NULL, FALSE);	

	    while (GetMessage(&msg, NULL, 0, 0)) {
			if (!TranslateAccelerator (gHwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

        return (msg.wParam); // Returns the value from PostQuitMessage
}


BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASS  wc;
	ATOM	  success;
    
	wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WndProc;       
    wc.cbClsExtra    = 0;                      
    wc.cbWndExtra    = 0;                     
    wc.hInstance     = hInstance;             
    wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APP)); 
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_GENERIC); 
	wc.lpszClassName = szAppName;              
	
    success = RegisterClass(&wc);

	return success > 0 ? TRUE : FALSE;
}

BOOL InitInstance(
        HINSTANCE       hInstance,
        int             nCmdShow)
{

    hInst = hInstance; 
	gHwnd =  CreateWindowEx(
		WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,	// extended window style
		szAppName,	// pointer to registered class name
		szTitle,	// pointer to window name
		WS_OVERLAPPED | WS_CAPTION |WS_THICKFRAME | WS_SYSMENU,	// window style
        25, 25, 400, 450, 	 // fixed size windows
		NULL,	// handle to parent or owner window
		NULL,	// handle to menu, or child-window identifier
		hInstance,	// handle to application instance
		NULL 	// pointer to window-creation data
	);

    if (!gHwnd) 
	{
        return (FALSE);
    }


    ShowWindow(gHwnd, nCmdShow); 
    UpdateWindow(gHwnd); 

	return( TRUE );
}


LRESULT CALLBACK WndProc(
                HWND hWnd,        
                UINT message,      
                WPARAM uParam,     
                LPARAM lParam)   
{
        int wmId, wmEvent;

        switch (message) 
		{
            case WM_COMMAND:

                wmId    = LOWORD(uParam);
                wmEvent = HIWORD(uParam);

                switch (wmId) 
				{
                    case IDM_EXIT:
                        DestroyWindow (hWnd);
                        break;

					default:
                        return (DefWindowProc(hWnd, message, uParam, lParam));
                }
                break;


			case WM_CREATE:
				{
				RECT rect;
				DWORD	dwFlags, flags;
#if defined (NOLINKVIEWER) /* not linked with 3DViewer.LIB */
				HMODULE hDll;
#endif 
				gViewer = NULL;
				
				flags = kQ3ViewerDefault;
				GetClientRect(hWnd, (LPRECT)&rect);

				dwFlags = WS_VISIBLE;

				if (!(flags & WS_POPUP))
					dwFlags |= WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

#if defined (NOLINKVIEWER) /* we are not linked with 3DViewer.LIB; manually load library */
				if( hDll = LoadLibrary( VIEWERDLL ) )
					gViewer =  CreateWindowEx (
								flags & kQ3ViewerDraggingInOff ? 0 : WS_EX_ACCEPTFILES , 
								kQ3ViewerClassName, 
								"My 3D Viewer", 
								dwFlags | flags, 
								rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
								hWnd, 
								NULL, 
								hInst, 
								NULL);
#else  /* linked with 3DViewer.LIB */
				gViewer =  CreateWindowEx (
							flags & kQ3ViewerDraggingInOff ? 0 : WS_EX_ACCEPTFILES , 
							kQ3ViewerClassName,		/* this is the only symbol in 3DViewer we link against */
							"My 3D Viewer", 
							dwFlags | flags, 
							rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
							hWnd, 
							NULL, 
							hInst, 
							NULL);
#endif 
				if( gViewer == NULL )
					return -1;
				}
				break;

            case WM_DESTROY:  // message: window being destroyed
                PostQuitMessage(0);
                break;

			case WM_SETFOCUS:
				SetFocus( gViewer );
				break;

			case WM_SYSCOLORCHANGE:
				SendMessage (gViewer , WM_SYSCOLORCHANGE, 0 , 0);
				break;

            default:          // Passes it on if unproccessed
                    return (DefWindowProc(hWnd, message, uParam, lParam));
        }
        return (0);
}





