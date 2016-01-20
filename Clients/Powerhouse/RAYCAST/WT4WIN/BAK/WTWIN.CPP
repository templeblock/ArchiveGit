////////////////////////////////////////////////////////////////////////////
// WTWIN.CPP
//
// Shell to drive the WT 3D Engine.  Shamelessly hacked from Microsoft
// WinG DOGGIE.CPP program - but all that remains is the generic boilerplate
// WinG windows shell.  Albiet a good one, one that seamlessly ports between
// 16 and 32 bits.  Thanks Microsoft!  I'm sure they don't mind...after all
// this is a cool win32 WinG demo!
//
// 7/12/94 Chris Thomas (GCT)
//
// 7/25/94 GCT - cleaned up code a bit, still has massive TAB alignment
//		problems.  
////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>		//for timer services.
#include <commdlg.h>
#include "wing.h"
#include "wingdll.h"
#include "wtwin.h"

#if defined(WIN32)
#define _export /*_export*/
#endif

#define DEFAULT_WORLD_FILEPATH  "worlds\\newcastl.wor"
#define DEFAULT_WORLD_FILETITLE "newcastl.wor"

extern "C" {
#include "error.h"
#include "dos-keyb.h"		//for the funky scancode enum, until I get 
									//  a better scheme in place.
extern unsigned char kbPressed[128];

//from wt.c
extern int wt_init(char *pszWorldFile, int w, int h);
extern int wt_render(void);
extern int wt_input(void);
extern void wt_term(void);
extern void wt_reinit(int width,int height);
extern int wt_new_world(char *szFileTitle);   
extern void wt_set_path(char *szModulePath);
extern void wt_reset_input(void);
extern void wt_reset_view(void);
extern void wt_get_player_info(float *,float *,float *,float *);

//from wingraph.c
extern void wt_set_fb_mem(void *pMem);
extern void wt_get_palette_entry(int i, int *r, int *g, int *b);
extern int wt_load_palette(void);

//from error.c
extern void wt_setlink(HWND hwnd, WORD msg);
}

//from winpal.cpp (really a hacked PALANIM sample.c)
extern void ClearSystemPalette(void);
extern HPALETTE CreateIdentityPalette(RGBQUAD aRGB[], int nColors);
extern void AppActivate(BOOL fActive);

void CreateWTPalette(void);				//at end of this module.

/*----------------------------------------------------------------------------*\
|                                                                              |
|   g l o b a l   v a r i a b l e s                                            |
|                                                                              |
\*----------------------------------------------------------------------------*/
static	char	szAppName[]="WT 3D Engine";
static	char	szAppBaseWindowTitle[]="WT";

static  HINSTANCE hInstApp;
static  HWND      hwndApp;
static  HPALETTE  hpalApp;
static  BOOL	   bAppActive;
static  WORD 		msgApp;

WinGdll WinG;

typedef struct header
{
	BITMAPINFOHEADER Header;
	RGBQUAD aColors[256];
} header;

header BufferHeader;
long Orientation = 0;		//assume Bottom-up DIBs, in which case we crash (!)
void *pBuffer = 0;
HDC Buffer = 0;

BOOL bShowRendering;
BOOL bWTinitialized = FALSE;
RGBQUAD ColorTable[256];

typedef struct pal
{
	WORD Version;
	WORD NumberOfEntries;
	PALETTEENTRY aEntries[256];
} pal;

pal LogicalPalette =
{
	0x300,
	256
};

HBITMAP gbmOldMonoBitmap = 0;

int StretchFactor = 1;
int nBitmapW, nBitmapH;			//WinG bitmap dimensions.

/* new variables for common dialogs */

OPENFILENAME ofn;
char szFilterSpec [128] =                       /* file type filters */
             "World Files (*.WOR)\0*.WOR\0All Files (*.*)\0*.*\0";

char szFileName[MAXFILENAME];
char szFileTitle[MAXFILENAME];

//current paths.
char szModulePath[MAXFILENAME];        //has trailing slash.
char szDefaultWorldFileName[MAXFILENAME];

/*----------------------------------------------------------------------------*\
|                                                                              |
|   f u n c t i o n   d e f i n i t i o n s                                    |
|                                                                              |
\*----------------------------------------------------------------------------*/

LONG FAR PASCAL _export AppWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
LONG AppCommand (HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

void AppExit(void);
BOOL AppIdle(void);

/*----------------------------------------------------------------------------*\
|   AppAbout( hDlg, uiMessage, wParam, lParam )                                |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the "About" dialog box.    |
|       The only message that it looks for is WM_COMMAND, indicating the use   |
|       has pressed the "OK" button.  When this happens, it takes down         |
|       the dialog box.                                                        |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of about dialog window                   |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if message has been processed, else FALSE                         |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL FAR PASCAL _export AppAbout(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
        case WM_COMMAND:
	    if (LOWORD(wParam) == IDOK)
            {
                EndDialog(hwnd,TRUE);
            }
            break;

        case WM_INITDIALOG:
            return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*\
|   AppInit( hInst, hPrev)                                                     |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|       memory.  It performs all initialization that doesn't need to be done   |
|       once per instance.                                                     |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of current instance                    |
|       hPrev           instance handle of previous instance                   |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL AppInit(HINSTANCE hInst,HINSTANCE hPrev,int sw,LPSTR szCmdLine)
{
    WNDCLASS cls;

    /* Save instance handle for DialogBoxs */
    hInstApp = hInst;

    if (!hPrev)
    {
        /*
         *  Register a class for the main application window
         */
        cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon          = LoadIcon(hInst,"AppIcon");
        cls.lpszMenuName   = "AppMenu";
        cls.lpszClassName  = szAppName;
        cls.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        cls.hInstance      = hInst;
        cls.style          = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
        cls.lpfnWndProc    = (WNDPROC)AppWndProc;
        cls.cbWndExtra     = 0;
        cls.cbClsExtra     = 0;

        if (!RegisterClass(&cls))
            return FALSE;
    }

   nBitmapW = 320;			//default size (window client size to use).
   nBitmapH = 200;

	//work backwards from client size to desired window size.  doesn't have
	//to be super accurate, approximate will do.

	int windx, windy;

	windx = windy = GetSystemMetrics(SM_CXFRAME)*2;
	windx += nBitmapW;	
	windy += nBitmapH + GetSystemMetrics(SM_CYCAPTION) - 1;
	windy += GetSystemMetrics(SM_CYMENU);

   hwndApp = CreateWindow (szAppName,    // Class name
                           szAppName,              // Caption
                           WS_OVERLAPPEDWINDOW,    // Style bits
                           CW_USEDEFAULT, 0,       // Position
		                     windx,windy,		    // Size
                           (HWND)NULL,             // Parent window (no parent)
                           (HMENU)NULL,            // use class menu
                           hInst,                  // handle to window instance
                           (LPSTR)NULL             // no params to pass on
                           );
    ShowWindow(hwndApp,sw);

		//Get the path for this EXE, and pass it to the WT subsystem.  This
		//is so textures will be loaded from the texture subdirectory.

    int nLen = GetModuleFileName(NULL, szModulePath, MAXFILENAME);
    if (nLen <= 0)
       wt_set_path("");
    else {
       //find the last slash in the path, starting from last char.  this is
       //so we can ditch the .EXE filename (but leave the slash).
       for ( nLen--; nLen>=0; --nLen)
         if (szModulePath[nLen] == '\\') {
            szModulePath[nLen+1] = '\0';     //unicode problems?
            break;
         }
       wt_set_path(szModulePath);		//give WT the path.
    }

	 StretchFactor = 1;						//we'll start with a 1:1 stretch.
    HMENU Menu = GetMenu(hwndApp);
    CheckMenuItem(Menu,ID_STUFF_1X,MF_CHECKED);

	 bShowRendering = FALSE;

    //If you want to see renderings in progress, then comment out this
    //call to EnableMenuItem(), and uncomment the calls to UpdateWT() 
    //throughout render.c.
    EnableMenuItem(Menu,ID_STUFF_SHOW_RENDERING,MF_GRAYED);

	//get a private Windows message for this App.  This is used so the WT C-
	//code, specifically routines in error.c, could SendMessage() a request
	//to update the screen (ie, blit the image).
	msgApp = RegisterWindowMessage(szAppName);
	wt_setlink(hwndApp, msgApp);

    return TRUE;
}


/*----------------------------------------------------------------------------*\
|   AppExit()								       |
|                                                                              |
|   Description:                                                               |
|	app is just about to exit, cleanup				       |
|                                                                              |
\*----------------------------------------------------------------------------*/
void AppExit()
{
   if (bWTinitialized) {
      wt_term();				
      bWTinitialized = FALSE;
   }

	if (Buffer)
	{
		HBITMAP hbm;

		//  Retrieve the current WinGBitmap, replace with the original
		hbm = (HBITMAP)SelectObject(Buffer, gbmOldMonoBitmap);

		//  And delete the WinGBitmap and WinGDC
		DeleteObject(hbm);
		DeleteDC(Buffer);
	}

	if(hpalApp)
	{
		DeleteObject(hpalApp);
	}
}

/*----------------------------------------------------------------------------*\
|   WinMain( hInst, hPrev, lpszCmdLine, cmdShow )                              |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInst           instance handle of this instance of the app            |
|       hPrev           instance handle of previous instance, NULL if first    |
|       szCmdLine       ->null-terminated command line                         |
|       cmdShow         specifies how the window is initially displayed        |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    MSG     msg;

    if (!WinG.Load()) {				//link to WinG DLL.
		   MessageBox(0,"Can't find WING32.DLL","WTWIN Error!",MB_OK);
		   return FALSE;
    }

    /* Call initialization procedure */
    if (!AppInit(hInst,hPrev,sw,szCmdLine))
			return FALSE;

    /*
     * Polling messages from event queue
     */
    for (;;)
    {
        if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
		 {
	    if (AppIdle())
                WaitMessage();
        }
    }

    AppExit();
    WinG.Free();						//terminate WinG DLL link.
    return msg.wParam;
}

////////////////////////////////////////////////////////////////////////////
// AppPaint(hwnd, hdc)                                                        |
//
// Simply blits the WT bitmap to the screen.
//
AppPaint (HWND hwnd, HDC hdc)
{
	if (StretchFactor != 1)
		WinG.pStretchBlt(hdc,0,0,StretchFactor*nBitmapW,StretchFactor*nBitmapH,
							Buffer,0,0,nBitmapW,nBitmapH);
	else
		WinG.pBitBlt(hdc,0,0,nBitmapW,nBitmapH,Buffer,0,0);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// AppCycle().  Do one iteration of the WT cycle.
//
void AppDoCycle(void)
{
	wt_input();
	wt_render();

	HDC hdc = GetDC(hwndApp);
	//don't bother to realize the palette, because we've got AppActivate().
	AppPaint(hwndApp, hdc);
	ReleaseDC(hwndApp, hdc);
}


////////////////////////////////////////////////////////////////////////////
// AppIdle().
//
// Do one iteration of WT.  Allows WT to continue to cycle even when user
// isn't actively playing.
//
//RETURN TRUE IF YOU HAVE NOTHING TO DO OTHERWISE YOUR APP WILL BE A CPU PIG!
//
BOOL AppIdle()
{
	if (bAppActive && bWTinitialized) {
   	// we are the foreground app.
		AppDoCycle();
		return FALSE;
    }
    else
    {
		//background app.
		AppDoCycle();
		return FALSE;
    }
}


/////////////////////////////////////////////////////////////////////////////
// AppSetCaption().  Sets the window caption to the specified string,
// appended to the WT base caption.
//
void AppSetCaption(char *szCaption)
{
	char szWindowTitle[MAXFILENAME];

	lstrcpy(szWindowTitle, szAppBaseWindowTitle);
	lstrcat(szWindowTitle, " - ");
	lstrcat(szWindowTitle, szCaption);
	SetWindowText(hwndApp, szWindowTitle);
}



/////////////////////////////////////////////////////////////////////////////
// AppWndProc( hwnd, uiMessage, wParam, lParam )
//
// The window proc for the app's main (tiled) window.  This processes all
// of the parent window's messages.
//
LONG FAR PASCAL _export AppWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    BOOL f;

		//is it the registered message?  (this proc is called after the creation
		//of the msgApp).

	 if (msg == msgApp) {				
 		 if (bShowRendering) {
		 	HDC hdc = GetDC(hwndApp);
		 	AppPaint(hwndApp, hdc);
		 	ReleaseDC(hwndApp, hdc);
		 	bShowRendering = (int)wParam;	//sent a 0 to turn off bShowRendering.
		 }
	    return 0L;
	 }
		
		//well, how about standard Windows messages?

    switch (msg)
    {
    case WM_CREATE:
			//let WM_SIZE do all the work.
	      break;

    case WM_ACTIVATEAPP:
		    bAppActive = (BOOL)wParam;
			wt_reset_input();
			// *** Remap the system colors and deal with the palette
			AppActivate(bAppActive);

			if (hpalApp)
			{
				HDC hdc = GetDC(hwnd);

				UnrealizeObject(hpalApp);
				SelectPalette(hdc, hpalApp, FALSE);
				RealizePalette(hdc);

				ReleaseDC(hwnd, hdc);
			}
			break;

    case WM_SIZE:			
			wt_reset_input();
		   nBitmapW = LOWORD(lParam);			//new size.
		   nBitmapH = HIWORD(lParam);

			nBitmapW /= StretchFactor;
			nBitmapH /= StretchFactor;

			//Windows DIBs, including WinG bitmaps, are always a multiple of 
			//32-bits wide.  For us, using the typical 8-bit per pixel bitmap,
			//that means we need to ensure the width is a multiple of 4.  This
			//is important because the WT engine treats a bitmap's width and 
			//pitch as one - there is no way for WT to draw into a bitmap
			//using a width different than the pitch.  So we force the bitmap's
			//width to be a multiple of 4, to be both Windows and WT compatible.
			//Note we could have patched WT to deal with the concept of a 
			//bitmap pitch, but that's too much change.

			nBitmapW = ((nBitmapW+3)/4)*4;

			if(Buffer)
			{						//resizing, minimizing, maximizing.
			 	HBITMAP hbm;
			   int Counter;

				//Create a new 8-bit WinGBitmap with the new size
				BufferHeader.Header.biWidth = nBitmapW;
				BufferHeader.Header.biHeight = nBitmapH * Orientation;

				//probably don't need to do this, but do it anyway.
			   for(Counter = 0;Counter < 256;Counter++) {
				    	BufferHeader.aColors[Counter].rgbRed   = ColorTable[Counter].rgbRed;
				    	BufferHeader.aColors[Counter].rgbGreen = ColorTable[Counter].rgbGreen;
				    	BufferHeader.aColors[Counter].rgbBlue  = ColorTable[Counter].rgbBlue;
				    	BufferHeader.aColors[Counter].rgbReserved = 0;
				}

				hbm = WinG.pCreateBitmap(Buffer,
					(BITMAPINFO *)&BufferHeader, &pBuffer);

				//  Select it in and delete the old one
				hbm = (HBITMAP)SelectObject(Buffer, hbm);
				DeleteObject(hbm);

   			PatBlt(Buffer, 0,0,nBitmapW,nBitmapH, BLACKNESS);

	         wt_set_fb_mem(pBuffer);		//tell WT about new bitmap address.
			   wt_reinit(nBitmapW,nBitmapH);  //and about new bitmap size.
				wt_render();					//and have WT render a frame.

			}
			else						//first time.
			{
				//  Create a new WinGDC and 8-bit WinGBitmap

				HBITMAP hbm;
			   int Counter;

				//  Get WinG to recommend the fastest DIB format

				if(WinG.pRecommendDIBFormat((BITMAPINFO *)&BufferHeader))
				{
					//  make sure it's 8bpp and remember the orientation

					BufferHeader.Header.biBitCount = 8;
					BufferHeader.Header.biCompression = BI_RGB;
					Orientation = BufferHeader.Header.biHeight;
					if (Orientation > 0) {
						DebugMsg("WT requires a top-down bitmap!\nYou are about to hit a brick wall at 90 MPH!");
						PostQuitMessage(1);		//works but slams palette.  bummer.
					}
				}
				else
				{
					//  set it up ourselves
	
					BufferHeader.Header.biSize = sizeof(BITMAPINFOHEADER);
					BufferHeader.Header.biPlanes = 1;
					BufferHeader.Header.biBitCount = 8;
					BufferHeader.Header.biCompression = BI_RGB;
					BufferHeader.Header.biSizeImage = 0;
					BufferHeader.Header.biClrUsed = 0;
					BufferHeader.Header.biClrImportant = 0;
				}

				BufferHeader.Header.biWidth = nBitmapW;
				BufferHeader.Header.biHeight = nBitmapH * Orientation;

//#define BAD_PALETTE_CODE
#ifdef BAD_PALETTE_CODE
//This code sets an incorrect palette, but at least you can still use
//regular windows tools.  Good for debugging.
			   HDC Screen;
			   RGBQUAD *pColorTable;

				//  create an identity palette from the DIB's color table

				//  get the 20 system colors as PALETTEENTRIES
    
			    Screen = GetDC(0);

			    GetSystemPaletteEntries(Screen,0,10,LogicalPalette.aEntries);
			    GetSystemPaletteEntries(Screen,246,10,
								LogicalPalette.aEntries + 246);

				ReleaseDC(0,Screen);

				// initialize the logical palette and DIB color table

			    for(Counter = 0;Counter < 10;Counter++)
			    {
			    	// copy the system colors into the DIB header
			    	// WinG will do this in WinGRecommendDIBFormat,
			    	// but it may have failed above so do it here anyway
			    	
			    	BufferHeader.aColors[Counter].rgbRed =
			    					LogicalPalette.aEntries[Counter].peRed;
			    	BufferHeader.aColors[Counter].rgbGreen =
			    					LogicalPalette.aEntries[Counter].peGreen;
			    	BufferHeader.aColors[Counter].rgbBlue =
			    					LogicalPalette.aEntries[Counter].peBlue;
			    	BufferHeader.aColors[Counter].rgbReserved = 0;

   					LogicalPalette.aEntries[Counter].peFlags = 0;

			    	BufferHeader.aColors[Counter + 246].rgbRed =
		    					LogicalPalette.aEntries[Counter + 246].peRed;
			    	BufferHeader.aColors[Counter + 246].rgbGreen =
		    					LogicalPalette.aEntries[Counter + 246].peGreen;
			    	BufferHeader.aColors[Counter + 246].rgbBlue =
		    					LogicalPalette.aEntries[Counter + 246].peBlue;
			    	BufferHeader.aColors[Counter + 246].rgbReserved = 0;

   					LogicalPalette.aEntries[Counter + 246].peFlags = 0;
   				}

            for (i=0; i<256; i++) {
               ColorTable[i].rgbRed = 0;
               ColorTable[i].rgbGreen = 0;
               ColorTable[i].rgbBlue = 0;
            }

            nColors = wt_load_palette();
            for (i=0; i<nColors; i++) {
               int r,g,b;
               wt_get_palette_entry(i,&r,&g,&b);
               ColorTable[i].rgbRed = r;
               ColorTable[i].rgbGreen = g;
               ColorTable[i].rgbBlue = b;
            }
            pColorTable = &ColorTable[0];

			    for(Counter = 10;Counter < 246;Counter++)
			    {
			    	// copy from the original color table to the WinGBitmap's
			    	// color table and the logical palette

			    	BufferHeader.aColors[Counter].rgbRed =
    					LogicalPalette.aEntries[Counter].peRed =
    						pColorTable[Counter].rgbRed;
			    	BufferHeader.aColors[Counter].rgbGreen =
    					LogicalPalette.aEntries[Counter].peGreen =
    						pColorTable[Counter].rgbGreen;
			    	BufferHeader.aColors[Counter].rgbBlue =
    					LogicalPalette.aEntries[Counter].peBlue =
    						pColorTable[Counter].rgbBlue;
			    	BufferHeader.aColors[Counter].rgbReserved = 0;
  					LogicalPalette.aEntries[Counter].peFlags = PC_NOCOLLAPSE;
			    }

			    hpalApp = CreatePalette((LOGPALETTE *)&LogicalPalette);
#else
//GOOD_PALETTE_CODE
//Working palette code.  Has correct colors. And identity.  Same frame
//rate as bad palette code.  This really hoses Windows colors, so
//a GUI debugger's windows are really hard to read.  I couldn't read the
//Symantec IDDE's windows - so I #ifdef'd the bad palette code in for
//debugging.
//Anyway, this code works.  Need 3 things for identity, as far as I can tell:
//1. you have to be writing to your bitmap using a specific palette, 
//2. this palette has to be made into a Windows palette, and selected.
//3. this palette has to be copied into the BitmapInfo header of the WinG
//   bitmap, before creating it (actually as a parameter to creating it).
//
				ClearSystemPalette();
				CreateWTPalette();

			    for(Counter = 0;Counter < 256;Counter++) {
				    	BufferHeader.aColors[Counter].rgbRed   = ColorTable[Counter].rgbRed;
				    	BufferHeader.aColors[Counter].rgbGreen = ColorTable[Counter].rgbGreen;
				    	BufferHeader.aColors[Counter].rgbBlue  = ColorTable[Counter].rgbBlue;
				    	BufferHeader.aColors[Counter].rgbReserved = 0;
				}
#endif
			    
				//  Create a WinGDC and Bitmap, then select away
				Buffer = WinG.pCreateDC();
				hbm = WinG.pCreateBitmap(Buffer,
					(BITMAPINFO *)&BufferHeader, &pBuffer);

				//  Store the old hbitmap to select back in before deleting
				gbmOldMonoBitmap = (HBITMAP)SelectObject(Buffer, hbm);

   			PatBlt(Buffer, 0,0,nBitmapW,nBitmapH, BLACKNESS);

            wt_set_fb_mem(pBuffer);		//hack to get around WT's code.
            strcpy(szDefaultWorldFileName, szModulePath);
            strcat(szDefaultWorldFileName, DEFAULT_WORLD_FILEPATH);
            wt_init(szDefaultWorldFileName, nBitmapW,nBitmapH); 
				AppSetCaption(DEFAULT_WORLD_FILETITLE);

            wt_render();
			}
			bWTinitialized = TRUE;		
            break;

		case WM_KEYDOWN:
						//set WT's keyboard array, then do a WT cycle.
				switch (wParam) {
					case VK_UP:
						kbPressed[kbUpArrow] = 1; break;
					case VK_DOWN:
						kbPressed[kbDownArrow] = 1; break;
					case VK_LEFT:
						kbPressed[kbLeftArrow] = 1; break;
					case VK_RIGHT:
						kbPressed[kbRightArrow] = 1; break;
					case VK_CONTROL:
						kbPressed[kbCtrl] = 1; break;
					case VK_ESCAPE:
						kbPressed[kbEsc] = 1; 
						//DestroyWindow() here?  let's check to ensure that func
						//  will send the proper msgs to close stuff.
						break;
					case VK_SPACE:
						kbPressed[kbSpace] = 1; break;
					case VK_SHIFT:
						kbPressed[kbLeftShift] = 1; break;
					case VK_TAB:
						kbPressed[kbAlt] = 1; break;
				}

					//tried using wt_input/wt_render/InvalidateRect sequence
					//here, but was sometimes jerky (missed frames, actually), 
					//if there was too much keyboard activity.  
					//I think windows was collapsing queued/pending
					//WM_PAINT messages, so that the app got one instead of a 
					//sudden stream.  Anyhow now I draw immediately, and it works
					//great.  Note that AppIdle() processing is required to
					//have acceleration/deceleration/monsters/events occur without
					//keyboard input.  I guess a timer could also be used.
					//So I ended up using this helper routine to repaint.

				AppDoCycle();
				break;

		case WM_KEYUP:
						//set WT's keyboard array, then do a WT cycle.
				switch (wParam) {
					case VK_UP:
						kbPressed[kbUpArrow] = 0; break;
					case VK_DOWN:
						kbPressed[kbDownArrow] = 0; break;
					case VK_LEFT:
						kbPressed[kbLeftArrow] = 0; break;
					case VK_RIGHT:
						kbPressed[kbRightArrow] = 0; break;
					case VK_CONTROL:
						kbPressed[kbCtrl] = 0; break;
					case VK_ESCAPE:
						kbPressed[kbEsc] = 0; break;
					case VK_SPACE:
						kbPressed[kbSpace] = 0; break;
					case VK_SHIFT:
						kbPressed[kbLeftShift] = 0; break;
					case VK_TAB:
						kbPressed[kbAlt] = 0; break;
				}

				AppDoCycle();
				break;

		case WM_LBUTTONDOWN:
				break;

		case WM_RBUTTONDOWN:
				break;

      case WM_MOUSEMOVE:
        	break;

      case WM_COMMAND:
            return AppCommand(hwnd,msg,wParam,lParam);

		case WM_DESTROY:
            PostQuitMessage(0);
            break;

      case WM_CLOSE:
		 	   break;

      case WM_PALETTECHANGED:
		    	if ((HWND)wParam == hwnd)
					break;

	    // fall through to WM_QUERYNEWPALETTE

		case WM_QUERYNEWPALETTE:
		    hdc = GetDC(hwnd);

		    if (hpalApp)
				SelectPalette(hdc, hpalApp, FALSE);
	
		    f = RealizePalette(hdc);
		    ReleaseDC(hwnd,hdc);
	
		    if (f)
				InvalidateRect(hwnd,NULL,TRUE);
	
		    return f;
	
	     case WM_PAINT:
		    hdc = BeginPaint(hwnd,&ps);
			 SelectPalette(hdc, hpalApp, FALSE);
			 RealizePalette(hdc);
		    AppPaint (hwnd,hdc);
	       EndPaint(hwnd,&ps);
	       return 0L;
	    }
	return DefWindowProc(hwnd,msg,wParam,lParam);
}


/////////////////////////////////////////////////////////////////////////////
// Handler for File|OpenWorld menu command.  Prompt user, via dialog box, for
// a world-file to open, then re-init WT with this new world.
//
BOOL AppOnFileOpen(void)
{
	//fill in non-variant fields of OPENFILENAME struct.  don't have to do
	//this every time, but I really don't care.

   ofn.lStructSize       = sizeof(OPENFILENAME);
   ofn.hwndOwner	        = hwndApp;
   ofn.lpstrFilter	     = szFilterSpec;
   ofn.lpstrCustomFilter = NULL;
   ofn.nMaxCustFilter	  = 0;
   ofn.nFilterIndex	     = 1;
   ofn.lpstrFile         = szFileName;
   ofn.nMaxFile	        = MAXFILENAME;
   ofn.lpstrInitialDir   = NULL;
   ofn.lpstrFileTitle    = szFileTitle;
   ofn.nMaxFileTitle     = MAXFILENAME;
   ofn.lpstrTitle        = NULL;
   ofn.lpstrDefExt       = "TXT";
   ofn.Flags             = 0;

   if (!GetOpenFileName ((LPOPENFILENAME)&ofn))
      return FALSE;

	AppSetCaption(szFileTitle);
   wt_new_world(szFileName);   
   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// GetFrameRate.  Times the frame rate in one of four ways: WT only without
// any WinG blitting, WinG only without any WT rendering, WT rendering with
// WinG blitting with static rendering, WT rendering with WinG blitting with
// dynamic rendering (player is rotating).
//
DWORD GetFrameRate(int reps, WPARAM what)
{
	HDC hdc = GetDC(hwndApp);
	if (hpalApp)
	{
		SelectPalette(hdc, hpalApp, FALSE);
		RealizePalette(hdc);
	}

	if (what == ID_STUFF_FRAMERATE_DYNAMIC) {
		kbPressed[kbUpArrow] = 1;
		wt_input();
		wt_input();
		wt_input();
		wt_input();
		wt_input();
		kbPressed[kbUpArrow] = 0;

		kbPressed[kbLeftArrow] = 1;
	}

	DWORD Time = timeGetTime();

	for (int i=0; i<reps; i++) {
		if (what != ID_STUFF_FRAMERATE_RAW_WING) {
			if (what == ID_STUFF_FRAMERATE_DYNAMIC)
				wt_input();
		  	wt_render();
		}
		if (what != ID_STUFF_FRAMERATE_RAW_WT)
			AppPaint(hwndApp, hdc);
	}

	Time = timeGetTime() - Time;
	ReleaseDC(hwndApp, hdc);

	if (what == ID_STUFF_FRAMERATE_DYNAMIC)
		kbPressed[kbLeftArrow] = 0;

	return(Time);
}

////////////////////////////////////////////////////////////////////////////
// AppFrameRate().  Handles the Frame Rate popup menu.  Does performance
// benchmarks, as well as a boatload of other randomness, befitting for a
// hacker's WT port.
//
void AppOnStuff(WPARAM wParam)
{
	DWORD TimeStatic, TimeDynamic, TimeRaw;
	char szMsg[128];
	char szTitle[40];
	BOOL bMsgBox = FALSE;

	wt_reset_input();

	switch (wParam) {
		case ID_STUFF_FRAMERATE_RAW_WING:
			TimeRaw = GetFrameRate(50, wParam);
			wsprintf(szMsg,"50 frames, WinG only, in %d ms\n(does not include WT)", TimeRaw);
			bMsgBox = TRUE;
			strcpy(szTitle,"WTWIN Frame Rate - without WT");
			break;

		case ID_STUFF_FRAMERATE_RAW_WT:
			TimeRaw = GetFrameRate(50, wParam);
			wsprintf(szMsg,"50 frames, WT only, in %d ms\n(does not include WinG)", TimeRaw);
			bMsgBox = TRUE;
			strcpy(szTitle,"WTWIN Frame Rate - without WinG");
			break;

		case ID_STUFF_FRAMERATE_STATIC:
			TimeStatic = GetFrameRate(50, wParam);
			wsprintf(szMsg,"50 frames, static, in %d ms", TimeStatic);
			bMsgBox = TRUE;
			strcpy(szTitle,"WTWIN Frame Rate");
			break;

		case ID_STUFF_FRAMERATE_DYNAMIC:
			TimeDynamic = GetFrameRate(50, wParam);
			wsprintf(szMsg,"50 frames, dynamic, in %d ms", TimeDynamic);
			strcpy(szTitle,"WTWIN Frame Rate");
			bMsgBox = TRUE;
			break;

		case ID_STUFF_STATISTICS:
//7/25/94 GCT - need to add Stretch mode and fix client size.  this is kind
//of a bogus feature, anyway.
			{
				RECT rWin,rClient;
				GetWindowRect(hwndApp,&rWin);
				GetClientRect(hwndApp,&rClient);
				int dx = rClient.right - rClient.left + 1;
				int dy = rClient.bottom - rClient.top + 1;
wsprintf(szMsg,"Window rect %d,%d,%d,%d\nClient rect %d,%d,%d,%d\nClient size %d,%d\nWinG bitmap size %d,%d",
						rWin.left, rWin.top, rWin.right, rWin.bottom,
						rClient.left, rClient.top, rClient.right, rClient.bottom,
						dx,dy,
						nBitmapW,nBitmapH);
				strcpy(szTitle,"WTWIN Statistics");
				bMsgBox = TRUE;
			}
			break;

		case ID_STUFF_PLAYER_INFO:	
			{
				float fX,fY,fAngle,fHeight;
				wt_get_player_info(&fX,&fY,&fAngle,&fHeight);
				sprintf(szMsg,"X,Y %f,%f\nAngle %f\nHeight %f",
							fX,fY,fAngle,fHeight);		//wsprintf has no %f's
				strcpy(szTitle,"WTWIN Player Info");
				bMsgBox = TRUE;
			}
			break;

		case ID_STUFF_RESET:
			//input was reset above.
			wt_reset_view();
			break;
	}

	if (bMsgBox)
		MessageBox(0,szMsg,szTitle,MB_OK);
}


/////////////////////////////////////////////////////////////////////////////
// AppCommand(hwnd, msg, wParam, lParam )
//
//	handles WM_COMMAND messages for the main window (hwndApp)
// of the parent window's messages.
//
LONG AppCommand (HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(wParam)
    {
        case ID_FILE_ABOUT:
	    		DialogBox(hInstApp,"AppAbout",hwnd,(DLGPROC)AppAbout);
            break;

        case ID_FILE_EXIT:
            PostMessage(hwnd,WM_CLOSE,0,0L);
            break;

        case ID_FILE_OPEN:
            AppOnFileOpen();
            break;

		  case ID_STUFF_FRAMERATE_RAW_WING:
		  case ID_STUFF_FRAMERATE_RAW_WT:
		  case ID_STUFF_FRAMERATE_STATIC:
		  case ID_STUFF_FRAMERATE_DYNAMIC:
		  case ID_STUFF_STATISTICS:
		  case ID_STUFF_RESET:
 		  case ID_STUFF_PLAYER_INFO:
				AppOnStuff(wParam);
            break;

		  case ID_STUFF_SHOW_RENDERING:
			bShowRendering = bShowRendering ? FALSE : TRUE;
			//don't bother to check the menu item, since this will be active
			//only for one frame, during which user can't do anything anyway.
			break;				

		  case ID_STUFF_1X:			//stretch choices.
		  case ID_STUFF_2X:
				{
			   HMENU Menu = GetMenu(hwndApp);

					// uncheck current selection
				CheckMenuItem(Menu,ID_STUFF_1X - 1 + StretchFactor,MF_UNCHECKED);

				// get the stretch factor
				StretchFactor = 1 + wParam - ID_STUFF_1X;

				CheckMenuItem(Menu,wParam,MF_CHECKED);	//check new selection.

				InvalidateRect(hwnd,NULL,TRUE);			//force a re-Blit
				}

				//I could have caused a WM_SIZE here, which would have refit the
				//stretched/shrunk bitmap to the client size, but this way you
				//can really see it shrink/expand as you toggle this menu entry.
				//If you toggle the entry, then resize the window to nearly the
				//same size (windows recognizes a same-size resize and ignores
				//it, so you have to alter the size a little), a new bitmap will
				//be created that honors the Stretch setting.  So if you're at
				//320x200 1:1, and you select 2X, the window will be repainted
				//with the upper-left corner of the stretched 2:1, 640x400 bitmap.
				//Then if you resize just ever so little, a new bitmap about
				//160x100 will be created, and displayed 2:1 at 320x200, filling
				//the new window.  Whew.  Needlessly complex, I would say.

				break;

	}
    return 0L;
}


/////////////////////////////////////////////////////////////////////////////
// CreateWTPalette.
//
// Creates at Windows palette from WT's palette.  First requests WT to read
// it's palette, then gets the entries and populates the RGB table.  Then it
// calls CreateIdentityPalette(), which is a help routine in winpal.cpp
// (provided generously by Microsoft, in the WinG sample PALANIM sample.c).
//
void CreateWTPalette(void)
{
	int i;
	int nColors;
	int r,g,b;
	pal syspal;
   HDC Screen;

   nColors = wt_load_palette();

   for (i=0; i<256; i++) {
		ColorTable[i].rgbRed = 0;
      ColorTable[i].rgbGreen = 0;
      ColorTable[i].rgbBlue = 0;
		ColorTable[i].rgbReserved = 0;
	}

			//WT only has 192 colors, at the moment.  Let's populate our table's
			//upper colors with the System colors, so perhaps WT won't be so
			//self-centered with colors - makes little difference, but I 
			//thought it best to have some colors, peferrably something visible,
			//in the unused palette slots.  This way a bug might be found.
			
   Screen = GetDC(0);
   GetSystemPaletteEntries(Screen,0,10,syspal.aEntries);
   GetSystemPaletteEntries(Screen,246,10,syspal.aEntries + 246);
	ReleaseDC(0,Screen);

			//put system last 10 colors in our last 10 slots, and first 10
			//in second-to-last 10 slots.
   for (i=0; i<10; i++) {
		ColorTable[246+i].rgbRed   = syspal.aEntries[246+i].peRed;
		ColorTable[246+i].rgbGreen = syspal.aEntries[246+i].peGreen;
		ColorTable[246+i].rgbBlue  = syspal.aEntries[246+i].peBlue;

		ColorTable[236+i].rgbRed   = syspal.aEntries[i].peRed;
		ColorTable[236+i].rgbGreen = syspal.aEntries[i].peGreen;
		ColorTable[236+i].rgbBlue  = syspal.aEntries[i].peBlue;
	}

   for (i=0; i<nColors; i++) {
		wt_get_palette_entry(i,&r,&g,&b);
		ColorTable[i].rgbRed = r;
      ColorTable[i].rgbGreen = g;
      ColorTable[i].rgbBlue = b;
		ColorTable[i].rgbReserved = 0;
	}

	if (hpalApp)
		DeleteObject(hpalApp);
	hpalApp = CreateIdentityPalette(ColorTable, nColors);
}

