/*
// $Workfile: SIGNDOC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SIGNDOC.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 4     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 3     8/05/98 10:57a Mwilson
// added a goto first page for multipage t-shirts
// 
// 2     4/17/98 4:47p Fredf
// T-Shirts
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "signdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPosterDoc

IMPLEMENT_SERIAL(CPosterDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The poster project panel.
*/

PANEL_INFO poster_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{    0,    0,    0,    0 },
		0,
		2
	}
};

/*
// The poster project info.
*/

ProjectInfo CPosterDoc::StandardInfo =
{
	PROJECT_TYPE_Poster,
	0,
	PORTRAIT,
	sizeof(poster_panels)/sizeof(poster_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	poster_panels
};

CPosterDoc::CPosterDoc()
{
	info = StandardInfo;
}

CPosterDoc::~CPosterDoc()
{
}

BEGIN_MESSAGE_MAP(CPosterDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CPosterDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosterDoc commands

/////////////////////////////////////////////////////////////////////////////
// CCertificateDoc

IMPLEMENT_SERIAL(CCertificateDoc, CPosterDoc, 0 /* schema number*/ )

/*
// The certificate project info.
*/

ProjectInfo CCertificateDoc::StandardInfo =
{
	PROJECT_TYPE_Certificate,
	0,
	PORTRAIT,
	sizeof(poster_panels)/sizeof(poster_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	poster_panels
};

CCertificateDoc::CCertificateDoc()
{
	info = StandardInfo;
}

CCertificateDoc::~CCertificateDoc()
{
}

BEGIN_MESSAGE_MAP(CCertificateDoc, CPosterDoc)
	//{{AFX_MSG_MAP(CCertificateDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCertificateDoc commands

/////////////////////////////////////////////////////////////////////////////
// CFaxCoverDoc

IMPLEMENT_SERIAL(CFaxCoverDoc, CPosterDoc, 0 /* schema number*/ )

/*
// The certificate project info.
*/

ProjectInfo CFaxCoverDoc::StandardInfo =
{
	PROJECT_TYPE_FaxCover,
	0,
	PORTRAIT,
	sizeof(poster_panels)/sizeof(poster_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	poster_panels
};

CFaxCoverDoc::CFaxCoverDoc()
{
	info = StandardInfo;
}

CFaxCoverDoc::~CFaxCoverDoc()
{
}

BEGIN_MESSAGE_MAP(CFaxCoverDoc, CPosterDoc)
	//{{AFX_MSG_MAP(CFaxCoverDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaxCoverDoc commands

/////////////////////////////////////////////////////////////////////////////
// CStationeryDoc

IMPLEMENT_SERIAL(CStationeryDoc, CPosterDoc, 0 /* schema number*/ )

/*
// The certificate project info.
*/

ProjectInfo CStationeryDoc::StandardInfo =
{
	PROJECT_TYPE_Stationery,
	0,
	PORTRAIT,
	sizeof(poster_panels)/sizeof(poster_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	poster_panels
};

CStationeryDoc::CStationeryDoc()
{
	info = StandardInfo;
}

CStationeryDoc::~CStationeryDoc()
{
}

BEGIN_MESSAGE_MAP(CStationeryDoc, CPosterDoc)
	//{{AFX_MSG_MAP(CStationeryDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationeryDoc commands

