/*-------------------------------------------------------------------------
    %%File:  tabdlg.c
    %%Owner: stevebu
    
-------------------------------------------------------------------------*/
#include "_tabdlg.h"

LPTSW vlptsw = lpNil;		// Head of Linked List
HFONT vhfnt = (HFONT)NULL;
HFONT vhfntLight = (HFONT)NULL;
short vfLightFont;			// Used to Communicate with Gray String
HBRUSH  vhbrsGray;			// Gray Brush used in Invert Caret

#define	HfntSetFont(hdc) ((vhfnt == (HFONT)NULL) ? (HFONT) NULL: \
					    SelectObject((hdc), vhfnt))
					    
#define chAccelerator '&'
#define ichAccelNil		((unsigned int)0xFFFF)

// Forward Declarations
long FAR PASCAL EXPORT TabSwitcherWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
long FAR PASCAL EXPORT TabBorderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
                 
/* L P B  C O P Y  L P R G B */
/*----------------------------------------------------------------------------
	%%Function: LpbCopyLprgb
	%%Contact: stevebu

	A block transfer of bytes from lpbFrom to lpbTo.  The size of the
	block is cb bytes.  This handles the case of overlapping source and
	destination.  Returns a pointer to the end of the destination buffer
	(lpbTo + cb).

----------------------------------------------------------------------------*/
void FAR *LpbCopyLprgb(lpbFrom, lpbTo, cb)
void FAR *lpbFrom;
void FAR *lpbTo;
uint cb;
{
	if (lpbTo < lpbFrom)
		{
		while (cb--)
			*((uchar FAR *)lpbTo)++ = *((uchar FAR *)lpbFrom)++;

		return lpbTo;
		}
	else
		{
		void FAR *lpbRet;

		lpbRet = ((uchar FAR *)lpbTo) += cb;
		(uchar FAR *)lpbFrom += cb;

		while (cb--)
			*--(uchar FAR *)lpbTo = *--(uchar FAR *)lpbFrom;

		return lpbRet;
		}
}


/* S E T  B Y T E S */
/*----------------------------------------------------------------------------
	%%Function: SetBytes
	%%Contact: stevebu

	Fills cb bytes of memory starting at FAR location lpb with bFill.

----------------------------------------------------------------------------*/
void SetBytes(lpb, bFill, cb)
void FAR *lpb;
uint bFill;
uint cb;
{
	while (cb--)
		*((char FAR *)lpb)++ = bFill;
}


/* C C H  L S Z */
/*----------------------------------------------------------------------------
	%%Function: CchLsz
	%%Contact: stevebu

	returns length of lsz.

----------------------------------------------------------------------------*/
uint CchLsz(lsz)
const uchar FAR *lsz;
{
	const uchar FAR *lpch = lsz;

	while (*lpch++ != 0)
		;

	return lpch - lsz - 1;
}
                    

/*   S T O R E  L P T S W  */
/*-------------------------------------------------------------------------
    %%Function: StoreLptsw
    %%Owner:    stevebu
    %%Reviewed: 

    Stores lptsw in hwndTsw.
    
-------------------------------------------------------------------------*/
void StoreLptsw(lptsw, hwndTsw)
LPTSW lptsw;
HWND hwndTsw;					    					    
{
	LPTSW FAR *lplptsw = &vlptsw;

	while (*lplptsw != lpNil)
		lplptsw = &((*lplptsw)->lptswNext);
	
	*lplptsw = lptsw;
}

/*  L P T S W  F R O M  T S W  */
/*-------------------------------------------------------------------------
    %%Function: LptswFromTsw
    %%Owner:    stevebu
    %%Reviewed: 

    Returns lptsw associated with hwndTsw or cidTswBdr
-------------------------------------------------------------------------*/
LPTSW LptswFromTsw(HWND hwndTsw, int cidTswBdr)
{
	LPTSW lptsw = vlptsw;

	Assert(hwndTsw == (HWND)NULL || cidTswBdr == cidNull);

	while (lptsw)
		{
		if (lptsw->tswh.hwndTsw == hwndTsw || lptsw->tswh.cidTswBdr == cidTswBdr)
			break;
		lptsw = lptsw->lptswNext;
		}
	
	return lptsw;
}


/*  L P T S W  I N I T  */
/*-------------------------------------------------------------------------
    %%Function: LptswInit
    %%Owner:    stevebu
    %%Reviewed: 

    Initialized lptsw based on lptswlp filled out by the user.
    
-------------------------------------------------------------------------*/
LPTSW LptswInitHwndTsw(HWND hwndTsw)
{
	HWND hwndDlg = GetParent(hwndTsw);
	FARPROC lpfnDlgProc = (FARPROC)GetWindowLong(hwndDlg, DWL_DLGPROC);
	TSWLPARAM tswlp;
	LPTSW lptsw;
	TSWH FAR *lptswh;

	// Allocate a tsw
	if ((lptsw = (LPTSW)LpvAllocCb(CbTswCtswe(0), GMEM_MOVEABLE |GMEM_ZEROINIT)) == lpNil)
		return lpNil;

	ClearBytes(&tswlp, cbof(tswlp));

	// Call-back user to get their defaults
	(*lpfnDlgProc)(hwndDlg, (UINT)WM_TABINIT, (WPARAM)GetDlgCtrlID(hwndTsw), (LPARAM)(TSWLPARAM FAR *)&tswlp);
		
	lptswh = &(lptsw->tswh);
	lptswh->isel = tswlp.iselInit;
	lptswh->cRows = tswlp.cRows;
	lptswh->fOnlyOne = !!(tswlp.ftsw & ftswOnlyOne);
	lptswh->fDirectAccel = !!(tswlp.ftsw & ftswDirectAccel);
	lptswh->fValidate = !!(tswlp.ftsw & ftswValidate);
	lptswh->fChiseled = !!(tswlp.ftsw & ftswChiseled);
	lptswh->fCallOnInit = !!(tswlp.ftsw & ftswCallOnInit);
	lptswh->cidTswBdr = tswlp.cidTswBdr; 
	lptswh->hwndTsw = hwndTsw;
		
	return lptsw;
}

#define czaInch          1440
#define czaPoint           20
#define	czaHps			   10 // twips per hps unit
#define hpsInch			  144 // hps units per inch
#define hpsDlgFont 16
#define hpsLtDlgFont 16


/*  N  M U L T  D I V  */
/*-------------------------------------------------------------------------
    %%Function: NMultDiv
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
int NMultDiv(short sw, short swNumer, short swDenom)
{
	long lT;
	int fNeg;

	fNeg = fFalse;

	if (swDenom <= 0)
		{
		if (swDenom == 0)
			return intMax;

		Assert(!fNeg);
		fNeg = fTrue;
		swDenom = -swDenom;
		}

	if ((lT = sw * swNumer) < 0)
		{
		fNeg = !fNeg;
		lT = -lT;
		}

	Assert(0 <= lT);

	lT += swDenom >> 1;
	lT /= swDenom;

	Assert(0 <= lT && lT <= intMax);

	return (int)fNeg ? -lT : lT;
}


/*  F I L L  D L G  F O N T  P L F  */
/*-------------------------------------------------------------------------
    %%Function: FillDlgFontPlf
    %%Owner:    stevebu
    %%Reviewed: 

    Fills out the LOGFONT structure for Microsoft's Dialog Font
    
-------------------------------------------------------------------------*/
void FillDlgFontPlf(plf, hps, dyConvert, fBold)
LOGFONT *plf;
int hps;
int dyConvert;
int fBold;
{
	ClearBytes(plf, cbof(LOGFONT));

	if (AddFontResource("DIALOG.FON"))
		{
		uchar FAR *lpch = ((fBold) ?  "MS Dialog" : "MS Dialog Light");
		
		CopyLsz(lpch, plf->lfFaceName);
		SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
		}
						
	plf->lfWeight = (fBold) ? FW_BOLD : FW_NORMAL;
	plf->lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
	
	plf->lfHeight = -NMultDiv(hps * (czaPoint / 2), dyConvert, czaInch);
}


/*  F  I N I T  T A B  S W I T C H E R  */
/*-------------------------------------------------------------------------
    %%Function: FInitTabSwitcher
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
void FAR PASCAL InitTabSwitcher(HINSTANCE hinst)
{
	WNDCLASS wc;

	ClearBytes(&wc, cbof(wc));
	wc.lpfnWndProc = TabSwitcherWndProc;
	wc.cbWndExtra = cbof(LPARAM);
	wc.hInstance = hinst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = szTswClass;
	
	// Register Tab Switcher Class
	RegisterClass(&wc);

	wc.lpszClassName = szTswBorderClass;
	wc.lpfnWndProc = TabBorderWndProc;

	// Register Tab Switcher Border Class
	RegisterClass(&wc); 	
}

void FAR PASCAL ExitTabSwitcher()
{
	// Free All of Our GDI objects
	if (vhfnt || vhfntLight)
		{
		DeleteObject(vhfnt);
		DeleteObject(vhfntLight);
		if (RemoveFontResource("DIALOG.FON"))
		    SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
		}

	if (vhbrsGray != (HBRUSH)NULL)
		DeleteObject(vhbrsGray);
}


/*  L I B  M A I N  */
/*-------------------------------------------------------------------------
    %%Function: LibMain
    %%Owner:    stevebu
    %%Reviewed: 

    Main Entry Point for TabDlg DLL
    
-------------------------------------------------------------------------*/
int CALLBACK LibMain(hinst, wDataSeg, cbHeapSize, lpszCmdLine)
HINSTANCE hinst;
WORD wDataSeg;
WORD cbHeapSize;
LPSTR lpszCmdLine;
{
//	if (cbHeapSize > 0)
//		UnlockData(0);	
	
	return fTrue; 
}

/*  W  E  P  */
/*-------------------------------------------------------------------------
    %%Function: WEP
    %%Owner:    stevebu
    %%Reviewed: 

    WEP cleanup routine
-------------------------------------------------------------------------*/
int CALLBACK WEP(nExitType)
int nExitType;
{		
	Assert(vlptsw == lpNil);
	
	// Free All of Our GDI objects
	if (vhfnt || vhfntLight)
		{
		DeleteObject(vhfnt);
		DeleteObject(vhfntLight);
		if (RemoveFontResource("DIALOG.FON"))
		    SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
		}

	if (vhbrsGray != (HBRUSH)NULL)
		DeleteObject(vhbrsGray);
	    
	return fTrue;
}


// ***** UTILITY ROUTINES *****


/*  C C H  I N  D X  L S Z  */
/*-------------------------------------------------------------------------
    %%Function: CchInDxSz
    %%Owner:    stevebu
    %%Reviewed: 

	Calculate the maximum number of characters in the given string (and
	current font that can fit in the given width.
	Cannot fail due to OOM.

-------------------------------------------------------------------------*/
UINT CchInDxLsz(dx, lsz, hdc)
int dx;
char FAR *lsz;
HDC	hdc;
{
	UINT 	cch;
	SIZE	sz;
	int 	dxT;
	UINT 	cchLim = CchLsz(lsz) + 1;
	UINT 	cchFirst = 0;

	Assert(dx >= 0);
	Assert(lsz != (char FAR *)NULL);

	for (;;)
		{
		Assert(cchFirst < cchLim);

		if (cchLim - cchFirst == 1)
			return cchFirst;

		cch = ((cchLim - cchFirst) >> 1) + cchFirst;

#ifdef WIN32
		GetTextExtentPoint32(hdc, (LPSTR)lsz, cch, &sz);
		dxT = sz.cx;
#else
		dxT = (XY)GetTextExtent(hdc, (LPSTR)lsz, cch);
#endif

		if (dxT > dx)
			cchLim = cch;
		else
			cchFirst = cch;
		}
}

/*  C L R  G R A Y  H D C  */
/*-------------------------------------------------------------------------
    %%Function: ClrGrayHdc
    %%Owner:    stevebu
    %%Reviewed: 

    Find nearest gray color
    
-------------------------------------------------------------------------*/
COLORREF ClrGrayHdc(hdc, clrFore, clrBack)
HDC hdc;
COLORREF clrFore, clrBack;
	{
	COLORREF clrGray = GetNearestColor(hdc, GetSysColor(COLOR_GRAYTEXT));
	
	if (clrGray == (clrBack = GetNearestColor(hdc, clrBack)))
		if (clrBack == (clrGray = GetNearestColor(hdc, RGB(0x80,0x80,0x80))))
			clrGray = clrFore;
			
	return clrGray;
	}

/*  S T R I N G  O U T  */
/*-------------------------------------------------------------------------
    %%Function: StringOut
    %%Owner:    stevebu
    %%Reviewed: 

	Callback used by GrayString if font is custom-supplied. 
   
-------------------------------------------------------------------------*/
BOOL CALLBACK StringOut(hdc, l, cch)
HDC hdc;
LPARAM l;
int	cch;
{
	BOOL fVal;
	HFONT hfnt = vfLightFont ? vhfntLight : vhfnt;

	if (hfnt != (HFONT)NULL)
		hfnt = SelectObject(hdc, hfnt);
	fVal = ExtTextOut(hdc, 0, 0, 0, NULL, (LPCSTR)l, cch, NULL);
	if (hfnt != (HFONT)NULL)
		hfnt = SelectObject(hdc, hfnt);

	return fVal;
}

/*  G R A Y  T E X T  */
/*-------------------------------------------------------------------------
    %%Function: GrayText
    %%Owner:    stevebu
    %%Reviewed: 

    Draw Gray Text
    
-------------------------------------------------------------------------*/
VOID GrayText(hdc, hwnd, hbrs, lsz, cch, lprec, fLightFont)
HDC hdc;
HWND hwnd;
HBRUSH hbrs;
LPSTR lsz;
UINT cch;
REC FAR *lprec;
BOOL fLightFont;
	{
	COLORREF clrBackSav	= GetBkColor(hdc);
	COLORREF clrForeSav	= GetTextColor(hdc);
	COLORREF clrFore = ClrGrayHdc(hdc, clrForeSav, clrBackSav);

	if (clrFore != clrForeSav)
		{
		SetTextColor(hdc, clrFore);
		ExtTextOut(hdc, lprec->x, lprec->y, 0, NULL, lsz, cch, NULL);
		}
	else
		{
		vfLightFont = !!fLightFont;
		GrayString(hdc, hbrs, StringOut, (LPARAM)lsz, cch, lprec->x, lprec->y, 0, 0);

		// Kludge Alert! GrayString() blew away the foreground and 
		//  background colors.
		SetBkColor(hdc, clrBackSav);
		}
	SetTextColor(hdc, clrForeSav);
	}



/*  D X  P A I N T  T E X T  */
/*-------------------------------------------------------------------------
    %%Function: DxPaintText
    %%Owner:    stevebu
    %%Reviewed: 

    Paint Text and return Dx of Painted Text.
    
-------------------------------------------------------------------------*/
XY DxPaintText(x, y, xMax, lsz, lszLim, fpdt, iAccel, fDraw, hwnd, hdc, hbrs)
int x;
int y;
int xMax;
char FAR *lsz;
char FAR *lszLim;
FPDT fpdt;
UINT iAccel;
BOOL fDraw;
HWND hwnd;
HDC hdc;
HBRUSH	hbrs;
{
	HFONT 		hfontPrev = (HFONT)NULL;
	UINT 		cch;
	int 		dxText,	dxItem;
	TEXTMETRIC 	tm;
	RECT 		rect;
	SIZE		sz;
	
	if (x >= xMax)
		return dxText;	// Why bother?

	if (fpdt & fpdtLight)
		{
		Assert(vhfntLight != (HFONT)NULL);
		hfontPrev = (HFONT)SelectObject(hdc, vhfntLight);
		}
		
	GetTextMetrics(hdc, &tm);

	// If item extent is smaller than text extent, truncate.
#ifdef WIN32
	GetTextExtentPoint32(hdc, (LPCSTR)lsz, cch = lszLim - lsz, &sz);
	dxText = sz.cx;
	dxItem = xMax - x;
	if (dxItem < dxText)
#else
	if ((dxItem = xMax - x) <
	    (dxText = (XY)GetTextExtent(hdc, (LPCSTR)lsz, cch = lszLim - lsz)))
#endif
		{
		cch = CchInDxLsz(dxItem, lsz, hdc);
#ifdef WIN32
		GetTextExtentPoint32(hdc, (LPCSTR)lsz, (int)cch, &sz);
		dxText = sz.cx;
#else
		dxText = (XY)GetTextExtent(hdc, (LPCSTR)lsz, (int)cch);
#endif

		Assert(dxText <= dxItem);
		}

	if (fDraw)
		{
		REC	rec;
		
		if (fpdt & fpdtErase)				//erase the entire rec
			rec.x = x, rec.dx = dxItem;

		AssertSz(!((fpdt & fpdtRight) && (fpdt & fpdtCenter)),
			    "Centered AND right-justified text?");
		if (fpdt & fpdtCenter)
			x += (dxItem - dxText) >> 1;
		else if (fpdt & fpdtRight)
			x += dxItem - dxText;

		if (fpdt & (fpdtErase | fpdtEraseText))
			{
			if (fpdt & fpdtEraseText)		//erase just the bounding rec
				rec.x = x, rec.dx = (dxText + GetSystemMetrics(SM_CXBORDER));
			rec.y = y, rec.dy = tm.tmHeight + GetSystemMetrics(SM_CYBORDER);
			RectOfRec(rect, rec);
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, (LPINT)NULL);
			}

		if (fpdt & fpdtEnable)
			ExtTextOut(hdc, x, y, 0, NULL, lsz, cch, NULL);
		else
			{
			rec.x = x;
			rec.dx = dxText;
			rec.y = y;
			rec.dy = tm.tmHeight;

			GrayText(hdc, hwnd, hbrs, (LPSTR)lsz, cch, (REC FAR *)&rec, fpdt & fpdtLight);
			}

		if ((fpdt & (fpdtEnable | fpdtPrefix)) ==
		    (fpdtEnable | fpdtPrefix) && iAccel != ichAccelNil)
			{
			TEXTMETRIC tm;

			GetTextMetrics(hdc, &tm);
#ifdef WIN32
			rec.x = (XY)GetTextExtentPoint32(hdc, (LPSTR)lsz, iAccel,&sz);
			rec.x = sz.cx;
			GetTextExtentPoint32(hdc, (LPSTR)lsz, iAccel + 1, &sz);
			rec.dx = sz.cx - rec.x;
#else
			rec.x = (XY)GetTextExtent(hdc, (LPSTR)lsz, iAccel);
			rec.dx = (XY)GetTextExtent(hdc, (LPSTR)lsz, iAccel + 1) - rec.x;
#endif
			rec.x += x - tm.tmOverhang;
			rec.dx++;

			// Don't draw underscore if character to underline has
			// been clipped out.
			if (rec.x < x + dxText)
				{
				COLORREF clrBackSav;
				
				if (rec.x + rec.dx > x + dxText)
					rec.dx = x + dxText - rec.x;
				if (rec.x < x)
					rec.x = x;
					
				rec.y = y + tm.tmAscent + 1;
				rec.dy = GetSystemMetrics(SM_CYBORDER);
				clrBackSav = SetBkColor(hdc, GetTextColor(hdc));
				RectOfRec(rect, rec);
				ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, (LPINT)NULL);
				SetBkColor(hdc, clrBackSav);
				}
			}
		}

	if (hfontPrev != (HFONT)NULL)
		SelectObject(hdc, hfontPrev);

	return dxText;
}

/*  I N V E R T  C A R E T  */
/*-------------------------------------------------------------------------
    %%Function: InvertCaret
    %%Owner:    stevebu
    %%Reviewed: 

    Invert a Carent
    
-------------------------------------------------------------------------*/
VOID InvertCaret(hwnd, hdc, x, y, dx, dy, hbrsGray)
HWND hwnd;
HDC hdc;
XY x;
XY y;
XY dx;
XY dy;
HBRUSH hbrsGray;
{
#ifdef MS_PIECE_OF_SHIT
	LONG	rop;
	HBRUSH	hbrsSav;
	COLORREF clrForeSav;
	COLORREF clrBackSav;

	if (hdc == (HDC)NULL)
		return;

	hbrsSav = (HBRUSH)SelectObject(hdc, hbrsGray);

	if (y & 1)
		rop = PATINVERT;
	else
		rop = 0x00A50065;

	clrBackSav = SetBkColor(hdc, RGB(255, 255, 255));
	clrForeSav = SetTextColor(hdc, RGB(0, 0, 0));
	PatBlt(hdc, x, y, dx + 1, 1, rop);
	PatBlt(hdc, x, (y + dy), dx, 1, rop);
	PatBlt(hdc, x, y, 1, dy + 1, rop);
	PatBlt(hdc, (x + dx), y, 1, dy, rop);
	SetBkColor(hdc, clrBackSav);
	SetTextColor(hdc, clrForeSav);
	if (hbrsSav != (HBRUSH)NULL)
		SelectObject(hdc, hbrsSav);
#else
	RECT rFocus;

	if (hdc == (HDC)NULL)
		return;

	rFocus.left = x;
	rFocus.top = y;
	rFocus.right = rFocus.left + dx;
	rFocus.bottom = rFocus.top + dy;
	DrawFocusRect(hdc, &rFocus);
#endif
}


/*  F I L L  R E C  T S W  */
/*-------------------------------------------------------------------------
    %%Function: FillRecTsw
    %%Owner:    stevebu
    %%Reviewed: 

    Fills in rec with colorref clr.
    
-------------------------------------------------------------------------*/
VOID FillRecTsw(hdc, lprec, clr)
HDC hdc;
REC FAR *lprec;
COLORREF clr;
{
	RECT rect;
	
	SetBkColor(hdc, clr);
	RectOfRec(rect, *lprec);
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, 
			(LPRECT)&rect, (LPSTR)NULL, 0, (LPINT)NULL);
}

#define BITMAP_CACHE

/*  T A B  S W I T C H E R  W N D  P R O C  */
/*-------------------------------------------------------------------------
    %%Function: TabSwitcherWndProc
    %%Owner:    stevebu
    %%Reviewed: 

    Window Proc for the Tab Switcher (row of tabs)
    
-------------------------------------------------------------------------*/
long FAR PASCAL EXPORT TabSwitcherWndProc(hwnd, msg, wParam, lParam)
HWND hwnd;
UINT msg;
WPARAM wParam;
LPARAM lParam;
{
	TSWH FAR *lptswh;
	LPTSW lptsw;

	switch (msg)
		{
		case WM_GETDLGCODE:
			return (DLGC_WANTCHARS | DLGC_WANTARROWS);
		// Prevent Windows from doing our erasing and invalidate entire control
		case WM_ERASEBKGND:
			{
			RECT rect;
			COLORREF clrSave;

			if ((lptsw = LptswFromHwndTsw(hwnd)) == lpNil)
				break;

			clrSave = SetBkColor((HDC)wParam, 
					GetSysColor(FChiseledTsw(lptsw) ? COLOR_BTNFACE : COLOR_WINDOW));

			GetUpdateRect(hwnd, &rect, fFalse /*fErase*/); 
			ExtTextOut((HDC)wParam, 0, 0, ETO_OPAQUE,
				    (LPRECT)&rect, (LPSTR)NULL, 0, (LPINT)NULL);
			SetBkColor((HDC)wParam, clrSave);
			return fTrue;
			}

		case WM_CREATE:
			{
			TSWLPARAM tswlp;
			int cidTsw = GetDlgCtrlID(hwnd);
			HWND hwndDlg = GetParent(hwnd);
			UINT itsw;
			UINT iRow;
			BOOL fEnabled = fTrue;
			FARPROC lpfnDlgProc;
			RECT rect;
			HDC hdc;

			// Call-back dialog get user values
			if ((lptsw = LptswInitHwndTsw(hwnd)) == lpNil)
				return fFalse;

			GetClientRect(hwnd, &rect);
			RecOfRect(lptsw->tswh.rec, rect);

			// Hold off using lptswh for a while now, as the following
			//  system and LptswAddTsw calls might shuffle memory
			
			if ((hdc = GetDC(hwnd)) != (HDC)NULL)
				{
				// Create Dialog Fonts
				if (vhfnt == (HFONT)NULL)
					{
					LOGFONT lf;
					int dyConvert = GetDeviceCaps(hdc, LOGPIXELSY);

					Assert(vhfntLight == NULL);
					FillDlgFontPlf(&lf, hpsDlgFont, dyConvert, fTrue /*fBold*/);
					vhfnt = CreateFontIndirect(&lf);
					
					FillDlgFontPlf(&lf, hpsLtDlgFont, dyConvert, fFalse/*fBold*/);
					vhfntLight = CreateFontIndirect(&lf);
					}
#ifdef FOOBAR
				// Create Gray Brush
				if (vhbrsGray == (HBRUSH)NULL)
					{
					short patGray[8];
					HBITMAP	hbmGray	= NULL;
					int i;

					for (i = 0; i < 8; i++)
						patGray[i] = (0x55 << (i & 1));
						
					if ((hbmGray = CreateBitmap(8, 8, 1, 1, (LPSTR)patGray)) == NULL)
						return fFalse;
						
					if ((vhbrsGray = CreatePatternBrush(hbmGray)) == (HBRUSH)NULL)
						return fFalse;
						
					DeleteObject(hbmGray);
					}
#endif // FOOBAR					

#ifdef BITMAP_CACHE				
				if ((lptsw->tswh.hdc = CreateCompatibleDC(hdc)) != (HDC)NULL)
					{
					lptsw->tswh.hfntSav = HfntSetFont(lptsw->tswh.hdc);
					lptsw->tswh.hbm = CreateCompatibleBitmap(hdc, lptsw->tswh.rec.dx, lptsw->tswh.rec.dy);
					}
#endif // BITMAP_CACHE
				
				ReleaseDC(hwnd, hdc);
				}
				
#ifdef BITMAP_CACHE				
			if (lptsw->tswh.hbm == (HBITMAP)NULL)
				return fFalse;
#endif // BITMAP_CACHE				

			// Get the items.
			lpfnDlgProc = (FARPROC)GetWindowLong(hwndDlg, DWL_DLGPROC);

			ClearBytes(&tswlp, cbof(tswlp));	
			
			while (fEnabled = (*lpfnDlgProc)(hwndDlg, (UINT)WM_TABTEXT, (WPARAM)cidTsw, (LPARAM)(LPTSWLPARAM)&tswlp))
				{
				fEnabled &= ftswEnabled;
				
				if (lptsw->tswh.fOnlyOne && tswlp.isz != lptsw->tswh.isel)
					fEnabled = fFalse;

				if ((lptsw = LptswAddTsw(hwnd, lptsw, fEnabled, tswlp.szTab)) == lpNil)
					return fFalse;
					
				tswlp.isz++;
				}

			// Store Lptsw with hwndTsw
			StoreLptsw(lptsw, hwnd);
				
			if (lptsw->tswh.cRows == cRowsAutoTsw)
				{
				REC rec;
				TSWH FAR *lptswh;
				XY dxMax = 0;
				UINT ctswRow;
				TEXTMETRIC tm;
				
				for (itsw = 0, hdc = GetDC(hwnd); itsw < tswlp.isz; itsw++)
					{
					POINT pt;
					TSWE FAR *lptswe = &(lptsw->rgtswe[itsw]);
					
					// the fTrue below means item selected: bigger font
					GetTextExtentTsw(hdc, lptswe->lsz, lptswe->tswx.cch, &pt, fTrue);
					if (pt.x > dxMax)
						dxMax = pt.x;
					}

				GetTextMetrics(hdc, &tm);
				ReleaseDC(hwnd, hdc);

				lptswh = &lptsw->tswh;
				rec = lptswh->rec;
				ctswRow = max(1, rec.dx / dxMax);
				lptswh->cRows = min(cRowsMaxTsw, (tswlp.isz + ctswRow - 1) / ctswRow);
				rec.dy = (tm.tmAscent * 2) * lptswh->cRows;

				if (rec.dy < lptswh->rec.dy)
					{
					lptswh->rec.y += lptswh->rec.dy - rec.dy;
					lptswh->rec.dy = rec.dy;
					}
				}

			Assert(lptsw->tswh.cRows > 0);
			Assert(lptsw->tswh.cRows <= cRowsMaxTsw);

			// Get lptswh now, since we're done shufflin' memory for a while
			lptswh = &(lptsw->tswh);

			// Calculate control metrics.
			Assert(lptswh == &(lptsw->tswh));
			lptswh->rec.dy -= 3;

			// Handle Two Tab Case by simulating three tabs
			if (lptswh->cRows == 1 && tswlp.isz == 2)
				lptswh->ctswRow = 3;
			else
				{
				lptswh->ctswRow = tswlp.isz/ lptswh->cRows;
				lptswh->ctswRow += (tswlp.isz % lptswh->cRows) ? 1 : 0;
				}

			lptswh->dyRow = lptswh->rec.dy / lptswh->cRows;
			lptswh->dxtsw = (lptswh->rec.dx - IRowLastTswh(lptswh) * dxTswShift) / lptswh->ctswRow;

			// Layout rows & move row with sel to bottom.
			itsw = 0;
			for (iRow = 0; iRow < lptswh->cRows; iRow++)
				{
				lptswh->rgRows[iRow] = itsw;
				itsw += lptswh->ctswRow;
				}

			RotateIselRowToBottom(lptswh);

			return fTrue;
			}

		case WM_PAINT:
			{
			UINT itsw, iRowTsw, iColTsw;
			XY dxLine, dxShift, dxShortPrev;
			XY yLine, yBottom;
			REC recTsw, rec, recT;
			RECT rectT;
			PAINTSTRUCT ps;
#ifdef BITMAP_CACHE		
			HDC hdcTsw;
			HBITMAP hbmSav;
			HDC hdc;
#else		
			HDC hdcTsw;	
			COLORREF clrBack;
#endif // BITMAP_CACHE			
			COLORREF clrDlg;

			if ((lptsw = LptswFromHwndTsw(hwnd)) == lpNil)
				break;

			// Notify User of First Tab Switch
			if (lptsw->tswh.fCallOnInit)
				{
				HWND hwndDlg = GetParent(hwnd);
				FARPROC lpfnDlgProc = (FARPROC)GetWindowLong(hwndDlg, DWL_DLGPROC);

				(*lpfnDlgProc)(hwndDlg, (UINT)WM_TABSWITCH, (WPARAM)GetDlgCtrlID(hwnd), \
					(LPARAM)MAKELONG(lptsw->tswh.isel, lptsw->tswh.isel));
				lptsw->tswh.fCallOnInit = fFalse;
				}

#ifdef BITMAP_CACHE		
			hdc = BeginPaint(hwnd, &ps);
			hdcTsw = lptsw->tswh.hdc;
			hbmSav = SelectObject(hdcTsw, lptsw->tswh.hbm);
#else		
			hdcTsw = BeginPaint(hwnd, &ps);	
			clrBack = GetBkColor(hdcTsw);
#endif // BITMAP_CACHE	

			clrDlg = GetSysColor(FChiseledTsw(lptsw) ? COLOR_BTNFACE : COLOR_WINDOW);
			SetTextColor(hdcTsw, GetSysColor(COLOR_BTNTEXT));
			
			// erase background
			GetClientRect(hwnd, &rectT);
			RecOfRect(recTsw, rectT);
			
#ifdef BITMAP_CACHE
			recTsw.x= recTsw.y= 0;
#endif // BITMAP_CACHE			

			rec = recTsw;

			FillRecTsw(hdcTsw, (REC FAR *)&rec, clrDlg);

			lptswh = &(lptsw->tswh);
			rec.y = recTsw.y + IRowLastTswh(lptswh) * lptswh->dyRow;
			rec.dx = lptswh->dxtsw;
			rec.dy = lptswh->dyRow;

			// draw the rows
			dxShift = rec.x;
			iRowTsw = lptswh->cRows;
			dxShortPrev = 0;
			while (iRowTsw-- > 0)
				{
				BOOL fCur;
				XY dxShort = 0, xT, yT, dxT, dyT;
				
				for (iColTsw = 0, itsw = lptsw->tswh.rgRows[iRowTsw]; 
						iColTsw < lptsw->tswh.ctswRow; 
						iColTsw++, itsw++, rec.x += lptsw->tswh.dxtsw - 1)
					{
					if (itsw >= lptsw->ctswe)
						{
						if (lptsw->tswh.cRows == 1)
							continue;
							
						dxShort += rec.dx - 1;
						continue;
						}

					fCur = (itsw == lptsw->tswh.isel);
					DrawTabButtonBack(hdcTsw, rec.x, rec.y, rec.x + rec.dx, 
							rec.y + rec.dy, fCur, lptsw->tswh.fChiseled);
							
					if (itsw < lptsw->ctswe)
						{
						TSWE FAR *lptswe = &(lptsw->rgtswe[itsw]);
						UINT cch = lptswe->tswx.cch;
						uchar FAR *lsz = lptswe->lsz;
						POINT FAR *lppt = fCur ? &(lptswe->tswx.ptSizeIfCur)
								: &(lptswe->tswx.ptSizeNotCur);
						FPDT fpdt = fpdtPrefix | fpdtLeft;
						
						if (!fCur)
							fpdt |= fpdtLight;
							
						if (FEnabledItsw(lptsw, itsw))
							fpdt |= fpdtEnable;
							
						if (lppt->x == 0)
							GetTextExtentTsw(hdcTsw, lsz, cch, lppt, fCur);
							
						xT = rec.x + (rec.dx - (dxT = lppt->x)) / 2;
						yT = rec.y + (rec.dy - (dyT = lppt->y)) / 2 + 1;
						
						SetBkColor(hdcTsw, clrDlg);

						DxPaintText(xT, yT + 1, xT + dxT, lsz, lsz+cch, 
								fpdt, lptswe->tswx.ichAccel, fTrue, (HWND)NULL, 
								hdcTsw, GetStockObject(GRAY_BRUSH));
								
						if (fCur && lptsw->tswh.fFocus)
							InvertCaret((HWND)NULL, hdcTsw, xT-2, yT, dxT+3, dyT+2, vhbrsGray);
						}
					}

				xT = rec.x;
				yT = rec.y + rec.dy;
				for (dxT = dxShortPrev; dxT > 0; dxT -= rec.dx - 1)
					{
					xT -= rec.dx - 1;
					PolyLineTsw(hdcTsw, GetSysColor(COLOR_WINDOWFRAME), 2, xT, yT, xT, yT + 2);
							
					if (lptsw->tswh.fChiseled)
						{
						PolyLineTsw(hdcTsw, GetSysColor(COLOR_BTNSHADOW), 2,
								xT - 1, yT,
								xT - 1, yT + 2);
						PolyLineTsw(hdcTsw, GetSysColor(COLOR_BTNHIGHLIGHT), 2,
								xT + 1, yT,
								xT + 1, yT + 4);
						}
					}

				// draw the edge gap lines
				if (iRowTsw != IRowLastTswh(&(lptsw->tswh)))
					{
					recT.x = rec.x;
					recT.dx = 1;
					recT.y = rec.y + lptswh->dyRow;
					recT.dy = recTsw.dy + recTsw.y - recT.y;
					
					if (dxShort)
						recT.y += 2, recT.dy -= 2;
						
					FillRecTsw(hdcTsw, &recT, GetSysColor(COLOR_WINDOWFRAME));

					recT.x--;
					
					if (lptsw->tswh.fChiseled)
						FillRecTsw(hdcTsw, &recT, GetSysColor(COLOR_BTNSHADOW));

					if (dxShort || dxShortPrev)
						{
						recT.x -= dxTswShift + dxShortPrev;
						recT.dx = dxTswShift + dxShortPrev + 2;
						
						if (dxShortPrev)
							recT.dx -= rec.dx - 1;
							
						if (!dxShort)
							recT.y += 2;
							
						recT.dy = 1;
						FillRecTsw(hdcTsw, &recT, GetSysColor(COLOR_WINDOWFRAME));

						if (lptsw->tswh.fChiseled)
							{
							recT.y++, recT.x++, recT.dx--;
							
							if (dxShort)
								recT.dx--;
								
							FillRecTsw(hdcTsw, &recT, GetSysColor(COLOR_BTNHIGHLIGHT));
							}
						}
					}

				dxShortPrev = dxShort;
				rec.y -= lptsw->tswh.dyRow;
				dxShift += dxTswShift;
				rec.x = dxShift;
				}

			// draw the bottom line
			yLine = recTsw.y + lptsw->tswh.cRows * lptsw->tswh.dyRow;
			yBottom = recTsw.y + recTsw.dy;
			dxLine = lptsw->tswh.ctswRow * lptsw->tswh.dxtsw - lptsw->tswh.ctswRow;

			PolyLineTsw(hdcTsw, GetSysColor(COLOR_WINDOWFRAME), 4,
					recTsw.x, yBottom,
					recTsw.x, yLine,
					recTsw.x+ dxLine, yLine,
					recTsw.x+ dxLine, yBottom+1);

			if (lptsw->tswh.fChiseled)
				{
				PolyLineTsw(hdcTsw, GetSysColor(COLOR_BTNHIGHLIGHT), 4,
						recTsw.x + 1, yBottom,
						recTsw.x + 1, yLine+1,
						recTsw.x + dxLine-1, yLine+1,
						recTsw.x + dxLine-1, yBottom+1);

				PolyLineTsw(hdcTsw, GetSysColor(COLOR_BTNHIGHLIGHT), 4,
						recTsw.x + 2, yBottom,
						recTsw.x + 2, yLine+2,
						recTsw.x + dxLine-2, yLine+2,
						recTsw.x + dxLine-2, yBottom+1);
				}

			// Column # of current selection
			iColTsw = lptsw->tswh.isel - lptsw->tswh.rgRows[IRowLastTswh(&(lptsw->tswh))];

			// notch out hole for selected button
			rec.x = recTsw.x + lptsw->tswh.dxtsw * iColTsw + 1 - iColTsw;
			rec.y = yLine;
			rec.dx = lptsw->tswh.dxtsw-2;
			rec.dy = 3;
			
			if (lptsw->tswh.fChiseled)
				rec.x += 2, rec.dx -= 4;

			FillRecTsw(hdcTsw, &rec, clrDlg);

			rec.x -= 2;
			rec.dx = 2;

			if (lptsw->tswh.fChiseled)
				FillRecTsw(hdcTsw, &rec, GetSysColor(COLOR_BTNHIGHLIGHT));

			// Paint right edge stuff & hole right edge if not edge selection
			rec.x= recTsw.x + dxLine-2;
			rec.dx = 2;
			rec.dy = yBottom - yLine;

			if (lptsw->tswh.fChiseled)
				FillRecTsw(hdcTsw, &rec, GetSysColor(COLOR_BTNHIGHLIGHT));

			if (iColTsw != IColLastTswh(&(lptsw->tswh)))
				{
				// hole edge
				rec.x = recTsw.x + lptsw->tswh.dxtsw * (iColTsw + 1) - 3 - iColTsw;
				rec.dx = rec.dy = 2;
				
				if (lptsw->tswh.fChiseled)
					{
					FillRecTsw(hdcTsw, &rec, GetSysColor(COLOR_BTNHIGHLIGHT));
					SetPixel(hdcTsw, rec.x, yLine+2, GetSysColor(COLOR_BTNHIGHLIGHT));
					}

				// right edge
				dxLine += recTsw.x;
				SetPixel(hdcTsw, dxLine-1, yLine, GetSysColor(COLOR_WINDOWFRAME));
				SetPixel(hdcTsw, dxLine-2, yLine, GetSysColor(COLOR_WINDOWFRAME));
				
				if (lptsw->tswh.fChiseled)
					{
					SetPixel(hdcTsw, dxLine-1, yLine+1, GetSysColor(COLOR_BTNHIGHLIGHT));
					SetPixel(hdcTsw, dxLine-2, yLine+1, GetSysColor(COLOR_BTNHIGHLIGHT));
					SetPixel(hdcTsw, dxLine-2, yLine+2, GetSysColor(COLOR_BTNHIGHLIGHT));
					}
				}
			else if (lptsw->tswh.fChiseled)
				FillRecTsw(hdcTsw, &rec, GetSysColor(COLOR_BTNSHADOW));


		// transfer the results & cleanup
#ifdef BITMAP_CACHE		
		RecOfRect(rec, rectT);
		BitBlt(hdc, rec.x, rec.y, rec.dx, rec.dy, hdcTsw, 0, 0, SRCCOPY);
		SelectObject(hdcTsw, hbmSav);
#else
		SetBkColor(hdcTsw, clrBack);
#endif // BITMAP_CACHE

		EndPaint(hwnd, &ps);
		return fTrue;
		}

	case WM_LBUTTONDOWN:
			{
			POINT pt;
			UINT iRow, iCol;
			UINT iselNew;

			if ((lptsw = LptswFromHwndTsw(hwnd)) == lpNil)
				break;

			lptswh = &(lptsw->tswh);
			iselNew = lptswh->isel;
			
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			Assert(lptswh == &(lptsw->tswh));
			pt.x -= lptswh->rec.x;
			pt.y -= lptswh->rec.y;

			// is it below the tabs, but in the picture proc rect?
			if (pt.y >= lptswh->rec.dy)
				return fTrue;

			iRow = pt.y / lptswh->dyRow;

			// account for shifting
			pt.x -= (IRowLastTswh(lptswh) - iRow) * dxTswShift;

			// is it in the left shift gap?
			if (pt.x < 0)
				return fTrue;

			// is it in the right shift gap?
			if (pt.x > (XY)(lptswh->ctswRow * lptswh->dxtsw - lptswh->ctswRow))
				return fTrue;

			iCol = pt.x / lptswh->dxtsw;

			iselNew = lptswh->rgRows[iRow] + iCol;

			if (iselNew >= lptsw->ctswe || !FEnabledItsw(lptsw, iselNew))
				iselNew = itswNil;

			if (iselNew == itswNil)
				return fFalse;

			// Update the control
			DoTabSwitch(lptsw, iselNew, fTrue);
			return fTrue;
			}

		case WM_CHAR:		
		case WM_KEYUP:
			{
			unsigned vk = (unsigned)wParam;
			UINT iselNew = itswNil;

			if ((lptsw = LptswFromHwndTsw(hwnd)) == lpNil)
				break;

			switch (vk)
				{
				default:
					if (msg == WM_CHAR)
						iselNew = ItswAccelerate(lptsw, (UINT)vk);
					break;
				case VK_LEFT:
				case VK_UP:
				case VK_RIGHT:
				case VK_DOWN:
					iselNew = ItswHandleDirKeys(lptsw, vk);
					break;
				}

			// Hack to get Windows to process the message
			if (iselNew == itswNil)
				return fTrue;

			// Update the control
			DoTabSwitch(lptsw, iselNew, fTrue);
			return fFalse;
			}
		case WM_KILLFOCUS:
		case WM_SETFOCUS:
			{
			if ((lptsw = LptswFromHwndTsw(hwnd)) == lpNil)
				break;

			lptsw->tswh.fFocus = (msg == WM_SETFOCUS);
			
			InvalidateSwitcher(lptsw);
			return fTrue;
			}
		}
	
	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/*  T A B  B O R D E R  W N D  P R O C  */
/*-------------------------------------------------------------------------
    %%Function: TabBorderWndProc
    %%Owner:    stevebu
    %%Reviewed: 

    Window Proc for the Tab Border
    
-------------------------------------------------------------------------*/
long FAR PASCAL EXPORT TabBorderWndProc(hwnd, msg, wParam, lParam)
HWND hwnd;
UINT msg;
WPARAM wParam;
LPARAM lParam;
{
	switch (msg)
		{
		default:
			break;
		case WM_DESTROY:
			{
			LPTSW FAR *lplptsw = &vlptsw;
			int cidTswBdr = GetDlgCtrlID(hwnd);
			LPTSW lptswT;

			while ((lptswT = *lplptsw) != lpNil)
				{
				if (lptswT->tswh.cidTswBdr == cidTswBdr)
					{
#ifdef BITMAP_CACHE
					if (lptswT->tswh.hbm != (HBITMAP)NULL)
						DeleteObject(lptswT->tswh.hbm);
					if (lptswT->tswh.hdc != (HDC)NULL)
						{
 					    if (lptswT->tswh.hfntSav != (HFONT)NULL)
							SelectObject(lptswT->tswh.hdc, lptswT->tswh.hfntSav);
						DeleteDC(lptswT->tswh.hdc);
						}
#endif //!NO_CACHED_DRAW

						// Delete Tab Strings
						while (lptswT->ctswe--)
							FreeLpv(lptswT->rgtswe[lptswT->ctswe].lsz);
					
					*lplptsw = lptswT->lptswNext;
					FreeLpv(lptswT);
					break;
					}
					
				lplptsw = &lptswT->lptswNext;
				}
			
			break;
			}
		case WM_ERASEBKGND:
			{
			RECT rect;
			LPTSW lptsw = (LPTSW)LptswFromCidTswBdr(GetDlgCtrlID(hwnd));
			COLORREF clrSave;

			if (lptsw == lpNil)
				break;
				
			clrSave = SetBkColor((HDC)wParam, 
					GetSysColor(FChiseledTsw(lptsw) ? COLOR_BTNFACE : COLOR_WINDOW));

			GetUpdateRect(hwnd, &rect, fFalse /*fErase*/); 
			ExtTextOut((HDC)wParam, 0, 0, ETO_OPAQUE,
				    (LPRECT)&rect, (LPSTR)NULL, 0, (LPINT)NULL);
			SetBkColor((HDC)wParam, clrSave);
			return fTrue;
			}

		case WM_PAINT:
			{
			LPTSW lptsw = (LPTSW)LptswFromCidTswBdr(GetDlgCtrlID(hwnd));
			TSWH FAR *lptswh;
			RECT rect, rectAll;
			XY dxLine;
			UINT ic;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			COLORREF clrBack = GetBkColor(hdc);
			COLORREF clrButtonShadow = GetSysColor(COLOR_BTNSHADOW);
			REC rec;

			if (lptsw == lpNil)
			{
				EndPaint(hwnd, &ps);
				break;
			}
			
			lptswh = &(lptsw->tswh); 
			dxLine = lptswh->ctswRow * lptswh->dxtsw - lptswh->ctswRow;
			GetClientRect(hwnd, &rectAll);
			
			// draw border around front card
			rect = rectAll;

			PolyLineTsw(hdc, GetSysColor(COLOR_WINDOWFRAME), 4,
					rect.left, rect.top,
					rect.left, rect.bottom,
					rect.left + dxLine, rect.bottom,
					rect.left + dxLine, rect.top);

			InflateRect(&rect, -1, -1);
			RecOfRect(rec, rect);
			rec.dx = 2;
			if (FChiseledTsw(lptsw))
				{
				FillRecTsw(hdc, &rec, GetSysColor(COLOR_BTNHIGHLIGHT));
				SetPixel(hdc, rec.x+1, rec.y + rec.dy - 1, clrButtonShadow);
				}

			rec.x += (dxLine - 3);
			if (FChiseledTsw(lptsw))
				FillRecTsw(hdc, &rec, clrButtonShadow);

			rec.x = rectAll.left + 3;
			rec.dx = dxLine - 3;
			rec.y += rec.dy - 2;
			rec.dy = 2;
			if (FChiseledTsw(lptsw))
				FillRecTsw(hdc, &rec, clrButtonShadow);

			// draw exosed part of other cards
			rect = rectAll;
			RecOfRect(rec, rect);

			rec.y += rec.dy-1;
			rec.dy = 1;
			rec.dx = dxLine;
			
			// Draw Bottom Border
			FillRecTsw(hdc, &rec, GetSysColor(COLOR_WINDOWFRAME));
							
			rect.left += dxLine;
			ic = IRowLastTswh(&(lptsw->tswh));
			while (ic-- > 0)
				{
				rect.bottom -= dxTswShift;

				PolyLineTsw(hdc, GetSysColor(COLOR_WINDOWFRAME), 3,
						rect.left+1, rect.bottom,
						rect.left+dxTswShift, rect.bottom,
						rect.left+dxTswShift, rect.top);

				if (FChiseledTsw(lptsw))
					PolyLineTsw(hdc, clrButtonShadow, 3,
							rect.left+1, rect.bottom-1,
							rect.left+dxTswShift-1, rect.bottom-1,
							rect.left+dxTswShift-1, rect.top);

				rect.left += dxTswShift;
				}

			SetBkColor(hdc, clrBack);
			EndPaint(hwnd, &ps);
			return fTrue;
			}
		}
		
	return DefWindowProc(hwnd, msg, wParam, lParam);
}



/*  C C H  C O P Y  S T R I P  */
/*-------------------------------------------------------------------------
    %%Function: CchCopyStrip
    %%Owner:    stevebu
    %%Reviewed: 

	Copys lsz into lpch and returns ichAccel in pichAccel.
	
-------------------------------------------------------------------------*/
unsigned int CchCopyStrip(lsz, lpch, lpichAccel)
uchar FAR *lsz;
uchar FAR *lpch;
unsigned int FAR *lpichAccel;		// Index of accel escape.
{
	uchar FAR *lpchStart = lpch;

	// Copy string and strip accelerate escapes if fStrip.
	if (lpichAccel != NULL)
		{
		*lpichAccel = (UINT)-1;
		for (;;)
			{
			if (*lsz == chAccelerator)
				{
				if (*lpichAccel == (UINT)-1 && lsz[1] != chAccelerator)
					*lpichAccel = (UINT)(lpch - lpchStart);
					
				lsz++;
				}

			// Kludge alert:
			//	We can't have (*pch++ = *osz++) without
			//	getting a "please simplify" bug from CSL.
			if ((*lpch = *lsz++) == '\0')
				break;
			lpch++;
			}
		}
	else
		{
		for (;;)
			{
			if ((*lpch = *lsz++) == '\0')
				break;
			lpch++;
			}
		}
	return (UINT)(lpch - lpchStart);
}


/* L P T S W  A D D  T S W */
/*---------------------------------------------------------------------------
	%%Function: LptswAddTsw
	%%Contact: stevebu

	Add a tab and it's accelerator (if it is directly accelerated)
	information to the control.

---------------------------------------------------------------------------*/
LPTSW LptswAddTsw(hwndTsw, lptsw, fEnabled, lsz)
HWND hwndTsw;
LPTSW lptsw;
BOOL fEnabled;
uchar FAR *lsz;
{
	UINT cb = CbLsz(lsz);
	uchar FAR *lszNew;
	TSWH FAR *lptswh;
	TSWE FAR *lptswe;
	TSWX FAR *lptswx;
	
	if ((lptsw = LpvReallocCb(lptsw, CbTswCtswe(lptsw->ctswe + 1), GMEM_MOVEABLE | GMEM_ZEROINIT)) == lpNil)
		return lpNil;
		
	if ((lszNew = (uchar FAR *)LpvAllocCb(cb, GMEM_MOVEABLE)) == lpNil)
		return lpNil;
		
	lptswh = &(lptsw->tswh);
	lptswe = &(lptsw->rgtswe[lptsw->ctswe++]);
	lptswx = &(lptswe->tswx);

	lptswe->lsz = lszNew;
	
	if (lptswh->fDirectAccel)
		lptswx->cch = CchCopyStrip(lsz, lszNew, &lptswx->ichAccel);
	else
		{
		LpbCopyLprgb(lsz, lszNew, cb);
		lptswx->ichAccel = (UINT)(-1);
		lptswx->cch = cb - 1;
		}

	lptswx->fEnabled = fEnabled;
	lptswx->ptSizeIfCur.x = 0;	//these will be calc'd later
	lptswx->ptSizeNotCur.x = 0;

	return lptsw;
}


/*  I N V A L I D A T E  S W I T C H E R  */
/*-------------------------------------------------------------------------
    %%Function: InvalidateSwitcher
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
VOID InvalidateSwitcher(lptsw)
LPTSW lptsw;
{
	RECT rect;
	
	GetClientRect(HwndTswFromLptsw(lptsw), &rect);
	InvalidateRect(HwndTswFromLptsw(lptsw), &rect, fFalse /*fErase*/);
}


/*  S E T  S W I T C H E R  I S E L  */
/*-------------------------------------------------------------------------
    %%Function: SetSwitcherIsel
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
VOID SetSwitcherIsel(lptsw, isel)
LPTSW lptsw;
UINT isel;
{
	lptsw->tswh.isel = isel;
	RotateIselRowToBottom(&lptsw->tswh);
	InvalidateSwitcher(lptsw);
}


/* I  R O W  F I N D  I T S W */
/*---------------------------------------------------------------------------
	%%Function: IRowFindItsw
	%%Contact: stevebu

	Find the row that isel is on.

---------------------------------------------------------------------------*/
UINT IRowFindItsw(lptswh, itsw)
TSWH FAR *lptswh;
UINT itsw;
{
	UINT iRow;

	for (iRow = 0; iRow < lptswh->cRows; iRow++)
		{
		UINT itswT = lptswh->rgRows[iRow];
		if (FBetween(itsw, itswT, itswT + lptswh->ctswRow - 1))
			break;
		}

	return iRow;
}

/* R O T A T E  I S E L  R O W  T O  B O T T O M */
/*---------------------------------------------------------------------------
	%%Function: RotateIselRowToBottom
	%%Contact: stevebu

	The row with the selection must be on the bottom.  Rotate the row
	array to meet this criteria.

---------------------------------------------------------------------------*/
VOID RotateIselRowToBottom(lptswh)
TSWH FAR *lptswh;
	{
	UINT iRow, iRowIsel, dRot;
	UINT rgRowsT[cRowsMaxTsw];

	// One row case doesn't need rotation
	if (lptswh->cRows == 1)
		return;

	iRowIsel = IRowFindItsw(lptswh, lptswh->isel);

	// Already on bottom
	if (iRowIsel == IRowLastTswh(lptswh))
		return;

	// Rotate to bottom
	LpbCopyLprgb(lptswh->rgRows, rgRowsT, sizeof(rgRowsT));
	dRot = IRowLastTswh(lptswh) - iRowIsel;
	for (iRow = 0; iRow < lptswh->cRows; iRow++)
		{
		if (iRow + dRot > IRowLastTswh(lptswh))
			lptswh->rgRows[iRow + dRot - lptswh->cRows] = rgRowsT[iRow];
		else
			lptswh->rgRows[iRow + dRot] = rgRowsT[iRow];
		}
	}


/* I T S W  H A N D L E  D I R  K E Y S */
/*---------------------------------------------------------------------------
	%%Function: ItswHandleDirKeys
	%%Contact: stevebu

	Return the new itsw based on the direction key pressed.

---------------------------------------------------------------------------*/
UINT ItswHandleDirKeys(lptsw, vk)
LPTSW lptsw;
unsigned vk;
	{
	TSWH FAR *lptswh = &(lptsw->tswh);
	UINT iRowIsel;
	UINT itsw;

	itsw = lptswh->isel;
	do //while (out-o'-range or !FEnabled(itsw))
		{
		iRowIsel = IRowFindItsw(lptswh, itsw);
	
		switch (vk)
			{
		case VK_LEFT:
			if (lptswh->rgRows[iRowIsel] != itsw)
				itsw--;
			else // Wrap
				itsw = lptswh->rgRows[iRowIsel] + IColLastTswh(lptswh);
			break;

		case VK_UP:
			if (iRowIsel != 0)
				{
				itsw = lptswh->rgRows[iRowIsel-1] +
					(itsw - lptswh->rgRows[iRowIsel]);
				}
			else // Wrap
				{
				itsw = lptswh->rgRows[IRowLastTswh(lptswh)] +
					(itsw - lptswh->rgRows[iRowIsel]);
				}
			break;

		case VK_RIGHT:
			if (lptswh->rgRows[iRowIsel] + lptswh->ctswRow - 1 != itsw)
				itsw++;
			else // Wrap
				itsw = lptswh->rgRows[iRowIsel];
			break;

		case VK_DOWN:
			if (iRowIsel != IRowLastTswh(lptswh))
				{
				itsw = lptswh->rgRows[iRowIsel+1] +
					(itsw - lptswh->rgRows[iRowIsel]);
				}
			else // Wrap
				{
				itsw = lptswh->rgRows[0] +
					(itsw - lptswh->rgRows[iRowIsel]);
				}
			break;
			}
		}
	while (itsw >= lptsw->ctswe || !FEnabledItsw(lptsw, itsw)); //end do

	return itsw;
}

/* C H  U P P E R */
/*----------------------------------------------------------------------------
	%%Function: ChUpper
	%%Contact: stevebu

	return uppercase ch

----------------------------------------------------------------------------*/
int ChUpper(ch)
uchar ch;
{
	register int wch = (int) ch;

	if (FBetween(wch, 'a', 'z'))
		return wch - ('a' - 'A');
	else if (wch < 128)
		return wch;

	return (int)((long) AnsiUpper((LPSTR)ch));
}

/* C H  A C C E L  F R O M  I T S W */
/*---------------------------------------------------------------------------
	%%Function: ChAccelFromItsw
	%%Contact: stevebu

	Return the accelerator key for the given itsw.

---------------------------------------------------------------------------*/
UINT ChAccelFromItsw(lptsw, itswe)
LPTSW lptsw;
UINT itswe;
{
	TSWH FAR *lptswh = &(lptsw->tswh);
	TSWE FAR *lptswe = &(lptsw->rgtswe[itswe]);
	
	return (lptswh->fDirectAccel)
			? ((lptswe->tswx.ichAccel == (UINT)(-1)) ? 0 
				: ChUpper(lptswe->lsz[lptswe->tswx.ichAccel]))
			: lptswe->lsz[0];
}


/* I T S W  A C C E L E R A T E */
/*---------------------------------------------------------------------------
	%%Function: ItswAccelerate
	%%Contact: stevebu

	Return the next itsw after isel that's first character matches the
	passed in character, itswNil if no match.

---------------------------------------------------------------------------*/
UINT ItswAccelerate(lptsw, ch)
LPTSW lptsw;
UINT ch;
{
	TSWH FAR *lptswh = &(lptsw->tswh);
	UINT ctsw = lptsw->ctswe;
	UINT itsw;

	for (itsw = lptswh->isel + 1; itsw < ctsw; itsw++)
		if (ChAccelFromItsw(lptsw, itsw) == (unsigned int)ChUpper(ch)
				&& FEnabledItsw(lptsw, itsw))
			return itsw;

	for (itsw = 0; itsw <= lptswh->isel; itsw++)
		if (ChAccelFromItsw(lptsw, itsw) == (unsigned int)ChUpper(ch)
				&& FEnabledItsw(lptsw, itsw))
			return itsw;

	return itswNil;
}



/*  P O L Y  L I N E  T S W  */
/*-------------------------------------------------------------------------
    %%Function: PolyLineTsw
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
VOID CDECL PolyLineTsw(hdc, clr, cpt, x1, y1, x2, y2, ...)
HDC hdc;
COLORREF clr;
UINT cpt;
int x1, y1, x2, y2;
	{
	HPEN hpen = CreatePen(PS_SOLID, 1, clr);
	HPEN hpenSav = SelectObject(hdc, hpen);
	Polyline(hdc, (LPPOINT)&x1, cpt);
	SelectObject(hdc, hpenSav);
	if (hpen != (HPEN)NULL)
		DeleteObject(hpen);
	}

/*  D R A W  T A B  B U T T O N  B A C K  */
/*-------------------------------------------------------------------------
    %%Function: DrawTabButtonBack
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
VOID DrawTabButtonBack(hdc, xLeft, yTop, xRight, yBottom, fThickEdges, fChiseled)
HDC hdc;
int xLeft, yTop, xRight, yBottom;
short fThickEdges;
short fChiseled;
{
	COLORREF clrFrameTsw = GetSysColor(COLOR_WINDOWFRAME);
	COLORREF clrButtonShadow = GetSysColor(COLOR_BTNSHADOW);
	COLORREF clrButtonReflection = GetSysColor(COLOR_BTNHIGHLIGHT);
	
	yBottom--;
	xRight--;

	// Border
	PolyLineTsw(hdc, clrFrameTsw, 6, 
			xLeft, yBottom,
			xLeft, yTop + dyCorner,
			xLeft + dxCorner, yTop,
			xRight - dxCorner, yTop,
			xRight, yTop + dyCorner,
			xRight, yBottom + 1);

	if (fChiseled)
		{
		// Hilight
		PolyLineTsw(hdc, clrButtonReflection, 4, 
				xLeft + 1, yBottom,
				xLeft + 1, yTop + dyCorner,
				xLeft + dxCorner, yTop + 1,
				xRight - dxCorner + 1, yTop + 1);
		// Shadow
		PolyLineTsw(hdc, clrButtonShadow, 3, 
				xRight - dxCorner + 1, yTop + 2,
				xRight - 1, yTop + dyCorner,
				xRight - 1, yBottom + 1);

		// Thick hilight
		if (fThickEdges)
			{
			PolyLineTsw(hdc, clrButtonReflection, 4, 
					xLeft + 2, yBottom,
					xLeft + 2, yTop + dyCorner,
					xLeft + dxCorner, yTop + 2,
					xRight - dxCorner + 1, yTop + 2);
			PolyLineTsw(hdc, clrButtonReflection, 2, 
					xLeft + 2, yTop + dyCorner + 1,
					xLeft + dxCorner + 1, yTop + 2);
			}

		// Thick shadow
		if (fThickEdges)
			{
			PolyLineTsw(hdc, clrButtonShadow, 3, 
					xRight - dxCorner + 1, yTop + 2,
					xRight - 2, yTop + dyCorner + 1,
					xRight - 2, yBottom + 1);
			PolyLineTsw(hdc, clrButtonShadow, 2, 
					xRight - dxCorner, yTop + 2,
					xRight - 3, yTop + dyCorner + 1);
			}
		}
}


/*  G E T  T E X T  E X T E N T  T S W  */
/*-------------------------------------------------------------------------
    %%Function: GetTextExtentTsw
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
VOID GetTextExtentTsw(hdc, lsz, cb, lppt, fSel)
HDC hdc;
uchar FAR *lsz;
int cb;
POINT FAR *lppt;
BOOL fSel;
{
	HFONT hfnt = (fSel ? vhfnt : vhfntLight);
	
	if (hfnt != (HFONT)NULL)
		hfnt = SelectObject(hdc, hfnt);
#ifdef WIN32
	GetTextExtentPoint32(hdc, (LPCSTR)lsz, (int)cb, (LPSIZE)lppt);
#else
	*(DWORD FAR *)lppt = GetTextExtent(hdc, (LPCSTR)lsz, (int)cb);
#endif
	
	if (hfnt != (HFONT)NULL)
		SelectObject(hdc, hfnt);
}


/*  D O  T A B  S W I T C H  */
/*-------------------------------------------------------------------------
    %%Function: DoTabSwitch
    %%Owner:    stevebu
    %%Reviewed: 
    
-------------------------------------------------------------------------*/
VOID DoTabSwitch(lptsw, iselNew, fFocus)
LPTSW lptsw;
UINT iselNew;
BOOL fFocus;
{
	if (fFocus && !lptsw->tswh.fFocus)
		SetFocus(HwndTswFromLptsw(lptsw));
		
	if (iselNew != lptsw->tswh.isel)
		{
		HWND hwndDlg = GetParent(lptsw->tswh.hwndTsw);
		FARPROC lpfnDlgProc = (FARPROC)GetWindowLong(hwndDlg, DWL_DLGPROC);
		LONG l = MAKELONG(lptsw->tswh.isel, iselNew);

		Assert(IselNewFromLParam(l) == iselNew);
		Assert(IselOldFromLParam(l) == lptsw->tswh.isel);
		lptsw->tswh.isel = iselNew;
		RotateIselRowToBottom(&(lptsw->tswh));

		// Call-back user with tab switch
		(*lpfnDlgProc)(hwndDlg, (UINT)WM_TABSWITCH, (WPARAM)GetDlgCtrlID(lptsw->tswh.hwndTsw), (LPARAM)l);
		InvalidateSwitcher(lptsw);
		}
}

/*  T A B  S W I T C H  */
/*-------------------------------------------------------------------------
    %%Function: TabSwitch
    %%Owner:    stevebu
    %%Reviewed: 

    Switches tabs to itswNew,  If fFocus, it sets the focus to the tab
    switcher.  CidTsw is the contol id of the BORDER tab switcher control.
-------------------------------------------------------------------------*/
VOID TabSwitch(cidTswBdr, itswNew, fFocus)
UINT cidTswBdr;
UINT itswNew;
BOOL fFocus;
{
	LPTSW lptsw;

	if ((lptsw = (LPTSW)LptswFromCidTswBdr(cidTswBdr)) != lpNil)
		DoTabSwitch(lptsw, itswNew, fFocus);
}                                       

              
/*  G E T  C U R R E N T  T A B  */
/*-------------------------------------------------------------------------
    %%Function: GetCurrentTab
    %%Owner:    stevebu
    %%Reviewed: 

    Returns the current tab given the control ID of a tab switcher border
    control.
-------------------------------------------------------------------------*/              
UINT GetCurrentTab(cidTswBdr)
UINT cidTswBdr;
{
	LPTSW lptsw;

	if ((lptsw = (LPTSW)LptswFromCidTswBdr(cidTswBdr)) != lpNil)
		return (lptsw->tswh.isel);
		
	return 0;
}


/*  F  T A B  S W I T C H E R  A C C E L  */
/*-------------------------------------------------------------------------
    %%Function: FTabSwitcherAccel
    %%Owner:    stevebu
    %%Reviewed: 

    Callback to handle accelerator messages for the tab switcher. Returns
    TRUE(1) if we switched tabs.
    
-------------------------------------------------------------------------*/
short FTabSwitcherAccel(hwndDlg, ch)
HWND hwndDlg;
WPARAM ch;
{
	UINT itswNew;
	LPTSW lptsw = vlptsw;

	while (lptsw)
		{
		if (GetParent(lptsw->tswh.hwndTsw) == hwndDlg &&
			(itswNew = ItswAccelerate(lptsw, ch)) != itswNil)
			{
			DoTabSwitch(lptsw, itswNew, fTrue);
			return fTrue;
			}
			
		lptsw = lptsw->lptswNext;
		}
		
	return fFalse;
}


// DEBUG ROUTINES

#ifdef DEBUG

/* C C H  L O N G  T O  P L P C H */
/*----------------------------------------------------------------------------
	%%Function: CchLongToPlpch
	%%Contact: stevebu

	Convert a long int into a string using radix wBase. The resulting
	number will have at least cchDigitMin digits (should be <= 32).
	Negative numbers will be preceded by a minus sign. The number is
	stored at *plpch and *plpch is updated to the end of the number.

----------------------------------------------------------------------------*/
int CchLongToPlpch(lw, plpch, cchDigitMin, wBase)
long lw;
uchar FAR **plpch;
int cchDigitMin;
int wBase;
{
	int w, cch;
	uchar *pch;
	int fNeg;
	uchar rgch[33];	/* big enough to hold base-2 number plus sign */

	Assert(FInRange(wBase, 2, 36));
	Assert(cchDigitMin <= 32);
	fNeg = lw < 0;
	if (fNeg)
		lw = -lw;
	for (cch = 0, pch = &rgch[32]; lw > 0 || cch < cchDigitMin > 0; cch++)
		{
		Assert(cch <= 31);
		w = (int) (lw % wBase);
		*pch-- = (w < 10) ? (uchar) ('0' + w) : (uchar) ('A' + (w - 10));
		lw /= wBase;
		}
	if (fNeg)
		{
		*pch-- = '-';
		cch++;
		}
	*plpch = LpbCopyLprgb(pch + 1, *plpch, cch);
	return(cch);
}

/* A S S E R T  S Z  P R O C  */
/*----------------------------------------------------------------------------
	%%Function: AssertSzProc
	%%Contact: stevebu

	DEBUG Assert Function used to test validality before a piece of code is
	exercised.

----------------------------------------------------------------------------*/
int AssertSzProc(lszMsg, lszFile, line)
uchar FAR *lszMsg;
uchar FAR *lszFile;
int line;
{
	HWND vhwndParent = NULL;
	uchar sz[cbMaxSz];
	uchar FAR *lpch;

	AssertSz(CchLsz(lszMsg) < cchMaxSz, "lszMsg Too Long");
	AssertSz(CchLsz(lszMsg) + CchLsz(lszFile) + 8 < cchMaxSz, "lszMsg Too Long");

	lpch = LpbCopyLprgb(lszMsg, sz, CchLsz(lszMsg));
	*lpch++ = '\n';
	lpch = LpbCopyLprgb(lszFile, lpch, CchLsz(lszFile));
	*lpch++ = ',';
	*lpch++ = ' ';
	
	// Convert Line Number to String
	CchLongToPlpch((long)line, &lpch, 1, 10);
	*lpch = '\0';
		
	switch (MessageBox(vhwndParent, sz, "Assertion Failure", MB_ABORTRETRYIGNORE))
		{
		default:
			break;
		case IDABORT:
//			TerminateApp(NULL, NO_UAE_BOX);
			return fFalse;
		case IDRETRY:
			DebugBreak();
			break;
		}
		
	return fTrue;
}
#endif // DEBUG
