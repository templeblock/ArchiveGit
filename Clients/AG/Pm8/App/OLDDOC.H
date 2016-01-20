/*
// $Header: /PM8/App/OLDDOC.H 1     3/03/99 6:08p Gbeddow $
//
// Definitions for old PMG documents.
//
// $Log: /PM8/App/OLDDOC.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:23:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:50   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 14:21:40   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:45:32   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:48   Jay
// Changes in 2.1
// 
//    Rev 2.3   27 Feb 1996 16:44:14   JAY
// 32-bit fix for record (RECTS, POINTS)
// 
//    Rev 2.2   08 Jul 1995 15:13:56   JAY
// Changed some 'Ptr's to '*'.
// 
//    Rev 2.1   07 Jun 1995 13:14:16   JAY
// Added destructor to OldProjectPreview to free string memory (memory leak).
// 
//    Rev 2.0   07 Feb 1995 15:48:46   JAY
// Initial revision.
// 
//    Rev 1.1   06 Oct 1994 13:35:00   JAY
// Margin info is now passed around for card panels.
// 
//    Rev 1.0   16 Dec 1993 16:26:12   JAY
//  
*/

#ifndef __OLDDOC_H__
#define __OLDDOC_H__

#include "oldgraf.h"
#include "calinfo.h"						/* This is the same! */

#define	OLD_MANUAL_LAYOUT		-2

typedef enum
{
	DISPLAY_NOTHING_NOTHING = 0x00,	/* white = nothing, black = nothing */
	DISPLAY_SET_NOTHING,					/* white = set, black = nothing */
	DISPLAY_MIX_NOTHING,					/* white = mix, black = nothing */
	DISPLAY_ERASE_NOTHING,				/* white = erase, black = nothing */
	DISPLAY_INVERT_NOTHING,				/* white = invert, black = nothing */

	DISPLAY_NOTHING_SET = 0x10,		/* white = nothing, black = set */
	DISPLAY_SET_SET,						/* white = set, black = set */
	DISPLAY_MIX_SET,						/* white = mix, black = set */
	DISPLAY_ERASE_SET,					/* white = erase, black = set */
	DISPLAY_INVERT_SET,					/* white = invert, black = set */

	DISPLAY_NOTHING_MIX = 0x20,		/* white = nothing, black = mix */
	DISPLAY_SET_MIX,						/* white = set, black = mix */
	DISPLAY_MIX_MIX,						/* white = mix, black = mix */
	DISPLAY_ERASE_MIX,					/* white = erase, black = mix */
	DISPLAY_INVERT_MIX,					/* white = invert, black = mix */

	DISPLAY_NOTHING_ERASE = 0x30,		/* white = nothing, black = erase */
	DISPLAY_SET_ERASE,					/* white = set, black = erase */
	DISPLAY_MIX_ERASE,					/* white = mix, black = erase */
	DISPLAY_ERASE_ERASE,					/* white = erase, black = erase */
	DISPLAY_INVERT_ERASE,				/* white = invert, black = erase */

	DISPLAY_NOTHING_INVERT = 0x40,	/* white = nothing, black = invert */
	DISPLAY_SET_INVERT,					/* white = set, black = invert */
	DISPLAY_MIX_INVERT,					/* white = mix, black = invert */
	DISPLAY_ERASE_INVERT,				/* white = erase, black = invert */
	DISPLAY_INVERT_INVERT,				/* white = invert, black = invert */

} DISPLAY_OP;

/*
 * THE INODE.
 */

typedef SHORT INODE, FAR *LPINODE;

/*
 * prototype METHODS.
 */

typedef enum
{
	OBJ_GRAPHIC,
	OBJ_FRAME,
	OBJ_PATTERN,
	OBJ_BORDER,
	OBJ_GROUP,
	NUMBER_OF_OLD_TYPES
} OLD_OBJECT_TYPE;

/* Set high bit on object type in document file if the size of the object
// is written with the type.
*/
#define OBJTYPE_FLAG_SavesSize		0x8000

/*
// Flags that an object may have to remember states.
*/

#define	OLD_OBJECT_FLAG_LOCKED			(1<<0)
#define	OLD_OBJECT_FLAG_LANDSCAPE		(1<<1)
#define	OLD_OBJECT_FLAG_XFLIPPED		(1<<2)
#define	OLD_OBJECT_FLAG_YFLIPPED		(1<<3)
#define	OLD_OBJECT_FLAG_NEEDS_CALC		(1<<4)
#define	OLD_OBJECT_FLAG_FLOW_LEFT		(1<<5)
#define	OLD_OBJECT_FLAG_FLOW_RIGHT		(1<<6)
#define	OLD_OBJECT_FLAG_SELECTED		(1<<7)
#define	OLD_OBJECT_FLAG_NEEDS_UPDATE	(1<<8)
#define	OLD_OBJECT_FLAG_NO_MASK			(1<<9)
#define	OLD_OBJECT_FLAG_RESIZE_IGNORE	(1<<10)
#define	OLD_OBJECT_FLAG_CALC_IGNORE	(1<<11)

#define	OLD_OBJECT_FLAG_FLOW			(OLD_OBJECT_FLAG_FLOW_LEFT|OLD_OBJECT_FLAG_FLOW_RIGHT)

/*
// Flags recording an object's selection status.
*/

#define	OLD_SELECT_FLAG_boundary			1				/* bounding box */
#define	OLD_SELECT_FLAG_size_handles		2				/* 8 handles */
#define	OLD_SELECT_FLAG_move_handle		4				/* middle move handle */
#define	OLD_SELECT_FLAG_can_drag			8				/* can drag outline */

#define	OLD_SELECT_FLAG_handles			(OLD_SELECT_FLAG_size_handles|OLD_SELECT_FLAG_move_handle)

typedef class OldObject far * OldObjectPtr;

FARCLASS OldObject
{
public:
	OLD_OBJECT_TYPE	type;
	OldObjectPtr	next;				/* next in list */
	struct
	{
		FLAGS			select_flags;	/* Selection flags */
		PBOX        bound;         /* bounding box in global coords. */
		FLAGS			flags;			/* flags in assorted colors. */
		SHORT			panel;			/* which panel is this object a part of */
		INODE			layout;       	/* inode of layout */
	} record;
/*
// Standard constructor for old object.
*/

	OldObject(OLD_OBJECT_TYPE t) : type(t) {}

/*
// Virtual destructor for derived classes.
*/

	virtual ~OldObject(VOID) {}

/*
// Read data from the device.
*/

	virtual ERRORCODE read(StorageDevicePtr device, SHORT size);
};

typedef OldObjectPtr (*OLD_OBJECT_CREATOR)(OLD_OBJECT_TYPE type);

/*
// A graphic object.
*/

FARCLASS OldGraphicObject : public OldObject
{
protected:
	OldGraphicObject(OLD_OBJECT_TYPE type);
public:
/*
// unique data.
*/

	struct
	{
		SHORT			element_count;	/* number of elements in this layout. */
		SHORT			error_array_size;	/* number of elements in error array */
		ERROR_INFO	far * far *error_info;	/* one for each element of the layout. */

	 	DISPLAY_OP	mode;				/* mode for the data */

		INODE			data;       	/* inode of graphic data */

		COLOR			color;
		COLOR			bcolor;
	} grecord;

/*
// The creator method.
*/

	static OldObjectPtr create(OLD_OBJECT_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device, SHORT size);
};

typedef OldGraphicObject far * OldGraphicObjectPtr;

/*
// The following BORDER object must match the GRAPHIC object for all the fields
// in the GRAPHIC object. BORDER-specific fields start at the end.
*/

FARCLASS OldBorderObject : public OldGraphicObject
{
private:
	OldBorderObject(OLD_OBJECT_TYPE type);
public:
	struct
	{
		PPNT			piece_size[8];				/* Size information for each piece */
		SHORT			scale;						/* The scale for the pieces. */
		SHORT			h_edge_count;				/* pieces on the horiz edges */
		SHORT			v_edge_count;				/* pieces on the vert edges */
	} brecord;
/*
// The creator method.
*/
	static OldObjectPtr create(OLD_OBJECT_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device, SHORT size);
};

typedef OldBorderObject far * OldBorderObjectPtr;

FARCLASS OldFrameObject : public OldObject
{
private:
	OldFrameObject(OLD_OBJECT_TYPE type);
public:
	struct
	{
	   INODE			frame;         /* really just an inode */

		/* This box describes the area occupied by the frame for purposes of
			redisplay.  It may be larger or smaller than the frame "bounds".
			Its coordinates are relative to the frame origin. */
	/* This field is missing for "old" old frame objects. */
		PBOX			refresh_offsets;
	} frecord;
/*
// The creator method.
*/
	static OldObjectPtr create(OLD_OBJECT_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device, SHORT size);
};

typedef OldFrameObject far * OldFrameObjectPtr;

FARCLASS OldPatternObject : public OldObject
{
private:
	OldPatternObject(OLD_OBJECT_TYPE type);
public:
	struct
	{
		DISPLAY_OP	operation;		/* operation for updating */
		SHORT			pattern;			/* which pattern in the PatTable to use */
		COLOR			color;			/* object color */
		COLOR			bcolor;			/* object bcolor */
	} precord;
/*
// The creator method.
*/
	static OldObjectPtr create(OLD_OBJECT_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device, SHORT size);
};

typedef OldPatternObject far * OldPatternObjectPtr;

/*
// A group object.
*/

FARCLASS OldGroupObject : public OldObject
{
private:
	OldGroupObject(OLD_OBJECT_TYPE type);
public:
	~OldGroupObject(VOID);
/*
// unique data
*/
	struct
	{
		OldObjectPtr	list;				/* list of objects in group. */
	} grecord;
/*
// The creator method.
*/
	static OldObjectPtr create(OLD_OBJECT_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device, SHORT size);
};

typedef OldGroupObject far * OldGroupObjectPtr;

/********************** INODE definitions ********************************/

typedef enum
{
	INODE_OLD_Frame,
	INODE_Page,
	INODE_Graphic,
	INODE_FontData,
	INODE_Bitmap,
	INODE_Document,
	INODE_OLD_Paragraph,
	INODE_ObjectLayout,
	INODE_CalendarInfo,
	INODE_ProjectPreview,
	INODE_Frame,
	INODE_Paragraph,
	NUMBER_OF_INODE_TYPES
} INODE_TYPE;

FARCLASS OldRecord
{
public:
	INODE_TYPE type;

/*
// The constructor for the OldRecord.
*/

	OldRecord(INODE_TYPE t) : type(t) {}

/*
// Virtual destructor for derived classes.
*/
	virtual ~OldRecord(VOID) {}

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device) = 0;
};

typedef OldRecord far *OldRecordPtr;

typedef OldRecordPtr (*OLD_RECORD_CREATOR)(INODE_TYPE type);

/*
// Inode states.
*/

#define IS_FREEINODE    0x0001      /* this node is free. */
#define IS_HERE         0x0010      /* is it in memory or on disk ? */
#define IS_LOCKED       0x0020      /* is locked in mem. */
#define IS_PURGEABLE    0x0100      /* is purgeable. */

#define set_inode_flag(t, f)     (t)->flags |=  (f)
#define clear_inode_flag(t, f)   (t)->flags &= ~(f)

#define is_free(t)               ( (t)->flags & IS_FREEINODE )
#define is_here(t)               ( (t)->flags & IS_HERE )
#define is_locked(t)             ( (t)->flags & IS_LOCKED )
#define is_purgeable(t)          ( (t)->flags & IS_PURGEABLE )

typedef struct
{
	FLAGS				flags;	/* flags. */
	INODE_TYPE		type;		/* type of node. */
	OldRecordPtr	data;		/* data in memory. */
	LONG				addr;		/* position on disk. */
	LONG				size;		/* size of data on disk. */
} _INODE, FAR *_INODE_PTR;

typedef _INODE_PTR INODE_TABLE, far *INODE_TABLE_PTR;

/*
// A secondary table entry (8 bytes).
*/

typedef struct
{
	LONG addr;				/* position on disk. 0 if unused. */
	INODE_TABLE table;	/* table in memory. NULL if not. */
} SECONDARY, FAR* SECONDARY_PTR;

/*
// A free area structure (8 bytes).
*/

typedef struct
{
   LONG start;
   LONG end;
} FREEAREA, FAR *FREEAREA_PTR;

/*
// Table counts.
// In order for the current code to work correctly, the INODE_BLOCK constant
// must be a power of 2 (see INODE_BLOCK_MASK!).
// NOTE: The INODE_BLOCK* defines are used by WINPORT.H, so beware.
*/

#define NUMBER_OF_SECONDARIES		128				/* 256 */
#define NUMBER_OF_FREEAREAS		100
#define INODE_BLOCK 					64					/* 256 */
#define INODE_BLOCK_MASK			(INODE_BLOCK-1)
#define INODE_BLOCK_SIZE			(INODE_BLOCK*sizeof(_INODE))

#define PRIMARY_TABLE( i ) 		((i) / INODE_BLOCK)
#define TABLE_ENTRY( i ) 			((i) & INODE_BLOCK_MASK)

typedef struct
{
	SHORT		count;	/* count of array. */
	SHORT		block;	/* block size of array. */
	VOIDPTR	data;		/* data for array. */
} ARRAY, FAR *ARRAY_PTR;

#include "oldtext.h"			/* Other text stuff.

/*
//	Page.
*/

FARCLASS OldPage : public OldRecord
{
private:
	OldPage(INODE_TYPE t);
public:

	~OldPage(VOID);

	struct
	{
		OldObjectPtr list;				/* items are held here when this is not the current page */
	} record;

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldPage far *OldPagePtr;

/*
//	Frame.
*/

FARCLASS OldFrame : public OldRecord
{
private:
	OldFrame(INODE_TYPE t);
public:

	struct
	{
		FLAGS flags;
	   INODE paragraph;			/* inode of the paragraph for this frame. */
	   PBOX  bound;				/* bounding box of frame. */
		ARRAY	line;					/* Array of lines (LINE) */
		PPNT stretch_dims;		/* original frame dims when stretching turned on. */
	} record;

/*
// The destructor.
*/
	~OldFrame(VOID);

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldFrame far *OldFramePtr;

/*
//	Paragraph.
*/

FARCLASS OldParagraph : public OldRecord
{
private:
	OldParagraph(INODE_TYPE t);
public:

	PARAGRAPH_RECORD record;
/*
// The destructor.
*/
	~OldParagraph(VOID);

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldParagraph far *OldParagraphPtr;

/*
// Graphic.
*/

FARCLASS OldGraphic : public OldRecord
{
private:
	OldGraphic(INODE_TYPE t);
public:

	OLD_GRAPHIC_DATA record;

	DB_RECORD_NUMBER converted_record;

	CString m_csName;
	CString m_csFileName;

/*
// The destructor.
*/
	~OldGraphic(VOID);

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldGraphic far *OldGraphicPtr;

/*
//	Font data.
*/

FARCLASS OldFontData : public OldRecord
{
private:
	OldFontData(INODE_TYPE t);
public:

	struct
	{
		LPSTR		OldName;						/* name of this font. (not filename.) */
	} record;

	CString m_csName;

	DB_RECORD_NUMBER converted_font;

/*
// The destructor.
*/
	~OldFontData(VOID);

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldFontData far *OldFontDataPtr;

typedef struct old_layout_element far *OLD_LAYOUT_ELEMENT_PTR;
typedef struct old_layout_element
{
	OLD_LAYOUT_ELEMENT_PTR	next;			/* old next pointer */
	FLAGS							flags;
	RECTS							where;
	POINTS						offset;		/* for repeated elements */
} OLD_LAYOUT_ELEMENT;

/*
//	Object layout.
*/

FARCLASS OldObjectLayout : public OldRecord
{
private:
	OldObjectLayout(INODE_TYPE t);
public:

	struct
	{
		LPSTR OldName;								/* the name of this layout. */
		USHORT reference_count;
		OLD_LAYOUT_ELEMENT_PTR elements;	/* layout elements (defined in layout.h) */
	} record;

	CString m_csName;

	DB_RECORD_NUMBER converted_record;

/*
// The destructor.
*/

	~OldObjectLayout(VOID);

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldObjectLayout far *OldObjectLayoutPtr;

/*
//	Calendar info.
*/

FARCLASS OldCalendarInfo : public OldRecord
{
private:
	OldCalendarInfo(INODE_TYPE t);
public:

	CALENDAR_INFO record;

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldCalendarInfo far *OldCalendarInfoPtr;

/*
// Project preview.
*/

FARCLASS OldProjectPreview : public OldRecord
{
private:
	OldProjectPreview(INODE_TYPE t);
public:
	OldProjectPreview();
	~OldProjectPreview();

	struct
	{
		BOOL		valid;					/* Is this preview valid? */
		SHORT		width;					/* Absolute width of bitmap. */
		SHORT		height;					/* Absolute height of bitmap. */
		SHORT		x_resolution;			/* X resolution of bitmap. */
		SHORT		y_resolution;			/* Y resolution of bitmap. */
		LPSTR		OldDescription;		/* A project description. */
		VOIDPTR	bitmap;					/* A pointer to the bitmap. */
	} record;

	CString m_csDescription;

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldProjectPreview far *OldProjectPreviewPtr;

/*
// Document.
*/

FARCLASS OldDocument : public OldRecord
{
private:
	OldDocument(INODE_TYPE t);
public:

	struct
	{
		SHORT			project_type;	/* enumerated in pmdefs.h */
	   PBOX        pagearea;      /* actual page area. */
		INODE			info_inode;		/* inode of the information for this project */
										/* 0 or -1 means none */

		SHORT			macro_count;

	   ORIENTATION	orientation;

	   PCOORD      xsize,ysize;   /* page size in page coordinates. */

	   OldObjectPtr	CurrentList;
	   OldObjectPtr	SelectedList;

	   INODE       CurrentPage;	/* inode of the current page. */

		LPSTR			filename;		/* original filename. */
		SHORT			modified;		/* modified flag for this document. */

	   ARRAY			page;				/* page inodes. */
	   ARRAY			font;				/* font inodes available in this document. */
	   ARRAY			layout;			/* layout inodes available in this document. */
	   ARRAY			graphic;			/* graphic inodes available in this document. */
	} record;

/*
// The creator.
*/
	static OldRecordPtr create(INODE_TYPE type);

/*
// Read data from the device.
*/
	virtual ERRORCODE read(StorageDevicePtr device);
};

typedef OldDocument far *OldDocumentPtr;

/*
// file types for PrintMaster Plus.
*/

enum FILE_TYPES
{
	DOCUMENT_FILE,
	TEMPLATE_FILE
};

/* Old document. */

FARCLASS OldPMGDocument
{
private:
	StorageDevicePtr device;
	PROJECT_TYPE project_type;
/*
// All the secondaries.
*/
	SECONDARY_PTR secondary;

	INODE document_inode;			/* Located document inode. */
	OldDocumentPtr document;		/* Locked document object. */

	VOID fix_primary(INODE_TABLE table);
	ERRORCODE read_primary(SHORT primary);
	ERRORCODE lock_table(SHORT primary, INODE_TABLE_PTR table);
	VOID free_primary(SHORT primary);

/*
// Conversion routines.
*/

	ERRORCODE convert_preview(PMGDatabasePtr new_database);

	ERRORCODE convert_rectangle_object(OldObjectPtr object, RectPageObjectPtr new_object);
	ERRORCODE convert_object(OldObjectPtr object, ObjectList* new_list);
	ERRORCODE convert_list(OldObjectPtr old_list, ObjectList* list);
	ERRORCODE convert_layout(INODE old_layout, SHORT type, DB_RECORD_NUMBER *layout, PMGDatabasePtr database);
	ERRORCODE convert_frame(INODE old_frame, DB_RECORD_NUMBER *frame, OldFrameObjectPtr f, PMGDatabasePtr database);
	ERRORCODE convert_graphic(INODE old_graphic, DB_RECORD_NUMBER *graphic, PMGDatabasePtr database);
	ERRORCODE convert_font(INODE old_font, DB_RECORD_NUMBER *font, PMGDatabasePtr database);
	ERRORCODE convert_paragraph(INODE old_paragraph, DB_RECORD_NUMBER *pargraph, PMGDatabasePtr database, DB_RECORD_NUMBER lFirstFrame);
	ERRORCODE convert_style(PMD_STYLE_PTR old_style, TextStyle& style);

/*
// Calendar support.
*/

	ERRORCODE init_calendar(PMGDatabasePtr database, ObjectList* list);
	ERRORCODE get_frame_style(INODE f_inode, CTextStyle& style);
	CalendarObjectPtr calobj;
	GroupObjectPtr skeleton_group, text_group, graphic_group;
	BOOL got_label_style;
	BOOL got_number_style;

	BOOL got_calendar;

/*
// Card support.
*/

	ERRORCODE init_card(PMGDatabasePtr database, ObjectList* list, ProjectInfo *info, LPCRECT margins);
	GroupObjectPtr card_group[4];

public:

	OldRecordPtr get_record(INODE which, LPERRORCODE perror = NULL);

	OldPMGDocument(StorageDevicePtr dev);
	~OldPMGDocument();
	ERRORCODE init(VOID);
	ERRORCODE convert(PMGDatabasePtr database, ProjectInfo *info, LPCRECT margins);
};

#endif						/* #ifndef __OLDDOC_H__ */
