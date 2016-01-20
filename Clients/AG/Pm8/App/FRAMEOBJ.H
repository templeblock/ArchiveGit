/*
// $Header: /PM8/App/FRAMEOBJ.H 1     3/03/99 6:05p Gbeddow $
//
// Text frame object definitions.
//
// $Log: /PM8/App/FRAMEOBJ.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 26    1/21/99 12:02p Cschendel
// Added a ETextWrapGutter type to the framerec for serialization and the
// the creation struct. Old files will have not text wrap gutter but newer
// files will have the default setting of the creation struct wich is
// GUTTER_medium
// 
// 25    1/14/99 9:29a Cschende
// made the array used for counting numbered list paragraphs a member,
// added the function InitListCounter to initialize it correctly when
// bulleted lists flow across linked frames
// 
// 24    12/10/98 4:10p Cschende
// Added bullet, numbered list and multilevel list support in text
// 
// 23    7/01/98 4:18p Johno
// Added RemoveHyperlink(); checks for disabled hyperlinks in
// UpdateFrame().
// 
// 22    6/04/98 7:52p Jayn
// Table fill down and right plus a lot of "fit to text" stuff
// 
// 21    6/03/98 1:46p Jayn
// Table Autoformat changes
// 
// 20    5/11/98 12:04p Johno
// Changed m_lFrameFlags to m_lExtraFlags to avoid confusion (there was
// already something similarly named).
// 
// 19    5/07/98 5:53p Johno
// Added TestFrameFlag()
// 
// 18    5/07/98 4:50p Johno
// Added m_lFrameFlags to CFrameObjectRecord, and methods.
// Added remarks to ConvertToHTML().
// 
// 17    4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 16    4/29/98 7:19p Johno
// Moved out CHTMLCompliantInfo to CHTMLObject
// 
// 15    4/28/98 11:29a Johno
// Changes for rotated and flipped image maps
// 
// 14    4/24/98 2:41p Johno
// GetURLList() handles wrapped text as multiple image maps
// 
// 13    4/23/98 3:23p Johno
// HTML changes for rotated text with links (as graphic with polygon image
// map)
// 
// 12    4/17/98 6:23p Johno
// Simple CHTMLCompliantInfo class
// 
// 11    4/17/98 3:56p Johno
// Changes for HTML design checker
// 
// 10    4/03/98 4:26p Johno
// Compile update
// 
// 9     4/01/98 12:09p Johno
// Added GetURLList; changed TRACE to JTRACE; text formatting
// 
// 8     3/31/98 10:12a Johno
// Preliminary GetURLList
// 
// 7     3/25/98 6:31p Johno
// Changes for text links
// 
// 6     2/10/98 1:20p Dennis
// Added paragraph alignment logic for HTML generation
// 
// 5     1/21/98 8:38a Jstoner
// made DrawFrameOutline virtual
// 
// 4     12/18/97 3:38p Dennis
// Added state logic for compliance check.
// 
// 3     11/14/97 11:27a Dennis
// HTML conversion support
 * 
 * 2     10/28/97 5:41p Dennis
// 
//    Rev 1.0   14 Aug 1997 15:21:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:58   Fred
// Initial revision.
// 
//    Rev 1.36   15 Jul 1997 17:35:26   Fred
// Saves and restores effects in object state
// 
//    Rev 1.35   30 Jun 1997 11:55:18   Jay
// New stretching logic.
// 
//    Rev 1.34   28 May 1997 11:05:52   Fred
// Frame background effects
// 
//    Rev 1.33   14 Apr 1997 15:42:46   Jay
// Text box editing changes
// 
//    Rev 1.32   17 Mar 1997 14:48:22   Jay
// Warp shape palette and support.
// 
//    Rev 1.31   12 Mar 1997 14:54:38   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.30   05 Mar 1997 14:37:42   Fred
// More text object stuff
// 
//    Rev 1.29   26 Feb 1997 10:57:34   Jay
// Cropping; GetClipRgn() changes
// 
//    Rev 1.28   25 Feb 1997 12:57:22   Jay
// Cropping and extensions
// 
//    Rev 1.27   25 Feb 1997 10:55:14   Fred
//  
// 
//    Rev 1.26   07 Feb 1997 08:44:56   Fred
// Start of new text drawing pipeline
// 
//    Rev 1.25   29 Jan 1997 09:12:20   Jay
// Removed CreateSelectPen. Added ObjectProperties routines.
// 
//    Rev 1.24   28 Jan 1997 16:56:46   Fred
// Start of component drawing
// 
//    Rev 1.23   08 Jan 1997 15:21:20   Fred
// More text box changes
// 
//    Rev 1.22   06 Jan 1997 10:15:22   Fred
// Start of new text object
// 
//    Rev 1.21   02 Oct 1996 08:32:38   Jay
// SetParagraphStyle
// 
//    Rev 1.20   26 Jul 1996 11:38:52   Jay
// Page stuff
// 
//    Rev 1.19   22 Jul 1996 18:07:42   Jay
// Tab code now works with tabs in columns.
// 
//    Rev 1.18   12 Jul 1996 16:47:46   Jay
// Text wrap
// 
//    Rev 1.17   10 Jul 1996 10:23:50   Jay
// Text states no longer allocated.
// 
//    Rev 1.16   08 Jul 1996 17:51:30   Jay
// Linked frames, etc.
// 
//    Rev 1.15   28 Jun 1996 17:31:58   Jay
// OnPage() routine.
// 
//    Rev 1.14   28 Jun 1996 16:18:12   Jay
//  
// 
//    Rev 1.13   27 Jun 1996 17:20:36   Jay
//  
// 
//    Rev 1.12   27 Jun 1996 13:27:36   Jay
//  
// 
//    Rev 1.11   26 Jun 1996 14:21:36   Jay
//  
// 
//    Rev 1.10   26 Jun 1996 09:06:22   Jay
//  
// 
//    Rev 1.9   24 Jun 1996 18:06:10   Jay
//  
// 
//    Rev 1.8   21 Jun 1996 15:43:34   Jay
// Stretch flow tweaks.
// 
//    Rev 1.7   21 Jun 1996 14:45:52   Jay
//  
// 
//    Rev 1.6   21 Jun 1996 14:30:22   Jay
// Stretch text, etc.
// 
//    Rev 1.5   20 Jun 1996 14:59:48   Jay
//  
// 
//    Rev 1.4   19 Jun 1996 13:45:46   Jay
//  
// 
//    Rev 1.3   13 Jun 1996 15:53:00   Jay
//  
// 
//    Rev 1.2   04 Jun 1996 17:38:56   Jay
//  
// 
//    Rev 1.1   24 May 1996 09:26:52   Jay
// New text code.
// 
//    Rev 1.0   14 Mar 1996 13:47:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:30   FRED
// Initial revision.
// 
//    Rev 2.8   09 Aug 1995 13:09:44   JAY
// Virtual override of MacrosInUse
// 
//    Rev 2.7   17 May 1995 14:24:54   FRED
// Warp text objec refresh bounds.
// 
// 
//    Rev 2.6   25 Apr 1995 15:23:10   JAY
// Added margins to the frame object.
// 
//    Rev 2.5   25 Apr 1995 08:51:56   JAY
// Added y-stretch capabilities.
// 
//    Rev 2.4   20 Apr 1995 16:15:50   FRED
// Warp Text Stuff
// 
//    Rev 2.3   19 Apr 1995 12:31:52   JAY
//  
// 
//    Rev 2.2   29 Mar 1995 08:58:56   JAY
// Lots of changes
// 
//    Rev 2.1   15 Feb 1995 17:03:12   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:36   JAY
// Initial revision.
// 
//    Rev 1.9   01 Jun 1994 16:35:30   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.8   15 Mar 1994 13:22:48   JAY
// Support for edit background.
// 
//    Rev 1.7   25 Feb 1994 13:34:44   MACDUFF
// update_frame() takes offscreen flag
// 
//    Rev 1.6   24 Feb 1994 14:44:18   JAY
// Some changes from good friend Macduff.
// If text is black and white, will draw using a monochrome bitmap.
// Will attempt to draw the toggle marks in the offscreen bitmap instead of to
// the screen when possible.
// 
//    Rev 1.5   17 Feb 1994 09:26:24   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// Added a m_editing variable, and set_editing() in preparation for special
// frame editing code.
// 
//    Rev 1.4   14 Feb 1994 09:25:06   MACDUFF
// Added modification count to DatabaseRecord.
// 
//    Rev 1.3   11 Feb 1994 14:50:00   MACDUFF
// Removed is_black_or_white param to update_frame
// 
//    Rev 1.2   28 Jan 1994 17:11:34   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:26   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:26:02   JAY
//  
*/

#ifndef __FRAMEOBJ_H__
#define __FRAMEOBJ_H__

#include "text.h"
#include "style.h"
#include <afxtempl.h>   // for CList

class CFrameRecord;
class CWarpField;
class CTextDisplayList;
class CHTMLTag;
class CHTMLTagParagraph;
class CImageMapElementArray;
class CHTMLCompliantInfo;

struct FRAMEOBJ_CREATE_STRUCT
{
// Be very sure this gets set correctly.
	FRAMEOBJ_CREATE_STRUCT()
	{
		memset(this, 0, sizeof(*this));
		m_Fill.SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);
		m_Outline.SetType(OutlineFormat::OutlineNone);
		m_Outline.SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);
		m_Shadow.SetType(ShadowFormat::ShadowNone);
		m_Shadow.SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);
		m_eGutter = GUTTER_medium;
	}

	PBOX bound;
	ALIGN_TYPE alignment;
	VERT_ALIGN_TYPE vert_alignment;
	DWORD flags;
	TextStyle* pStyle;		// No longer used.
	PCOORD LeftMargin;
	PCOORD TopMargin;
	PCOORD RightMargin;
	PCOORD BottomMargin;

	FillFormatV1 m_Fill;
	OutlineFormat m_Outline;
	ShadowFormat m_Shadow;

	ETextWrapGutter m_eGutter;
};

typedef FRAMEOBJ_CREATE_STRUCT far *FRAMEOBJ_CREATE_STRUCT_PTR;

/*
// The update state structure for frames.
*/

class FrameUpdateState : public UpdateState
{
public:
	BOOL				clearing;
	C_INDEX			c_index;
	W_INDEX			w_index;
	L_INDEX			l_index;
	PCOORD			x;
	int 				nParagraphModificationCount;
};

typedef FrameUpdateState far *FrameUpdateStatePtr;

// WARNING: Old frame object record. The new one is CFrameObjectRecord.
struct FrameObjectRecord
{
	DB_RECORD_NUMBER	f_record; /* record of frame data */

// Added with v 2.0.
	PCOORD LeftMargin;
	PCOORD TopMargin;
	PCOORD RightMargin;
	PCOORD BottomMargin;
};

/*
// The class for saving a FrameObject's state.
*/

// WARNING: Old frame object state. The new one is CFrameObjectState.
class FrameObjectState : public RectPageObjectState
{
public:
	FrameObjectRecord	m_FRecord;
};

/////////////////////////////////////////////////////////////////////////////
// CFrameObject (new frame object)
                                    // When converting to HTML,
                                    // ignore compliance check and:
#define  HTMLFrame_ForceText     1  // Keep this frame as text
#define  HTMLFrame_ForceGraphic  2  // Convert this frame to graphic

struct CFrameObjectRecord
{
	DB_RECORD_NUMBER	m_lFrameRecord;		/* record of frame data */
   DWORD             m_lExtraFlags;       // Added for HTML stuff
};

/*
// The class for saving a CFrameObject's state.
*/
class CFrameObjectState : public RectPageObjectState
{
public:
	CFrameObjectRecord	m_FRecord;
	DB_RECORD_NUMBER		m_lWarpField;

	FillFormatV1 m_Fill;
	OutlineFormat m_Outline;
	ShadowFormat m_Shadow;
};

/*
// A (new) frame object.
*/

class CFrameObject : public RectPageObject
{
	INHERIT(CFrameObject, RectPageObject)

protected:

	CFrameObjectRecord m_FRecord;

/*
// The frame's background while editing.
// It is assumed that the bitmap is managed elsewhere, so the frame is not
// responsible for deleting it.
*/

	HBITMAP m_hbmEditBackground;
	RECT m_rEdit;
	BOOL m_fEditing;

/*
// Handle any creation data.
// Use by this and derived classes.
*/

	ERRORCODE HandleCreationData(LPVOID creation_data);

/*
// Create the text record (and default text) for this object.
*/

	ERRORCODE CreateTextRecord(void);

	// Convert an old frame record to new style.

	ERRORCODE ConvertFrameRecord(const FrameObjectRecord* pOldData);

/*
// Private constructor.
// Call the create method.
*/

	CFrameObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

private:
	// This array is used at runtime to keep count of paragraphs 
	// for numbered and outline or multilevel numbered lists so the 
	// correct number can then be displayed. 
	unsigned char m_NumberedListCounter[BULLET_LEVEL_MAX];
public:

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Update an object into a redisplay context.
*/

	virtual UpdateStatePtr update(
		RedisplayContextPtr rc,
		PBOX_PTR extent,
		LPRECT clip,
		UpdateStatePtr ustate,
		REFRESH_TYPE refresh_type);

	// Draw the frame outline.
	virtual void DrawFrameOutline(RedisplayContextPtr rc);

/*
// Access the frame record number for this object.
*/

	void FrameRecord(DB_RECORD_NUMBER lFrameRecord)
		{ m_FRecord.m_lFrameRecord = lFrameRecord; }

	DB_RECORD_NUMBER FrameRecord(void) const
		{ return m_FRecord.m_lFrameRecord; }

/*
// Set the frame's editing state.
*/

	void SetEditing(BOOL fEditing);

/*
// Set the frame's editing background.
// It is assumed that the bitmap is managed elsewhere, so the frame is not
// responsible for deleting it.
*/

	void SetEditBackground(HBITMAP back_bitmap, RECT bitmap_rect);

/*
// Clear the edit background.
*/

	void ClearEditBackground(void)
		{ m_hbmEditBackground = NULL; }

/*
// Get the frame's editing background.
// It is assumed that the bitmap is managed elsewhere, so the frame is not
// responsible for deleting it.
*/

	HBITMAP GetEditBackground(void)
		{ return m_hbmEditBackground; }

	// Margins
	void Margins(const PBOX& Margins);
	PBOX Margins(void);

	// Lock the frame.
	CFrameRecord* LockFrameRecord(ERRORCODE* pError = NULL);

	// Lock the text.
	CTextRecord* LockTextRecord(ERRORCODE* pError = NULL);

	// Return the text record number.
	DB_RECORD_NUMBER TextRecord(void);

	// Destroy the frame and text record.
	void DestroyFrameAndText(void);


	// Set the style for the text in this frame.
	// This is used in special cases (like calendar, banner, etc).

	void SetTextStyle(const CTextStyle* pStyle);
	void SetParagraphStyle(const CTextStyle* pStyle);

	// Get the warp field attached to the frame.

	CWarpField* GetWarpField(void);

	// Return the bounding box of a bounds after it has been warped, flipped, and rotated.
	BOOL GetWarpedFlippedAndRotatedBoundingBox(PBOX* pBox);

	// Draw the frame background.
	BOOL DrawBackground(RedisplayContextPtr rc);

/////////////////////////////////////////////////////////////////////////////
// Frame update state

	class CUpdateState : public UpdateState
	{
	public:
		CUpdateState();
		virtual ~CUpdateState();

	public:
		BOOL					m_fClearing;
		// Line traversal.
		LINE_INDEX			m_lLineIndex;
		CHARACTER_INDEX	m_lFirstCharacterInLine;
		WORD_INDEX			m_lFirstWordInLine;
		// Word traversal.
		WORD_INDEX			m_lWordIndex;
		CHARACTER_INDEX	m_lFirstCharacterInWord;
		PCOORD				m_UnalignedOffset;
		// Character traversal.
		CHARACTER_INDEX	m_lCharacterIndex;
		PCOORD				m_XOffset;
		// Text record mods.
		int					m_nTextModificationCount;

	public:
		CTextDisplayList* m_pDisplayList;
	};

	// Update a frame.
	UpdateStatePtr UpdateFrame(
		RedisplayContextPtr rc,
		PBOX_PTR pExtent,
		PBOX_PTR pClearExtent,
		BOOL fClearOut,
		CUpdateState* pUpdateState,
		BOOL fIsOffscreen);

	// Draw a tab (leader).
	BOOL DrawTab(
		RedisplayContext* rc,
		CTextWord* pWord,
		CTextIterator* pIterator,
		PCOORD x,
		PCOORD y,
		BOOL fClearOut,
		PCOORD ColumnLeft,
		PCOORD ColumnRight,
		PCOORD* new_x,
	   const PBOX& Bounds,
		BOOL* pfUnderline,
		PCOORD UnalignedOffset,
		CTextDisplayList* pDisplayList = NULL);

		// draw a bullet character or number
		BOOL DrawBullet( 
			CTextIterator* pIterator, 
			RedisplayContextPtr rc, 
			PCOORD x, 
			PCOORD y,
			BOOL fClearOut, 
			const PBOX& Bounds, 
			CTextDisplayList* pDisplayList = NULL);

	// updates the counter for numbered lists
	int InitListCounter( CHARACTER_INDEX charIndex );

	// Is this frame a generic text frame (where "generic" means a frame
	// create by the user which behaves in a normal fashion - not special
	// banner or calendar frames)?
	BOOL GenericFrame(void);

   // IsHTMLCompliant will test if a this object is HTML compliant
   // and fill in the optional CHTMLCompliantInfo object
   // with which attributes were not compliant upon failure.
   BOOL IsHTMLTextCompliant(CHTMLCompliantInfo *pCompliantInfo=NULL);

   // This method converts text in frame to HTML equivalent
   ERRORCODE ConvertToHTML(CHTMLTag & refHTMLTag);

   void
   SetHTMLAlign(CHTMLTagParagraph &tp, CTextStyle &ts);
   // Finds last visible character in frame
   // Returns -1 upon failure.
   CHARACTER_INDEX   FindLastVisibleChar();

	void
   GetURLList(CImageMapElementArray *pMap);

   void
   AddImageMapElement(CImageMapElementArray *pMap, PBOX box, HyperlinkData &HData);

	void
	RemoveHyperlink(DB_RECORD_NUMBER linkno = 0);	// 0 = remove 'em all

   // Set the page for this object.
	virtual void Page(DB_RECORD_NUMBER lPage);

/////////////////////////////////////////////////////////////////////////////
// Stretch methods

	ERRORCODE StretchText(BOOL fFlowingMacros = FALSE, CWordArray* pMacros = NULL);
	ERRORCODE StretchFrame(void);
	BOOL CanStretch(void);
	BOOL IsStretched(void);
	// Helper function...
	void ComputeFlowBounds(CFrameRecord* pFrame, CTextRecord* pText, PPNT OriginalDims, PPNT* pTextExtent, BOOL fDoFlow, BOOL fFlowingMacros = FALSE, CWordArray* pMacros = NULL);
	PCOORD ComputeTextHeight(void);

/////////////////////////////////////////////////////////////////////////////
// Macro support.

	// Reflow macros for the story in this frame.
	ERRORCODE ReflowMacros(CWordArray* pMacros = NULL);

/////////////////////////////////////////////////////////////////////////////
// Story list support.

	// Make sure we are removed from the story list.
	void RemoveStory(void);

	// Make sure we are in the story list.
	void AddStory(void);

/////////////////////////////////////////////////////////////////////////////
// Intersections support.

	// Compute the intersections of this object with all following objects.
	// Returns whether things changed or not.
	BOOL ComputeIntersections(BOOL fReflow = TRUE);

/////////////////////////////////////////////////////////////////////////////
// Storage methods

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

	virtual ERRORCODE read_data(StorageDevicePtr device);

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

	virtual ERRORCODE write_data(StorageDevicePtr device);

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

	virtual ST_MAN_SIZE size_data(StorageDevicePtr device);

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual void destroy(void);

/*
// Toggle select marks for an object.
*/

	virtual BOOL toggle_selects(RedisplayContextPtr rc, FLAGS which);

/*
// Get an object's rotation.
*/

	virtual HCURSOR GetContentCursor(void);

/*
// Return whether a point is over an object's move area or not.
*/

	virtual BOOL pt_on_move_area(PPNT p, RedisplayContextPtr rc);

/*
// Recalc after a size or movement.
*/

	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

/*
// Flip an object left to right.
*/

	virtual void xflip(BOOL fChangeAngle = TRUE);

/*
// Flip an object top to bottom.
*/

	virtual void yflip(BOOL fChangeAngle = TRUE);

/*
// Return the refresh bounds for this object.
*/

	virtual void get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

/*
// Return the clipping region for this object.
*/

	virtual BOOL GetClipRgn(CRgn&, RedisplayContextPtr rc, int nType = CLIP_TYPE_Boundary);

/*
// Is the frame opaque?
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
// Get an object's name.
*/

	virtual BOOL get_name(LPSTR buffer, SHORT size, SHORT max_width);

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
// Set whether the object is on or off the page at the moment.
*/

	virtual void OnPage(BOOL fOnPage);
/*
// Get the properties for this object.
*/

	virtual void GetObjectProperties(CObjectProperties& Properties);

/*
// Set the properties for this object.
*/

	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

/*
// Manupulate special flags for this object
*/
   DWORD
   GetExtraFlags(void)
   {
      return m_FRecord.m_lExtraFlags;
   }

   DWORD
   SetExtraFlag(DWORD d)
   {
      m_FRecord.m_lExtraFlags |= d;
      return m_FRecord.m_lExtraFlags;
   }
   
   DWORD
   ClearExtraFlag(DWORD d)
   {
      m_FRecord.m_lExtraFlags = (m_FRecord.m_lExtraFlags & ~d);
      return m_FRecord.m_lExtraFlags;
   }
   
   DWORD
   SetAllExtraFlags(DWORD d)
   {
      m_FRecord.m_lExtraFlags = d;
      return m_FRecord.m_lExtraFlags;
   }

   void
   ClearExtraFlags(void)
   {
      m_FRecord.m_lExtraFlags = 0;
   }

   BOOL
   TestExtraFlag(DWORD Flag)
   {
      return (m_FRecord.m_lExtraFlags & Flag) == 0 ? FALSE : TRUE;
   }
};

#endif					// #ifndef __FRAMEOBJ_H__
