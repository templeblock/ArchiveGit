/*®PL1¯®FD1¯®BT0¯®TP0¯*/
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

//#define CS_SCOPE CS_GLOBALCLASS
#define CS_SCOPE 0

static WNDPROC lpEditProc;

extern HINSTANCE hInstAstral;

/***********************************************************************/
HANDLE WINPROC EXPORT CreateEditControl( LPCSTR lpClass, LPCSTR lpLabel,
	DWORD dwStyle, int X,int Y,int W,int H, HWND hParent,HMENU hMenu,
	LPVOID lpData)
/***********************************************************************/
{
HWND hResult;
HGLOBAL hMemory;

if ( !(hMemory = GlobalAlloc( GMEM_MOVEABLE|GMEM_ZEROINIT|GMEM_SHARE, 256L )) )
	hMemory = hInstAstral;

hResult = CreateWindow(lpClass, lpLabel, dwStyle, X, Y, W, H, hParent, hMenu,
			(HINSTANCE)hMemory, lpData);
SendMessage(hResult, EM_LIMITTEXT, 0, 0L);
return(hResult);
}


/***********************************************************************/
long WINPROC EXPORT EditControl( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
LRESULT lResult;
HINSTANCE hMemory;

lResult = CallWindowProc( (WNDPROC)lpEditProc, hWindow, message, wParam, lParam );
if (message == WM_NCDESTROY)
	{
	hMemory = (HINSTANCE)GET_WINDOW_INSTANCE(hWindow);
	if (hMemory != hInstAstral)
		{
		GlobalUnlock((HGLOBAL)hMemory);
		GlobalFree((HGLOBAL)hMemory);
		}
	}

return(lResult);
}


/***********************************************************************/
BOOL RegisterAllClasses( HINSTANCE hInstance )
/***********************************************************************/
{
WNDCLASS WndClass;
FARPROC lpProc;

/* Setup and define the window class for an edit control */
clr( (LPTR)&WndClass, sizeof(WndClass) );
if ( GetClassInfo( NULL, "edit", &WndClass ) )
	{
	lpEditProc = (WNDPROC)WndClass.lpfnWndProc;
	lpProc = MakeProcInstance((FARPROC)EditControl, hInstance);
	WndClass.lpfnWndProc = (WNDPROC)lpProc;
	WndClass.lpszClassName = (LPSTR)"cedit";
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.hInstance = hInstance;
	if (!RegisterClass( &WndClass ))
		return( FALSE );
	}

// subclass the standard dialog class
//if ( GetClassInfo( NULL, WC_DIALOG, &WndClass ) )
//	{
//	WndClass.lpszClassName	= (LPSTR)"graydialog";
//	WndClass.hbrBackground	= ( HBRUSH )GetStockObject(LTGRAY_BRUSH);
//	WndClass.style &= ~((UINT)(CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW));
//	WndClass.hInstance = hInstance;
//	if (!RegisterClass( &WndClass ))
//		return( FALSE );
//	}

/* Setup and define the window class for 3 dimensional icons */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"icon";
WndClass.hCursor	= LoadCursor( hInstance, MAKEINTRESOURCE(ID_HAND02) );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_GLOBALCLASS | CS_DBLCLKS | CS_SCOPE;
WndClass.lpfnWndProc	= IconControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 12;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for text blocks */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"block";
WndClass.hCursor	= LoadCursor( hInstance, MAKEINTRESOURCE(ID_HAND02) );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_GLOBALCLASS | CS_DBLCLKS | CS_SCOPE;
WndClass.lpfnWndProc	= TextBlockControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 8;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for a transparent bitmap */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"toy";
WndClass.hCursor	= LoadCursor( hInstance, MAKEINTRESOURCE(ID_HAND02) );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_GLOBALCLASS | CS_DBLCLKS | CS_SCOPE;
WndClass.lpfnWndProc	= ToyControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 14;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"color2";
WndClass.hCursor	= LoadCursor( hInstance, MAKEINTRESOURCE(ID_HAND02) );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_GLOBALCLASS | CS_DBLCLKS | CS_SCOPE;
WndClass.lpfnWndProc	= Color2Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 14;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"spin";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_GLOBALCLASS | CS_SCOPE; /* NO CS_DBLCLKS */
WndClass.lpfnWndProc	= SpinControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 26;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"ruler";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= (HBRUSH)((long)COLOR_BTNFACE)+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_SCOPE;
WndClass.lpfnWndProc	= (WNDPROC)RulerControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 10;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"image";
WndClass.hCursor	= LoadCursor( hInstance, MAKEINTRESOURCE(ID_HAND02) );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_GLOBALCLASS|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_SCOPE;
WndClass.lpfnWndProc	= ImageControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 20;

if (!RegisterClass( &WndClass ))
	return( FALSE );

if (! Slider_RegisterClass (hInstAstral, CS_SCOPE))
	return (FALSE);

if (! Radio_RegisterClass (hInstAstral, CS_SCOPE))
	return (FALSE);

if (! Check_RegisterClass (hInstAstral, CS_SCOPE))
	return (FALSE);

return( TRUE );
}

