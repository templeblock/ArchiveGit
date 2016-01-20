/*®FD1¯®PL1¯®TP0¯®BT0¯®RM240¯*/
#ifndef __share
#define __share

#include "types.h"
#include "select.h"
#include "maskio.h"
#include "wintools.h"
#include "cursors.h"
#include "browser.h"
#include "extbsr.h"
#include "effects.h"
#include "color.h"
#include "saveload.h"
#include "extname.h"
#include "flood.h"
#include "message.h"
#include "tablet.h"

// Used in tracer.c
typedef BOOL (FAR *LPGETPIXELPROC)(int x, int y, LPLONG lpPixel);
typedef BOOL (FAR *LPPUTPIXELPROC)(int x, int y, long Pixel);
typedef BOOL (FAR *LPADDPROC)(LPPOINT lpPoints, int nPoints);

// Source file: about.c
BOOL WINPROC DlgAboutProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

// Source file: blend.c
BOOL DoBlend ( LPFRAME lpFrame, LPMASK lpMask, int Pressure );

// Source file: bmpwrt.c
void invertData(LPTR lpData, int count);
void Dither16( LPTR lpInBuf, LPTR lpOutBuf, int iPixelCount, int iRow);

// Source file: calib.c
long Measure( LPIMAGE lpImage, LPRECT lpRect, LPCOLORINFO lpColorInfo );
long MeasureRGB( LPIMAGE lpImage, LPRECT lpRect, LPRGB lpRGB );

// Source file: circops.c
void ellipse(int xcenter,int ycenter,int a0,int b0,int on_off,int special_flag);

// Source file: color.c
LPFRAME ConvertLongColor( LPFRAME lpFrame );
void ConvertLongColor24( LPFRAME lpFrame, LPFRAME lpNewFrame );
void SetEachThirdValue( LPTR lpIn, LPTR lpOut, int iCount );
void GetEachThirdValue( LPTR lpIn, LPTR lpOut, int iCount );
LPFRAME ConvertWideColor( LPFRAME lpFrame );
void UnshuffleRGB( LPRGB lpRGB, LPTR lpRed, LPTR lpGreen, LPTR lpBlue, int iCount );
void InitRGBtoCMYK( void );
VOID CorrectRGB( LPRGB lpRGB, int iCount, BOOL fCalibrate, BOOL fPS);
VOID CorrectGray( LPTR lpGray, int iCount, BOOL fCalibrate, BOOL fPS);

VOID ClrRGBtoCMYK( LPRGB lpRGB, LPTR lpCyan, LPTR lpMagenta, LPTR lpYellow, LPTR lpBlack, int iCount, BOOL fCalibrate);
VOID ClrCMYKtoRGB( LPTR lpCyan, LPTR lpMagenta, LPTR lpYellow, LPTR lpBlack, LPRGB lpRGB, int iCount );

// Source file: combo.c

void CheckComboItem( HWND hWnd, int idComboBox, int idFirstItem, int idLastItem, int idSelectedItem );
void TableCheckComboItem( HWND hWnd, int idComboBox, LPINT lpItemID, int nItems, int idSelectedItem, BOOL bUseStrings = TRUE );
ITEMID HandleCombo( HWND hDlg, ITEMID idCombo, UINT codeNotify );
int TableHandleCombo( HWND hDlg, LPINT lpItemID, int nItems, ITEMID idCombo, UINT codeNotify );
void MeasureItem( HWND hDlg, LPMEASUREITEMSTRUCT lpMeasure, BOOL fIcon );
void DrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR * lpDraw, int fFlags );
void DrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR * lpDraw, int fFlags, BOOL fNoCategories );
void InfoMeasureItem( HWND hDlg, MEASUREITEMSTRUCT FAR *lpMeasure );
void InfoDrawItem( HWND hDlg, const DRAWITEMSTRUCT FAR *lpDraw );

#define OD_LOWERCASE 0x0001
void CombineStringID( LPTSTR lpString, WORD wId );
int ExtractStringID( LPTSTR lpString );

// Source file: cut.c
HANDLE ConstructMGXImage( LPTSTR lpImageFile);
HANDLE ConstructPP4MGXImage( LPTR lpImageFile, LPTR lpMaskFile );
HANDLE FrameToBitmap( LPFRAME lpFrame, LPRECT lpRect );
HANDLE ConstructPalette( HPALETTE hPal );
LPOBJECT GetClipboardObject(LPIMAGE lpImage = NULL, LPTSTR lpFileName = NULL,
    LPDWORD lpImageID = NULL, LPFRMTYPEINFO lpSrcTypeInfo = NULL, int cfFormat = -1);
LPOBJECT GetObjectFromInternalData( LPIMAGE lpDstImage, LPIMAGE lpSrcImage, BOOL fClipboard,
							 LPTSTR lpFileName, LPFRMTYPEINFO lpSrcTypeInfo);
LPOBJECT GetObjectFromExternalData( LPIMAGE lpDstImage,
									LPTSTR lpImageFile, LPTSTR lpMaskFile,
									BOOL fClipboard,
							 		LPTSTR lpFileName,
							 		LPFRMTYPEINFO lpSrcTypeInfo);
BOOL PasteDropData(UINT cfFormat, HANDLE hData, LPTSTR lpImageFile, LPTSTR lpMaskFile );
BOOL PasteFromClipboard( HWND hWindow, LPTSTR lpImageFile, LPTSTR lpMaskFile, int cbFormat = -1 );
HANDLE ConstructObject( LPIMAGE lpImage, LPRECT lpRect );
LPBITMAPINFOHEADER BitmapToDIB( HDC hDC, HBITMAP hBitmap, HPALETTE hPal );
LPFRAME BitmapToFrame( HDC hDC, HBITMAP hBitmap, HPALETTE hPal );

// Source file: dda.c
void ddaline( int x1, int y1, int x2, int y2, void (FAR *lpPointFunction)(int x, int y) );

// Source file: doclist.c
BOOL IsDoc(HWND doc);

// Source file: drag.c
LPDRAG DragAndDropBegin(HWND hWnd, WORD wType, LPTR lpData, LPPOINT lpStartPos, HRGN hRgnIn, LPRECT lpRect);
BOOL DragAndDropMove(LPDRAG lpDrag, int x, int y);
BOOL DragAndDropEnd(LPDRAG lpDrag);
LPDRAG DragAndDrop(HWND hWnd, WORD wType, LPTR lpData, LPPOINT lpStartPos, HRGN hRgnIn, LPRECT lpRect);

// Source file: epswrt.c
int dcswrt( LPTSTR lpOutFile, LPOBJECT lpObject, LPFRAME lpFrame, LPRECT lpRect, int DataType, BOOL fCompressed );
int epswrt( LPTSTR lpOutFile, LPOBJECT lpObject, LPFRAME lpFrame, LPRECT lpRect, int DataType, BOOL fCompressed );
int asciize(LPTR in,int count,LPTSTR out);
BOOL WINPROC DlgEpsOptionsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

// Source file: export.c
BOOL ExportImage( HWND hWnd, ITEMID idItem, ITEMID idIniSection );

// Source file: filter.c
void sharp8light(  int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharp8medium( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharp8heavy(  int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth8light( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth8medium(int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth8heavy( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharpsmooth8( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
SHARPSMOOTHPROC GetSharpSmooth8(int Type, int strength);

void sharp24light(  int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharp24medium( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharp24heavy(  int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth24light( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth24medium(int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth24heavy( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharpsmooth24( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
SHARPSMOOTHPROC GetSharpSmooth24(int Type, int strength);

void sharp32light(  int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharp32medium( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharp32heavy(  int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth32light( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth32medium(int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void smooth32heavy( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
void sharpsmooth32( int xc, LPTR buf1, LPTR buf2, LPTR buf3, LPTR lpDst);
SHARPSMOOTHPROC GetSharpSmooth32(int Type, int strength);

// Source file: findfile.c
BOOL FindFile( LPTSTR lpRootDirectory, LPTSTR lpFile, int MaxDepth, LPTSTR lpReturnedDirectory );

// Source file: fonts.c
HFONT CreateLightFont( HFONT hFont );
#ifdef WIN32
int WINPROC EnumFunc(LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetric, DWORD FontType, LPARAM lParam);
#else
int WINPROC EnumFunc(LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetric, short FontType, LPARAM lParam);
#endif
void GetFonts(HWND hWnd);
LPFONTLIST GetFontList(HDC hDC);
void SortFonts(LPFONTLIST lpFontList);
void GetSizes(HWND hWnd, int CurrentFont);
short GetStringExtent(HDC hDC, LPTSTR lpString, HFONT hFont);
WORD StringOut(HDC hDC, int x, int y, LPTSTR lpString, HFONT hFont);
void ShowSampleString(HWND hWnd);
void ShowCharacterSet(HDC hDC, HFONT hFont);
long WINPROC ShowFontWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINPROC Metric(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINPROC Log(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINPROC AddDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINPROC RemoveDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Source file: getdev.c
short InitPrinterListBox( HWND hDlg, LPTSTR lpDevice );
void GetPrinterListBox( HWND hDlg, LPTSTR lpDevice );
void GetDefaultPrinter( LPTSTR lpDevice );
void SetDefaultPrinter( LPTSTR lpDevice );
short SetupPrinter( HWND hWindow, LPTSTR lpDevice );
void ValidatePrinter( LPTSTR lpDevice );
void DeviceDescription( LPTSTR lpDevice, LPTSTR lpDescription );

// Source file: help.c
void Help( int idTopic, LPARAM lParam );
int HintLine( int id );
int HintLineSpecial( int id, int idOffset );
void ColorStatus( LPCOLORINFO lpColor, long lCount, LPTSTR lpLabel, BOOL bDoubleClick);
int Color2Status( LPCOLORINFO lpColor, BOOL fActive );
void StartF1Help( void );
void EndF1Help( void );
void EnableF1Help( BOOL Bool );
DWORD CALLBACK KeyboardHookProc( int nCode, WPARAM wParam, LPARAM lParam );
void StartContextHelp( void );
void EndContextHelp( void );
BOOL InContextHelpMode( void );
DWORD CALLBACK MessageHookProc( int nCode, WPARAM wParam, LPMSG lpMsg );
LPTSTR GetColorString(LPCOLORINFO lpColor, COLORSPACE ColorSpace,
					LPTSTR lpColorString);

// Source file: imgutil.c
void la2con( LPTR lpIn, int iCount, LPTR lpOut, BOOL bNormal );
void con2la( LPTR lpIn, int iCount, LPTR lpOut );
void diffuse( int x, int y, int iColor, LPTR lpLut, LPTR lpIn, int count, LPTR lpOut );
void dither( int x, int y, LPTR lpIn, int count, LPTR lpOut );

// Source file: jpginstl.c
BOOL RetrieveJPG(LPTSTR lpName);
BOOL WINPROC MDAProc (UINT Msg, LPTSTR lpName, int Num, LPVOID lpUser);

// Source file: jpgrdr.c
void FreeJPEGLibrary( void );

// Source file: jpgwrt.c
BOOL WINPROC DlgJPEGOptionsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void LoadJPEGPreferences( void );
void StoreJPEGPreferences( void );
BOOL JPEGAvailable( void );

// Source file: makepath.c
LPCPolyList GetPath(LPRECT lpRect, LPFRAME lpFrame, BYTE cutoff, LPINT InOut);

// Source file: marquee.c
BOOL GetObjectMarqueeRect(LPIMAGE lpImage, LPRECT lpRect);
BOOL GetMaskMarqueeRect(LPIMAGE lpImage, LPRECT lpRect);
BOOL ShowMarquee(LPIMAGE lpImage, BOOL fShow);
BOOL IsMarqueeVisible(LPIMAGE lpImage);
BOOL ConfirmMaskRemove(LPIMAGE lpImage);
BOOL EnableMarquee(BOOL bEnable);
BOOL EnableMarqueeEx(LPIMAGE lpImage, BOOL bEnable);
void RemoveMarquee(void);
void RemoveObjectMarquee(LPIMAGE lpImage);
void RemoveObjectMarqueeEx(LPIMAGE lpImage, BOOL fUpdate);
void RemoveMaskMarquee(LPIMAGE lpImage);
void Marquee(BOOL fDisplayAll);
void EnableMaskDisplay(LPIMAGE lpImage, BOOL fEnable);
void EnableSlime( LPIMAGE lpImage, BOOL fEnable);
BOOL PaintMarquee( HWND hWnd, HDC hDC, LPOBJECT lpObject, LPMASK lpMask, BOOL fDisplayAll, LPRECT lpUpdate, COLOR Color1, COLOR Color2, int xOffset, int yOffset );

// Source file: palio.c
BOOL Palette_Write(LPTSTR lpFileName, LPPALETTE lpPalette);
LPPALETTE Palette_Read(LPTSTR lpFileName, BOOL bCombine);
LPCOLORMAP Palette_ReadColorMap(LPTSTR lpFileName);
BOOL Palette_WriteColorMap(LPTSTR lpFileName, LPCOLORMAP lpColorMap, LPTSTR lpPaletteName);

// Source file: pallist.c
LPPALETTE Palette_New(LPTSTR lpPaletteName);
LPPALETTE Palette_CreateFromColorMap(LPCOLORMAP lpColorMap);
LPPALETTE Palette_Link(LPPALETTE lpHeadPalette, LPPALETTE lpPalette);
void Palette_FreeUp(LPPALETTE lpPalette);
LPPALETTE Palette_Delete(LPPALETTE lpHeadPalette, int iPalette);
int Palette_GetCount(LPPALETTE lpHeadPalette);
LPPALETTE Palette_Get(LPPALETTE lpHeadPalette, LPTSTR lpPaletteName, int index);
BOOL Palette_GetName(LPPALETTE lpHeadPalette, int iPalette, LPTSTR lpPaletteName);
int Palette_GetNum(LPPALETTE lpHeadPalette, LPTSTR lpPaletteName, LPPALETTE lpPalette);
BOOL Palette_Rename(LPPALETTE lpHeadPalette, int iPalette, LPTSTR lpNewName);
BOOL Palette_MoveEntry(LPPALETTE lpHeadPalette, int iPalette, int iOldEntry, int iNewEntry);
BOOL Palette_InsertEntry(LPPALETTE lpHeadPalette, int iPalette, int iBefore, LPCOLORINFO lpColorInfo);
BOOL Palette_AppendEntry(LPPALETTE lpHeadPalette, int iPalette, int iAfter, LPCOLORINFO lpColorInfo);
BOOL Palette_DeleteEntry(LPPALETTE lpHeadPalette, int iPalette, int iEntry);
BOOL Palette_DeleteEntries(LPPALETTE lpHeadPalette, int iPalette, int iStart, int iCount);
BOOL Palette_DeleteLabels(LPPALETTE lpHeadPalette, int iPalette, int iStart, int iCount);
BOOL Palette_AppendAdjustLabels(LPPALETTE lpHeadPalette, int iPalette, int iAfter, int iCount);
BOOL Palette_InsertAdjustLabels(LPPALETTE lpHeadPalette, int iPalette, int iBefore, int iCount);
BOOL Palette_SetEntryLabel(LPPALETTE lpHeadPalette, int iPalette, int iEntry, LPTSTR lpLabel);
BOOL Palette_SetEntryColor(LPPALETTE lpHeadPalette, int iPalette, int iEntry, LPCOLORINFO lpColorInfo);
BOOL Palette_GetEntryColor(LPPALETTE lpHeadPalette, int iPalette, int iEntry, LPCOLORINFO lpColorInfo);
BOOL Palette_GetEntryLabel(LPPALETTE lpHeadPalette, int iPalette, int iEntry, LPTSTR lpColorLabel);
void Palette_FormatEntryLabel(LPPALETTE lpPalette, BYTE Type, LPTSTR lpLabel, LPTSTR lpColorLabel);
BOOL Palette_FillEntries(LPPALETTE lpHeadPalette, int iPalette, int iAfter, LPCOLORINFO lpColor1, LPCOLORINFO lpColor2, int iEntries, BOOL DoHSL);
BOOL Palette_AddEntries(LPPALETTE lpHeadPalette, int iPalette, int iAfter, int iEntries, LPCOLORINFO lpColorInfo);
int Palette_FindEntryLabel(LPPALETTE lpHeadPalette, int iPalette, LPINT lpEntry, LPTSTR lpColorLabel, int iWhich);
BOOL Palette_MatchCriteria(LPTSTR lpLabel, LPTSTR lpCriteria);
LPPALETTE Palette_Combine(LPPALETTE lpHeadPalette);

// Source file: paste.c
BOOL WINPROC DlgPasteFromFileProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL UpdatePasteFromFile(HWND hDlg);

// Source file: path.c
LONG WINPROC Path_Control(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );

// Source file: pickrgb.c
BOOL ColorPicker( LPCOLORINFO lpColor, LPCOLORINFO lpAltColor );
BOOL WINPROC DlgColorPickerProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
long WINPROC Picker_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );

// Source file: popups.c
void PopupMenuEx( HWND hWnd, ITEMID idControl, ITEMID idMenu, ITEMID idMenuChange );
void PopupMenu( HWND hWnd, ITEMID idControl, ITEMID idMenuChange );
BOOL PopupWindow(HWND hParent, int idControl, int idDlg, DLGPROC lpDlgControl);

// Source file: ps.c
BOOL PSPrint( LPIMAGE lpImage, LPFRAME lpFrame, BYTE cSep, int xSrc, int ySrc, int dxSrc, int dySrc, int xDest, int yDest, int dxDest, int dyDest, int iPrResX, int iPrResY );
BOOL SendPSData( BOOL Asciize, LPTR lpBuffer, int dxDest );
void copyPS( LPTR lptr1, LPTR lptr2, int count );

// Source file: process.c
BOOL SizeImage( LPIMAGE lpImage, LPSIZE_PARMS lpParms );
LPFRAME SizeFrame(LPFRAME lpSrcFrame, int iWidth, int iHeight, int iRes, BOOL bSmartSize);
BOOL ExpandImage( LPIMAGE lpImage, LPEXPAND_PARMS lpParms );
LPFRAME ExpandFrame(LPFRAME lpSrcFrame, int iWidth, int iHeight, int iLeft, int iTop, LPCOLORINFO lpColor);
BOOL MirrorImage(LPIMAGE lpImage, LPMIRROR_PARMS lpParms);
LPFRAME DoSize( LPFRAME lpFrame, int pix, int lin, int res );
LPFRAME DoSmartSize( LPFRAME lpFrame, int pix, int lin, int res );
BOOL NewImage( LPIMAGE lpImage, LPNEW_PARMS lpParms );

// Source file: rotate.c
BOOL RotateImage( LPIMAGE lpImage, LPROTATE_PARMS lpParms );
LPFRAME RotateFrame( LPFRAME lpSrcFrame, LFIXED Angle, LPCOLORINFO lpBGColor, BOOL bSmartSize );
long average_pixel(LPFRAME lpFrame, LFIXED xp, LFIXED yp, int depth, int top, int left, int bottom, int right, BOOL offimage_average, COLOR bgPixel);
void average_pixel8(LPFRAME lpFrame, LFIXED xp,LFIXED yp,int top,int left,int bottom,int right,int offimage_average, BYTE bgPixel, LPTR lpNewPixel);
void average_pixel24(LPFRAME lpFrame, LFIXED xp,LFIXED yp,int top,int left,int bottom,int right,int offimage_average, RGBS bgPixel, LPRGB lpNewPixel);
void average_pixel32(LPFRAME lpFrame, LFIXED xp,LFIXED yp,int top,int left,int bottom,int right,int offimage_average, CMYKS bgPixel, LPCMYK lpNewPixel);

// Source file: rulers.c
void EnableRulers(BOOL bEnable);
void UpdateRulers( HWND hWnd );
int GetRulerSize( void );
BOOL CreateRulers( HWND hWnd );
void DestroyRulers( HWND hWnd );
void SetupAllRulers( void );
void SetupRulers( HWND hWnd );
void SetRulerTicks(HWND hWnd, int x, int y);
void SizeRulers( HWND hWnd );
long WINPROC Ruler_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );

// Source file: runtask.c
BOOL RunTask( HWND hWnd, ITEMID idItem, ITEMID idIniSection );
BOOL SetupRunMenus( HMENU hMenu, int iFileItem, ITEMID idItem, int iMaxItems, ITEMID idIniSection );
BOOL CALLBACK EXPORT EnumTaskCallback( HWND hWnd, LPARAM lParam );

 
// Source file: scroll.c
void SetScrollBar( HWND hDlg, int idControl, int Value, int MinValue, int MaxValue );
int HandleScrollBar( HWND hDlg, int idControl, WPARAM wParam, LPARAM lParam, int MinValue, int MaxValue );

// Source file: smart.c
LPVOID SmartSetup (int xIn, int xOut, int yIn, int yOut, int depth);
void SmartDone (LPVOID lpSmart);
BOOL SmartSize (LPVOID lpSmart, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine);
void pair2byte (LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount);

// Source file: tracer.c
int Tracer(int x, int y, RECT iRect, LPGETPIXELPROC GetPixelProc, LPPUTPIXELPROC PutPixelProc, LPPOINT lpFirstPoint, int max_points, BOOL DoDisplay, LPINT npoints, LPADDPROC lpAddProc);

// Source file: vignette.c
BOOL GradientImage( LPIMAGE lpImage, LPGRADIENT_PARMS lpParms );
BOOL TintFillImage( LPIMAGE lpImage, LPTINTFILL_PARMS lpParms );
BOOL TextureFillImage( LPIMAGE lpImage, LPTEXTUREFILL_PARMS lpParms );
BOOL InvertImage( LPIMAGE lpImage, LPCMD_PARMS lpParms );

// Source file: winpal.c
void SaveSystemPalette( HWND hWnd );
void RestoreSystemPalette( HWND hWnd );
HPALETTE CopySystemPalette( void );
HPALETTE CopyPalette(HPALETTE hPal);
HPALETTE CreateCustomPalette(LPRGB lpRGBmap, int iEntries);
void ApplyMap(int dirty);
void ApplyMapToFrame(LPFRAME lpFrame);
void MapSetPackedLut(LPTR lpLuts);
LPTR MapGetPackedLut(LPTR lpDst,LPTR HLut,LPTR SLut,LPTR MLut,LPTR RLut,LPTR GLut,LPTR BLut, LPTR KLut);
BOOL MapSetupLuts(LPTR HLut, LPTR SLut, LPTR MLut, LPTR RLut, LPTR GLut, LPTR BLut, LPTR KLut);
void ResetAllMaps( BOOL bapplyMToK = NO );
void ResetAllMapsEx( int type, BOOL bapplyMToK = NO );
void ResetBandAllMaps(WORD wBand);
BOOL AnimateMap( HWND hWnd, LPMAP lpMap, BOOL fDoGamma);
void MapPreview(HWND hWnd);
void UndoMapPreview( HWND hWnd );
BOOL Previewing( HWND hWnd );
int DisplayMappedImage(HDC hDC,LPFRAME lpFrame,LPRECT lpSourceRect, POINT destPoint, LPTR lpOutBuffer, int xDiva, int yDiva, LPVOID lpCmsXform);
void MapLine24HSL(LPTR lpHLut,LPTR lpSLut,LPTR lpLLut, LPTR lpData, int count);
void MapLine32HSL(LPTR lpHLut,LPTR lpSLut,LPTR lpLLut, LPTR lpData, int count);

// mapping.asm
#ifndef C_CODE
extern "C" {
#endif
void MapLine8(LPTR lpMLut, LPTR lpData, int count);
void MapLine8by2(LPTR lpMLut, LPTR lpLut, LPTR lpData, int count);
void MapLine24(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpData, int count);
void MapLine32(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpKLut, LPTR lpData, int count);
void MapLine32MToK(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpKLut, LPTR lpData, int count);
#ifndef C_CODE
}
#endif

// Source file: zoom.c
long WINPROC WndZoomBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL NewZoomWindow( HWND hParentWnd );
void StuffZoomDisplay(HWND hWnd, LPDISPLAY lpDisp);
BOOL HookZoom( void );
void UnhookZoom(HWND hWnd);
void ZoomSizeToDef( void );
void ComputeZoomDisp( void );
void SetZoomDefPos(int xPos, int yPos);
void SetZoomDefSize(int width, int height);
void ZoomMainViewChanged(HWND hWnd);
BOOL ResizeMoveZoom( void );
void ZoomBoundRect(int *x, int *y, int *width, int *height);
void ZoomBoundSize(MINMAXINFO FAR* lpInfo);
BOOL ZoomPaint(HWND hWnd);

#endif

