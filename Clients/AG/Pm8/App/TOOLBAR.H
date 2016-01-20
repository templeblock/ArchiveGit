//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/TOOLBAR.H 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/TOOLBAR.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 1     1/20/99 11:59a Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__
class CRecentFileList;

//cutom messages	
#define UM_BUILD_FONT_LIST WM_USER + 1300
#define UM_SHOW_PREVIEW WM_USER + 1301
#define UM_MOVE_PREVIEW WM_USER + 1302
#define UM_UPDATE_PREVIEW WM_USER + 1303
#define UM_PREVIEW_VISIBLE WM_USER + 1304
#define UM_GET_FONT WM_USER + 1305
#define UM_GET_FONT_SIZE WM_USER + 1306
#define UM_UPDATE_FONTS WM_USER + 1307
#define UM_UPDATE_FONT_MRU WM_USER + 1308
#define UM_HAS_FOCUS WM_USER + 1309

enum
{
	TOOLBAR_FORMAT = 1,
	TOOLBAR_FILE,
	TOOLBAR_EDIT1,
	TOOLBAR_EDIT2,
};

struct SFontComboInfo
{
	CComboBox* pCombo;
	CRecentFileList* pMRUList;
};
#endif
