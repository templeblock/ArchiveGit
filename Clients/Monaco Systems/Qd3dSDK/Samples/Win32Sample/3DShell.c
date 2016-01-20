#include <windows.h>		// required for all Windows applications
#include "resource.h"		// Windows resource IDs
#include "3dshell.h"		// specific to this program
#include <stdio.h>

#include "Box3DSupport.h"

HINSTANCE hInst;          	// current instance

DocumentPtr		gDocument = NULL;

char szAppName[] = "QuickDraw 3D Demo";		// The name of this application
char szTitle[]   = "QuickDraw 3D Demo";		// The title bar text
COLORREF customColors[16] = { 0L };			// for use by choosecolor dialog

typedef enum tStorageMethod
{
	kStorageUnixFile,
	kStorageUnixPath,
	kStorageWin32Handle,
	kStorageWin32MMFile
} tStorageMethod;

void		OpenModelFile( tStorageMethod );
void		SaveModelFile( tStorageMethod storageMethod, TQ3FileMode dstFileMode );

TQ3Status	BrowseForPathName(char *inPathName, BOOLEAN fOpen);

void		UpdateFrame(BOOLEAN step);

BOOL		InitDocumentData( DocumentPtr theDocument, HWND inWindow );
void		DisposeDocumentData( DocumentPtr theDocument);
void		DisposeRenderingData( DocumentPtr theDocument);
TQ3Status	DocumentDraw3DData( DocumentPtr theDocument );
int			Speedometer_ClockFrame( void );

TQ3Status	DocumentNewWindowSize(DocumentPtr theDocument, unsigned long width, unsigned long height);

TQ3Status	PaintDocumentWindow(DocumentPtr theDocument);

TQ3Status	GetToolBarPosition(DocumentPtr theDocument, unsigned long *x, unsigned long *y);
TQ3Status	GetToolBarSize(DocumentPtr theDocument, unsigned long *width, unsigned long *height);

void		InitQD3DMenu ( HWND hwnd );

void		MyErrorHandler(TQ3Error firstError, TQ3Error lastError, long refCon); 

#define REFERROR 00L
#define REFWARNING 01L
#define REFNOTICE 02L

static void StartTimer( void );
static void StopTimer( void );

VOID CALLBACK TimerProc( HWND, UINT, UINT, DWORD );	
UINT gTimer;
#define MILLSPERTICK 10  // milliseconds per timer tick


int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
        MSG msg;
        HANDLE hAccelTable;

		if (!InitApplication(hInstance)) 
		{
			return (FALSE);    
		}

        if (!InitInstance(hInstance, nCmdShow)) 
		{
			return (FALSE);
        }

        hAccelTable = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_GENERIC));

		StartTimer();
		while (GetMessage(&msg, NULL, 0, 0)) {
			if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		StopTimer();

        return (msg.wParam); // Returns the value from PostQuitMessage
}

void UpdateFrame(BOOLEAN step)
{
	TQ3Matrix4x4	tmp;
	RECT			aWinRect;
	BOOL			aResult;
	if( step )
	{
		Q3Matrix4x4_SetRotate_XYZ(&tmp, 0.05F, 0.05F, 0.05F);
		Q3Matrix4x4_Multiply(&gDocument->fRotation, &tmp, &gDocument->fRotation);
	}
	aResult = GetClientRect(gDocument->fWindow, (LPRECT)&aWinRect);
	aWinRect.bottom = aWinRect.top + gDocument->fHeight;
	aResult = InvalidateRect(gDocument->fWindow, &aWinRect, FALSE);	
}


BOOL InitApplication(HINSTANCE hInstance)
{
        WNDCLASS  wc;

        wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	/* CS_OWNDC is required to support the Win32DC Draw context */
        wc.lpfnWndProc   = (WNDPROC)WndProc;       
        wc.cbClsExtra    = 0;                      
        wc.cbWndExtra    = 0;                     
        wc.hInstance     = hInstance;             
        wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APP)); 
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wc.lpszMenuName  = MAKEINTRESOURCE(IDR_GENERIC); 
        wc.lpszClassName = szAppName;              

        return (RegisterClass(&wc));
}

BOOL InitInstance(
        HINSTANCE       hInstance,
        int             nCmdShow)
{
        HWND            hWnd; 

        hInst = hInstance; 
        hWnd = CreateWindowEx(
				WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,
                szAppName,          
                szTitle,             
                WS_OVERLAPPEDWINDOW, 
                25, 25, 640, 530, 	 // fixed size windows
                NULL,                
                NULL,                
                hInstance,           
                NULL                 
        );

        if (!hWnd) 
		{
            return (FALSE);
        }


        ShowWindow(hWnd, nCmdShow); 
        UpdateWindow(hWnd);        

        return (TRUE);             

}

static void StartTimer( void )
{
	if( gDocument && kQ3True == gDocument->spin)
		gTimer = SetTimer( NULL, 0, MILLSPERTICK, (TIMERPROC) TimerProc );
}

static void StopTimer( void )
{
	if( gDocument && kQ3True == gDocument->spin)
		KillTimer( NULL, gTimer );
}

VOID CALLBACK TimerProc(
    HWND hwnd,	// handle of window for timer messages 
    UINT uMsg,	// WM_TIMER message
    UINT idEvent,	// timer identifier
    DWORD dwTime 	// current system time
   )	
{
	if( kQ3True == gDocument->spin)
		UpdateFrame(TRUE);
}


LRESULT CALLBACK WndProc(
                HWND hWnd,        
                UINT message,      
                WPARAM uParam,     
                LPARAM lParam)   
{
        int wmId, wmEvent;
		TQ3Status	aStatus;
		PAINTSTRUCT PaintStruct;

        switch (message) 
		{
            case WM_COMMAND:

                wmId    = LOWORD(uParam);
                wmEvent = HIWORD(uParam);

                switch (wmId) 
				{
                    case IDM_ABOUT:
						StopTimer();
                        DialogBox(hInst,          
                                MAKEINTRESOURCE(IDD_ABOUTBOX),
                                hWnd,                 
                                (DLGPROC)About);
						StartTimer();
                        break;
					case ID_FILE_OPEN_WIN32:
						StopTimer();
						OpenModelFile(kStorageWin32Handle);
						StartTimer();
						break;
					case ID_FILE_OPEN_MMF:
						StopTimer();
						OpenModelFile(kStorageWin32MMFile);
						StartTimer();
						break;
					case ID_FILE_OPEN_UNIX:
						StopTimer();
						OpenModelFile(kStorageUnixFile);
						StartTimer();
						break;
					case ID_FILE_OPEN_PATH:
						StopTimer();
						OpenModelFile(kStorageUnixPath);
						StartTimer();
						break;
					case ID_FILE_SAVE_WIN32:
						StopTimer();
						SaveModelFile(kStorageWin32Handle, kQ3FileModeNormal );
						StartTimer();
						break;
					case ID_FILE_SAVE_WIN32_TEXT:
						StopTimer();
						SaveModelFile(kStorageWin32Handle, kQ3FileModeText );
						StartTimer();
						break;
					case ID_FILE_SAVE_UNIX:
						StopTimer();
						SaveModelFile(kStorageUnixFile, kQ3FileModeNormal );
						StartTimer();
						break;
					case ID_FILE_SAVE_UNIX_TEXT:
						StopTimer();
						SaveModelFile(kStorageUnixFile, kQ3FileModeText );
						StartTimer();
						break;
					case ID_FILE_SAVE_PATH:
						StopTimer();
						SaveModelFile(kStorageUnixPath, kQ3FileModeNormal );
						StartTimer();
						break;
					case ID_FILE_SAVE_PATH_TEXT:
						StopTimer();
						SaveModelFile(kStorageUnixPath, kQ3FileModeText );
						StartTimer();
						break;

                    case IDM_EXIT:
                        DestroyWindow (hWnd);
                        break;

                    case ID_QD3D_RENDERER_INTERACTIVE:
						if( kQ3RendererTypeInteractive != gDocument->rendererType)
						{
							gDocument->rendererType = kQ3RendererTypeInteractive;
							DisposeRenderingData( gDocument);
							InitDocumentData( gDocument, hWnd ); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;
                    
					case ID_QD3D_RENDERER_WIREFRAME:
						if( kQ3RendererTypeWireFrame != gDocument->rendererType)
						{
							gDocument->rendererType = kQ3RendererTypeWireFrame;
							DisposeRenderingData( gDocument);
							InitDocumentData( gDocument, hWnd ); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;
					
					case ID_QD3D_DRAWCONTEXT_WIN32DC:
						if( kQ3DrawContextTypeWin32DC != gDocument->drawcontextType)
						{
							gDocument->drawcontextType = kQ3DrawContextTypeWin32DC;
							DisposeRenderingData( gDocument);
							InitDocumentData( gDocument, hWnd ); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;

					case ID_QD3D_DRAWCONTEXT_PIXMAP16BIT555:
						if(!( kQ3DrawContextTypePixmap == gDocument->drawcontextType 
							&& kQ3PixelTypeRGB16 == gDocument->fPixelFormat ))
						{
							gDocument->drawcontextType = kQ3DrawContextTypePixmap;
							gDocument->fPixelFormat = kQ3PixelTypeRGB16;
							DisposeRenderingData( gDocument);
							InitDocumentData( gDocument, hWnd ); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;
					
					case ID_QD3D_DRAWCONTEXT_PIXMAP16BIT565:
						if(!( kQ3DrawContextTypePixmap == gDocument->drawcontextType 
							&& kQ3PixelTypeRGB16_565 == gDocument->fPixelFormat ))
						{
							gDocument->drawcontextType = kQ3DrawContextTypePixmap;
							gDocument->fPixelFormat = kQ3PixelTypeRGB16_565;
							DisposeRenderingData( gDocument);
							InitDocumentData( gDocument, hWnd ); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;

					case ID_QD3D_DRAWCONTEXT_PIXMAP24BIT:
						if( !(kQ3DrawContextTypePixmap == gDocument->drawcontextType 
							&& kQ3PixelTypeRGB24 == gDocument->fPixelFormat ))
						{
							gDocument->drawcontextType = kQ3DrawContextTypePixmap;
							gDocument->fPixelFormat = kQ3PixelTypeRGB24;
							DisposeRenderingData( gDocument);
							InitDocumentData( gDocument, hWnd ); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;
					
					case ID_QD3D_DRAWCONTEXT_PIXMAP32BIT:
						if( !(kQ3DrawContextTypePixmap == gDocument->drawcontextType 
							&& kQ3PixelTypeRGB32 == gDocument->fPixelFormat ) )
						{
							gDocument->drawcontextType = kQ3DrawContextTypePixmap;
							gDocument->fPixelFormat = kQ3PixelTypeRGB32;
							DisposeRenderingData( gDocument);
							InitDocumentData( gDocument, hWnd ); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;

					case ID_QD3D_SPIN:
						if( kQ3True == gDocument->spin)
						{
							gDocument->spin = kQ3False;
							StopTimer();
						}
						else
						{
							gDocument->spin = kQ3True;
							StartTimer();
						}
						break;
					
					case ID_QD3D_STEP:
						if( kQ3True == gDocument->spin)
						{
							gDocument->spin = kQ3False;
							StopTimer();
						}
						UpdateFrame(TRUE);	
						break;
					
					case ID_QD3D_CLEARCOLOR:
						{
						CHOOSECOLOR cc;
						memset(&cc,0,sizeof( CHOOSECOLOR ) ); 
						cc.lStructSize = sizeof( CHOOSECOLOR ); 
						cc.hwndOwner = hWnd; 
						cc.hInstance = hInst; 
						cc.lpCustColors = customColors;
						cc.rgbResult = RGB(gDocument->clearColorR,
							gDocument->clearColorG,
							gDocument->clearColorB); 
						cc.Flags = CC_RGBINIT; 

						if( ChooseColor( &cc ) )
						{
							TQ3DrawContextObject drawContext;
							TQ3ColorARGB q3Color;
							q3Color.a = 1.0F;
							gDocument->clearColorR = GetRValue(cc.rgbResult);
							q3Color.r = (float)gDocument->clearColorR/(float)255;
							gDocument->clearColorG = GetGValue(cc.rgbResult);
							q3Color.g = (float)gDocument->clearColorG/(float)255;
							gDocument->clearColorB = GetBValue(cc.rgbResult);
							q3Color.b = (float)gDocument->clearColorB/(float)255;
							if( gDocument->fView )
								if( Q3View_GetDrawContext(gDocument->fView, &drawContext) )
									Q3DrawContext_SetClearImageColor(drawContext, &q3Color); 
							UpdateFrame((BOOLEAN)(kQ3True == gDocument->spin));	
						}
						break;
						}
                    default:
                        return (DefWindowProc(hWnd, message, uParam, lParam));
                }
                break;

			case WM_CREATE:
				aStatus = Q3Initialize();
				if( aStatus == kQ3Failure )
					return -1;
				Q3Error_Register( MyErrorHandler, REFERROR );
				Q3Warning_Register( MyErrorHandler, REFWARNING );
				Q3Notice_Register( MyErrorHandler, REFNOTICE );
				gDocument = (DocumentPtr)malloc(sizeof(DocumentRec));
				memset( gDocument, 0, sizeof(DocumentRec) );

				/* these are the defaults */
				gDocument->rendererType = kQ3RendererTypeInteractive;
				gDocument->drawcontextType = kQ3DrawContextTypeWin32DC;
				gDocument->spin = kQ3True;
				gDocument->clearColorR = 255;
				gDocument->clearColorG = 255;
				gDocument->clearColorB = 255;

				if( InitDocumentData(gDocument, hWnd) == FALSE )
					return -1;
				break;

            case WM_DESTROY:  // message: window being destroyed
                PostQuitMessage(0);
				DisposeDocumentData(gDocument);
				if( gDocument )
					free(gDocument);
				gDocument = NULL;
				aStatus = Q3Exit();
                break;

			case WM_PAINT:
				BeginPaint(gDocument->fWindow, &PaintStruct);				
				DocumentDraw3DData(gDocument);
				EndPaint(gDocument->fWindow, &PaintStruct);
				break;

			case WM_INITMENU:
				InitQD3DMenu(hWnd);
				break;

			case WM_SIZE:
				{
				unsigned long width = LOWORD(lParam);  // width of client area 
				unsigned long height = HIWORD(lParam); // height of client area 

				DocumentNewWindowSize( gDocument, width, height );
				break;
				}
            default:          // Passes it on if unproccessed
                    return (DefWindowProc(hWnd, message, uParam, lParam));
        }
        return (0);
}

void InitQD3DMenu ( HWND hwnd )
{
	HMENU hMenu;
	if( NULL == gDocument )
		return;

	hMenu = GetMenu( hwnd );
	if (kQ3True == gDocument->spin) 
		CheckMenuItem( hMenu, ID_QD3D_SPIN, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, ID_QD3D_SPIN, MF_BYCOMMAND | MF_UNCHECKED );

	if (kQ3RendererTypeInteractive == gDocument->rendererType) 
		CheckMenuItem( hMenu, ID_QD3D_RENDERER_INTERACTIVE, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, ID_QD3D_RENDERER_INTERACTIVE, MF_BYCOMMAND | MF_UNCHECKED );
	
	if (kQ3RendererTypeWireFrame == gDocument->rendererType) 
		CheckMenuItem( hMenu, ID_QD3D_RENDERER_WIREFRAME, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, ID_QD3D_RENDERER_WIREFRAME, MF_BYCOMMAND | MF_UNCHECKED );

	if (kQ3DrawContextTypePixmap == gDocument->drawcontextType)
		switch( gDocument->fPixelFormat )
		{
			case kQ3PixelTypeRGB16:
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT555, MF_BYCOMMAND | MF_CHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT565, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP24BIT, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP32BIT, MF_BYCOMMAND | MF_UNCHECKED );
				break;
			case kQ3PixelTypeRGB16_565:
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT555, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT565, MF_BYCOMMAND | MF_CHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP24BIT, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP32BIT, MF_BYCOMMAND | MF_UNCHECKED );
				break;
			case kQ3PixelTypeRGB24:
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT555, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT565, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP24BIT, MF_BYCOMMAND | MF_CHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP32BIT, MF_BYCOMMAND | MF_UNCHECKED );
				break;
			case kQ3PixelTypeRGB32:
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT555, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT565, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP24BIT, MF_BYCOMMAND | MF_UNCHECKED );
				CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP32BIT, MF_BYCOMMAND | MF_CHECKED );
				break;
		}
	else
	{
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT555, MF_BYCOMMAND | MF_UNCHECKED );
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP16BIT565, MF_BYCOMMAND | MF_UNCHECKED );
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP24BIT, MF_BYCOMMAND | MF_UNCHECKED );
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_PIXMAP32BIT, MF_BYCOMMAND | MF_UNCHECKED );
	}

	if (kQ3DrawContextTypeWin32DC == gDocument->drawcontextType) 
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_WIN32DC, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_WIN32DC, MF_BYCOMMAND | MF_UNCHECKED );

	if (kQ3DrawContextTypeDDSurface == gDocument->drawcontextType) 
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_DIRECTDRAW, MF_BYCOMMAND | MF_CHECKED );
	else
		CheckMenuItem( hMenu, ID_QD3D_DRAWCONTEXT_DIRECTDRAW, MF_BYCOMMAND | MF_UNCHECKED );
	
}


void MyErrorHandler(TQ3Error firstError, TQ3Error lastError, long refCon) 
{ 
	char buf[512];
	switch( refCon )
	{
		case REFERROR:
			sprintf(buf, "QD3D ERROR %d\n", lastError); 
			break;
		case REFWARNING:
			sprintf(buf, "QD3D WARNING %d\n", lastError); 
			break;
		case REFNOTICE:
			sprintf(buf, "QD3D NOTICE %d\n", lastError); 
			break;
	}
	OutputDebugString(buf); 
}

char ExtFilter[] = "QuickDraw 3D Metafiles\0*.3dmf;*.3dm;*.q3d;*.qd3d\0All Files\0*.*\0\0";

// put up common dialog; fOpen == TRUE for Open, FALSE for Save As.
TQ3Status	BrowseForPathName(char *inPathName, BOOLEAN fOpen) 
{
	OPENFILENAME aFileName;
	TQ3Status	 aStatus = kQ3Success;

	inPathName[0] = 0;
	aFileName.lStructSize = sizeof(OPENFILENAME);
	aFileName.hwndOwner = gDocument->fWindow;
	aFileName.hInstance = NULL;
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
			aStatus = kQ3Success;
		else
			aStatus = kQ3Failure;
	else //saving
		if (GetSaveFileName((LPOPENFILENAME)&aFileName))
			aStatus = kQ3Success;
		else
			aStatus = kQ3Failure;

	return aStatus;
}

/* Note: OpenModelFile will use different storage classes based on storageMethod flag */
void		OpenModelFile( tStorageMethod storageMethod )
{
	#undef A3Assert
	#define	A3Assert(x)		if (!(x)) goto ExitOpenModelFile

	char				pathName[MAX_PATH];
	TQ3StorageObject	srcStorage = NULL;
	TQ3FileObject		srcFileObject = NULL;
	TQ3GroupObject		aGroup = NULL;
	TQ3Object			anObject = NULL;
	TQ3FileMode			srcFileMode;
	TQ3GroupPosition	aPosition;
	HANDLE				hFile;
	TQ3Status			aStatus;
	RECT				aWinRect;
	BOOL				aBoolResult;
	DWORD				dwFileSize;
	HANDLE				hFileMap;
	LPVOID				buffer;
	FILE				*aFile;


	if (BrowseForPathName(pathName, TRUE) != kQ3Success)
		goto ExitOpenModelFile;
	
	/* Open the file and create QD3D storage object */
	switch( storageMethod )
	{
	case kStorageWin32Handle:
	// Use Win32 Handle File Types 
		hFile = CreateFile(
			pathName,	// pointer to name of the file 
			GENERIC_READ,	// access (read-write) mode 
			0,	// share mode 
			NULL,	// pointer to security descriptor 
			OPEN_EXISTING,	// how to create 
			FILE_ATTRIBUTE_NORMAL,	// file attributes 
			NULL 	// handle to file with attributes to copy  
		);
		A3Assert(hFile != INVALID_HANDLE_VALUE);
		A3Assert((srcStorage = Q3Win32Storage_New(hFile)) != NULL);
		break;

	case kStorageWin32MMFile:
	// Use Win32's Memory Mapped File mechanism to read 3DMF file as 
	// QD3D Memory Buffer Storage Object
		hFile = CreateFile(
			pathName,	// pointer to name of the file 
			GENERIC_READ,	// access (read-write) mode 
			0,	// share mode 
			NULL,	// pointer to security descriptor 
			OPEN_EXISTING,	// how to create 
			FILE_ATTRIBUTE_NORMAL,	// file attributes 
			NULL 	// handle to file with attributes to copy  
		);
		A3Assert(hFile != INVALID_HANDLE_VALUE);
		// Create memory mapping of file
		dwFileSize = GetFileSize(hFile, NULL);
		// Create the file-mapping object. 
		hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL);
		if (hFileMap == NULL) 
		{
		  CloseHandle(hFile);
		  A3Assert(0);
		}
		// Get the address where the first byte of the file is mapped into memory.
		buffer = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (buffer == NULL) 
		{
		  CloseHandle(hFileMap);
		  CloseHandle(hFile);
		  A3Assert(0);
		}
		// Create QD3D Memory Buffer storage object
		A3Assert((srcStorage = Q3MemoryStorage_NewBuffer(buffer, dwFileSize, dwFileSize)) != NULL);
		break;

	case kStorageUnixFile:
		aFile = fopen(pathName, "rb");
		A3Assert((srcStorage = Q3UnixStorage_New(aFile)) != NULL);
		break;
	case kStorageUnixPath:
		A3Assert((srcStorage = Q3UnixPathStorage_New(pathName)) != NULL);
		break;
	}

	A3Assert((srcFileObject = Q3File_New()) != NULL);
	A3Assert((aStatus = Q3File_SetStorage(srcFileObject, srcStorage)) == kQ3Success);
	A3Assert((aStatus = Q3File_OpenRead(srcFileObject, &srcFileMode)) == kQ3Success);
	A3Assert((aGroup = Q3DisplayGroup_New()) != NULL);
	while (Q3File_IsEndOfFile(srcFileObject) == kQ3False)
	{
		A3Assert((anObject = Q3File_ReadObject(srcFileObject)) != NULL);
		A3Assert(Q3Error_Get(NULL) == kQ3ErrorNone);
		if (Q3Object_IsDrawable(anObject) == kQ3True)
			A3Assert((aPosition = Q3Group_AddObject(aGroup, anObject)) != NULL);
		A3Assert(Q3Object_Dispose(anObject) == kQ3Success);
		anObject = NULL;
	}
	A3Assert((Q3Object_Dispose(gDocument->fModel)) == kQ3Success);
	gDocument->fModel = aGroup;
	A3Assert((aStatus = Q3File_Close(srcFileObject)) == kQ3Success);
	
	/* Close the file system file as needed */
	switch( storageMethod )
	{
	case kStorageWin32Handle:
		CloseHandle( hFile );
		break;
	case kStorageWin32MMFile:
		UnmapViewOfFile( buffer );
		CloseHandle(hFileMap);
		CloseHandle( hFile );
		break;
	case kStorageUnixFile:
		fclose(aFile);
		break;
	case kStorageUnixPath:
		// Nothing extra needs to be done 
		break;
	}

	A3Assert((aStatus = Q3Object_Dispose(srcFileObject)) == kQ3Success);
	A3Assert((aStatus = Q3Object_Dispose(srcStorage)) == kQ3Success);
	pvCamera_Fit(gDocument);
	aBoolResult = GetClientRect(gDocument->fWindow, (LPRECT)&aWinRect);
	aBoolResult = InvalidateRect(gDocument->fWindow, &aWinRect, FALSE);	

ExitOpenModelFile:
	;
}


/* Note: SaveModelFile will use different storage classes based on storageMethod flag */
/* kStorageWin32MMFile is not supported */
void		SaveModelFile( tStorageMethod storageMethod, TQ3FileMode dstFileMode )
{
	#undef A3Assert
	#define	A3Assert(x)		if (!(x)) goto ExitSaveModelFile

	char				pathName[MAX_PATH];
	TQ3FileObject		dstFileObject = NULL;
	TQ3StorageObject	dstStorage = NULL;
	TQ3ViewObject		dstView;
	TQ3Status			aStatus;
	TQ3ViewStatus		aViewStatus;
	HANDLE				hFile;
	FILE				*aFile;

	if (BrowseForPathName(pathName, FALSE) != kQ3Success)
		goto ExitSaveModelFile;
	
	/* Open the file and create QD3D storage object */
	switch( storageMethod )
	{
	case kStorageWin32Handle:
	// Use Win32 Handle File Types 
		hFile = CreateFile(
			pathName,	// pointer to name of the file 
			GENERIC_WRITE,	// access (read-write) mode 
			0,	// share mode 
			NULL,	// pointer to security descriptor 
			CREATE_NEW,	// how to create 
			FILE_ATTRIBUTE_NORMAL,	// file attributes 
			NULL 	// handle to file with attributes to copy  
		);
		A3Assert(hFile != INVALID_HANDLE_VALUE);
		A3Assert((dstStorage = Q3Win32Storage_New(hFile)) != NULL);
		break;

	case kStorageWin32MMFile:
		A3Assert(0);	// not supported in this sample
		break;

	case kStorageUnixFile:
		if( dstFileMode == kQ3FileModeText )
			aFile = fopen(pathName, "wt"); 
		else
			aFile = fopen(pathName, "wb"); 
		A3Assert((dstStorage = Q3UnixStorage_New(aFile)) != NULL);
		break;

	case kStorageUnixPath:
		A3Assert((dstStorage = Q3UnixPathStorage_New(pathName)) != NULL);
		break;
	}


	A3Assert((dstFileObject = Q3File_New()) != NULL);
	A3Assert((aStatus = Q3File_SetStorage(dstFileObject, dstStorage)) == kQ3Success);
	A3Assert((dstView = Q3View_New()) != NULL);
	A3Assert((aStatus = Q3File_OpenWrite(dstFileObject, dstFileMode)) == kQ3Success);
	A3Assert((aStatus = Q3View_StartWriting(dstView, dstFileObject)) == kQ3Success);
	do
	{
		A3Assert((aStatus = Q3Object_Submit(gDocument->fModel, dstView)) == kQ3Success);
		A3Assert((aViewStatus = Q3View_EndWriting(dstView)) != kQ3ViewStatusError);
	} while (aViewStatus == kQ3ViewStatusRetraverse);
	A3Assert((aStatus = Q3File_Close(dstFileObject)) == kQ3Success);

	/* Close the file system file as needed */
	switch( storageMethod )
	{
	case kStorageWin32Handle:
		CloseHandle( hFile );
		break;
	case kStorageWin32MMFile:
		A3Assert(0); //unsupported in this sample
		break;
	case kStorageUnixFile:
		fclose(aFile);
		break;
	case kStorageUnixPath:
		// Nothing extra needs to be done 
		break;
	}

	A3Assert((aStatus = Q3Object_Dispose(dstFileObject)) == kQ3Success);
	A3Assert((aStatus = Q3Object_Dispose(dstStorage)) == kQ3Success);

ExitSaveModelFile:
	;
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


BOOL InitDocumentData( DocumentPtr theDocument, HWND inWindow ) 
{
	RECT				aWinRect;
	
	theDocument->fWindow = inWindow;
	(void) GetClientRect(inWindow, (LPRECT)&aWinRect);
	theDocument->fWidth = aWinRect.right - aWinRect.left;
	theDocument->fHeight = aWinRect.bottom - aWinRect.top;

	// sets up the 3d data for the scene
	// Create view for QuickDraw 3D
	theDocument->fView = MyNewView( theDocument ) ;
	if( NULL == theDocument->fView )
		return FALSE;

	// the main display group
	if( NULL == theDocument->fModel )
		theDocument->fModel = MyNewModel() ;
	if( NULL == theDocument->fModel )
		return FALSE;
	
	pvCamera_Fit(theDocument);

	// the drawing styles:
	theDocument->fInterpolation = Q3InterpolationStyle_New(kQ3InterpolationStyleVertex) ;
	if( NULL == theDocument->fInterpolation )
		return FALSE;

	theDocument->fBackFacing = Q3BackfacingStyle_New(kQ3BackfacingStyleRemove ) ;
	if( NULL == theDocument->fBackFacing )
		return FALSE;
	
	theDocument->fFillStyle = Q3FillStyle_New(kQ3FillStyleFilled ) ;
	if( NULL == theDocument->fFillStyle )
		return FALSE;

	theDocument->fIllumination = Q3PhongIllumination_New();
	if( NULL == theDocument->fIllumination )
		return FALSE;

	// set the rotation matrix the identity matrix
	Q3Matrix4x4_SetIdentity(&theDocument->fRotation);

	return TRUE;
}

void DisposeDocumentData( DocumentPtr theDocument)
{
	if( theDocument == NULL )
		return;
	DisposeRenderingData( theDocument);
	if( theDocument->fModel )
	{
		Q3Object_Dispose(theDocument->fModel) ;				// object in the scene being modelled
		theDocument->fModel = NULL;
	}
}

void DisposeRenderingData( DocumentPtr theDocument)
{	
	if( theDocument == NULL )
		return;
	if( theDocument->fView )
		Q3Object_Dispose(theDocument->fView) ;				// the view for the scene
	if( kQ3DrawContextTypePixmap == theDocument->drawcontextType)
	{
		if( NULL != theDocument->fBitmap )
			DeleteObject(theDocument->fBitmap);
		if( NULL != theDocument->fMemoryDC ) 
			DeleteDC(theDocument->fMemoryDC);
	}
	if( theDocument->fInterpolation )
		Q3Object_Dispose(theDocument->fInterpolation) ;		// interpolation style used when rendering
	if( theDocument->fBackFacing )
		Q3Object_Dispose(theDocument->fBackFacing) ;		// whether to draw shapes that face away from the camera
	if( theDocument->fFillStyle )
		Q3Object_Dispose(theDocument->fFillStyle) ;			// whether drawn as solid filled object or decomposed to components
	if( theDocument->fIllumination )
		Q3Object_Dispose(theDocument->fIllumination) ;
}

//-----------------------------------------------------------------------------
// 

TQ3Status DocumentDraw3DData( DocumentPtr theDocument )
{
	int fps;
	if( kQ3Success == Q3View_StartRendering(theDocument->fView ) )	
		do {	
			Q3Shader_Submit( theDocument->fIllumination, theDocument->fView );	
			Q3Style_Submit( theDocument->fInterpolation, theDocument->fView );	
			Q3Style_Submit( theDocument->fBackFacing, theDocument->fView );	
			Q3Style_Submit( theDocument->fFillStyle, theDocument->fView );	
			Q3MatrixTransform_Submit( &theDocument->fRotation, theDocument->fView );	
			Q3DisplayGroup_Submit( theDocument->fModel, theDocument->fView );	
		} while (Q3View_EndRendering(theDocument->fView) == kQ3ViewStatusRetraverse );	

	/* must manually blit from the pixmap draw context */
	if( kQ3DrawContextTypePixmap == theDocument->drawcontextType)
	{
	RECT			aWinRect;
	BOOL			aResult;
	HDC				hdc;
		aResult = GetClientRect(theDocument->fWindow, (LPRECT)&aWinRect);
		hdc = GetDC(theDocument->fWindow);
		BitBlt(hdc, 0, 0, theDocument->fWidth, theDocument->fHeight,	
				theDocument->fMemoryDC, 0, 0, SRCCOPY);					
		GdiFlush();	
		ReleaseDC(theDocument->fWindow, hdc);				
	}
	
	fps = Speedometer_ClockFrame();

	/* display frame rate */
	if ( GetAsyncKeyState( VK_SHIFT ) )
	{
		HDC				hdc;
		char string[25];
		sprintf( string, "fps: %i", fps );
		hdc = GetDC(theDocument->fWindow);
		TextOut( hdc, 0, 0, string, strlen(string) ); 
	}

	return kQ3Success ;
}

/* Speedometer_ClockFrame
 * call this routine once per frame, it will return an averaged frames per second value.

 */
int Speedometer_ClockFrame( void )
{
#define NUMDELTAS 25
	DWORD	timeNow;
	DWORD	aveDelta = 0;
	static DWORD timePrevious = 0;
	static DWORD deltas[NUMDELTAS] = { 0 };
	static int nextDelta = 0;
	int fps, i;


	if( 0 == timePrevious  )
		timePrevious = GetTickCount();
	timeNow =  GetTickCount();
	deltas[nextDelta] = (timeNow-timePrevious);
	
	nextDelta++;
	if( nextDelta >= NUMDELTAS )
		nextDelta = 0;
	
	for( i = 0; i < NUMDELTAS; i++ )
		aveDelta += deltas[i];
	aveDelta /= NUMDELTAS;
	
	if( 0 == aveDelta  )
		aveDelta = 1;

	/* calculate frames per second */
	fps = 1000/aveDelta;
	
	timePrevious = timeNow;
	
	return fps;
}

TQ3Status DocumentNewWindowSize(DocumentPtr theDocument, unsigned long width, unsigned long height)
{
	TQ3Status	aStatus;

	aStatus = MyResizeView( theDocument, width, height);

	pvCamera_Fit(gDocument);
	
	return aStatus;
}

