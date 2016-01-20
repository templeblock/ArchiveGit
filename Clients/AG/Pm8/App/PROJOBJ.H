/*
// $Workfile: PROJOBJ.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROJOBJ.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:36   Fred
// Initial revision.
// 
//    Rev 1.2   24 Jul 1997 17:35:38   Jay
// Fix to allow file-handle-limited 16-bit version to work.
// 
//    Rev 1.1   05 Jun 1997 17:11:54   doberthur
// 
// 
//    Rev 1.0   30 May 1997 18:34:52   doberthur
// Initial revision.
// 
*/

#ifndef __PROJOBJ_H__
#define __PROJOBJ_H__

typedef struct
{
	PBOX Bound;
	PPNT OriginalSize;
   LPCSTR file_name;
	LPCSTR friendly_name;
	CPmwDoc* m_pItem;     // project
} PROJECTOBJECT_CREATE_STRUCT;

struct ProjectObjectRecord
{
	DWORD		m_dwItemNumber;	/* Serial number within this document. */
	PPNT		m_OriginalSize;
};

class ProjectObject : public RectPageObject
{
	INHERIT(ProjectObject, RectPageObject)
protected:

   CString m_file_name;
	CString m_friendly_name;
   PBOX m_Bound;

	CPmwDoc* m_pItem;    // project

	BOOL OpenDocument(void);
	void CloseDocument(void);

/*
// Private constructor.
// Call the create method.
*/
public:
	ProjectObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);
   ~ProjectObject();
public:

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	ERRORCODE create(VOIDPTR creation_data, CPmwDoc *pDoc);

/*
//  Get the friendly name
*/
   BOOL get_name(LPSTR buffer, SHORT size, SHORT max_width);

/*
// Update an object into a redisplay context.
*/

	UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

/*
// Recalc after a size or movement.
*/

	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

/*
// Get the original dimensions of an object.
// This is used primarily for proportional stuff.
// If the object has none, it should return FALSE.
*/

	virtual BOOL original_dims(PPNT_PTR p);
};


#endif					// #ifndef __ProjectOBJ_H__
