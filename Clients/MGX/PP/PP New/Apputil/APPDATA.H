/*®FD1¯®PL1¯®TP0¯®BT0¯®RM240¯*/
#ifndef _APPDATA_H_
#define _APPDATA_H_

#include "mgxframe.h"

typedef struct _listlink
	{
	struct _listnode FAR *lpNext;
	struct _listnode FAR *lpPrev;
	} LISTLINK, FAR *LPLISTLINK;

typedef struct _listnode
	{
	LISTLINK	Link;
	} LISTNODE, FAR *LPLISTNODE;

typedef struct _list
	{
	LISTLINK	Link;
	} LIST, FAR *LPLIST;

typedef struct _pixmap
	{
	struct _frame far *EditFrame;
	struct _frame far *UndoFrame;
	BOOL    		UndoFailed;		// cannot create undo for this image
	LPTR			EditFlags;		// Dirty Lines buffer
	RECT			UndoRect;		// Undo area in file coordinates
	BOOL			fNewFrame;		// image has new edit frame, undo different
	} PIXMAP, FAR* LPPIXMAP;

typedef struct _mask
	{
	PIXMAP	Pixmap;					// The mask bitmap data
	RECT	rMask;					// Mask rectangle
	LPTR	mbuffer;				// Mask buffer 
	BOOL	bInvert;				// Invert mask flag
	RECT	rUndoMask;				// The undo mask rectangle
	BOOL	bInvertState;			// Current state of invert
	int		mbufsize;				// size of mbuffer
	} MASK, FAR* LPMASK;
typedef MASK ALPHA;
typedef LPMASK LPALPHA;

typedef struct _fontdesc
{
	TCHAR	fdFaceName[LF_FACESIZE];
	BYTE	fdPitchAndFamily;
	BYTE	fdCharSet;
	int	FontType;
} FONTDESC, FAR *LPFONTDESC;

typedef struct _fontlist
{
	BOOL		fCountFonts;
	int		FontCount;
	FONTDESC	FontDesc[1];
} FONTLIST, FAR *LPFONTLIST;

typedef struct _fontid
{
	FONTDESC	FontDesc;
	int		Size;
	BOOL		Italic;
	BOOL		Underline;
	BOOL		Strikeout;
	int 		Weight;
	int 		Angle;
} FONTID, FAR *LPFONTID;

class CTextObject
{
public:
	CTextObject(CString &Text,
				BOOL Dirty,
				int AntiAlias,
				LPCOLORINFO lpColor,
				int Justification,
				LPFONTID lpFontID,
				LPPOINT Pos,
				int iRes);
	CTextObject(const CTextObject &Src);
	~CTextObject();
public:
	CString        lpText;
	BOOL           Dirty;
	int				iBaseRes;	// for DestPos
	POINT			DestPos;
	int				AntiAlias;
	COLORINFO		Color;
	int		      Justification;
	FONTID         FontID;
};

typedef enum		// kind of object
	{
	OT_FRAME,		// Frame object
	OT_VECTOR,		// Vector object
	OT_ALPHA		// Alpha object
	} OBJECT_TYPE;

typedef enum		// how object can render
	{
	RT_FRAME,		// can render using FRAME technology
	RT_RASTER,		// can render into a raster bitmap
	RT_GDI			// needs GDI to render
	} RENDER_TYPE;

typedef enum 		// storage permanence of object
	{
	ST_PERMANENT,	// Object is permanent
	ST_TEMPORARY,	// Object is temporary
	ST_ALL			// Special type for passing to routines
	} STORAGE_TYPE;

typedef enum 		// Editing Target
	{
	ET_OBJECT,
	ET_SELOBJECTS,
	ET_ALLOBJECTS
	} EDIT_TARGET;


typedef enum 		// Merge Mode
	{
	MM_NORMAL,
	MM_ADDITIVE,
	MM_SUBTRACTIVE,
	MM_IFLIGHTER,
	MM_IFDARKER,
	MM_HUESAT,
	MM_HUE,
	MM_SAT,
	MM_LUM,
	MM_RED,
	MM_GREEN,
	MM_BLUE,
	MM_CYAN,
	MM_MAGENTA,
	MM_YELLOW,
	MM_BLACK,
	MM_SCREEN,
	MM_MULTIPLY,
	MM_DIFFERENCE,
	MM_TEXTURIZE,
	MM_INVERT
	} MERGE_MODE;

typedef ULONG UNDO_TYPE;

#define OBJECT_NAME_LEN 32

typedef struct _imobject
	{
	struct _imobject far* lpNext;
	struct _imobject far* lpPrev;
	OBJECT_TYPE		ObjectType;
	RENDER_TYPE		RenderType;
	STORAGE_TYPE	StorageType;
	RECT			rObject;
	BYTE			Opacity;
	MERGE_MODE		MergeMode;
	BOOL			fSelected;
	BOOL			fDeleted;
	WORD			wGroupID;

	LPALPHA			lpAlpha;		// Object alpha pointer
	PIXMAP			Pixmap;			// Object Pixmap - frame undo

	UNDO_TYPE		InitUndoType;	// Undo Type when ImgEditInit() called
	UNDO_TYPE		DataUndoType;	// Type of undo operation
	BOOL			DataUndoNotRedo;// Toggle between undo and redo
	int				DataDirty;		// Data dirty flag
	UNDO_TYPE		AlphaUndoType;	// Type of undo operation
	BOOL			AlphaUndoNotRedo;// Toggle between undo and redo
	int				AlphaDirty;		// Alpha dirty flag
	BOOL 			fBothDirty;		// Flag if data and alpha were edited together

	LPALPHA			lpUndoAlpha;	// Pointer to undo object alpha
	RECT			rUndoObject;	// Object rectangle undo
	int				UndoResolution;	// Undo object resolution
	BOOL			fUndoSelected;	// Undo object selection
	BOOL			fUndoDeleted;	// Undo of deleted state

	BOOL			fTransform; 	// This object should be transformed
	RECT			rTransform;		// Carry this around for transform
	BOOL			fPasteIntoMask;	// Special paste into mask case
	BOOL			fUndoFailed;	// Undo creation failed
	LPCOLORMAP		UndoColorMap;	// save of colormap
	BOOL			bLocked;		// Obj Locked flag
	CTextObject		*lpText;        // valid only if text object
	CTextObject		*lpUndoText;    // pointer to Undo object text
	DWORD			dwObjectID;	// uniquely identifying object id
	TCHAR			szObjectName[OBJECT_NAME_LEN];
	BOOL			bUndoLocked;		// Obj Locked flag
	WORD			UndoGroupID;
	} OBJECT, FAR* LPOBJECT;

typedef LPOBJECT FAR *LPPOBJECT;

#define LPPRIMOBJECT LPOBJECT
#define PRIMOBJECT OBJECT

typedef struct _objectlist
	{
	LPOBJECT	lpHead;
	LPOBJECT	lpTail;
	} OBJECTLIST, FAR* LPOBJECTLIST;

#endif // _APPDATA_H_

