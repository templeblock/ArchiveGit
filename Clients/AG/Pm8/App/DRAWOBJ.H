/*
// $Header: /PM8/App/DRAWOBJ.H 1     3/03/99 6:05p Gbeddow $
//
// $Log: /PM8/App/DRAWOBJ.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 4     5/14/98 6:19p Johno
// Added special drawing code to DrawingObject::update() for invisible hot
// spots
// 
// 3     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 2     4/29/98 5:58p Rlovejoy
// Modified draw functions to take fill objects.
// 
//    Rev 1.0   14 Aug 1997 15:20:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:38   Fred
// Initial revision.
// 
//    Rev 1.7   23 May 1997 17:08:20   Fred
// New effects
// 
//    Rev 1.6   04 Mar 1997 17:04:02   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.5   26 Feb 1997 10:57:32   Jay
// Cropping; GetClipRgn() changes
// 
//    Rev 1.4   29 Jan 1997 14:18:18   Jay
// Outline path and PtInPath
// 
//    Rev 1.3   24 Jan 1997 12:39:22   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.2   23 Jan 1997 11:05:46   Jay
// New popup palettes
// 
//    Rev 1.1   17 Jan 1997 17:03:16   Jay
// New select for drawing objects.
// 
//    Rev 1.0   14 Mar 1996 13:45:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:42   FRED
// Initial revision.
// 
//    Rev 1.7   14 Sep 1995 09:32:22   JAY
// 32-bit changes
// 
//    Rev 1.6   01 Aug 1995 12:59:38   JAY
// Added correct name.
// 
//    Rev 1.5   19 May 1995 09:24:34   FRED
// Added bounds to path record
// 
//    Rev 1.4   17 May 1995 16:58:16   FRED
// Draw object shadows
// 
//    Rev 1.3   13 May 1995 11:14:38   JAY
//  
// 
//    Rev 1.2   12 May 1995 10:33:04   FRED
// Added shadow color.
// 
// 
//    Rev 1.1   10 May 1995 13:14:40   JAY
// Added Record() access function.
// 
//    Rev 1.0   04 May 1995 17:15:50   JAY
// Initial revision.
*/

class DrawingObject;
class DrawingObjectState;

#ifndef __DRAWOBJ_H__
#define __DRAWOBJ_H__

class COutlinePath;

struct DrawingObjectRecord
{
	COLOR m_clOldFillColor;
	SHORT m_fOldFillColorExists;			// BOOL

	SHORT m_nOldLineWidth;					// int

	COLOR m_clOldLineColor;
	SHORT m_fOldLineColorExists;			// BOOL

	COLOR m_clOldShadowColor;
	SHORT m_fOldShadowColorExists;		// BOOL

	DB_RECORD_NUMBER m_PathRecord;

	SHORT m_nEffectsVersion;					// 0 = PMG 3.0
														// 1 = PMG 4.0
														// 2 = PMG 5.0
	// These are obsolete in V2. They have moved outside this structure
	// to allow for future extensibility, but they need to be here for
	// backward support (i.e. we have to live with them forever).
	FillFormatV1 m_OldFill;
	OutlineFormat m_OldOutline;
	ShadowFormat m_OldShadow;
};

class DrawingObjectState : public RectPageObjectState
{
public:
	DrawingObjectState();
	virtual ~DrawingObjectState();
	DrawingObjectRecord	m_DRecord;
	FillFormatV2 m_Fill;
	OutlineFormat m_Outline;
	ShadowFormat m_Shadow;
};

class DrawingObject : public RectPageObject
{
	INHERIT(DrawingObject, RectPageObject)
protected:
	DrawingObjectRecord m_DRecord;
	FillFormatV2 m_Fill;
	OutlineFormat m_Outline;
	ShadowFormat m_Shadow;

/*
// Protected constructor.
// Call the create method.
*/

	DrawingObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

public:

	~DrawingObject(void);

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, void* creation_data, PageObjectPtr far *object);

/*
// Toggle the bounds for the object.
*/

	virtual void ToggleBounds(RedisplayContext* rc, POINT* p);

/*
// Update ourselves into a redisplay context.
*/

	virtual UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

/*
// Return the refresh bounds for this object.
*/

	virtual void get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

/*
// Return the clipping region for this object.
*/

	virtual BOOL GetClipRgn(CRgn&, RedisplayContextPtr rc, int nType = CLIP_TYPE_Boundary);

/*
// Get the path for this object.
*/

	virtual BOOL GetPath(COutlinePath& Path, const CRect& crObject, const CRect* pcrOpenObject = NULL);

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
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

	ST_MAN_SIZE size_data(StorageDevicePtr device);

/*
// Get an object's name.
*/

	virtual BOOL get_name(LPSTR buffer, SHORT size, SHORT max_width);

/*
// Assign an object to this.
*/

	virtual ERRORCODE assign(PageObjectRef sobject);

/*
// Get the original dimensions of this drawing object.
*/

	virtual BOOL original_dims(PPNT_PTR p);
	
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
// Return a pointer to the drawing object record.
*/

	DrawingObjectRecord& Record(void)
		{ return m_DRecord; }

	FillFormatV2& GetFill(void)
		{ return m_Fill; }
	OutlineFormat& GetOutline(void)
		{ return m_Outline; }
	ShadowFormat& GetShadow(void)
		{ return m_Shadow; }

/*
// Set a new outline path, freeing any old one.
*/

	void set_new_outline_path(DB_RECORD_NUMBER l);
	
/*
// Figure out the X and Y shadow offsets.
*/
	
	virtual void GetShadowOffsets(PCOORD* pX, PCOORD* pY);
	virtual void GetOutlineWidth(PCOORD* pX, PCOORD* pY);
};

#endif					// #ifndef __DRAWOBJ_H__
