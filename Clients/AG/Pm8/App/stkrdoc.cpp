
#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "stkrdoc.h"
#include "util.h"
#include "paper.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStickerDoc

IMPLEMENT_SERIAL(CStickerDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The poster project panel.
*/

static PANEL_INFO
Sticker_panels[] =
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

ProjectInfo CStickerDoc::StandardInfo =
{
	PROJECT_TYPE_Sticker,
	0,
	PORTRAIT,
	sizeof(Sticker_panels)/sizeof(Sticker_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	Sticker_panels
};

CStickerDoc::CStickerDoc()
{
	info = StandardInfo;
}

CStickerDoc::~CStickerDoc()
{
}

int CStickerDoc::DefaultTextSize(int nDefault)
{
	PPNT Dims = get_dimensions();
	PCOORD Min = Dims.x < Dims.y ? Dims.x : Dims.y;
	nDefault = (int)scale_pcoord(Min, 72, PAGE_RESOLUTION);
	if ((nDefault /= 25) < 8)
	{
		nDefault = 8;
	}
	return nDefault;
}

BEGIN_MESSAGE_MAP(CStickerDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CStickerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStickerDoc commands
/*
// Resize the document to fit on the paper.
*/

void CStickerDoc::size_to_paper(VOID)
{
	CFixedPoint paperdims = m_pPaperInfo->PaperDims();
	PPNT new_dims;

	new_dims.x = (PCOORD)MulFixed(	paperdims.x, PAGE_RESOLUTION);
	new_dims.y = (PCOORD)MulFixed( paperdims.y, PAGE_RESOLUTION);
	
	/* Get the paper size. */
	if (get_orientation() == LANDSCAPE)
	{
		PCOORD t = new_dims.x;
		new_dims.x = new_dims.y;
		new_dims.y = t;
	}

	/* Get the document size. */

	PPNT dims = get_dimensions();

	
	PBOX portion;
	portion.x0 = 0;
	portion.y0 = 0;
	portion.x1 = dims.x;
	portion.y1 = dims.y;

	/* See if we need to adjust. */
	if (dims.x != new_dims.x || dims.y != new_dims.y)
	{
		document->set_dimensions(new_dims);
		document->set_paper_width(0);
		document->set_paper_height(0);

		SizeAllPages(dims, new_dims);

		portion.x0 = 0;
		portion.y0 = 0;
		portion.x1 = new_dims.x;
		portion.y1 = new_dims.y;
	}

	set_portion(&portion);
	reset_views();
}

CPaperInfo* CStickerDoc::PrintPaperInfo(void)
{
	return GET_PMWAPP()->GetPaperInfo(get_orientation());
}


BOOL CStickerDoc::OnOpenDocument(const char *pszPathName)
{                    
	if (!CPmwDoc::OnOpenDocument(pszPathName))
		return FALSE;

	LoadPage(GetPage(0));
	size_to_paper();
	return TRUE;
}

BOOL CStickerDoc::CanPageNext(void)
{
	// We allow the user to click off the last page since that
	// brings up the add pages dialog.
	return CurrentPageIndex() < NumberOfPages();
}

void CStickerDoc::DoPageNext(void)
{
	DWORD dwCurrentPage = CurrentPageIndex();
	if (dwCurrentPage < NumberOfPages()-1)
	{
		LoadPage(GetPage(dwCurrentPage+1));
	}
	else
	{
		// Ask the user if they want to add some pages.
		Util::SendWmCommand(AfxGetMainWnd(), ID_ADD_PAGE, NULL, 0);
	}
}