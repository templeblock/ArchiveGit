#include <windows.h>		// required for all Windows applications
#include "resource.h"		// Windows resource IDs
#include "3dshell.h"		// specific to this program

#include <stdio.h>
#include "QD3DWinViewer.h"
#include "TestRoutines.h"
#include "QD3DErrors.h"

HINSTANCE hInst;          	// current instance
HWND gHwnd;					// main hwnd

TQ3ViewerObject		gViewer;

COLORREF customColors[16] = { 0L };

char szAppName[] = "QuickDraw 3D Viewer Demo";		// The name of this application
char szTitle[]   = "QuickDraw 3D Viewer Demo";		// The title bar text

void DoFlagCommand (unsigned long flag);
void CreateNewViewer ();

BOOL		OpenModelFile();
void		SaveModelFile ();
TQ3Status	BrowseForPathName(char *inPathName, BOOLEAN fOpen);
void		OurChooseColor ();

void ErrorHandler (
	TQ3Error	firstError,
	TQ3Error	lastError,
	long		reference)
{
	char str[255];

	sprintf (str, "First Error: %d, Last Error: %d", firstError, lastError);

	MessageBox (gHwnd, str, "QD3D error", MB_OK);
}

int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{

        MSG msg;
        HANDLE hAccelTable;
		TQ3Status aStatus = kQ3Failure;

		if (!InitApplication(hInstance)) 
		{
			return (FALSE);    
		}

        if (!InitInstance(hInstance, nCmdShow)) 
		{
			return (FALSE);
        }

		Q3Error_Register (ErrorHandler, 0);

        hAccelTable = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_GENERIC));
		
		if( lpCmdLine != NULL && (strlen (lpCmdLine) > 0))
		{
			HANDLE fileh;

			fileh = CreateFile (lpCmdLine, GENERIC_READ, FILE_SHARE_READ, 
								   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			
			if (fileh != NULL)
				aStatus = Q3WinViewerUseFile (gViewer, fileh);
		}
	
		Q3WinViewerDraw( gViewer );

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
//	HWND containerWindow;

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

	
	
/*	containerWindow = CreateWindowEx (WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,
							kContainerWindowClass,
							"Quickdraw 3D Viewer Test Window",
							WS_OVERLAPPED | WS_CAPTION |WS_THICKFRAME | WS_SYSMENU,
							430, 25, 500, 500,
							NULL,
							NULL,
							hInstance,
							NULL);

	if (containerWindow == NULL)
	{
		MessageBox (gHwnd, "Creating conatiner window failed", "Windows Error", MB_OK);
		return FALSE;
	}

    ShowWindow(containerWindow, SW_SHOW); 
    UpdateWindow(containerWindow); */

	return( TRUE );
}

void DoFlagCommand (unsigned long flag )
{
	unsigned long flags;
	RECT		aRect;
	TQ3Status	status;

	if( NULL == gViewer )
		return;

	flags = Q3WinViewerGetFlags (gViewer);

	if (flags & flag) 
	{
		flags = flags & ~flag;
	}
	else
	{
		flags = flags | flag;
	}
	
	status = Q3WinViewerSetFlags (gViewer, flags);

	(void) GetClientRect(gHwnd, (LPRECT)&aRect);
	(void) InvalidateRect(gHwnd, &aRect, FALSE);	

}

void InitFlagMenu ( void )
{
	HMENU hMenu;
	unsigned long flags;

	if( NULL == gViewer )
		return;

	flags = Q3WinViewerGetFlags (gViewer);
	hMenu = GetMenu( gHwnd );

	if (flags & kQ3ViewerControllerVisible) 
		CheckMenuItem( hMenu, IDM_CONTROLSTRIP, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_CONTROLSTRIP, MF_BYCOMMAND | MF_UNCHECKED );
	
	if (flags & kQ3ViewerButtonCamera) 
		CheckMenuItem( hMenu, IDM_CAMERABUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_CAMERABUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonTruck) 
		CheckMenuItem( hMenu, IDM_TRUCKBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_TRUCKBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonOrbit) 
		CheckMenuItem( hMenu, IDM_ORBITBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_ORBITBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonDolly) 
		CheckMenuItem( hMenu, IDM_DOLLYBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_DOLLYBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonZoom)
		CheckMenuItem( hMenu, IDM_ZOOMBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_ZOOMBUTTON, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerButtonReset) 
		CheckMenuItem( hMenu, IDM_RESETBUTTON, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_RESETBUTTON, MF_BYCOMMAND | MF_UNCHECKED );
	
	if (flags & kQ3ViewerShowBadge) 
		CheckMenuItem( hMenu, IDM_BADGE, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_BADGE, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerOutputTextMode) 
		CheckMenuItem( hMenu, IDM_TEXTMODE, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_TEXTMODE, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerDraggingInOff) 
		CheckMenuItem( hMenu, IDM_DRAGGINGINOFF, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_DRAGGINGINOFF, MF_BYCOMMAND | MF_UNCHECKED );

	if (flags & kQ3ViewerActive) 
		CheckMenuItem( hMenu, IDM_ACTIVE, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_ACTIVE, MF_BYCOMMAND | MF_UNCHECKED );


	flags = GetWindowLong (Q3WinViewerGetWindow (gViewer), GWL_STYLE);

	if (flags & WS_BORDER) 
		CheckMenuItem( hMenu, IDM_DRAWFRAME, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, IDM_DRAWFRAME, MF_BYCOMMAND | MF_UNCHECKED );
	
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
                    case IDM_ABOUT:
                        DialogBox(hInst,          
                                MAKEINTRESOURCE(IDD_ABOUTBOX),
                                hWnd,                 
                                (DLGPROC)About);
                        break;
					case IDM_NEW:
						CreateNewViewer ();
						break;
					case IDM_OPEN:
						(void) OpenModelFile();
						break;
					case IDM_SAVEAS:
						SaveModelFile();
						break;

                    case IDM_EXIT:
                        DestroyWindow (hWnd);
                        break;

					case IDM_UNDO:
						if (Q3WinViewerUndo (gViewer) == kQ3Failure)
							MessageBox (gHwnd, "Q3WinViewerUndo failed", "Edit Menu error", MB_OK);
						else
							(void) Q3WinViewerDrawContent (gViewer);
						break;
					case IDM_CUT:
						if (Q3WinViewerCut (gViewer) == kQ3Failure)
							MessageBox (gHwnd, "Q3WinViewerCut failed", "Edit Menu error", MB_OK);
						break;
					case IDM_COPY:
						if (Q3WinViewerCopy (gViewer) == kQ3Failure)
							MessageBox (gHwnd, "Q3WinViewerCopy failed", "Edit Menu error", MB_OK);
						break;
					case IDM_PASTE:
						if (Q3WinViewerPaste (gViewer) == kQ3Failure)
							MessageBox (gHwnd, "Q3WinViewerPaste failed", "Edit Menu error", MB_OK);
						break;
					case IDM_CLEAR:
						if (Q3WinViewerClear (gViewer) == kQ3Failure)
							MessageBox (gHwnd, "Q3WinViewerClear failed", "Edit Menu error", MB_OK);
						break;

					case IDM_CONTROLSTRIP:
						DoFlagCommand (kQ3ViewerControllerVisible);
						break;

					case IDM_CAMERABUTTON:
						DoFlagCommand (kQ3ViewerButtonCamera);
						break;

					case IDM_TRUCKBUTTON:
						DoFlagCommand (kQ3ViewerButtonTruck);
						break;

					case IDM_ORBITBUTTON:
						DoFlagCommand (kQ3ViewerButtonOrbit);
						break;

					case IDM_DOLLYBUTTON:
						DoFlagCommand (kQ3ViewerButtonDolly);
						break;

					case IDM_ZOOMBUTTON:
						DoFlagCommand (kQ3ViewerButtonZoom);
						break;

					case IDM_RESETBUTTON:
						DoFlagCommand (kQ3ViewerButtonReset);
						break;

					case IDM_BADGE:
						DoFlagCommand (kQ3ViewerShowBadge);
						break;

					case IDM_TEXTMODE:
						DoFlagCommand (kQ3ViewerOutputTextMode);
						break;

					case IDM_DRAWFRAME:
						{
							unsigned long flags = GetWindowLong (Q3WinViewerGetWindow (gViewer), GWL_STYLE);
							
							SetLastError (0);
							flags = SetWindowLong (Q3WinViewerGetWindow (gViewer), GWL_STYLE, flags ^ WS_BORDER);
					
							if (flags == 0 && (GetLastError() != 0))
								MessageBox (gHwnd, "Setting the WS_BORDER flags failed", "Flag menu error", MB_OK);
						}
						break;

					case IDM_DRAGGINGINOFF:
						DoFlagCommand (kQ3ViewerDraggingInOff);
						break;

					case IDM_ACTIVE:
						DoFlagCommand (kQ3ViewerActive);
						break;

					case IDM_TESTSETWINDOW:

						DoSetWindowTest ();
						break;

					case IDM_TESTMINDIMENSION:
						DoTestMinDimension ();
						break;

					case IDM_TESTGETBUTTONRECT:
						DoTestGetButtonRect ();
						break;

					case IDM_TESTSETCAMERABUTTON: /*Q3WinViewerSetCurrentButton tests */
					case IDM_TESTSETTRUCKBUTTON:
					case IDM_TESTSETROTATEBUTTON:
					case IDM_TESTSETZOOMBUTTON:
					case IDM_TESTSETDOLLYBUTTON:
						DoTestSetCurrentButton (wmId);
						break;

					case IDM_TESTGETBITMAP:
						DoTestGetBitmap ();
						break;

					case IDM_GETSETCOLOR:
						OurChooseColor ();
						break;

					case IDM_TESTWRITEDATA:
						DoTestWriteData ();
						break;

					default:
                        return (DefWindowProc(hWnd, message, uParam, lParam));
                }
                break;

			case WM_INITMENU:
				{
					unsigned long	state;
					HMENU			menu;

					InitFlagMenu();

					state = Q3WinViewerGetState (gViewer);

					menu = GetMenu (gHwnd);
					
					if (menu == NULL)
					{
						MessageBox (gHwnd, "GetMenu failed in WM_INITMENU", "WM_INITMENU error", MB_OK);
						return 0;
					}

					if (state & kQ3ViewerHasUndo)
					{
						unsigned long actualSize;
						char *string;

						if (Q3WinViewerGetUndoString (gViewer, NULL, 0, &actualSize) == kQ3False)
						{
							MessageBox (gHwnd, "Q3WinViewerGetUndoString failed in WM_INITMENU", "WM_INITMENU error", MB_OK);
							return 0;
						}

						string = malloc (actualSize);

						if (string == NULL)
						{
							MessageBox (gHwnd, "malloc failed in WM_INITMENU", "WM_INITMENU error", MB_OK);
							return 0;
						}

						if (Q3WinViewerGetUndoString (gViewer, string, actualSize, &actualSize) == kQ3False)
						{
							MessageBox (gHwnd, "Q3WinViewerGetUndoString failed in WM_INITMENU", "WM_INITMENU error", MB_OK);
							free (string);
							return 0;
						}

						if (ModifyMenu (menu, IDM_UNDO, MF_BYCOMMAND | MF_STRING, IDM_UNDO, string) == FALSE)
						{
							MessageBox (gHwnd, "ModifyMenu failed in WM_INITMENU", "WM_INITMENU error", MB_OK);
							free (string);
							return 0;
						}

						EnableMenuItem (menu, IDM_UNDO, MF_BYCOMMAND | MF_ENABLED);
						
						free (string);
					}
					else
					{
						BOOL val = EnableMenuItem (menu, IDM_UNDO, MF_BYCOMMAND |  MF_GRAYED);
						if (ModifyMenu (menu, IDM_UNDO, MF_BYCOMMAND | MF_STRING, IDM_UNDO, "Undo") == FALSE)
						{
							MessageBox (gHwnd, "ModifyMenu failed in WM_INITMENU", "WM_INITMENU error", MB_OK);
							return 0;
						}



					}
				}
				break;

			case WM_CREATE:
				{
				RECT aWinRect;
				GetClientRect(hWnd, (LPRECT)&aWinRect);

			//	InflateRect (&aWinRect, -50, -50);
				gViewer = Q3WinViewerNew (hWnd, &aWinRect, (unsigned long) kQ3ViewerDefault);

				if( gViewer == NULL )
					return -1;
				}
				break;

            case WM_DESTROY:  // message: window being destroyed
                PostQuitMessage(0);
                break;

			case WM_SIZE:
				{
				long width  = LOWORD(lParam);
                long height  = HIWORD(lParam);
				RECT rect;
				
				rect.top = 0;
				rect.left = 0;
				rect.right = width;
				rect.bottom = height;

				//InflateRect (&rect, -50, -50);
				Q3WinViewerSetBounds (gViewer, &rect);
				}
				break;

			case WM_SETFOCUS:
				SetFocus( Q3WinViewerGetWindow( gViewer ) );
				break;

			case WM_SYSCOLORCHANGE:
				SendMessage (Q3WinViewerGetWindow (gViewer), WM_SYSCOLORCHANGE, 0 , 0);
				break;

            default:          // Passes it on if unproccessed
                    return (DefWindowProc(hWnd, message, uParam, lParam));
        }
        return (0);
}


char ExtFilter[] = "QuickDraw 3D Metafiles\0*.3dmf;*.3dm;*.q3d;*.qd3d\0All Files\0*.*\0\0";

// put up common dialog; fOpen == TRUE for Open, FALSE for Save As.
BOOL	BrowseForPathName(char *inPathName, BOOLEAN fOpen) 
{
	OPENFILENAME aFileName;
	BOOL	 aStatus = TRUE;

	inPathName[0] = 0;
	aFileName.lStructSize = sizeof(OPENFILENAME);
	aFileName.hwndOwner = gHwnd;
	aFileName.hInstance = hInst;
	aFileName.lpstrFilter = ExtFilter;
    aFileName.lpstrCustomFilter = NULL; 
    aFileName.nMaxCustFilter = 0L; 
    aFileName.nFilterIndex = 0; 
    aFileName.lpstrFile = inPathName; 
    aFileName.nMaxFile = 255; 
    aFileName.lpstrFileTitle = NULL;	
    aFileName.nMaxFileTitle = 0; 
    aFileName.lpstrInitialDir = NULL;  
    aFileName.lpstrTitle = NULL;	
    aFileName.Flags = OFN_EXPLORER + OFN_LONGNAMES + OFN_PATHMUSTEXIST; 
	if( fOpen )
		aFileName.Flags += OFN_FILEMUSTEXIST;
    aFileName.nFileOffset = 0; 
    aFileName.nFileExtension = 0; 
    aFileName.lpstrDefExt = NULL; 
    aFileName.lCustData = 0; 
    aFileName.lpfnHook = NULL; 
    aFileName.lpTemplateName = NULL; 
	if (fOpen )
		if (GetOpenFileName((LPOPENFILENAME)&aFileName))
			aStatus = TRUE;
		else
			aStatus = FALSE;
	else //saving
		if (GetSaveFileName((LPOPENFILENAME)&aFileName))
			aStatus = TRUE;
		else
			aStatus = FALSE;

	return aStatus;
}

BOOL	OpenModelFile()
{
	char				*pathName;
	char				pathNameChars[255];
	TQ3Status			aStatus;
	RECT				aWinRect;
	HANDLE				hFile;

	pathName = &(pathNameChars[0]);
	if (BrowseForPathName(pathName, TRUE) != kQ3Success)
		return FALSE;

	hFile = CreateFile (pathName, GENERIC_READ, FILE_SHARE_READ, 
							   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == NULL)
		MessageBox (gHwnd, "CreateFile failed", "Yikes", MB_OK);
	else
	{
		aStatus = Q3WinViewerUseFile (gViewer, hFile);
		CloseHandle (hFile);
	}
			


	(void) GetClientRect(gHwnd, (LPRECT)&aWinRect);
	(void) InvalidateRect(gHwnd, &aWinRect, FALSE);	
	return TRUE;
}


BOOL CenterWindow (HWND hwndChild, HWND hwndParent)
{
        RECT    rChild, rParent;
        int     wChild, hChild, wParent, hParent;
        int     wScreen, hScreen, xNew, yNew;
        HDC     hdc;

        // Get the Height and Width of the child window
        GetWindowRect (hwndChild, &rChild);
        wChild = rChild.right - rChild.left;
        hChild = rChild.bottom - rChild.top;

        // Get the Height and Width of the parent window
        GetWindowRect (hwndParent, &rParent);
        wParent = rParent.right - rParent.left;
        hParent = rParent.bottom - rParent.top;

        // Get the display limits
        hdc = GetDC (hwndChild);
        wScreen = GetDeviceCaps (hdc, HORZRES);
        hScreen = GetDeviceCaps (hdc, VERTRES);
        ReleaseDC (hwndChild, hdc);

        // Calculate new X position, then adjust for screen
        xNew = rParent.left + ((wParent - wChild) /2);
        if (xNew < 0) {
                xNew = 0;
        } else if ((xNew+wChild) > wScreen) {
                xNew = wScreen - wChild;
        }

        // Calculate new Y position, then adjust for screen
        yNew = rParent.top  + ((hParent - hChild) /2);
        if (yNew < 0) {
                yNew = 0;
        } else if ((yNew+hChild) > hScreen) {
                yNew = hScreen - hChild;
        }

        // Set it, and return
        return SetWindowPos (hwndChild, NULL,
                xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


// About box callback
LRESULT CALLBACK About(
                HWND hDlg,           // window handle of the dialog box
                UINT message,        // type of message
                WPARAM uParam,       // message-specific information
                LPARAM lParam)
{
        static  HFONT hfontDlg;
        LPSTR   lpVersion;
        DWORD   dwVerInfoSize;
        DWORD   dwVerHnd;
        UINT    uVersionLen;
        WORD    wRootLen;
        BOOL    bRetCode;
        int     i;
        char    szFullPath[256];
        char    szResult[256];
        char    szGetName[256];

        switch (message) {
                case WM_INITDIALOG:  // message: initialize dialog box
                        // Center the dialog over the application window
                        CenterWindow (hDlg, GetWindow (hDlg, GW_OWNER));

                        // Get version information from the application
                        GetModuleFileName (hInst, szFullPath, sizeof(szFullPath));
                        dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
                        if (dwVerInfoSize) {
                                // If we were able to get the information, process it:
                                LPSTR   lpstrVffInfo;
                                HANDLE  hMem;
                                hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
                                lpstrVffInfo  = GlobalLock(hMem);
                                GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);
                                lstrcpy(szGetName, "\\StringFileInfo\\040904e4\\");
                                wRootLen = lstrlen(szGetName);

                                // Walk through the dialog items that we want to replace:
                                for (i = IDC_FILEDESCRIPTION; i <= IDC_LEGALTRADEMARKS; i++) {
                                        GetDlgItemText(hDlg, i, szResult, sizeof(szResult));
                                        szGetName[wRootLen] = (char)0;
                                        lstrcat (szGetName, szResult);
                                        uVersionLen   = 0;
                                        lpVersion     = NULL;
                                        bRetCode      =  VerQueryValue((LPVOID)lpstrVffInfo,
                                                (LPSTR)szGetName,
                                                (LPVOID)&lpVersion,
                                                (LPDWORD)&uVersionLen); // For MIPS strictness

                                        if ( bRetCode && uVersionLen && lpVersion) {
                                                // Replace dialog item text with version info
                                                lstrcpy(szResult, lpVersion);
                                                SetDlgItemText(hDlg, i, szResult);
                                        }
                                }

                                GlobalUnlock(hMem);
                                GlobalFree(hMem);
                        } // if (dwVerInfoSize)
                        return (TRUE);

                case WM_COMMAND:                      // message: received a command
                        if (LOWORD(uParam) == IDOK        // "OK" box selected?
                        || LOWORD(uParam) == IDCANCEL) {  // System menu close command?
                                EndDialog(hDlg, TRUE);        // Exit the dialog
                                DeleteObject (hfontDlg);
                                return (TRUE);
                        }
                        break;
        }
        return (FALSE); // Didn't process the message
}


void CreateNewViewer ()
{
	HWND window =  CreateWindowEx(
			WS_EX_WINDOWEDGE|WS_EX_CLIENTEDGE,	// extended window style
			szAppName,	// pointer to registered class name
			szTitle,	// pointer to window name
			WS_OVERLAPPED | WS_CAPTION |WS_THICKFRAME | WS_SYSMENU,	// window style
            25, 25, 400, 450, 	 // fixed size windows
			NULL,	// handle to parent or owner window
			NULL,	// handle to menu, or child-window identifier
			hInst,	// handle to application instance
			NULL 	// pointer to window-creation data
		);

    ShowWindow(window, SW_SHOW); 
    UpdateWindow(window);        


}

void	SaveModelFile(void)
{
	char				*pathName;
	char				pathNameChars[255];
	HANDLE				hFile;

	pathName = &(pathNameChars[0]);
	if (BrowseForPathName(pathName, FALSE) != kQ3Success)
		goto ExitSaveModelFile;

	
	hFile = CreateFile(
		pathName,	// pointer to name of the file 
		GENERIC_WRITE,	// access (read-write) mode 
		0,	// share mode 
		NULL,	// pointer to security descriptor 
		CREATE_NEW,	// how to create 
		FILE_ATTRIBUTE_NORMAL,	// file attributes 
		NULL 	// handle to file with attributes to copy  
	);

	if (hFile == NULL)
	{
		MessageBox (gHwnd, "CreateFile failed", "Save As Error", MB_OK);
		return;
	}

	if (Q3WinViewerWriteFile (gViewer, hFile) == kQ3Failure)
	{
		MessageBox (gHwnd, "Q3WinViewerWriteFile failec", "Save As Error", MB_OK);
	}
	CloseHandle( hFile );

ExitSaveModelFile:
	;
}

void OurChooseColor ()
{
	CHOOSECOLOR cc;
	TQ3ColorARGB color;
	char r, g, b;

	memset(&cc,0,sizeof( CHOOSECOLOR ) ); 
	cc.lStructSize = sizeof( CHOOSECOLOR ); 
	cc.hwndOwner = gHwnd; 
	cc.hInstance = hInst; 
	cc.lpCustColors = customColors;

	if (Q3WinViewerGetBackgroundColor (gViewer, &color) == kQ3Failure)
	{
		MessageBox (gHwnd, "Q3WinViewerGetBackgroundColor", "Choose color error", MB_OK);
		return;
	}

	r = (char ) (color.r * 255.0F) ;
	g = (char) (color.g * 255.0F);
	b = (char) (color.b * 255.0f);

	cc.rgbResult = RGB(r,g, b); 
	cc.Flags = CC_RGBINIT; 

	if( ChooseColor( &cc ) )
	{
		color.a = 1.0F;
		color.r = (float)GetRValue(cc.rgbResult)/(float)255;
		color.g = (float)GetGValue(cc.rgbResult)/(float)255;
		color.b = (float)GetBValue(cc.rgbResult)/(float)255;

		if (Q3WinViewerSetBackgroundColor (gViewer, &color) == kQ3Failure)
			MessageBox (gHwnd, "Q3WinViewerSetBackgroundColor", "Choose color error", MB_OK);
		else
			Q3WinViewerDrawContent (gViewer);
	}
	else
		MessageBox (gHwnd, "ChooseColor failed", "Choose color error", MB_OK);
}