//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _BRUSH_H_
#define _BRUSH_H_

/********************************************************************* 
 **** Brush contants
 *********************************************************************/
#define MAX_BRUSH_SIZE 999

// DestData
#define DD_CURRENT_IMAGE	0
#define DD_CURRENT_UNDO		1

// SourceColor
#define SC_ACTIVE_COLOR		0
#define SC_ALTERNATE_COLOR	1
#define SC_RGB_COLOR		2

// SourceData
#define SD_CURRENT_DEST		0
#define SD_CURRENT_UNDO		1
#define SD_FILE				2
#define SD_NONE				3
#define SD_LAST_DEST 	    4
#define SD_MODIFIED_DEST 	5

// SourceFunction
#define SF_SOLID_COLOR		0
#define SF_COPY_IMAGE		1
#define SF_SHARPEN_IMAGE	2
#define SF_SMOOTH_IMAGE		3
#define SF_LIGHTEN_IMAGE	4
#define SF_DARKEN_IMAGE		5
#define SF_SMEAR_IMAGE		6

// SourceFlipMode
#define SFM_NO_FLIP		0
#define SFM_MIRROR		1
#define SFM_TILE		2

// StrokeMethod
#define SM_CONNECTED	0
#define SM_UNCONNECTED	1

// EdgeType
#define ET_PAINT	0
#define ET_AIRBRUSH	1

// State
#define STATE_BEGIN		    1
#define STATE_FIRSTSTROKE 	2
#define STATE_NEWSTROKE		4
#define STATE_END		    8

// ReadOnlyMask bits
#define RO_SHAPE		1
#define RO_SIZE			2
#define RO_OPACITY		4
#define RO_SOURCE_DATA	8
#define RO_FLIPMODE		16
#define RO_PRESSURE		32
#define RO_MERGEMODE	64
#define RO_FEATHER		128

// Protection 
#define PS_NORMAL		0
#define PS_READONLY		1
#define PS_NODELETE		2

#define SSI_NONE    0
#define SSI_DST	    1
#define SSI_SRC	    2

/********************************************************************* 
 **** MGXBRUSH: The brush structure
 *********************************************************************/
typedef struct _mgxbrush
	{
	BRUSH_TYPE	Type;
	EXTNAME		Name;
	int			DestData;
	int			SourceColor;
	COLORINFO	ColorStruct;
	int			SourceData;
	LPFRAME		SourceFrame;
	int			SourceFlipMode;
	BOOL		SourceFlipHorz;
	BOOL		SourceFlipVert;
	int			SourceFunction;
	int			StrokeMethod;
	int			Shape;
	EXTNAME		ShapeName;
	int			Style;
	int			Size;
	HPTR		BrushMask;
	int			Pressure;
	int			EdgeType;
	int			Feather;
	int			FadeSize;
	int			FadeOpacity;
	int			Opacity;
	int			OpacityScale;
	int			Tilt;
	MERGE_MODE 	MergeMode;
	STRING		SurfaceName;
	LPFRAME		SurfaceFrame;
	BOOL		SurfaceFlipHorz;
	BOOL		SurfaceFlipVert;
	WORD		Timer;
	LFIXED		MaxFlowRate;
	int			Sensitivity;
	int			MaxFlowChange;
	int			StylePressure;
	ULONG		ReadOnlyMask;
	int			Weight;
	STRING		szSourceExtName;
	FNAME		szSource;
	int			xSrcOffset;
	int			ySrcOffset;
	int			Spacing;
	int			Smoothness;
	BOOL		NoOverlap;
	WORD		Protection;
	} MGXBRUSH;
typedef MGXBRUSH FAR *LPMGXBRUSH;
typedef LPTR HMGXBRUSH;

typedef void (FAR *LPBRUSHUPDATEPROC)(HWND hWnd, LPRECT lpUpdateRect, BOOL fUpdateCursor);
typedef void (FAR *LPSRCPOSPROC)(HWND hWnd, int x, int y, int BrushSize, LPINT lpX, LPINT lpY);

/********************************************************************* 
 **** MGXBRUSHPARMS: The brush engine parameters structure
 *********************************************************************/
typedef struct _mgxbrushparms
	{
	HMGXBRUSH			hBrush;			// the brush to draw with
	LPPIXMAP			lpPixmap;		// the pixmap to draw into
	int					xOffset;		// offset of pixmap on image
	int					yOffset;		// area for doing masking
	HWND				hWnd;			// window to display brush into
	BOOL				fUsePolling;	// is engine being controlled by the
										// the user moving the mouse?
	BOOL				fNotAnImage;	// is the destination an image?
	LPBRUSHUPDATEPROC	lpUpdateProc;	// proc to update brush display
										// as well as image data
	LPSRCPOSPROC		lpSrcPosProc;	// get source mouse position
	DWORD				dwSaveStrokeInfo;	// save stroke info type
	LPIMAGE				lpImage;		// image to work on
	} MGXBRUSHPARMS, FAR *LPMGXBRUSHPARMS;

HPSTROKEINFO MgxBrushGetStrokeInfo(LPINT nPoints, LPBOOL fSavePressure, LPINT lpDstRes, LPINT lpSrcRes);
void MgxBrushSetDefaults(LPMGXBRUSHPARMS lpParms,
					HWND hWndIn, LPPIXMAP lpPixmapIn, int xOffset, int yOffset,
					HMGXBRUSH hBrush);
void MgxBrushSetAdvanced(LPMGXBRUSHPARMS lpParms,
					HWND hWndIn, LPPIXMAP lpPixmapIn, int xOffset, int yOffset,
					HMGXBRUSH hBrush,
					BOOL fUsePolling,
					LPIMAGE lpImage,
					LPBRUSHUPDATEPROC lpUpdateProcIn,
					LPSRCPOSPROC lpSrcPosProc);
BOOL SetupBrushBuffers(BOOL fInit);
BOOL MgxBrushBeginParms(LPMGXBRUSHPARMS lpParms);
BOOL MgxBrushBegin(HWND hWndIn, LPPIXMAP lpPixmapIn, int xOffset, int yOffset,
					HMGXBRUSH hBrush);
BOOL MgxBrushBeginEx(HWND hWndIn, LPPIXMAP lpPixmapIn, int xOffset, int yOffset,
					HMGXBRUSH hBrush,
					BOOL fUsePolling,
					BOOL fNotAnImageSetting,
					LPBRUSHUPDATEPROC lpUpdateProcIn,
					LPSRCPOSPROC lpSrcPosProc);
void MgxBrushEnd(BOOL fNoBrushDisplay);
BOOL MgxBrushActive();
void MgxBrushStroke(int sx, int sy, int dx, int dy, int curs_x, int curs_y);
void MgxGetEditRect(LPRECT lpEditRect);
HMGXBRUSH CreateMgxBrush(LPTSTR lpExtName, ITEMID wType, FRMTYPEINFO TypeInfo);
HMGXBRUSH CreateMgxEraser(ITEMID wEraserType);
HMGXBRUSH CreateMgxColorBrush(LPCOLORINFO lpColorStruct, int Feather);
HMGXBRUSH CreateFeatherMaskBrush(int Size, BYTE gray);
LPMGXBRUSH CopyMgxBrush(HMGXBRUSH hBrush);
BOOL WriteMgxBrush(HMGXBRUSH hBrush, LPTSTR lpFileName);
void DestroyMgxBrush(HMGXBRUSH hBrush);
void FreeUpMgxBrushMask(HMGXBRUSH hBrush);
void SetMgxBrushSource(HMGXBRUSH hBrush, LPFRAME lpFrame, int xSrcOffset, int ySrcOffset);
int GetMgxBrushSource(HMGXBRUSH hBrush);
LPFRAME GetMgxBrushSourceFrame(HMGXBRUSH hBrush);
int SetMgxBrushStyle(HMGXBRUSH hBrush, int BrushStyle);
int SetMgxStylePressure(HMGXBRUSH hBrush, int StylePressure);
void SetMgxBrushSize(HMGXBRUSH hBrush, int BrushSize);
void SetMgxBrushFeather(HMGXBRUSH hBrush, int Feather);
BOOL SetMgxBrushShape(HMGXBRUSH hBrush, int BrushShape, LPTSTR lpShapeName);
int SetMgxBrushOpacity(HMGXBRUSH hBrush, int Opacity);
int SetMgxBrushOpacityScale(HMGXBRUSH hBrush, int OpacityScale);
void SetMgxBrushFlipMode(HMGXBRUSH hBrush, int FlipMode, BOOL FlipHorz, BOOL FlipVert);
void SetMgxBrushPressure(HMGXBRUSH hBrush, int Pressure);
MERGE_MODE SetMgxBrushMergeMode(HMGXBRUSH hBrush, MERGE_MODE MergeMode);
void SetMgxBrushColor(HMGXBRUSH hBrush, LPCOLORINFO lpColorStruct);
int SetMgxBrushStrokeMethod(HMGXBRUSH hBrush, int idStrokeMethod);
int GetMgxBrushSize(HMGXBRUSH hBrush);
int GetMgxBrushFeather(HMGXBRUSH hBrush);
int GetMgxBrushShape(HMGXBRUSH hBrush);
int GetMgxBrushOpacity(HMGXBRUSH hBrush);
int GetMgxBrushOpacityScale(HMGXBRUSH hBrush);
int GetMgxBrushFlipMode(HMGXBRUSH hBrush, LPINT FlipHorz, LPINT FlipVert);
int GetMgxBrushPressure(HMGXBRUSH hBrush);
int GetMgxBrushStyle(HMGXBRUSH hBrush);
int GetMgxStylePressure(HMGXBRUSH hBrush);
void GetMgxBrushSourceColor(HMGXBRUSH hBrush, LPCOLORINFO lpColor);
MERGE_MODE GetMgxBrushMergeMode(HMGXBRUSH hBrush, LPINT lpReadOnly);
void MgxBrushMirror(HMGXBRUSH hBrush, LPRECT lpSrcRect, LPRECT lpDstRect, int npix, int nlin);
void MgxMirrorRect(LPRECT lpSrcRect, LPRECT lpDstRect, int npix, int nlin, BOOL FlipHorz, BOOL FlipVert);
void CALLBACK BrushTimerProc( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
void FadeBrush();
void FadeOutBrush();
void ModifyFlowRate(int dx, int dy, BOOL fStrokeProcessed);
void PressureInit( void );
void PressureSetup(int delta);
void MergeData(LPTR lpDst, LPRECT lpDstRect, int DstWidth, LPTR lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);
BOOL MgxOSBrushBegin(HMGXBRUSH hBrush, LPRECT lpStrokeRect, LPRECT lpMaxRect);
void MgxOSPickupBrush(void);
LPFRAME MgxOSBrushEnd(LPRECT lpOSRect);
void MgxOSBrushStroke(int dx, int dy);
LPMGXBRUSH CreateDefaultBrush();

#endif // _BRUSH_H_



