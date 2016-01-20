/*
// $Header: /PM8/App/PMGOBJ.H 2     3/08/99 1:14p Rgrenfel $
//
// Definitions for a PrintMaster Gold page database.
//
// $Log: /PM8/App/PMGOBJ.H $
// 
// 2     3/08/99 1:14p Rgrenfel
// Added methods to allow an object to have its data record ID set,
// fetched and detached.
// 
// 38    3/05/99 6:51p Rgrenfel
// Added methods to allow an object to have its ID set, detached and
// fetched.
// 
// 37    2/26/99 8:38p Rgrenfel
// Created a way to get the original image data path.
// 
// 36    2/04/99 5:57p Rgrenfel
// Allowed the detection that the raw object data was in JPEG format.
// 
// 35    2/04/99 11:58a Rgrenfel
// Added methods for determining the raw image size and to allow the
// export of the raw image.
// 
// 34    2/01/99 4:34p Lwilson
// Added initial support for converting PM objects into component objects.
// 
// 33    1/29/99 12:59p Lwilson
// Moved RColor object properties fill data member and into Object
// Properties.
// 
// 32    1/27/99 1:43p Lwilson
// Added RColor support to FillForegroundColor property.
// 
// 31    1/25/99 3:44p Rgrenfell
// Added support for requesting if the component is an animated GIF image.
// 
// 30    1/19/99 4:11p Cschendel
// added enum for text wrap types
// 
// 29    12/08/98 5:18p Lwilson
// Initial Print Shop integration.
// 
// 28    7/24/98 11:58a Rlovejoy
// Added 'flags' to PageRecordStruct.
// 
// 27    7/18/98 6:24p Jayn
// Table fixes.
// 
// 26    5/14/98 4:48p Johno
// Moved :MakeInvisible() and IsInvisible() to CObjectProperties
// 
// 25    5/14/98 12:39p Johno
// Added PMGPageObject::MakeInvisible() and PMGPageObject::IsInvisible()
// 
// 24    5/13/98 4:34p Johno
// IsInvisibleHotSpot() (preliminary)
// 
// 23    4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 22    4/29/98 6:31p Rlovejoy
// Updated for new fill format.
// 
// 21    4/23/98 3:28p Johno
// HTML changes for rotated text (as graphic with polygon image map)
// 
// 20    4/23/98 10:49a Johno
// Added PMObjectShapeData, GetShapeData()
// for polygon image maps
// 
// 19    4/14/98 4:41p Jayn
// Changes for the new table object.
// 
// 18    3/25/98 6:36p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 17    3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 16    3/13/98 2:54p Johno
// Added GetHyperlinkData
// 
// 15    3/11/98 5:53p Johno
// Use HyperlinkData struct
// 
// 14    3/10/98 4:12p Johno
// Added GetHyperlinkRecordNumber
// 
// 13    3/05/98 5:46p Johno
// Added  OBJECT_TYPE_Hyperlink, m_HyperlinkRecord to PMGPageObjectRecord.
// 
// 12    2/17/98 11:43a Dennis
// Added Background Object
// 
// 11    2/09/98 8:26p Hforman
// add set_object_image()
// 
// 10    2/03/98 10:34a Jstoner
// saving incremental progress 
// 
// 9     1/28/98 10:39a Jstoner
// incremental update to table project
// 
// 8     1/20/98 8:54a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
// 
// 7     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 6     1/07/98 7:47a Jstoner
// added object type cell for table object
// 
// 5     1/05/98 4:54p Hforman
// add OBJECT_TYPE_Border
// 
// 4     12/11/97 10:45a Jstoner
// added tableobject type 
// 
// 3     11/14/97 11:39a Hforman
// added OBJECT_TYPE_Guides
//
// 2     10/28/97 5:40p Dennis
// Added IsEmpty()
// 
//    Rev 1.0   14 Aug 1997 15:23:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:10   Fred
// Initial revision.
// 
//    Rev 1.32   18 Jul 1997 14:49:36   Jay
// Moved CObjectStateArray to pmgobj.
// 
//    Rev 1.31   05 Jun 1997 16:48:10   Jay
// UPDATE_TYPE_HitUptoObject
// 
//    Rev 1.30   23 May 1997 17:06:44   Fred
// New effects
// 
//    Rev 1.29   14 Apr 1997 15:42:46   Jay
// Text box editing changes
// 
//    Rev 1.28   26 Mar 1997 14:52:04   Jay
// Interruptable preview
// 
//    Rev 1.27   17 Mar 1997 14:48:24   Jay
// Warp shape palette and support.
// 
//    Rev 1.26   04 Mar 1997 17:04:06   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.25   26 Feb 1997 10:57:36   Jay
// Cropping; GetClipRgn() changes
// 
//    Rev 1.24   25 Feb 1997 12:57:24   Jay
// Cropping and extensions
// 
//    Rev 1.23   21 Feb 1997 15:59:58   Fred
// UpdateState::object
// 
//    Rev 1.22   21 Feb 1997 15:18:14   Jay
// Got rid of object member of UpdateState.
// 
//    Rev 1.21   21 Feb 1997 15:14:42   Jay
// New mode handling; crop mode support
// 
//    Rev 1.21   21 Feb 1997 15:11:36   Jay
// New mode handling; crop mode support
// 
//    Rev 1.20   19 Feb 1997 16:54:12   Jay
// Beginning of cropping
// 
//    Rev 1.19   07 Feb 1997 08:51:30   Fred
// Added GetClipRgn() object method
// 
//    Rev 1.18   27 Jan 1997 13:55:24   Jay
// Default object attributes and size.
// 
//    Rev 1.17   23 Jan 1997 11:05:48   Jay
// New popup palettes
// 
//    Rev 1.16   17 Jan 1997 17:03:18   Jay
// New select for drawing objects.
// 
//    Rev 1.15   08 Jan 1997 15:22:32   Fred
// More text box changes
// 
//    Rev 1.14   06 Jan 1997 10:17:14   Fred
// Start of new text object
// 
//    Rev 1.13   09 Dec 1996 10:19:48   Jay
// More image stuff.
// 
//    Rev 1.12   28 Oct 1996 13:46:32   Jay
// Changes from 3.01 code
// 
//    Rev 1.12   25 Oct 1996 10:29:28   Jay
// Frame not on page support.
// 
//    Rev 1.11   07 Oct 1996 17:02:34   Jay
// TimeStamp
// 
//    Rev 1.10   26 Jul 1996 11:38:58   Jay
// Page stuff
// 
//    Rev 1.9   25 Jul 1996 19:38:10   Jay
//  
// 
//    Rev 1.8   15 Jul 1996 11:34:30   Jay
//  
// 
//    Rev 1.7   12 Jul 1996 16:47:56   Jay
// Text wrap
// 
//    Rev 1.6   08 Jul 1996 08:49:08   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.5   28 Jun 1996 17:32:00   Jay
// OnPage() routine.
// 
//    Rev 1.4   27 Jun 1996 13:28:22   Jay
//  
// 
//    Rev 1.3   26 Jun 1996 14:21:44   Jay
//  
// 
//    Rev 1.2   26 Jun 1996 09:07:06   Jay
//  
// 
//    Rev 1.1   24 May 1996 09:27:30   Jay
// New text code.
// 
//    Rev 1.0   14 Mar 1996 13:47:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:20   FRED
// Initial revision.
// 
//    Rev 2.9   14 Aug 1995 10:09:58   JAY
// Added rotation save to object (for move handle and move object).
// 
//    Rev 2.8   13 May 1995 11:14:12   JAY
//  
// 
//    Rev 2.7   11 May 1995 07:56:34   JAY
// Set unrotated bound. Constrained rotate.
// 
//    Rev 2.6   10 May 1995 13:12:26   JAY
// Made calc_bounds() virtual.
// 
//    Rev 2.5   04 May 1995 17:16:02   JAY
// Fixed-point. Drawing objects.
// 
//    Rev 2.4   19 Apr 1995 16:05:16   JAY
// Removed some symbols.
// 
//    Rev 2.3   19 Apr 1995 14:46:38   FRED
// Added preliminary support for warped text object
// 
//    Rev 2.2   20 Mar 1995 16:25:42   JAY
// Added OleObject type.
// 
//    Rev 2.1   15 Feb 1995 17:02:50   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:52   JAY
// Initial revision.
// 
//    Rev 1.9   04 Aug 1994 07:32:46   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   09 Mar 1994 09:22:02   MACDUFF
// Added ExtraPixels param to refresh extents
// 
//    Rev 1.7   17 Feb 1994 09:21:46   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.6   10 Feb 1994 15:14:26   MACDUFF
// Toggle functions now return flag indicating whether they drew anything.
// 
//    Rev 1.5   07 Feb 1994 11:49:00   JAY
// did_erase field now in refresh extent not update state.
// 
//    Rev 1.4   28 Jan 1994 17:11:26   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:01:04   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 08:08:16   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:11:00   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:25:46   JAY
//  
*/

#ifndef __PMGOBJ_H__
#define __PMGOBJ_H__

#include "objdb.h"
#include "pagedefs.h"
#include "redisp.h"
#include "refresh.h"
#include "effects.h"
#include "timestmp.h"

class COutlinePath;
class CObjectImage;
class HyperlinkRecord;
struct HyperlinkData;
class CPGIFAnimator;

/*
   Some class hierarchy information:

   ListElement->PageObject->PMGPageObject->RectPageObject->DrawingObject->RectangleObject

*/

#define MANUAL_LAYOUT	-2

typedef enum
{
   X_BOUNDS,
   Y_BOUNDS,
} BOUNDS;

/*
// An action ID.
// I didn't make this an enum so we could add/change them more easily
// (without a major rebuild every time).
//
// The current action IDs are in ACTION.H which isn't included here (for the
// reason in the previous paragraph).
*/

typedef SHORT PMG_OBJECT_ACTION;

/*
// States used by move_handle() and move_object() sequences.
*/

typedef enum
{
	DRAG_STATE_begin,
	DRAG_STATE_continue,
	DRAG_STATE_end,
	DRAG_STATE_end_drag,
	DRAG_STATE_abort
} DRAG_STATE;

enum FLOW_AROUND
{
	FLOW_AROUND_none,				// Text flows over
	FLOW_AROUND_all				// Text flows around
};

/*
// Flags that an object may have to remember states.
*/

#define	OBJECT_FLAG_locked			0x0001
#define	OBJECT_FLAG_landscape		0x0002
#define	OBJECT_FLAG_xflipped			0x0004
#define	OBJECT_FLAG_yflipped			0x0008
#define	OBJECT_FLAG_needs_calc		0x0010
#define	OBJECT_FLAG_flow_left		0x0020
#define	OBJECT_FLAG_flow_right		0x0040
#define	OBJECT_FLAG_selected			0x0080
#define	OBJECT_FLAG_needs_update	0x0100
#define	OBJECT_FLAG_no_mask			0x0200
#define	OBJECT_FLAG_grouped			0x0400
#define	OBJECT_FLAG_calc_ignore		0x0800		/* This is definitely a kludge */
#define	OBJECT_FLAG_resize_ignore	0x1000
#define	OBJECT_FLAG_hidden			0x2000

#define	OBJECT_FLAG_flow			(OBJECT_FLAG_flow_left|OBJECT_FLAG_flow_right)

/*
// Flags recording an object's refresh state.
*/

#define	REFRESH_FLAG_makes_image	0x0001		/* Can make an image */
#define	REFRESH_FLAG_has_mask		0x0002		/* Has a mask */
#define	REFRESH_FLAG_has_image		0x0004		/* Has an image to use */
#define	REFRESH_FLAG_opaque			0x0008		/* Is always opaque */
#define	REFRESH_FLAG_intersect		0x0010		/* Compute intersections */
#define	REFRESH_FLAG_moving			0x0020		/* Object is on the move */
#define	REFRESH_FLAG_sizing			0x0040		/* Object is being resized */
#define	REFRESH_FLAG_cropping		0x0080		/* Object is being cropped */
#define	REFRESH_FLAG_panning			0x0100		/* Object is being panned */

/*
// Flags recording an object's selection status.
*/

#define	SELECT_FLAG_boundary			0x01			/* bounding box */
#define	SELECT_FLAG_size_handles	0x02			/* 8 handles */
#define	SELECT_FLAG_move_handle		0x04			/* middle move handle */
#define	SELECT_FLAG_drag_handle		0x08			/* can drag outline */
#define	SELECT_FLAG_rotate_handle	0x10			/* can rotate */
#define	SELECT_FLAG_solo_select		0x20			/* deselect all else */
#define	SELECT_FLAG_draw_boundary	0x40			/* during cropping */
#define	SELECT_FLAG_subselect		0x80			/* table cells, etc */

#define	SELECT_FLAG_handles					(SELECT_FLAG_size_handles|SELECT_FLAG_move_handle|SELECT_FLAG_rotate_handle)
#define	SELECT_FLAG_handles_no_rotate		(SELECT_FLAG_size_handles|SELECT_FLAG_move_handle)

/*
// Object handle standard values.
*/

typedef enum
{
	OBJECT_HANDLE_NONE	= -1,					/* Value for no handle */
	OBJECT_HANDLE_UL		= 0,					/* Standard eight bound handles */
	OBJECT_HANDLE_UPPER	= 1,
	OBJECT_HANDLE_UR		= 2,
	OBJECT_HANDLE_LEFT	= 3,
	OBJECT_HANDLE_MOVE	= 4,					/* The center movement handle */
	OBJECT_HANDLE_RIGHT	= 5,
	OBJECT_HANDLE_LL		= 6,
	OBJECT_HANDLE_LOWER	= 7,
	OBJECT_HANDLE_LR		= 8,
	OBJECT_HANDLE_ROTATE	= 9,
	OBJECT_HANDLE_RESIZE_COL = 10,
	OBJECT_HANDLE_RESIZE_ROW = 11,
	OBJECT_HANDLE_SELECT_COL = 12,
	OBJECT_HANDLE_SELECT_ROW = 13,
	NUMBER_OF_OBJECT_HANDLES
} OBJECT_HANDLE;

typedef class PMGPageObject far *PMGPageObjectPtr;

/*
// Types of completion state when an update state is returned.
// (If an update is completed, no update state should be returned.)
//
// Important note:
// As of now, Interrupted states are C++ objects which need to be freed (by
// using 'delete').
//
// ALL OTHER STATES ARE STATIC and should not be delete'd.
*/

typedef enum
{
	UPDATE_TYPE_Interrupted,
	UPDATE_TYPE_Error,
	UPDATE_TYPE_Terminated,
	UPDATE_TYPE_HitUptoObject,

} UPDATE_TYPE;

/*
// A generally useful class specifically used in PMGPageObject.
*/

struct CDoubleRect
{
	double left;
	double top;
	double right;
	double bottom;
	double Width(void) const
		{ return right-left; }
	double Height(void) const
		{ return bottom-top; }
	BOOL operator==(const CDoubleRect& r) const
		{ return memcmp(&r, this, sizeof(*this)) == 0; }
	BOOL operator!=(const CDoubleRect& r) const
		{ return !(operator==(r)); }
};

/*
// The "base class" for an update state.
// Actual state structures will include this as part of the beginning as
// well as any object type specific information needed.
*/

class UpdateState
{
public:
	virtual ~UpdateState() {}
	BOOL					allocates_memory;
	UPDATE_TYPE			type;
	ERRORCODE			error;			/* Error if ERROR */

	// This is candidate for REMOVAL!
	PMGPageObjectPtr	object;
};

/*
// This is a class to hold all possible object properties.
// Objects only need to support those properties applicable
// to the object.
*/

class CObjectProperties
{
public:
	CObjectProperties(PMGDatabase* pDatabase = NULL);
	~CObjectProperties();

	void SetDatabase(PMGDatabase* pDatabase)
		{ m_pDatabase = pDatabase; }
	void Reset(void);
	void MergeWith(CObjectProperties& OtherProperties);
	void FreeWarpField(void);

	// Pattern

	void SetFillPattern(SHORT nPattern, BOOL fMerge = TRUE);
	BOOL ApplyFillPattern(SHORT& nPattern);

	void SetFillType(SHORT nType, BOOL fMerge = TRUE);
	BOOL ApplyFillType(SHORT& nType);

	void SetFillForegroundColor(COLOR ForegroundColor, BOOL fMerge = TRUE);
	BOOL ApplyFillForegroundColor(COLOR& ForegroundColor);

	void SetFillForegroundColor(const RColor& ForegroundColor, BOOL fMerge = TRUE);
	BOOL ApplyFillForegroundColor(RColor& ForegroundColor);

	void SetFillGradientBlendColor(COLOR GradientBlendColor, BOOL fMerge = TRUE);
	BOOL ApplyFillGradientBlendColor(COLOR& GradientBlendColor);

	// Outline

	void SetOutlineStyle(SHORT nStyle, BOOL fMerge = TRUE);
	BOOL ApplyOutlineStyle(SHORT& nStyle);

	void SetOutlineWidthType(SHORT nWidthType, BOOL fMerge = TRUE);
	BOOL ApplyOutlineWidthType(SHORT& nWidthType);

	void SetOutlineWidth(CFixed lWidth, BOOL fMerge = TRUE);
	BOOL ApplyOutlineWidth(CFixed& lWidth);

	void SetOutlinePattern(SHORT nPattern, BOOL fMerge = TRUE);
	BOOL ApplyOutlinePattern(SHORT& nPattern);

	void SetOutlineForegroundColor(COLOR ForegroundColor, BOOL fMerge = TRUE);
	BOOL ApplyOutlineForegroundColor(COLOR& ForegroundColor);

	// Shadow

	void SetShadowStyle(SHORT nStyle, BOOL fMerge = TRUE);
	BOOL ApplyShadowStyle(SHORT& nStyle);

	void SetShadowOffsetType(SHORT nOffsetType, BOOL fMerge = TRUE);
	BOOL ApplyShadowOffsetType(SHORT& nOffsetType);

	void SetShadowXOffset(CFixed lXOffset, BOOL fMerge = TRUE);
	BOOL ApplyShadowXOffset(CFixed& lXOffset);

	void SetShadowYOffset(CFixed lYOffset, BOOL fMerge = TRUE);
	BOOL ApplyShadowYOffset(CFixed& lYOffset);

	void SetShadowPattern(SHORT nPattern, BOOL fMerge = TRUE);
	BOOL ApplyShadowPattern(SHORT& nPattern);

	void SetShadowForegroundColor(COLOR ForegroundColor, BOOL fMerge = TRUE);
	BOOL ApplyShadowForegroundColor(COLOR& ForegroundColor);

	// Warp field

	void SetWarpField(DB_RECORD_NUMBER lWarpField, BOOL fMerge = TRUE);
	BOOL ApplyWarpField(DB_RECORD_NUMBER& lWarpField);

#if 0
	void SetFillColor(COLOR clColor, BOOL fMerge = TRUE);
	BOOL ApplyFillColor(COLOR& clColor);

	void SetFillColorExists(BOOL fColorExists, BOOL fMerge = TRUE);
	BOOL ApplyFillColorExists(BOOL& fColorExists);

	void SetLineWidth(int nWidth, BOOL fMerge = TRUE);
	BOOL ApplyLineWidth(int& nWidth);

	void SetLineColor(COLOR clColor, BOOL fMerge = TRUE);
	BOOL ApplyLineColor(COLOR& clColor);

	void SetLineColorExists(BOOL fColorExists, BOOL fMerge = TRUE);
	BOOL ApplyLineColorExists(BOOL& fColorExists);

	void SetShadowColor(COLOR clColor, BOOL fMerge = TRUE);
	BOOL ApplyShadowColor(COLOR& clColor);

	void SetShadowColorExists(BOOL fColorExists, BOOL fMerge = TRUE);
	BOOL ApplyShadowColorExists(BOOL& fColorExists);

	void SetWarpField(DB_RECORD_NUMBER lWarpField, BOOL fMerge = TRUE);
	BOOL ApplyWarpField(DB_RECORD_NUMBER& lWarpField);
#endif

   void
   MakeInvisible(void);

   BOOL
   IsInvisible(void);

protected:

	PMGDatabase* m_pDatabase;			// For database specific stuff.

	/*
	// There are three cases for properties we care about:
	// 1) No object has a value for the property (not Set).
	// 2) One or more objects have the same value for the property (Set but not Undefined).
	// 3) The objects have more than one value for the property (Set and Undefined).
	*/

	WORD m_wPropertySet;
	WORD m_wPropertyUndefined;		// Multiply-defined

	enum
	{
		FLAG_FillPattern					= 0x0001,
		FLAG_FillForegroundColor		= 0x0002,
		FLAG_FillType						= 0x4000,
		FLAG_FillGradientBlendColor	= 0x8000,
		FLAG_OutlineStyle					= 0x0004,
		FLAG_OutlineWidthType			= 0x0008,
		FLAG_OutlineWidth					= 0x0010,
		FLAG_OutlinePattern				= 0x0020,
		FLAG_OutlineForegroundColor	= 0x0040,
		FLAG_ShadowStyle					= 0x0080,
		FLAG_ShadowOffsetType			= 0x0100,
		FLAG_ShadowXOffset				= 0x0200,
		FLAG_ShadowYOffset				= 0x0400,
		FLAG_ShadowPattern				= 0x0800,
		FLAG_ShadowForegroundColor		= 0x1000,
		FLAG_WarpField						= 0x2000
	};

public: //j
	// Fill
	FillFormatV2 m_Fill;
	RColor		 m_crFill;

	// Outline
	OutlineFormat m_Outline;

	// Shadow
	ShadowFormat m_Shadow;

	// Warp field (shape) component
	DB_RECORD_NUMBER m_lWarpField;
};

/*
// The rotation component of an object.
*/

typedef struct
{
	ANGLE		rotation;
	PPNT		origin;
} ROTATION_COMPONENT, far *ROTATION_COMPONENT_PTR;

enum ETextFlow
{
	FLOW_none = 0,						// text does not flow around the object
	FLOW_Box,							// text flows to the bounding box of the object
	FLOW_Shape							// text flows to the (irregular) shape of the object
};

/*
// The structure a PMGPageObject has internally.
*/
struct PMGPageObjectRecord
{
	ROTATION_COMPONENT	rotc;				      /* Rotation component. */
	SHORT						panel;			      /* which panel is this object a part of */
	FLAGS						select_flags;	      /* Selection flags */
	FLAGS						flags;			      /* flags in assorted colors. */
	DB_RECORD_NUMBER		layout;       	      /* inode of layout */
	ROTATION_COMPONENT	group_rotc;		      /* Rotc when it joined group */
	PMG_OBJECT_ACTION		primary_action;
	PMG_OBJECT_ACTION		secondary_action;
	LONG						m_lFlowAround;	/* How does text flow around me? */ 
	DB_RECORD_NUMBER		m_lPage;			      /* What page are we on? */
	// Visible rect
	CDoubleRect				m_VisibleRect;
   DB_RECORD_NUMBER		m_HyperlinkRecord;   // For 5.0 Hyperlinks
};

/*
// The class for saving an PMGPageObject's state.
*/

class PMGPageObjectState : public PageObjectState
{
public:
	virtual ~PMGPageObjectState();

	PMGPageObjectRecord	m_Record;
	PMGDatabasePtr			m_pDatabase;		// For any records.
};

/*
// An array to save object states.
*/

class CObjectStateArray : public CPtrArray
{
public:
	virtual ~CObjectStateArray();

	void DeleteAll(void);
	PageObjectState* GetAt(int nIndex) const
		{ return (PageObjectState*)CPtrArray::GetAt(nIndex); }
};

class PMObjectShapeData
{
   public:

   BOOL
   IsRect(void) const;

   BOOL
   GetRect(CRect &Rect) const;

   void
   GetPoints(CArray<CPoint, CPoint&> &Shape) const;

   void
   SetPoints(PPNT *p, int n);
   // For unrotated rect
   void
   SetPoints(PBOX &pb);

   void
   SetPoints(CRect Rect);

   void
   SetPoints(CArray<CPoint, CPoint&> &Shape);

   int
   NumPoints(void)
   {
      return mShape.GetSize();
   }

   void
   CopyTo(PMObjectShapeData &sd);
   
   protected:
   CArray<CPoint, CPoint&> mShape;
};

/*
// A PrintMaster Gold object.
*/

class PMGPageObject : public PageObject
{
	INHERIT(PMGPageObject, PageObject)
private:

	PMGPageObjectPtr	next_selected;

// Local routine.
	BOOL ImageChanged(PMGPageObjectState* pState);

protected:

	WORD					refresh_flags;	/* Flags used by the refresh code. */
	CObjectImage*		object_image;
	BOOL					m_fOnPage;

/*
// The record.
*/

	PMGPageObjectRecord record;

/*
// This is the object which LOGICALLY contains this object. It may or may
// not PHYSICALLY contain this object (so don't assume it does). This
// variable is zero by default but may be used by container objects at their
// own discretion.
//
// NOTE that this variable is in no way saved or loaded when a document
// goes to disk or is brought back. It should be considered fairly transient.
*/

	PMGPageObjectPtr	container;

/*
// This is the object which PHYSICALLY contains this object. It may or may
// not LOGICALLY contain this object.
//
// NOTE that this variable is not saved or loaded with the document (since it
// obviously can't be), and it is up to a parent object's read routine to
// re-establish it.
*/

	PMGPageObjectPtr parent;

	// Compute a draw bound from another bound.
	// Helper function.
	PBOX ComputeDrawBound(PBOX Bound, CDoubleRect* pRect = NULL);

public:

	PMGPageObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);
	virtual ~PMGPageObject();

	ERRORCODE	read(StorageDevicePtr device);
	ERRORCODE	write(StorageDevicePtr device);
	ST_MAN_SIZE	size(StorageDevicePtr device);

	virtual void SetRecordID( DB_RECORD_NUMBER nRecNum )
		{ TRACE("Not supported by child class!\n" ); ASSERT( 0 ); }
	virtual void DetachRecord()
		{ SetRecordID( 0 ); }
	virtual DB_RECORD_NUMBER GetRecordID()
		{ TRACE("Not supported by child class!\n" ); ASSERT( 0 ); return 0; }

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

/********************************************/
/* Methods for accessing the object's data. */
/********************************************/

/*
// Get the next selected object in the list.
*/

	PMGPageObjectPtr next_selected_object();

/*
// Get the address of the next selected object entry for this object.
// Used mainly for list manipulation.
*/

	PMGPageObjectPtr far *next_selected_address();

/*
// Set the next selected object in the list.
*/

	VOID set_next_selected_object(PMGPageObjectPtr o);

/*
// Get an object's bound.
*/

	virtual PBOX get_bound(VOID) = 0;

/*
// Get an object's rotation.
*/

	ANGLE get_rotation(VOID);

/*
// Get an object's origin.
*/

	PPNT get_origin(VOID);

/*
// Set an object's rotation.
// You will need to call calc() after this to update things dependent
// on the rotation.
*/

	VOID set_rotation(ANGLE rot);

/*
// Set an object's panel.
*/

	VOID set_panel(SHORT p);

/*
// Get an object's panel.
*/

	SHORT get_panel(VOID);

/*
// Set an object's layout.
*/

	VOID set_layout(DB_RECORD_NUMBER l);

/*
// Set an object's layout (freeing any old one).
*/

	VOID set_new_layout(DB_RECORD_NUMBER l);

/*
// Get an object's layout.
*/

	DB_RECORD_NUMBER get_layout(VOID);

/*
// Get an object's visible rect.
*/

	CDoubleRect& GetVisibleRect(void)
		{ return record.m_VisibleRect; }

/*
// Set an object's select flags.
*/

	VOID set_select_flags(FLAGS f);

/*
// Get an object's select flags.
*/

	FLAGS get_select_flags(VOID);

/*
// Add one or more flags to this object's select flags.
*/

	VOID add_select_flags(FLAGS f);

/*
// Remove one or more flags from this object's select flags.
*/

	VOID remove_select_flags(FLAGS f);

/*
// Set an object's flags.
*/

	VOID set_flags(FLAGS f);

/*
// Get an object's flags.
*/

	FLAGS get_flags(VOID);

/*
// Add one or more flags to this object's flags.
*/

	VOID add_flags(FLAGS f);

/*
// Remove one or more flags from this object's flags.
*/

	VOID remove_flags(FLAGS f);

	// Get/Set an object's image info.
	CObjectImage* get_object_image();
	void set_object_image(CObjectImage* objimg);

/*
// Set an object's refresh flags.
*/

	VOID set_refresh_flags(FLAGS f);

/*
// Get an object's refresh flags.
*/

	FLAGS get_refresh_flags(VOID);

/*
// Add one or more flags to this object's refresh flags.
*/

	VOID add_refresh_flags(FLAGS f);

/*
// Remove one or more flags from this object's refresh flags.
*/

	VOID remove_refresh_flags(FLAGS f);

/*
// Set the primary action for an object.
*/

	VOID set_primary_action(PMG_OBJECT_ACTION action);

/*
// Set the secondary action for an object.
*/

	VOID set_secondary_action(PMG_OBJECT_ACTION action);

/*
// Get the container object for this object.
*/

	PMGPageObjectPtr get_container(VOID);

/*
// Set the container object for this object.
*/

	VOID set_container(PMGPageObjectPtr c);

/*
// Get the parent object for this object.
*/

	PMGPageObjectPtr get_parent(VOID);

/*
// Set the parent object for this object.
*/

	VOID set_parent(PMGPageObjectPtr p);

/*
// Copy the source objects attributes
*/
	virtual void CopyAttributes( PMGPageObjectPtr pObject );

/*
// Get the object's database.
*/

	PMGDatabasePtr get_database(VOID);

/*
// Get and set the object's page number.
*/
	virtual void Page(DB_RECORD_NUMBER lPage);
	DB_RECORD_NUMBER Page(void) const
		{ return record.m_lPage; }

	// Get the object's time stamp.
	virtual CTimeStamp GetTimeStamp(void);

/************************************/
/* Generally useful object methods. */
/************************************/

/*
// Primary action.
// This is what the object wants to have happen when the user clicks on it.
// The default is to SELECT the object (once the object is selected,
// movement and resizing are possible).
//
// On entry data points to a variable of LPVOID. This variable gets any
// specific data that goes with the action. This data should not be ignored
// in certain cases (like EDIT actions) because the object may be a place-
// holder for another object. This variable will contain the address of the
// object ('this') on entry, since this is a good default.
//
// If a NULL pointer is passed, this means that the object is not the only
// one selected (like with a shift click or multiple select). The action
// may be different in this case.
*/

	virtual PMG_OBJECT_ACTION primary_action(LPVOID far *data = NULL);

/*
// Secondary action.
// This is what the object wants to have happen when the user double-clicks
// on it. The default is to edit the object in some default (type-specific)
// way that the application chooses.
//
// On entry data points to a variable of LPVOID. This variable gets any
// specific data that goes with the action. This data should not be ignored
// in certain cases (like EDIT actions) because the object may be a place-
// holder for another object. This variable will contain the address of the
// object ('this') on entry, since this is a good default.
//
// If a NULL pointer is passed, this means that the object is not the only
// one selected (like with a shift click or multiple select). The action
// may be different in this case.
*/

	virtual PMG_OBJECT_ACTION secondary_action(LPVOID far *data = NULL);

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Toggle select marks for an object.
*/

	virtual BOOL toggle_selects(RedisplayContextPtr rc, FLAGS which) = 0;

/*
// Update an object into a redisplay context.
*/

	virtual UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

/*
// Get an object's rotation.
*/

	virtual HCURSOR GetContentCursor(void);

/*
// Return whether a point is in an object or not.
*/

	virtual BOOL pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object) = 0;

/*
// Return whether a point is over an object's move area or not.
*/

	virtual BOOL pt_on_move_area(PPNT p, RedisplayContextPtr rc) = 0;

/*
// Move an object a certain amount.
*/

	virtual PMG_OBJECT_ACTION move_object(DRAG_STATE state, PPNT vector, PPNT current_xy, LPVOID far *data = NULL, BOOL fPanning = FALSE) = 0;

/*
// Return whether a box is over a handle or not.
*/

	virtual OBJECT_HANDLE box_on_handle(PBOX_PTR box, RedisplayContextPtr rc) = 0;

/*
// Move an object's handle.
*/

	virtual PMG_OBJECT_ACTION move_handle(DRAG_STATE state, PPNT pos, OBJECT_HANDLE handle, FLAGS shift_status, LPVOID far *data = NULL, BOOL fCropping = FALSE) = 0;

/*
// Recalc after a size or movement.
*/

	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0) = 0;

/*
// Flip an object left to right.
*/

	virtual VOID xflip(BOOL fChangeAngle = TRUE);

/*
// Flip an object top to bottom.
*/

	virtual VOID yflip(BOOL fChangeAngle = TRUE);

/*
// Is this point in the object opaque?
*/

	virtual BOOL PtIsOpaque(PPNT p, RedisplayContextPtr rc);

/*
// Get the properties for this object.
*/

	virtual void GetObjectProperties(CObjectProperties& Properties);

/*
// Set the properties for this object.
*/

	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

/*
// We are being selected or deselected.
//
// This is mostly a notification. You should be careful if you use this to set 
// a state since the state could change without this function being called
// (e.g. if the document is saved while an object is selected).
//
// This function will be called with TRUE before the object has been toggled
// on, and it will called with FALSE before the object is toggled off.
*/

	virtual void OnSelectChange(BOOL fSelected = TRUE);

/********************************/
/* Image manipulation routines. */
/********************************/

/*
// Begin an image (Pre-update initialization).
*/

	virtual ERRORCODE begin_image(RedisplayContextPtr rc, PBOX world);

/*
// End an image (Post-update clean-up).
*/

	virtual ERRORCODE end_image(RedisplayContextPtr rc);

/*
// Free a the data for an object's image.
*/

	virtual VOID free_image(VOID);

/*
// Draw the (created) image for an object.
*/

	virtual ERRORCODE draw_image(RedisplayContextPtr rc, PBOX extent);

/*
// Notify an object that its image has changed.
*/

	virtual VOID changed_image(VOID);

/*
// Are we currently making an image?
*/

	virtual BOOL making_image(VOID);

/*
// Return the refresh bounds for this object.
*/

	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

	// Types of clip regions.
	enum
	{
		CLIP_TYPE_Boundary,
		CLIP_TYPE_Shadow,				// For drawing objects.
	};
/*
// Return the clipping region for this object.
*/

	virtual BOOL GetClipRgn(CRgn&, RedisplayContextPtr rc, int nType = CLIP_TYPE_Boundary);

/*
// Apply the clip region to a dc.
*/

	BOOL ApplyClipRgn(RedisplayContext* rc, int nType = CLIP_TYPE_Boundary);

/*
// Restore the clip region after an ApplyClipRgn.
*/

	void RestoreClipRgn(RedisplayContext* rc, BOOL f);

/*
// Return the actual drawing extent of this object (handles cropping).
*/

	virtual PBOX GetDrawBound(void);

/*
// Is an object opaque?
*/

	virtual BOOL is_opaque(RedisplayContextPtr rc);

/*
// Check if object has data?  It's up to derived class to do check
*/
   virtual BOOL IsEmpty()
      { return FALSE; }

/*
// Join a group.
// This is called in two passes:
//		On pass 1, the bound passed is NULL. This is a Join Inquiry. The object
//			should return TRUE if is will be joining the group.
//		On pass 2, all objects to be grouped are known and the actual group
//			bound is passed. This is the Join Execution.
*/

	virtual BOOL join_group(PBOX_PTR group_bound, PMGPageObjectPtr parent) = 0;

/*
// Leave a group.
// This is called when the group is being dissolved.
*/

	virtual VOID leave_group(VOID) = 0;

/*
// Do a group calc.
// This has additional parameters from the normal calc method.
// Should this have a different name or just overload calc()?
// (I personally prefer a different name because it seems clearer.)
*/

	virtual VOID group_calc(PBOX_PTR current_group_bound,
 									PBOX_PTR org_group_bound,
 									FLAGS group_flags,
 									ROTATION_COMPONENT_PTR group_rotc) = 0;
/*
// Assign method.
// This needs to duplicate all database references.
// This could ALMOST have been the assignment operator. Oh, well...
*/

	virtual ERRORCODE assign(PageObjectRef obj);

/*
// Get the original dimensions of an object.
// This is used primarily for proportional stuff.
// If the object has none, it should return FALSE.
*/

	virtual BOOL original_dims(PPNT_PTR p);

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
// Set whether the object is on or off the page at the moment.
*/

	virtual BOOL IsOnPage(void);

/*
// Add the outline for this object to the passed array.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

	virtual void AddOutline(COutlinePath& Path, PBOX* pClipBox = NULL);

/*
// How does text flow around us?
*/

	virtual long GetFlowAround(void);
	virtual void SetFlowAround(long lFlowAround);
// Web publishing stuff
   HyperlinkRecord*
   GetHyperlink(void);

   virtual DB_RECORD_NUMBER
   GetHyperlinkRecordNumber(void)
   {
      return record.m_HyperlinkRecord;
   }
   
   virtual DB_RECORD_NUMBER
   SetHyperlinkRecord(HyperlinkData* p);

   virtual void
   KillHyperlinkRecord(void); 

   virtual void
   GetHyperlinkData(HyperlinkData* p);

   virtual void
   GetShapeData(PMObjectShapeData &sd) = 0;

   BOOL
   IsInvisible(void);
   
   BOOL
   IsInvisibleHotSpot(void);

   void
   MakeInvisible(void);

	virtual const CString *GetOriginalDataPath()
		{ return NULL; }

/*
 * Animated GIF support handling.
 */
	virtual bool IsAnimatedGIF()
		{ return FALSE; }
	virtual CPGIFAnimator *CreateGIFAnimator()
		{ return NULL; }

/*
 *	Raw Image format support.
 */
	virtual bool IsWebImageFormat()
		{ return FALSE; }
	virtual bool ExportRawData( CString *pstrFileName )
		{ return FALSE; }
	virtual void GetRawImageSize( CSize *psizeImage )
		{}
	virtual bool IsJPEGImage()
		{ return FALSE; }
};

struct RectPageObjectRecord
{
   PBOX		bound;				/* bounding box in global coords. */
   PBOX		unrotated_bound;	/* unrotated bounding box in global coords. */
	PBOX		group_bound;		/* The bound when it joined group */
	FLAGS		flags;				/* Flags when it joined the group */
};

/*
// The class for saving an PMGPageObject's state.
*/

class RectPageObjectState : public PMGPageObjectState
{
public:
	RectPageObjectRecord	m_RRecord;
};

/*
// Normal rectangular page objects.
*/

class RectPageObject : public PMGPageObject
{
	INHERIT(RectPageObject, PMGPageObject)
protected:
	RectPageObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

/* The main record. */

	RectPageObjectRecord rrecord;


/*
// Handle movement helpers.
*/

	VOID stretch_bounds(BOUNDS bounds, BOOL physical);
	virtual VOID calc_bounds(PCOORD_PTR the_corner_x, PCOORD_PTR the_corner_y, PCOORD_PTR opp_corner_x, PCOORD_PTR opp_corner_y, PPNT pos, FLAGS key);

/*
// Location of certain critical handles.
// This information is computed but not stored.
*/

	PPNT		handles[4+1];	/* Critical object handles. */

/* For move_handle, etc. */

	static PPNT near prop_dim;
	static PPNT near real_dim;
	static ANGLE near starting_angle;

/* For drags (including abort). */

	FLAGS select_flags_save;
	PBOX bound_save;
	ANGLE angle_save;
	CDoubleRect m_VisibleRectSave;

/*
// Compute the handle array from the bound.
*/

	VOID compute_handles(VOID);

/*
// Turn a select off if it's on.
*/

	VOID turn_select_off(FLAGS which);

/*
// Restore selects to what they were when one was turned off.
*/

	VOID restore_select(VOID);

/* Routines for dealing with rotated objects. */

	VOID get_handle_point(OBJECT_HANDLE handle, PPNT_PTR pnt);
	VOID get_anchor_point(OBJECT_HANDLE handle, PPNT_PTR pnt);
	VOID move_to_anchor(OBJECT_HANDLE handle, PPNT old_pnt);

	virtual HPEN CreateSelectPen(void);

public:

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

/********************************************/
/* Methods for accessing the object's data. */
/********************************************/

/*
// Get an object's bound.
*/

	virtual PBOX get_bound(VOID);

/*
// Get an object's unrotated bound.
*/

	PBOX get_unrotated_bound(VOID);

/*
// Set an object's unrotated bound.
*/

	void set_unrotated_bound(PBOX Bound);

/*
// Set an object's bound.
*/

	VOID set_bound(PBOX pb);

/*
// Return the clipping region for this object.
*/

	virtual BOOL GetClipRgn(CRgn&, RedisplayContextPtr rc, int nType = CLIP_TYPE_Boundary);

/*
// Return the actual drawing extent of this object (handles cropping).
*/

	virtual PBOX GetDrawBound(void);

/*
// Return the actual drawing extent of this object (handles cropping).
*/

	virtual PBOX GetUnrotatedDrawBound(void);

/*
// Copy the source objects attributes
*/
	virtual void CopyAttributes( PMGPageObjectPtr pObject );

/************************************/
/* Generally useful object methods. */
/************************************/

/*
// Toggle select marks for an object.
*/

	virtual BOOL toggle_selects(RedisplayContextPtr rc, FLAGS which);

/*
// Return whether a point is in an object or not.
*/

	virtual BOOL pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object);

/*
// Return whether a point is over an object's move area or not.
*/

	virtual BOOL pt_on_move_area(PPNT p, RedisplayContextPtr rc);

/*
// Move an object a certain amount.
*/

	virtual PMG_OBJECT_ACTION move_object(DRAG_STATE state, PPNT vector, PPNT current_xy, LPVOID far *data = NULL, BOOL fPanning = FALSE);

/*
// Return whether a box is over a handle or not.
*/

	virtual OBJECT_HANDLE box_on_handle(PBOX_PTR box, RedisplayContextPtr rc);

/*
// Move an object's handle.
*/

	virtual PMG_OBJECT_ACTION move_handle(DRAG_STATE state, PPNT pos, OBJECT_HANDLE handle, FLAGS shift_status, LPVOID far *data = NULL, BOOL fCropping = FALSE);

	// Before a handle move.
	virtual void OnBeginHandleMove(PPNT pos, OBJECT_HANDLE handle, BOOL fCropping);
	// After a handle move (called even if aborted).
	virtual void OnEndHandleMove(OBJECT_HANDLE handle, BOOL fCropping);
	// If move is aborted.
	virtual void OnAbortHandleMove(OBJECT_HANDLE handle, BOOL fCropping);

/*
// Recalc after a size or movement.
*/

	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

/*
// Join a group.
// This is called in two passes:
//		On pass 1, the bound passed is NULL. This is a Join Inquiry. The object
//			should return TRUE if is will be joining the group.
//		On pass 2, all objects to be grouped are known and the actual group
//			bound is passed. This is the Join Execution.
*/

	virtual BOOL join_group(PBOX_PTR group_bound, PMGPageObjectPtr parent);

/*
// Leave a group.
// This is called when the group is being dissolved.
*/

	virtual VOID leave_group(VOID);

/*
// Do a group calc.
// This has additional parameters from the normal calc method.
// Should this have a different name or just overload calc()?
// (I personally prefer a different name because it seems clearer.)
*/

	virtual VOID group_calc(PBOX_PTR current_group_bound,
 									PBOX_PTR org_group_bound,
 									FLAGS group_flags,
 									ROTATION_COMPONENT_PTR group_rotc);

/*
// Assign method.
// This needs to duplicate all database references.
// This could ALMOST have been the assignment operator. Oh, well...
*/

	virtual ERRORCODE assign(PageObjectRef obj);

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
// Add the outline for this object to the passed array.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

	virtual void AddOutline(COutlinePath& Path, PBOX* pClipBox = NULL);

// Toggle the bounds for the object.
	virtual void ToggleBounds(RedisplayContext* rc, POINT* p);
   
   virtual void
   GetShapeData(PMObjectShapeData &sd);

   virtual void
   RotatePBOXToFourPoints(PBOX &Box, PPNT pp [4], double rotation);
};

typedef RectPageObject far *RectPageObjectPtr;
typedef RectPageObject far &RectPageObjectRef;

/*
// Object-related refresh definitions.
*/

typedef struct
{
	PBOX_PTR				pextent;
	REFRESH_TYPE		type;
	PMGPageObjectPtr	object;
	LPRECT				lprExtraPixels;
} REFRESH_STRUCT;

struct REFRESH_EXTENT
{
	PBOX					extent;
	REFRESH_TYPE		refresh_type;
	PMGPageObjectPtr	my_object;

	PMGPageObjectPtr	update_object;
	UpdateStatePtr		update_state;
	BOOL					did_erase;

	// Variables for print preview.
	int					m_nPreviewPage;
	int					m_nPreviewSlot;

	void FreeUpdateState(void)
		{ delete update_state; update_state = NULL; }
};

typedef REFRESH_EXTENT FAR *REFRESH_EXTENT_PTR;

extern UpdateStatePtr error_update_state(UpdateStatePtr ustate, UPDATE_TYPE type = UPDATE_TYPE_Error, ERRORCODE error = ERRORCODE_None);

/*
// In-line functions.
*/

/* PMGPageObject */

inline ERRORCODE PMGPageObject::read_data(StorageDevicePtr device)
	{ return ERRORCODE_None; }
inline ERRORCODE PMGPageObject::write_data(StorageDevicePtr device)
	{ return ERRORCODE_None; }
inline ST_MAN_SIZE PMGPageObject::size_data(StorageDevicePtr device)
	{ return 0; }
inline PMGPageObjectPtr PMGPageObject::next_selected_object()
	{ return next_selected; }
inline PMGPageObjectPtr far * PMGPageObject::next_selected_address()
	{ return &next_selected; }
inline VOID PMGPageObject::set_next_selected_object(PMGPageObjectPtr o)
	{ next_selected = o; }
inline ANGLE PMGPageObject::get_rotation(VOID)
	{ return record.rotc.rotation; }
inline VOID PMGPageObject::set_rotation(ANGLE rot)
	{ record.rotc.rotation = rot; }
inline VOID PMGPageObject::set_panel(SHORT p)
	{ record.panel = p; }
inline SHORT PMGPageObject::get_panel(VOID)
	{ return record.panel; }
inline VOID PMGPageObject::set_layout(DB_RECORD_NUMBER l)
	{ record.layout = l; }
inline DB_RECORD_NUMBER PMGPageObject::get_layout(VOID)
	{ return record.layout; }
inline VOID PMGPageObject::set_select_flags(FLAGS f)
	{ record.select_flags = f; }
inline FLAGS PMGPageObject::get_select_flags(VOID)
	{ return record.select_flags; }
inline VOID PMGPageObject::add_select_flags(FLAGS f)
	{ record.select_flags |= f; }
inline VOID PMGPageObject::remove_select_flags(FLAGS f)
	{ record.select_flags &= ~f; }
inline VOID PMGPageObject::set_flags(FLAGS f)
	{ record.flags = f; }
inline FLAGS PMGPageObject::get_flags(VOID)
	{ return record.flags; }
inline VOID PMGPageObject::add_flags(FLAGS f)
	{ record.flags |= f; }
inline VOID PMGPageObject::remove_flags(FLAGS f)
	{ record.flags &= ~f; }
inline CObjectImage* PMGPageObject::get_object_image()
	{ return object_image; }
inline void PMGPageObject::set_object_image(CObjectImage* objimg)
	{ object_image = objimg; }
inline VOID PMGPageObject::set_refresh_flags(FLAGS f)
	{ refresh_flags = f; }
inline FLAGS PMGPageObject::get_refresh_flags(VOID)
	{ return refresh_flags; }
inline VOID PMGPageObject::add_refresh_flags(FLAGS f)
	{ refresh_flags |= f; }
inline VOID PMGPageObject::remove_refresh_flags(FLAGS f)
	{ refresh_flags &= ~f; }
inline VOID PMGPageObject::set_primary_action(PMG_OBJECT_ACTION action)
	{ 	record.primary_action = action; }
inline VOID PMGPageObject::set_secondary_action(PMG_OBJECT_ACTION action)
	{ 	record.secondary_action = action; }
inline PMGPageObjectPtr PMGPageObject::get_container(VOID)
	{ return container; }
inline VOID PMGPageObject::set_container(PMGPageObjectPtr c)
	{ container = c; }
inline PMGPageObjectPtr PMGPageObject::get_parent(VOID)
	{ return parent; }
inline VOID PMGPageObject::set_parent(PMGPageObjectPtr p)
	{ parent = p; }
inline PMGDatabasePtr PMGPageObject::get_database(VOID)
	{ return (PMGDatabasePtr)database; }
inline UpdateStatePtr PMGPageObject::update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type)
	{ return NULL; }
inline PPNT PMGPageObject::get_origin(VOID)
	{ return record.rotc.origin; }

/* RectPageObject */

inline PBOX RectPageObject::get_unrotated_bound(VOID)
	{ return rrecord.unrotated_bound; }

typedef enum
{
	OBJECT_TYPE_Graphic = 1,
	OBJECT_TYPE_OldFrame,				// Superceded
	OBJECT_TYPE_Pattern,
	OBJECT_TYPE_Background,
	OBJECT_TYPE_Group,
	OBJECT_TYPE_Calendar,
	OBJECT_TYPE_OldCalendarDate,		// Superceded
	OBJECT_TYPE_OldWarpText,			// Superceded
	OBJECT_TYPE_Rectangle,
	OBJECT_TYPE_Ellipse,
	OBJECT_TYPE_Line,
	OBJECT_TYPE_DrawingObject,
	OBJECT_TYPE_OleObject,			// Defined elsewhere
	OBJECT_TYPE_Frame,
	OBJECT_TYPE_CalendarDate,
	OBJECT_TYPE_WarpText,
	OBJECT_TYPE_Table,
	OBJECT_TYPE_Border,
	OBJECT_TYPE_Cell,             // for table cells
	OBJECT_TYPE_Hyperlink,
	OBJECT_TYPE_Serif,				// Serif ("drawing object")
	OBJECT_TYPE_Component,
   // Add new objects here...
   // Guide object
   OBJECT_TYPE_Guides   = 4000, 
   OBJECT_TYPE_PageBackground    // for Background Page 
} PMG_OBJECT_TYPE;

class GraphicObject;
typedef GraphicObject far *GraphicObjectPtr;

class CFrameObject;

class PatternObject;
typedef PatternObject far *PatternObjectPtr;

class BackgroundObject;
typedef BackgroundObject far *BackgroundObjectPtr;

class GroupObject;
typedef GroupObject far *GroupObjectPtr;

class CalendarObject;
typedef CalendarObject far *CalendarObjectPtr;

class CalendarDateObject;
typedef CalendarDateObject far *CalendarDateObjectPtr;

class WarpTextObject;
class RectangleObject;
class EllipseObject;
class LineObject;
class DrawingObject;

class ComponentObject;
typedef ComponentObject far *ComponentObjectPtr;

#endif						/* #ifndef __PMGOBJ_H__ */
