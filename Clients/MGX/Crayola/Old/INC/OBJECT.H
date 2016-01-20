//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "framelib.h"
#include "pixmap.h"
#include "mask.h"

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
	RT_GDI,			// needs GDI to render
	} RENDER_TYPE;

typedef enum 		// storage permanence of object
	{
	ST_PERMANENT,	// Object is permanent
	ST_TEMPORARY,	// Object is temporary
	ST_ALL			// Special type for passing to routines
	} STORAGE_TYPE;

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
	MM_TEXTURIZE
	} MERGE_MODE;

typedef enum 		// Editing Target
	{
	ET_OBJECT,
	ET_SELOBJECTS,
	} EDIT_TARGET;

typedef ULONG UNDO_TYPE;

#define	UT_DATA				0x00000001L
#define	UT_ALPHA			0x00000002L
#define	UT_NEWDATA			0x00000004L
#define	UT_NEWALPHA			0x00000008L
#define	UT_OBJECTRECT		0x00000010L
#define	UT_RESOLUTION		0x00000020L
#define	UT_CREATEOBJECTS	0x00000040L		// special base case
#define	UT_DELETEOBJECTS	0x00000080L		// special base case
#define	UT_CREATEMASK		0x00000100L		// special base case
#define	UT_DELETEMASK		0x00000200L		// special base case
#define	UT_SELECTION		0x00000400L		// special base case
#define	UT_MOVEOBJECTS		0x00000800L		// special base case

#define UT_DATAUNDO		(UT_DATA|UT_NEWDATA|UT_OBJECTRECT|UT_RESOLUTION|UT_CREATEOBJECTS|UT_DELETEOBJECTS|UT_SELECTION|UT_MOVEOBJECTS)
#define UT_ALPHAUNDO	(UT_ALPHA|UT_NEWALPHA|UT_CREATEMASK|UT_DELETEMASK)

// Constants for ObjectDataID field of _primobject:
#define OBJECT_DATA_NONE              0x0000
#define OBJECT_DATA_STOCK_ANIMATION   0x0001

#define IS_STOCK_ANIMATION(lp)                             \
    ((lp)->ObjectDataID == OBJECT_DATA_STOCK_ANIMATION)

typedef struct _primobject
	{
	struct _primobject far* lpNext;
	struct _primobject far* lpPrev;
	OBJECT_TYPE		ObjectType;
	RENDER_TYPE		RenderType;
	STORAGE_TYPE	StorageType;
	RECT			rObject;
	BYTE			Opacity;
	MERGE_MODE		MergeMode;
	BOOL			fSelected;
	BOOL			fChanged;
	BOOL			fDeleted;
    BOOL            fHidden;
	WORD			wGroupID;
    WORD            ObjectDataID;
    DWORD           dwObjectData;
	} PRIMOBJECT, FAR* LPPRIMOBJECT;

typedef struct _imobject
	{
//	PRIMOBJECT;						// Generic object data
// This struct had to be copied here because of the stupid C++ compiler.
	struct _primobject far* lpNext;
	struct _primobject far* lpPrev;
	OBJECT_TYPE		ObjectType;
	RENDER_TYPE		RenderType;
	STORAGE_TYPE	StorageType;
	RECT			rObject;
	BYTE			Opacity;
	MERGE_MODE		MergeMode;
	BOOL			fSelected;
	BOOL			fChanged;
	BOOL			fDeleted;
    BOOL            fHidden;
	WORD			wGroupID;
    WORD            ObjectDataID;
    DWORD           dwObjectData;

// new stuff begins here.
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
	} OBJECT, FAR* LPOBJECT;

typedef struct _alphaobject
	{
//	PRIMOBJECT;						// Generic object data
// This struct had to be copied here because of the stupid C++ compiler.
	struct _primobject far* lpNext;
	struct _primobject far* lpPrev;
	OBJECT_TYPE		ObjectType;
	RENDER_TYPE		RenderType;
	STORAGE_TYPE	StorageType;
	RECT			rObject;
	BYTE			Opacity;
	MERGE_MODE		MergeMode;
	BOOL			fSelected;
	BOOL			fChanged;
	BOOL			fDeleted;
    BOOL            fHidden;
	WORD			wGroupID;
    WORD            ObjectDataID;
    DWORD           dwObjectData;

// new stuff begins here.
	LPMASK	lpAlpha;
	} ALPHAOBJECT, FAR* LPALPHAOBJECT;

typedef struct _objectlist
	{
	LPPRIMOBJECT	lpHead;
	LPPRIMOBJECT	lpTail;
	} OBJECTLIST, FAR* LPOBJECTLIST;

#endif // _OBJECT_H_
