/*®FD1¯®PL1¯®TP0¯®BT0¯*/
/***********************************************************************
 * Routines.h	declarations of all procedures in picture publisher
***********************************************************************/
#define ROUTINES
#ifndef TYPES
    #include "types.h"
#endif

/************************************************************************/
/* Macro Subroutines							*/
/************************************************************************/

#define CreateProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_CREATE )
#define ButtonDownProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_LBUTTONDOWN )
#define ButtonUpProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_LBUTTONUP )
#define MoveProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_MOUSEMOVE )
#define DoubleClickProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_LBUTTONDBLCLK )
#define TimerProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_TIMER )
#define DestroyProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_DESTROY )
#define CommandProc( hWindow, lParam ) \
	(*Window.ToolProc)( hWindow, lParam, WM_COMMAND )
#define ModalCreateProc( hWindow, lParam ) \
	(*Window.ModalProc)( hWindow, lParam, WM_CREATE )
#define ModalButtonDownProc( hWindow, lParam ) \
	(*Window.ModalProc)( hWindow, lParam, WM_LBUTTONDOWN )
#define ModalButtonUpProc( hWindow, lParam ) \
	(*Window.ModalProc)( hWindow, lParam, WM_LBUTTONUP )
#define ModalMoveProc( hWindow, lParam ) \
	(*Window.ModalProc)( hWindow, lParam, WM_MOUSEMOVE )
#define ModalDoubleClickProc( hWindow, lParam ) \
	(*Window.ModalProc)( hWindow, lParam, WM_LBUTTONDBLCLK )
#define ModalDestroyProc( hWindow, lParam ) \
	(*Window.ModalProc)( hWindow, lParam, WM_DESTROY )
#define ModalCommandProc( hWindow, lParam ) \
	(*Window.ModalProc)( hWindow, lParam, WM_COMMAND )

#define RectWidth( lpRect )  ( (lpRect)->right - (lpRect)->left + 1 )
#define RectHeight( lpRect ) ( (lpRect)->bottom - (lpRect)->top + 1 )
#define IsEMS( lp ) ( ((ULONG)(lp) & 0xF0000000L) == 0xF0000000L )
#define SPAN64K(lp,x) (((long)(((long)lp) & 0xFFFFL) + ((long)x)) >= 65536L)
#define FGET( num, denom ) fget( (long)(num), (long)(denom) )
#define FMUL( num, fixed ) fmul( (long)(num), (FIXED)(fixed) )
#define TMUL( num, fixed ) tmul( (long)(num), (FIXED)(fixed) )
#define TOFIXED(num) ( (num)>0 ? ((long)(num))<<16 : -(((long)-(num))<<16) )
#define FHALF	0x00008000L
#define FONE    0x00010000L
#define RGB2long(rgb) RGB( rgb.red, rgb.green, rgb.blue )
#define ToMiniRGB(r,g,b) \
	( (((WORD)(b) << 5 | (WORD)(g)) << 5) | (WORD)(r) )
#define MiniR(v) (BYTE)( 31  & ( (WORD)(v)       ) )
#define MiniG(v) (BYTE)( 31  & ( (WORD)(v) >> 5  ) )
#define MiniB(v) (BYTE)( 31  & ( (WORD)(v) >> 10 ) )
#define MaxiR(v) (BYTE)( ToMaxi[ MiniR(v) ] )
#define MaxiG(v) (BYTE)( ToMaxi[ MiniG(v) ] )
#define MaxiB(v) (BYTE)( ToMaxi[ MiniB(v) ] )
#define AVG(a,b) (((a)+(b))>>1)
#define TOLUM(r,g,b) ( r<g ? ( r>b ? AVG(b,g) : (g>b ? AVG(g,r) : AVG(b,r)) ) \
			   : ( g>b ? AVG(b,r) : (r>b ? AVG(g,r) : AVG(b,g)) ) )
#define DN(v) ((WORD)(v)>>3)
#define RGBtoMiniRGB(R) \
	(((WORD) ( DN((R)->blue) << 5 | DN((R)->green) ) << 5) | DN((R)->red) )
#define BRMASK(x,y) Retouch.BrushMask[ FullBrush * y + x ]
#define sign( num ) \
	( (num) == 0 ? 0 : ( (num) < 0 ? -1 : 1 ) )
#define abs( num ) \
	( (num) < 0  ? -(num) : (num) )
#define mbound(val,lower,upper) \
	( (val) < (lower) ? (lower) : ( (val) > (upper) ? (upper) : (val) ) )
#define WITHIN(val,lower,upper) \
	( (val) < (lower) ? 0 : ( (val) > (upper) ? 0 : 1 ) )

#define CopyRGB(s, d)							\
{									\
*((LPWORD)(d)) = *((LPWORD)(s));						\
*((LPTR)(d)+2) = *((LPTR)(s)+2);					\
}

#define depth1toRGB( lpPixel, rgb )					\
{									\
BYTE b;									\
b = *((LPTR)(lpPixel));							\
(rgb)->red   = b;							\
(rgb)->green = b;							\
(rgb)->blue  = b;							\
}

#define depth1toHSL( lpPixel, hsl )					\
{									\
(hsl)->hue = 0;								\
(hsl)->sat = 0;								\
(hsl)->lum = *((LPTR)(lpPixel));					\
}

#define depth3toRGB( lpPixel, rgb )					\
{									\
(rgb)->red   = *((LPTR)(lpPixel));					\
(rgb)->green = *((LPTR)(lpPixel)+1);					\
(rgb)->blue  = *((LPTR)(lpPixel)+2);					\
}

#define depth3toHSL( lpPixel, hsl )					\
{									\
RGBtoHSL( *((LPTR)(lpPixel)),						\
	  *((LPTR)(lpPixel)+1),						\
	  *((LPTR)(lpPixel)+2), hsl );					\
}

#define depth2toRGB( lpPixel, rgb )					\
{									\
WORD w;									\
w = *((LPWORD)(lpPixel));						\
(rgb)->red   = MaxiR(w);						\
(rgb)->green = MaxiG(w);						\
(rgb)->blue  = MaxiB(w);						\
}

#define depth2toHSL( lpPixel, hsl )					\
{									\
WORD w;									\
w = *((LPWORD)(lpPixel));						\
RGBtoHSL( MaxiR(w), MaxiG(w), MaxiB(w), hsl );				\
}

#define frame_getpixel(lpLine, colorPtr )				\
{									\
if (DEPTH == 3)		{ *(colorPtr) = 0; CopyRGB(lpLine, colorPtr); }	\
else if (DEPTH == 1)	{ *((LPTR)(colorPtr)) = *((LPTR)(lpLine)); }	\
     else		{ *((LPWORD)(colorPtr)) = *((LPWORD)(lpLine)); }\
}

#define frame_getRGB( lpPixel, rgb )					\
{									\
if (DEPTH == 3)		{ depth3toRGB( lpPixel, rgb ); }		\
else if (DEPTH == 1)	{ depth1toRGB( lpPixel, rgb ); }		\
     else		{ depth2toRGB( lpPixel, rgb ); }		\
}

#define frame_getHSL( lpPixel, hsl )					\
{									\
if (DEPTH == 3)		{ depth3toHSL( lpPixel, hsl ); }		\
else if (DEPTH == 1)	{ depth1toHSL( lpPixel, hsl ); }		\
     else		{ depth2toHSL( lpPixel, hsl ); }		\
}

#define RGBtodepth1( lpPixel, rgb )					\
{									\
*((LPTR)(lpPixel)) = TOLUM( (rgb)->red, (rgb)->green, (rgb)->blue );	\
}

#define HSLtodepth1( lpPixel, hsl )					\
{									\
*((LPTR)(lpPixel)) = (hsl)->lum;					\
}

#define RGBtodepth3( lpPixel, rgb )					\
{									\
*((LPTR)(lpPixel))   = (rgb)->red;   					\
*((LPTR)(lpPixel)+1) = (rgb)->green; 					\
*((LPTR)(lpPixel)+2) = (rgb)->blue;					\
}

#define HSLtodepth3( lpPixel, hsl )					\
{									\
HSLtoRGB( (hsl)->hue, (hsl)->sat, (hsl)->lum, (LPRGB)lpPixel );		\
}

#define RGBtodepth2( lpPixel, rgb )					\
{									\
*((LPWORD)(lpPixel)) = RGBtoMiniRGB( rgb );				\
}

#define HSLtodepth2( lpPixel, hsl )					\
{									\
RGBS rgb;								\
HSLtoRGB( (hsl)->hue, (hsl)->sat, (hsl)->lum, &rgb );			\
*((LPWORD)(lpPixel)) = RGBtoMiniRGB( &rgb );				\
}

#define frame_putpixel( lpLine, color )				   	\
{									\
if (DEPTH == 3)		{ CopyRGB(&color, lpLine); }			\
else if (DEPTH == 1)	{ *((LPTR)(lpLine)) = *((LPTR)&(color)); }	\
     else		{ *((LPWORD)(lpLine)) = *((LPWORD)&(color)); }	\
}

#define frame_putRGB( lpPixel, rgb )					\
{									\
if (DEPTH == 3)		{ RGBtodepth3( lpPixel, rgb ); }		\
else if (DEPTH == 1)	{ RGBtodepth1( lpPixel, rgb ); }		\
     else		{ RGBtodepth2( lpPixel, rgb ); }		\
}

#define frame_putHSL( lpPixel, hsl )					\
{									\
if (DEPTH == 3)		{ HSLtodepth3( lpPixel, hsl ); }		\
else if (DEPTH == 1)	{ HSLtodepth1( lpPixel, hsl ); }		\
     else		{ HSLtodepth2( lpPixel, hsl ); }		\
}

#define frame_copypixel(lpSrc, lpDst, depth )				\
{									\
if (depth == 3)		{ CopyRGB(lpSrc, lpDst); }			\
else if (depth == 1)	{ *((LPTR)(lpDst)) = *((LPTR)(lpSrc)); }	\
     else		{ *((LPWORD)(lpDst)) = *((LPWORD)(lpSrc)); }	\
}

#define IsProtected( x,y,onMask,lpRGB,lpPixel )				\
( InColorMask(lpRGB,lpPixel) || (IsMasked(x,y) != (onMask)) )

#define CONTROL ( GetAsyncKeyState( VK_CONTROL )<0 )
#define ALT ( GetAsyncKeyState( VK_MENU )<0 )
#define CONSTRAIN_Y CONTROL
#define CONSTRAIN_X ALT
#define ESCAPE ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define CANCEL ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define BACKSPACE ( GetAsyncKeyState( VK_BACK )<0 )
#define SHIFT ( GetAsyncKeyState( VK_SHIFT )<0 )
#define SPACE ( GetAsyncKeyState( VK_SPACE )<0 )
#define CAPSLOCK ( GetAsyncKeyState( VK_CAPITAL )&1 )
#define LBUTTON ( GetAsyncKeyState( VK_LBUTTON)<0 )
#define MBUTTON ( GetAsyncKeyState( VK_MBUTTON)<0 )
#define RBUTTON ( GetAsyncKeyState( VK_RBUTTON)<0 )
#define HELP 0
#define MAKEWORD(a,b) ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define PointIsNull(point) (!(point)->x && !(point)->y)

/* These are non-portable macros */
#undef  HIWORD
#define HIWORD(l) (*((WORD*)&l+1))
#undef  LOWORD
#define LOWORD(l) (*((WORD*)&l))
#undef  HIBYTE
#define HIBYTE(w) (*((BYTE*)&w+1))
#undef  LOBYTE
#define LOBYTE(w) (*((BYTE*)&w))

#define FP 16
#define fix2int(num) ((int)((num) >> FP))
#define ROUND(num) ((int)(((num) + HALF) >> FP))
#define fix2fix(num) ((FIXED)((unsigned long)(num) >> FP))
#define rotate_x(cosx, siny, xc) (ROUND((cosx) + (siny) + (xc)))
#define rotate_y(sinx, cosy, yc) (ROUND((yc) - (sinx) - (cosy)))
#define frotate_x(cosx, siny, xc) ((cosx) + (siny) + (xc))
#define frotate_y(sinx, cosy, yc) ((yc) - (sinx) - (cosy))

extern PTR ltoa(long, PTR, int);
extern PTR itoa( int, PTR, int );
extern int atoi( PSTR );

#define CP_GETBEEP	1
#define CP_SETBEEP	2
#define CP_GETMOUSE	3
#define CP_SETMOUSE	4
#define CP_GETBORDER	5
#define CP_SETBORDER	6
#define CP_TIMEOUTS	7
#define CP_KANJIMENU	8
void FAR PASCAL ControlPanelInfo(WORD, WORD, LONG);
DWORD FAR PASCAL GlobalDosAlloc(DWORD);
WORD FAR PASCAL GlobalDosFree(WORD);

/* acquire.asm */
extern	int acquire(int);
extern	void fieldsu(int,int,int);
extern	void willnrd(int,LPTR);
extern	void aitechrd(int,int,LPTR);

/* asmutils.asm */
extern void ProcessData8(LPTR,LPTR,LPTR,int);
extern void ProcessData24(LPRGB,LPRGB,LPTR,int);
extern void MergeMask(LPTR,LPRECT,int,LPTR,LPRECT,int,LPRECT);
extern void MergeData8(LPTR,LPRECT,int,LPTR,LPRECT,int,LPRECT);
extern void MergeData24(LPRGB,LPRECT,int,LPRGB,LPRECT,int,LPRECT);
extern void ScatterMask(LPTR, LPTR, LPINT, int, int);
extern void copy(LPTR,LPTR,int);
extern void clr(LPTR,int);
extern void set(LPTR,int,int);
extern void setifset(LPTR,int,int);
extern void set24(LPRGB,int,long);
extern  void Convert24to24A(struct _rgb far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva, LPTR lpMatrix);
extern  void Convert8to24A(LPTR lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva, LPTR lpMatrix);
extern  void Convert24to4A(struct _rgb far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva, LPTR lpMatrix);

/* do_int.asm */
extern void do_int(int,LPMREGS,LPMREGS);

/* handheld.asm */
extern int FAR HHopen(void);
extern void FAR HHcall(LPTR);
extern void FAR HHclose(void);

/* hpdos21.asm */
extern int Int21X(LPREGS,LPREGS,LPSREGS);
extern int Int80X(LPREGS,LPREGS,LPSREGS);
extern void SegRead(LPSREGS);

/* memory.asm */
extern int ExtCount(void);

/* tools.asm */
extern LPSTR FAR PASCAL lstrcpy( LPSTR, LPSTR );
extern LPSTR FAR PASCAL lstrcat( LPSTR, LPSTR );
extern int   FAR PASCAL lstrlen( LPSTR );

/* scancall.asm */
extern int FAR SCNOpen(void);
extern void FAR SCNCall(LPTR);
extern void FAR SCNClose(void);

/* emstools.asm */
extern BYTE ems_allocate(WORD pages, LPWORD handle);
extern BYTE ems_deallocate(WORD handle);
extern BYTE ems_gahc(LPWORD active_handle_count);
extern BYTE ems_get_pmap(LPTR page_map);
extern BYTE ems_getset_pmap(LPTR get_page_map, LPTR set_page_map);
extern BYTE ems_ghpc(WORD handle, LPWORD page_count);
extern BYTE ems_gpc(LPWORD tpc, LPWORD upc);
extern BYTE ems_gstatus(void);
extern BYTE ems_installed(void);
extern BYTE ems_map(WORD handle, WORD logical_page, WORD physical_page);
extern BYTE ems_pfba(LPPTR frame_adr);
extern BYTE ems_restore_page_map(WORD handle);
extern BYTE ems_save_page_map(WORD handle);
extern BYTE ems_set_pmap(LPTR set_page_map);
extern BYTE ems_size_pmap(LPWORD pmap_size);
extern BYTE ems_version(LPTR version);

// sample.asm
extern  void sample8(unsigned char far *input,int inskip,unsigned char far *output,int outskip,int count,long rate);
extern  void sample8MW(unsigned char far *input,int inskip,unsigned char far *output,int outskip,int count,long rate,unsigned char far *mask);
extern  void sample16(unsigned int far *input,int inskip,unsigned int far *output,int outskip,int count,long rate);
extern  void sample16MW(unsigned int far *input,int inskip,unsigned int far *output,int outskip,int count,long rate,unsigned char far *mask);
extern  void sample24(unsigned char far *input,int inskip,unsigned char far *output,int outskip,int count,long rate);
extern  void sample24MW(unsigned char far *input,int inskip,unsigned char far *output,int outskip,int count,long rate,unsigned char far *mask);
extern  void qsample8(unsigned char far *lpIn,unsigned char far *lpOut,int x,int count,int far *XSample);
extern  void qsample16(unsigned int far *lpIn,unsigned int far *lpOut,int x,int count,int far *XSample);
extern  void qsample24(unsigned char far *lpIn,unsigned char far *lpOut,int x,int count,int far *XSample);
extern  void csample16(unsigned int far *input,int inskip,unsigned int far *output,int outskip,int count,long rate);

/* spawn.asm */
extern int far pascal Int21Function4B( BYTE, LPTR, LPTR );

/* stack.asm */
extern	void PASCAL InitStackUsed(void);
extern	int  PASCAL ReportStackUsed(void);

/* ppscan.exe */
extern  int ScanOpen(void );
extern  int ScanClose(void );
extern  int ScanCall(int msg,unsigned char far *lpParam);

/* imports from the VGA driver */
extern BOOL FAR PASCAL	PicwinOff( LPSTR, unsigned, LPSTR, LPSTR, LPSTR );
extern BOOL FAR PASCAL	PicwinOn( LPSTR );

/* imports from the Grabber driver */
extern  int pascal far AcquireProc(unsigned int hWindow,unsigned int msg,unsigned int wParam,long lParam);
extern VOID AcquireCommand(HWND,WORD);

/* libmain.obj libmain */
extern  int pascal LibMain(unsigned int hInstance,unsigned int wDataSeg,unsigned int wHeapSize,unsigned char far *lpCmdLine);

/* Everything below was gotten from compiling all sources */
/* with the -Zsg switch, and can be regenerated */

  // about.obj about.c
static  void pascal near blink_readme(unsigned int hDlg,int bReset);
static  void pascal near display_cast(unsigned int hDlg,int bScroll);
static  short pascal near get_version_string(char near *pLibName,char near *pVerStr,short nBytes);
static  void pascal near paint_app_icon(unsigned int hCtl);

  // astral.obj astral.c
extern  int pascal WinMain(unsigned int hInstance,unsigned int hPrev,char far *lpszCmdLine,int cmdShow);
extern  int AstralInit(unsigned int hInstance);
extern  int AstralGetMessage(struct tagMSG far *lpMsg);
extern  void AstralProcessMessage(struct tagMSG far *lpMsg);
extern  long pascal far AstralWndProc(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  int SetupCursor(unsigned int hWnd,unsigned int HitCode,unsigned int msg);
extern  int SetCurView(unsigned int hWnd,unsigned int far *lphOldWnd);
extern  unsigned int ActivateDocument(unsigned int hWnd);
extern  void HandlePaletteChange(unsigned int hWnd);
extern  void AstralCommand(unsigned int hWindow,unsigned int wParam);
extern  void AstralCursor(char far *lpCursor);
extern  int AstralClockCursor(int part,int whole);
extern  void AstralBeachCursor(void );
extern  void SetFileMenus(void );
extern  void SetMaskMenus(void );
extern  void SetUndoMenus(void );
extern  void SetWindowMenus(void );
extern  void SetShowHideMenus(unsigned int idMenu,unsigned int idDialogBox);
extern  void SetPasteMenus(void );
extern  int NewImageWindow(struct image_struct far *lpOldImage,unsigned char far *szName,struct _frame far *lpFrame,int FileType,int fSingleBit,int fNewView,int ImageType,unsigned char far *ImageName);
extern  void CloseImage(int fCloseAll);
extern  int ConfirmClose(int fCloseAll);
extern  char far *GetUntitledName(void );
extern  int OKToOpen(unsigned char far *lpFile,unsigned char far * far *lppName);
extern  int pascal far MessageHookProc(int nCode,unsigned int Dummy,struct tagMSG far *lpMsg);

  // automask.obj automask.c
extern  void automask(int x,int y,struct tagPOINT far * far *lpVecptr,int far *lpVeccnt,int far *lpMx,int far *lpMy,int far *lpDx,int far *lpDy);
extern  long EdgeValue8(int x,int y,int FromDir,int dx,int dy,int depth);
extern  long Distance16(unsigned int v1,unsigned int v2);
extern  long Distance24(unsigned char far *lpV1,unsigned char far *lpV2);
extern  long EdgeValue16(int x,int y,int FromDir,int dx,int dy,int depth);
extern  long EdgeValue24(int x,int y,int FromDir,int dx,int dy,int depth);
extern  int GoingAway(int dx,int dy);
extern  int direction(int dx,int dy);

  // bmprdr.obj bmprdr.c
static  int bmpReadHeader(struct _filebuf *ifd,struct tagBITMAPFILEHEADER *hdr);
extern  struct _frame far *bmprdr(unsigned char far *ifile,int outdepth,int far *lpBPP);

  // bmpwrt.obj bmpwrt.c
extern  int bmpwrt(unsigned char far *ifile,struct _frame far *lpFrame,struct tagRECT far *lpRect,int flag,int fCompressed);

  // cache.obj cache.c
extern  void CacheNewFrame(struct _frame far *lpNewFrame);
extern  void CacheFree(struct image_struct far *lpImage);
extern  void CacheUndo(void );
extern  unsigned char far *CachePtr(int Depth,int x,int y,int bModify);
extern  int CacheInitUndo(void );
extern  void CacheApply(void );
extern  int CacheInit(int fForceApplyUndo,int fNeedUndo);
extern  void CacheSyncLines(void );
extern  void CacheSyncLine(int y);
extern  int CacheWrite(struct _span far *span,unsigned char far *pixels,int count);
extern  int CacheRead(struct _span far *span,unsigned char far *pixels,int count);
extern  int CacheCheckApplyUndo(void );

  // calib.obj calib.c
extern  void DoMeasure(int column,struct _rgb far *lpRGB,struct tagRECT mRect);
extern  void MeasureTestStrip(void );
extern  int AutoMeasure(int iMode,int iColor,struct _rgb far *Krgb,struct _rgb far *Crgb,struct _rgb far *Mrgb,struct _rgb far *Yrgb,struct _rgb far *Rrgb,struct _rgb far *Grgb,struct _rgb far *Brgb);
extern  long Measure(struct tagRECT far *lpRect,struct _rgb far *lpRGB,int fFileCoords);

  // cmask.obj cmask.c
extern  int InColorMask(struct _rgb far *lpRGB,unsigned char far *lpPixel);
extern  void SetupShield(void );
extern  void SetupColorMask(unsigned char Mask,struct _rgb far *lpRGB1,struct _rgb far *lpRGB2,int On);
extern  void UpdateColorMask(struct tagRECT far *lpRect);
extern  int ShowShieldProc(unsigned int hWindow,long lParam,int msg);
extern  void ShowShieldPreview(void );
extern  void ShowShieldReset(void );
extern  int ShieldFloatProc(int yline,int xleft,int xright,unsigned char far *lpDst,unsigned char far *lpSrc,unsigned char far *lpMask);

  // color.obj color.c
extern  struct _frame far *ConvertLongColor(struct _frame far *lpFrame);
extern  void ConvertLongColor24(struct _frame far *lpFrame,struct _frame far *lpNewFrame);
extern  void SetEachThirdValue(unsigned char far *lpIn,unsigned char far *lpOut,int iCount);
extern  struct _frame far *ConvertWideColor(struct _frame far *lpFrame);
extern  unsigned int AverageMiniRGB(unsigned int val1,unsigned int val2);
extern  unsigned int AverageRGB(struct _rgb far *lpRGB1,struct _rgb far *lpRGB2);
extern  void ExpandRGB(unsigned int far *lpMiniRGB,struct _rgb far *lpRGB,int iCount);
extern  void UnshuffleRGB(struct _rgb far *lpRGB,unsigned char far *lpRed,unsigned char far *lpGreen,unsigned char far *lpBlue,int iCount);
extern  void RGBtoCMYK(struct _rgb far *lpRGB,unsigned char far *lpCyan,unsigned char far *lpMagenta,unsigned char far *lpYellow,unsigned char far *lpBlack,int iCount);

  // compress.obj compress.c
extern  int CompressLZW_2(int ofp,unsigned char far *lpIn,int InCount,int (*WriteCode)(),int minCodeLength);
extern  int DecompressLZW_2(int ifp,unsigned char far *lpOut,int OutCount,int (*ReadCode)(),int minCodeLength);
static  void InitCodeTable(void );
static  int FindCode(int Code,unsigned char Char);
static  int AddCode(int Code,unsigned char Char);
extern  void CheckoutCodeLength(void );
static  unsigned char FirstChar(int Code);
static  void BufferString(int Code);
static  int TiffReadCode(void );
extern  int DecompressLZW(int ifh,unsigned char far *lpOut,int OutCount);
static  int GifReadCode(void );
extern  int DecompressLZW_GIF(int ifh,unsigned char far *lpOut,int OutCount,int codeSize);
static  void TiffWriteCode(int Code);
extern  int CompressLZW(int ofp,unsigned char far *lpIn,int InCount);
static  void GifWriteCode(int Code);
extern  int CompressLZW_GIF(int ofp,unsigned char far *lpIn,int InCount,int codeSize);

  // controls.obj controls.c
extern  long pascal far StickControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  int DrawStick(unsigned int hDC,struct tagRECT far *lpRect,struct tagPOINT far *lpLastPoint,struct tagPOINT Point);
extern  long pascal far LeverControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawLever(unsigned int hDC,struct tagRECT far *lpRect,int iOldValue,int iValue,unsigned long Color);
extern  long pascal far MapControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  int ClosestPoint(struct tagPOINT far *lpPoints,int nPoints,struct tagPOINT Point);
extern  int DrawMap(unsigned int hDC,struct tagRECT far *lpRect,struct tagPOINT far *lpLastPoints,struct tagPOINT far *lpPoints,int nPoints,int idPointToChange,struct tagPOINT Point,int bDrawHandles);
extern  long pascal far IconControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawBitmap(unsigned int hDC,int x,int y,unsigned int hBitmap);
extern  long pascal far ActiveToolControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  long pascal far ScrollControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawScroll(unsigned int hDC,struct tagRECT far *lpRect,int bPlus,int bDown);
extern  long pascal far ColorControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawColorPatch(unsigned int hDC,struct tagRECT far *lpRect,unsigned long Color);
extern  long pascal far Color2Control(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  long pascal far HueControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawHueBar(unsigned int hDC,struct tagRECT far *lpRect,int hue,int hueDefault,int bHasFocus);
extern  long pascal far HistoControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawHisto(unsigned int hDC,struct tagRECT far *lpRect,int iValue,unsigned int far *lpHisto,struct _rgb far *lpRGB,int iDrawValue);
extern  long pascal far QuartersControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void ModifyQuartertones(struct _map *lpMap,int iQTone,int iValue);
extern  void SetControlValue(unsigned int hDlg,int nIDDlgItem,int nIndex,long lValue,int fNotify);
extern  long GetControlValue(unsigned int hDlg,int nIDDlgItem,int nIndex);
extern  long pascal far TextBlockControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawSculptedBox(unsigned int hDC,struct tagRECT far *lpRect,int bFocus,int bDown,int bDoubleShadow);
extern  void DrawStaticBox(unsigned int hDC,struct tagRECT far *lpRect,int bDoubleShadow);
extern  long pascal far StaticTextControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawStaticText(unsigned int hDC,unsigned int hWnd,char far *lpString);
extern  void EraseControlBackground(unsigned int hDC,unsigned int hWnd,struct tagRECT far *lpRect,unsigned int wType);
extern  unsigned int GetBackgroundBrush(unsigned int hDC,unsigned int hWnd,unsigned int wType);
extern  unsigned int SelectBackgroundBrush(unsigned int hDC,unsigned int hWnd,unsigned int wType);
extern  long pascal far ButtonControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawCheckBox(unsigned int hDC,unsigned int hWnd);

  // convert.obj convert.c
extern  void Convert24to8(struct _rgb far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert24to8G(struct _rgb far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert16to8(unsigned int far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert16to8G(unsigned int far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert8to8(unsigned char far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert8to8C(unsigned char far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert24to4(struct _rgb far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert16to4(unsigned int far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert8to4(unsigned char far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert24to24(struct _rgb far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert16to24(unsigned int far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert8to24(unsigned char far *lpPixel,int iCount,int far *lpError,unsigned char far *lpOutput,int xDiva,int yDiva);
extern  void Convert24to16(struct _rgb far *lpPixel,int iCount,int far *lpError,unsigned int far *lpOutput,int xDiva,int yDiva);
extern  void Convert16to16(unsigned int far *lpPixel,int iCount,int far *lpError,unsigned int far *lpOutput,int xDiva,int yDiva);
extern  void Convert8to16(unsigned char far *lpPixel,int iCount,int far *lpError,unsigned int far *lpOutput,int xDiva,int yDiva);

  // cpo.obj cpo.c
static  int SplitCube(struct _cube far *lpCubeList,int nCubes);
static  long SumPlaneX(unsigned int x,struct _cube far *lpCube);
static  long SumPlaneY(unsigned int y,struct _cube far *lpCube);
static  long SumPlaneZ(unsigned int z,struct _cube far *lpCube);
extern  long CreateHistogram(unsigned int far *lpHisto);
extern  unsigned char far *CreateOptimizedPalette(struct _rgb far *lpRGBmap);

  // dda.obj dda.c
extern  void ddaline(int x1,int y1,int x2,int y2,int (far *lpPointFunction)());

  // defaults.obj defaults.c
extern  unsigned int GetDefaultInt(unsigned char far *lpKeyName,int nDefault);
extern  unsigned int GetDefaultString(unsigned char far *lpKeyName,unsigned char far *lpDefault,unsigned char far *lpReturnedString,int nSize);
extern  int PutDefaultString(unsigned char far *lpKeyName,unsigned char far *lpString);
extern  void LoadAllPreferences(unsigned char far *lpProgHome);
extern  void StoreAllPreferences(unsigned char far *lpProgHome);
static  int paletteWrite(unsigned char far *lpFileName,struct _palett *pal);
static  void paletteWriteRGB(struct _filebuf far *ofd,struct _rgb far *rgb);
static  int paletteRead(unsigned char far *lpFileName,struct _palett *pal);
static  void paletteReadRGB(struct _filebuf far *ifd,struct _rgb far *rgb);
static  void GetRetouchBrush(char *name,int brush);
static  void PutRetouchBrush(char *name,int brush);
extern  unsigned char far *stripstruct(unsigned char far *lpString);
extern  unsigned char far *LoadPreferences(unsigned char far *lpFileName);
extern  unsigned char far *FindPreference(unsigned char far *lpKeyName);
extern  void SaveToolPreferences(unsigned int idTool);

  // demo.obj demo.c
extern  unsigned long pascal far Recorder(int iCode,unsigned int wParam,struct tagEVENTMSG far *lpEM);
extern  int InstallRecorder(unsigned char near *pRecordFile);
extern  int RemoveRecorder(void );
extern  unsigned long pascal far Player(int iCode,unsigned int wParam,struct tagEVENTMSG far *lpEM);
extern  int InstallPlayer(unsigned char near *pPlayFile);
extern  int RemovePlayer(void );
static  void HookOn(void );
static  void HookOff(void );
extern  int DemoMessage(void );
extern  void BeginLine(int x1,int y1,int x2,int y2);
extern  void GetNextPoint(int far *lpx,int far *lpy);
extern  int DemoRunning(void );
extern  void Click(void );
extern  int FindAWindow(struct tagPOINT Point,unsigned char far *lpString,int iStringLen);
static  void WriteEvent(struct _iobuf *fp,struct tagEVENTMSG far *lpEvent);
static  int ReadEvent(struct _iobuf *fp,struct tagEVENTMSG far *lpEvent);

  // dlgfile.obj dlgfile.c
extern  int DoOpenDlg(unsigned int hDlg,int idDlg,unsigned int idType,unsigned char near *szFileNameOut,int fSavingIn);
extern  int pascal far DlgOpenProc(unsigned int hDlg,unsigned int iMessage,unsigned int wParam,long lParam);
extern  unsigned char far *RemoveDots(unsigned char far *lpFileName);
extern  unsigned char far *DriveName(int iDrive,unsigned char far *lpName);
extern  unsigned char far *DriveSize(int iDrive,unsigned char far *lpSize);
extern  unsigned char far *DirectoryName(int iDirectory,unsigned char far *lpName);
extern  void LoadComboWithDrives(unsigned int hWnd,unsigned int idComboBox);
extern  void LoadComboWithDirectories(unsigned int hWnd,unsigned int idComboBox,int boole);
extern  int LookupExtension(unsigned int idFileType,unsigned char far *lpExtension);
extern  int GetFileLocation(unsigned int idFileType,unsigned char far *lpPath);
extern  int SetFileLocation(unsigned int idFileType,unsigned char far *lpPath);
extern  unsigned char far *SizeToAscii(unsigned long lSize,unsigned char far *lpString);

  // dlgmgr.obj dlgmgr.c
extern  int AstralControlRect(unsigned int hWindow,int idControl,struct tagRECT far *lpRect);
extern  int AstralControlPaint(unsigned int hWindow,int idControl);
extern  int AstralControlRepaint(unsigned int hWindow,int idControl);
extern  int ControlEnable(unsigned int hWindow,int idControl,int bGray);
extern  void InitDlgItemSpin(HWND hWindow, int idControl, int iValue, BOOL bSigned, int iMin, int iMax);
extern  void SetDlgItemSpin(HWND hWindow, int idControl, int iValue, BOOL bSigned);
extern  int  GetDlgItemSpin(HWND hWindow, int idControl, LPINT lpTranslated, BOOL bSigned);
extern  int AstralDlg(int iModeless,unsigned int hInstance,unsigned int hWndParent,int idDlgIn,int (pascal far *lpDlgControl)());
extern  int AstralDlgEnd(unsigned int hDlg,int iResult);
extern  int AstralDlgMsg(struct tagMSG far *lpMsg);
extern  int AstralDlgClear(unsigned int hDlg);
extern  unsigned int AstralDlgGet(int idDlg);
extern  int AstralDlgGetID(unsigned int hDlg);
extern  int AstralDlgCheck(unsigned int hDlg);
extern  int AstralDlgSet(int idDlg,unsigned int hDlg);
extern  void AstralDlgShow(int idDlg);
extern  void AstralDlgHide(int idDlg);
extern  void AstralDlgIcon(int idDlg);
extern  void NewSelection(unsigned int hWnd,unsigned int hDC,struct tagRECT far *lpOldRect,struct tagRECT far *lpNewRect,int fFlags);
extern  void StartSelection(unsigned int hWnd,unsigned int hDC,struct tagPOINT ptCurrent,struct tagRECT far *lpSelectRect,int fFlags);
extern  void UpdateSelection(unsigned int hWnd,unsigned int hDC,struct tagPOINT ptCurrent,struct tagRECT far *lpSelectRect,int fFlags);
extern  void EndSelection(unsigned int hWnd,unsigned int hDC,struct tagPOINT ptCurrent,struct tagRECT far *lpSelectRect,int fFlags);
extern  int InvertSelection(unsigned int hWnd,unsigned int hDC,struct tagRECT far *lpSelectRect,int fFlags);
extern  void ConstrainHandles(struct tagPOINT ptCurrent,struct tagRECT far *lpSelectRect);
extern  void Help(unsigned int idTopic,unsigned long lParam);
extern  int IsDemo(char *Program);
static  int DrawHandles(unsigned int hDC,struct tagRECT far *lpSelectRect);
static  int DrawDottedLines(unsigned int hDC,struct tagRECT far *lpSelectRect,struct tagRECT far *lpWindowRect);

  // dlgtools.obj dlgtools.c
extern  int pascal far DlgTextProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgPaintProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgFreeProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgShieldProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgShieldFloatProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgVignetteProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgEllipticalProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgRectangularProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgMagicProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgFloodProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgTransformerProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgPointerProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgTintFillProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgTextureFillProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgViewProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgProbeProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgCopyToFileProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgSmoothProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgSharpProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgEdgeProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgSpecialProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgMaskLoadProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgMaskSaveProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgApplyUndoProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgScanPrintProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgCalibrateScanProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgCalibratePrintProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  void LoadMeasurements(unsigned int hDlg);
extern  int pascal far DlgCustomBrushProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);

  // doclist.obj doclist.c
extern  int AddDoc(unsigned int doc);
extern  int RemDoc(unsigned int doc);
extern  int IsDoc(unsigned int doc);
extern  int NumDocs(void );
extern  unsigned int GetDoc(int index);

  // edit.obj edit.c
static  int fill_filt_buf(unsigned char far * far *filt_buf,int yline,int filter_size,int far *cur_off);
static  void BlendOr(unsigned char far *src1,unsigned char far *src2,unsigned char far *dst,int xstart,int xend);
extern  struct tagPOINT far *BlendBuildOutline(struct _mask far *lpMask,int *nPoints,int xmin,int xmax);
extern  int DoBlend(void );

  // ems.obj ems.c
extern  int EMSstatus(unsigned int far *wpTotal,unsigned int far *wpAvail);
extern  unsigned char far *EMSmap(unsigned char far *lpMemory);
extern  void EMSclose(void );
extern  unsigned char far *EMSalloc(long lCount);
extern  void EMSfree(unsigned char far *lpMemory);

  // engine.obj engine.c
extern  int LineEngine(struct tagRECT far *lpRect,struct _rgb far *lpRGB,struct _frame far *lpSourceFrame,struct _frame far *lpDestFrame,int (far *lpFunction)(),int TileMode,int Opacity);
extern  int EngineInit(int h,int v,struct _frame far *lpSourceFrame,struct _frame far *lpDestFrame,int (far *lpFunction)(),long FadeOutIncrement,int iSourceTileMode,int x,int y,int fDisplayOnSetting);
extern  void EngineDone(void );
extern  void FadeBrush(int fSetupMask);
extern  void FadeOutBrush(int fSetupMask);
extern  void FadeInBrush(int fSetupMask);
extern  void BltRectList(void );
extern  void EngineStroke(int sx,int sy,int dx,int dy,int curs_x,int curs_y);
extern  void BuildHorzRectList(int x,int y);
extern  void BuildVertRectList(int x,int y);
extern  void ProcessRectList(int x,int y);
extern  void BltRect(struct tagRECT far *lpRect,unsigned char far *pb,struct tagRECT far *lpMergeRect,unsigned char far *lpMergeData,struct tagRECT far *lpSectRect);
extern  void DoStroke(int dx,int dy,int sx,int sy);
extern  void BltToDisplay(int x,int y,int dh,int dv,unsigned char far *pdata,struct tagRECT far *lpRect);
extern  void BltToFrame(int x,int y,int dh,int dv,unsigned char far *pdata,struct tagRECT far *lpRect);
extern  int mload(int xs,int ys,int dx,int dy,unsigned char far *pm,unsigned char far *pd,struct _frame far *lpFrame);
extern  int mload8(int xs,int ys,int dx,int dy,unsigned char far *pm,unsigned char far *pd);
extern  int mload16(int xs,int ys,int dx,int dy,unsigned char far *pm,unsigned int far *pd);
extern  int mload24(int xs,int ys,int dx,int dy,unsigned char far *pm,struct _rgb far *pd);
extern  int bload(int x,int y,int dx,int dy,unsigned char far *pdata,struct _frame far *lpFrame,struct tagRECT far *lpRect,int iTileMode,struct _rgb far *lpRGB);
extern  void copyifset(unsigned char far *lptr1,unsigned char far *lptr2,int count);
extern  void MergeData16(unsigned int far *lpDst,struct tagRECT far *lpDstRect,int DstWidth,unsigned int far *lpSrc,struct tagRECT far *lpSrcRect,int SrcWidth,struct tagRECT far *lpSectRect);
extern  void ProcessData(unsigned char far *lpDst,unsigned char far *lpSrc,unsigned char far *lpMsk,int iCount,int depth);
extern  void ProcessData16(unsigned int far *lpDst,unsigned int far *lpSrc,unsigned char far *lpMsk,int iCount);
extern  void MergeData(unsigned char far *lpDst,struct tagRECT far *lpDstRect,int DstWidth,unsigned char far *lpSrc,struct tagRECT far *lpSrcRect,int SrcWidth,struct tagRECT far *lpSectRect);
static  int InsertElement(struct _element far *lpElement);
static  int AddElement(struct _element far *lpElement);
static  int NumElements(void );
static  struct _element far *GetElement(int index);
static  int RemElement(int index);
static  void DestroyElements(void );

  // epswrt.obj epswrt.c
extern  int epswrt(unsigned char far *ifile,struct _frame far *lpSomedayFrame,struct tagRECT far *lpSomedayRect,int DataType,int fCompressed,unsigned char far *SplitDir);
extern  int asciize(unsigned char far *in,int count,unsigned char far *out);
extern  void PSout(unsigned char far *lpFormat,...);

  // extname.obj extname.c
extern  void InitExtName(unsigned int hWnd,unsigned int idControl,unsigned int idStatic,unsigned char far *lpExtName,unsigned int wType);
static  void AddToComboBox(unsigned char far *lpExtName,unsigned char far *lpFileName,unsigned int wType,long lData);
static  void AddToListBox(unsigned char far *lpExtName,unsigned char far *lpFileName,unsigned int wType,long lData);
extern  void GetExtName(unsigned int hWnd,unsigned int idControl,unsigned int idStatic,unsigned char far *lpExtName,unsigned int wType,unsigned int wMsg);
extern  void ExtNameManager(unsigned int hWnd,unsigned int idControl,unsigned int idStatic,unsigned char far *lpExtName,unsigned int wType,unsigned int idFunction,int fLinkedFiles);
static  void AddExtName(unsigned int hWnd,unsigned int idControl,unsigned int idStatic,unsigned char far *lpExtName,unsigned int wType);
static  void RenameExtName(unsigned int hWnd,unsigned int idControl,unsigned int idStatic,unsigned char far *lpExtName,unsigned int wType,int fLinkedFiles);
static  void DeleteExtName(unsigned int hWnd,unsigned int idControl,unsigned int idStatic,unsigned char far *lpExtName,unsigned int wType,int fLinkedFiles);
extern  int PromptForExtName(unsigned int hWnd,unsigned char far *lpExtName,unsigned char far *lpFileName,unsigned int wType);
extern  int pascal far DlgAddExtNameProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int AddExtFile(unsigned char far *lpExtName,unsigned char far *lpFileName,unsigned int wType);
extern  int LookupExtFile(unsigned char far *lpExtName,unsigned char far *lpFileName,unsigned int wType);
extern  int DeleteExtFile(unsigned char far *lpExtName,unsigned int wType,int fLinkedFiles);
extern  int RenameExtFile(unsigned char far *lpExtName,unsigned char far *lpNewExtName,unsigned int wType,int fLinkedFiles);
extern  int ListExtFiles(unsigned int wType,int (far *lpAddFunction)(),long lData);
extern  int GetExtFile(unsigned char far *lpExtName,unsigned char far *lpFileName,int idExtension,unsigned char far *lpNewFileName);
static  int OpenExtNameFile(char *mode);
static  int CloseExtNameFile(void );
static  int GetExtNamePath(char *path);
static  unsigned int GetExtNameData(char *ext_name,char *file_name);
static  int DeleteExtFileEntry(unsigned char far *lpExtName,unsigned int wType);
static  void GetExtFilePath(unsigned char far *lpFileName);
extern  unsigned char far *stripfile(unsigned char far *lpPathName);
extern  unsigned char far *stripext(unsigned char far *lpFileName);
static  int GetExtension(char *file_name,char *extension);
static  unsigned char near *GetTypeString(unsigned int wType);
static  unsigned int GetTypeWord(unsigned char far *lpType);
static  int IsCombo(unsigned int hWnd);

  // fb.obj fb.c
extern  void FreeLinePool(struct _linepool far *lpLinePool);
extern  int AllocLinePool(int UseEMS,long lMemWanted,long lMemMinimum,struct _linepool far *lpLinePool);
extern  struct _frame far *frame_open(unsigned int Depth,unsigned int x_size,unsigned int y_size,unsigned int resolution);
static  void AddLink(int y);
static  void RemoveLink(int y);
static  void TouchLink(int y);
extern  int Age(int age);
static  int CacheRelease(long lSize);
static  int FreeLine(unsigned char far *lpMem);
static  unsigned char far *AllocLine(void );
extern  void InitFreeList(struct _frame far *frame);
extern  void frame_mode(int ReleaseMode);
extern  void frame_close(struct _frame far *lpFrame);
extern  int frame_flush(void );
extern  struct _frame far *frame_set(struct _frame far *lpNewFrame);
extern  int frame_inmemory(void );
extern  int frame_write(struct _span far *span,unsigned char far *pixels,int count);
extern  int frame_read(struct _span far *span,unsigned char far *pixels,int count);
extern  unsigned char far *frame_ptr(int Depth,int x,int y,int bModify);
extern  unsigned char near *cache_name(int n);
static  int horiz_write(struct _span far *span,unsigned char far *pixels,int count);
static  int horiz_Mwrite(struct _span far *span,unsigned char far *pixels,int count);
static  int horiz_read(struct _span far *span,unsigned char far *pixels,int count);
static  int horiz_Mread(struct _span far *span,unsigned char far *pixels,int count);
extern  int frame_mask(struct _span far *span,unsigned char far *pixels,int count);
extern  int frame_preload(int fp,int invert);
extern  void frame_sample(unsigned char far *input,int inskip,unsigned char far *output,int outskip,int count,long rate);
extern  long frame_tocache(struct _rgb far *lpFullRGB);
extern  void frame_getpixelHSL(int dummy,long color,struct _hsl far *lpHSL);
extern  void frame_getpixelRGB(int dummy,long color,struct _rgb far *lpRGB);
extern  void frame_mirror(unsigned char far *buf,int count);
extern  void frame_kernal(unsigned char far *line1,unsigned char far *line2,unsigned char far *line3,int iCount,struct _kernal *pKernal,unsigned char far *Output);
extern  long frame_sharpsmooth(int Type,int xc,unsigned char far *buf1,unsigned char far *buf2,unsigned char far *buf3,int Strength);
extern  void frame_flood(unsigned int hDC,int x,int y);
extern  long frame_edgevalue(int x,int y,int FromDir,int dx,int dy,int depth);
extern  void frame_setpixels(unsigned char far *ptr,int count,long value);
extern  void frame_mappixels(unsigned char far *lpSrc,unsigned char far *lpMask,int iCount);
extern  int frame_width(void );
extern  int frame_depth(void );
extern  int frame_clip(int y,int far *left,int far *right);
extern  unsigned char far *BumpPtr(unsigned char far *lp,unsigned long lOffset);
extern  long ToCache8(struct _rgb far *lpFullRGB);
extern  long ToCache16(struct _rgb far *lpFullRGB);
extern  long ToCache24(struct _rgb far *lpFullRGB);
extern  void getpixelHSL8(long color,struct _hsl far *lpHSL);
extern  void getpixelRGB8(long color,struct _rgb far *lpRGB);
extern  void getpixelHSL16(long color,struct _hsl far *lpHSL);
extern  void getpixelRGB16(long color,struct _rgb far *lpRGB);
extern  void getpixelHSL24(long color,struct _hsl far *lpHSL);
extern  void getpixelRGB24(long color,struct _rgb far *lpRGB);
extern  int MapPixels8(unsigned char far *lpSrc,unsigned char far *lpMask,int iCount);
extern  int MapPixels16(unsigned int far *lpSrc,unsigned char far *lpMask,int iCount);
extern  int MapPixels24(struct _rgb far *lpSrc,unsigned char far *lpMask,int iCount);
extern  void frame_copyline(struct _frame far *lpInFrame,struct _frame far *lpOutFrame,int y);
extern  unsigned int GetAHINCR(void );
extern  int readimage(unsigned int lineno,unsigned int xstart,unsigned int xend,unsigned int numpix,unsigned char far *buffer);
extern  int writeimage(unsigned int lineno,unsigned int xstart,unsigned int numpix,unsigned char far *buffer);

  // filt.obj filt.c
extern  int FilterImage(char far *lpFilterFile);
extern  int FilterImageDLL(char far *lpFilterFile);
extern  int FilterImageKernal(char far *lpFilterFile);
extern  int Convolve(struct _kernal *pKernal,int total);
extern  void kernal8(unsigned char far *Line1,unsigned char far *Line2,unsigned char far *Line3,int iCount,struct _kernal *pKernal,unsigned char far *Output);
extern  void kernal16(unsigned int far *startLine1,unsigned int far *startLine2,unsigned int far *startLine3,int startiCount,struct _kernal *pKernal,unsigned int far *startOutput);
extern  void kernal24(unsigned char far *startLine1,unsigned char far *startLine2,unsigned char far *startLine3,int startiCount,struct _kernal *pKernal,unsigned char far *startOutput);
extern  void BuildLut(int iValue,int far *Lut,int iCount);
extern  void BuildOutLut(int iValue1,int iValue2,int iValue3,int iValue4,char *Outlut,int iCount);
extern  void nullify(char *pBuffer,int iCount);
extern  int DLL_AstralClockCursor(int part,int whole);
extern  unsigned char far *DLL_CachePtr(int Depth,int x,int y,int bModify);
extern  unsigned char far *DLL_frame_set(unsigned char far *lpFrame);
extern  int DLL_FilterWrite(struct _span far *span,unsigned char far *pixels,int count);
extern  int DLL_frame_getRGB(unsigned char far *lpData,struct _rgb far *lpRGB);
extern  int DLL_frame_putRGB(unsigned char far *lpData,struct _rgb far *lpRGB);

  // filter.obj filter.c
extern  long sharpsmooth8(int Type,int xc,unsigned char far *buf1,unsigned char far *buf2,unsigned char far *buf3,int strength);
extern  long sharpsmooth16(int Type,int xc,unsigned int far *buf1,unsigned int far *buf2,unsigned int far *buf3,int strength);
extern  long sharpsmooth16L(int Type,int xc,unsigned int far *buf1,unsigned int far *buf2,unsigned int far *buf3,int strength);
extern  long sharpsmooth24(int Type,int xc,unsigned char far *startBuf1,unsigned char far *startBuf2,unsigned char far *startBuf3,int strength);
extern  long sharpsmooth24L(int Type,int xc,unsigned char far *buf1,unsigned char far *buf2,unsigned char far *buf3,int strength);

  // flood.obj flood.c
extern  void Flood8(unsigned int hmyDC,int x,int y);
extern  void Flood16(unsigned int hmyDC,int x,int y);
extern  void Flood24(unsigned int hmyDC,int x,int y);
static  int do_region_fill8(int x,int y,int dir,int pxl,int pxr);
static  int do_region_fill16(int x,int y,int dir,int pxl,int pxr);
static  int do_region_fill24(int x,int y,int dir,int pxl,int pxr);
static  long CDistance(unsigned int v);
static  long CDistance24(long v);

  // fonts.obj fonts.c
extern  int pascal far EnumFunc(struct tagLOGFONT far *lpLogFont,struct tagTEXTMETRIC far *lpTextMetric,short FontType,unsigned char far *lpData);
extern  void GetFonts(unsigned int hWnd);
extern  void SortFonts(void );
extern  void GetSizes(unsigned int hWnd,int CurrentFont);
extern  short GetStringExtent(unsigned int hDC,unsigned char far *lpString,unsigned int hFont);
extern  unsigned int StringOut(unsigned int hDC,int x,int y,unsigned char far *lpString,unsigned int hFont);

  // gifrdr.obj gifrdr.c
extern  int gifReadHeader(struct _filebuf *ifd,struct _gifhdr *hdr);
extern  int gifReadImDesc(struct _filebuf *ifd,struct _gifdesc *ImDesc);
extern  void gifInitColorMap(struct _gifmap *map);
extern  int gifReadColorMap(struct _filebuf *ifd,int bpp,struct _gifmap *map);
extern  struct _frame far *gifrdr(unsigned char far *ifile,int outdepth,int far *lpBPP);

  // gifwrt.obj gifwrt.c
extern  int gifWriteImDesc(struct _filebuf *ofd,struct _gifdesc *imDesc,struct _gifmap *ColorMap,int codeSize);
extern  int gifwrt(unsigned char far *ifile,struct _frame far *lpFrame,struct tagRECT far *lpRect,int flag,int fCompressed);

  // grab.obj grab.c
extern  void back_to_windows(void );
extern  void grabber(int port,unsigned int hInstance,unsigned int hWindow);
extern  unsigned int GetA000H(void );

  // getdev.obj getdev.c
extern  short InitPrinterListBox(unsigned int hDlg,unsigned char far *lpDevice);
extern  void GetPrinterListBox(unsigned int hDlg,unsigned char far *lpDevice);
extern  void SetDefaultPrinter(unsigned char far *lpDevice);
extern  short SetupPrinter(unsigned int hWindow,unsigned char far *lpDevice);
extern  void DeviceDescription(unsigned char far *lpDevice,unsigned char far *lpDescription);
static  unsigned char far *GetNextParm(unsigned char far *lpString);

  // hsl.obj hsl.c
extern  unsigned char MiniRGBtoL(unsigned int miniRGB);
extern  unsigned char RGBtoL(struct _rgb far *lpRGB);
extern  void MiniRGBtoHSL(unsigned int miniRGB,struct _hsl far *lpHSL);
extern  unsigned int HSLtoMiniRGB(struct _hsl far *lpHSL);
extern  void RGBtoHSL(unsigned char R,unsigned char G,unsigned char B,struct _hsl far *lpHSL);
extern  void HSLtoRGB(unsigned char hue,unsigned char sat,unsigned char lum,struct _rgb far *lpRGB);

  // imgutil.obj imgutil.c
extern  void ConvertData(unsigned char far *lpIn,int iInDepth,int iCount,unsigned char far *lpOut,int iOutDepth);
extern  void con1to8(unsigned char far *lpIn,int iCount,unsigned char far *lpOut);
extern  void con1to16(unsigned char far *lpIn,int iCount,unsigned int far *lpOut);
extern  void con1to24(unsigned char far *lpIn,int iCount,struct _rgb far *lpOut);
extern  void con8to1(unsigned char far *lpIn,int iCount,unsigned char far *lpOut);
extern  void con8to16(unsigned char far *lpIn,int iCount,unsigned int far *lpOut);
extern  void con8to24(unsigned char far *lpIn,int iCount,struct _rgb far *lpOut);
extern  void con16to1(unsigned int far *lpIn,int iCount,unsigned char far *lpOut);
extern  void con16to8(unsigned int far *lpMiniRGB,int iCount,unsigned char far *lpOut);
extern  void con16to24(unsigned int far *lpMiniRGB,int iCount,struct _rgb far *lpRGB);
extern  void con24to1(struct _rgb far *lpIn,int iCount,unsigned char far *lpOut);
extern  void con24to8(struct _rgb far *lpIn,int iCount,unsigned char far *lpOut);
extern  void con24to16(struct _rgb far *lpRGB,int iCount,unsigned int far *lpMiniRGB);
extern  int CreateHisto(struct _frame far *lpFrame,unsigned int far * far *lppRHisto,unsigned int far * far *lppGHisto,unsigned int far * far *lppBHisto);

  // intelio.obj intelio.c
extern  void FileFDOpenRdr(struct _filebuf far *ofd,int ofh,unsigned char far *buf,int bsiz);
extern  void FileFDOpenWrt(struct _filebuf far *ofd,int ofh,unsigned char far *buf,int bsiz);
extern  int FileFlush(struct _filebuf far *ofd);
extern  long FileSeek(struct _filebuf far *ofd,long offset,int whence);
extern  int FileWrite(struct _filebuf far *ofd,unsigned char far *buf,int len);
extern  int FileRead(struct _filebuf far *ifd,unsigned char far *buf,int len);
extern  void intelWriteWord(struct _filebuf far *ofd,unsigned int w);
extern  void intelWriteDWord(struct _filebuf far *ofd,unsigned long dw);
extern  void intelReadWord(struct _filebuf far *ifd,unsigned int far *w);
extern  void intelReadDWord(struct _filebuf far *ifd,unsigned long far *dw);
extern  void motWriteWord(struct _filebuf far *ofd,unsigned int w);
extern  void motWriteDWord(struct _filebuf far *ofd,unsigned long dw);
extern  void motReadWord(struct _filebuf far *ifd,unsigned int far *w);
extern  void motReadDWord(struct _filebuf far *ifd,unsigned long far *dw);

  // map.obj map.c
extern  void map(unsigned char far *lut,unsigned char far *buf,int count);
extern  void map16(unsigned int far *lut,unsigned int far *buf,int count);

  // mask.obj mask.c
extern  struct _shape far *BuildShapes(struct tagPOINT far *pnts,int far *nPoints,int nShapes,int fCircle);
extern  struct _mask far *BuildEdges(struct _mask far *lpMask,struct _shape far *lpShape);
extern  struct _mask far *mask_create(struct _mask far *lpMask,struct tagPOINT far *pnts,int far *nPoints,int nShapes,int fCircle);
extern  int mask_compute(struct _mask far *lpMask,int y,int state);
extern  unsigned char far *mask_point(struct _mask far *lpMask,int x);
extern  struct tagPOINT far *mask_getedges(struct _mask far *lpMask,int y);
extern  void FreeUpShapes(struct _shape far *lpShape);
extern  int mask_close(struct _mask far *lpMask);
extern  void mask_link(struct _mask far *lpMask,int type);
extern  void mask_unlink(struct _mask far *lpMask,int type);
extern  int mask_active(void );
extern  int mask_rect(struct tagRECT far *lpRect);
extern  int IsDisplayMasked(int x,int y);
extern  int IsMasked(int x,int y);
extern  int BuildEdgesFromShapes(struct _mask far *lpMask,struct _shape far *lpShape,struct tagPOINT far *lpInEdges,int (far *lpDxEdgeProc)(),int (far *lpDyEdgeProc)(),int fEstimate);
static  int BuildEdgesFromVector(struct tagPOINT far *lpEdge,struct tagPOINT far *pnt1,struct tagPOINT far *pnt2,int (far *lpDxEdgeProc)(),int (far *lpDyEdgeProc)());
static  void BuildEdgeProc(int x,int y);
extern  void BuildMarqueeProc(int x,int y);
extern  void BuildDyMarqueeProc(int x,int y);
extern  void BuildDxMarqueeProc(int x,int y);
extern  int CompareEdges(struct tagPOINT far *lpEdge1,struct tagPOINT far *lpEdge2);
extern  int BezierToPoints(struct _tform far *lpTForm,struct tagPOINT far *lpPoints,int nPoints,struct tagPOINT far *lpOutputPoints);
extern  int ShapeConvertPoints(struct _shape far *lpShape,struct tagPOINT far *lpOutputPoint);
extern  void SetNewMask(struct tagPOINT far *lpPoint,int far *count,int nShapes,int fCircle);
extern  void SetDrawnMask(struct tagPOINT far *lpPoint,int count,int fCircle);
extern  void RemoveMask(void );

  // maskio.obj maskio.c
extern  int WriteShapes(int fp,struct _mask far *lpMask,int fRelative);
extern  struct _shape far *ReadShapes(int fp,int fSwap);
extern  int WriteMask(char far *szClipName,struct _mask far *lpMask,int fRelative);
extern  struct _shape far *ReadMask(char far *szClipName);

  // marquee.obj marquee.c
extern  void Marquee(int fDisplayAll);
extern  void DottedLine(unsigned int hDC,int x1,int y1,int x2,int y2,int idPattern);
extern  void pascal far DottedLineProc(int x,int y,unsigned char far *lpData);
extern  void BuildMarquee(void );

  // message.obj message.c
extern  int AstralStr(int idString,unsigned char far * far *lppString);
extern  void dbg(unsigned char far *lpFormat,...);
extern  void dbgv(unsigned char far *lpFormat,unsigned char far *lpArguments);
extern  int Print(unsigned char far *lpFormat,...);
extern  int Message(int idStr,...);
extern  void PrintStatus(unsigned char far *lpFormat,...);
extern  void MessageStatus(int idStr,...);
extern  void ColorStatus(struct _rgb far *lpRGB,long lCount);
extern  void StatusOfRectangle(struct tagRECT far *lpRect);
extern  void Status(unsigned char far *lpString);
extern  int AstralConfirm(int idStr,...);
extern  int AstralOKCancel(int idStr,...);
extern  void StatusBox(int idStr,...);

  // mgxbtn.obj mgxbtn.c
extern  long pascal far MgxBtnWndFn(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern BOOL FAR PASCAL register_mgxbtn_class (HANDLE hInst, BOOL bGlobal);

  // paste.obj paste.c
extern  int pascal far DlgPasteFromFileProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int PasteProc(unsigned int hWindow,long lParam,int msg);
extern  int InitPaste(void );
extern  void PasteRefresh(void );
extern  void PasteRedisplay(void );
extern  void DonePaste(int fCancel);
extern  void SetPasteMask(struct _shape far *lpHeadShape,struct _tform far *lpTForm);
extern  int ApplyPaste(struct _tform far *lpTForm);
extern  int PasteFloatProc(int yline,int xleft,int xright,unsigned char far *lpDst,unsigned char far *lpSrc,unsigned char far *lpMask);

  // path.obj path.c
extern  unsigned long pascal far PathControl(unsigned int hPath,unsigned int Message,unsigned int wParm,long lParm);
static  short pascal near get_path_text(unsigned int hPath,short Xpos,char far *lpText,short MaxChars);
static  void pascal near highlight_path(unsigned int hPath,short nHighlight);
static  void pascal near path_mouse(unsigned int hPath,unsigned int Message,unsigned int Flags,struct tagPOINT far *lpPos);
static  void pascal near path_paint(unsigned int hWnd);

  // pcxrdr.obj pcxrdr.c
static  int pcxReadHeader(struct _filebuf *ifd,struct _pcxhdr *hdr);
static  int pcxReadPlane(struct _filebuf *ifd,int bpl,unsigned char far *buffer);
static  int pcxReadRow(struct _filebuf *ifd,struct _pcxhdr far *hdr,unsigned char far *buffer);
extern  struct _frame far *pcxrdr(unsigned char far *ifile,int outdepth,int far *lpBPP);

  // pcxwrt.obj pcxwrt.c
static  void pcxEncPut(struct _filebuf *ofd,unsigned char cnt,unsigned char byte);
static  void pcxEncLine(struct _filebuf *ofd,unsigned char far *buf,int len);
extern  int pcxwrt(unsigned char far *ifile,struct _frame far *lpFrame,struct tagRECT far *lpRect,int flag,int fCompressed);

  // pickrgb.obj pickrgb.c
extern  int ColorPicker(struct _rgb far *lpRGB);
extern  int pascal far DlgColorPickerProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  long pascal far PickerControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  void DrawColorPicker(unsigned int hDC,struct tagRECT far *lpRect,int Hue,struct tagRECT far *lpRepairRect);
extern  void DrawColorPickerCross(unsigned int hWindow,long lValue,int on);

  // popups.obj popups.c
extern  void PopupMenu(unsigned int hWnd,unsigned int idControl,unsigned int idMenuChange);
extern  unsigned int InitPopupTools(void );
extern  void PopupTools(unsigned int hWnd,unsigned int idTool);
extern  void MeasurePopupMenu(struct tagMEASUREITEMSTRUCT far *lpMeasure);
extern  void DrawPopupMenu(struct tagDRAWITEMSTRUCT far *lpDraw);
extern  void MeasurePopupBrush(unsigned int hWnd,struct tagMEASUREITEMSTRUCT far *lpMeasure);
extern  void DrawPopupBrush(struct tagDRAWITEMSTRUCT far *lpDraw);

  // print.obj print.c
extern  int PrintFile(unsigned int hWnd,unsigned char far *szFile);
extern  void OutputFurniture(unsigned int hDC,struct tagRECT far *lpBandRect,struct tagRECT far *lpRect,int xRes,int yRes,unsigned char cSep);
extern  int PrintLabel(unsigned int hDC,int x,int y,unsigned long Color,int idStr,...);
extern  void OutputRegMark(unsigned int hDC,struct tagRECT far *lpRect);
extern  int pascal far AbortTest(unsigned int hDC,int iCode);
extern  int pascal far DlgFnAbort(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  unsigned int GetPrinterDC(void );
extern  void PrintErr(unsigned int hWnd,int ierr);
extern  int LaserPalette(unsigned char far *lpPalette,int nSize);
extern  int AstralLaserPrint(int xSrc,int ySrc,int dxSrc,int dySrc,int xDest,int yDest,int dxDest,int dyDest,int iPrResX,int iPrResY);
extern  int LaserPrint(unsigned char far *lpFormat,...);

  // process.obj process.c
extern  int process(void );
extern  int DoMirror(int xmirror,int ymirror);
extern  struct _frame far *DoSize(void );
extern  struct _frame far *DoSmartSize(void );
extern  int DoCrop(struct tagRECT far *lpCropRect);
extern  int new(int Depth);
extern  int CreateScratchPad(void );

  // ps.obj ps.c
extern  int PSPrint(unsigned char cSep,int xSrc,int ySrc,int dxSrc,int dySrc,int xDest,int yDest,int dxDest,int dyDest,int iPrResX,int iPrResY);
extern  int SendPSData(int Asciize,unsigned char far *lpBuffer,int dxDest);

  // qsort.obj qsort.c
extern  void qsort(unsigned char far *base,long n,int size,int (far *compare)());
static  void swap(long i,long j);
static  void qsort1(long first,long last);

  // retouch.obj retouch.c
extern  void SetupMask(void );
extern  int SetupCustomBrushMask(void );
extern  void horizontal_mask(int BrushVal);
extern  void vertical_mask(int BrushVal);
extern  void slash_mask(int BrushVal);
extern  void backslash_mask(int BrushVal);
extern  void circle_mask(int BrushVal);
extern  void DrawBrushOutline(int xcenter,int ycenter,int dxBrush,int dyBrush,unsigned char *BrushOutline);
extern  void set4pixels(int xp,int yp,unsigned char *BrushOutline);
extern  void FillBrushOutline(int xcenter,int ycenter,unsigned char *BrushOutline);
extern  void BuildThresholdTable(unsigned char *BrushThreshold);

  // rifrdr.obj rifrdr.c
static  int unrlescan(unsigned char far *rlebuff,unsigned char far *outbuff,short bytes,short recCount);
static  int unhuffscan(unsigned char far *huffbuff,unsigned char far *outbuff,short bytes,short recCount);
static  int rifReadScanLine(struct _filebuf *ifd,short flags,short flat,short bytes,unsigned char far *lpFileBuf,unsigned char far *lpCacheBuf);
static  int rifReadHeader(struct _filebuf *ifd,struct _rifhdr *hdr);
extern  struct _frame far *rifrdr(unsigned char far *ifile,int outdepth,int far *lpBPP);

  // rotate.obj rotate.c
extern  int RotateImage(int Angle,int bSmartSize);
extern  struct _frame far *rotate_image_hq(int far *rpix,int far *rlin);
extern  struct _frame far *rotate_image_lq(int far *rpix,int far *rlin);
extern  void frotate_point(int x,int y,long xc,long yc,long cosine,long sine,long far *rx,long far *ry);
extern  long average_pixel(long xp,long yp,int top,int left,int bottom,int right,int offimage_average);
extern  long average_pixel8(long xp,long yp,int top,int left,int bottom,int right,int offimage_average);
extern  long average_pixel16(long xp,long yp,int top,int left,int bottom,int right,int offimage_average);
extern  long average_pixel24(long xp,long yp,int top,int left,int bottom,int right,int offimage_average);

  // rulers.obj rulers.c
extern	BOOL	CreateRulers(void);
extern	void	DestroyRulers(void);
extern	void	SetupRulers(void);
extern	void	SetupAllRulers(void);
extern	void	SizeRulers(void);
extern	long pascal far RulerControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern	void	PaintRuler(HDC hDC, HWND hWnd);
extern	void	HorzRulerPaint(HDC hDC, HWND hWnd);
extern	void	VertRulerPaint(HDC hDC, HWND hWnd);
extern  void	PaintRulerTick(HDC, HWND, int, int);
extern  void	SetRulerTicks(LONG);

  // saveload.obj saveload.c
extern  struct _frame far *AstralFrameLoad(char far *szName,int depth,int far *lpBPP,int far *lpType);
extern  int AstralFrameSave(unsigned char far *szName,struct _frame far *lpFrame,struct tagRECT far *lpRect,int DataType,int FileType,int fCompressed);
extern  int AstralImageLoad(char far *szName);
extern  void AddRecallImageName(unsigned char far *lpFileName);
extern  int AstralImageSave(unsigned char far *szName);
extern  int LoadMap(struct _map far *lpMap0,struct _map far *lpMap1,struct _map far *lpMap2,struct _map far *lpMap3,unsigned char far *lpFileName);
extern  int SaveMap(struct _map far *lpMap0,struct _map far *lpMap1,struct _map far *lpMap2,struct _map far *lpMap3,unsigned char far *lpFileName);
extern  int LoadPrintStyle(struct _style far *lpStyle,unsigned char far *lpFileName);
extern  int SavePrintStyle(struct _style far *lpStyle,unsigned char far *lpFileName);

  // scan.obj scan.c
extern  int Scanner(void );
extern  int DoScan(int iPercentage);
extern  int DoPreScan(unsigned int hDlg,int bCheck);
extern  long pascal far FrameControl(unsigned int hWindow,unsigned int message,unsigned int wParam,long lParam);
extern  int pascal far DlgScanProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  void DoScanInits(unsigned int hDlg);
extern  int pascal far DlgScanSetupProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgGrabSetupProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int ScanImagePaint(unsigned int hWindow,struct tagRECT far *lpSelectRect);

  // scroll.obj scroll.c
extern  void SetScrollBar(unsigned int hDlg,int idControl,int Value,int MinValue,int MaxValue);
extern  int HandleMiniScroll(unsigned int hDlg,unsigned int wParam,long lParam);
extern  int HandleScrollBar(unsigned int hDlg,int idControl,unsigned int wParam,long lParam,int MinValue,int MaxValue);

  // shape.obj shape.c
extern  void ShapeGetTForm(struct _tform far *lpTForm);
extern  struct _shape far *ShapeMultiSelect(struct _mask far *lpMask,struct tagRECT far *lpRect);
extern  struct _shape far *ShapeSelect(struct _mask far *lpMask,int x,int y);
extern  struct _shape far *ShapeUnlink(struct _mask far *lpMask,struct _shape far *lpShape);
extern  void ShapeLink(struct _mask far *lpMask,struct _shape far *lpShape,struct _shape far *lpPrevShape);
extern  void ShapeNewShapes(struct _shape far *lpShapes);
extern  void ShapeTransformInit(struct _shape far *lpShape,struct _tform far *lpTForm);
extern  void ShapeTransformDone(void );
extern  void ShapeTransformMouseDown(int x,int y);
extern  void ShapeTransformMouseUp(int x,int y);
extern  void ShapeTransformMouseMove(int x,int y);
extern  void ShapeTransformDraw(unsigned int hDC,struct tagRECT far *lpRect,int on,int num_handles);
extern  void ShapeEditInit(struct _shape far *lpShape);
extern  void ShapeEditDone(void );
extern  void ShapeEditMouseDown(int x,int y);
extern  void ShapeEditMouseUp(int x,int y);
extern  void ShapeEditMouseMove(int x,int y);
extern  void ShapeEditDraw(unsigned int hDC,int on);
extern  int ShapeDrawPoint(int on);

  // smart.obj smart.c
extern  int SmartSetup(int xIn,int xOut,int yIn,int yOut,int depth);
extern  void SmartDone(void );
extern  int SmartSize(unsigned char far *lpIn,unsigned char far *lpOut,int far *lpNeedsNewLine);
static  int SmartSizeUp(unsigned char far *lpIn,unsigned char far *lpOut,int far *lpNeedsNewLine);
static  void DoSizeUp(unsigned char far *lpIn,long peak,long far *lpLine);
static  int SmartSizeDown(unsigned char far *lpIn,unsigned char far *lpOut,int far *lpNeedsNewLine);
static  void DoSizeDown(unsigned char far *lpIn,long peak,long far *lpLine,long far *lpNumber);
static  void pair2byte(long far *lpSum,long far *lpNum,unsigned char far *lp,int iCount);
static  void fixed2byte(long far *lpSum,unsigned char far *lp,int iCount);
static  void pair2byteRGB(long far *lpSum,long far *lpNum,unsigned char far *lp,int iCount);
static  void DoSizeDownRGB(unsigned char far *lpIn,long peak,long far *lpLine,long far *lpNumber);
static  int SmartSizeDownRGB(unsigned char far *lpIn,unsigned char far *lpOut,int far *lpNeedsNewLine);
static  void fixed2byteRGB(long far *lpSum,unsigned char far *lp,int iCount);
static  void DoSizeUpRGB(unsigned char far *lpIn,long peak,long far *lpLine);
static  int SmartSizeUpRGB(unsigned char far *lpIn,unsigned char far *lpOut,int far *lpNeedsNewLine);

  // superblt.obj superblt.c
extern  void InitSuperBlt(unsigned int hDC,struct _blt far *lpBlt,int nTotalBits);
static  void Init16BitDevice(struct _blt far *lpBlt);
static  void Init24BitDevice(struct _blt far *lpBlt);
static  void Init256ColorDevice(struct _blt far *lpBlt);
static  void Init16ColorDevice(struct _blt far *lpBlt);
static  unsigned int Create256GrayPalette(void );
static  unsigned int Create256ColorPalette(void );
static  unsigned int Create16GrayPalette(void );
static  unsigned int Create16ColorPalette(void );
extern  void StartSuperBlt(unsigned int hDCDevice,struct _blt far *lpBltDevice,struct tagRECT far *lpRect,int Depth,int MaxLines,int xDivaStart,int yDivaStart);
extern  void SuperBlt(unsigned char far *lp);

  // tform.obj tform.c
extern  void TInit(struct _tform far *lpTForm);
extern  void TMultiply(struct _tform far *lpTFormS1,struct _tform far *lpTFormS2,struct _tform far *lpTFormD);
extern  void TMove(struct _tform far *lpTForm,int tx,int ty);
extern  void TScale(struct _tform far *lpTForm,long sx,long sy);
extern  void TShearX(struct _tform far *lpTForm,int height,int dx);
extern  void TShearY(struct _tform far *lpTForm,int width,int dy);
extern  void TRotate(struct _tform far *lpTForm,int anglex,int angley);
extern  void TInvert(struct _tform far *lpTForm);
extern  void FTransformer(struct _tform far *lpTForm,struct tagPOINT far *lpPoint,long far *lpx,long far *lpy);
extern  void Transformer(struct _tform far *lpTForm,struct tagPOINT far *lpPoint,int far *lpx,int far *lpy);
extern  void TransformRect(struct _tform far *lpTForm,struct tagRECT far *lpRect,struct tagRECT far *lpNewRect);

  // text.obj text.c
extern  int InTextRegion(long v);
extern  int PutTextPixel(int x,int y,long Pixel);
extern  int GetTextPixel(int x,int y,long far *lpPixel);
extern  void DIB2Contone(unsigned char far *lpIn,unsigned char far *lpOut,int width,int height,int rowbytes);
extern  int VectorizeCharacters(char far *lpString,struct tagPOINT far *lpFirstPoint,unsigned char far *lpFirstCode,int dx,int dy,int max_points,int far *npoints);
extern  int DoText(struct tagRECT far *lpTextRect);

  // tgardr.obj tgardr.c
static  void tgaReadHeader(struct _filebuf *ifd,struct _tgahdr *hdr);
static  int CheckPic(struct _tgahdr *head);
extern  void tgaGetPixel(struct _filebuf *ifd,int InputBytes,struct _rgb *RGBmap,struct _rgb far *lpRGB);
extern  void tgaGetCPixel(struct _tgastate *tgaState,struct _rgb far *lpRGB);
extern  struct _frame far *tgardr(unsigned char far *ifile,int outdepth,int far *lpBPP);

  // tgawrt.obj tgawrt.c
extern  int tgaComp8Bit(struct _filebuf *ofd,unsigned char far *buf,int npix);
extern  int tgaComp24Bit(struct _filebuf *ofd,unsigned char far *buf,int npix);
extern  int tgawrt(unsigned char far *ifile,struct _frame far *lpFrame,struct tagRECT far *lpRect,int flag,int fCompressed);

  // tifrdr.obj tifrdr.c
extern  void ReadHorzDiff8(unsigned char far *lpImage,int iCount);
extern  void ReadHorzDiff24(unsigned char far *lpImage,int iCount);
extern  void image_read_startstrip(void );
extern  int image_read(unsigned char far *ptr,unsigned int size_of_ptr,int count,int ifh);
extern  struct _frame far *tifrdr(unsigned char far *ifile,int out_depth,int far *lpBPP);
static  void diff_fixtagval(struct _tag far *tagp);
static  void rightjust_tagval(struct _tag far *tagp);
static  int flexiread(int file,unsigned char far *ptr,int size);

  // tifwrt.obj tifwrt.c
extern  int tifwrt(unsigned char far *ifile,struct _frame far *lpFrame,struct tagRECT far *lpRect,int flag,int fCompressed);

  // toolproc.obj toolproc.c
extern  int DrawShapeProc(int y,int left,int right,unsigned char far *lpDst,unsigned char far *lpSrc);
extern  int MaskDisp2File(struct tagPOINT far *lpPoint,int count,int fCircle);
extern  void DrawShape(struct _mask far *lpMask,int fPolyline);
extern  int PaintProc(unsigned int hWindow,long lParam,int msg);
extern  int SprayProc(unsigned int hWindow,long lParam,int msg);
extern  int CloneProc(unsigned int hWindow,long lParam,int msg);
extern  int TextureProc(unsigned int hWindow,long lParam,int msg);
extern  int SharpSmoothProc(unsigned int hWindow,long lParam,int msg);
static  void sharpsmooth_proc(int x,int y,int dx,int dy,unsigned char far *lpDest,unsigned char far *lpSource,unsigned char far *lpMask,int start_x,int start_y,int end_x,int end_y);
extern  int LightenProc(unsigned int hWindow,long lParam,int msg);
extern  int DarkenProc(unsigned int hWindow,long lParam,int msg);
static  void lightendarken_proc(int x,int y,int dx,int dy,unsigned char far *lpDest,unsigned char far *lpSource);
extern  int FloodProc(unsigned int hWindow,long lParam,int msg);
extern  int SmearProc(unsigned int hWindow,long lParam,int msg);
static  void smear_proc(int rx,int ry,int rdx,int rdy,unsigned char far *lpDest,unsigned char far *lpSource,unsigned char far *lpMask,int x1,int y1,int x2,int y2);
extern  int FreeProc(unsigned int hWindow,long lParam,int msg);
extern  int MaskRedrawProc(unsigned int hDC,struct tagRECT far *lpRect,int on);
extern  void vector_draw(int x,int y,int bCapture);
extern  void MaskUndraw(struct tagPOINT far *lpPoint,int far *npoints,int nShapes,int fFileUnits);
extern  void MaskFreeBackup(void );
extern  int CircleProc(unsigned int hWindow,long lParam,int msg);
extern  int SquareProc(unsigned int hWindow,long lParam,int msg);
extern  int CustomProc(unsigned int hWindow,long lParam,int msg);
extern  int MagicProc(unsigned int hWindow,long lParam,int msg);
extern  int TransformerProc(unsigned int hWindow,long lParam,int msg);
extern  int ShapeFloatProc(unsigned int hDC,struct tagRECT far *lpRect,int on);
extern  int PointerProc(unsigned int hWindow,long lParam,int msg);
extern  int ShapeEditProc(unsigned int hDC,struct tagRECT far *lpRect,int on);
extern  int GrabberProc(unsigned int hWindow,long lParam,int msg);
extern  int EraserProc(unsigned int hWindow,long lParam,int msg);
extern  int ProbeProc(unsigned int hWindow,long lParam,int msg);
extern  void SetActiveColor(struct _rgb far *lpRGB);
extern  int ShieldProc(unsigned int hWindow,long lParam,int msg);
extern  int TextProc(unsigned int hWindow,long lParam,int msg);
extern  int VignetteProc(unsigned int hWindow,long lParam,int msg);
extern  int TextureFillProc(unsigned int hWindow,long lParam,int msg);
extern  int TintFillProc(unsigned int hWindow,long lParam,int msg);
extern  void WinBrush(unsigned int hDC,int x,int y,int flag,int newflag,int fTickMarks);
extern  void CreateFuzzTable(unsigned char far *fuzz,int table_size,int FuzzyPixels,int Opacity);
extern  void SetupFuzzyBrush(void );
extern  void CloneBrush(int x,int y,int on);
extern  void DisplayBrush(unsigned int hWnd,int x,int y,int on);

  // tracer.obj tracer.c
extern  long CD24(long v);
extern  int InRegion(long v);
extern  int GetaPixel(int x,int y,long far *lpPixel);
extern  int ChangeDirection(int Direction,int Delta);
extern  int GetNeighbor(struct tagPOINT P,int neighbor,struct tagPOINT far *N,long far *v);
extern  int Tracer(int x,int y,struct tagRECT iRect,int (far *GetPixelProc)(),int (far *PutPixelProc)(),int (far *InRegionProc)(),struct tagPOINT far *lpFirstPoint,unsigned char far *lpFirstCode,int max_points,int DoDisplay,int far *npoints);
extern  int TraceHoles(struct tagRECT iRect,int (far *GetPixelProc)(),int (far *PutPixelProc)(),int (far *InRegionProc)(),struct tagPOINT far *lpFirstPoint,unsigned char far *lpFirstCode,int max_points,int DoDisplay,int far *npoints);
extern  int DoTracer(int x,int y);

  // track.obj track.c
extern  void TrackInit(int (far *tlpDrawRoutine)(),struct _tform far *lpTForm,struct tagRECT far *lpRect);
extern  void TrackGrab(int x,int y);
extern  int TrackMove(int x,int y);
extern  void TrackUp(int x,int y);
extern  void TrackDone(struct tagRECT *rect,int undraw);
extern  void TrackDraw(struct tagRECT far *lpRect,int on);
extern  int TrackMoved(void );
extern  void TrackGetTForm(struct _tform far *lpTForm);
extern  void TrackTransform(int x,int y);
extern  void TrackRotate(int x,int y,int hx,int hy);
extern  void TrackScale(int x,int y,int hx,int hy);
extern  void TrackShear(int x,int y,int hx,int hy,int ox,int oy);
extern  void TrackDrawRect(struct tagRECT far *lpRect,int on,int num_handles);
extern  void TrackDrawEllipse(struct tagRECT far *lpRect,int on,int num_handles);
extern  void TrackDrawClip(struct tagRECT far *lpRect,int on,int num_handles);
extern  void TrackDrawHandles(unsigned int hDC,struct tagRECT far *lpRect,int on,int num_handles);
extern  void TrackDrawHandle(unsigned int hDC,struct tagRECT far *lpRect,int on);
extern  void TrackHandleMinMax(int far *minx,int far *maxx,int far *miny,int far *maxy);
extern  void TrackCalcShearDxDy(int far *dx,int far *dy,struct tagPOINT thePoint,struct tagPOINT Point1,struct tagPOINT Point2);
extern  int TrackGetAngle(struct tagPOINT center,int x,int y);
extern  void TrackResetHandles(void );
extern  void TrackSetHandles(void );
extern  void TrackDispReset(void );

  // util.obj util.c
extern  int bound(int val,int lower,int upper);
extern  void AstralUnionRect(struct tagRECT far *lpDestRect,struct tagRECT far *lpSrc1Rect,struct tagRECT far *lpSrc2Rect);
extern  int AstralIntersectRect(struct tagRECT far *lpDest,struct tagRECT far *lpSrc1,struct tagRECT far *lpSrc2);
extern  int AstralSubtractRect(struct tagRECT far *lpDest,struct tagRECT far *lpSrc);
extern  void BoundRect(struct tagRECT far *lpRect,int left,int top,int right,int bottom);
extern  void diffuse(int x,int y,int iColor,unsigned char far *lpLut,unsigned char far *lpIn,int count,unsigned char far *lpOut);
extern  void dither(int x,int y,unsigned char far *lpIn,int count,unsigned char far *lpOut);
extern  unsigned char far *stripdir(unsigned char far *pathname);
extern  void lmemcpy(unsigned char far *lpOut,unsigned char far *lpIn,int iCount);
extern  void mirror(unsigned char far *pixels,int count);
extern  void mirror16(unsigned int far *pixels,int count);
extern  void mirror24(unsigned char far *pixels,int count);
extern  long fget(long numer,long denom);
extern  long fmul(long a,long b);
extern  long tmul(long a,long b);
extern  unsigned char far *FixedAscii(long Fixed,unsigned char far *lpAscii);
extern  long AsciiFixed(unsigned char far *lpAscii);
extern  unsigned char near *RGBAscii(struct _rgb far *lpRGB,unsigned char near *pAscii);
extern  void AsciiRGB(unsigned char near *pAscii,struct _rgb far *lpRGB);
extern  long ScaleToFit(int *DestWidth,int *DestHeight,int SrcWidth,int SrcHeight);
extern  void la2con(unsigned char far *lpIn,int iCount,unsigned char far *lpOut,int normal);
extern  void con2la(unsigned char far *lpIn,int iCount,unsigned char far *lpOut);
extern  int FileExists(unsigned char far *szFileName);
extern  unsigned char far *itoh(unsigned int i);
extern  int htoi(unsigned char far *s);
extern  unsigned char far *Lowercase(unsigned char far *lp);
extern  void negate(unsigned char far *buf,long lCount);
extern  void ramp(unsigned char far *lut,int inx1,int outx1,int inx2,int outx2);
extern  void lclr(unsigned char far *ptr,long lCount);
extern  void set16(unsigned int far *ptr,int count,unsigned int value);
extern  void hist(unsigned char far *pixels,int count,long *histo);
extern  unsigned char far *Alloc(long lCount);
extern  unsigned char far *AllocExtend(unsigned char far *lpMemory,long lCount);
extern  void FreeUp(unsigned char far *lpMemory);
extern  unsigned long AvailableMemory(void );
extern  unsigned long ConventionalMemory(void );
extern  void swapw(unsigned int far *word);
extern  void swapl(unsigned long far *dword);
extern  void Unpack4(unsigned char far *lpIn,int iCount,unsigned char far *lpOut);
extern  void ScaleRect(struct tagRECT far *lpRect,long Scale);
extern  void SinCos(int iAngle,long far *lplSin,long far *lplCos);
extern  void Delay(long lTicks);
extern  int DelayIfMouseDown(long lTicks);
extern  unsigned int lsqrt(long lValue);
extern  int CopyFile(unsigned char far *lpInFile,unsigned char far *lpOutFile);
extern  int AllocLines(unsigned char far * far *lpList,int nLines,int iWidth,int iDepth);
extern  unsigned long DiskSpace(unsigned char far *lpPath);
extern  void File2DispTForm(struct _tform far *lpTForm);
extern  void Disp2FileTForm(struct _tform far *lpTForm);
extern  int StringsEqual(unsigned char far *lpString1,unsigned char far *lpString2);
extern  unsigned char far *GetCurrentDir(unsigned char far *lpBuffer,int maxlen);
extern  int SetCurrentDir(unsigned char far *lpCurrentDir);
extern  unsigned int AstralLoadLibrary(unsigned char far *lpLibraryName);

  // view.obj view.c
extern  void Zoom(int x,int y,int value,int fIncremental,int fAllowSizeChange);
extern	void ViewRect(LPRECT);
extern  void NewImageSizeWindow(unsigned int hWindow);
extern  void FullViewWindow(unsigned int hWindow,int x,int y,int cx,int cy,int fClipToImage);
extern  int ComputeDispRect(void );
extern  int GetDispRect(struct tagRECT far *lpRect,int iHorzMode,int iVertMode);
extern  int ComputeFileRect(int x,int y,long FileRate);
extern  void ScrollImage(unsigned int hWnd,unsigned int msg,unsigned int wParam,long lParam);
extern  void SaveLastView(void );
extern  void RevertLastView(void );

  // vignette.obj vignette.c
extern  int vignette_proc(int y,int left,int right,unsigned char far *lpDst,unsigned char far *lpSrc);
extern  void Vignette(int sx1,int sy1,int sx2,int sy2);
extern  void TintFill(struct _rgb far *lpRGB);
extern  void TextureFill(struct _frame far *lpTexture,int TileMode);
extern  int invert_proc(int y,int left,int right,unsigned char far *lpDst,unsigned char far *lpSrc);
extern  void InvertFill(void );

  // winimage.obj winimage.c
extern  void EnablePaintAbort(int fEnable);
extern  void AstralUpdateWindow(unsigned int hWnd);
extern  long pascal far WndImageProc(unsigned int hWnd,unsigned int msg,unsigned int wParam,long lParam);
extern  int UpdateImage(struct tagRECT far *lpRepairRect,int fInvalidate);
extern  int UpdateImageSize(void );
extern  int UpdateMarquee(void );
extern  int AstralImagePaint(unsigned int hWindow);
extern  int PaintBorder(unsigned int hDC,struct tagRECT far *lpRect);
extern  int AbortUpdate(void );
extern  int PaintImage(unsigned int hDC,struct tagRECT far *lpRepairRect,struct tagRECT far *lpInvalidRect);
extern  int PrintImage(unsigned int hDC,struct tagRECT far *lpSourceRect,struct tagRECT far *lpDestRect,int xDiva,int yDiva,int yTop,int yTotal);
extern  void SetTitleBar(unsigned int hWindow);
extern  int SetupScrollBars(unsigned int hWindow);
extern  int SetupRates(unsigned int hWindow);
extern  void Display2File1(int far *lpx,int far *lpy);
extern  void Display2File(int far *lpx,int far *lpy);
extern  void File2Display(int far *lpx,int far *lpy);
extern  void File2DispRect(struct tagRECT far *fileRect,struct tagRECT far *dispRect);
extern  int ConstrainXY(long far *lpMousePos,int fDown,unsigned int msg);
extern  int ConstrainX(void );
extern  int ConstrainY(void );
extern  void DrawRulers(unsigned int hWnd);
extern  void DrawGrowBox(HWND);

  // winprocs.obj winprocs.c
extern  int pascal far DlgStatusProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  void UpdateStatusBar(int bMemory,int bCache,int bMask,int bShield);
extern  int pascal far DlgHeraldProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgWarningProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgAboutProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgPasteProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgMapProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  struct _map *ResetMapChannel(unsigned int hDlg,unsigned int wChannel);
extern  void ActivateMapControls(unsigned int hDlg,unsigned int wTool,unsigned int far *lpOldTool,int bDelay);
extern  int pascal far DlgContBritProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  struct _map *ResetCBChannel(unsigned int hDlg,unsigned int wChannel);
extern  struct _map *ResetPosterChannel(unsigned int hDlg,unsigned int wChannel);
extern  struct _map *ActivateChannel(unsigned int wChannel);
extern  int pascal far DlgPosterizeProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgHueProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  void ResetHues(unsigned int hDlg);
extern  int pascal far DlgQuarterProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  void SetHistoValues(unsigned int hDlg,struct _map *lpMap,unsigned int far *lpRHisto,unsigned int far *lpGHisto,unsigned int far *lpBHisto,int fNotify);
extern  void DoSizeInits(unsigned int hDlg);
extern  void DoNewInits(unsigned int hDlg);
extern  int AstralImageMemory(void );
extern  void DoSizeControls(unsigned int hDlg,unsigned int wParam,long lParam);
extern  int pascal far DlgProcessProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgRotateProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgPrintProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgPrintSetupProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgPrintStyleProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  void ActivateStyleControls(unsigned int hDlg,unsigned int wStyle,unsigned int far *lpOldStyle,int bDelay);
extern  int pascal far DlgMapEditProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgNewProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgToolsProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  void SaveRetouchBrush(int brush);
extern  void SelectRetouchBrush(int brush);
extern  void ActivateTool(unsigned int idTool);
extern  void DeactivateTool(void );
extern  int pascal far DlgPaletteProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgStatusBoxProc(unsigned int hDlg,unsigned int message,unsigned int wParam,long lParam);
extern  int pascal far DlgPrefProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);
extern  int pascal far DlgInfoProc(unsigned int hDlg,unsigned int msg,unsigned int wParam,long lParam);

  // winpaint.obj winpaint.c
extern  int AstralPaint(unsigned int hWindow);
extern  void PaintIcon(unsigned int hWindow,int idIcon);

  // winpal.obj winpal.c
extern  int MapSetupLuts(unsigned char far *HLut,unsigned char far *SLut,unsigned char far *MLut,unsigned char far *RLut,unsigned char far *GLut,unsigned char far *BLut);
extern  void ResetAllMaps(void );
extern  void ResetMap(struct _map far *lpMap,int maxpoints);
extern  void InvertMap(struct _map far *lpMap);
extern  int map_proc(int y,int left,int right,unsigned char far *lpDst,unsigned char far *lpSrc);
extern  void ApplyMap(int dirty);
extern  void MakeMap(struct _map far *lpMap);
extern  void MapRGBint(int far *lpRed,int far *lpGreen,int far *lpBlue);
extern  void MapRGB(unsigned char far *lpRed,unsigned char far *lpGreen,unsigned char far *lpBlue);
extern  int AnimateMap(struct _map far *lpMap);
extern  void MapPreview(void );
extern  void UndoMapPreview(int iFinal);
extern  int MapPreviewProc(int yline,int xleft,int xright,unsigned char far *lpDst,unsigned char far *lpSrc,unsigned char far *lpMask);
extern  int Previewing(void );

  // wintools.obj wintools.c
extern  unsigned long GetWinXY(unsigned int hWnd);
extern  int ColorText(unsigned int hDC,int x,int y,unsigned char far *szStr,int length,unsigned long Color);
extern  void CenterPopup(unsigned int hWnd);
extern  void GetScreenCenter(unsigned int hWnd,struct tagPOINT far *ppt,int dx,int dy);
extern  void RightPopup(unsigned int hWnd);
extern  void StretchPopup(unsigned int hWnd);
extern  void DisableSavebits(unsigned int hWnd);
extern  void EnableSavebits(unsigned int hWnd);
extern  void SetClientRect(unsigned int hWnd,struct tagRECT far *lpRect);
extern  void GetDrawClientRect(unsigned int hWnd,struct tagRECT far *lpRect);
extern  void GetRealClientRect(unsigned int hWnd,struct tagRECT far *lpRect);
extern  void GetScrollBarSize(int far *lpCx,int far *lpCy);
extern  void SetFullClientRect(unsigned int hWnd);
extern  void AstralMenuCheck(int menuID,int *oldmenuID);
extern  void SetDlgItemFixed(unsigned int hDlg,int idDlg,long Fixed,int bNoConvert);
extern  unsigned char far *Unit2String(long Fixed,unsigned char far *lpFixed);
extern  long Pixels2Inches(int Pixels);
extern  long GetDlgItemFixed(unsigned int hDlg,int idDlg,int far *lpTranslated,int bNoConvert);
extern  unsigned char far *WhichEdgeCursor(struct tagPOINT ptPoint,struct tagRECT far *lpRect);
extern  void Dline(unsigned int hDC,int x1,int y1,int x2,int y2,int flag);
extern  void Drect(unsigned int hDC,struct tagRECT far *lpRect,int flag);
extern  void SetKeyState(int nVirtKey,int fDown,int fToggled);
extern  void ShowControl(unsigned int hDlg,unsigned int idControl1,unsigned int idControl2,int bShow,int bDefer);
extern  void CheckComboItem(unsigned int hWnd,int idComboBox,int idFirstItem,int idLastItem,int idSelectedItem);
extern  void EraseDialogBackground(unsigned int hDC,unsigned int hDlg);
extern  unsigned int SetControlColors(unsigned int hDC,unsigned int hDlg,unsigned int hControl,unsigned int wType);
extern  void DlgOptionPaint(unsigned int hDlg);
extern void HilightRect(HDC,LPRECT,HBRUSH);

