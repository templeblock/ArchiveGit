//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
#ifndef __routines
#define __routines

// This has to stay here because MGXPS.DRV is not yet fully WIN3.1!
#define NOTWIN31

/***********************************************************************
 * Routines.h	declarations of all procedures in picture publisher
***********************************************************************/

#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "types.h"
#include "pixutil.h"
#include "maskutil.h"
#include "list.h"
#include "framelib.h"
#include "imgutil.h"
#include "superblt.h"
#include "root.h"
#include "pplist.h"
#include "poly.h"
#include "editpoly.h"
#include "image.h"
#include "app.h"
#include "appstyle.h"
#include "avi.h"
#include "shapedit.h"
#include "fileopt.h"
#include "engine.h"
#include "clrmng.h"
#include "cmdparms.h"
#include "objedit.h"
#include "gauss.h"
#include "tselect.h"
#include "tcontrol.h"
#include "brush.h"
#include "tdraw.h"
#include "imgio.h"
#include "dialog.h"
#include "controls.h"
#include "taskman.h"
#include "map.h"
#include "data.h"
#include "share.h"
#include "imgcmd.h"
#include "thread.h"
#include "ctllib.h"
#include "cmdmacro.h"
//#include "mapprocs.h"
#include "fitpoly.h"
#include "dlgmgr.h"
#include "winimage.h"
#include "dlgcms.h"
//UN-COMMENT THIS SECTION WHEN MAPPROCS IS REMOVED!!!
extern "C"
{
#include "dyndlg.h"
}

/************************************************************************/
/* Macro Subroutines							*/
/************************************************************************/

#define CreateProc( hWindow, x, y, keyFlags ) \
	(*Tool.lpToolProc)( hWindow, WM_CREATE, x, y, (UINT32)keyFlags )
#define ButtonDownProc( hWindow, x, y, keyFlags ) \
	(*Tool.lpToolProc)( hWindow, WM_LBUTTONDOWN, x, y, (UINT32)keyFlags )
#define ButtonUpProc( hWindow, x, y, keyFlags ) \
	(*Tool.lpToolProc)( hWindow, WM_LBUTTONUP, x, y, (UINT32)keyFlags )
#define MoveProc( hWindow, x, y, keyFlags ) \
	(*Tool.lpToolProc)( hWindow, WM_MOUSEMOVE, x, y, (UINT32)keyFlags )
#define DoubleClickProc( hWindow, x, y, keyFlags ) \
	(*Tool.lpToolProc)( hWindow, WM_LBUTTONDBLCLK, x, y, (UINT32)keyFlags )
#define TimerProc( hWindow, id ) \
	(*Tool.lpToolProc)( hWindow, WM_TIMER, 0, 0, (UINT32)id )
#define DestroyProc( hWindow, fCancel ) \
	(*Tool.lpToolProc)( hWindow, WM_DESTROY, 0, 0, (UINT32)fCancel )
#define CommandProc( hWindow, cmd ) \
	(*Tool.lpToolProc)( hWindow, WM_COMMAND, 0, 0, (UINT32)cmd )
#define ActivateProc( hWindow, fActivate ) \
	(*Tool.lpToolProc)( hWindow, WM_ACTIVATE, 0, 0, (UINT32)fActivate )
#define MsgProc( hWindow, msg, x, y, option ) \
	(*Tool.lpToolProc)( hWindow, msg, x, y, option )
#define CursorProc(hWindow, hWndCursor) \
	(*Tool.lpToolProc)( hWindow, WM_SETCURSOR, 0, 0, (UINT32)hWndCursor)
#define SendRibbonEnable(code) \
	{if(Tool.hRibbon) SEND_WM_CONTROLENABLE( Tool.hRibbon, code);}

// Because TRY CATCH blocks are UGLY!!!
#define TRY_BLOCK 	TRY{
#define	CATCH_BLOCK	}CATCH_ALL(e){
#define CATCH_END 	}END_CATCH_ALL


/* enginea.asm */
void MergeData8(LPTR lpDst, LPRECT lpDstRect, int DstWidth, LPTR lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);
void MergeData24(LPRGB lpDst, LPRECT lpDstRect, int DstWidth, LPRGB lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);

/* */
void pair2byte( LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount );

#ifdef UNUSED
/* vignetta.asm */
void ProcessVignette8( LPTR, int, LPLONG, LPLONG, int, BOOL, int, int, int, long, long, long );
void ProcessVignette24( LPRGB, int, LPLONG, LPLONG, int, BOOL, int, int, int, long, long, long );
#endif // UNUSED

/* Everything below was gotten from compiling all sources */
/* with the "calls" program, and can be regenerated */

// Source file: astral.c
#include "astral.h"

// Source file: automask.c
BOOL automask(HWND hWnd, int x, int y, LPCPoly lpPoly, int idColorModel, BOOL bUpdateVisual);
long Distance24(LPTR lpV1, LPTR lpV2);
long Distance24HSL(LPTR lpV1, LPTR lpV2);
long Distance24H(LPTR lpV1, LPTR lpV2);
long Distance24L(LPTR lpV1, LPTR lpV2);
long Distance24S(LPTR lpV1, LPTR lpV2);
long Distance24SL(LPTR lpV1, LPTR lpV2);
long Distance24HL(LPTR lpV1, LPTR lpV2);
long Distance24HS(LPTR lpV1, LPTR lpV2);

long Distance32(LPTR lpV1, LPTR lpV2);
long Distance32HSL(LPTR lpV1, LPTR lpV2);
long Distance32H(LPTR lpV1, LPTR lpV2);
long Distance32L(LPTR lpV1, LPTR lpV2);
long Distance32S(LPTR lpV1, LPTR lpV2);
long Distance32SL(LPTR lpV1, LPTR lpV2);
long Distance32HL(LPTR lpV1, LPTR lpV2);
long Distance32HS(LPTR lpV1, LPTR lpV2);

int GoingAway( int dx, int dy );
int direction( int dx, int dy );

// Source file: class.c
BOOL RegisterAllClasses( HINSTANCE hInstance );

// Source file: cmask.c
BOOL InColorMask(LPRGB lpRGB, LPTR lpPixel);
void SetupShield( void );
void UpdateColorMask( LPCOLORINFO lpColor );
void ShowShieldProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );

// Source file: commands.c
long DoCommand( HWND hWindow, UINT id, HWND hControl, UINT codeNotify );
BOOL HandleOpenCommand(HWND hWindow);
BOOL HandleImportCommand(HWND hWindow);
BOOL HandleCommandLine( HWND hWindow, LPTSTR lpszCmdLine, LPINT lpPrint );

// Source file: convto.c
BOOL ConvertTo( LPIMAGE lpImage, int iFunction );
BOOL ConvertImage( LPIMAGE lpImage, LPCONVERT_PARMS lpParms );
BOOL WINPROC DlgConvertProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowConvertOptions(HWND hDlg, int idDataType, int idOrigDataType);

// Source file: defaults.c
#ifndef _DESIGNERFIX
BOOL DesignerEditorInProgress ();
#endif //_DESIGNERFIX
BOOL NeedSPAWarning();
BOOL GetIniPath( LPTSTR lpIniFile );
int GetDefaultInt( LPTSTR lpKeyName, int nDefault );
WORD GetDefaultString( LPTSTR lpKeyName, LPTSTR lpDefault, LPTSTR lpReturnedString, int nSize);
BOOL PutDefaultInt( LPTSTR lpKeyName, int nDefault );
BOOL PutDefaultString( LPTSTR lpKeyName, LPTSTR lpString);
BOOL PutPrivateInt( LPTSTR lpszProfile, LPTSTR lpszSection, LPTSTR lpszKey, int nDefault );
BOOL PutPrivateString( LPTSTR lpszProfile, LPTSTR lpszSection, LPTSTR lpszKey, LPTSTR lpszString );
void LoadAllPreferences(BOOL fMemOnly);
void LoadPrintStyleDefaults( void );
void SavePrintStyleDefaults( void );
void SavePreferences( void );
void SaveMiscPreferences( void );
void SaveRecallList( void );
void PutRetouchBrush( int idTool );
LPTSTR stripstruct( LPTSTR lpString );
LPTSTR LoadPreferences( LPTSTR lpFileName );
LPTSTR FindPreference( LPTSTR lpKeyName );
BOOL SaveToolPreferences( int idTool );
void SaveBrowserPrefs(void);
void LoadBrowserPrefs(void);
void SaveBrowserState(void);
void LoadBrowserState(void);
void SaveSaveOptions(void);
void LoadSaveOptions(void);
void SetCommonSettings(void);
void SetCommonMode(int iModes);
void SetCommonOpacity(int Opacity);
void SetCommonMergeMode(MERGE_MODE MergeMode);
void SetCommonColorModel(int idColorModel);
void SetCommonRange(int Range);
void SetCommonWidth(LFIXED Width);
void SetCommonHeight(LFIXED Height);
void SetCommonConstrain(int Constrain);
void SetCommonBrushShape(int BrushShape);
void SetCommonBrushSize(int BrushSize);
void SetCommonPressure(int Pressure);
void SetCommonFeather(int Feather);
void SetCommonDoCircle(BOOL DoCircle);
void SetCommonUnits(int Units);

// Source file: dlgtools.c
BOOL WINPROC DlgPointerProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgViewProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgCopyToFileProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMaskLoadProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMaskSaveProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgApplyUndoProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgCustomBrushProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgGlobalFloatProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CopyToNewImage( LPIMAGE lpImage, LPCMD_PARMS lpParms );

// Source file: fastbits.c
BOOL DoLowResLoad( ITEMID idFileType, LPTSTR lpFileName, LPCMSINFO pCmsInfo = NULL );
BOOL WINPROC DlgLowResProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL DoFastbitsSave( ITEMID idFileType, ITEMID idDataType, LPTSTR lpFileName);
//OLE2
BOOL DoFastbitsLoad(ITEMID idFileType, LPTSTR lpFileName, LPCMSINFO pCmsInfo = NULL, 
		LPFRAME lpPreview = NULL, LPRECT lpGridRect = NULL, BOOL bOpenNow = TRUE);
BOOL WINPROC DlgFastbitsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL PreviewBeingUsed(LPFRAME lpPreviewFrame);
BOOL LowResLoad( LPIMAGE lpImage, LPLOWRESLOAD_PARMS lpParms );

// Source file: filt.c
BOOL ConvolveData( LPFRAME lpFrame, LPINT lpValues );
BOOL FindEdges( LPFRAME lpFrame, LPRECT lpRect, LPTR lpOutput);

// Source file: grab.c
BOOL IsGrabberOK( LPTSTR lpExtName, LPTSTR lpString );
HANDLE LoadGrabberLibrary( LPTSTR lpExtName );
void back_to_windows(int flag);
void grabber(LPTSTR lpExtName, int port, HINSTANCE hInstance, HWND hWindow);
WORD GetA000H( void );

// Source file: unknown
BOOL WINPROC DlgCalibrateScanProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgScanVisualProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgCalibratePrintProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPrintVisualProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgCalibrateApplyProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMapEditProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgQuarterProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgHistogramProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgScanLoadProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPrintLoadProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void LoadMeasurementControls( HWND hDlg, LPTR lpStep1, LPTR lpStep2, LPTR lpStep3, LPTR lpStep4 );
void ActivateMapControls( HWND hDlg, WORD wTool, LPWORD lpOldTool, BOOL bDelay );
void SetHistoValues( HWND hDlg, MAP * lpMap, LPWORD lpRHisto, LPWORD lpGHisto, LPWORD lpBHisto, BOOL fNotify );

// Source file: mgxbtn.c
BOOL WINPROC register_mgxbtn_class (HINSTANCE hInst,BOOL bGlobal);
LONG WINPROC MgxBtnWndFn (HWND hControl,WORD wMsg,WPARAM wParam,LPARAM lParam);

// Source file: objalgn.c
BOOL WINPROC DlgAlignProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL AlignObjects( LPIMAGE lpImage, LPALIGNOBJECTS_PARMS lpAlignParms );
BOOL WINPROC DlgPosProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL PosObjects( LPIMAGE lpImage, LPPOSOBJECTS_PARMS lpPosParms );
BOOL LockObjects( LPIMAGE lpImage, LPCMD_PARMS lpCmdParms );
BOOL CombineObjObjects( LPIMAGE lpImage, LPCMD_PARMS lpCmdParms );

// Source file: oldframe.c
LPFRAME frame_set(LPFRAME lpFrame);

// Source file: palette.c
long WINPROC Palette_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
void GetChipLayout( HWND hWnd, LPRECT lpRect, LPINT lpRows, LPINT lpCols, LPINT lpRowIncr, LPINT lpColIncr, LPINT lpStart );
void GetChipSize(LPINT lpDx, LPINT lpDy);

// Source file: palprocs.c
BOOL PaletteDlgClose(BOOL bHaveToClose,HWND hThisDlg = NULL);
void MovePaletteColor(HWND hDlg, int iOldEntry, int iNewEntry);
void InsertPaletteColor(HWND hDlg, int iEntry, LPCOLORINFO lpColorInfo, BOOL fInsert);
BOOL WINPROC DlgPaletteProc(HWND hDlg, unsigned msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPaletteNewProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPaletteLoadProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPaletteMergeProc(HWND hDlg, unsigned msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPaletteSaveProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgLabelProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgFindProc(HWND hDlg, unsigned msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgSizePaletteProc(HWND hDlg, unsigned msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgFillPaletteProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL PaletteVertical( void );

// Source file: print.c
int my_sqrt(int value);
int PrintLabelCentered( HDC hDC, int x, int y, COLOR Color, int idStr, ... );
int PrintTest( HWND hWnd, LPTSTR szFile, LPIMAGE lpImage );
int PrintFile (HWND hWnd, LPTSTR szFile, BOOL EjectPage, LPIMAGE lpImage, LPFRAME lpFrame);
int PrintImage( HDC hDC, BOOL bToScreen, LPRECT lpSourceRect, LPRECT lpDestRect, int xDiva, int yDiva, int yTop, int yTotal, int nTotalBits, LPIMAGE lpImage, LPFRAME lpFrame, BOOL bUsePrinterScreening);
void OutputFurniture( LPIMAGE lpImage, HDC hDC, LPRECT lpBandRect, LPRECT lpRect, int xRes, int yRes, BYTE cSep );
int PrintLabel( HDC hDC, int x, int y, COLOR Color, int idStr, ... );
void OutputRegMark( HDC hDC, LPRECT lpRect );
BOOL CALLBACK AbortTest( HDC hDC, int iCode );
BOOL WINPROC DlgPrintAbortProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
HDC GetPrinterDC( void );
void PrintErr(HWND hWnd, int ierr);
BOOL AstralLaserPrint( LPIMAGE lpImage, LPFRAME lpFrame, int xSrc, int ySrc, int dxSrc, int dySrc, int xDest, int yDest, int dxDest, int dyDest, int iPrResX, int iPrResY );
BOOL LaserPrint( LPTSTR lpFormat, ... );
BOOL IsPostScript( void );
void SuperLock( BOOL Bool );
BOOL SuperPaint( HWND hWnd );

// Source file: prnprocs.c
BOOL WINPROC DlgPrintProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPrintSetupProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPrintStyleProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgSeparateProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgHalftoneProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgBlackGenerateProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgCalPrintEditProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgCalScanEditProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

BOOL LoadPrintStyle( LPTSTR lpExtName );
BOOL SavePrintStyle( HWND hDlg, LPTSTR lpExtName );

BOOL LoadHalftoneStyle( LPTSTR lpExtName );
BOOL SaveHalftoneStyle( HWND hDlg, LPTSTR lpExtName );

BOOL LoadSeparateStyle( LPTSTR lpExtName );
BOOL SaveSeparateStyle( HWND hDlg, LPTSTR lpExtName );

BOOL LoadCalibrateStyle( LPTSTR lpExtName );
BOOL SaveCalibrateStyle( HWND hDlg, LPTSTR lpExtName );

BOOL LoadScanCalibrate( LPTSTR lpExtName );
BOOL SaveScanCalibrate( HWND hDlg, LPTSTR lpExtName );

BOOL LoadBlackGeneration( LPSEPARATE lpSeparate );
 
// Source file: retouch.c
void SetupBrushMask(LPMGXBRUSH lpBrush, FRMDATATYPE Type);
BOOL SetupCustomBrushMask( LPMGXBRUSH lpBrush, LPTSTR lpExtName );
void CreateFuzzTable(LPTR fuzz, int table_size, int FuzzyPixels, int Opacity, int EdgeType);
void SetupFuzzyBrush(LPMGXBRUSH lpBrush, FRMDATATYPE Type);
BOOL SetupBrushMaskBuffers(BOOL fInit);

// Source file: dca_acq.cpp
BOOL CheckTwainOK( void );

// Source file: scan.c
HANDLE LoadScannerLibrary( LPTSTR lpExtName );

// Source file: split.c
void SplitRGB( LPIMAGE lpImage );
void CombineRGB( LPIMAGE lpImage );
void SplitHSL( LPIMAGE lpImage );
void CombineHSL( LPIMAGE lpImage );
void SplitCMYK( LPIMAGE lpImage );
void CombineCMYK( LPIMAGE lpImage );

// Source file: tcrop.c
BOOL WINPROC DlgCropProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void CropProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );

// Source file: text.c
BOOL TextEdit(LPIMAGE lpImage, LPTEXT_PARMS lpParms);
BOOL TextApply(LPIMAGE lpImage, LPTEXT_PARMS lpParms);
BOOL TextBegin(HWND hWnd, int x, int y);
void TextEnd(HWND hWnd, BOOL fCancel);
void TextMove(HWND hWnd, int x, int y);
void TextRMove(HWND hWnd, int dx, int dy);
void TextKeystroke(HWND hWnd, WORD nVKey);
void TextFontChanged(HWND hWnd);
void TextDisplayCaret(HWND hWnd, HDC hInDC);
BOOL TextCanRotate(HDC hDC, LPFONTDESC lpFontDesc);
int TextGetFontDesc(LPFONTLIST lpFontList, LPTSTR lpFaceName, LPFONTDESC lpFontDesc);

// Source file: toolbar.c
BOOL WINPROC DlgToolsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
ITEMID ActivateTool( ITEMID idTool );
void DeactivateTool( void );
void InstallTool(BOOL fAlternate);

// Source file: toolproc.c
void CustomProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void PointerProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void GrabberProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );

// Source file: track.
void TrackInit(HWND hWnd, int iMode, void (FAR *tlpDrawRoutine)(HWND hWnd, HDC hDC, BOOL on), LPRECT lpRect, LFIXED fScale);
void TrackMapOriginalRect( LPDISTORT lpDistort, LPRECT lpSrcRect, LPRECT lpDstRect );
void TrackGrab(HWND hWnd, int x, int y);
BOOL TrackMove(HWND hWnd, int x, int y);
void TrackMode(HWND hWnd, int iMode, BOOL fDisplay);
void TrackRotateConstrain( BOOL fConstrainX, BOOL fConstrainY );
void TrackFlip(HWND hWnd, BOOL fFlipX, BOOL fFlipY, BOOL fDisplay);
void TrackUp(HWND hWnd, int x, int y);
BOOL TrackDone(HWND hWnd, BOOL undraw);
void TrackSetCursor(HWND hWnd, int x, int y);
BOOL TrackMoved( void );
void TrackGetRect(LPRECT lpRect);
void TrackGetForwardDistort( LPDISTORT lpDistort );
void TrackDelta(HWND hWnd, int dx, int dy);
void TrackExtents(LPRECT lpRect);
LFIXED TrackGetAngle(POINT center, int x, int y);
void TrackDraw( HWND hWnd, HDC hDC, BOOL on );

// Source file: tfill.c
BOOL WINPROC DlgFloodProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgTintFillProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgVignetteProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgTextureFillProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void FloodProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void TintFillProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void VignetteProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void TextureFillProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );

// Source file: tobjsel.c
BOOL SelectObjectsImage(LPIMAGE lpImage, LPSELECTOBJECTS_PARMS lpParms);
BOOL MoveObjectsImage(LPIMAGE lpImage, LPMOVEOBJECTS_PARMS lpParms);
BOOL SetAttrObjectsImage(LPIMAGE lpImage, LPSETATTROBJECTS_PARMS lpParms);
BOOL LayerObjectsImage(LPIMAGE lpImage, LPLAYEROBJECTS_PARMS lpParms);
BOOL GroupObjectsImage(LPIMAGE lpImage, CMD_PARMS lpParms);
BOOL DuplicateObjectsImage(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL DeleteObjectsImage(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL WINPROC DlgSelectorProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void SelectorProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
MERGE_MODE HandleMergeCombo(HWND hDlg, ITEMID idCombo, BOOL fInit, MERGE_MODE ModeSelected, UINT codeNotify);
MERGE_MODE HandleMergeComboEx(HWND hDlg, ITEMID idCombo, BOOL fInit, MERGE_MODE ModeSelected, UINT codeNotify, BOOL fAllModes);
LPOBJECT ImgGetObject(LPIMAGE lpImage, int nObject);
int ImgCountFloaters(LPIMAGE lpImage);
void SetupMiniViews(HWND hDlg, BOOL fSelectChange);
BOOL WINPROC DlgObjFloatProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

// Source file: tpaint.c
void RetouchInit(HWND hDlg, HWND hWndFocus, LPARAM lParam, LPRETOUCHDLG lpRD);
void RetouchEnable(HWND hDlg, int Flags, LPRETOUCHDLG lpRD);
BOOL RetouchCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify, LPRETOUCHDLG lpRD);
BOOL WINPROC DlgPaintProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPaintOnProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgTextureProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgFilterProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void PaintProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void TextureProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void EraserProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void AltEraserProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void AltPaintOnProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void FilterProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void PaintOnProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void CloneProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void DisplayBrush(HWND hWnd, int x, int y, BOOL on );
BOOL DisplayBrushEx(HWND hWnd, int x, int y, BOOL on, LPRECT lpOffRect );
BOOL WINPROC DlgMaskStrokeProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgCreateBrushProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL RetouchImage(LPIMAGE lpImage, LPRETOUCH_PARMS lpParms);
BOOL AltEraserImage(LPIMAGE lpImage, LPRETOUCH_PARMS lpParms);
BOOL PaintMaskImage(LPIMAGE lpImage, LPRETOUCH_PARMS lpParms);
BOOL StrokeMaskImage(LPIMAGE lpImage, LPSTROKEMASK_PARMS lpParms);

// Source file: tprobe.c
BOOL WINPROC DlgProbeProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void ProbeProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void		SetActiveColorString( LPTSTR lpString, COLORSPACE ColorSpace );
void		SetAlternateColorString( LPTSTR lpString, COLORSPACE ColorSpace );
COLORSPACE	GetActiveColorString( LPTSTR lpString );
COLORSPACE	GetAlternateColorString( LPTSTR lpString );
void        RevertColorSwatch(void);
void		   SwapActiveAlternate( void );
void        SetActiveColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace, BOOL bToolBar );
void        SetActiveRGB( LPRGB lpRGB, BOOL bToolBar );
COLORSPACE  GetActiveColor( LPCOLORINFO lpColor );
COLORSPACE  GetActiveColorFromType( FRMDATATYPE Type, LPCOLORINFO lpColor );
COLORSPACE  GetActiveColorFromTypeInfo( FRMTYPEINFO TypeInfo, LPCOLORINFO lpColor );
LPCOLORINFO GetActiveColorInfo( void );
LPRGB       GetActiveRGB( LPRGB lpDst );
void        SetAlternateColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace, BOOL bToolBar );
void        SetAlternateRGB( LPRGB lpRGB, BOOL bToolBar );
COLORSPACE  GetAlternateColor( LPCOLORINFO lpColor );
COLORSPACE  GetAlternateColorFromType( FRMDATATYPE Type, LPCOLORINFO lpColor );
COLORSPACE  GetAlternateColorFromTypeInfo( FRMTYPEINFO TypeInfo, LPCOLORINFO lpColor );
LPCOLORINFO GetAlternateColorInfo( void );
LPRGB       GetAlternateRGB( LPRGB lpDst );
BOOL QuickProbeStart( HWND hWnd );
BOOL QuickProbeEnd( HWND hWnd, int x, int y );
BOOL QuickProbeGet( HWND hWnd, int x, int y, LPCOLORINFO lpProbeColor );

// Source file: tshield.c
BOOL WINPROC DlgShieldFloatProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

// Source file: tstitch.c
BOOL WINPROC DlgStitchProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgDoStitchProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void StitchProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void StitchControlEnable(HWND hDlg);
void StitchInit(HWND hWindow, int iSlot);
void StitchDone(HWND hWindow);
void StitchValidate( void );
BOOL StitchReady( void );
void StitchDisplaySetup(BOOL fEnable);
void StitchMouseDown(HWND hWindow, int x, int y, BOOL bFirst);
void StitchMouseUp(HWND hWindow, int x, int y);
void StitchMouseMove(HWND hWindow, int x, int y);
void StitchDrawPoints(HWND hWnd, HDC hInDC, BOOL on, int index );
BOOL DoStitch( void );
LPFRAME StitchImages(LPFRAME lpFrame1, POINT p1_1, POINT p2_1, LPFRAME lpFrame2, POINT p1_2, POINT p2_2, BOOL iAdjustLum, int BlendPressure, BOOL fSmartStitch);
int DoStitchGetLumDiff( void );
void StitchPointButton(int iSlot, int iPoint);
void DoStitchMovePoint(int iPoint);
void DoStitchAutoAlign( void );
BOOL StitchAutoAlign(LPFRAME lpFrame1, LPPOINT lpP1_1, LPPOINT lpP2_1, LPFRAME lpFrame2, LPPOINT lpP1_2, LPPOINT lpP2_2);
BOOL StitchValidatePoints(LPFRAME lpFrame1, LPPOINT lpP1_1, LPPOINT lpP2_1, LPFRAME lpFrame2, LPPOINT lpP1_2, LPPOINT lpP2_2, LPINT lpWhichEdge, BOOL fNoSwap);
BOOL StitchAdjustPoints(LPFRAME lpFrame1, LPPOINT lpP1_1, LPPOINT lpP2_1, LPFRAME lpFrame2, LPPOINT lpP1_2, LPPOINT lpP2_2, int iEdge);
BOOL StitchAdjustPoint(LPFRAME lpFrame1, LPPOINT lpP_1, LPFRAME lpFrame2, LPPOINT lpP_2, int iEdge);
long StitchAmountSame(LPTR lp1Start, int width1, LPTR lp2Start, int width2, int height);
void StitchConvert(LPTR lpIn, int width, int height, int depth, int threshold);
int StitchGetLumDiff(LPFRAME lpFrame1, POINT p1_1, POINT p2_1, LPFRAME lpFrame2, POINT p1_2, POINT p2_2);
void StitchCreateTransform(LPTFORM lpTForm, POINT p1_1, POINT p2_1, POINT p1_2, POINT p2_2);
int StitchGetPointLumDiff(LPFRAME lpFrame1, POINT p1, LPFRAME lpFrame2, POINT p2);
LPFRAME StitchFrames(LPTFORM lpTForm, LPFRAME lpFrame1, LPFRAME lpFrame2, int iAdjustLum, BOOL fSmartStitch);
BOOL StitchBlend(LPTFORM lpTForm, LPFRAME lpNewFrame, LPFRAME lpFrame, int iWhichEdge, int BlendPressure);
BOOL StitchCreateRects(LPFRAME lpFrame1, LPRECT lpRect1, POINT p1, LPFRAME lpFrame2, LPRECT lpRect2, POINT p2, int offset, int iEdge);

// Source file: ttext.c
BOOL WINPROC DlgTextProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void TextProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );

// Source file: ttform.c
typedef STATUS_CODE (FAR *LPTRANSFORMENDPROC)(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);

BOOL TransformImage(LPIMAGE lpImage, LPTRANSFORMER_PARMS lpParms);
BOOL TransformObjectsImage(LPIMAGE lpImage, LPTRANSFORMOBJECTS_PARMS lpParms);
BOOL PasteFromImage(LPIMAGE lpImage, LPPASTEFROM_PARMS lpParms);
BOOL PasteImage(LPIMAGE lpImage, LPPASTE_PARMS lpParms);
BOOL LoadMaskImage(LPIMAGE lpImage, LPLOADMASK_PARMS lpParms);
BOOL LoadMaskStart(LPIMAGE lpImage, LPALPHA lpAlpha, BOOL fDeleteMask,	BOOL fTransform, LPRECT lpUpdateRect, LPBOOL lpfTransform);
BOOL WINPROC DlgTransformerProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgObjectTransformProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void TransformerProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void TransformStartLoadMaskProc(LPIMAGE lpImage, BOOL fDeleteMask);
void TransformObjectsStart(LPIMAGE lpImage, LPTRANSFORMENDPROC lpProc,
			int idTool, BOOL fNewObject, int idDirty, BOOL fDoUndo,
			LPTSTR lpFileName = NULL, DWORD dwClipOwner = 0, BOOL bExtName = TRUE, LPFRMTYPEINFO lpSrcTypeInfo = NULL);
BOOL TransformLine( LPOBJECT lpObject, int x1, int y, int dx, LPTR lpDataBuf, int iDataDepth, LPTR lpAlphaBuf, int iAlphaDepth, LPDISTORT lpDistort, BOOL fHiQuality );
void TransformObjectsProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
STATUS_CODE TransformObjectsEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);
STATUS_CODE PasteFromEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);
STATUS_CODE PasteEndProc(LPIMAGE lpImage, BOOL fCancel, BOOL fDelete, BOOL fMoveOnly);
void RestoreSelectedObjects(LPIMAGE lpImage);
void SaveSelectedObjects(LPIMAGE lpImage);

// Source file: view.c
LPIMAGE CreateImage( LPTSTR szName, LPFRAME lpFrame, LPOBJECT lpBase, LPCMDLIST lpCmdList, int FileType, int DataType, int DocumentType, LPTSTR ImageName );
void DestroyImage(LPIMAGE lpImage);
void ImgColorMapChanged(LPIMAGE lpImage);
HWND NewImageWindow( LPTSTR szName, LPFRAME lpFrame, LPOBJECT lpBase, LPCMDLIST lpCmdList, int FileType, int DataType, int DocumentType, LPTSTR ImageName, int iDoZoom );
HWND NewImageView( LPIMAGE lpImage, int iDoZoom );
void ImgInitDisplay(LPIMAGE lpImage, BOOL fGeneratePaletteLUT = TRUE);
void Zoom( HWND hWnd, int x, int y, int value, BOOL fIncremental, BOOL fAllowSizeChange );
void ViewXY(HWND hWnd, int x, int y);
void ViewRect(HWND hWnd, LPRECT lpRect, BOOL fResizeWindow);
void ViewAll( HWND hWnd );
void View1TO1();
void NewImageSizeWindow( HWND hWindow );
void FullViewWindow(HWND hWindow, int x, int y, int cx, int cy, BOOL fClipToImage);
void GetDocumentPosition(LPINT lpX, LPINT lpY);
void CalcFullViewWindowSize(LPRECT lpWindowRect, LPRECT lpFileRect, LFIXED FileRate, int npix, int nlin, int x, int y, BOOL fHasRulers, BOOL fClipToImage);
BOOL CanZoom(HWND hWnd, LFIXED FileRate, LPRECT lpFileRect, LPRECT lpDispRect);
BOOL ZoomEnable( HWND hWnd, int value, BOOL fIncremental);
void ComputeDispRect( HWND hWnd );
void ComputeFileRect( HWND hWnd, int x, int y, LFIXED FileRate );
BOOL ScrollImage( HWND hWnd, BOOL fVert, UINT code, int pos );
void SaveLastView( HWND hWnd );
void RevertLastView( HWND hWnd );
BOOL WINPROC DlgFullScreenViewProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

//OLE2
void SetupScrollBars( HWND hWindow );
void SetDocSystemMenu(HWND hWnd, HMENU hMenu);
BOOL SetDisplayScrollRange( HWND hWindow, LPDISPLAY lpDisplay, int fnBar, int iMin, int iMax, BOOL fRedraw);
int SetDisplayScrollPos( HWND hWindow, LPDISPLAY lpDisplay, int fnBar, int iVal, BOOL fRedraw);
BOOL GetDisplayScrollRange(HWND hWindow, LPDISPLAY lpDisplay, int fnBar, LPINT lpMin, LPINT lpMax);
int GetDisplayScrollPos(HWND hWindow, LPDISPLAY lpDisplay, int fnBar);
int GetDisplayScrollDelta(HWND hWindow, LPDISPLAY lpDisplay, int fnBar, int adjust);
BOOL FocusObjectStart( HWND hWnd );
BOOL FocusObjectEnd( HWND hWnd );
BOOL FocusObjectSelect( HWND hWnd );
BOOL FocusObjectDelete( HWND hWnd );

BOOL ConstrainXY(LPINT lpX, LPINT lpY, BOOL fDown, BOOL fButtonDown, BOOL fConstrainMode);
BOOL ConstrainX( void );
BOOL ConstrainY( void );
BOOL DoCPOPreview(HWND hWnd);
BOOL ImageIsDoc( LPIMAGE lpImageDoc );
LPIMAGE ImageSet( LPIMAGE lpImage );
HWND ActivateDocument( HWND hWnd );
void SetupImageDisplayPalette(LPIMAGE lpTheImage);
LPIMAGE GetActiveImage(void);
LPDISPLAY GetActiveDisplay(void);
HWND GetActiveDoc(void);
BOOL ImageIsBusy(LPIMAGE lpImage);
int NumImageWindows(LPIMAGE lpImage);
BOOL IsOnImage(HWND hWnd, int x, int y);
BOOL IsInFile(HWND hWnd, int x, int y);
void EnableImageWindows(LPIMAGE lpImage, BOOL fEnable);
void SetImagePtr(HWND hWnd, LPIMAGE lpImage);
LPIMAGE GetImagePtr(HWND hWnd);
void SetDisplayPtr(HWND hWnd, LPDISPLAY lpDisplay);
LPDISPLAY GetDisplayPtr(HWND hWnd);

// Source file: winprocs.c
BOOL WINPROC DlgInfoFloatProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void DisplayInfo(int x, int y, LPRECT lpRect);
BOOL WINPROC DlgComboProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgHeraldProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgStatusProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateStatusBar( BOOL bMemory, BOOL bCache, BOOL bMask, BOOL bShield );
BOOL WINPROC DlgWarningProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgTrialDaysLeftProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgTrialExpiredProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgReductionProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgSplitProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void DoSizeInits( HWND hDlg );
void ImageMemory( HWND hDlg, ITEMID idControl, LFIXED fWidth, LFIXED fHeight, int iResolution, int iDepth );
void DoSizeControls( HWND hDlg, ITEMID id, UINT codeNotify, BOOL bResAdjust );
BOOL WINPROC DlgProcessProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgExpandProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgRotateProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgNewProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
VOID	SetUnitLabels(HWND hDlg);
void SaveRetouchBrush( int brush );
void SelectRetouchBrush( int brush );
BOOL WINPROC DlgStatusBoxProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
BOOL WINPROC DlgPrefProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgLocationsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgInfoProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgDriversProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LONG WINPROC RibbonDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
