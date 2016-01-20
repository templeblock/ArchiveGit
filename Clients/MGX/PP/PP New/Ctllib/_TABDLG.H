/*-------------------------------------------------------------------------
    %%File:  _tabdlg.h
    %%Owner: stevebu

    Private declarations for tabdlg.h
    
-------------------------------------------------------------------------*/
#include "tools.h"
#include "tabdlg.h"

// Hungarian Definitions
#define fTrue	1
#define fFalse	0

#define cbMaxSz	256
#define cchMaxSz 255
#define dxTswShift 10
#define dxCorner 4
#define dyCorner dxCorner
#define intMax (0x7fff)


typedef unsigned int uint;
typedef unsigned char uchar;
typedef VOID *PPV;

// Flags for DxDrawText
typedef	UINT	FPDT;
#define	fpdtNull	((FPDT)0x0000)	// Use defaults.
#define fpdtEnable	((FPDT)0x0004)	// Paint as enabled.
#define	fpdtErase	((FPDT)0x0008)	// Erase before painting.
#define fpdtPrefix	((FPDT)0x0020)	// Remove accelerator escape and draw
					// underscore.
#define	fpdtMultiLine	((FPDT)0x0040)	// print text on multiple lines,
					// break on word breaks
#define	fpdtExpandTabs	((FPDT)0x0080)	// expand tabs before calling TextOut
#define	fpdtEraseText	((FPDT)0x0100)	// Erase before text area painting.
#define fpdtLight		((FPDT)0x0200)	// Use light font (dac.hfontLight)
#define fpdtLeft	((FPDT)0x0400)
#define fpdtCenter	((FPDT)0x0800)
#define fpdtRight	((FPDT)0x1000)


#define hNil ((PPV) 0)

#define dxTswShift 10
#define dxCorner 4
#define dyCorner dxCorner
#define cRowsMaxTsw 4
#define cRowsAutoTsw (-1)

typedef struct _REC
	{
	int	x;
	int	y;
	int	dx;
	int	dy;
	} REC;	

#define RecOfRect(P1, P2)						\
	{								\
	(P1).x = ((P2).left);					\
	(P1).dx = ((P2).right - (P2).left);			\
	(P1).dy = ((P2).bottom - (P2).top);			\
	(P1).y = ((P2).top);					\
	}

#define	RectOfRec(P1, P2)						\
	{								\
	(P1).left = (P2).x;						\
	(P1).top = (P2).y;						\
	(P1).right = (P2).x + (P2).dx;					\
	(P1).bottom = (P2).y + (P2).dy;					\
	}
	
typedef struct _TSWH				// Tab SWitcher Header
	{
	HWND hwndTsw;					// Window Handle of Tab Switcher
	int dyRow;						// Height of a row
	int dxtsw;						// Width of a single tab
	UINT cRows;						// # of rows
	UINT ctswRow;					// # of tabs per row
	UINT isel;						// Current selection
	UINT rgRows[cRowsMaxTsw];		// Row layout
	HBITMAP hbm;					// Bitmap to draw into
	HDC hdc;						// Compatible dc for bitmap
	HFONT hfntSav;					// Original font in DC
	int cidTswBdr;					// Control ID of Tab Switcher Border Window
	REC rec;						// RC of Tab Switcher
	BF fOnlyOne : 1;				// only allow selection, others gray
	BF fDirectAccel : 1;			// tabs have direct accelerators
	BF fFocus : 1;					// control has the focus
	BF fValidate : 1;				// validate the dialog before switching
	BF fChiseled : 1;				// Use 3D dialog looks
	BF fCallOnInit: 1;				// Callback with WM_TABSWITCH on init
	BF fSpare		:10;
	} TSWH;


typedef struct _TSWX				// Tab SWitcher eXtra
	{
	POINT ptSizeIfCur;			// text extent when selected
	POINT ptSizeNotCur;			// text extent when not selected
	UINT  cch;						// length of text
	BF fEnabled;					// tab is enabled
	UINT ichAccel;					// index of accelerator char
	} TSWX;


typedef struct _TSWE				// Tab SWitcher Entry
	{
	unsigned char FAR *lsz;		// text
	TSWX tswx;						// other info
	} TSWE;


typedef struct _TSW				// Tab SWitcher info
	{
	struct _TSW FAR *lptswNext;
	TSWH tswh;
	UINT ctswe;
	TSWE rgtswe[1];
	} TSW;

typedef struct _TSW FAR *LPTSW;

#define CbTswCtswe(ctswe) (sizeof(TSW) + (ctswe) * sizeof(TSWE) - sizeof(TSWE))
#define IselGetSwitcher(lptsw) ((lptsw)->tswh.isel)
#define FEnabledItsw(lptsw, itsw) ((lptsw)->rgtswe[itsw].tswx.fEnabled)
#define LptswFromHwndTsw(hwndTsw) (LptswFromTsw((hwndTsw), cidNull))
#define LptswFromCidTswBdr(cidTswBdr) (LptswFromTsw(NULL, (cidTswBdr)))
#define HwndTswFromLptsw(lptsw) ((lptsw)->tswh.hwndTsw)
#define FChiseledTsw(lpstw) ((lpstw)->tswh.fChiseled)

// Function Prototypes
VOID RotateIselRowToBottom(TSWH FAR *);
VOID CDECL PolyLineTsw(HDC, COLORREF, UINT, int, int, int, int, ...);
VOID FillRecTsw(HDC, REC FAR *, COLORREF);
LPTSW LptswAddTsw(HWND hwnd, LPTSW lptsw, BOOL fEnabled, uchar FAR *lsz);
VOID GetTextExtentTsw(HDC hdc, uchar FAR *lsz, int cb, POINT FAR *lppt, BOOL fSel);
VOID DrawTabButtonBack(HDC hdc, int xLeft, int yTop, int xRight, int yBottom, short fThickEdges, short fChiseled);
BOOL CALLBACK StringOut(HDC hdc, LPARAM l, int cch);
VOID DoTabSwitch(LPTSW lptsw, UINT iselNew, BOOL fFocus); 
UINT ItswAccelerate(LPTSW lptsw, UINT ch);
UINT ItswDirectlyAccel(LPTSW lptsw, UINT ch);
UINT ItswHandleDirKeys(LPTSW lptsw, unsigned vk);
VOID InvalidateSwitcher(LPTSW lptsw);
LPTSW LptswInitHwndTsw(HWND hwndTsw);
int FStoreLptsw(LPTSW lptsw, HWND hwndTsw);
LPTSW LptswFromTsw(HWND hwndTsw, int cidTswBdr);
unsigned int CchCopyStrip(uchar FAR *lsz, uchar FAR *lpch, unsigned int FAR *lpichAccel);
VOID InvertCaret(HWND hwnd, HDC hdc, XY x, XY y, XY dx, XY dy, HBRUSH hbrsGray);

