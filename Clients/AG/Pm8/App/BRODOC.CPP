/*
// $Workfile: BRODOC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BRODOC.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 3     6/18/98 7:05p Psasse
// force first page to appear in workspace for templates
// 
// 2     3/10/98 5:23p Jayn
// Master page changes
// 
//    Rev 1.0   14 Aug 1997 15:18:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:14   Fred
// Initial revision.
// 
//    Rev 1.5   03 Apr 1997 14:31:08   Jay
// Resource cleanup
// 
//    Rev 1.4   28 Mar 1997 17:10:40   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.3   01 Sep 1996 13:44:30   Fred
// Always builds album thumbnail from first page
// 
//    Rev 1.2   28 Jul 1996 11:28:38   Jay
// Brochure stuff
// 
//    Rev 1.1   25 Jul 1996 11:16:58   Jay
// More page stuff
// 
//    Rev 1.0   23 Jul 1996 10:39:34   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "brodoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrochureDoc

IMPLEMENT_SERIAL(CBrochureDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The newsletter project panel.
*/

static PANEL_INFO
brochure_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{    0,    0,    0,    0 },
		0,
		2
	}
};

/*
// The brochure project info.
*/

ProjectInfo CBrochureDoc::StandardInfo =
{
	PROJECT_TYPE_Brochure,
	0,
	PORTRAIT,
	sizeof(brochure_panels)/sizeof(brochure_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	brochure_panels
};

CBrochureDoc::CBrochureDoc()
{
	info = StandardInfo;
	info.project_sub_type = 2;		// default to something.
}

CBrochureDoc::~CBrochureDoc()
{
}

/*
// The "new" document handler for brochures.
*/

BOOL CBrochureDoc::OnNewDocument()
{
	// Pass through the sub-type.
	info.project_sub_type = GET_PMWAPP()->last_new_info.subtype;
	return CPmwDoc::OnNewDocument();
}

/*
// Create the two pages for our document.
*/

ERRORCODE CBrochureDoc::CreatePages(void)
{
	DB_RECORD_NUMBER lPage;
	ERRORCODE error;
	// Create front page.
	if ((error = document->new_page(&lPage)) == ERRORCODE_None)
	{
		// Create back page.
		error = document->new_page(&lPage);
	}
	return error;
}

BOOL CBrochureDoc::DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc)
{
	// Preview the first page.
	DWORD dwSavePage = CurrentPageIndex();
	GotoPage(0);
	BOOL fTerminated = CPmwDoc::DrawPreview(re, rc);
	GotoPage(dwSavePage);

	return fTerminated;
}

void CBrochureDoc::GoToFirstPage()
{
	/* Used for templates only */

	ASSERT(NumberOfPages() > 0);
	LoadPage(GetPage(0));
}


BEGIN_MESSAGE_MAP(CBrochureDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CBrochureDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrochureDoc commands
