//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯				  
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _APP_H_
#define _APP_H_

/* void Cls_OnAddRecall(HWND hwnd, LPTSTR lpFileName) */
#define HANDLE_WM_ADDRECALL(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (LPTSTR)lParam), 0L)
#define FORWARD_WM_ADDRECALL(hwnd, lpFileName, fn) \
    (void)(fn)((hwnd), WM_ADDRECALL, 0, (LPARAM)lpFileName)
#define FORWARD_WM_TASKMANAGE(hwnd, idMsg, lpThreadData, fn) \
    ((fn)((hwnd), WM_TASKMANAGE, (WPARAM)idMsg, (LPARAM)lpThreadData))

/* void Cls_OnMessageStatus(HWND hwnd, ITEMID idMsg, LPTR lpArguments) */
#define HANDLE_WM_MESSAGESTATUS(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (ITEMID)wParam, (LPTR)lParam), 0L)
#define FORWARD_WM_MESSAGESTATUS(hwnd, idMsg, lpArguments, fn) \
    (BOOL)(fn)((hwnd), WM_MESSAGESTATUS, (WPARAM)idMsg, (LPARAM)lpArguments)
/* BOOL Cls_OnProcessingDone(HWND hwnd, LPCMDLIST lpCmdList) */
#define HANDLE_WM_PROCESSINGDONE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (LPCMDLIST)lParam))
#define FORWARD_WM_PROCESSINGDONE(hwnd, lpCmdList, fn) \
    (BOOL)(fn)((hwnd), WM_PROCESSINGDONE, 0, (LPARAM)lpCmdList)
#define HANDLE_WM_MESSAGEBOX(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (LPCTSTR)wParam, (UINT)lParam))
#define HANDLE_WM_GETMESSAGEPARENT(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd))
#define HANDLE_WM_PROMPTFORFILENAME(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (LPTSTR)wParam, (LPTSTR)lParam))
#define HANDLE_WM_STATUSMSG(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (LPCTSTR)wParam), 0L)

/* void Cls_OnSetState(HWND hwnd, BOOL fState) */
#define HANDLE_BM_SETSTATE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
/* void Cls_OnSetCheck(HWND hwnd, BOOL fCheck) */
#define HANDLE_BM_SETCHECK(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
/* BOOL Cls_OnGetCheck(HWND hwnd) */
#define HANDLE_BM_GETCHECK(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(hwnd), 0L)

#ifndef WINDOWS95_HACK
#define BM_GETCHECK16	    (WM_USER+0)
#define BM_SETCHECK16	    (WM_USER+1)
#define BM_GETSTATE16	    (WM_USER+2)
#define BM_SETSTATE16	    (WM_USER+3)

/* void Cls_OnSetState(HWND hwnd, BOOL fState) */
#define HANDLE_BM_SETSTATE16(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
/* void Cls_OnSetCheck(HWND hwnd, BOOL fCheck) */
#define HANDLE_BM_SETCHECK16(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
/* BOOL Cls_OnGetCheck(HWND hwnd) */
#define HANDLE_BM_GETCHECK16(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(hwnd), 0L)
#endif

/* BOOL Cls_OnDragQuery(HWND hwnd, WORD wState, int x, int y) */
#define HANDLE_WM_DRAGQUERY(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(hwnd, (WORD)wParam, (int)LOWORD(lParam), (int)HIWORD(lParam)), 0L)
/* BOOL Cls_OnDragAccept(HWND hwnd, LPDRAG lpDrag) */
#define HANDLE_WM_DRAGACCEPT(hwnd, wParam, lParam, fn) \
    MAKELRESULT((BOOL)(fn)(hwnd, (LPDRAG)lParam), 0L)
/* LONG Cls_OnImageSpace(HWND hwnd) */
#define HANDLE_WM_IMAGESPACE(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd))
/* COLOR Cls_OnGetColor(HWND hwnd, BOOL fAlternate) */
#define HANDLE_WM_GETCOLOR(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)(hwnd, (BOOL)wParam))
/* void Cls_OnSetControl(HWND hWindow, int nIndex, long lValue) */
#define HANDLE_WM_SETCONTROL(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (int)wParam, (long)lParam), 0L)
/* void Cls_OnDocActivated(HWND hWindow) */
#define HANDLE_WM_DOCACTIVATED(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
/* void Cls_OnDocChanged(HWND hWindow) */
#define HANDLE_WM_DOCCHANGED(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
#define SEND_WM_DOCCHANGED(hWnd) \
	::SendMessage(hWnd, WM_DOCCHANGED, 0, 0);
#define POST_WM_DOCCHANGED(hWnd) \
	::PostMessage(hWnd, WM_DOCCHANGED, 0, 0);
/* void Cls_OnShieldChange(HWND hWindow) */
#define HANDLE_WM_SHIELDCHANGE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd), 0L)
/* void Cls_OnControlEnable(HWND hWindow, int fFlags) */
#define HANDLE_WM_CONTROLENABLE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (int)wParam), 0L)
#define SEND_WM_CONTROLENABLE(hwnd, fFlags) \
	::SendMessage(hwnd, WM_CONTROLENABLE, (WPARAM)fFlags, 0L);
/* void Cls_OnSelectPalette(HWND hWindow, INT16 iSelect) */
#define HANDLE_WM_SELECTPALETTE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (INT16)wParam), 0L)
/* void Cls_OnSetPalette(HWND hWindow, LPPALETTE lpPalette, INT16 iSelect) */
#define HANDLE_WM_SETPALETTE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (LPPALETTE)lParam, (INT16)wParam), 0L)
/* void Cls_OnCtlSetup(HWND hWindow, int msg, UINT32 Data) */
#define HANDLE_WM_CTLSETUP(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (int)wParam, (UINT32)lParam), 0L)
// void Cls_OnXFERDone(HWND hWnd, LPVOID lpVoid, int DataType)
#define HANDLE_PM_XFERDONE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (LPVOID)lParam, (int)wParam), 0L)
#define HANDLE_DLGRET(hwnd, message, fn)    \
    case (message): return (BOOL)HANDLE_##message((hwnd), (wParam), (lParam), (fn))
#define HANDLE_DLGMSG(hwnd, message, fn)    \
    case (message): HANDLE_##message((hwnd), (wParam), (lParam), (fn)); return TRUE;
#define MONITOR_DLGMSG(hwnd, message, fn)    \
    case (message): HANDLE_##message((hwnd), (wParam), (lParam), (fn)); return FALSE;
#define SEND_WM_COMMAND(hwnd, id, hwndCtl, codeNotify) \
	FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, ::SendMessage);
/* int Cls_OnMCINotify(HWND hwnd, int cmd, UINT idDevice) */
#define HANDLE_MM_MCINOTIFY(hwnd, wParam, lParam, fn) \
    MAKELRESULT((int)(fn)(hwnd, (int)wParam, (UINT)LOWORD(lParam)), 0L)
#define HANDLE_WM_TASKMANAGE(hwnd, wParam, lParam, fn)											\
	((fn)(hwnd, (LPTHREADDATA)lParam, (TM_OPCODE)wParam))

// WIN16/WIN32 portable macros
#ifdef WIN32
#define HANDLE_CONTROL_COLORS(hDlg, fn)											\
	case WM_CTLCOLORMSGBOX:															\
	return (BOOL)HANDLE_WM_CTLCOLORMSGBOX(hDlg, wParam, lParam, fn);	\
	case WM_CTLCOLOREDIT:															\
	return (BOOL)HANDLE_WM_CTLCOLOREDIT(hDlg, wParam, lParam, fn);	\
	case WM_CTLCOLORLISTBOX:														\
	return (BOOL)HANDLE_WM_CTLCOLORLISTBOX(hDlg, wParam, lParam, fn);	\
	case WM_CTLCOLORBTN:															\
	return (BOOL)HANDLE_WM_CTLCOLORBTN(hDlg, wParam, lParam, fn);	\
	case WM_CTLCOLORDLG:															\
	return (BOOL)HANDLE_WM_CTLCOLORDLG(hDlg, wParam, lParam, fn);	\
	case WM_CTLCOLORSCROLLBAR:														\
	return (BOOL)HANDLE_WM_CTLCOLORSCROLLBAR(hDlg, wParam, lParam, fn);	\
	case WM_CTLCOLORSTATIC:															\
	return (BOOL)HANDLE_WM_CTLCOLORSTATIC(hDlg, wParam, lParam, fn);
#else
#define HANDLE_CONTROL_COLORS(hDlg, fn)							\
   case WM_CTLCOLOR:																\
	return (BOOL)HANDLE_WM_CTLCOLOR(hDlg, wParam, lParam, fn);
#define GET_WM_COMMAND_ID(wp, lp)               (int)(wp)
#define GET_WM_COMMAND_HWND(wp, lp)             (HWND)LOWORD(lp)
#define GET_WM_COMMAND_CMD(wp, lp)              (UINT)HIWORD(lp)
#endif

// App specific
#define MIN_WINDOW_SIZE 20
#define MAX_HUES	252
#define SMALL_MOVEMENT	4
#define TEXTCOLOR(n) ((n > 127) ? RGB(0,0,0): RGB(255,255,255))
#define GETGRAY(n) RGB(n,n,n)
#define TOANGLE(n)	((int)( ((359L * (n)) + 125) / 251 ))
#define FROMANGLE(n)	((int)( ((251L * (n)) + 179) / 359 ))
#define TOPERCENT(n)	((int)( ((100L * (n)) + 127) / 255 ))
#define TOGRAY(n)	((int)( ((255L * (n)) +  50) / 100 ))
#define PIX2DISP(n)	(Control.DisplayPixelValue ? (n) : TOPERCENT(n))
#define GRAY2DISP(n) (Control.DisplayPixelValue ? (n) : 100-TOPERCENT(n))
#define DISP2PIX(n)	(Control.DisplayPixelValue ? (n) : TOGRAY(n))
#define MAXPIXDISP (Control.DisplayPixelValue ? 255 : 100)
#define TOTRANSPARENCY(Opacity) (TOPERCENT(255-(Opacity)))
#define TOOPACITY(Transparency) (255-TOGRAY(Transparency))
#define ISCOLOR(Type) ((Type) != FDT_LINEART && (Type) != FDT_GRAYSCALE)
#define CANDOTRANSPARENCY(Type) ((Type) != FDT_LINEART && (Type) != FDT_PALETTECOLOR)
#define CANDOMERGEMODE(Type) ((Type) != FDT_LINEART && (Type) != FDT_PALETTECOLOR)
#define	ISDRAWTOOL(id) ((id)==IDC_DRAWFREE || (id)==IDC_PENCIL)
#define ISCLOSED(id) ((id) != IDC_PENCIL)

#define MOVEKEY SPACE

#define CTLCOLOR_CUSTOM 100

#ifndef WS_EX_SMCAPTION
#define WS_EX_SMCAPTION	0x00000080L
#endif

/* All controls should have the first 4 words reserved */
/* either private data or a data pointer should follow */
#define GWW_STATE		0
#define GWL_FONT		(GWW_STATE + sizeof(WORD))
#define GWW_STATUS		(GWL_FONT + sizeof(long))
#define GWL_CTLDATAPTR	(GWW_STATUS + sizeof(WORD))
#define GWW_START		(GWL_CTLDATAPTR + sizeof(long))

/* GWW_STATUS possible values */
#define ST_TRACK		0x0001
#define ST_INRECT		0x0002
#define ST_TIMERON		0x0004
#define ST_DRAGANDDROP	0x0008

/* Icon control window word defintions */
#define GWL_IMAGE				(GWW_ICONID+sizeof(WORD)) // Used in icon control also
#define ICON_EXTRA				(GWL_IMAGE+sizeof(long))
#define GWL_IMAGE_TOOLPROC		(GWL_IMAGE+sizeof(long)) // An image control tool proc
#define IMAGE_EXTRA			    (GWL_IMAGE_TOOLPROC+sizeof(long))
#define GWL_IMAGE_TOOLPROCDATA  (GWL_IMAGE_TOOLPROC+sizeof(long)) // An image control tool proc
#define IMAGE_EXTRATOOLDATA		(GWL_IMAGE_TOOLPROCDATA+sizeof(long))
#define LABELEDFRAME_EXTRA		(GWL_IMAGE+sizeof(long))

/* Path Control */
#define PATH_CURPOS			0
#define PATH_DBLCLKSEEN		(PATH_CURPOS+sizeof(WORD))
#define PATH_NHIGHLIGHTED  	(PATH_DBLCLKSEEN+sizeof(WORD))
#define PATH_EXTRA				(PATH_NHIGHLIGHTED+sizeof(WORD))

/* Lever Control */
#define GWL_LEVERCOLOR			0
#define LEVER_EXTRA			(GWL_LEVERCOLOR+sizeof(long))

/* TextBlock Control */
#define TEXTBLOCK_EXTRA		GWW_START

/* Panel Control */
#define PANEL_EXTRA			GWW_START

/* Picker Control */
#define PICKER_EXTRA			0

/* Hue Control */
#define GWL_HUECOLOR			0
#define HUE_EXTRA				(GWL_HUECOLOR+sizeof(long))

/* Gamma Control */
#define GWW_ANIMATE			0
#define GWW_ANIMATECOLOR		(GWW_ANIMATE+sizeof(WORD))
#define GAMMA_EXTRA			(GWW_ANIMATECOLOR+sizeof(WORD))

/* Histo Control */
#define HISTO_EXTRA		20  // 5 long's


/* Frame Control */
#define FRAME_EXTRA		0

/* Palette control */
#define GWL_PALETTE			GWW_START
#define GWL_SELECT				(GWL_PALETTE+sizeof(long))
#define PALETTE_EXTRA			(GWL_SELECT+sizeof(long))

/* Ruler Control */
#define RULER_EXTRA		GWW_START

/* Color and Color2 control window word defintions */
#define GWL_ACTIVE			GWW_START
#define COLOR_EXTRA		(GWL_ACTIVE+sizeof(long))
#define GWL_ALTERNATE		(GWL_ACTIVE+sizeof(long))
#define COLOR2_EXTRA		(GWL_ALTERNATE+sizeof(long))

// Visual control
#define GWL_LPVISUAL		(GWW_START)
#define VISUALFRAME_EXTRA	(GWL_LPVISUAL+sizeof(long)) 

// Zoom Window extras
#define GWW_ZOOMBOXLEFT 	20		// inverted box in zoom window
#define GWW_ZOOMBOXTOP 		22		
#define GWW_ZOOMBOXRIGHT	24		
#define GWW_ZOOMBOXBOTTOM	26		
#define GWL_ZOOMOWNER		28		
#define WNDEXTRA_ZOOMBOX	32

/* Private Messages */
#define WM_SETCONTROL		(WM_USER)
#define WM_SETPALETTE		(WM_USER+1)
#define WM_PALETTESIZE		(WM_USER+2)
#define WM_SELECTPALETTE	(WM_USER+3)
#define WM_DOCCHANGED		(WM_USER+4)
#define WM_DOCACTIVATED		(WM_USER+5)
#define WM_CONTROLENABLE	(WM_USER+6)
#define WM_GETCOLOR		(WM_USER+7)
#define WM_DRAGQUERY		(WM_USER+8)
#define WM_DRAGACCEPT		(WM_USER+9)
#define WM_IMAGESPACE		(WM_USER+10)
#define WM_SHIELDCHANGE	(WM_USER+11)
#define WM_CTLSETUP		(WM_USER+12)
#define WM_PROCESSINGDONE	(WM_USER+13)
#define WM_MESSAGESTATUS	(WM_USER+14)
#define WM_ADDRECALL		(WM_USER+15)
#define WM_TASKMANAGE		(WM_USER+16)
#define WM_DROPACTIVATE	    (WM_USER+17)
#define WM_PASTEFROMFILECLOSED (WM_USER+18)
#define WM_MESSAGEBOX 		(WM_USER+19)
#define WM_GETMESSAGEPARENT (WM_USER+20)
#define WM_PROMPTFORFILENAME (WM_USER+21)
#define WM_STATUSMSG		(WM_USER+22)

// Default macros
#define GetDefTmp(N,STR) GetDefaultString(#N,STR,t,sizeof(t))
#define GetDefStr(N,STR) GetDefaultString(#N,STR,N,sizeof(N))
#define GetDefInt(N,INT) (N=GetDefaultInt(#N,INT))
#define GetDefFix(N,FIX) { TCHAR t[50]; GetDefTmp(N,FixedAscii(FIX, t, 45)); N=AsciiFixed(t); }
#define GetDefHex(N,INT) { TCHAR t[40], t2[40]; GetDefTmp(N,itoh(INT,t2)); N=htoi(t); }
#define GetDefRGB(N,RGB) { TCHAR t[40]; GetDefTmp(N,RGB); AsciiRGB(t,&N); }
#define PutDefStr(N,STR) PutDefaultString(#N,STR)
#define PutDefInt(N,INT) PutDefaultInt(#N,INT)
#define PutDefFix(N,FIX) { TCHAR t[40]; PutDefaultString(#N,FixedAscii(FIX,t,-1)); }
#define PutDefRGB(N,RGB) { TCHAR t[40]; PutDefaultString(#N,RGBAscii(&RGB,t)); }
#define PutDefHex(N,INT) { TCHAR t[40]; PutDefaultString(#N,itoh(INT,t)); }
#define PutPrivInt(STR1,STR2,N,INT) PutPrivateInt(STR1,STR2,#N,INT)
#define PutPrivFix(STR1,STR2,N,FIX) { TCHAR t[40]; PutPrivateString(STR1,STR2,#N,FixedAscii(FIX,t,-1)); }

typedef void (FAR *LPTOOLPROC)( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
typedef void ( FAR *SHARPSMOOTHPROC )( int, LPTR, LPTR, LPTR, LPTR );

/************************************************************************/
/* Retouch Dialog Description											*/
/************************************************************************/
typedef struct _retouchdlg
	{
	ITEMID		idTool;				// Tool id
	ITEMID		idFirstTool;		// id of first tool in consecutive group
	ITEMID		idFirstStyle;		// id of first brush style in group
	ITEMID		idCreateDlg;		// id of brush creator dialog
	BOOL		fIsRibbon;			// is this dialog a ribbon?
	FNAME 	FAR	*StyleName;			// array of style names for each tool
	} RETOUCHDLG, FAR *LPRETOUCHDLG;

/************************************************************************/
/* Convolution kernal definition					*/
/************************************************************************/
typedef struct _kernal
	{
	int	absflag;
	int	rotation;
	int	edgetype;
	int	lut1[256];
	int	lut2[256];
	int	lut3[256];
	int	lut4[256];
	int	lut5[256];
	int	lut6[256];
	int	lut7[256];
	int	lut8[256];
	int	lut9[256];
	TCHAR	outlut[256];
} KERNAL;
typedef KERNAL FAR *LPKERNAL;

/********************************************************************* 
 **** STYLE:
 *********************************************************************/

typedef struct _style
{
FNAME	HalftoneName;
FNAME	SeparateName;
FNAME	CalibrateName;
BOOL	fUseCMS;
BOOL	TrimMarks;
BOOL	RegMarks;
BOOL	Labels;
BOOL	StepScale;
BOOL	Negative;
BOOL	EmulsionDown;
} STYLE;
typedef STYLE FAR *LPSTYLE;

/********************************************************************* 
 **** PALETTE:
 *********************************************************************/

typedef struct _palett
{
FNAME	szName;
LPRGB	lpColors;
int		iColors;
LPTSTR	lpLabels;
WORD	LabelsLength;
DWORD	dwGroup;
STRING	szFormat;
struct _palett far *lpNext;
LPCOLORINFO lpColorInfo;
} PALETTE;
typedef PALETTE far *LPPALETTE;

//**********************************************************************
// 4 Corner Distortion definition
//**********************************************************************
typedef struct _distort
	{
	RECT RectOrig;
	RECT Rect;
	POINT p[4];
} DISTORT;
typedef DISTORT FAR *LPDISTORT;

//**********************************************************************
// Drop and drag definition
//**********************************************************************
typedef struct _drag
	{
	HWND	hWnd;
	WORD	wType;
	LPTR	lpData;
	POINT	pos;
	HRGN	hRgn;
	HCURSOR	hOldCursor;
	HCURSOR	hDragMove;
	HCURSOR hDragCopy;
	HCURSOR hDragNot;
	HWND	hDragWnd;
	BOOL	fAccept;
} DRAG;
typedef DRAG FAR *LPDRAG;

#define DAD_RGB			0
#define DAD_FILENAME	1
#define DAD_ID			2

#endif // _APP_H_



