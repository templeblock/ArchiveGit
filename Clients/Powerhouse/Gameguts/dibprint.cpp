#include <windows.h>
#include "proto.h"
#include "commdlg.h"

typedef struct 
   {
   BOOL bGraphics;            // Band includes graphics
   BOOL bText;                // Band includes text.
   RECT GraphicsRect;         // Rectangle to output graphics into.
   }
BANDINFOSTRUCT;

// Globals for this module.
static HWND hDlgAbort    = NULL;        // Handle to abort dialog box.
static BOOL bAbort       = FALSE;       // Abort a print operation?

// Macros
#define WM_CHANGEPCT (WM_USER+1)
#define ChangePrintPercent(nPct)    SendMessage(hDlgAbort, WM_CHANGEPCT, nPct, NULL)

// Function prototypes.
BOOL FAR PASCAL PrintAbortProc (HDC hDC, short code);
int  FAR PASCAL PrintAbortDlg  (HWND hWnd, 
                            unsigned msg, 
                                WORD wParam, 
                                LONG lParam);

//---------------------------------------------------------------------
//
// Function:   CDib::Print
//
// Purpose:    This routine drives the printing operation.  It has the code
//             to handle both banding and non-banding printers.  A banding
//             printer can be distinguished by the GetDeviceCaps() API (see
//             the code below.  On banding devices, must repeatedly call the
//             NEXTBAND escape to get the next banding rectangle to print
//             into.  If the device supports the BANDINFO escape, it should
//             be used to determine whether the band "wants" text or
//             graphics (or both).  On non-banding devices, we can ignore
//             all this and call PrintPage() on the entire page!
//
// Parms:      hDIB        == Handle to global memory with a DIB spec in it.
//                              can be either a Win30 DIB or an OS/2 PM DIB.
//             lpPrintRect == Rect to print (decoded based on next parm)
//             wUnits      == Units lpPrintRect is in (see 
//                              TranslatePrintRect()).
//             dwROP       == Raster operation to use.
//!!!!!!!!!!!!!!!!!!!!dwROP isn't used !!!!!!!!!!!!!!!!!!!!!!
//             fBanding    == TRUE when want to do banding (use NEXTBAND).
//
// Returns:   Encoded error value -- bitwise combination of ERR_PRN_*
//             in PRINT.H.  More than one error can be returned --
//             the application can parse the bits in the DWORD returned,
//             or call ShowPrintError() to display all the errors
//             that ocurred.
//
// History:   Date      Reason
//             6/01/91  Created
//            10/26/91  Added error return codes.
//                      Use DeviceSupportsEscape() instead
//                        of QUERYESCSUPPORT.
//            10/29/91  Added the fUse31APIs flag.
//            11/14/91  Added more error checking.
//                      Added lpDocName as a parameter.
//             
//---------------------------------------------------------------------

void PrintLabel( HDC hPrnDC, LPRECT lpRect, LPSTR lpString, int iPoints )
{
	if ( !lpRect )
		return;

    LOGFONT Font;
	Font.lfHeight = (int)(GetDeviceCaps(hPrnDC, LOGPIXELSY) * ((float)iPoints/72.0)); // points
	Font.lfWidth = 0;
	Font.lfEscapement = 0;
	Font.lfOrientation = 0;
	Font.lfWeight = FW_NORMAL;
	Font.lfItalic = 0;
	Font.lfUnderline = 0;
	Font.lfStrikeOut = 0;
	Font.lfCharSet = ANSI_CHARSET;
	Font.lfOutPrecision = OUT_DEFAULT_PRECIS;
	Font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	Font.lfQuality = DEFAULT_QUALITY;
	Font.lfPitchAndFamily = FF_ROMAN;
	lstrcpy(Font.lfFaceName, "");
	HFONT hFont = CreateFontIndirect(&Font);
	HFONT hOldFont = (HFONT)SelectObject(hPrnDC, hFont);

	// Print out the label
	SetTextAlign( hPrnDC, TA_LEFT | TA_BOTTOM );

	long lTextHeight;
	#ifdef WIN32
		SIZE Size;
		GetTextExtentPoint32(hPrnDC, lpString, lstrlen(lpString), &Size);
		lTextHeight = Size.cy;
	#else
		DWORD extent = GetTextExtent(hPrnDC, lpString, lstrlen(lpString));
		lTextHeight = HIWORD(extent);
	#endif

	lTextHeight *= 5;
	lTextHeight /= 4;

	DWORD OldColor = SetTextColor(hPrnDC, RGB(0,0,0));
	TextOut(hPrnDC, lpRect->left, lpRect->bottom/* - (int)lTextHeight*/, lpString, lstrlen(lpString));
	SetTextColor(hPrnDC, OldColor);
	SelectObject(hPrnDC, hOldFont);
	lpRect->bottom -= (int)lTextHeight;
}


BOOL CDib::Print(HWND hWnd, HINSTANCE hInstance, int idPrintDlg, LPSTR lpszDocName)
{
	HDC			hPrnDC;
	RECT        rPrint;
	static ABORTPROC lpAbortProc;
	static DLGPROC 	lpAbortDlg;
    DOCINFO 	DocInfo;
	BOOL		fBanding = FALSE;
	BOOL		fSuccess = TRUE;

	hDlgAbort = NULL;
	if (hPrnDC = GetPrinterDC ())
	{
      	GetPrintRect (hPrnDC, &rPrint);
        // Initialize the abort procedure.  Then STARTDOC.
      	bAbort      = FALSE;
		if (idPrintDlg)
		{
	      	lpAbortProc = (ABORTPROC)MakeProcInstance((FARPROC)PrintAbortProc, hInstance);
      		lpAbortDlg  = (DLGPROC)MakeProcInstance((FARPROC)PrintAbortDlg,  hInstance);
      		hDlgAbort   = CreateDialog(hInstance, MAKEINTRESOURCE(idPrintDlg), hWnd, lpAbortDlg);
			if (SetAbortProc (hPrnDC, lpAbortProc) < 0)
    	    	goto PRINTERRORCLEANUP;
		}

      	DocInfo.cbSize      = sizeof (DocInfo);
      	DocInfo.lpszDocName = lpszDocName;
      	DocInfo.lpszOutput  = NULL;

        if (StartDoc (hPrnDC, &DocInfo) < 0)
        	goto PRINTERRORCLEANUP;

		if (StartPage(hPrnDC) < 0)
        	goto PRINTERRORCLEANUP;

		PrintLabel(hPrnDC, &rPrint, lpszDocName, 10/*iPoints*/);

      	if (fBanding) 
         	fSuccess = BandToPrinter (hPrnDC, &rPrint);
      	else
            // When not doing banding, call PrintABand() to dump the
            //  entire page to the printer in one shot (i.e. give it
            //  a band that covers the entire printing rectangle,
            //  and tell it to print graphics and text).
         	fSuccess = PrintABand (hPrnDC, &rPrint, &rPrint, TRUE, TRUE);

    	EndPage (hPrnDC);

         // End the print operation.  Only send the ENDDOC if
         //   we didn't abort or error.
		if (!bAbort)
			EndDoc (hPrnDC);

         // All done, clean up.
		PRINTERRORCLEANUP:

		if (hDlgAbort)
			DestroyWindow (hDlgAbort);
		if (idPrintDlg)
		{
			FreeProcInstance((FARPROC)lpAbortProc);
			FreeProcInstance((FARPROC)lpAbortDlg);
		}

		DeleteDC (hPrnDC);
	}
	else
		fSuccess = FALSE;
		
	return (fSuccess);
}



//---------------------------------------------------------------------
//
// Function:   BandDIBToPrinter
//
// Purpose:    Repeatedly call the NEXTBAND escape to get the next
//             banding rectangle to print into.  If the device supports
//             the BANDINFO escape, use it to determine whether the band
//             wants text or graphics (or both).  For each band, call
//             PrintABand() to do the actual output.
//
// Parms:      hPrnDC   == DC to printer.
//             lpDIBHdr == Ptr to DIB header (BITMAPINFOHEADER or 
//                         BITMAPCOREHEADER)
//             lpBits   == Ptr to DIB's bitmap bits.
//
// Returns:    WORD -- One (or more) of the printer errors defined as
//             ERR_PRN_* in PRINT.H.
//
//             ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//            10/26/91  Chopped out of DIBPrint().
//                      Use DeviceSupportsEscape() instead of
//                        QUERYESCSUPPORT.
//            11/14/91  Added Error return codes ERR_PRN_BANDINFO
//                        and errors from PrintABand.
//            01/22/91  Fixed NEXTBAND error return check (was checking
//                        if != 0, now check if > 0).
//             
//---------------------------------------------------------------------

BOOL CDib::BandToPrinter (HDC hPrnDC, LPRECT lpPrintRect)
{
	BANDINFOSTRUCT 	bi;
	BOOL           	bBandInfoDevice;
	RECT           	rect;
	int            	nEscRet;
	BOOL			fSuccess = TRUE;

	// All printers should support the NEXTBAND escape -- we'll
	//  check here, just in case, though!
	if (!DeviceSupportsEscape (hPrnDC, NEXTBAND))
		return(FALSE);

	// Check if device supports the BANDINFO escape.  Then setup
	//  the BANDINFOSTRUCT (we'll use the values we put into it
	//  here later even if the device doesn't support BANDINFO).
	bBandInfoDevice = DeviceSupportsEscape (hPrnDC, BANDINFO);
	bi.bGraphics    = TRUE;
	bi.bText        = TRUE;
	bi.GraphicsRect = *lpPrintRect;

    // Enter the banding loop.  For each band, call BANDINFO if
    //  appropriate.  Then call PrintABand() to do the actual
    //  output.  Terminate loop when NEXTBAND returns an empty rect.
	while (((nEscRet = Escape (hPrnDC, NEXTBAND, NULL, NULL, (LPSTR) &rect)) > 0) &&
         !IsRectEmpty (&rect))
	{
    	if (bBandInfoDevice)
         	if (!Escape (hPrnDC, BANDINFO, sizeof (BANDINFOSTRUCT), (LPSTR) &bi, (LPSTR) &bi))
			{
				fSuccess = FALSE;
				break;
			}

      	if (!PrintABand (hPrnDC, lpPrintRect, &rect, bi.bText, bi.bGraphics))
		{
			fSuccess = FALSE;
			break;
		}
	}

	if (nEscRet <= 0)
		fSuccess = FALSE;

	return fSuccess;
}




//---------------------------------------------------------------------
//
// Function:   PrintABand
//
// Purpose:    This routine does ALL output to the printer.  It is driven by
//             BandDIBToPrinter().  It is called for both banding and non-
//             banding printing devices.  lpClipRect contains the rectangular
//             area we should do our output into (i.e. we should clip our
//             output to this area).  The flags fDoText and fDoGraphics
//             should be set appropriately (if we want any text output to
//             the rectangle, set fDoText to true).  Normally these flags
//             are returned on banding devices which support the BANDINFO
//             escape.  On non-banding devices, all output goes to the
//             entire page, so this routine is passes a rectangle for
//             the entire page, and fDoText = fDoGraphics = TRUE.
//
//             This routine is also responsible for doing stretching of
//             the DIB.  As such, the lpRectOut parameter points to a
//             rectangle on the printed page where the entire DIB will
//             fit -- the DIB is stretched appropriately to fit in this
//             rectangle.
//
//             After printing a band, updates the print % shown in the
//             abort dialog box.
//
// Parms:      hDC         == DC to do output into.
//             lpRectOut   == Rectangle on DC DIB should fit in.
//             lpRectClip  == Rectangle to output during THIS call.
//             fDoText     == Output text into this rectangle (unused by DIBView)?
//             fDoGraphics == Output graphics into this rectangle?
//             lpDIBHdr    == Pointer to DIB's header (either a
//                              BITMAPINFOHEADER or a BITMAPCOREHEADER)
//             lpDIBBits   == Pointer to the DIB's bitmap bits.
//
// Returns:    One or more of the ERR_PRN_* errors in PRINT.H (or'd
//             together. ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

BOOL CDib::PrintABand (HDC hDC,
               			LPRECT lpRectOut,
               			LPRECT lpRectClip,
               			BOOL fDoText,
               			BOOL fDoGraphics)
{
	int    iHeight, iWidth;
	RECT   rect;
	double dblXScaling, dblYScaling;
	BOOL fSuccess = TRUE;


	if (fDoGraphics)
	{
		iWidth = GetWidth();
		iHeight = abs(GetHeight());

		dblXScaling = ((double) lpRectOut->right - lpRectOut->left) / iWidth;
		dblYScaling = ((double) lpRectOut->bottom - lpRectOut->top) / iHeight;

		// Now we set up a temporary rectangle -- this rectangle
		//  holds the coordinates on the paper where our bitmap
		//  WILL be output.  We can intersect this rectangle with
		//  the lpClipRect to see what we NEED to output to this
		//  band.  Then, we determine the coordinates in the DIB
		//  to which this rectangle corresponds (using dbl?Scaling).
		IntersectRect (&rect, lpRectOut, lpRectClip);
		if (!IsRectEmpty (&rect))
		{
			RECT rectIn;
			int  nPct;

			rectIn.left   = (int) ((rect.left - lpRectOut->left) / 
			                        dblXScaling + 0.5);
			rectIn.top    = (int) ((rect.top  - lpRectOut->top) / 
			                        dblYScaling + 0.5);
			rectIn.right  = (int) (rectIn.left + (rect.right  - rect.left) / 
			                        dblXScaling + 0.5);
			rectIn.bottom = (int) (rectIn.top  +  (rect.bottom - rect.top) / 
			                        dblYScaling + 0.5);

			DCBlt(  hDC,
					rect.left,
					rect.top,
					rect.right-rect.left,
					rect.bottom-rect.top,
					rectIn.left,
					rectIn.top,
					rectIn.right-rectIn.left,
					rectIn.bottom-rectIn.top,
					SRCCOPY);

			// Change percentage of print shown in abort dialog.
			nPct = MulDiv (rect.bottom, 
			               100, 
			               lpRectOut->bottom);
			ChangePrintPercent (nPct);
		}
	}

	return fSuccess;
}



//---------------------------------------------------------------------
//
// Function:   DeviceSupportsEscape
//
// Purpose:    Uses QUERYESCSUPPORT to see if the given device
//             supports the given escape code.
//
// Parms:      hDC         == Device to check if escape is supported on.
//             nEscapeCode == Escape code to check for.
//
// History:   Date      Reason
//            10/26/91  Created
//             
//---------------------------------------------------------------------

BOOL CDib::DeviceSupportsEscape (HDC hDC, int nEscapeCode)
{
   return Escape(hDC, QUERYESCSUPPORT, sizeof(int), (LPSTR) &nEscapeCode, NULL);
}

void CDib::GetPrintRect (HDC hDC, LPRECT lpPrintRect)
{
	int cxPage, cyPage, iWidth, iHeight;

	if (!hDC)
  		return;

   	iWidth = cxPage = GetDeviceCaps (hDC, HORZRES);
   	iHeight = cyPage = GetDeviceCaps (hDC, VERTRES);

	ScaleToFit(&iWidth, &iHeight, GetWidth(), abs(GetHeight()), TRUE);
	//iWidth = GetWidth();
	//iHeight = abs(GetHeight());

   	lpPrintRect->left   = (cxPage - iWidth) / 2;
   	lpPrintRect->top    = (cyPage - iHeight) / 2;
	lpPrintRect->right  = lpPrintRect->left + iWidth;
   	lpPrintRect->bottom = lpPrintRect->top + iHeight;
}

//---------------------------------------------------------------------
//
// Function:   GetPrinterDC
//
// Purpose:    Returns a DC to the currently selected printer.  Returns
//             NULL on error.
//
// Parms:      None
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

HDC CDib::GetPrinterDC (void)
{
   PRINTDLG pd;

   pd.lStructSize          = sizeof (pd);
   pd.hwndOwner            = NULL;
   pd.hDevMode             = NULL;
   pd.hDevNames            = NULL;
   pd.hDC                  = NULL;
   pd.Flags                = PD_RETURNDC | PD_RETURNDEFAULT;
   pd.nFromPage            = 0;
   pd.nToPage              = 0;
   pd.nMinPage             = 0;
   pd.nMaxPage             = 0;
   pd.nCopies              = 0;
   pd.hInstance            = NULL;
   pd.lCustData            = NULL;
   pd.lpfnPrintHook        = NULL;
   pd.lpfnSetupHook        = NULL;
   pd.lpPrintTemplateName  = NULL;
   pd.lpSetupTemplateName  = NULL;
   pd.hPrintTemplate       = NULL;
   pd.hSetupTemplate       = NULL;

   if (PrintDlg (&pd))
      return pd.hDC;
   else
      return NULL;
}






//-------------------- Abort Routines ----------------------------


//---------------------------------------------------------------------
//
// Function:   PrintAbortProc
//
// Purpose:    Abort procedure while printing is occurring.  Registered
//             with Windows via the SETABORTPROC escape.  This routine
//             is called regularly by the sytem during a print operation.
//
//             By putting a PeekMessage loop here, multitasking can occur.
//             PeekMessage will yield to other apps if they have messages
//             waiting for them.
//
//             Doesn't bother if the global, bAbort, is set.  This var
//             is set by PrintAbortDlg() when a user cancels a print
//             operation.
//
// Parms:      hDC  == DC printing is being done to
//             code == Error code (see docs for SETABORTPROC printer
//                      escape).
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

BOOL FAR PASCAL PrintAbortProc(HDC hDC, short code)
{
   MSG msg;

   bAbort |= (code != 0);

   while (!bAbort && PeekMessage (&msg, 0, 0, 0, PM_REMOVE))
      if (!IsDialogMessage (hDlgAbort, &msg))
         {
         TranslateMessage (&msg);
         DispatchMessage (&msg);
         }

   return (!bAbort);
}



//---------------------------------------------------------------------
//
// Function:   PrintAbortDlg
//
// Purpose:    Dialog box window procedure for the "cancel" dialog
//             box put up while DIBView is printing.
//
//             Functions sets bAbort (a global variable) to true
//             if the user aborts the print operation.  Other functions
//             in this module then "do the right thing."
//
//             Also handles MYWM_CHANGEPCT to change % done displayed
//             in dialog box.
//
// Parms:      hWnd    == Handle to this dialog box.
//             message == Message for window.
//             wParam  == Depends on message.
//             lParam  == Depends on message.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

int FAR PASCAL PrintAbortDlg(HWND hWnd, unsigned msg, WORD wParam, LONG lParam)
{
   switch (msg)
      {
      case WM_INITDIALOG:
		 CenterWindow(hWnd);
         SetFocus(hWnd);
         return TRUE;


      case WM_COMMAND:
         bAbort = TRUE;
         return TRUE;


      case WM_CHANGEPCT:
         {
         //char szBuf[20];

         //wsprintf (szBuf, "%3d%% done", wParam);
         //SetDlgItemText (hWnd, IDC_PRNPCT, szBuf);
         return TRUE;
         }
      }

   return FALSE;
}


