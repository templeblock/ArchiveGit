//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/GUID.CPP 1     3/03/99 6:05p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/GUID.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 7     10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 6     10/02/98 7:10p Jayn
// More content manager changes
// 
// 5     9/25/98 7:05p Jayn
// Beginnings of the content server.
// 
// 4     9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 3     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 1     9/14/98 2:42p Jayn
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <initguid.h>			// This causes the definition to happen.

// Include all GUID definitions here that need initialization.

#include <wpguid.h>				// Web post definitions
#include "iuserset.h"			// IUserSettings (IID_IUserSettings)
#include "idrvmgr.h"				// IDriveManager (IID_IDriveManager)
#include "ipathmgr.h"			// IPathManager (IID_IPathManager)
#include "icollmgr.h"			// ICollectionManager (IID_ICollectionManager)
#include "icontsrv.h"			// IContentServer (IID_IContentServer)
#include "imedntfy.h"			// IMediaPromptNotify (IID_IMediaPromptNotify)
#include "icontmgr.h"			// IContentManager (IID_IContentManager)
#include "icolldrw.h"			// ICollectionDraw (IID_ICollectionDraw)
#include "iconnmgr.h"			// IConnectionManager (IID_IConnectionManager)
