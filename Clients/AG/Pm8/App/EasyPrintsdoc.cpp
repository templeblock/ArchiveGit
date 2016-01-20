/*
// $Workfile: EasyPrintsdoc.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
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
// $Log: /PM8/App/EasyPrintsdoc.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 7     2/15/99 1:32p Psasse
// m_flScaleFactor, DrawPreview() and PreviewBound() for Thumbs
// 
// 6     2/01/99 1:01a Psasse
// Improved PhotoProjects printing
// 
// 5     1/30/99 11:31p Psasse
// Updated and fairly stable EasyPrints code
// 
// 4     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     12/23/98 5:43p Psasse
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "lbldoc.h"
#include "photoprj.h"
#include "easyprintsdoc.h"
#include "lblview.h"
#include "easyprintsview.h"
#include "printer.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define NUM_SUBSECTIONS 3
/////////////////////////////////////////////////////////////////////////////
// CEasyPrintsDoc

IMPLEMENT_SERIAL(CEasyPrintsDoc, CLabelDoc, 0 /* schema number*/ )

/*
// The EasyPrints project panel.
*/

PANEL_INFO EasyPrints_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{    0,    0,    0,    0 },
		0,
		2
	}
};

/*
// The EasyPrints project info.
*/

ProjectInfo CEasyPrintsDoc::StandardInfo =
{
	PROJECT_TYPE_PhotoProjects,
	0,
	PORTRAIT,
	sizeof(EasyPrints_panels)/sizeof(EasyPrints_panels[0]),
	IDS_POSTER_FRAME_LAYOUT,
	IDS_POSTER_GRAPHIC_LAYOUT,
	IDS_POSTER_DUPLICATE_LAYOUT,
	EasyPrints_panels
};

CEasyPrintsDoc::CEasyPrintsDoc()
{
	info = StandardInfo;
	m_nCurrentSubArea = 0;
	m_nType = 0;
	m_pPhotoProjectInfo = NULL;
	m_pPhotoProjectInfo = new CPhotoPrjData;
	m_flScaleFactor = 1;

}

CEasyPrintsDoc::~CEasyPrintsDoc()
{
	if(m_pPhotoProjectInfo)
		delete m_pPhotoProjectInfo;

}

BEGIN_MESSAGE_MAP(CEasyPrintsDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CEasyPrintsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CEasyPrintsDoc::OnNewDocument()
{
	CPmwApp* pApp = GET_PMWAPP();

	CPaperInfo* pPaper = pApp->last_new_info.m_pPaperInfo;
	*(CPaperInfo*)m_pPhotoProjectInfo->m_pSubAreaData[0] = *pPaper;
	ASSERT(pPaper != NULL);
	{
		info.project_sub_type = pApp->last_new_info.subtype;
		m_pPhotoProjectInfo->m_nType = pApp->last_new_info.subtype; 
		FillInSubAreaInfo();		
	}

	if (!CPmwDoc::OnNewDocument())
		return FALSE;

	DocumentRecord()->SetProjectSubtype(pApp->last_new_info.subtype);
	
	return TRUE;

}

BOOL CEasyPrintsDoc::OnOpenDocument( LPCTSTR lpszPathName )
{
	if (!CPmwDoc::OnOpenDocument(lpszPathName))
		return FALSE;

	m_pPhotoProjectInfo->m_nType = document->GetProjectSubtype();
	FillInSubAreaInfo();		

	return TRUE;
}

void CEasyPrintsDoc::FillInSubAreaInfo(void)
{
	m_List.ReadList("PHOTOPRJ.DAT");
	
	CPmwApp* pApp = GET_PMWAPP();

	CPhotoPrjData* pPhotoPrj = (CPhotoPrjData*)m_List.PhotoProject(m_pPhotoProjectInfo->m_nType);
	if(pPhotoPrj != NULL)
	{
		for(int y = 0; y < NUM_SUBSECTIONS; y++)
		{
			*m_pPhotoProjectInfo->m_pSubAreaData[y] = *pPhotoPrj->m_pSubAreaData[y];
		}
	}
}


CPaperInfo* CEasyPrintsDoc::GetPaperInfo()
{
	*(CPaperInfo*)m_pPhotoProjectInfo->m_pSubAreaData[0] = *m_pPaperInfo;

	return m_pPhotoProjectInfo->m_pSubAreaData[m_nCurrentSubArea];
}

CPaperInfo* CEasyPrintsDoc::GetPaperInfo(int x)
{
	if(m_pPhotoProjectInfo)
		return m_pPhotoProjectInfo->m_pSubAreaData[x];
	else
		return CPmwDoc::GetPaperInfo();
}

int CEasyPrintsDoc::GetTotalSlots (void)
{
	int nTotal = 0;

	for (int x = 0; x < NUM_SUBSECTIONS; x++)
	{
		nTotal += m_pPhotoProjectInfo->m_pSubAreaData[x]->Slots();
	}

	return nTotal;
}

void CEasyPrintsDoc::SizeToInternalPaper(void)
{
	PPNT new_dims = GetPaperInfo()->GetProjectDims();

/* Get the paper size. */

	if (get_orientation() == LANDSCAPE)
	{
		PCOORD t = new_dims.x;
		new_dims.x = new_dims.y;
		new_dims.y = t;
	}

/* Get the document size. */

	PPNT dims = get_dimensions();

/* Banners are assumed to be OK in the horizontal direction. */

	if (get_project_type() == PROJECT_TYPE_Banner)
	{
	/* Ignore width. */
		new_dims.x = dims.x;
	}

/* See if we need to adjust. */

	if(new_dims.x != 0 && new_dims.y != 0 && dims.x != 0 && dims.y != 0)
	{
		if (dims.x != new_dims.x || dims.y != new_dims.y)
		{
			document->set_dimensions(new_dims);
			document->set_paper_width(0);
			document->set_paper_height(0);

			SizeAllPages(dims, new_dims);

		/* We need to reshow the current view. */

			set_portion();
			reset_views();
		}
	}
}

BOOL CEasyPrintsDoc::DrawPreview(REFRESH_EXTENT_PTR re, RedisplayContext* rc)
{

/*
// Save the scaled_source x0 and y0 in the printer rc since we will be
// changing them.
*/

	// Save the in extent since it gets change during interrupt.
	PBOX InExtent = re->extent;

	PBOX SourcePbox = re->extent;

	float flScaleFactor = m_flScaleFactor;

	int x_resolution = rc->x_resolution;
	int y_resolution = rc->y_resolution;
	
	PPNT SourceOrigin;
	SourceOrigin.x = rc->scaled_source_x0;
	SourceOrigin.y = rc->scaled_source_y0;

	/* Get the App and its info. */
	CPmwApp* pApp = GET_PMWAPP();
	//CPmwDoc* pDoc = GetDocument();
	PPNT ppnt = get_dimensions();
	int nPrintOrientation = get_orientation();
	BOOL fLandscape = (nPrintOrientation == LANDSCAPE);

	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(nPrintOrientation);

	CFixedRect PaperPhysicalMargins = pPaperInfo->PhysicalMargins();

	for (int x = 0; x < NUM_SUBSECTIONS; x++)
	{

	/* Get the document to work with. */
		SetCurrentSubArea(x);

		/* Get the paper info. */
		CPaperInfo* pLabelInfo = GetPaperInfo();
		
		if(x > 0)
		{
			//change the objects position
			if(pLabelInfo->Orientation() != 3) //CPaperInfo::PAPER_Landscape)
			  ResetPaperInfo(pLabelInfo);
		}

		/* Get the paper info. */
		pLabelInfo = GetPaperInfo();

		if((pLabelInfo->SlotsAcross() != 0) && 
			(pLabelInfo->SlotsDown() != 0))
		{
			CFixedRect Margins = pLabelInfo->Margins();

		/* Compute the number of slots we need to print. */
			int nSlot = re->update_object == NULL ? 0 : re->m_nPreviewSlot;
			int nSlots = pLabelInfo->Slots();

		/* Run through all the slots. */
			while (nSlot < nSlots)
			{
			/*
			// Setup the parameters for this slot so that we will draw in the correct
			// spot on the page.
			*/

				ASSERT(CurrentPageIndex() == 0 || CurrentPageIndex() == 1);
				CFixedRect SlotBounds = pLabelInfo->SlotBounds(nSlot);

				CFixed lLeft = SlotBounds.Left;
				CFixed lTop = SlotBounds.Top;
				CFixed lWidth = SlotBounds.Width();
				CFixed lHeight = SlotBounds.Height();

				CFixedPoint Org;
				
				Org.x = Margins.Left-PaperPhysicalMargins.Left;
				Org.y = Margins.Top-PaperPhysicalMargins.Top;
			
				if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
				{
					SlotBounds.Left += ((lWidth - lHeight)/2);
					SlotBounds.Top += ((lHeight - lWidth)/2);
				}

				PPNT Delta;
				Delta.x = MulFixed(SlotBounds.Left + Org.x, PAGE_RESOLUTION);
				Delta.y = MulFixed(SlotBounds.Top + Org.y, PAGE_RESOLUTION);

				rc->source_pbox.x0 = SourcePbox.x0 - Delta.x;
				rc->source_pbox.y0 = SourcePbox.y0 - Delta.y;
				rc->source_pbox.x1 = SourcePbox.x1 - Delta.x;
				rc->source_pbox.y1 = SourcePbox.y1 - Delta.y;

				rc->scaled_source_x0 = /*SourceOrigin.x*/
										- Delta.x*rc->x_resolution;
				rc->scaled_source_y0 = /*SourceOrigin.y*/
										- Delta.y*rc->y_resolution;

				if (IsBoxOnBox(&rc->source_pbox, &InExtent))
				{
					if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
					{
						select_all();
					   group_selects();  
						ChangeRotation(DEG2RAD(90));
					}

					// Do the draw.
					BOOL fTerminated = CPmwDoc::DrawPreview(re, rc);

					if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
					{
						ChangeRotation(DEG2RAD(0));
					   ungroup_selects();  
					}

					if (re->update_object != NULL)
					{
						re->m_nPreviewSlot = nSlot;
						re->extent = InExtent;
						rc->x_resolution = x_resolution;
						rc->y_resolution = y_resolution;
						break;
					}
				}

				rc->x_resolution = x_resolution;
				rc->y_resolution = y_resolution;
			/*
			// Update the parameters for the next label to draw.
			*/

				nSlot++;
			}
		}
	}
/*
// Restore the scaled source x0 from the saved values.
*/
	SetCurrentSubArea(0);
	GetPaperInfo()->SetProjectDims(ppnt);
	size_to_paper();

	rc->source_pbox = SourcePbox;
	rc->scaled_source_x0 = SourceOrigin.x;
	rc->scaled_source_y0 = SourceOrigin.y;
	m_flScaleFactor = flScaleFactor;

	return FALSE;	
	
}

void CEasyPrintsDoc::GetPreviewBound(PBOX* pBound)
{
	//Get the app's paper info (The entire page, not one slot)
	
	CPaperInfo* pPaperInfo = GET_PMWAPP()->GetPaperInfo(get_orientation());

   if (pPaperInfo != NULL)
   {
      CFixedRect Margins = pPaperInfo->Margins();
      CFixedPoint Dims = pPaperInfo->PaperDims();

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
}


void CEasyPrintsDoc::ResetPaperInfo(CPaperInfo* pPaperInfo)
{

	if(pPaperInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
	{
		m_flScaleFactor = (float)((float)(GetPaperInfo(0)->SlotDims().x) / (float)(GetPaperInfo()->SlotDims().y));
	}
	else
	{
		m_flScaleFactor = (float)((float)(GetPaperInfo(0)->SlotDims().x) / (float)(GetPaperInfo()->SlotDims().x));
	}

	PPNT ppnt = get_dimensions();
	ppnt.x /= m_flScaleFactor;
	ppnt.y /= m_flScaleFactor;
	GetPaperInfo()->SetProjectDims(ppnt);
	CLabelDoc::size_to_paper();

}
