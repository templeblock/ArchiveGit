//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "imgutil.h"
#include "mgxframe.h"
#include "merge.h"
#include "appdata.h"

#define	UT_DATA				0x00000001L
#define	UT_ALPHA			   0x00000002L
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
#define	UT_COLORMAP			0x00001000L
#define  UT_UNDOTEXTDATA   0x00002000L
#define  UT_GROUPOBJECTS   0x00004000L
#define  UT_LOCKOBJECTS    0x00008000L

#define UT_EDITTEXT  (UT_NEWDATA|UT_NEWALPHA|UT_UNDOTEXTDATA|UT_OBJECTRECT)
#define UT_DATAUNDO1	(UT_DATA|UT_NEWDATA|UT_OBJECTRECT|UT_RESOLUTION|UT_CREATEOBJECTS|UT_GROUPOBJECTS)
#define UT_DATAUNDO2	(UT_DELETEOBJECTS|UT_SELECTION|UT_MOVEOBJECTS|UT_COLORMAP|UT_UNDOTEXTDATA|UT_LOCKOBJECTS)
#define UT_DATAUNDO  (UT_DATAUNDO1|UT_DATAUNDO2)
#define UT_ALPHAUNDO	(UT_ALPHA|UT_NEWALPHA|UT_CREATEMASK|UT_DELETEMASK)

LPOBJECT ObjCreateFromFrame(STORAGE_TYPE StorageType, LPFRAME lpFrame,
						LPMASK lpAlpha, LPRECT lpRect, BOOL NoUndo);
LPOBJECT ObjCreateFromPixmap(STORAGE_TYPE StorageType, LPPIXMAP lpPixmap,
						LPMASK lpAlpha, LPRECT lpRect, BOOL NoUndo);
void ObjFreeUp(LPOBJECT lpObject);
LPMASK ObjGetAlpha(LPOBJECT lpObject);
void ObjSetAlpha(LPOBJECT lpObject, LPMASK lpAlpha);
LPFRAME ObjGetEditFrame(LPOBJECT lpObject);
LPFRAME ObjGetUndoFrame(LPOBJECT lpObject);
void ObjAddTail(LPOBJECTLIST lpObjList, LPOBJECT lpObject);
void ObjAddListTail(LPOBJECTLIST lpObjList, LPOBJECT lpObject);
void ObjDeleteObjects(LPOBJECTLIST lpObjList, STORAGE_TYPE StorageType);
BOOL ObjDeleteObject(LPOBJECTLIST lpObjList, LPOBJECT lpObject);
void ObjLinkObject(LPOBJECTLIST lpObjList, LPOBJECT lpLinkObject, LPOBJECT lpObject, BOOL fAfter);
void ObjUnlinkObject(LPOBJECTLIST lpObjList, LPOBJECT lpObject);
int ObjCountObjects(LPOBJECTLIST lpObjList);
BOOL ObjSelectObjects(LPOBJECTLIST lpObjList, LPRECT lpRect, BOOL fSelect, BOOL fBottomUp, BOOL fMultiSelect);
BOOL ObjDeselectAll(LPOBJECTLIST lpObjList);
void ObjSelectObject(LPOBJECT lpObject, BOOL fSelect);
LPOBJECT ObjGetSelectedObject(LPOBJECTLIST lpObjList, LPOBJECT lpObject, BOOL fBottomUp);
LPOBJECT ObjGetNextObject(LPOBJECTLIST lpObjList, LPOBJECT lpObject, BOOL fBottomUp);
BOOL IsGroupLocked( WORD wGroupID );

#endif // _OBJECT_H_

