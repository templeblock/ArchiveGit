// bannview.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pmwprint.h"
#include "bannview.h"
#include "desktop.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBannerView

IMPLEMENT_DYNCREATE(CBannerView, CPmwView)

CBannerView::CBannerView()
{
}

CBannerView::~CBannerView()
{
}

BEGIN_MESSAGE_MAP(CBannerView, CPmwView)
	//{{AFX_MSG_MAP(CBannerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBannerView drawing

void CBannerView::OnDraw(PrintContext* pc)
{
   // Bypass the variant below which is used for previews.
   CPmwView::OnDraw(&pc->m_dcPrint);
}

void CBannerView::OnDraw(CDC* pDC)
{
	if (pDC->IsPrinting())
	{
		if(GetDocument()->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
		{
			BOOL fShowYMarks = (m_pc.m_tile_info.m_y_tile_overlap != 0);

			add_cut_marks(	FALSE,
								fShowYMarks && m_nPreviewPage != 1,
								FALSE,
								fShowYMarks && m_nPreviewPage != m_pc.m_tile_info.m_y_tiles);

		}
		else
		{
			BOOL fShowXMarks = (m_pc.m_tile_info.m_x_tile_overlap != 0);

			add_cut_marks(	fShowXMarks && m_nPreviewPage != 1,
								FALSE,
								fShowXMarks && m_nPreviewPage != m_pc.m_tile_info.m_x_tiles,
								FALSE);
		}
		
		CPmwView::OnDraw(pDC);

		remove_cut_marks();
	}
	else
	{
		CPmwView::OnDraw(pDC);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBannerView message handlers

/*
// Create the edit desktop for a card.
*/

void CBannerView::CreateEditDesktop(void)
{
	m_pDesktop = new CBannerEditDesktop(this);
}

BOOL CBannerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Compute tiles for our preview.
	if (pInfo != NULL && pInfo->m_bPreview)
	{
		m_pc.m_tile_info.compute(GetDocument(), 1.0);
	}

	// And do the base prepare.
	return CPmwView::OnPreparePrinting(pInfo);
}

void CBannerView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo /*=NULL*/)
{
	// Do the base class first.
	CPmwView::OnPrepareDC(pDC, pInfo);

	if(GetDocument()->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
	{
		// Set the source if we are doing a preview.
		if (pInfo != NULL && pInfo->m_bPreview)
		{
			CPmwDoc* pDoc = GetDocument();
			PPNT Dims = pDoc->get_dimensions();

			m_pc.m_source.x0 = 0;
			m_pc.m_source.y0 = (pInfo->m_nCurPage-1)*(m_pc.m_tile_info.m_tile_height-m_pc.m_tile_info.m_y_tile_overlap);
			m_pc.m_source.x1 = m_pc.m_source.x0 + m_pc.m_tile_info.m_tile_width;
			m_pc.m_source.y1 = m_pc.m_source.y0 + m_pc.m_tile_info.m_tile_height;//m_pc.m_tile_info.m_tile_height;

			if (m_pc.m_source.y1 > Dims.y)
			{
				m_pc.m_source.y1 = Dims.y;
			}
		}
	}
	else
	{
		// Set the source if we are doing a preview.
		if (pInfo != NULL && pInfo->m_bPreview)
		{
			CPmwDoc* pDoc = GetDocument();
			PPNT Dims = pDoc->get_dimensions();

			m_pc.m_source.x0 = (pInfo->m_nCurPage-1)*(m_pc.m_tile_info.m_tile_width-m_pc.m_tile_info.m_x_tile_overlap);
			m_pc.m_source.y0 = 0;
			m_pc.m_source.x1 = m_pc.m_source.x0 + m_pc.m_tile_info.m_tile_width;
			m_pc.m_source.y1 = m_pc.m_tile_info.m_tile_height;

			if (m_pc.m_source.x1 > Dims.x)
			{
				m_pc.m_source.x1 = Dims.x;
			}
		}
	}
}

int CBannerView::PrintPreviewPageCount(void)
{
   m_pc.m_tile_info.compute(GetDocument(), 1.0);
	
	if(GetDocument()->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
	{
		ASSERT(m_pc.m_tile_info.m_x_tiles == 1);
		return m_pc.m_tile_info.m_y_tiles;
	}
	else
	{
		ASSERT(m_pc.m_tile_info.m_y_tiles == 1);
		return m_pc.m_tile_info.m_x_tiles;
	}
}

int CBannerView::SetPrintPreviewPage(int nPage)
{
	return (m_nPreviewPage = nPage);
}

void CBannerView::OnInitialUpdate()
{
	CPmwView::OnInitialUpdate();

	OnTextBanner();

	realign_frame(ALIGN_center);
	m_Txp.SetHorizontalAlignment(ALIGN_center);

	if(GetDocument()->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
	{
		m_Txp.InsertString("B\na\nn\nn\ne\nr");//\n");
	}
	else
	{
		m_Txp.InsertString("Banner");
	}

	GetDocument()->resize_project();
}
