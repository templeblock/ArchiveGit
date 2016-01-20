/*®PL1¯®FD1¯®BT0¯®TP0¯*/
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"

#define COLOR_PATCH_CLASS 1
#define RIBBON_ICON_CLASS 2
//#define CS_SCOPE CS_GLOBALCLASS
#define CS_SCOPE 0

/***********************************************************************/
BOOL RegisterAllClasses( HINSTANCE hInstance )
/***********************************************************************/
{
WNDCLASS WndClass;

// subclass the standard dialog class
if ( GetClassInfo( NULL, WC_DIALOG, &WndClass ) )
	{
	WndClass.lpszClassName	= (LPSTR)"ppdialog";
	WndClass.hbrBackground	= ( HBRUSH )(COLOR_BTNFACE+1);
	WndClass.style &= ~((UINT)(CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW));
	WndClass.hInstance = hInstance;
	if (!PictPubApp.MyRegisterClass( &WndClass )) 
		return( FALSE );
	}

// subclass the standard dialog class
if ( GetClassInfo( NULL, WC_DIALOG, &WndClass ) )
	{
	WndClass.lpfnWndProc = (WNDPROC)RibbonDlgProc;
	WndClass.lpszClassName	= (LPSTR)"ppribbon";
	WndClass.hbrBackground	= ( HBRUSH )(COLOR_BTNFACE+1);
	WndClass.style &= ~((UINT)(CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW));
	WndClass.hInstance = hInstance;
	if (!RegisterClass( &WndClass ))
		return( FALSE );
	}

/* Setup and define a window class for the Zoom windows */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"ppzoom";
WndClass.hCursor	= LoadCursor( PictPubApp.GetResourceHandle(),
    MAKEINTRESOURCE(ID_CUSTOMVIEW));
WndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOWTEXT+1); //GetStockObject(LTGRAY_BRUSH);
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE; // | CS_BYTEALIGNCLIENT;
WndClass.lpfnWndProc	= (WNDPROC)WndZoomBoxProc;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= WNDEXTRA_ZOOMBOX;

if (!PictPubApp.MyRegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a static path */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"path";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Path_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= PATH_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a lever */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"lever";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Lever_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= LEVER_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for 3 dimensional icons */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"icon";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Icon_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= ICON_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class labeled frame control */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"labeledframe";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= LabeledFrame_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= LABELEDFRAME_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for visual calibration*/
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"visualframe";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= VisualFrame_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= VISUALFRAME_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for 3 dimensional icons */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"ricon";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Icon_Control;
WndClass.cbClsExtra	= RIBBON_ICON_CLASS;
WndClass.cbWndExtra	= ICON_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for text blocks */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"block";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= TextBlock_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= TEXTBLOCK_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for panel control */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"panel";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Panel_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= PANEL_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a hue control */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"hue";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Hue_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= HUE_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a color picker control */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"picker";
WndClass.hCursor	= LoadCursor( PictPubApp.GetResourceHandle(), 
    MAKEINTRESOURCE(ID_SELECTOR));
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Picker_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= PICKER_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"color";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Color_Control;
WndClass.cbClsExtra	= COLOR_PATCH_CLASS;
WndClass.cbWndExtra	= COLOR_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"color2";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Color2_Control;
WndClass.cbClsExtra	= COLOR_PATCH_CLASS;
WndClass.cbWndExtra	= COLOR2_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"ruler";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
WndClass.hInstance	= hInstance;
WndClass.style		= CS_SCOPE;
WndClass.lpfnWndProc	= Ruler_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= RULER_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"palette";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_SCOPE;
WndClass.lpfnWndProc	= Palette_Control;
WndClass.cbClsExtra	= COLOR_PATCH_CLASS;
WndClass.cbWndExtra	= PALETTE_EXTRA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"image";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_SCOPE;
WndClass.lpfnWndProc	= Image_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= IMAGE_EXTRATOOLDATA;

if (!RegisterClass( &WndClass ))
	return( FALSE );

return( TRUE );
}
