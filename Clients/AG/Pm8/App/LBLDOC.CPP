/*
// $Workfile: LBLDOC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLDOC.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 8     1/14/99 3:25p Johno
// No more m_fNormalHTML
// 
// 7     1/06/99 6:50p Johno
// Added m_fNormalHRML flag treatment in constructor
// 
// 6     8/24/98 6:02p Jayn
// Labels now preview out to label edge (for rounded case).
// 
// 5     7/15/98 11:49a Johno
// Added GoToFirstPage() and DrawPreview() to CPostCardDoc
// 
// 4     6/12/98 11:09a Cboggio
// added CBizCard::size_to_paper for localized business cards
// 
// 3     6/03/98 1:22p Cboggio
// 
// 2     6/03/98 1:16p Cboggio
// changes to force correct business card size - a better solution is
// required, but this will do for now.
// 
//    Rev 1.2   26 Sep 1997 16:51:24   johno
// Added PostCardDoc
// 
//    Rev 1.1   19 Sep 1997 17:33:56   johno
// Changes for new Post Card project type
// (preliminary)
// 
// 
//    Rev 1.0   14 Aug 1997 15:22:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:00   Fred
// Initial revision.
// 
//    Rev 1.2   03 Apr 1997 14:31:16   Jay
// Resource cleanup
// 
//    Rev 1.1   28 Mar 1997 17:10:46   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.0   14 Mar 1996 13:32:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:08   FRED
// Initial revision.
// 
//    Rev 1.4   06 Oct 1995 11:59:50   JAY
// New print code, etc.
// 
//    Rev 1.3   30 Aug 1995 14:13:40   JAY
// Added PrintPaperInfo() method.
// 
//    Rev 1.2   09 Aug 1995 13:11:24   JAY
// Misc changes.
// 
//    Rev 1.1   31 Jul 1995 10:27:30   JAY
// New projects.
// 
//    Rev 1.0   20 Jul 1995 12:52:20   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "lbldoc.h"
#include "utils.h"
#include "paper.h"
#include "printer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelDoc

IMPLEMENT_SERIAL(CLabelDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The poster project panel.
*/

static PANEL_INFO
label_panels[] =
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

ProjectInfo CLabelDoc::StandardInfo =
{
	PROJECT_TYPE_Label,
	0,
	PORTRAIT,
	sizeof(label_panels)/sizeof(label_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	label_panels
};

CLabelDoc::CLabelDoc()
{
	info = StandardInfo;
}

CLabelDoc::~CLabelDoc()
{
}

int CLabelDoc::DefaultTextSize(int nDefault)
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

BEGIN_MESSAGE_MAP(CLabelDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CLabelDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelDoc commands
/*
// Resize the document to fit on the paper.
*/

void CLabelDoc::size_to_paper(VOID)
{
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	// Get the printer's paper info
	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(get_orientation());
	
	// Get a pointer to our paper info.
	
	CPaperInfo* pLabelInfo = GetPaperInfo();
	
/*
// Remember the original physical margins.
// If they change, we invalidate at least.
*/

	CFixedRect OriginalPhysicalMargins = pLabelInfo->PhysicalMargins();

	CFixedRect PaperBounds = pPaperInfo->PhysicalMargins();

	PaperBounds.Right = pLabelInfo->PaperDims().x - PaperBounds.Right;
	PaperBounds.Bottom = pLabelInfo->PaperDims().y - PaperBounds.Bottom;

	CFixedRect PhysicalMargins;
	
	PhysicalMargins.Left = 0;
	PhysicalMargins.Top = 0;
	PhysicalMargins.Right = 0;
	PhysicalMargins.Bottom = 0;
	
	int nSlots = pLabelInfo->Slots();
	for (int nSlot = 0; nSlot < nSlots; nSlot++)
	{
		CFixedRect SlotBounds = pLabelInfo->SlotBounds(nSlot);
		if (SlotBounds.Left < PaperBounds.Left)
		{
			CFixed NewLeft = PaperBounds.Left - SlotBounds.Left;
			if (NewLeft > PhysicalMargins.Left)
			{
				PhysicalMargins.Left = NewLeft;
			}
		}
		if (SlotBounds.Right > PaperBounds.Right)
		{
			CFixed NewRight = SlotBounds.Right - PaperBounds.Right;
			if (NewRight > PhysicalMargins.Right)
			{
				PhysicalMargins.Right = NewRight;
			}
		}

#if 0
		if (SlotBounds.Top < PaperBounds.Top)
		{
			CFixed NewTop = PaperBounds.Top - SlotBounds.Top;
			if (NewTop > PhysicalMargins.Top)
			{
				PhysicalMargins.Top = NewTop;
			}
		}
		if (SlotBounds.Bottom > PaperBounds.Bottom)
		{
			CFixed NewBottom = SlotBounds.Bottom - PaperBounds.Bottom;
			if (NewBottom > PhysicalMargins.Bottom)
			{
				PhysicalMargins.Bottom = NewBottom;
			}
		}
#endif
	}
	pLabelInfo->SetPhysicalMargins(PhysicalMargins);

	SizeToInternalPaper();

/* In case only the physical margins changed. */

	if (PhysicalMargins != OriginalPhysicalMargins)
	{
	/* Cause all views to refresh. */
		UpdateAllViews(NULL, -1);
	}
}

CPaperInfo* CLabelDoc::PrintPaperInfo(void)
{
	return GET_PMWAPP()->GetPaperInfo(get_orientation());
}

void CLabelDoc::GetPreviewBound(PBOX* pBound)
{
   CPaperInfo* pPaperInfo = GetPaperInfo();

   if (pPaperInfo != NULL)
   {
      CFixedRect Margins = pPaperInfo->Margins();
      CFixedPoint Dims = pPaperInfo->SlotDims();

      CFixedRect Bounds;

      if (get_orientation() == LANDSCAPE)
      {
         GET_PMWAPP()->GetPrinterInfo(LANDSCAPE)->OrientRect(&Margins);
         
         CFixed t = Dims.x;
         Dims.x = Dims.y;
         Dims.y = t;
      }

      Bounds.Left = -Margins.Left;
      Bounds.Top = -Margins.Top;
      Bounds.Right = Bounds.Left + Dims.x;
      Bounds.Bottom  = Bounds.Top + Dims.y;

      pBound->x0 = MulFixed(Bounds.Left, PAGE_RESOLUTION);
      pBound->y0 = MulFixed(Bounds.Top, PAGE_RESOLUTION);
      pBound->x1 = MulFixed(Bounds.Right, PAGE_RESOLUTION);
      pBound->y1 = MulFixed(Bounds.Bottom, PAGE_RESOLUTION);
   }
	else
	{
		get_panel_world(pBound);
	}
#if 0
	PPNT dims = get_dimensions();
	pBound->x0 =
		pBound->y0 = 0;
	pBound->x1 = dims.x;
	pBound->y1 = dims.y;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CBizCardDoc

IMPLEMENT_SERIAL(CBizCardDoc, CLabelDoc, 0 /* schema number*/ )

/*
// The poster project info.
*/

ProjectInfo CBizCardDoc::StandardInfo =
{
	PROJECT_TYPE_BusinessCard,
	0,
	PORTRAIT,
	sizeof(label_panels)/sizeof(label_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	label_panels
};

#ifdef LOCALIZE
VOID CBizCardDoc::size_to_paper(VOID)
{
	// cb June 11 1998 - to force business card size to the correct value
	// TODO find a more permanent, more flexible solution
	// this will have to do for now
	CLabelDoc::size_to_paper();
	SetNewBusinessCardSize();
	AssignPaperInfo(*(GET_PMWAPP()->last_new_info.m_pPaperInfo));
}
#endif


CBizCardDoc::CBizCardDoc()
{
	info = StandardInfo;
}

CBizCardDoc::~CBizCardDoc()
{
}

BEGIN_MESSAGE_MAP(CBizCardDoc, CLabelDoc)
	//{{AFX_MSG_MAP(CBizCardDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBizCardDoc commands

/////////////////////////////////////////////////////////////////////////////
// CPostCardDoc

IMPLEMENT_SERIAL(CPostCardDoc, CLabelDoc, 0 /* schema number*/ )

/*
// The poster project info.
*/

ProjectInfo CPostCardDoc::StandardInfo =
{
	PROJECT_TYPE_PostCard,
	0,
	PORTRAIT,
	sizeof(label_panels)/sizeof(label_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	label_panels
};

CPostCardDoc::CPostCardDoc()
{
	info = StandardInfo;
}

CPostCardDoc::~CPostCardDoc()
{
}

ERRORCODE CPostCardDoc::CreatePages(void)
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

void CPostCardDoc::GoToFirstPage()
{
	/* Used for templates only */

	ASSERT(NumberOfPages() > 0);
	LoadPage(GetPage(0));
}

BOOL CPostCardDoc::DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc)
{
	DWORD dwSavePage = CurrentPageIndex();
	GotoPage(0);
	BOOL fTerminated = CPmwDoc::DrawPreview(re, rc);
	GotoPage(dwSavePage);
	return fTerminated;
}

BEGIN_MESSAGE_MAP(CPostCardDoc, CLabelDoc)
	//{{AFX_MSG_MAP(CPostCardDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostCardDoc commands
