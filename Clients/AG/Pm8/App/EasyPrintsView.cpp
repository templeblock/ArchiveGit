/*
// $Workfile: EasyPrintsView.cpp $
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
// $Log: /PM8/App/EasyPrintsView.cpp $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 5     2/15/99 1:31p Psasse
// m_flScaleFactor and ResetPaperInfo() move to the doc
// 
// 4     2/01/99 1:01a Psasse
// Improved PhotoProjects printing
// 
// 3     1/30/99 11:31p Psasse
// Updated and fairly stable EasyPrints code
// 
// 2     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     12/23/98 5:43p Psasse
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "lbldoc.h"
#include "easyprintsdoc.h"
#include "pmwview.h"
#include "lblview.h"
#include "signview.h"
#include "util.h"
#include "utils.h"
#include "nwsprint.h"
#include "paper.h"
#include "photoprj.h"
#include "printer.h"
#include "labeldlg.h"
#include "lblprint.h"
#include "phoprjprint.h"
#include "macrostd.h"
#include "namerec.h"
#include "addrbook.h"
#include "pmwcfg.h"
#include "pcrdprnt.h"

#include "easyprintsview.h"

#define	NUM_SUBSECTIONS 3

extern BOOL NEAR UserAbort;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEasyPrintsView

IMPLEMENT_DYNCREATE(CEasyPrintsView, CLabelView)

CEasyPrintsView::CEasyPrintsView()
{
}

CEasyPrintsView::~CEasyPrintsView()
{
}


BEGIN_MESSAGE_MAP(CEasyPrintsView, CLabelView)
	//{{AFX_MSG_MAP(CEasyPrintsView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEasyPrintsView drawing

/////////////////////////////////////////////////////////////////////////////
// CEasyPrintsView message handlers

//this is the OnDraw called for Print Preview

void CEasyPrintsView::OnDraw(CDC* pDC)
{
	// Print preview comes in as printing. Printing case does not come here.
	// So, IsPrinting() flag determines preview.
	if (!pDC->IsPrinting() || m_pPreviewExtent == NULL)
	{
		INHERITED::OnDraw(pDC);
		return;
	}

/*
// Save the scaled_source x0 and y0 in the printer rc since we will be
// changing them.
*/

	// Save the in extent since it gets change during interrupt.
	PBOX InExtent = m_pPreviewExtent->extent;

	PBOX SourcePbox = printer_rc.source_pbox;
	
	int x_resolution = printer_rc.x_resolution;
	int y_resolution = printer_rc.y_resolution;
	
	PPNT SourceOrigin;
	SourceOrigin.x = printer_rc.scaled_source_x0;
	SourceOrigin.y = printer_rc.scaled_source_y0;


	/* Get the App and its info. */
	CPmwApp* pApp = GET_PMWAPP();
	CEasyPrintsDoc* pDoc = (CEasyPrintsDoc*)GetDocument();
	float flScaleFactor = pDoc->GetScaleFactor();
	PPNT ppnt = pDoc->get_dimensions();
	int nPrintOrientation = pDoc->get_orientation();
	BOOL fLandscape = (nPrintOrientation == LANDSCAPE);

	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(nPrintOrientation);

	CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(nPrintOrientation);
	CFixedRect PaperPhysicalMargins = pPaperInfo->PhysicalMargins();

	for (int x = 0; x < NUM_SUBSECTIONS; x++)
	{

	/* Get the document to work with. */
		pDoc->SetCurrentSubArea(x);

		/* Get the paper info. */
		CPaperInfo* pLabelInfo = pDoc->GetPaperInfo();
		
		if(x > 0)
		{
			//change the objects position
			if(pLabelInfo->Orientation() != 3) //CPaperInfo::PAPER_Landscape)
			  pDoc->ResetPaperInfo(pLabelInfo);
		}

		/* Get the paper info. */
		pLabelInfo = pDoc->GetPaperInfo();

		if((pLabelInfo->SlotsAcross() != 0) && 
			(pLabelInfo->SlotsDown() != 0))
		{
			CFixedRect Margins = pLabelInfo->Margins();

			// If this is landscape, rotate the margins correctly.

			pPrinterInfo->OrientRect(&Margins);

		/* Compute the number of slots we need to print. */
			int nSlot = m_pPreviewExtent->update_object == NULL ? 0 : m_pPreviewExtent->m_nPreviewSlot;
			int nSlots = pLabelInfo->Slots();

		/* Run through all the slots. */
			while (nSlot < nSlots)
			{
			/*
			// Setup the parameters for this slot so that we will draw in the correct
			// spot on the page.
			*/

				ASSERT(pDoc->CurrentPageIndex() == 0 || pDoc->CurrentPageIndex() == 1);
				CFixedRect SlotBounds = pLabelInfo->SlotBounds(nSlot);
				BOOL fBack = (pDoc->CurrentPageIndex() == 1);

				CFixedPoint Dims = pLabelInfo->PaperDims();

				CFixed lLeft = SlotBounds.Left;
				CFixed lTop = SlotBounds.Top;
				CFixed lWidth = SlotBounds.Width();
				CFixed lHeight = SlotBounds.Height();

				if (fLandscape)
				{
					switch (pPrinterInfo->Orientation())
					{
						case 90:
						{
						// 90 degree rotation (like lasers)
							SlotBounds.Left = Dims.y - (lTop + lHeight);
							SlotBounds.Top = lLeft;
							break;
						}
						default:
						{
						// 270 degree rotation (like dot-matrix)
							SlotBounds.Left = lTop;
							SlotBounds.Top = Dims.x - (lLeft + lWidth);
							break;
						}
					}
					if (fBack)
					{
						// We are printing the back side. Reverse the slot.
						SlotBounds.Left = Dims.y - (SlotBounds.Left + lHeight);
					}
					SlotBounds.Right = SlotBounds.Left + lHeight;
					SlotBounds.Bottom = SlotBounds.Top + lWidth;
				}
				else
				{
					// Portrait.
					if (fBack)
					{
						// We are printing the back side. Reverse the slot.
						SlotBounds.Left = Dims.x - SlotBounds.Right;
						SlotBounds.Right = SlotBounds.Left + lWidth;
					}
				}

				CFixedPoint Org;
				
				Org.x = Margins.Left-PaperPhysicalMargins.Left;
				Org.y = Margins.Top-PaperPhysicalMargins.Top;
			
				if (fLandscape)
				{
				/* Printout is landscape. Reverse the origins. */
					CFixed t = Org.x;
					Org.x = Org.y;
					Org.y = t;
				}
				
				if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
				{
					SlotBounds.Left += ((lWidth - lHeight)/2);
					SlotBounds.Top += ((lHeight - lWidth)/2);
				}

				PPNT Delta;
				Delta.x = MulFixed(SlotBounds.Left + Org.x, PAGE_RESOLUTION);
				Delta.y = MulFixed(SlotBounds.Top + Org.y, PAGE_RESOLUTION);

				printer_rc.source_pbox.x0 = SourcePbox.x0 - Delta.x;
				printer_rc.source_pbox.y0 = SourcePbox.y0 - Delta.y;
				printer_rc.source_pbox.x1 = SourcePbox.x1 - Delta.x;
				printer_rc.source_pbox.y1 = SourcePbox.y1 - Delta.y;

				printer_rc.scaled_source_x0 = /*SourceOrigin.x*/
										- Delta.x*printer_rc.x_resolution;
				printer_rc.scaled_source_y0 = /*SourceOrigin.y*/
										- Delta.y*printer_rc.y_resolution;

				if (IsBoxOnBox(&printer_rc.source_pbox, &InExtent))
				{
					if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
					{
						GetDocument()->select_all();
					   GetDocument()->group_selects();  
						DoSetRotation(90);
					}

					// Do the draw.
					INHERITED::OnDraw(pDC);

					if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
					{
						DoSetRotation(0);
					   GetDocument()->ungroup_selects();  
					}

					if (m_pPreviewExtent->update_object != NULL)
					{
						m_pPreviewExtent->m_nPreviewSlot = nSlot;
						m_pPreviewExtent->extent = InExtent;
						printer_rc.x_resolution = x_resolution;
						printer_rc.y_resolution = y_resolution;
						break;
					}
				}

				printer_rc.x_resolution = x_resolution;
				printer_rc.y_resolution = y_resolution;
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
	pDoc->SetCurrentSubArea(0);
	pDoc->GetPaperInfo()->SetProjectDims(ppnt);
	pDoc->size_to_paper();

	printer_rc.source_pbox = SourcePbox;
	printer_rc.scaled_source_x0 = SourceOrigin.x;
	printer_rc.scaled_source_y0 = SourceOrigin.y;
	pDoc->SetScaleFactor(flScaleFactor);

}


/*
// This is the OnDraw called at print time.
//
// We need to generate multiple images of the label into the appropriate
// places on the page (i.e. the printout).
*/

void CEasyPrintsView::OnDraw(PrintContext* pc)
{
	CEasyPrintsDoc* pDoc = (CEasyPrintsDoc*)GetDocument();
	DWORD dwPage = pDoc->CurrentPageIndex();

	// Verify we are doing a normal page.
	if (dwPage != 0 && dwPage != 1)
	{
		// Non-standard page (like the "key" page). Defer to the base class.
		CPmwView::OnDraw(pc);
		return;
	}

/*
// Save the scaled_source x0 and y0 in the printer rc since we will be
// changing them.
*/

	PBOX SourcePbox = printer_rc.source_pbox;

	PPNT SourceOrigin;
	SourceOrigin.x = printer_rc.scaled_source_x0;
	SourceOrigin.y = printer_rc.scaled_source_y0;
	float flScaleFactor = pDoc->GetScaleFactor();

	int x_resolution = printer_rc.x_resolution;
	int y_resolution = printer_rc.y_resolution;
	
	PPNT ppnt = pDoc->get_dimensions();

/* Get the document to work with. */
	int nPrintOrientation = pDoc->get_orientation();
	BOOL fLandscape = (nPrintOrientation == LANDSCAPE);

/* Get the App and its info. */
	CPmwApp* pApp = GET_PMWAPP();
	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(nPrintOrientation);
	CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(nPrintOrientation);
	CFixedRect PaperPhysicalMargins = pPaperInfo->PhysicalMargins();
	
	BOOL fDone = FALSE;

	for (int x = 0; x < NUM_SUBSECTIONS; x++)
	{
		if(fDone)
			break;

	/* Get the document to work with. */
		pDoc->SetCurrentSubArea(x);

		/* Get the paper info. */
		CPaperInfo* pLabelInfo = pDoc->GetPaperInfo();
		
		if(x > 0)
		{
			if(pLabelInfo->Orientation() != 3) //CPaperInfo::PAPER_Landscape)
				pDoc->ResetPaperInfo(pLabelInfo);
		}

		if((pLabelInfo->SlotsAcross() != 0) && 
			(pLabelInfo->SlotsDown() != 0))
		{

			CFixedRect Margins = pLabelInfo->Margins();
			
			// If this is landscape, rotate the margins correctly.

			pPrinterInfo->OrientRect(&Margins);

		/* Get the label print dialog. */
			CLabelPrintDialog* pDialog = (CLabelPrintDialog*)pc->m_pPD;
		/* Get the photoproject print dialog. */
			CPhotoProjectPrintDialog* pPhotoDialog = (CPhotoProjectPrintDialog*)pc->m_pPD;

		/* Get the name list. */
			CStdMacroServer* pMacroServer = pDoc->GetMacroServer();
			NameListRecord* pNameList = pMacroServer->GetNameList();

		/* Compute the number of slots we need to print. */
			int nSubSection = pPhotoDialog->m_nStartingSubSection;

			if(nSubSection <= x)
			{
				int nSlot = pc->m_nStartingLabel;
				
				if( nSubSection < x)
					nSlot = 0;

				int nSlots = pLabelInfo->Slots();

			/* Run through all the slots. */
				while (nSlot < nSlots && !UserAbort)
				{
				/*
				// See if the name we want is the one that's bound.
				*/
					if (pc->m_nBoundName != pc->m_nCurrentName)
					{
						pc->m_nBoundName = pc->m_nCurrentName;
						ASSERT(pc->m_pCursor != NULL);
						if (pc->m_pCursor != NULL)
						{
							pMacroServer->BindToRecord(pc->m_pCursor, pNameList->Name(pc->m_nBoundName));
							pDoc->ReflowMacros();
						}
					}

				/*
				// Setup the parameters for this slot so that we will draw in the correct
				// spot on the page.
				*/

					ASSERT(dwPage == 0 || dwPage == 1);
					CFixedRect SlotBounds = pLabelInfo->SlotBounds(nSlot);
					BOOL fBack = (dwPage == 1);

					CFixedPoint Dims = pLabelInfo->PaperDims();

					CFixed lLeft = SlotBounds.Left;
					CFixed lTop = SlotBounds.Top;
					CFixed lWidth = SlotBounds.Width();
					CFixed lHeight = SlotBounds.Height();

					if (fLandscape)
					{
						switch (pPrinterInfo->Orientation())
						{
							case 90:
							{
							// 90 degree rotation (like lasers)
								SlotBounds.Left = Dims.y - (lTop + lHeight);
								SlotBounds.Top = lLeft;
								break;
							}
							default:
							{
							// 270 degree rotation (like dot-matrix)
								SlotBounds.Left = lTop;
								SlotBounds.Top = Dims.x - (lLeft + lWidth);
								break;
							}
						}
						if (fBack)
						{
							// We are printing the back side. Reverse the slot.
							SlotBounds.Left = Dims.y - (SlotBounds.Left + lHeight);
						}
						SlotBounds.Right = SlotBounds.Left + lHeight;
						SlotBounds.Bottom = SlotBounds.Top + lWidth;
					}
					else
					{
						// Portrait.
						if (fBack)
						{
							// We are printing the back side. Reverse the slot.
							SlotBounds.Left = Dims.x - SlotBounds.Right;
							SlotBounds.Right = SlotBounds.Left + lWidth;
						}
					}

					CFixedPoint Org;

					Org.x = Margins.Left-PaperPhysicalMargins.Left;
					Org.y = Margins.Top-PaperPhysicalMargins.Top;
				
					if (fLandscape)
					{
					/* Printout is landscape. Reverse the origins. */
						CFixed t = Org.x;
						Org.x = Org.y;
						Org.y = t;
					}
					
					if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
					{
						SlotBounds.Left += ((lWidth - lHeight)/2);
						SlotBounds.Top += ((lHeight - lWidth)/2);
					}

					PPNT Delta;
					Delta.x = MulFixed(SlotBounds.Left + Org.x, PAGE_RESOLUTION);
					Delta.y = MulFixed(SlotBounds.Top + Org.y, PAGE_RESOLUTION);

					printer_rc.source_pbox.x0 = SourcePbox.x0 - Delta.x;
					printer_rc.source_pbox.y0 = SourcePbox.y0 - Delta.y;
					printer_rc.source_pbox.x1 = SourcePbox.x1 - Delta.x;
					printer_rc.source_pbox.y1 = SourcePbox.y1 - Delta.y;

					printer_rc.scaled_source_x0 = /*SourceOrigin.x*/
											- Delta.x*printer_rc.x_resolution;
					printer_rc.scaled_source_y0 = /*SourceOrigin.y*/
											- Delta.y*printer_rc.y_resolution;

					if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
					{
						GetDocument()->select_all();
					   GetDocument()->group_selects();  
						DoSetRotation(90);
					}

					/*
					// Do the draw.
					*/

					CPmwView::OnDraw(&pc->m_dcPrint);

					if(pLabelInfo->Orientation() == 3) //CPaperInfo::PAPER_Landscape)
					{
						DoSetRotation(0);
					   GetDocument()->ungroup_selects();  
					}


				/*
				// Update the parameters for the next label to draw.
				*/

					if (pDialog->m_fCollate)
					{
						if (++pc->m_nCurrentName == pNameList->Names())
						{
						/* Reset the name index. */
							pc->m_nCurrentName = (pNameList->Names() == 0) ? -1 : 0;
						/* Advance to the next copy. */
							pc->m_nCurrentCopy++;
							if (pc->m_nCurrentCopy == pDialog->m_nLabelCopies)
							{
							/* All done! */
								fDone = TRUE;
								break;
							}

						/*
						// Finished another set.
						// If we need to stop here, do so now.
						*/

							if (pDialog->m_fPageBreak)
							{
							/* Move to the next page. */
								break;
							}
						}
					}
					else
					{
					/* We want to do the next copy. */
						if (++pc->m_nCurrentCopy == pDialog->m_nLabelCopies)
						{
						/* Reset the copy count. */
							pc->m_nCurrentCopy = 0;
						/* Advance to the next name. */
							pc->m_nCurrentName++;
							if (pc->m_nCurrentName == pNameList->Names())
							{
							/* All done! */
								fDone = TRUE;
								break;
							}

						/*
						// Finished another set.
						// If we need to stop here, do so now.
						*/

							if (pDialog->m_fPageBreak)
							{
							/* Move to the next page. */
								break;
							}
						}
					}

					printer_rc.x_resolution = x_resolution;
					printer_rc.y_resolution = y_resolution;

					nSlot++;
				}
			}
		}

		printer_rc.x_resolution = x_resolution;
		printer_rc.y_resolution = y_resolution;
	}
/*
// Restore the scaled source x0 from the saved values.
*/

	pDoc->SetCurrentSubArea(0);
	pDoc->GetPaperInfo()->SetProjectDims(ppnt);
	pDoc->size_to_paper();

	printer_rc.source_pbox = SourcePbox;
	printer_rc.scaled_source_x0 = SourceOrigin.x;
	printer_rc.scaled_source_y0 = SourceOrigin.y;
	pDoc->SetScaleFactor(flScaleFactor);

}

void CEasyPrintsView::DoPreviewDraw(CDC* pDC)
{
	CPmwView::OnDraw(pDC);
}

void CEasyPrintsView::generate_preview_vars(CDC *pDC, CPrintInfo* pInfo)
{
   RECT r = pInfo->m_rectDraw;

   int nPrinterResolutionX = pDC->GetDeviceCaps(LOGPIXELSX);
   int nPrinterResolutionY = pDC->GetDeviceCaps(LOGPIXELSY);

   ASSERT(pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)));

   /* Get a DC that doesn't have the printer DC attached. */
   CDC* pScreenDC = CDC::FromHandle(pDC->m_hDC);

   POINT VOrg, WOrg;
   SIZE VExt, WExt;

   VOrg = pScreenDC->GetViewportOrg();
   VExt = pScreenDC->GetViewportExt();
   WOrg = pScreenDC->GetWindowOrg();
   WExt = pScreenDC->GetWindowExt();

   ASSERT(WExt.cx == WExt.cy);
   ASSERT(VExt.cx == VExt.cy);

   /* Get the real (unscaled) resolution. */
   int real_x_resolution = pScreenDC->GetDeviceCaps(LOGPIXELSX);
   int real_y_resolution = pScreenDC->GetDeviceCaps(LOGPIXELSY);

   printer_rc.SetScaling((USHORT)scale_number(VExt.cx, nPrinterResolutionX, real_x_resolution), (USHORT)WExt.cx);

   TRACE("r: %d, %d, %d, %d -> ", r);
   pScreenDC->LPtoDP((LPPOINT)&r, 2);
   pScreenDC->SetViewportExt(1, 1);
   pScreenDC->SetWindowExt(1, 1);
   pScreenDC->DPtoLP((LPPOINT)&r, 2);
   TRACE("r: %d, %d, %d, %d\n", r);

   CPmwApp *pApp = GET_PMWAPP();

/*
// Compute how much we need to adjust our coordinate system by.
*/

   CPaperInfo* pPaperInfo = GetDocument()->PrintPaperInfo();
   CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(GetDocument()->get_orientation());
   
   CFixedRect PhysicalMargins = pPaperInfo->PhysicalMargins();
   CFixedRect Margins = pPaperInfo->Margins();
   CFixedPoint OriginOffset = pPaperInfo->OriginOffset();

   if (GetDocument()->get_orientation() == LANDSCAPE)
   {
      pPrinterInfo->OrientRect(&PhysicalMargins);
      pPrinterInfo->OrientRect(&Margins);

      CFixed t = OriginOffset.x;
      OriginOffset.x = OriginOffset.y;
      OriginOffset.y = t;
   }

   CFixedPoint Adjust;

   Adjust.x = OriginOffset.x;
   Adjust.y = OriginOffset.y;
   
   if ((GetDocument()->get_project_type() != PROJECT_TYPE_Envelope) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_Label) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_BusinessCard))
   {
      Adjust.x += Margins.Left - PhysicalMargins.Left;
      Adjust.y += Margins.Top - PhysicalMargins.Top;
   }

   POINT PrinterAdjust;

   PrinterAdjust.x = (int)MulFixed(Adjust.x, real_x_resolution);
   PrinterAdjust.y = (int)MulFixed(Adjust.y, real_y_resolution);

   PPNT PageAdjust;

   PageAdjust.x = MulFixed(Adjust.x, PAGE_RESOLUTION);
   PageAdjust.x = scale_pcoord(PageAdjust.x,
                               real_x_resolution,
                               printer_rc.x_resolution);

   PageAdjust.y = MulFixed(Adjust.y, PAGE_RESOLUTION);
   PageAdjust.y = scale_pcoord(PageAdjust.y,
                               real_y_resolution,
                               printer_rc.y_resolution);

/* Generate the source box. */

   PBOX print_source_rect;

   print_source_rect.x0 = (PCOORD)r.left;
   print_source_rect.y0 = (PCOORD)r.top;
   print_source_rect.x1 = (PCOORD)r.right;
   print_source_rect.y1 = (PCOORD)r.bottom;

	printer_rc.scaled_source_x0 = (print_source_rect.x0 - PrinterAdjust.x)*PAGE_RESOLUTION
                        + m_pc.m_source.x0*printer_rc.x_resolution;
   printer_rc.scaled_source_y0 = (print_source_rect.y0 - PrinterAdjust.y)*PAGE_RESOLUTION
                        + m_pc.m_source.y0*printer_rc.y_resolution;

	m_scaled_source_x0 = printer_rc.scaled_source_x0;
	m_scaled_source_y0 = printer_rc.scaled_source_y0;

   ScalePBOXWithRounding(&print_source_rect,
               PAGE_RESOLUTION, printer_rc.x_resolution,
               PAGE_RESOLUTION, printer_rc.y_resolution);

   ScalePBOXWithRounding(&print_source_rect,
               printer_rc.GetScaleDenominator(), printer_rc.GetScaleNumerator(),
               printer_rc.GetScaleDenominator(), printer_rc.GetScaleNumerator());

/* Compute the source pbox. */

   printer_rc.source_pbox.x0 = print_source_rect.x0 + m_pc.m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y0 = print_source_rect.y0 + m_pc.m_source.y0 - PageAdjust.y;
   printer_rc.source_pbox.x1 = print_source_rect.x1 + m_pc.m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y1 = print_source_rect.y1 + m_pc.m_source.y0 - PageAdjust.y;

/* Clip it to the document bounds. */

   ScalePBOXWithRounding(&print_source_rect,
                        printer_rc.x_resolution,
                        PAGE_RESOLUTION,
                        printer_rc.y_resolution,
                        PAGE_RESOLUTION);

   ScalePBOXWithRounding(&print_source_rect,
                   printer_rc.GetScaleNumerator(), printer_rc.GetScaleDenominator(),
                   printer_rc.GetScaleNumerator(), printer_rc.GetScaleDenominator());

   printer_rc.destination_rect.left = (SHORT)print_source_rect.x0 + VOrg.x;
   printer_rc.destination_rect.top = (SHORT)print_source_rect.y0 + VOrg.y;
   printer_rc.destination_rect.right = (SHORT)print_source_rect.x1 + VOrg.x;
   printer_rc.destination_rect.bottom = (SHORT)print_source_rect.y1 + VOrg.y;

   printer_rc.destination_x0 = (SHORT)printer_rc.destination_rect.left;
   printer_rc.destination_y0 = (SHORT)printer_rc.destination_rect.top;

   printer_rc.clip_rect = printer_rc.destination_rect;

   TRACE("pdr: %d, %d; source: %ld, %ld, %ld, %ld; clip %d, %d, %d, %d\n",
               printer_rc.destination_rect.top,
               printer_rc.destination_rect.bottom,
               printer_rc.source_pbox,
               printer_rc.clip_rect);
}


VOID CEasyPrintsView::generate_print_vars(PrintContext *pc, RECT r)
{
/* Get the real (unscaled) resolution. */
   SHORT real_x_resolution = pc->m_dcPrint.GetDeviceCaps(LOGPIXELSX);
   SHORT real_y_resolution = pc->m_dcPrint.GetDeviceCaps(LOGPIXELSY);

/* Handle the driver scaling factor if necessary. */

   POINT p = pc->m_scaling;

   if (p.x != 0 || p.y != 0)
   {
      real_x_resolution >>= p.x;
      real_y_resolution >>= p.y;

      /*
      // When banding, the rectangle has not yet been scaled,
      // so we need to do it now.
      // When not banding, the rectangle has already been scaled (DPtoLP).
      */

      if (pc->m_fBanding)
      {
         if (p.x != 0)
         {
            r.left >>= p.x;
            r.right >>= p.x;
         }

         if (p.y != 0)
         {
            r.top >>= p.y;
            r.bottom >>= p.y;
         }
      }

      pc->m_dcPrint.SetMapMode(MM_ANISOTROPIC);
      pc->m_dcPrint.SetWindowExt(1, 1);
      pc->m_dcPrint.SetViewportExt(1 << p.x, 1 << p.y);

   }

   CPmwApp *pApp = GET_PMWAPP();

/*
// Compute how much we need to adjust our coordinate system by.
*/

   CPaperInfo* pPaperInfo = GetDocument()->PrintPaperInfo();
   CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(GetDocument()->get_orientation());
   
   CFixedRect PhysicalMargins = pPaperInfo->PhysicalMargins();
   CFixedRect Margins = pPaperInfo->Margins();
   CFixedPoint OriginOffset = pPaperInfo->OriginOffset();

   if (GetDocument()->get_orientation() == LANDSCAPE)
   {
      pPrinterInfo->OrientRect(&PhysicalMargins);
      pPrinterInfo->OrientRect(&Margins);

      CFixed t = OriginOffset.x;
      OriginOffset.x = OriginOffset.y;
      OriginOffset.y = t;
   }

   CFixedPoint Adjust;

   Adjust.x = OriginOffset.x;
   Adjust.y = OriginOffset.y;
   
   if ((GetDocument()->get_project_type() != PROJECT_TYPE_Envelope) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_Label) &&
         (GetDocument()->get_project_type() != PROJECT_TYPE_BusinessCard))
   {
      Adjust.x += Margins.Left - PhysicalMargins.Left;
      Adjust.y += Margins.Top - PhysicalMargins.Top;
   }

   POINT PrinterAdjust;

   PrinterAdjust.x = (int)MulFixed(Adjust.x, real_x_resolution);
   PrinterAdjust.y = (int)MulFixed(Adjust.y, real_y_resolution);

   PPNT PageAdjust;

   PageAdjust.x = MulFixed(Adjust.x, PAGE_RESOLUTION);
   PageAdjust.x = scale_pcoord(PageAdjust.x,
                               real_x_resolution,
                               printer_rc.x_resolution);
   PageAdjust.y = MulFixed(Adjust.y, PAGE_RESOLUTION);
   PageAdjust.y = scale_pcoord(PageAdjust.y,
                               real_y_resolution,
                               printer_rc.y_resolution);

/* Generate the source box. */

   PBOX print_source_rect;

   print_source_rect.x0 = (PCOORD)r.left;
   print_source_rect.y0 = (PCOORD)r.top;
   print_source_rect.x1 = (PCOORD)r.right;
   print_source_rect.y1 = (PCOORD)r.bottom;

   printer_rc.scaled_source_x0 = (print_source_rect.x0 - PrinterAdjust.x)*PAGE_RESOLUTION
                        + pc->m_source.x0*printer_rc.x_resolution;
   printer_rc.scaled_source_y0 = (print_source_rect.y0 - PrinterAdjust.y)*PAGE_RESOLUTION
                        + pc->m_source.y0*printer_rc.y_resolution;

   ScalePBOXWithRounding(&print_source_rect,
								 PAGE_RESOLUTION, printer_rc.x_resolution,
								 PAGE_RESOLUTION, printer_rc.y_resolution);

/* Compute the source pbox. */

   printer_rc.source_pbox.x0 = print_source_rect.x0 + pc->m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y0 = print_source_rect.y0 + pc->m_source.y0 - PageAdjust.y;
   printer_rc.source_pbox.x1 = print_source_rect.x1 + pc->m_source.x0 - PageAdjust.x;
   printer_rc.source_pbox.y1 = print_source_rect.y1 + pc->m_source.y0 - PageAdjust.y;

/* Clip it to the document bounds. */

   ScalePBOXWithRounding(&print_source_rect,
                        printer_rc.x_resolution,
                        PAGE_RESOLUTION,
                        printer_rc.y_resolution,
                        PAGE_RESOLUTION);

   printer_rc.destination_rect.left = (SHORT)print_source_rect.x0;
   printer_rc.destination_rect.top = (SHORT)print_source_rect.y0;
   printer_rc.destination_rect.right = (SHORT)print_source_rect.x1;
   printer_rc.destination_rect.bottom = (SHORT)print_source_rect.y1;

   printer_rc.destination_x0 = (SHORT)printer_rc.destination_rect.left;
   printer_rc.destination_y0 = (SHORT)printer_rc.destination_rect.top;

   printer_rc.clip_rect = printer_rc.destination_rect;

   od("pdr: %d, %d; source: %ld, %ld, %ld, %ld; clip %d, %d, %d, %d\r\n",
               printer_rc.destination_rect.top,
               printer_rc.destination_rect.bottom,
               printer_rc.source_pbox,
               printer_rc.clip_rect);
}

SHORT CEasyPrintsView::OnPrintDocument(PrintContext &pc,
													int32 aCurrentDocument, int32 aNumDocuments)
{
	return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}


CPmwPrint* CEasyPrintsView::CreatePrintDialog(void)
{
	return new CPhotoProjectPrintDialog(this);
}


int CEasyPrintsView::GrindOutPrintJob(PrintContext& pc)
{
/*
// Print our document(s).
*/

	CPhotoProjectPrintDialog* pDialog = (CPhotoProjectPrintDialog*)pc.m_pPD;

	CPmwDoc* pDoc = GetDocument();

	pDialog->ComputeTotals();

/* Extract the name list. */

	CStdMacroServer* pMacroServer = pDoc->GetMacroServer();
	NameListRecord* pNameList = pMacroServer->GetNameList();

	BOOL fMergePrint = FALSE;

	int nNames = pNameList->Names();

	int nTotalPages = pDialog->m_nTotalPages;
	int nError = FALSE;

	BOOL fOldShow = pMacroServer->ShowMacroValues();

	StorageFile* pDatabaseFile = NULL;
	CFlatFileDatabase* pDatabase = NULL;
	CFlatFileDatabaseCursor* pCursor = NULL;

	if (GetConfiguration()->SupportsAddressBook(FALSE))
	{
		// get sender values for address book.	
		pMacroServer->BindSender();
	}

	if (!fOldShow)
	{
		pMacroServer->ShowMacroValues(TRUE);
		pDoc->ReflowMacros();
	}

	pc.m_pCursor = NULL;

	PPNT dims = pDoc->get_dimensions();
	pc.m_source.x0 = 0;
	pc.m_source.y0 = 0;
	pc.m_source.x1 = dims.x;
	pc.m_source.y1 = dims.y;

/*
// Set the printing conditions.
*/

	pc.m_nBoundName = -1;
	pc.m_nCurrentName = (nNames == 0) ? -1 : 0;
	pc.m_nCurrentCopy = 0;

	CString csFullName;
	TRY
	{
		csFullName = pDoc->GetPathManager()->LocatePath(pNameList->AddressBookName());
		TRACE("Open database %s for data!\r\n", (LPCSTR)csFullName);

		if (nNames != 0)
		{
			ERRORCODE Error = CAddressBook::OpenAddressBookDatabase(csFullName, pDatabaseFile, pDatabase, FALSE);
			if (Error != ERRORCODE_None)
			{
				ThrowErrorcodeException(Error);
			}

			// Create a cursor on the database.
			pCursor = new CFlatFileDatabaseCursor;
			pCursor->Attach(pDatabase);

			pc.m_pCursor = pCursor;
		}

		int nFromPage = pc.m_pInfo->GetFromPage();
		int nToPage = pc.m_pInfo->GetToPage();

		int nError = 0;
		pc.SaveStartOfBand();

		for (;;)
		{
			if (pc.m_fDoubleSided && (nFromPage != nToPage))
			{
				//
				// 1. Put up the dialog telling the user we are about to begin.
				//

				CPmwDialog BeginDialog(IDD_DOUBLE_SIDED_BEGIN);
				if (BeginDialog.DoModal() != IDOK)
				{
					nError = -1;
					break;
				}

				// We need to do the double-sided thing.
				//
				// 2. Print the first half of the document.
				//

				// Print the key sheet now if it goes first.
				if ((pc.m_wDoubleSidedType & DBLPRN_FRONTKEYMASK) == DBLPRN_FrontKeyFirst)
				{
					if ((nError = PrintKeySheetFront(pc)) != 0)
					{
						break;
					}
				}

				// Print the document pages.
				pc.ToStartOfBand();
				if ((nError = PrintLabelsFront(pc)) != 0)
				{
					break;
				}

				// Print the key sheet now if it goes last.
				if ((pc.m_wDoubleSidedType & DBLPRN_FRONTKEYMASK) == DBLPRN_FrontKeyLast)
				{
					if ((nError = PrintKeySheetFront(pc)) != 0)
					{
						break;
					}
				}

				// End the first print job.
				EndPrintDoc(pc, 0);

				//
				// 3. Put up the dialog telling the user to re-insert the stack.
				//

				CPmwDialog NotifyDialog(IDD_DOUBLE_SIDED_NOTIFY);
				if (NotifyDialog.DoModal() != IDOK)
				{
					nError = -1;
					break;
				}

				//
				// 4. Print the second half of the document.
				//

				// Start up the second print job.
				if ((nError = StartPrintDoc(pc)) != 0)
				{
					break;
				}

				// Print the key sheet now if it goes first.
				if ((pc.m_wDoubleSidedType & DBLPRN_BACKKEYMASK) == DBLPRN_BackKeyFirst)
				{
					if ((nError = PrintKeySheetBack(pc)) != 0)
					{
						break;
					}
				}

				// Print the document pages.
				pc.ToStartOfBand();
				if ((nError = PrintLabelsBack(pc)) != 0)
				{
					break;
				}

				// Print the key sheet now if it goes last.
				if ((pc.m_wDoubleSidedType & DBLPRN_BACKKEYMASK) == DBLPRN_BackKeyLast)
				{
					if ((nError = PrintKeySheetBack(pc)) != 0)
					{
						break;
					}
				}
			}
			else
			{
				// Not double-sided. See what page(s) to print.
				//
				// Here are the cases:
				// From == 1, To == 1 (Front only)
				// From == 2, To == 2 (Back only)
				// From == 1, To == 2 (Both pages)
				//
				// So, we print page 1 if "from" is 1, and we print page 2 if "to" is 2.

				if (nFromPage == 1)
				{
					// Print the fronts in forward order.
					pc.ToStartOfBand();
					if ((nError = PrintLabelsForward(pc, 0)) != 0)
					{
						break;
					}
				}
				if (nToPage == 2)
				{
					// Print the backs in forward order.
					pc.ToStartOfBand();
					if ((nError = PrintLabelsForward(pc, 1)) != 0)
					{
						break;
					}
				}
			}
			// Done. Always leave!
			break;
		}
	}
	END_TRY

	BOOL fReflow = !fOldShow;
	pMacroServer->ShowMacroValues(fOldShow);
	if (nNames != 0 && pc.m_nCurrentName > 0)
	{
	/* Go back to the first name. */
		if (pCursor != NULL)
		{
			pMacroServer->BindToRecord(pCursor, pNameList->Name(0));
		}
		fReflow = TRUE;
	}

	if (fReflow)
	{
		pDoc->ReflowMacros();
	}

	delete pCursor;
	pCursor = NULL;
	CAddressBook::CloseAddressBookDatabase(pDatabaseFile, pDatabase);

	return nError;
}


















