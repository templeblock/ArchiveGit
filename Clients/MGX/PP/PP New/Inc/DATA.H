//®PL1¯®FD1¯®BT0¯®TP0¯®RM250¯
#ifdef __routines
#ifndef __data
#define __data

// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// This file contains the global data structures.  Any source file that
// does not reference any global data should not need to include this file.
// Therefore, do not define any structures or types that are used for other
// generic purposes - use TYPES.H.

/********************************************************************* 
 **** WINDOW: The image window structure
 *********************************************************************/

#include "forum.h"
typedef struct _window
{
HDC		hDC;			/* The image window device context */
HCURSOR hCursor;		/* Handle to the image window's current cursor */
BOOL	fLButtonDown;	/* Is the left mouse button down? */
BOOL	fMButtonDown;	/* Is the middle mouse button down? */
BOOL	fRButtonDown;	/* Is the right mouse button down? */
int		iLabelFont;		/* font for ribbon labels and status bar */
HCURSOR hNullCursor;	/* The null blank cursor */
BOOL 	fHasZoom; 		// does the user want a zoom window
HBRUSH	hBGBrush;		// The MDI client backbround brush
HMENU	hObjectMenu;	// Save Object menu
STRING  szObjectText;	// Text of object menu item
} WINDOW;

/********************************************************************* 
 **** TOOLDEF: The tool definition structure
 *********************************************************************/
typedef struct _tooldef
{
ITEMID		idCursor;		// The tool's cursor id
ITEMID		idRibbon;		// The tool's ribbon dialog id
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

typedef enum        // current view mode of image
    {
	WV_NT,
	WV_WIN32S,
    WV_WINDOWS95
    } WIN_VERSION;


typedef struct _control
{
TCHAR	ProgHome[64];	// The location of the program being executed
TCHAR	RamDisk[64];	// The location the cache files
TCHAR	PouchPath[64];	// The location of the pouch
int		ComboHeight;	// The owner draw height for combo boxes
long	Dummy;
int		Points;			// Conversion factor for points
int		Dummy1;
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
LFIXED	NewWidth;
LFIXED	NewHeight;
int		NewResolution;
BOOL	UseTablet;
WORD	cfImage;
WORD	cfImagePP4;
BOOL	Hints;
BOOL	LineArtAsGray;
ITEMID	idMaskTool, idRetouchTool, idFillTool, idDrawTool;
BOOL	Save24BitWallpaper;
RGBS	rgbBackground;	// The image and app background color
int 	UnitRes;		// Goes with APP.Unit
BOOL	ScreenColor;
LFIXED	ScreenWidth;
int		ScratchpadWidth;
int		ScratchpadHeight;
BOOL	IsScratchpadColor;
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
TCHAR	PluginPath1[64]; // First  place to look for plugins
TCHAR	PluginPath2[64]; // Second place to look for plugins
BOOL    UsePlugins;
BOOL	IndependentSettings;
int    ScanMapMode;
int	DefaultFileType;
BOOL	UseThreading;
#ifdef WIN32
DWORD	ThreadDataTlsIndex;
CRITICAL_SECTION StatusCriticalSection;
CRITICAL_SECTION UntitledCriticalSection;
#endif
BOOL	UseFunctionThreading;
WIN_VERSION	WinVer;
BOOL	NTBeta;
DITHER_TYPE DefaultDitherType;
BOOL	DoCommandList;
BOOL	UseShortCommandNames;
BOOL    UseFileForClipboard;	// True if clipboard transfers use a file (instead of memory)
BOOL	CMSEnabled;				// Color Management system enabled
BOOL	CMSOverRide;			// Color Management system enabled
BOOL	bUseCMS;				// Color Management system enabled
BOOL	WriteFileOnPaste;
BOOL	UseProjectDirectory;
BOOL	Dummy2;
BOOL	UseGrayScaleIcons;
BOOL	DisableAutoScroll;
BOOL	UseBrushEditColor;
BOOL	fPrintCalVisual;
BOOL    TGACMYK;     
BOOL	fPCDUseCMS;				// Photo CD CMS enabled
DWORD	dwImageID;
BOOL    bUnlinkWintab;
BOOL	bNoHiResRebuild;
WORD	cfImage5Ptr;		    // Ptr to pp5.0 lpImage 
WORD    wDaysLeft, wTrialsLeft; // Bomb vars 
BOOL    bIntruder, bFirstTime;  // Bomb vars
BOOL	fNewLook;
} APP;
  
/*=======================================================================*\
	BLTSETTINGS: The super blt settings
\*=======================================================================*/
typedef struct _bltsettings
{
	LFIXED	RGamma;
	LFIXED	GGamma;
	LFIXED	BGamma;
	BOOL	fGammaDisable;
}
BLTSETTINGS, FAR *LPBLTSETTINGS;

/********************************************************************* 
 **** BROWSER: The browser structure
 *********************************************************************/

typedef struct _browser
{
TCHAR	ThumbnailDir[64];	// The location of the thumbnails
TCHAR	DBFInfoDir[64];  	// The location of Album and Directory DBFs
TCHAR	AlbumFileDir[64];	// Where files go when saved to an album
BOOL	ViewThumbnails;		// In Thumbnail Mode ??
BOOL	AutoThumbnails;		// Auto-Creation of thumbnails
BOOL 	ViewAlbums;			// In Album Mode??
WORD	bsrFileType;		// Default file type for image browser(IDT_)
TCHAR	LastAlbum[64];		// Default album for combo box
TCHAR	SearchAlbum[64];	// Default album for search results
int		Dummy;
BOOL	OKtoSavePath;		// Save path when file is opened/saved
BOOL	ConfirmOnDelFile;
BOOL	ConfirmOnMoveFile;
BOOL	ConfirmOnCopyFile;
BOOL	ConfirmOnRenameFile;
BOOL	ConfirmOnAddFile;
BOOL	ConfirmOnCreateDir;
BOOL	ConfirmOnRenameDir;
BOOL	ConfirmOnDeleteDir;
BOOL	ConfirmOnRenameAlbum;
BOOL	ConfirmOnDeleteAlbum;
TCHAR	DriveThumbnailDir[64];	// The location of the thumbnails on removable media
TCHAR	DiskMarker[13];			// The name of the hidden readonly marker file
BOOL  	bPTRows;				// Print thumbs use rows?
int		PTNumRows;				// Print thumbs number of columns
int		PTNumCols;				// Print thumbs	number of rows
BOOL 	CreateThumbnailOnSave; 	// create thumbnails when saving files
WORD	bsrImpFileType;			// Default file type for import browser(IDT_)
BOOL 	UseVolumeLabels;		// For WFW bug reading labels of dismounted drives
BOOL    WarnReadOnly;
} BROWSER;

// for extened names browsers
typedef struct _extbrowser
{
	// Paste From/copy To
	BOOL PasteAutoThumbnails;		// autocreate thumbnails in dialog
	BOOL PasteThumbCreate;			// Create thumbs on copy to?
	BOOL PasteViewThumbnails;		// View Thumbs / view file names.
} EXTBROWSER;

/********************************************************************* 
 **** IMPORTSETUP: The Import setup structure
 *********************************************************************/
typedef struct {
    int Import_Anti_Alias;
    int Import_Resolution;     
    int Import_Units;     
    int Import_BitmapMode;
} IMPORTSETUP, FAR * LPIMPORTSETUP;

/********************************************************************* 
 **** TOOLSETTINGS: The Common Tool Settings structure
 *********************************************************************/
typedef struct _toolsettings
{
int		iModes;
int		Opacity;
MERGE_MODE	MergeMode;
int		idColorModel;
int		Range;
LFIXED	Width;
LFIXED	Height;
int		Constrain;
int		BrushShape;	/* The brush shape: Circle, Square, etc. */
int		BrushSize;	/* The brush size in pixels */
int		Pressure;
int		Feather;
BOOL	DoCircle;
int		CommonUnits;
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
FNAME	Scanner;
FNAME	ScanMap;
FNAME	PasteImageFile;
FNAME	PasteMaskFile;
FNAME	Clipboard;
FNAME	ClipImageFile;
FNAME	ClipMaskFile;
FNAME	CustomBrush;
FNAME	MainManifest;
FNAME	SupManifest;
FNAME	InstallerDisk;
FNAME	Macro;
FNAME	StartMacro;
FNAME	ColorManager;
FNAME	BrushStyle;
FNAME	Printer;
FNAME	PrintStyle;
FNAME	Shape;
FNAME	szPackageDir;
} NAMES;

/********************************************************************* 
 **** QTONE: The tone balance structure
 *********************************************************************/
typedef struct _qtone
{
int		Dummy1;
} QTONE;

/********************************************************************* 
 **** MAPPING: For mapping dialogs
 *******************************************************************/
typedef struct _mapping
{
BOOL Map_bAuto;			// autopreview
int Map_nPoints;		// default points
BOOL Map_bGrid;			// use grid in map control
BOOL Map_bPercent;		// use percentages or values 
BOOL Map_bIntensity;	// use intensity (white = 255) 
BOOL Map_bCurves;		// use curve interpolation?
BOOL Hue_bAuto;			// not used 
BOOL Quarter_bAuto;		// autopreview
BOOL Quarter_HSacrifice;	// how much highlight to sacrafice
BOOL Quarter_SSacrifice;	// how much shadow to sacrafice
BOOL Quarter_bUseMidPer;	// Midtone by percent or value
BOOL Quarter_bPercent;	// use percentages or values 
int  Map_CurveType;		// The type of curve to use (smoothness)
BOOL Poster_bAuto;		// autopreview
BOOL ContBrit_bAuto;	// autopreview
BOOL Balance_bAuto;	    // autopreview
BOOL HueShift_bAuto;	// autopreview
BOOL HueMap_bAuto;	    // autopreview
} MAPPING;

/********************************************************************* 
 **** MACRO: The macro structure
 *********************************************************************/
typedef struct _macro
{
int		nRepeat;
BOOL	fJournalMacro;
BOOL	fBatchClose;
int		BatchSave;
FNAME	BatchDir;
FNAME	BatchAlbum;
BOOL	fBatchChangeType;
ITEMID	BatchFileType;
} MACRO;

/********************************************************************* 
 **** PALETTESETTINGS: The palette edit structure
 *********************************************************************/
typedef struct _palettesettings
{
BOOL			fReMap;
DITHER_TYPE	DitherType;
} PALETTESETTINGS;

/********************************************************************* 
 **** RETOUCH: The retouch control structure
 *********************************************************************/

#define RETOUCH_STYLES	6
#define FILTER_STYLES	4
#define STROKE_STYLES	1
#define DRAW_STYLES		3

#define NO_BRUSH -1
#define PAINT_BRUSH 	0
#define SPRAY_BRUSH 	1
#define CLONE_BRUSH		2
#define TEXTURE_BRUSH	3
#define SMEAR_BRUSH		4
#define ERASER_BRUSH	5
#define SHARPEN_BRUSH	6
#define SMOOTH_BRUSH	7
#define LIGHTEN_BRUSH	8
#define DARKEN_BRUSH	9
#define DRAW_RECT_BRUSH 10
#define DRAW_FREE_BRUSH	11
#define DRAW_PENCIL_BRUSH 12
#define PAINTON_BRUSH	13
#define STROKEMASK_BRUSH 14

#define TOTAL_BRUSHES 15

//*********************************************************************
typedef struct _brushsettings
//*********************************************************************
{
int		 BrushShape;
int		 BrushSize;
int		 Opacity;
int		 Pressure;
MERGE_MODE MergeMode;
int		 Feather;
int		 OpacityScale;
} BRUSHSETTINGS;
typedef BRUSHSETTINGS FAR *LPBRUSHSETTINGS;

//*********************************************************************
typedef struct _retouch
//*********************************************************************
{
int		BrushShape;	/* The brush shape: Circle, Square, etc. */
int		BrushSize;	/* The brush size in pixels */
int		Opacity;	/* The transparency percentage value */ 
FNAME	RetouchStyleName[RETOUCH_STYLES]; /* The brush style name */
FNAME	FilterStyleName[FILTER_STYLES]; /* The brush style name */
FNAME	DrawStyleName[DRAW_STYLES]; /* The brush style name */
FNAME	StrokeStyleName[STROKE_STYLES]; /* The brush style name */
HMGXBRUSH hBrush; 	/* The current brush style */
FNAME	CustomBrushLoaded;
int		Pressure;
MERGE_MODE MergeMode;
int		Feather;
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
BOOL	DisableDither;
} VIGNETTE;

/********************************************************************* 
 **** FILL:
 *********************************************************************/
typedef struct _fill
{
int		FillOpacity;
int		FloodRange;			// 0-100%
int		FloodFade;
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
LFIXED	Angle;
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
BOOL	IsCCW;
COLORINFO FillColor;
} EDIT;


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
int	PrintScaling;	// For overriding printer scaling
						// 0 - no scaling down of images during print
						// 1 - scale images down during print (default)
						// >1 - real resolution of printer 
BOOL	UsePrinterScreening; // Use GDI printer screening
BOOL	GeneratePS;	// generate PostScript ourselves
} PAGE;
typedef PAGE FAR *LPPAGE;

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

typedef struct _halftone
{
BOOL	DoHalftoning;
int		DotShape;
LFIXED	ScreenRuling[4];
LFIXED	ScreenAngle[4];
} HTSETTINGS;
typedef HTSETTINGS FAR *LPHTSETTINGS;

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
int		BlackLimit;
int		TotalInk;
CMSINFO	cmsInfo;
int		SatBoost;
} SEPARATE;
typedef SEPARATE FAR *LPSEPARATE;

/********************************************************************* 
 **** CALIBRATE:
 *********************************************************************/

#define CAL_MAPS	5
#define CAL_MASTER	0
#define CAL_CYAN	1
#define CAL_MAGENTA	2
#define	CAL_YELLOW	3
#define CAL_BLACK	4
#define CAL_RED		CAL_CYAN
#define CAL_GREEN	CAL_MAGENTA
#define CAL_BLUE	CAL_YELLOW
#define CAL_GRAY	CAL_BLACK

typedef struct _calibrate
{
BOOL	DoCalibration;
MAP		CalMap[CAL_MAPS];
int		DotGain;
BOOL	DoCalOnConvert;
int		Hilight;
int		Shadow;
} CALIBRATE;
typedef CALIBRATE FAR *LPCALIBRATE;


/********************************************************************* 
 **** MASKS:
 *********************************************************************/

typedef struct _masks
{
int		Dummy1;					// Unknown
int		AutoSearchDepth;		// Automask stuff
int		AutoMinEdge;			// Automask stuff
int		ConstrainRectAspect;	// Shape Mask Method
LFIXED	ConstrainRectWidth;		// Shape Mask Width
LFIXED	ConstrainRectHeight;	// Shape Mask Height
int		ConstrainCropAspect;	// Crop Method
LFIXED	ConstrainCropWidth;		// Crop Width
LFIXED	ConstrainCropHeight;	// Crop Height
int		WandRange;				// Magic Wand Range
int		WandFade;				// Magic Wand Fade (0-100%)
BOOL	Dummy2;					// Transformer - UNUSED
int		Dummy3;					// Transformer - UNUSED
int		PasteQuality;			// Transformer quality
int		TransformOpacity;		// Transformer opacity
int		TransformImage;			// Transformer Modify
int		PointerFunction;		// Point Editor
BOOL	AutoMask;				// Free Mask Method
int		TransMode;				// Transformer 
int		iShapeModes;			// Shape Mask Mode
int 	idMagicColorModel;		// Magic Wand Color Model
BOOL	PointEdit;				// Free Mask Method
int 	MakePathContinuity;		// Pointer Editor, for curves
int 	MakePathCurveTolerance;	// Pointer Editor, for curves
MERGE_MODE TransformMergeMode;	// Transformer Merge Mode
int		wFeatherEdge;			// Feather Edge Type - one of below
int		FeatherEdgeSoft;		// Feather Soft setting
int		FeatherEdgeNormal;		// Feather Normal setting
int		FeatherEdgeHard;		// Feather Hard setting
BOOL	DoMaskCircle;			// Shape Mask Shape
int		FeatherAmount;			// Feather Radius
int		FeatherInside;			// Feather Direction
int 	idAutoColorModel;		// AutoMask Color Model
int 	GrowAmount;				// Magic Wand Grow value
int 	PointIModes;			// Point Editor mode
BOOL	DeleteMask;				// Load Mask
int		iFreeModes;				// Free Mask mode
int		iPaintOnModes;			// PaintOn Mask mode
int		iWandModes;				// Magic Wand mode
int		iTransformModes;		// Mask Transformer mode
BOOL	PaintOnObject;			// PaintOn Mask
int		StrokeTool;				// Stroke Mask
int		ShapeUnits;				// Shape Mask Units
int		CropUnits;				// Crop Tool Units
int 	   MakePathType;			// Pickup Beziers or Lines
BOOL     OneBitMask;          	// One Bit Masks for LineArt, and Palette images
BOOL	PasteAtRealSize;		// Paste objects at real world size
int 	iShapeScale;				// Scale paths on load
BOOL 	FreeAntialias;					// Antialias in freehand mask tool
BOOL 	PointAntialias;					// Antialias in mask point editor
BOOL	DisplayQuality;			// Paste display quality
int 	MakePathTolerance;		// Pointer Editor, for lines
} MASKS;

/********************************************************************* 
 **** SCAN:
 *********************************************************************/

#define CALSCAN_MAPS	4
#define CALSCAN_GRAY 	0
#define CALSCAN_RED		1
#define CALSCAN_GREEN	2
#define	CALSCAN_BLUE	3

typedef struct _scans
{
BOOL	LockAspect;
BOOL	SavePreview;
LPFRAME lpScanFrame;	/* The Prescan bitmap frame */
BOOL	DoCalibration;
MAP		CalMap[CALSCAN_MAPS];
BOOL	ColorPreview;
BOOL	ScanLAInvert;
BOOL	fScanUseCMS;
BOOL	fScanCalVisual;
BOOL	fUseRGBForGray;
BOOL	fUseGrayForRGB;
} SCANS;

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

#define GetDefMergeMode(N,INT) (N=(MERGE_MODE)GetDefaultInt(#N,INT))

class CTextForum : public CForum
{
	public:
	void Load();
	void Save();
	void FillFontList();
	void SetAttr(LPFONTID lpFontID, int just, int AntiAlias);
	int  GetFontDesc(LPTSTR lpFaceName, LPFONTDESC lpFontDesc);
	void FillCombo(HWND hWnd);
	void SortFonts();

	LPFONTLIST 	GetFontList(){return(FontList);}
	LPFONTID	GetCurrentFont(){return(&CurrentFont);}
	LPTSTR		GetFontName(){return(FontName);}
	int			GetFontIndex(){return(FontIndex);}
	void		SetFontIndex(int index,CAttendee * pSkip = NULL);
	DEFINE_FORUM_TOPIC(AntiAlias, int);
	DEFINE_FORUM_TOPIC(Opacity, int);
	DEFINE_FORUM_TOPIC(MergeMode, MERGE_MODE);
	DEFINE_FORUM_TOPIC(Justification, int);
	protected:
	int			FontIndex;
	TCHAR		FontName[LF_FACESIZE];
	LPFONTLIST	FontList;
	FONTID		CurrentFont;
};

class CZoomForum : public CForum
{
public:
	CZoomForum();
	~CZoomForum() {}
	void FillCombo(HWND hWnd);

	int			GetViewPercentage() { return m_iPercentage; }
	void		SetViewPercentage(int iPercentage,CAttendee * pSkip = NULL);
protected:
	int			m_iPercentage;
};

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
 **** STITCH:
 *********************************************************************/
typedef struct _stitch
{
BOOL	AutoBrightness;
int		iBrightness;
BOOL	AutoBlend;
int		Pressure;
BOOL	SmartStitch;
int		iTransform;
} STITCH;

/********************************************************************* 
 **** CONVERT:
 *********************************************************************/

typedef struct _convert
{
ITEMID			MiniType;		// MiniColor Option; 256, 16, or 8
BOOL			fOptimize;
BOOL			fScatter;
BOOL			fDither;
CONVERT_TYPE	ConvertType;
DITHER_TYPE		ConvertDitherType;
EXTNAME			ConvertPalette;
int				ConvertColors;
} CONVERT;
typedef CONVERT FAR *LPCONVERT;

/********************************************************************* 
 **** HINTS:
 *********************************************************************/

typedef struct HINTS_tag
{
	BOOL fBubbleHintsOn;
	BOOL fStatusHintsOn;
	BOOL fHintWindowOn;
} HINTS;
typedef HINTS FAR *LPHINTS;

// Channel
#define CHANNEL_RGB	0
#define CHANNEL_HSL	1
#define CHANNEL_R	2
#define CHANNEL_G	3
#define CHANNEL_B	4
#define CHANNEL_H	8
#define CHANNEL_S	9
#define CHANNEL_L	10

// ChannelEdit
#define EDIT_ALWAYS	0
#define EDIT_IFGREATER	1
#define EDIT_IFLESS	2

// ChannelCombine
#define COMBINE_REPLACE	0
#define COMBINE_ADDITIVE 1
#define COMBINE_SUBTRACTIVE 2

extern WINDOW Window;
extern APP	Control;
extern VIEW View;
extern NAMES Names;
extern MAPPING Mapping;
extern MAP	MasterMap;
extern MAP	RedMap;
extern MAP	GreenMap;
extern MAP	BlueMap;
extern MAP	BlackMap;
extern MAP	SatMap;
extern MAP	HueMap;
extern COLORMASK ColorMask;
extern RETOUCH	Retouch;
extern DRAW	Draw;
extern VIGNETTE Vignette;
extern FILL	Fill;
extern EDIT	Edit;
extern PAGE	Page;
extern STYLE	PrintStyle;
extern HTSETTINGS Halftone;
extern SEPARATE Separate;
extern CALIBRATE Calibrate;
extern MASKS	Mask;
extern CTextForum	TextForum;
extern CZoomForum	ZoomForum;
extern STITCH  Stitch;
extern LPTR	LineBuffer[4];
extern LPBLT	lpBltScreen;
extern BLTSETTINGS BltSettings;
extern HWND	hZoomWindow;
extern CONVERT Convert;
extern SCANS	Scans;
extern TEXTURE Texture;
extern QTONE	Qtone;
extern BROWSER Browser;
extern EXTBROWSER ExtBrowser;
extern LPBRUSHSETTINGS lpBrushSettings;
extern int		RetouchBrushCurrent;
extern TOOL	    Tool;
extern EPSOPTIONS EPSOptions;
extern TIFFOPTIONS TIFFOptions;
extern TGAOPTIONS TGAOptions;
extern JPEGOPTIONS JPEGOptions;
extern PPFFOPTIONS PPFFOptions;
extern PPFFOPENOPTIONS PPFFOpenOptions;
extern AVIOPTIONS AVIOptions;
extern TOOLSETTINGS Common;
extern MACRO Macro;
extern PALETTESETTINGS PaletteSettings;
extern HINTS Hints;
extern COLORMANAGER ColorManager;
extern HINSTANCE   hInstIconLib;   
extern MAP Map1;
extern MAP Map2;
extern MAP Map3;
extern MAP Map4;
extern MAP Map5;
extern BYTE Step1[CALPOINTS];
extern BYTE Step2[CALPOINTS];
extern BYTE	Step3[CALPOINTS];
extern BYTE Step4[CALPOINTS];

#ifdef WIN32
extern SYSTEM_INFO SystemInfo;
#endif

#endif
#endif


