/*
// $Header: /PM8/App/GRAFOBJ.H 1     3/03/99 6:05p Gbeddow $
//
// Graphic object definitions.
//
// $Log: /PM8/App/GRAFOBJ.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 7     2/26/99 8:39p Rgrenfel
// Supported the mechanism to get the original object's data path.
// 
// 6     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     8/10/98 3:47p Hforman
// Crop Copyright modifications
// 
// 4     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 3     12/11/97 9:20a Jstoner
// vector to bitmap conversion for photo enhancer
 * 
 * 2     10/28/97 5:40p Dennis
 * Added IsEmpty()
// 
//    Rev 1.0   14 Aug 1997 15:21:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:02   Fred
// Initial revision.
// 
//    Rev 1.7   23 Jun 1997 12:37:10   Fred
// Object properties (color) support
// 
//    Rev 1.6   05 Jun 1997 16:48:24   Jay
// Transparent bitmaps and PostScript.
// 
//    Rev 1.5   09 Apr 1997 17:13:28   Jay
// LockGraphicRecord
// 
//    Rev 1.4   01 Nov 1996 17:11:50   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.3   10 Oct 1996 14:30:40   Jay
// More time stamps
// 
//    Rev 1.2   07 Oct 1996 17:02:36   Jay
// TimeStamp
// 
//    Rev 1.1   20 Sep 1996 13:41:22   Jay
// Support for Temp DIB graphic records.
// 
//    Rev 1.0   14 Mar 1996 13:42:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:16   FRED
// Initial revision.
// 
//    Rev 2.5   09 Oct 1995 15:15:02   JAY
// New direct (fast) bitmap printing
// 
//    Rev 2.4   14 Sep 1995 09:32:18   JAY
// 32-bit changes
// 
//    Rev 2.3   18 Aug 1995 15:05:58   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.2   08 Aug 1995 17:34:14   JAY
// Support for setting the rotate handle on a dynamic basis.
// 
//    Rev 2.1   15 Feb 1995 17:03:14   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:38   JAY
// Initial revision.
// 
//    Rev 1.5   22 Jul 1994 16:28:40   JAY
// Added some missing access methods.
// 
//    Rev 1.4   01 Jun 1994 16:35:28   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.3   17 Feb 1994 09:25:14   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.2   13 Jan 1994 17:01:12   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:24   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:56   JAY
//  
*/

#ifndef __GRAFOBJ_H__
#define __GRAFOBJ_H__

#include "pmwdoc.h"

class ImageDriver;

/*
// Error diffusion information.
*/

typedef struct
{
	int y;							/* source y line */
	int x0;							/* dest x0 */
	int x1;							/* dest x1 */
	int planes;					/* dest planes */
	int lineno;					/* how many lines have been generated? */
	SHORT errors[1];				/* actual error values; 2-d array for planes */
} ERROR_INFO, far *ERROR_INFO_PTR;

#define GRAPHIC_FLAG_coloring_book	1
#define GRAPHIC_FLAG_smoothed			2

struct GraphicObjectRecord
{
	SHORT			element_count;	/* number of elements in this layout. */

	SHORT			operation;		/* mode for the data */

	DB_RECORD_NUMBER	g_record; /* record of graphic data */

	COLOR			color;
	COLOR			bcolor;

	FLAGS			graphic_flags;
	SHORT			render_method;		// RENDER_METHOD
};

/*
// The class for saving a GraphicObject's state.
*/

class GraphicObjectState : public RectPageObjectState
{
public:
	virtual ~GraphicObjectState();
	GraphicObjectRecord	m_GRecord;
};

/*
// A graphic object.
*/

FARCLASS GraphicObject : public RectPageObject
{
	INHERIT(GraphicObject, RectPageObject)
private:
	GraphicObjectRecord grecord;

	SHORT				error_array_size;			/* number of elements in error array */
	ERROR_INFO_PTR	far *error_info;	/* one for each element of the layout. */
	CTimeStamp m_DrawnTimeStamp;

	BOOL ImageChanged(GraphicObjectState* pState);

protected:
/*
// Handle any creation data.
// Use by this and derived classes.
*/

	ERRORCODE handle_creation_data(VOIDPTR creation_data);

/*
// Private constructor.
// Call the create method.
*/

	GraphicObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

/*
// Show this graphic as a pattern.
*/

	UpdateStatePtr draw_pattern_image(LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate, BYTE *pattern = NULL);

/*
// Draw the actual source image for this graphic.
*/

	UpdateStatePtr draw_source_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate, PBOX* pExtent = NULL);

/*
// Draw the in-memory image for this graphic.
*/

	UpdateStatePtr draw_memory_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate);

/*
// Draw the image (alternate source) for this graphic.
// Mostly used by Photo CD.
// (Sorry for the name confusion.)
*/

	UpdateStatePtr draw_graphic_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate);

/*
// Draw the DIB image for this graphic.
*/

	UpdateStatePtr draw_dib_image(GraphicPtr graphic, LPRECT clip, RedisplayContextPtr rc, UpdateStatePtr ustate);

public:

/*
// The destructor for a graphic object.
*/

	~GraphicObject();

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Update an object into a redisplay context.
*/

	UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

	ERRORCODE read_data(StorageDevicePtr device);

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

	ERRORCODE write_data(StorageDevicePtr device);


/*
// Convert the vector data for this object and write as BMP data.
*/

	ERRORCODE WriteVectorAsBMP(StorageDevice* pDevice, CPmwDoc* pDoc);
	
/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

	ST_MAN_SIZE size_data(StorageDevicePtr device);

/*
// Set the graphic record number for this object.
*/

	VOID set_graphic_record(DB_RECORD_NUMBER n);

/*
// Get the graphic record number for this object.
*/

	DB_RECORD_NUMBER get_graphic_record(VOID) { return grecord.g_record; }

/*
// Lock the graphic record for this object.
// The caller must do a release().
*/

	GraphicRecord* LockGraphicRecord(void);

/*
// Get the render method for this object.
*/

	RENDER_METHOD get_render_method(VOID) { return (RENDER_METHOD)grecord.render_method; }

/*
// Set the render method for this object.
*/

	VOID set_render_method(RENDER_METHOD render_method)
		{ grecord.render_method = render_method; }

/*
// Update the rotate handle status depending on the graphic type.
*/

	void UpdateRotateHandle(void);

/*
// Allocate the error array.
*/

	BOOL allocate_error_array(SHORT new_count = -1);

/*
// Get the element count.
*/

	SHORT get_element_count(void)
		{ return grecord.element_count; }

/*
// Get a graphic's "white" color.
*/

	COLOR get_color(VOID)	{ return grecord.color; }

/*
// Set a graphic's "white" color.
*/

	VOID set_color(COLOR color)	{ grecord.color = color; }

/*
// Get a graphic's "black" color.
*/

	COLOR get_bcolor(VOID)	{ return grecord.bcolor; }

/*
// Set a graphic's "black" color.
*/

	VOID set_bcolor(COLOR bcolor)	{ grecord.bcolor = bcolor; }

/*
// Get a graphic's operation.
*/

	SHORT get_operation(VOID)	{ return grecord.operation; }

/*
// Get the graphic flags.
*/

	FLAGS get_graphic_flags(VOID)
 		{ return grecord.graphic_flags; }

/*
// Set the graphic flags.
*/

	VOID set_graphic_flags(FLAGS flags)
 		{ grecord.graphic_flags = flags; }

/*
// Add graphic flags.
*/

	VOID add_graphic_flags(FLAGS flags)
 		{ grecord.graphic_flags |= flags; }

/*
// Remove graphic flags.
*/

	VOID remove_graphic_flags(FLAGS flags)
 		{ grecord.graphic_flags &= ~flags; }

/*
// Get a graphic's error info.
*/

	ERROR_INFO_PTR far *get_error_info(VOID)	{ return error_info; }

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Is this object opaque?
*/

	virtual BOOL is_opaque(RedisplayContextPtr rc);

/*
// Check if object has data?  
*/
   virtual BOOL IsEmpty();

/*
// Assign an object to this.
*/

	virtual ERRORCODE assign(PageObjectRef sobject);

/*
// Get the original dimensions of this graphic.
*/

	virtual BOOL original_dims(PPNT_PTR p);

/*
// Get an object's name.
*/

	virtual BOOL get_name(LPSTR buffer, SHORT size, SHORT max_width);

/*
// Begin an image (Pre-update initialization).
*/

	virtual ERRORCODE begin_image(RedisplayContextPtr rc, PBOX world);

	// Get the object's time stamp.
	virtual CTimeStamp GetTimeStamp(void);
	virtual BOOL UpdateTimeStamp(void);

/*
// Create a state for saving.
// We return a pointer to a new object. This object needs to be freed by
// the caller at some point.
*/

	virtual PageObjectState* CreateState(void);

/*
// Save our state.
*/

	virtual void SaveState(PageObjectState* pState);

/*
// Restore our state.
// The state passed in is NOT deleted by us. It is still the responsibility
// of the caller (or whomever).
*/

	virtual void RestoreState(PageObjectState* pState);

/*
// Get the properties for this object.
*/

	virtual void GetObjectProperties(CObjectProperties& Properties);

/*
// Set the properties for this object.
*/

	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

/*
// Set the flag for direct bitmap printing.
// Currently this is global, so the function is static.
*/

	static void SetDirectBitmapPrinting(BOOL fDirectBitmapPrinting);

/*
// Get the flag for direct bitmap printing.
// This is used for dialog display (i.e. user interface).
*/

	static BOOL GetDirectBitmapPrinting(void);

/*
// Get the direct bitmap printing flag for this object.
// This requires a redisplay context in order to make intelligent
// decisions based on the current output state.
*/

	BOOL DirectBitmapPrint(RedisplayContext* pContext);

//
// Paint a bitmap graphic.
// This primarily exists to handle transparent bitmap graphics on PostScript
// printers.
//

	UpdateStatePtr PaintBitmapImage(LPRECT clip,
					GraphicPtr record, ImageDriver* driver,
					RedisplayContextPtr rc, UpdateStatePtr ustate,
					PBOX* pExtent = NULL);


	// set VisibleRect if we need to crop the copyright off bottom
	ERRORCODE GraphicObject::CropIfNeeded();

	virtual const CString *GetOriginalDataPath();

protected:
	static BOOL m_fDirectBitmapPrinting;
};

extern UpdateStatePtr graphic_error_state(UpdateStatePtr ustate, UPDATE_TYPE type = UPDATE_TYPE_Error, ERRORCODE error = ERRORCODE_None);

#define graphic_nofile_state(ustate) graphic_error_state(ustate, UPDATE_TYPE_Error, ERRORCODE_Open)
#define graphic_terminated_state(ustate) graphic_error_state(ustate, UPDATE_TYPE_Terminated)

#endif					// #ifndef __GRAFOBJ_H__
