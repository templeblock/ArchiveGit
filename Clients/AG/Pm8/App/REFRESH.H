/*
// $Header: /PM8/App/REFRESH.H 1     3/03/99 6:11p Gbeddow $
//
// Basic refresh definitions.
//
// $Log: /PM8/App/REFRESH.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     2/27/98 2:47p Jayn
// "Draw as bitmaps"
// 
// 2     1/06/98 6:49p Hforman
// add extern standard_check_interrupt()
// 
//    Rev 1.0   14 Aug 1997 15:25:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:44   Fred
// Initial revision.
// 
//    Rev 1.2   26 Mar 1997 14:52:06   Jay
// Interruptable preview
// 
//    Rev 1.1   07 Feb 1997 09:00:58   Fred
// Attempted to document refresh types
// 
//    Rev 1.0   14 Mar 1996 13:45:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:18   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:56   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:26:08   JAY
//  
*/


// This is an attempt at some notes documenting the meaning of the various
// refresh types and their use in PMG 4.0:
//
// Background:
//
//		PMG uses a structure called REFRESH_EXTENT (PMGOBJ.H) to keep track of an
//		area that needs to be refreshed.
//
//		Each view has its own array of refresh extents and these are processed
//		at idle time. CPmwApp::OnIdle() calls CPmwDoc::idle_refresh() for each
//		open document. CPmwDoc::idle_refresh() calls CPmwView::refresh() (REFRESH.CPP)
//		for each of the views attached to the document. CPmwView::refresh() sets up
//		its redisplay context (rc) for the drawing to its surface and calls
//		CPmwDoc::draw_objects() for each refresh extent in its refresh extent
//		array. CPmwDoc::draw_objects() calls the static function DrawObjects()
//		(PMWDOC.CPP) to draw the various objects in the document in order to
//		refresh the current extent. And thus the screen or other surface is once
//		again made whole.
//
//		Each refresh extent has a refresh type. This type is used to optimize certain
//		drawing cases.
//
//		In PMG 4.0, the only object that uses a refresh type other than REFRESH_ALL is
//		the text frame object which uses REFRESH_ERASE and REFRESH_REDRAW.
//
//		Basically, REFRESH_ERASE and REFRESH_REDRAW can be understood as "hints" to
//		DrawObjects() and CFrameObject::update(). In certain cases, these hints allow
//		the refresh code to do less work, or use offscreen bitmaps. In other cases,
//		DrawObjects() will change the refresh type to REFRESH_ALL if the optimizations
//		are not possible.
//
//		Each refresh extent also has a pointer to a PMGPageObject called "my_object".
//		This pointer is used only when refreshing text frame objects and points to
//		the text frame object that generated the refresh extent. Examination of the code
//		suggests that the my_object is set to NULL if the refresh type is REFRESH_ALL.
//
//		One more thing, check out CPmwView::refresh_array_compact() (REFRESH.CPP) to
//		see how overlapping refresh extents with different refresh types are handled.
//
//		Finally, it seems that the only object which looks at the refresh type is
//		CFrameObject (text frame). What probably started out a "general solution" for
//		refresh optimization is, in PMG 4.0, specific to text boxes. DrawObjects()
//		switches from REFRESH_ERASE and REFRESH_REDRAW to REFRESH_ALL for any object
//		other than the one which requested the REFRESH_ERASE and REFRESH_REDRAW type.
//		
// REFRESH_ALL
//
//		This is the generic refresh type. It is used is all "normal" refresh situations
//		and always for objects other than text frames.
//
//		It basically means the entire refresh extent needs to be rebuilt from scratch.
//		So, DrawObjects() erases the background of the refresh extents and draws the objects
//		that overlap the refresh extent into the redisplay context. The update() function
//		of the objects do not need to concern themselves with anything but drawing themselves
//		into the specified area since DrawObjects() has taken care of drawing the background
//		for the object.
//
// RFERESH_ERASE
//
//		This refresh type is a hint to DrawObjects() that the (text frame) object which
//		generated the extent (my_object) only wants the area erased (the OBJECT WILL DO
//		THE ERASING), it does not have anything to draw in the area. DrawObjects() will
//		call the update function for the object which can then optimize the erase.
//		CFrameObject::update() optimizes the erase by skipping the call to
//		CFrameObject::UpdateFrame() which traverses the line and word arrays.
//		If DrawObjects() finds an object underneath the object requesting REFRESH_ERASE,
//		then the refresh type is changed to REFRESH_ALL.
//
//		Noteworthy special case: DrawObjects() looks for the top level opaque object
//		that fully contains the refresh extent. A text box that is being edited and has
//		a background bitmap is flagged as an opaque object. Since the text box is opaque,
//		it is typically the first object to be drawn and thus CFrameObject::update(),
//		will be called with with REFRESH_ERASE which will erase using the edit
//		background bitmap.
//
// REFRESH_DRAW
//
//		This refresh type is not used in PMG 4.0. It's barely used in PMG 2.0 and 3.0.
//		The best guess is that it meant "draw the object without erasing". Presumably
//		there was some case in some ancient version of PrintMaster for which REFRESH_DRAW
//		was useful. Anyway, it not used anymore and the current code does not support it.
//
//		*** DO NOT USE REFRESH_DRAW ***
//
// REFRESH_REDRAW
//
//		This refresh type is a hint to DrawObjects() that the (text frame) object which
//		generated the extent (my_object) wants the area erased and then redrawn, but the
//		OBJECT WILL DO THE ERASING AND DRAWING. This allows the object to perform erase
//		and drawing optimizations assuming there are no objects beneath it. If DrawObjects()
//		discovers an object beneath the object requesting REFRESH_REDRAW, then the refresh
//		type is changed to REFRESH_ALL.
//
//		Note: If the object requesting REFRESH_REDRAW is a text box that is being edited
//		and has a background bitmap, then it may not be true there are no objects behind
//		the text box, but, instead, that all objects beneath the text box are accounted
//		for in the background bitmap.

#ifndef __REFRESH_H__
#define __REFRESH_H__

#include "array.h"			// for Array definitions

struct REFRESH_EXTENT;
struct PBOX;

typedef enum
{
	REFRESH_ALL,			// full redraw -- DrawObjects will draw background, update() will draw foreground
	REFRESH_ERASE,			// object will erase, no redraw necessary
	REFRESH_DRAW,			// no longer used in PMG 4.0
	REFRESH_REDRAW			// object will erase and redraw
} REFRESH_TYPE;

/////////////////////////////////////////////////////////////////////////////
// CRefreshArray

class CRefreshArray : public Array
{
public:
	CRefreshArray();
	virtual ~CRefreshArray();

	int NumberOfExtents(void)
		{ return count(); }

	void Add(REFRESH_EXTENT* pExtent, BOOL fCompact = TRUE);

	REFRESH_EXTENT* GetExtent(int nExtent)
		{ return (REFRESH_EXTENT*)get_element(nExtent); }

	void DeleteExtent(int nExtent)
		{ delete_element(nExtent); }

	void FreeAllExtents(void);

	void Compact(void);

protected:
	BOOL do_erase_draw_breakup(PBOX* box1, PBOX* box2,
										REFRESH_TYPE type1, REFRESH_TYPE type2,
										REFRESH_EXTENT* overlap_extent);

	ERRORCODE AddExtent(REFRESH_EXTENT* pExtent)
		{ return add_element(pExtent); }
};

extern BOOL standard_check_interrupt(LPVOID data);

#endif					// #ifndef __REFRESH_H__
