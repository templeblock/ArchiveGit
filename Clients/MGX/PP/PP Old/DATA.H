/*®TP5¯®BT5¯®RFA®FC¯Page ®PN¯ of ®FP¯ -- ppv11a ®VA$FI¯ -- 1:53 PM , May 31, 1989¯®PL1¯®FD1¯®TP0¯®BT0¯*/
#ifndef TYPES
    #include "types.h"
#endif

typedef struct _bmi
    {
    BITMAPINFOHEADER	bmiHeader;
    union {
	RGBQUAD		Colors[256];
	WORD		Index[256];
	} bmi;
} BMI;
typedef BMI far *LPBMI;

/********************************************************************* 
 **** LINEPOOL: The line pool buffer structure
 *********************************************************************/

typedef struct _linepool {
    struct _linepool far	*Next;
    long			Size;
} LINEPOOL;
typedef LINEPOOL far *LPLINEPOOL;

/********************************************************************* 
 **** FRAME: The frame buffer structure
 *********************************************************************/

typedef struct _frame
{
BYTE	Depth;
BYTE	MustUnpack;
BYTE	ReleaseMode;
WORD	Xsize;
WORD	Ysize;
WORD	Width;
WORD	Resolution;
WORD	Background;
int	CacheSeq;
int	CacheFile;
LPMASK	ReadMask;
LPMASK	WriteMask;
LPTR	BackScratch;
LPADDR	AddrList;
LPLINK	LinkList;
LPLINK	HeadLink;
LPPTR	FreePool;
LPPTR	FreePoolEnd;
LPPTR	FreeNext;
LPLINEPOOL LinePool;
LPTR 	LinePtr;  // Unpacked lineart buffer
int	CacheLine; // Linenumber in the unpacked buffer
LPROC	DoSample;
LPROC	DoSampleMW;
LPROC	DoMirror;
LPFL	DoToCache; // return a long
LPROC	DoGetPixelHSL;
LPROC	DoGetPixelRGB;
LPROC	Dummy1;
LPROC	Dummy2;
LPROC	DoPutPixel;
LPROC	DoGetPixel;
LPROC	DoKernal;
LPFL	DoSharpSmooth; // return a long
LPFL	DoEdgeValue; // return a long
LPROC	DoFlood;
LPROC	DoSetPixels;
LPROC	DoMapPixels;
} FRAME;
typedef FRAME far *LPFRAME;

#define RELEASE_NEWEST 0
#define RELEASE_OLDEST 1

/********************************************************************* 
 **** DISPLAY: The display structure
 *********************************************************************/
typedef struct display_struct {
RECT	DispRect;	/* The display rectangle */
RECT	FileRect;	/* The file rectangle */
FIXED	DispRate;	/* The ratio of display pixels to file pixels */
FIXED	FileRate;	/* The ratio of file pixels to display pixels */
int	ViewPercentage;	/* The viewing percentage */
int	xDiva;		/* x-start position of dither */
int	yDiva;		/* y-start position of dither */
RECT	LastWindowRect;	/* The last display rectangle */
RECT	LastFileRect;	/* The last file rectangle */
FIXED	LastFileRate;	/* The last ratio of file pixels to display pixels */
int	LastViewPercentage; /* The last viewing percentage */
LPROC	FloatingImageProc;	/* proc for redisplaying floating image */
LPROC	FloatingGraphicsProc;	/* proc for redisplaying floating grahpics */
RECT	UpdateRect;
RECT	WindowRect;
BOOL	ResizeType;
BOOL	HasRulers;
} DISPLAY;
typedef DISPLAY far *LPDISPLAY;

#define DISP_DX (lpImage->lpDisplay->DispRect.right - lpImage->lpDisplay->DispRect.left + 1)
#define DISP_DY (lpImage->lpDisplay->DispRect.bottom - lpImage->lpDisplay->DispRect.top + 1)
#define FILE_DX (lpImage->lpDisplay->FileRect.right - lpImage->lpDisplay->FileRect.left + 1)
#define FILE_DY (lpImage->lpDisplay->FileRect.bottom - lpImage->lpDisplay->FileRect.top + 1)
#define ONIMAGE( x, y ) \
	( (x) >= lpImage->lpDisplay->DispRect.left && (x) <= lpImage->lpDisplay->DispRect.right && \
	  (y) >= lpImage->lpDisplay->DispRect.top && (y) <= lpImage->lpDisplay->DispRect.bottom )
#define ONIMAGE1( x, y ) \
	( (x) > lpImage->lpDisplay->DispRect.left && (x) < lpImage->lpDisplay->DispRect.right && \
	  (y) > lpImage->lpDisplay->DispRect.top && (y) < lpImage->lpDisplay->DispRect.bottom )

/********************************************************************* 
 **** IMAGE: The image structure
 *********************************************************************/

typedef struct image_struct {
long	dirty;		/* has the image display been modified? */
FNAME	CurFile;	/* Name of current picture in the window */
WORD	hWnd;		// Handle to the image window containing this image
int	npix, nlin;	/* Number of high res pixels in image */
int	DataType;	/* Lineart (IDC_SAVELA), Contone (IDC_SAVECT), etc. */
int	FileType;	/* TIFF (IDC_SAVETIFF), EPS (IDC_SAVEEPS), etc. */
BOOL 	Compressed;	/* Was the file read in as compressed */
int	xres, yres;	/* Picture resolution values in pixels/inch */
BOOL	fChanged;	/* current file modified? */
BOOL	fUntitled;	/* current file untitled? */
BYTE	Depth;		/* data depth - 1, 2, or 3 */
LPFRAME EditFrame;	/* editing frame */
LPFRAME UndoFrame;	/* undo frame */
BOOL    NoUndo;		/* cannot create undo for this image */
LPTR	EditFlags;	/* Dirty Lines buffer */
RECT	UndoRect;	/* Undo area in file coordinates */
BOOL 	UndoNotRedo;	/* Toggle between undo and redo */
int	MarqueeStatus;	/* The draw status of the marquee */
WORD	hPal;		/* Color palette for this image */
int 	nPaletteEntries; /* entries in Palette */
PALETTEENTRY Palette[257]; /* current realized palette */
LPDISPLAY lpDisplay;	/* pointer to current view display state */
BOOL	fNewFrame; 	/* image has new edit frame, undo frame different */
int	ImageType;	/* IMG_DOCUMENT, IMG_BRUSH, IMG_CLIPBOARD */
FNAME	ImageName;	/* Name of the image */
} IMAGE;
typedef IMAGE far *LPIMAGE;

#define IMG_DOCUMENT		1
#define IMG_BRUSH		2
#define IMG_CLIPBOARD		3

#define MARQUEE_NOT_READY	128
#define MARQUEE_CYCLE		64

#define INFILE( x, y ) \
	( (x) >= 0 && (x) < lpImage->npix && (y) >= 0 && (y) < lpImage->nlin )
#define INFILE1( x, y ) \
	( (x) > 0 && (x) < lpImage->npix-1 && (y) > 0 && (y) < lpImage->nlin-1)
//#define INFILE( x, y) \
//	( (x) >= lpImage->FileRect.left && (x) <= lpImage->FileRect.right && \
//	  (y) >= lpImage->FileRect.top && (y) <= lpImage->FileRect.bottom )
//#define INFILE1( x, y) \
//	( (x) > lpImage->FileRect.left && (x) < lpImage->FileRect.right && \
//	  (y) > lpImage->FileRect.top && (y) < lpImage->FileRect.bottom )

/* Defines for image 'dirty' */
#define DIRTY_RETOUCH	1L
#define DIRTY_EDIT	2L
#define DIRTY_PASTE	4L
#define DIRTY_MASK	8L
#define DIRTY_TEMP	16L
#define DIRTY_TINT	32L
#define DIRTY_INVERT	64L
#define DIRTY_FLIP	128L
#define DIRTY_VIGNETTE	256L
#define DIRTY_BLEND	512L
#define DIRTY_CLEAR 1024L
#define DIRTY_SHARPSMOOTH 2048L
#define DIRTY_PROCESS	4096L
#define DIRTY_FILTER   8192L
#define DIRTY_CUT 16384L
#define DIRTY_MAP 32768L
#define DIRTY_HUE  65536L
#define DIRTY_CB   131072L
#define DIRTY_TONEMAP 262144L
#define DIRTY_THRESHOLD 524288L
#define DIRTY_POSTERIZE 1048576L
#define DIRTY_SATURATION 2097152L
#define DIRTY_QUARTERTONES 4194304L
#define DIRTY_APPLYMAP	8388608L
#define DIRTY_QT		16777216L
#define DIRTY_TEXT		33554432L

/********************************************************************* 
 **** WINDOW: The picture window control structure
 *********************************************************************/

typedef struct _window
{
LPROC	ToolProc;	/* The active function procedure for the image */
WORD	hButtonPen;	/* Handle to a button face pen */
WORD	hHilightBrush;	/* Handle to a green hilight brush */
WORD	hButtonBrush;	/* Handle to a button face brush */
WORD	hShadowBrush;	/* Handle to a shadow brush */
WORD	hWhiteBrush;	/* Handle to a white brush */
WORD	hBlackBrush;	/* Handle to a black brush */
WORD	hShadowPen;	/* Handle to a shadow pen */
WORD	hWhitePen;	/* Handle to a white pen */
WORD	hBlackPen;	/* Handle to a black pen */
BOOL	ToolActive;	/* Is the tool active or not */
BOOL	fButtonDown;	/* Is the left mouse button down? */
HDC	hDC;		/* The image window device context */
WORD	hDottedPen;	/* Handle to a dotted pen for the marquee */
WORD	hHelv10Font;	/* Handle to a Helvetica 10 font */
WORD	hCursor;	/* Handle to the image window's cursor */
WORD	idOptionBox;	/* The option dialog box id */
FARPROC OptionProc;	/* The proc for the option dialog box */
LPROC	ModalProc;	/* The active function procedure for the image */
WORD	hNullCursor;	/* The null blank cursor */
WORD	hLabelFont;	/* font for labels in option box */
} WINDOW;

/********************************************************************* 
 **** BLT: The super blt structure
 *********************************************************************/

typedef struct _blt
{
BMI	BitMapInfo;	/* The DIB info structure */
WORD	hGrayPal;	/* Handle to a Windows palette */
WORD	hColorPal;	/* Handle to a Windows palette */
LPROC	fpConvert1;	/* Display convertor for depth 1 images */
LPROC	fpConvert2;	/* Display convertor for depth 2 images */
LPROC	fpConvert3;	/* Display convertor for depth 3 images */
LPROC   fpConvert1C;	/* Convertor for depth 1 images with color palette */
LPROC   fpConvert2G;	/* Convertor for depth 2 images with gray palette */
LPROC   fpConvert3G;	/* Convertor for depth 3 images with gray palette */
HPALETTE hPal;		/* Current Display Palette */
} BLT;
typedef BLT far *LPBLT;

/********************************************************************* 
 **** CONTROL: The main program structure
 *********************************************************************/

#define MAX_RECALL 5

typedef struct _control
{
char	ProgHome[64];	/* The location of the program being executed	*/
char	RamDisk[64];	/* The location of the ramdisk (cache file)	*/
char	EPSSplitDir[64]; /* The location of the EPS hires files		*/
BYTE	Status[64];	/* The last status string displayed */
int	Function;	/* Command word indicating the active function	*/
int	Crippled;	/* The program cannot save image files		*/
int	FGPort;		/* Physical port for Willow Frame grabber	*/
int	SmartPaste;	/* Flag to indicate the paste transform to use	*/
int	MainMemMin;	/* The minimum amount of memory to leave behind */
int	MainMemFactor;	/* The percentage of memory to allocate		*/
WORD	Units;		/* 1=inches, 2=mm, 3=picas:points		*/
WORD	EMSdisable, EMSavail, EMStotal;
long	Dummy;
BOOL	InPicwin;	/* Is the Picture window being displayed? */
BOOL	ColorEnabled;	/* Is color enabled in this program? */
BOOL	Junior;		/* Is this a Junior version? */
BOOL	Sound;		/* Should sound be used for the tutorial? */
BOOL	Is30;		/* Is this Windows 3.0? */
BOOL	IsPmode;	/* Is this Windows protected mode? */
LPTR	lpSelector;	/* DOS memory as a selector/offset address */
LPTR	lpSegment;	/* DOS memory as a segment/offset address */
BOOL	UseApply;
POINT	ImageWhere;
int	RecallNext;
int	RecallCount;
FNAME	RecallImage[MAX_RECALL];
int	UntitledNo;
BOOL	bPreview;
BOOL	Dummy1;
char	PouchPath[64];	/* The location of the pouch			*/
WORD	ImageType;
FIXED	ScreenWidth;
BOOL	IsEnchanted;	/* Is this Windows Enchanted(Enhanced) mode */
int	ScratchpadWidth;
int	ScratchpadHeight;
BOOL	IsScratchpadColor;
BOOL	ZoomWindow;
BOOL	ZoomOut;
BOOL	NewIsColor;
} CONTROL;

#define MODAL_PASTE 1
#define MODAL_QUARTERTONE 2
#define MODAL_DENSITYMASK 3

/********************************************************************* 
 **** NAMES: The names structure
 *********************************************************************/

typedef struct _names
{
FNAME	Saved;
FNAME	Map;
FNAME	Smooth;
FNAME	Sharp;
FNAME	Edge;
FNAME	Special;
FNAME	Mask;
FNAME	Scanner;
FNAME	Grabber;
FNAME	ScanMap;
FNAME	Printer;
FNAME	PrintStyle;
FNAME	Clipboard;
FNAME	ClipImageFile;
FNAME	ClipMaskFile;
FNAME	PrintMap;
} NAMES;
typedef NAMES far *LPNAMES;

/********************************************************************* 
 **** OLDMAP: The graymap control structure
 *********************************************************************/

typedef struct _oldmap
{
int	Type;		/* The graymap sub-function */
int	Color;		/* Is this a HMAP, SMAP, or LMAP structure? */
int	Levels;		/* Number of posterization levels 0-256 */
int	Negate;		/* Is the image negated? */
int	Threshold;	/* The value at which image is thresheld 0-100 */
int	Points;		/* The number of graymap points from 3-11 */
BYTE	PntX[11];	/* Curve break points in pixels from 0-255 */
BYTE	PntY[11];
BYTE	Unused0[43];
char	Contrast;	/* The Contrast setting from -50 to +50 */
char	Brightness;	/* The Brightness setting from -50 to +50 */
BYTE	Lut[256];	/* the lut created from the values in this table */
BOOL	bStretch;	/* Should the points be stretched to 0 and 100%? */
BYTE	Unused1[62];
} OLDMAP;
typedef OLDMAP far *LPOLDMAP;

/********************************************************************* 
 **** MAP: The map structure
 *********************************************************************/

#define MINPOINTS 3
#define MAXPOINTS 12
#define CALPOINTS 11
#define MAPPOINTS 11
#define LUTSIZE 256
#define DLUTSIZE 64
#define QTONEPOINTS 5

typedef struct _map
{
int	Color;		/* Is this a HMAP, SMAP, or LMAP structure? */
int	Levels;		/* Number of posterization levels 0-256 */
int	Negate;		/* Is the image negated? */
int	Threshold;	/* The value at which image is thresheld 0-100 */
int	Points;		/* The number of graymap points from 3-11 */
int	Contrast;	/* The Contrast setting from -100 to +100 */
int	Brightness;	/* The Brightness setting from -100 to +100 */
BOOL	bStretch;	/* Should the points be stretched to 0 and 100%? */
BOOL	bModified;	/* Has the map been modified? */
POINT	Pnt[MAXPOINTS]; /* Curve break points in pixels from 0-255 */
BYTE	Lut[LUTSIZE];	/* the lut created from the values in this table */
} MAP;
typedef MAP far *LPMAP;

/* defines for map.Color */
#define HMAP 1
#define SMAP 2
#define LMAP 3
#define RMAP 4
#define GMAP 5
#define BMAP 6
#define CMAP 7
#define MMAP 8
#define YMAP 9

/********************************************************************* 
 **** QTONE: The quartertones structure
 *********************************************************************/
typedef struct _quartertone
{
POINT	QTfile[QTONEPOINTS];	/* File locations of quartertone points */
POINT	QTdisp[QTONEPOINTS];	/* Display locations of quartertone points */
BYTE	QTvalue[QTONEPOINTS];	/* Values at the quartertone file locations */
long	MarkData[QTONEPOINTS];	/* Quartertone marker data */
RECT	MarkRect[QTONEPOINTS];	/* Quartertone rectangles */
BOOL	MarkOnImage[QTONEPOINTS];
int	ActiveMark;		/* Active marker */
BOOL	QTmoved[QTONEPOINTS];
} QTONE;
typedef QTONE far *LPQTONE;

/********************************************************************* 
 **** RETOUCH: The retouch control structure
 *********************************************************************/

#define MAX_BRUSH_SIZE 50
#define NO_BRUSH -1
#define PAINT_BRUSH 0
#define SPRAY_BRUSH 1
#define SMEAR_BRUSH 2
#define CLONE_BRUSH 3
#define LIGHTEN_BRUSH 4
#define DARKEN_BRUSH 5
#define SHARPEN_BRUSH 6
#define SMOOTH_BRUSH 7
#define ERASER_BRUSH 8
#define TEXTURE_BRUSH 9
#define DRAW_LINE_BRUSH 10
#define DRAW_RECT_BRUSH 11
#define DRAW_ELLIPSE_BRUSH 12
#define DRAW_FREE_BRUSH 13
#define TOTAL_BRUSHES 14

typedef struct _brushsettings
{
int Pressure;
int BrushShape;
int BrushStyle;
int BrushSize;
int FuzzyPixels;
int Opacity;
FIXED Fadeout;
} BRUSHSETTINGS;

typedef struct _retouch
{
int	Pressure;	/* The retouch pressure: Light, Medium, Heavy */
int	BrushShape;	/* The brush shape: Circle or Square */
int	BrushStyle;	/* The brush style */
int	BrushSize;	/* The brush size in pixels */
//int	BrushLock;	/* The brush directional lock */
int	FloodRange;	/* The range for flood */
BYTE	BrushMask[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
int	FuzzyPixels;	/* The number of fuzzy pixels */
int	Opacity;	/* The transpoarency percentage value */ 
FIXED	Fadeout;	/* The Fadeout value */
BOOL	SyncBrushes; /* The flag for synchronizing brush settings */
FNAME   CustomBrushName;
} RETOUCH;

/********************************************************************* 
 **** EDIT:
 *********************************************************************/

typedef struct _edit
{
BOOL	Distortable;
BOOL	SmartSize;
FIXED	Width;
FIXED	Height;
WORD	ScaleX;
WORD	ScaleY;
WORD	Resolution;
int	Angle;
RECT	Crop;
//BOOL	PasteBlend;
//int	BlendPressure;
//int	PasteQuality;
WORD	CropResolution;
//int	Opacity;
int	Depth;
} EDIT;

#define CROP_WIDTH  (Edit.Crop.right-Edit.Crop.left+1)
#define CROP_HEIGHT (Edit.Crop.bottom-Edit.Crop.top+1)

/********************************************************************* 
 **** SAVE:
 *********************************************************************/

typedef struct _save
{
int	Compressed;
} SAVE;

/********************************************************************* 
 **** PAGE:
 *********************************************************************/

typedef struct _page
{
int	Type;		// IDC_PRINT_COLOR,GRAY,BLACKSEPS,COLORSEPS
int	SepFlags;	// C, Y, M, K
BOOL	ScatterPrint;
BOOL	Centered;
FIXED	OffsetX;
FIXED	OffsetY;
BYTE	UCRlut[256];
BYTE	Dotlut[256];
} PAGE;
typedef PAGE far *LPPAGE;

/********************************************************************* 
 **** STYLE:
 *********************************************************************/

typedef struct _style
{
int	Type;		// RGB, CYMK, MONO
BOOL	DoHalftoning;
BOOL	DoToneRepro;
BOOL	DoCalibration;
BOOL	DoInkCorrect;
BOOL	DoBlackAndUCR;
BOOL	TrimMarks;
BOOL	RegMarks;
BOOL	Labels;
BOOL	StepScale;
BOOL	Negative;
BOOL	EmulsionDown;
int	DotShape;
int	Hilight;
int	Shadow;
FIXED	ScreenRuling[4];
FIXED	ScreenAngle[4];
FNAME	CalName;
MAP	CalMap[4];
FNAME	BGName;
MAP	BGMap;
int	UCR;
int	MfromR;
int	YfromG;
int	CfromB;
BYTE	Unused[200];
} STYLE;
typedef STYLE far *LPSTYLE;

#define RGB_SEPS 1
#define CMYK_SEPS 2
#define C_SEP 4
#define M_SEP 8
#define Y_SEP 16
#define K_SEP 32


/********************************************************************* 
 **** MASKS:
 *********************************************************************/

typedef struct _masks
{
int	Type;
int	IsOutside;
int	AutoSearchDepth;
int	AutoMinEdge;
BOOL	ConstrainRectAspect;
FIXED	ConstrainRectWidth;
FIXED	ConstrainRectHeight;
BOOL	ConstrainCircleAspect;
FIXED	ConstrainCircleWidth;
FIXED	ConstrainCircleHeight;
int	WandRange;
RECT	RepairRect;	// A display rectangle for repairing the display
BOOL	PasteBlend;
int	BlendPressure;
int	PasteQuality;
int	Opacity;
int	VignetteType;
int	VigRepeatCount;
BOOL	VigSoftTransition;
BOOL	TransformImage;
BOOL	RemoveImage;
int	PointerFunction;
BOOL	AutoMask;
BOOL	Rotate;
} MASKS;

/********************************************************************* 
 **** SCAN:
 *********************************************************************/

typedef struct _scans
{
BOOL	LockAspect;
BOOL	SavePreview;
LPFRAME lpScanFrame;	/* The Prescan bitmap frame */
BOOL	DoCalibration;
MAP	CalMap[4];
} SCANS;

/*********************************************************************
 **** COLORMASK: The frame buffer structure
 *********************************************************************/
 
#define NUM_COLORMASKS 8

typedef struct _colormask
{
int  MaskShield;
BOOL Mask;
RGBS MaskColor[NUM_COLORMASKS];
int  MaskRange[NUM_COLORMASKS];
BYTE RMask[256];
BYTE GMask[256];
BYTE BMask[256];
BOOL Include;
BOOL Off;
int ShieldMethod;
int ProbeMethod;
} COLORMASK;

/********************************************************************* 
 **** TEXT:
 *********************************************************************/

#define MAX_FONTS 10
#define MAX_FONT_SIZES 20
#define MAX_TEXT_WEIGHT 100

typedef struct _text
{
BOOL	Italic;
BOOL	Underline;
BOOL	Strikeout;
int 	Weight;
STRING	TextString;
STRING	FontName;
char	FontList[MAX_FONTS][32];
int	FontCount;
int	nFont;
int	SizeList[MAX_FONT_SIZES];
int	SizeCount;
int	Size;
BYTE	PitchAndFamily[MAX_FONTS];
BYTE	CharSet[MAX_FONTS];
} TEXT;

/********************************************************************* 
 **** TEXTURE:
 *********************************************************************/

typedef struct _texture
{
BOOL	fHorzFlip;
BOOL	fVertFlip;
STRING	Name;
} TEXTURE;

/********************************************************************* 
 **** PALETTE:
 *********************************************************************/

#define MAX_USER_COLORS 128

// Changing this will cause the palette files to become invalid
typedef struct _palett
{
RGBS	RGB[MAX_USER_COLORS];
RGBS	ActiveRGB;
RGBS	AlternateRGB;
RGBS	Dummy;
} PALETTE;

/* If this is the main routine, declare the space for the structures */
/* Any other file will automatically have them declared as externals */

#ifdef MAIN
	#define DECLARE ;
#else
	#define DECLARE extern
#endif

//extern BYTE Lut100to255[];
extern BYTE Lut255to100[];
extern BYTE ShiftUp2Lut[];
extern BYTE ToMaxi[32];

DECLARE	WINDOW	Window;
DECLARE	CONTROL	Control;
DECLARE	NAMES	Names;
DECLARE	MAP	MasterMap;
DECLARE	MAP	RedMap;
DECLARE	MAP	GreenMap;
DECLARE	MAP	BlueMap;
DECLARE	MAP	SatMap;
DECLARE	MAP	HueMap;
DECLARE COLORMASK ColorMask;
DECLARE	RETOUCH	Retouch;
DECLARE	EDIT	Edit;
DECLARE	SAVE	Save;
DECLARE	PAGE	Page;
DECLARE	STYLE	Printer;
DECLARE	MASKS	Mask;
DECLARE	SCANS	Scans;
DECLARE	TEXT	Text;
DECLARE TEXTURE Texture;
DECLARE LPTR	LineBuffer[4];
DECLARE	QTONE	Qtone;
DECLARE BYTE	ToMaxi[32];
DECLARE PALETTE	Palette;
DECLARE LPIMAGE lpImage;
DECLARE BLT	BltScreen;
DECLARE BRUSHSETTINGS FAR *RetouchBrushes;
DECLARE int	RetouchBrushCurrent;
