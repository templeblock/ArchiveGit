/*
// $Header: /PM8/App/CompObj.h 2     3/08/99 1:15p Rgrenfel $
//
// Component object definitions.
//
// $Log: /PM8/App/CompObj.h $
// 
// 2     3/08/99 1:15p Rgrenfel
// Added virtual over-rides to the record ID get/set methods.
// 
// 23    3/05/99 6:50p Rgrenfel
// Added a virtual override to the record ID get/set methods.
// 
// 22    2/26/99 8:40p Rgrenfel
// Supported the mechanism to get the original object's data path and
// added support for CGM graphic components.
// 
// 21    2/25/99 3:27p Lwilson
// Changed component creation to use a pointer to a CCreationData class or
// variant for in place of an RComponentDocument pointer.
// 
// 20    2/11/99 10:09a Lwilson
// Added GetContextMenuIndex() to determine which context menu to display
// for the embedded component.
// 
// 19    2/10/99 5:06p Rgrenfel
// Added protected method to allow the detachment of the embedded
// component document.
// 
// 18    2/10/99 1:29p Lwilson
// Added changed_image() override to re-calc the component bounds and to
// invalidate the render cache.  Also, updated the calc() method to call
// the component's ClearFlippedFlags() to keep the flipped flags in sync.
// 
// 17    2/10/99 12:11p Lwilson
// Updated SetComponentData to accept flags to determine whether or not
// the current component's attributes and/or size should be kept.
// 
// 16    2/04/99 5:57p Rgrenfel
// Allowed the detection that the raw object data was in JPEG format.
// 
// 15    2/04/99 11:58a Rgrenfel
// Added methods for determining the raw image size and to allow the
// export of the raw image.
// 
// 14    2/01/99 4:35p Lwilson
// Added CreateFromObject method to support converting PM objects into
// component objects.  Also implemented the conversion for drawing,
// ellipse, and rectangle objects.
// 
// 13    1/27/99 1:59p Lwilson
// Added shadow support and tracker feedback rendering.
// 
// 12    1/25/99 3:44p Rgrenfell
// Added support for requesting if the component is an animated GIF image.
// 
// 11    1/25/99 9:58a Lwilson
// Initial path component support
// 
// 10    1/20/99 4:32p Jfleischhauer
// added fResize flag to ComponentObject::SetComponentData()
 * 
 * 9     1/19/99 2:57p Lwilson
 * Overwrote calc() method to set the components bounding box when ever
 * it's position or size changes.
// 
// 8     1/13/99 4:59p Cschende
// overrode AddOutline to support giving a non-rectangular shape used for
// irregular text wrap
// 
// 7     1/11/99 1:32p Gbeddow
// support for reading/writing Print Shop square, row & column vector
// graphics in Pretzel collection files
// 
// 6     12/17/98 8:57a Rgrenfel
// Changed the name for getting an image document to a generic GetDocument
// call.
// 
// 5     12/16/98 6:22p Rgrenfel
// Added a method to allow a component object to create an image document
// from a Graphic Creation structure.
// 
// 4     12/15/98 3:37p Lwilson
// Initial support for PS component editing.
// 
// 3     12/14/98 1:06p Lwilson
// Added default sizing support for PS components.
// 
// 2     12/10/98 12:56p Lwilson
// Implemented is_opaque() and IsEmpty() member functions.  Fixed
// selection handle update problem, as well as updated drawing surface to
// account for printing.
// 
// 1     12/08/98 5:13p Lwilson
// Support for Print Shop integration.
// 
*/

#ifndef __COMPOBJ_H__
#define __COMPOBJ_H__

#include "PMGObj.h"
#include "GraphicInterface.h"
#include "GrafRec.h"				// Defines GRAPHIC_TYPE

const WORD kCopyAttributes = 0x0001;
const WORD kResetSize      = 0x0002;

class CPmwDoc;
class ComponentRecord;
class RComponentView;
class RComponentDocument;
class CPGIFAnimator;
class CCreationData;

struct GRAPHIC_CREATE_STRUCT;

class CPComponentToContextMenuMap : public CMapStringToPtr
{
public:

										CPComponentToContextMenuMap();
};

struct ComponentObjectRecord
{
	DB_RECORD_NUMBER	recno; /* record of component data */
};

class ComponentObjectState : public RectPageObjectState
{
public:

	virtual ~ComponentObjectState();
	ComponentObjectRecord	comp_record;
};

FARCLASS ComponentObject : public RectPageObject
{
	INHERIT(ComponentObject, RectPageObject)

	ComponentObjectRecord comp_record;

//	BOOL ImageChanged(ComponentObjectState* pState);

protected:

										// Protected constructor.  Call the create method.
										ComponentObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

public:

										~ComponentObject();

	// The creator for this type of object. Note that this is static so that we can register it with the database.
	static ERRORCODE				create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);
	virtual VOID					destroy(VOID);

	virtual void SetRecordID( DB_RECORD_NUMBER nRecNum )
		{ comp_record.recno = nRecNum; }
	virtual DB_RECORD_NUMBER GetRecordID()
		{ return comp_record.recno; }

	static PMGPageObject*		CreateFromObject( PMGPageObject* pObject );
	static RComponentDocument *CreateGraphicDocument( GRAPHIC_CREATE_STRUCT *pGcs, GRAPHIC_TYPE graphic_type );
	static RComponentDocument *CreateImageDocument( GRAPHIC_CREATE_STRUCT *pGcs );

	ERRORCODE						read_data(StorageDevicePtr device);
	ERRORCODE						write_data(StorageDevicePtr device);
	ST_MAN_SIZE						size_data(StorageDevicePtr device);

	virtual PageObjectState*	CreateState(void);
	virtual void					SaveState(PageObjectState* pState);
	virtual void					RestoreState(PageObjectState* pState);
	virtual void					GetObjectProperties(CObjectProperties& Properties);
	virtual BOOL					SetObjectProperties(CObjectProperties& Properties);


	virtual ERRORCODE				assign(PageObjectRef sobject);
	virtual VOID					calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);
	virtual VOID					changed_image(VOID);
	virtual void					get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc);
   virtual BOOL					IsEmpty();
	virtual BOOL					is_opaque(RedisplayContextPtr rc);
	virtual BOOL					original_dims(PPNT_PTR p);
	UpdateStatePtr					update( RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type );

	virtual void					AddOutline(COutlinePath& Path, PBOX* pClipBox = NULL);
	virtual void					ToggleBounds(RedisplayContext* rc, POINT* p);

	void								SetDefaultSize( PBOX_PTR pPanelSize );
	ERRORCODE						SetComponentData( RComponentDocument* pComponentDoc, WORD wFlags );
	ERRORCODE						SetComponentData( CCreationData* pData, WORD wFlags );

	YComponentType					GetComponentType();
	RComponentView*				GetComponentView();
	SHORT								GetContextMenuIndex();

	virtual const CString *		GetOriginalDataPath();

/*
 * Animated GIF support handling.
 */
	virtual bool 					IsAnimatedGIF();
	virtual CPGIFAnimator *		CreateGIFAnimator();

/*
 *	Raw Image format support.
 */
	virtual bool					IsWebImageFormat();
	virtual bool					ExportRawData( CString *pstrFileName );
	virtual void					GetRawImageSize( CSize *psizeImage );
	virtual bool 					IsJPEGImage();

public:

	static ERRORCODE				PasteComponent( COleDataObject* pDataObject, CPmwDoc* pDoc, PPNT& pastePoint );
	static RComponentDocument*	CreateNewComponent( const YComponentType& type, YComponentBoundingRect rBoundingRect );


/*
// Assign an object to this.
*/


/*
// Get the original dimensions of this graphic.
*/


/*
// Get an object's name.
*/

//	virtual BOOL get_name(LPSTR buffer, SHORT size, SHORT max_width);


	// Get the object's time stamp.
//	virtual CTimeStamp GetTimeStamp(void);
//	virtual BOOL UpdateTimeStamp(void);


/*
// Get the properties for this object.
*/

//	virtual void GetObjectProperties(CObjectProperties& Properties);

/*
// Set the properties for this object.
*/

//	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

protected:

	CRect							GetClipBox( RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip );
	ComponentRecord*			LockComponentRecord();
	RRealSize					GetDefaultSize( PBOX_PTR pPanelSize );

	// Extracts the component document from the object's control.  Used for
	// Grouped Component Object dialogs when the "More Graphics" button is processed.
	// We allow this to avoid having to duplicate image memory, for this reason
	// the RCommonGroupedObjectDialog dialog has been made a friend class.
	friend class RCommonGroupedObjectDialog;
	RComponentDocument*			DetachComponentDocument();

private:

	static YComponentType GetYComponentType( GRAPHIC_TYPE graphic_type );
	static RGraphicInterface::EGraphicType GetEGraphicType( GRAPHIC_TYPE graphic_type );

};


#endif // __COMPOBJ_H__
