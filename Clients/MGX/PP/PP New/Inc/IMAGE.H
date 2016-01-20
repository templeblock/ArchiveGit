//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "superblt.h"
#include "object.h"
#include "cmdlist.h"
#include "suminfo.h" 

#define GWL_IMAGEPTR    12
#define GWL_DISPLAYPTR  8

//typedef void (FAR *LPDISPLAYHOOK)(HWND hWnd, HDC hDC, LPRECT lpRect);
//typedef void (FAR *LPIMAGEHOOK)(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc, LPTR lpDst, LPTR lpMsk, LPMASK lpMask);
typedef void (FAR *LPMASKHOOK)(int dx,  FRMTYPEINFO TypeInfo,	LPTR lpLine, LPTR lpMask, BOOL MaskExist);

/*********************************************************************
 *** HookHandler providing call back mechanism within classes and access
 *** to instance this pointers
 *********************************************************************/
class CHookHandler
{
public:

    CHookHandler () {};

    // DISPLAY HOOK CAPS
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect) = 0;
    // IMAGE HOOK CAPS
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) = 0;
protected:
private:
};

/********************************************************************* 
 **** DISPLAY: The display structure
 *********************************************************************/
typedef struct display_struct {
RECT    DispRect;       /* The display rectangle */
RECT    FileRect;       /* The file rectangle */
LFIXED  DispRate;       /* The ratio of display pixels to file pixels */
LFIXED  FileRate;       /* The ratio of file pixels to display pixels */
int     ViewPercentage; /* The viewing percentage */
int     xDiva;          /* x-start position of dither */
int     yDiva;          /* y-start position of dither */
RECT    LastWindowRect; /* The last display rectangle */
RECT    LastFileRect;   /* The last file rectangle */
LFIXED  LastFileRate;   /* The last ratio of file pixels to display pixels */
int     LastViewPercentage; /* The last viewing percentage */
CHookHandler   *lpImageHook;    /* Image hook proc */
CHookHandler   *lpDisplayHook; /* display hook floating graphics */
RECT    UpdateRect;
RECT    WindowRect;
BOOL    ResizeType;
BOOL    HasRulers;
BOOL    bOptimized;     /* Use CPO display */
RECT    ZoomWndRect;    // pos and size of zoom wnd for restoration.
RECT    PaintRect;
BOOL    WindowPosChanged;
int     iScrollMin[2/*h&v*/];   // the true min & max of the actual scroll bars
int     iScrollMax[2/*h&v*/];
int     iScrollOutMin[2/*h&v*/]; // the virtual min and max scroll values (as seen by the app)
int     iScrollOutMax[2/*h&v*/];
LFIXED  ScrollRate[2/*h&v*/];   // the conversion factor for the scroll bars 
LPMASKHOOK lpMaskHook;          /* Mask hook proc */
} DISPLAY;
typedef DISPLAY FAR *LPDISPLAY;

#define ONIMAGE( lpDisplay, x, y ) \
    ( (x) >= (lpDisplay)->DispRect.left && (x) <= (lpDisplay)->DispRect.right && \
      (y) >= (lpDisplay)->DispRect.top && (y) <= (lpDisplay)->DispRect.bottom )
#define ONIMAGE1( lpDisplay, x, y ) \
    ( (x) > (lpDisplay)->DispRect.left && (x) < (lpDisplay)->DispRect.right && \
      (y) > (lpDisplay)->DispRect.top && (y) < (lpDisplay)->DispRect.bottom )

#include "framelib.h"

typedef enum        // current view mode of image
    {
    VM_NORMAL,      // normal viewing of image
    VM_SLIME        // view with alpha channel slimed
    } VIEW_MODE;

#define IMG_DOCUMENT        0x0001
#define IMG_BRUSH           0x0002
#define IMG_CLIPBOARD       0x0003
#define IMG_SPLITR          0x0004
#define IMG_SPLITG          0x0005
#define IMG_SPLITB          0x0006
#define IMG_SPLITC          0x0007
#define IMG_SPLITM          0x0008
#define IMG_SPLITY          0x0009
#define IMG_SPLITK          0x000A
#define IMG_SPLITH          0x000B
#define IMG_SPLITS          0x000C
#define IMG_SPLITL          0x000D
#define IMG_DOCPIECE        0x000E
#define IMG_AVIDOC          0x000F
#define IMG_LOWRES          0x0010

#define IMG_NOSPLIT         0x0000
#define IMG_SPLITRGB        0x0100
#define IMG_SPLITCMY        0x0200
#define IMG_SPLITCMYK       0x0300
#define IMG_SPLITHSL        0x0400

/********************************************************************* 
 **** IMAGE: The image structure
 *********************************************************************/

class CImage
{
public:
	// FILE - image.cpp (basic image functions)
	// Constructor
	CImage(	ITEMID idDataType,
			LPTSTR lpFileName = NULL,
			LPCMDLIST lpCommandList = NULL,
			ITEMID idFileType = 0,
			int DocType = IMG_DOCUMENT,
			LPTSTR lpImageName = NULL);

	// Destructor
	~CImage();

	// Function to add image data to the CImage by adding a LPFRAME
	BOOL 		Create(LPFRAME lpFrame); 
	// Function to add image data to the CImage by adding a list of LPOBJECT's
	BOOL 		Create(LPOBJECT lpBase);

    // Function to Get the Root Compound Storage associated with each Image
    LPSTORAGE GetRootCompoundStorage(BOOL *pbTempStg=NULL);
    BOOL WriteSummaryInfo();

	// get the base object for editing
	LPOBJECT   	GetBase() { return (LPOBJECT)ObjList.lpHead; }
	// get size, depth, datatype, and resolution info 
	void 		GetInfo(LPINT lpXSize = NULL,
						LPINT lpYSize = NULL,
                    	LPINT lpDepth = NULL,
						LPFRMDATATYPE lpType = NULL,
						LPINT lpResolution = NULL);
	// get FRMTYPEINFO
	void 		GetTypeInfo(LPFRMTYPEINFO lpTypeInfo);
	// get the base object edit frame
	LPFRAME 	GetBaseEditFrame();
	// get the base object undo frame
	LPFRAME 	GetBaseUndoFrame();
	// get the image mask
	LPMASK 		GetMask();
	// get or create the image mask
	LPMASK 		GetMaskEx(BOOL fOn, BOOL fNoUndo, LPINT bCreated, LPRECT lpUpdateRect);
	// set a new image mask
	void 		SetMask(LPMASK lpMask);
	// get base object of what we are displaying
	LPOBJECT 	GetDisplayObject();
	// get the base object used for sliming data
	LPOBJECT 	GetSlimeObject();
	// get current view mode
	VIEW_MODE 	GetViewMode() { return m_ViewMode; }
	// Get rectangle of all objects in lpObject group
 	void 		GetGroupRect(LPOBJECT lpObject, LPRECT lpRect);
	// get the bounding rectangle for the image mask
	BOOL 		GetMaskRect(LPRECT lpRect);
	// get the bounding rectangle for the image
	void 		GetImageRect(LPRECT lpRect);
	// determine whether image thread is busy
	BOOL 		IsBusy();
	// get the bounding rectangle of all selected objects
	BOOL 		GetSelObjectRect( LPRECT lpRect, BOOL bExcludeBase );
	// get the bounding rect of all objects excluding the base
	BOOL 		GetFloatingObjectsRect( LPRECT lpRect );
	// adjust the grouping of selected objects
	void 		GroupSelObjects(BOOL fGroup);
	// return grouped state of selected objects
	BOOL 		IfSelObjectsGrouped();
	// should the marquee be displayed for this image
	BOOL 		DisplayMarquees();
	// get object to use the mask marquee
	LPOBJECT 	GetMaskMarqueeObject();
	// get object to use for object marquee
	LPOBJECT 	GetObjectMarqueeObject();
	// get mask to use for mask marquee
	LPMASK 		GetMarqueeMask();
	// get update area for a mask change
	BOOL 		GetMaskUpdateRect(BOOL fNewMask, BOOL fInvert, LPRECT lpRect);
	// get undo/redo state of image
	BOOL 		GetUndoState();
	// get dirty flag for image
	int 		GetDirtyState();
	// get changed state of image
	BOOL 		Changed();
	// set changed state of image
	void 		SetChanged(BOOL fChanged);
	// delete all objects for an image starting with passed in object
	void 		DeleteObjects(LPOBJECT lpObject, BOOL bOnlyIfSelected);
	// get rid of deleted objects
	void 		PurgeObjects();
	// delete 1 objects
	void 		DeleteObject(LPOBJECT lpObject );
	// get state of mask edit mode
	BOOL 		InMaskEditMode();
	// set view mode for image
	void 		SetViewMode(VIEW_MODE ViewMode);
	// add an object to the image from a frame
	LPOBJECT 	AddFrameObject(LPFRAME lpFrame, LPMASK lpAlpha, LPRECT lpRect);
	// add a new object to the image
	void 		AddNewObject(LPOBJECT lpObject, BOOL fSetObjRes = TRUE);
	// delete all objects with temporary flag
	void 		DeleteTempObjects();
	// make all temporary objects permanent
	void 		KeepFrameTempObjects();
	// get next available id for a new group
	WORD 		GetNextGroupID();
	// determine whether image is sizable
	BOOL 		IsSizable() { return !fNoSize; }

	// FILE - imglist.cpp (functions that work with object lists)
 	// see if there are multiple objects ( >1 )
	BOOL 		MultipleObjects();
	// get the total number of objects including the base
	int 		CountObjects();
	// get number of selected objects
	int 		CountSelObjects(LPOBJECT lpObject = NULL);
	// select an object
	void 		ChangeSelection(LPOBJECT lpObject, BOOL fSelect, BOOL fExtendedSel);
	// select an object based on a point or rectangle
	BOOL 		SelObject(LPOBJECT lpObject, LPRECT lpRect,
						BOOL fExtendedSel, BOOL fExcludeBase);
	// the the next selected object in the object list
	LPOBJECT 	GetSelObject(LPOBJECT lpObject = NULL, BOOL fBottomUp = TRUE);
	// get the next object in the object list
	LPOBJECT 	GetNextObject(LPOBJECT lpObject,
                        	BOOL fBottomUp = TRUE,
							BOOL fLoop = FALSE,
							BOOL fGetDeletedObjects = FALSE);
	// invert the current selection
	void 		InvertSelection();
	// get selected state of an object
	BOOL 		IsSelectedObject(LPOBJECT lpObject);
	// find the selected object at a point
	LPOBJECT 	FindSelObject(int x, int y, BOOL fBottomUp = TRUE);
	// find the object at a point
	LPOBJECT 	FindObject(int x, int y, BOOL fBottomUp, BOOL fExcludeBase);
	// adjust the layering of selected objects
	void 		AdjustLayerSelObj(BOOL fToTop, BOOL fOneLevel);
	// adjust the layer of one object with respect to another
	void 		AdjustLayerObject(LPOBJECT lpObject,
                        	LPOBJECT lpToObject, BOOL fAfter);

	// FILE - imgclip.cpp (clipboard related functions)
	// write area out for clipboard use with PP4
	BOOL 		WritePP4ClipOut(LPTR lpFileName, LPTR lpMaskName,
								LPRECT lpRect, int DataType );
	// write area out for clipboard use
	BOOL 		WriteClipOut(LPTSTR lpFileName, int DataType ,ITEMID Type, BOOL bUpdateThumb);
	// create an object based on clipboard
	LPOBJECT 	CreateClipOutObject(BOOL CopyObj);
	// create a special mask object for the mask transformer
	LPOBJECT 	CreateMaskObject(LPRECT lpRect);
	// create a new object based on rectangle or mask rectangle
	LPOBJECT 	CreateCutoutObject(LPRECT lpRect);
  
	// FILE - imgfunc.cpp (routines that perform functions/commands)
	// combine objects to base
	BOOL 		CombineObjects(BOOL fNoUndo, BOOL fCreateMask,
                        	BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect);
	// combine selected objects to base
	BOOL 		CombineSelObjects(BOOL fNoUndo, BOOL fCreateMask,
                        	BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect);
	// duplicate all selected objects
	BOOL 		DuplicateSelObjects();
	// turn on or off mask edit mode
	BOOL 		SetupMaskEditMode();
	// crop the image
	BOOL 		Crop( LPRECT lpCropRect );

	// FILE - imgedit.cpp (routines for handling undo for an image)
	// initializes undo for an image - always call before editing an image
	BOOL EditInit( EDIT_TARGET Target, UNDO_TYPE UndoType, LPOBJECT lpTargetObject);
	// apply edits to all objects (i.e. - gets rid of undos)
	void EditApplyAll( BOOL fApplyData, BOOL fApplyAlpha );
	// does an undo or redo
	void EditUndo( BOOL fEditUndo, BOOL fMaskUndo,
					LPUPDATE_TYPE lpUpdateType, LPRECT lpUpdateRect);
	// does a mask undo or redo
	void EditMaskUndo();
	// recover undo for selected objects (called when a operation fails)
	BOOL EditRecoverSelObj( BOOL fEditUndo, BOOL fMaskUndo);
	// recover undo for an object (called when a operation fails)
	BOOL EditRecoverObj( LPOBJECT lpObject,
					BOOL fEditUndo, BOOL fMaskUndo);
	// called when an object has been successfully edited (in-place)
	void EditedObject(LPOBJECT lpObject, int iDirty, LPRECT lpEditRect);
	// called when an object has been successfully edited (new frame)
	void EditedObjectFrame(LPOBJECT lpObject, int iDirty,
					LPRECT lpEditRect,
					LPFRAME lpDataFrame, LPFRAME lpAlphaFrame); 
	// handles the apply of an object (gets rid of undo)
	void ObjEditApply(LPOBJECT lpObject,
		   		BOOL fApplyData, BOOL fApplyAlpha,
				BOOL fInitFrame);
	// handles the undo of an object
	void ObjEditUndo(LPOBJECT lpObject, BOOL fUndoData, BOOL fUndoAlpha);

	// FILE - imgpaint.cpp (routines for drawing an image)
	// paint the image
	BOOL 		Paint(HWND hWnd, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid = NULL);
	// setup for PaintRaster, if you call PaintRaster explicitly
	// for performance, you must call PaintSetup first.
	BOOL 		PaintSetup(LPDISPLAY lpDisplay, BOOL fDoInterrupt);
	// paint raster data
	BOOL 		PaintRaster(LPDISPLAY lpDisplay, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid = NULL);

private:
	// FILE - image.cpp
	BOOL 		Create();

	// FILE - imglist.cpp
	void 		AdjustGroupSelection(BOOL fSelect);
	BOOL 		SelectObjects(LPOBJECT lpStartObject, LPRECT lpRect,
                			BOOL fSelect, BOOL fBottomUp, BOOL fExcludeBase);
	
	// FILE - imgpaint.cpp
	BOOL 		PaintAlloc(int FileWidth, int DispWidth, int depth, int sDepth);
	// paint pluggable display hook - used for tools
	BOOL 		PaintDisplayHook(HWND hWnd, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid);
	// slime a line of data - for ruby mode
	void 		SlimeLine(LPMASKHOOK lpMaskHook, LPOBJECT lpObject,
                        int x, int y, int dx,
                        LPTR lpLine, LPTR lpSlime, int sDepth);
	// check for interruptable redraw abort
	BOOL 		InterruptableAbort();

	// FILE - imgedit.cpp
	// inits undo for an object
	BOOL ObjEditInit(LPOBJECT lpObject, UNDO_TYPE UndoType);
	// frees up memory associated with the undo of an object
	void ObjEditFreeUndo( LPOBJECT lpObject, BOOL fFreeData, BOOL fFreeAlpha );
	// returns whether the undo is discardable (manual apply only)
	BOOL ObjEditUndoDiscardable( LPOBJECT lpObject,
							BOOL fCheckData, BOOL fCheckAlpha );
	// checks to see if the undo is discardable - prompts user if necessary
	BOOL ObjEditDiscardUndoCheck( LPOBJECT lpObject,
					BOOL fCheckData, BOOL fCheckAlpha );

public: // someday to be private
	BOOL        fOwnedByClient; // Does a client app own this image's data?
	BOOL        fRelease;       // Document released flag
	FNAME       CurFile;        /* Name of current picture in the window */
	FNAME       ImageName;      /* Name of the image */
	int         npix, nlin;     /* Number of high res pixels in image */
	ITEMID      FileType;       /* TIFF (IDN_TIFF), EPS (IDN_EPS), etc. */
	ITEMID      DataType;       /* Lineart (IDC_SAVELA), Contone (IDC_SAVECT), etc. */
	ITEMID      UndoDataType;   /* for undoing image's data type */
	int         DocumentType;   /* IMG_DOCUMENT, IMG_BRUSH, IMG_CLIPBOARD */
 	int         Element;        // AVI frame number
	BOOL        Compressed;     /* Was the file read in as compressed */
	BOOL        fChanged;       /* current file modified? */
	BOOL        fUntitled;      /* current file untitled? */
	BOOL        UseObjectMarquee;
	int         MarqueeStatus;  /* The draw status of the marquee */
	OBJECTLIST  ObjList;    // The list of objects for this image
	OBJECTLIST  AltObjList; // The alternate list of objects
 	VIEW_MODE   m_ViewMode;   // Viewing mode for this image
	LPFRAME     lpFastbitsPreview;  /* editing frame */
	RECT        rFastbitsGrid;  /* Undo area in file coordinates */
	HPALETTE    hPal;           /* Color palette for this image */
	int         nPaletteEntries; /* entries in Palette */
	PALETTEENTRY Palette[257]; /* current realized palette */
	BOOL        fNoSize;        /* This image cannot be resized */
	struct _cmdlist FAR *lpCmdList; /* pointer to command list for this image */
	int         iUndoCreateMaskDirty;
	BOOL        fSaveNoSize;
	LPCOLORMAP  lpDummy;     /* palette for this image */
	LPTR        lpPaletteLUT;   /* palette lookup table for this image */
	PALETTE_TYPE PaletteType;  /* display palette type */
	EDIT_TARGET	EditTarget;
	BOOL		fDisableUndos; // disable undo's for this image
	LPVOID		hLastBrush;		// save last used brush 
	EXTNAME		szLastBrushStyle;
	ITEMID		idLastBrush;
	CServerDoc  *pServerDoc; // CServerDoc Type 
	DWORD		dwObjectID;
	LPVOID		m_cmsXform;		// Precision Transform used for display.
	PTINFO		SaveInfo;		// Save for possible recombine.
	LPOBJECT	lpClipObject;	// Private image clipboard object
	BOOL		m_bDoCmsGamma;	// Dst == Mon == no Xform but Cms Gamma
	BOOL		fMaskCreate;	// mask created going into alpha edit?
	BOOL		fDiscardUndo;	// discard undo flag
	PTINFO		PtInfo;			// Save to avoid a reconnect of Monitor Xform.
private:
	LPOBJECT			m_lpDispObject;
	LPOBJECT			m_lpSlimeObject;
	BOOL				m_fDoSample;
	BOOL				m_fDoInterrupt;
	LPFRAME				m_lpDispFrame;
	int					m_depth;
	int					m_sDepth;
	FRMTYPEINFO			m_TypeInfo;
	RECT				m_rDisp;
	RECT				m_rFile;
	int					m_xDiva;
	int					m_yDiva;
	LFIXED				m_xrate;
	LFIXED				m_yrate;
	CFrameTypeConvert 	m_SlimeConvert;
	static LPTR 		m_lpPRDispBuf;
	static LPTR 		m_lpPRFileBuf;
	static LPTR 		m_lpPRSlimeBuf;
	static LPTR			m_lpWideSlimeBuf;
	static long			m_lDispBufSize;
	static long			m_lFileBufSize;
	static long 		m_lSlimeBufSize;
	static long			m_lWideSlimeBufSize;
};
typedef CImage IMAGE;
typedef CImage FAR *LPIMAGE;
typedef LPIMAGE FAR *LPPIMAGE;

#define MARQUEE_OFF         1
#define MARQUEE_CYCLE       64
#define MARQUEE_NOT_READY   128

typedef struct _imagewincreate_struct {
LPIMAGE lpImage;    // the image pointer
LPDISPLAY   lpDisplay;  // the display info pointer
} IMAGEWINCREATE;
typedef IMAGEWINCREATE FAR *LPIMAGEWINCREATE;

#define INFILE(lpImage, x, y ) \
    ( (x) >= 0 && (x) < (lpImage)->npix && (y) >= 0 && (y) < (lpImage)->nlin )
#define INFILE1( lpImage, x, y ) \
    ( (x) > 0 && (x) < (lpImage)->npix-1 && (y) > 0 && (y) < (lpImage)->nlin-1)

LPOBJECT ImgGetBase(LPIMAGE lpImage);
void ImgSetMask(LPIMAGE lpImage, LPMASK lpMask);
LPMASK ImgGetMaskEx(LPIMAGE lpImage, BOOL fOn, BOOL fNoUndo, LPINT bCreated, LPRECT lpUpdateRect);
LPMASK ImgGetMask(LPIMAGE lpImage);
LPFRAME ImgGetBaseEditFrame(LPIMAGE lpImage);
LPFRAME ImgGetBaseUndoFrame(LPIMAGE lpImage);
BOOL ImgSelObject(LPIMAGE lpImage, LPOBJECT lpObject, LPRECT lpRect, BOOL fExtendedSel, BOOL fExcludeBase);
void ImgChangeSelection(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fSelect, BOOL fExtendedSel);
LPOBJECT ImgFindSelObject(LPIMAGE lpImage, int x, int y, BOOL fBottomUp);
LPOBJECT ImgFindObject(LPIMAGE lpImage, int x, int y, BOOL fBottomUp, BOOL fExcludeBase);
LPOBJECT ImgGetSelObject(LPIMAGE lpImage, LPOBJECT lpObject);
BOOL ImgIsSelectedObject(LPIMAGE lpImage, LPOBJECT lpObject);
LPOBJECT ImgGetNextObject(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fBottomUp, BOOL fLoop);
LPOBJECT ImgGetNextObjectEx(LPIMAGE lpImage, LPOBJECT lpObject, BOOL fBottomUp, BOOL fLoop, BOOL fGetDeletedObjects);
BOOL ImgGetSelObjectRect( LPIMAGE lpImage, LPRECT lpRect, BOOL bExcludeBase );
BOOL ImgGetFloatingObjectsRect( LPIMAGE lpImage, LPRECT lpRect );
int ImgCountSelObjects(LPIMAGE lpImage, LPOBJECT lpObject);
int ImgCountObjects(LPIMAGE lpImage);
void ImgAdjustLayerSelObj(LPIMAGE lpImage, BOOL fToTop, BOOL fOneLevel);
void ImgAdjustLayerObject(LPIMAGE lpImage, LPOBJECT lpObject, LPOBJECT lpToObject, BOOL fAfter);
void ImgGroupSelObjects(LPIMAGE lpImage, BOOL fGroup);
BOOL ImgIfSelObjectsGrouped(LPIMAGE lpImage);
BOOL ImgMultipleObjects(LPIMAGE lpImage);
BOOL ImgDisplayMarquees(LPIMAGE lpImage);
LPOBJECT ImgGetMaskMarqueeObject(LPIMAGE lpImage);
LPOBJECT ImgGetObjectMarqueeObject(LPIMAGE lpImage);
LPMASK ImgGetMarqueeMask(LPIMAGE lpImage);
LPOBJECT ImgGetSlimeObject(LPIMAGE lpImage, HWND hWnd);
BOOL ImgGetMaskUpdateRect(LPIMAGE lpImage, BOOL fNewMask, BOOL fInvert, LPRECT lpUpdate);
BOOL ImgAnyUndoFrames(LPIMAGE lpImage);
BOOL ImgGetDirtyObjects(LPIMAGE lpImage);
BOOL ImgGetUndoState(LPIMAGE lpImage);
int ImgGetDirtyState(LPIMAGE lpImage);
BOOL ImgSetupMaskEditMode(LPIMAGE lpImage);
BOOL ImgInMaskEditMode(LPIMAGE lpImage);
void ImgSetViewMode(LPIMAGE lpImage, VIEW_MODE ViewMode);
VIEW_MODE ImgGetViewMode(LPIMAGE lpImage);
LPOBJECT ImgAddFrameObject(LPIMAGE lpImage, LPFRAME lpFrame, LPMASK lpAlpha, LPRECT lpRect);
void ImgDeleteTempObjects(LPIMAGE lpImage);
BOOL ImgKeepFrameTempObjects(LPIMAGE lpImage);
BOOL ImgChanged(LPIMAGE lpImage);
void SetImgChanged(LPIMAGE lpImage, BOOL fChanged);
void ImgAddUndoArea(LPOBJECT lpObject, LPRECT lpUndo);
BOOL ImgGetLine(LPIMAGE lpImage, LPOBJECT lpObject, int x, int y, int dx, LPTR lpOut, CHookHandler* lpImageHook = NULL, BOOL fSelected = FALSE);
BOOL ImgGetObjectLine(LPIMAGE lpImage, LPOBJECT lpBaseObject, int x, int y, int dx, LPTR lpOut, BOOL fSelected, BOOL bClipBaseToMask, LPCOLORINFO lpColor);
BOOL ImgMaskLoad(LPIMAGE lpImage, LPOBJECT lpBaseObject, LPMASK lpMaskIn,
                int x, int y, int dx, LPTR lpMaskBuf, BOOL fSelected, COMBINE_ROP ROP, int xOffset, int yOffset);
LPFRAME ImgCopyFrame(LPIMAGE lpImage, LPOBJECT lpBaseObject, LPRECT lpRect, BOOL bCrop, BOOL fSelected);
LPFRAME ImgCopyMaskedFrame(LPIMAGE lpImage, LPOBJECT lpBaseObject, LPRECT lpRect,BOOL bCrop, BOOL fSelected, BOOL bClipBaseToMask, LPCOLORINFO lpColor);
BOOL ImgGetMaskRect(LPIMAGE lpImage, LPRECT lpRect);
void ImgGetImageRect(LPIMAGE lpImage, LPRECT lpRect);
BOOL ImgCombineObjects(LPIMAGE lpImage, BOOL fNoUndo, BOOL fCreateMask,
                        BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect);
BOOL ImgCombineSelObjects(LPIMAGE lpImage, BOOL fNoUndo, BOOL fCreateMask,
                        BOOL fForceIt, ITEMID idDirty, LPRECT lpUpdateRect);
BOOL ImgCrop( LPIMAGE lpImage, LPRECT lpRect );
void ImgAddNewObject(LPIMAGE lpImage, LPOBJECT lpObject, BOOL bSetObjRes = TRUE);
void ImgDeleteObject(LPIMAGE lpImage, LPOBJECT lpObject);
void ImgDeleteObjects(LPIMAGE lpImage, LPOBJECT lpObject,BOOL bOnlyIfSelected);
void ImgDeleteObjectList(LPOBJECTLIST lpObjList);
void ImgPurgeObjects(LPIMAGE lpImage);
BOOL ImgDuplicateSelObjects(LPIMAGE lpImage);
void ImgInvertSelection(LPIMAGE lpImage);
void ImgGetInfo(LPIMAGE lpImage, LPINT lpXSize, LPINT lpYSize, LPINT lpDepth, LPFRMDATATYPE lpType);
void ImgGetInfoEx(LPIMAGE lpImage, LPINT lpXSize, LPINT lpYSize, LPINT lpDepth, LPFRMDATATYPE lpType, LPINT lpResolution);
void ImgGetTypeInfo(LPIMAGE lpImage, LPFRMTYPEINFO lpTypeInfo);
void ImgGetGroupRect( LPIMAGE lpImage, LPOBJECT lpObject, LPRECT lpRect );

BOOL ImgEditInit( LPIMAGE lpImage, EDIT_TARGET Target, UNDO_TYPE UndoType, LPOBJECT lpObject );
void ImgEditApplyAll( LPIMAGE lpImage, BOOL fInitData, BOOL fInitAlpha );
void ImgEditMaskUndo( LPIMAGE lpImage );
BOOL ImgEditRecoverSelObj( LPIMAGE lpImage, BOOL fEditUndo, BOOL fMaskUndo);
BOOL ImgEditRecoverObj( LPIMAGE lpImage, LPOBJECT lpObject, BOOL fEditUndo, BOOL fMaskUndo);
void ImgEditedObject(LPIMAGE lpImage, LPOBJECT lpObject, int iDirty, LPRECT lpEditRect);
void ImgEditedObjectFrame(LPIMAGE lpImage, LPOBJECT lpObject, int iDirty, LPRECT lpEditRect, LPFRAME lpDataFrame, LPFRAME lpAlphaFrame);
BOOL CheckAbortInPlaceEdit(LPPIXMAP lpPixmap);
LPOBJECT ImgCreateMaskObject(LPIMAGE lpImage,LPRECT lpRect);
LPOBJECT ImgCreateCutoutObject(LPIMAGE lpImage,LPRECT lpRect);
BOOL ImgWriteClipOut(LPIMAGE lpImage, LPTSTR lpFileName, int DataType,ITEMID Type, BOOL bUpdateThumb);
BOOL ImgWritePP4ClipOut(LPIMAGE lpImage, LPTR lpFileName, LPTR lpMaskName,
					LPRECT lpRect, int DataType );
LPOBJECT ImgCreateClipOutObject(LPIMAGE lpImage, BOOL CopyObj = FALSE);

// imgclip.cpp
LPOBJECT ImgCreateClipInObject(LPIMAGE lpImage, LPTSTR lpDataFile, LPTSTR lpMaskFile = NULL, BOOL bDoCMS = FALSE, LPFRMTYPEINFO lpSrcToReturn = NULL, LPFRMTYPEINFO lpSrcToUse = NULL);
void GetUniqueStreamName(LPSTORAGE lpStorage, LPCTSTR lpTemplate, LPCTSTR lpExtension, LPTSTR lpStreamName);
BOOL CopyToStream(LPTSTR lpSrcFile, LPTSTR lpDstStream, LPSTORAGE lpStorage);
BOOL CopyFromStream(LPTSTR lpSrcStream, LPTSTR lpDstFile, LPSTORAGE pRootStorage);
BOOL UpdateClipBits(LPSTORAGE pRootStorage, LPLIST pPacketList, BOOL bSavingCmdList, BOOL fUpdateNamedClipbits);
COleStreamFile *OpenClipStream(LPSTORAGE lpRootStorage, LPTSTR lpStreamName, LPSTORAGE *lppClipStorage);
void CloseClipStream(LPSTORAGE lpStorage, COleStreamFile *pFile);
BOOL IsStorageClipbit(LPTSTR lpFileName);
BOOL UpdateMaskBits(LPSTORAGE pRootStorage, LPLIST pPacketList, BOOL bSavingCmdList, BOOL fUpdateNamedMaskbits);
BOOL IsStorageMaskbit(LPTSTR lpFileName);

/**************************** Inline Functions ****************************/
inline DWORD AFXAPI ReleaseInterface(LPUNKNOWN* lplpUnknown)
{
	ASSERT(lplpUnknown != NULL);
	if (*lplpUnknown != NULL)
	{
		DWORD dwRef = (*lplpUnknown)->Release();
		*lplpUnknown = NULL;
		return dwRef;
	}
	return 0;
}

/**************************************************************************/

#endif // _IMAGE_H_



