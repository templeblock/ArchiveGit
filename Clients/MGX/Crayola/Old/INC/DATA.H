//®PL1¯®FD1¯®BT0¯®TP0¯®RM250¯
#ifndef __data
#define __data

// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "routines.h"

// This file contains the global data structures.  Any source file that
// does not reference any global data should not need to include this file.
// Therefore, do not define any structures or types that are used for other
// generic purposes - use TYPES.H.

/********************************************************************* 
 **** WINDOW: The image window structure
 *********************************************************************/

typedef struct _window
{
HDC		hDC;			/* The image window device context */
HCURSOR	hCursor;		/* Handle to the image window's current cursor */
BOOL	fLButtonDown;	/* Is the left mouse button down? */
BOOL	fMButtonDown;	/* Is the middle mouse button down? */
BOOL	fRButtonDown;	/* Is the right mouse button down? */
HPEN	hButtonPen;		/* Handle to a button face pen */
HBRUSH	hHilightBrush;	/* Handle to a hilight brush */
HBRUSH	hButtonBrush;	/* Handle to a button face brush */
HBRUSH	hShadowBrush;	/* Handle to a shadow brush */
HPEN	hShadowPen;		/* Handle to a shadow pen */
int		iLabelFont;		/* font for ribbon labels and status bar */
HCURSOR	hNullCursor;	/* The null blank cursor */
BOOL 	fHasZoom; 		// does the user want a zoom window
HBRUSH	hBGBrush;		// The MDI client backbround brush
} WINDOW;

/********************************************************************* 
 **** TOOLDEF: The tool definition structure
 *********************************************************************/
typedef struct _tooldef
{
int		idCursor;		// The tool's cursor id
int		idRibbon;		// The tool's ribbon dialog id
DLGPROC	lpRibbonProc;	// The tool's ribbon dialog proc
LPTOOLPROC	lpToolProc;		// The tool's procedure
BOOL	fDisplaysInfo;	// Does the tool display info?
BOOL	fSetCursor;		// Does the tool proc want to set the cursor?
BOOL	fConstrain;		// Does tool want automatic XY constrain on CONTROL key?
BOOL	fAutoScrollSetPos;	// set the cursor position on autoscroll
} TOOLDEF;

/********************************************************************* 
 **** TOOL: The tool structure
 *********************************************************************/
typedef struct _tool
{
ITEMID 	id;				// The id for the tool
ITEMID	idLast;			// The id for the tool
BOOL	bActive;		// Is the tool active or not
BOOL	bAltTool;		// Using alternate tool?
BOOL	bPassRButton;   // Pass right button up through as left button up
ITEMID	idRibbon;		// The ribbon dialog box id
HWND	hRibbon;		// The ribbon dialog box handle
LPTOOLPROC	lpToolProc;		// The tool procedure
BOOL	fDisplaysInfo;	// Does the tool display info
BOOL	fSetCursor;		// Does the tool proc want to set the cursor?
BOOL	fConstrain;		// Does tool want automatic XY constrain on CONTROL key?
BOOL	fAutoScrollSetPos;	// set the cursor position on autoscroll
TOOLDEF	Prim;			// Stucture containing the primary tool info			// Stucture containing the primary tool info
TOOLDEF	Alt;			// Stucture containing the alternate tool info
} TOOL;

/********************************************************************* 
 **** APP: The main program structure
 *********************************************************************/

#define MAX_RECALL 9

typedef struct _control
{
char	ProgHome[64];	// The location of the program being executed
char	RamDisk[64];	// The location the cache files
char	PouchPath[64];	// The location of the pouch
char	WorkPath[64];	// Path to a writeable drive
char	CurrentUser[32];// The currently signed in user
int		ComboHeight;	// The owner draw height for combo boxes
int		Points;			// Conversion factor for points
int		Crippled;		// The program cannot save image files
int		MainMemMin;		// The minimum amount of memory to leave behind
int		MainMemFactor;	// The percentage of memory to allocate
WORD	Units;			// 1=inches, 2=mm, 3=picas:points
BOOL	Retail;			// Is this a retail package?
BOOL	DoPicture;		// Should metafiles be cut to the clipbard?
BOOL	DoBitmap;		// Should bitmaps be cut to the clipbard?
LPTR	lpSegment;		// DOS memory as a segment/offset address
BOOL	UseApply;
int		RecallCount;
FNAME	RecallImage[MAX_RECALL];
int		UntitledNo;
BOOL	bNetworkInstall;
int		NewDepth;
int		NewWidth;
int		NewHeight;
int		NewResolution;
BOOL	UseWacom;
WORD	cfImage;
BOOL	Hints;			// 1 = hints configuration on
BOOL	LineArtAsGray;
ITEMID	idMaskTool, idRetouchTool, idFillTool, idDrawTool;
BOOL	Save24BitWallpaper;
RGBS	rgbBackground;	// The image and app background color
int 	UnitRes;		// Goes with APP.Unit
BOOL	ScreenColor;
LFIXED	ScreenWidth;
BOOL	NoUndo;
BOOL	MultipleObjects;
BOOL 	UseMaskAndObjects;
BOOL	UseObjectMarquee;
BOOL	DisplayPixelValue;
int		xImage;
int		yImage;
BOOL	GammaLock;
BOOL 	UndoMasks;
BOOL 	UndoObjects;
int     MaskTint;
BOOL	IndependentSettings;
BOOL	bNoTheme;
int		iAdventBrush;
int		iMaxAnimations;
int		iScrnSaverTime;
BOOL	bUseWaveMix;
//SMS - this didn't quite work out
//BOOL	bAudioHints;	// 1 = audio hints on
//BOOL	bBubbleHints;	// 1 = bubble hints on
} APP;

/********************************************************************* 
 **** TOOLSETTINGS: The Common Tool Settings structure
 *********************************************************************/
typedef struct _toolsettings
{
int		Opacity;
MERGE_MODE	MergeMode;
int		BrushSize;	/* The brush size in pixels */
} TOOLSETTINGS;

/********************************************************************* 
 **** NAMES: The names structure
 *********************************************************************/

typedef struct _names
{
FNAME	Saved;
FNAME	ColorMap;
FNAME	Special;
FNAME	Mask;
FNAME	PasteImageFile;
FNAME	PasteMaskFile;
FNAME	Clipboard;
FNAME	ClipImageFile;
FNAME	ClipMaskFile;
FNAME	CustomBrush;
FNAME	BrushStyle;
FNAME	Printer;
FNAME	PrintStyle;
} NAMES;

/********************************************************************* 
 **** RETOUCH: The retouch settings control structure
 *********************************************************************/

#define TOTAL_BRUSHES 6
// PAINT, CRAYON, MARKER, LINES, SHAPES, ERASER


//*********************************************************************
typedef struct _brushsettings
//*********************************************************************
{
int		BrushShape;
int		BrushSize;
int		Opacity;
int		Pressure;
int		Feather;
MERGE_MODE MergeMode;
FNAME	StyleName;
FNAME	CustomBrushLoaded;
int		OpacityScale;
} BRUSHSETTINGS;
typedef BRUSHSETTINGS far *LPBRUSHSETTINGS;

//*********************************************************************
typedef struct _retouch
//*********************************************************************
{
int		BrushShape;	/* The brush shape: Circle, Square, etc. */
int		BrushSize;	/* The brush size in pixels */
int		Opacity;	/* The transparency percentage value */ 
int		Pressure;
int		Feather;
MERGE_MODE MergeMode;
FNAME	StyleName;	/* The brush style name */
FNAME	CustomBrushLoaded;
HMGXBRUSH hBrush; 	/* The current brush style */
} RETOUCH;

/********************************************************************* 
 **** DRAW:
 *********************************************************************/
typedef struct _draw
{
BOOL	FillInterior;
BOOL	DrawExterior;
WORD	wCircleStyle;
WORD	wSquareStyle;
WORD	wFreeStyle;
BOOL	DoDrawCircle;
} DRAW;

/********************************************************************* 
 **** VIGNETTE:
 *********************************************************************/
typedef struct _vignette
{
int		Gradient;
int		RepeatCount;
BOOL	SoftTransition;
int		VigOpacity;
int		Midpoint;
int		VigColorModel;
MERGE_MODE VigMergeMode;
} VIGNETTE;

/********************************************************************* 
 **** FILL:
 *********************************************************************/
typedef struct _fill
{
int		FillOpacity;
int		FloodRange;
MERGE_MODE FillMergeMode;
int 	idFillColorModel;
int		FloodOpacity;
MERGE_MODE FloodMergeMode;
} FILL;

/********************************************************************* 
 **** VIEW: The view structure
 *********************************************************************/

typedef struct _view
{
BOOL	FullScreen;
BOOL	ZoomWindow;
BOOL	ZoomOut;
BOOL	UseRulers;
LFIXED	ZoomDefaultSize;	// fraction of parent window for def zoom window
RECT	rZoomRect;			// rectangle to fit zoom box in
} VIEW;

/********************************************************************* 
 **** EDIT:
 *********************************************************************/

typedef struct _edit
{
BOOL	Distortable;
BOOL	SmartSize;
LFIXED	Width;
LFIXED	Height;
WORD	ScaleX;
WORD	ScaleY;
WORD	Resolution;
int		Angle;
RECT	Crop;
WORD	CropResolution;
int		Depth;
WORD	SaveUnits;
BOOL	MaintainSize;
WORD	OrigResolution;
LFIXED  OrigWidth;
LFIXED  OrigHeight;
LFIXED	Left;
LFIXED	Top;
} EDIT;

#define CROP_WIDTH  (Edit.Crop.right-Edit.Crop.left+1)
#define CROP_HEIGHT (Edit.Crop.bottom-Edit.Crop.top+1)


/********************************************************************* 
 **** PAGE:
 *********************************************************************/

typedef struct _page
{
int		Type;		// RGB, CYMK, MONO
int		PrintNumeric;
int		OutputType;	// IDC_PRINT_COLOR,GRAY,BLACKSEPS,COLORSEPS
int		SepFlags;	// C, Y, M, K
BOOL	ScatterPrint;
BOOL	Centered;
LFIXED	OffsetX;
LFIXED	OffsetY;
BOOL	TrimMarks;
BOOL	RegMarks;
BOOL	Labels;
BOOL	StepScale;
BOOL	Negative;
BOOL	EmulsionDown;
BOOL	BinaryPS;		// For binary Postscript
BOOL	BinaryEPS;		// For binary Encapsulated Postscript
BOOL	UsePrinterScreening;
} PAGE;
typedef PAGE far *LPPAGE;

// Printer types
#define RGB_SEPS 1
#define CMYK_SEPS 2
#define C_SEP 4
#define M_SEP 8
#define Y_SEP 16
#define K_SEP 32

/********************************************************************* 
 **** HALFTONE:
 *********************************************************************/

#ifdef WIN32
#undef HALFTONE
#endif

typedef struct _halftone
{
BOOL	DoHalftoning;
int		DotShape;
int		Hilight;
int		Shadow;
LFIXED	ScreenRuling[4];
LFIXED	ScreenAngle[4];
} HALFTONE;
typedef HALFTONE _far *LPHALFTONE;

/********************************************************************* 
 **** SEPARATE
 *********************************************************************/

typedef struct _separate
{
BOOL	DoInkCorrect;
BOOL	DoBlackAndUCR;
FNAME	BGName;
MAP		BGMap;
int		UCR;
int		MfromR;
int		YfromG;
int		CfromB;
} SEPARATE;
typedef SEPARATE _far *LPSEPARATE;

/********************************************************************* 
 **** CALIBRATE:
 *********************************************************************/

typedef struct _calibrate
{
BOOL	DoCalibration;
MAP		CalMap[4];
} CALIBRATE;
typedef CALIBRATE _far *LPCALIBRATE;



/********************************************************************* 
 **** MASKS:
 *********************************************************************/

typedef struct _masks
{
int		PasteQuality;			// Transformer quality
int		TransformOpacity;		// Transformer opacity
int		TransformImage;			// Transformer Modify
int		TransMode;				// Transformer 
MERGE_MODE TransformMergeMode;	// Transformer Merge Mode
int		iTransformModes;		// Mask Transformer mode
} MASKS;

/*********************************************************************
 **** COLORMASK: The frame buffer structure
 *********************************************************************/
 
#define NUM_COLORMASKS 8

typedef struct _colormask
{
int		MaskShield;
int		Mask;
COLORINFO MaskColor[NUM_COLORMASKS];
int		MaskRange[NUM_COLORMASKS];
BYTE	RMask[256];
BYTE	GMask[256];
BYTE	BMask[256];
BYTE	CMask[256];
BYTE	MMask[256];
BYTE	YMask[256];
BYTE	KMask[256];
BOOL	Include;
BOOL	On;
int		ShieldMethod;
int		ProbeMethod;
BOOL	IfHidden;
} COLORMASK;

/********************************************************************* 
 **** TEXT:
 *********************************************************************/

typedef struct _fontdesc
{
char	fdFaceName[LF_FACESIZE];
BYTE	fdPitchAndFamily;
BYTE	fdCharSet;
int		FontType;
} FONTDESC;
typedef FONTDESC far *LPFONTDESC;

typedef struct _text
{
BOOL	Italic;
BOOL	Underline;
BOOL	Strikeout;
int		Weight;
int		FontCount;
int		nFont;
int		Size;
LPFONTDESC	lpFontList;
int		AntiAlias;
int		Angle;
int		TextOpacity;
BOOL	AutoFill;
MERGE_MODE	TextMergeMode;
BOOL	CreateObject;
} TEXT;

/********************************************************************* 
 **** TEXTURE:
 *********************************************************************/

typedef struct _texture
{
BOOL	fHorzFlip;
BOOL	fVertFlip;
STRING	TextureName;
int		TextureOpacity;
MERGE_MODE TextureMergeMode;
} TEXTURE;

/********************************************************************* 
 **** CONVERT:
 *********************************************************************/

typedef struct _convert
{
int		MiniType;		// MiniColor Option; 256, 16, or 8
BOOL	fOptimize;
BOOL	fScatter;
BOOL	fDither;
} CONVERT;
typedef CONVERT far *LPCONVERT;


/********************************************************************* 
 **** SAVE:
 *********************************************************************/

typedef struct _save
{
int		Compressed;
BOOL	OKtoSavePath;
BOOL	DoDCS;
int		PreviewBPP;
BOOL	DoSplit;
int		CompositeBPP;
BOOL	ViewThumbnails;
BOOL	AutoThumbnails;		// Auto-Creation of thumbnails
int		ThumbnailHeight;
char	ThumbnailDir[64];	// The location of the thumbnails
char	DriveThumbnailDir[64];  // The location of drive thumbnails
int		FileType;
BOOL	bSaveMask;
} SAVE;

// ChannelEdit
#define EDIT_ALWAYS	0
#define EDIT_IFGREATER	1
#define EDIT_IFLESS	2

/* If this is the main routine, declare the space for the structures */
/* Any other file will automatically have them declared as externals */

#ifdef MAIN
	#define DECLARE ;
#else
	#define DECLARE extern
#endif

DECLARE	WINDOW Window;
DECLARE	APP	Control;
DECLARE	VIEW View;
DECLARE	NAMES Names;
DECLARE	MAP	MasterMap;
DECLARE	MAP	RedMap;
DECLARE	MAP	GreenMap;
DECLARE	MAP	BlueMap;
DECLARE	MAP	SatMap;
DECLARE	MAP	HueMap;
DECLARE COLORMASK ColorMask;
DECLARE	RETOUCH	Retouch;
DECLARE	DRAW	Draw;
DECLARE	VIGNETTE Vignette;
DECLARE	FILL	Fill;
DECLARE	EDIT	Edit;
DECLARE	PAGE	Page;
DECLARE	STYLE	PrintStyle;
DECLARE	HALFTONE Halftone;
DECLARE	SEPARATE Separate;
DECLARE	CALIBRATE Calibrate;
DECLARE	MASKS	Mask;
DECLARE	TEXT	Text;
DECLARE LPSTR	LineBuffer[4];
DECLARE LPSTR	SBLTLineBuffer[4];
DECLARE LPIMAGE lpImage;
DECLARE LPBLT	lpBltScreen;
DECLARE BLTSETTINGS BltSettings;
DECLARE HWND	hZoomWindow;
DECLARE CONVERT Convert;
DECLARE TEXTURE Texture;
DECLARE LPBRUSHSETTINGS lpBrushSettings;
DECLARE TOOL	Tool;
DECLARE TOOLSETTINGS Common;
DECLARE FNAME szPreview;
DECLARE ITEMID idCurrentRoom;
DECLARE ITEMID idPreviousRoom;
DECLARE SAVE Save;
#endif


