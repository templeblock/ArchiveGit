/*
// $Workfile: webdoc.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/webdoc.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 14    1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 13    12/22/98 10:30a Johno
// Moved most HTML conversion stuff to pmwdoc.*
// 
// 12    6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 11    5/28/98 4:58p Johno
// Changes for HTML compliance groups
// 
// 10    5/26/98 6:55p Johno
// 
// 9     5/22/98 3:46p Johno
// Changes for design checker
// 
// 8     5/20/98 12:14p Johno
// Changes for design checker
// 
// 7     4/03/98 4:42p Johno
// Changes for page links
// 
// 6     3/24/98 5:26p Johno
// Compile update
// 
// 5     3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 4     3/17/98 5:20p Johno
// Compile update
// 
// 3     3/02/98 5:04p Johno
// Compile update
// 
// 2     12/18/97 3:21p Dennis
// Added overide for DefaultFace()
// 
// 1     12/11/97 1:43p Dennis
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "util.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "webview.h"
#include "grpobj.h"
#include "docdib.h"		// For CDIBInfo
#include "bkgobj.h"

#include "webdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#define  DUMP_DOC

/////////////////////////////////////////////////////////////////////////////
// CWebDoc

IMPLEMENT_SERIAL(CWebDoc, CNewsletterDoc, 0 /* schema number*/ )

static PANEL_INFO
s_WebPanels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{    0,    0,    0,    0 },
		0,
		2
	}
};

ProjectInfo CWebDoc::StandardInfo =
{
	PROJECT_TYPE_WebPub,
	0,
	PORTRAIT,
	sizeof(s_WebPanels)/sizeof(s_WebPanels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	s_WebPanels
};

CWebDoc::CWebDoc()
{
	info = StandardInfo;
   
   //m_pBackgroundObject = new CBackgroundObject(OBJECT_TYPE_PageBackground, get_database());
}

CWebDoc::~CWebDoc()
{
/*
   if(m_pBackgroundObject)
      delete m_pBackgroundObject;
*/
}

void CWebDoc::GoToFirstPage()
{
	/* Used for templates only */

	ASSERT(NumberOfPages() > 0);
	LoadPage(GetPage(0));
}

LPCSTR CWebDoc::DefaultFace()
{
	return "Times New Roman";
}

BEGIN_MESSAGE_MAP(CWebDoc, CNewsletterDoc)
	//{{AFX_MSG_MAP(CWebDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebDoc commands


