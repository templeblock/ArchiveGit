/*
// $Workfile: ENVVIEW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVVIEW.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:20   Fred
// Initial revision.
// 
//    Rev 1.3   15 Apr 1997 17:33:18   johno
// 
//    Rev 1.2   15 Apr 1997 16:54:14   johno
// Changes for "New Project" path
// 
//    Rev 1.1   24 Mar 1997 10:35:26   Jay
// Print preview for labels and envelopes
// 
//    Rev 1.0   14 Mar 1996 13:33:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:24   FRED
// Initial revision.
// 
//    Rev 1.7   06 Oct 1995 11:59:54   JAY
// New print code, etc.
// 
//    Rev 1.6   03 Oct 1995 10:03:12   JAY
// Label/envelope printing fixes
// 
//    Rev 1.5   16 Aug 1995 18:02:36   JAY
// Envelope type.
// 
//    Rev 1.4   16 Aug 1995 08:14:18   JAY
// Fixed printing.
// 
//    Rev 1.3   09 Aug 1995 13:12:54   JAY
// Misc changes.
// 
//    Rev 1.2   08 Aug 1995 11:01:42   JAY
// Envelope stuff.
// 
//    Rev 1.1   28 Jul 1995 12:31:22   JAY
// Printer info. Envelope printing.
// 
//    Rev 1.0   20 Jul 1995 12:52:20   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "envdoc.h"
#include "envview.h"
#include "envprint.h"
#include "paper.h"
#include "printer.h"
#include "envtype.h"
#include "objlist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeView

IMPLEMENT_DYNCREATE(CEnvelopeView, CPmwView)

CEnvelopeView::CEnvelopeView()
{
}

CEnvelopeView::~CEnvelopeView()
{
}

BEGIN_MESSAGE_MAP(CEnvelopeView, CPmwView)
	//{{AFX_MSG_MAP(CEnvelopeView)
	ON_COMMAND(ID_ENVELOPE_TYPE, OnEnvelopeType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeView message handlers

CPmwPrint* CEnvelopeView::CreatePrintDialog(void)
{
	return new CEnvelopePrintDialog(this);
}

void CEnvelopeView::FlipProject(void)
{
	CPmwDoc* pDoc = GetDocument();

	ObjectList* pObjects = pDoc->object_list();

	PMGPageObjectPtr pObject;
	PBOX world;

	pDoc->get_panel_world(&world);

	for (pObject = (PMGPageObjectPtr)pObjects->first_object();
					pObject != NULL;
					pObject = (PMGPageObjectPtr)pObject->next_object())
	{
	/* To alternate position. */

		pObject->xflip();
		pObject->yflip();

		if (pObject->get_layout() <= 0)
		{
			PBOX bound = ((RectPageObjectPtr)pObject)->get_unrotated_bound();
			PCOORD tmp;

			tmp = world.x1 - (bound.x0 - world.x0);
			bound.x0 = world.x1 - (bound.x1 - world.x0);
			bound.x1 = tmp;

			tmp = world.y1 - (bound.y0 - world.y0);
			bound.y0 = world.y1 - (bound.y1 - world.y0);
			bound.y1 = tmp;

			((RectPageObjectPtr)pObject)->set_unrotated_bound(bound);
		}
		pDoc->calc_object(pObject);
	}
}

/*
// This is the OnDraw called at print time.
//
// We need to position the label at the appropriate place on the page.
*/

void CEnvelopeView::OnDraw(CDC* pDC)
{
	if (pDC->IsPrinting())
	{
	/*
	// Save the scaled_source x0 and y0 in the printer rc since we will be
	// changing them.
	*/

		PBOX SourcePbox = printer_rc.source_pbox;

		PPNT SourceOrigin;
		SourceOrigin.x = printer_rc.scaled_source_x0;
		SourceOrigin.y = printer_rc.scaled_source_y0;

	/* Get the document to work with. */
		CPmwDoc* pDoc = GetDocument();

	/* Get the App and its info. */
		CPmwApp* pApp = GET_PMWAPP();
		int nPrintOrientation = ((CEnvelopeDoc*)pDoc)->GetPrintOrientation();
		CPaperInfo* pPaperInfo = pApp->GetPaperInfo(nPrintOrientation);
		CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(nPrintOrientation);
		CFixedRect PaperPhysicalMargins = pPaperInfo->PhysicalMargins();

	/* Get the envelope info. */
		CPaperInfo* pEnvelopeInfo = pDoc->GetPaperInfo();
		CFixedRect Margins = pEnvelopeInfo->Margins();

	/*
	// Setup the parameters for the envelope so that we will draw in the correct
	// spot on the page.
	*/

		WORD wPosition = pPrinterInfo->GetEnvelopePosition();
		BOOL fWide = (wPosition & ENVELOPE_OMASK) == ENVELOPE_Wide;
		
		CFixedPoint PaperDims = pPaperInfo->PaperDims();
		CFixedPoint EnvelopeDims = pEnvelopeInfo->PaperDims();
		BOOL fReverseLandscape = !fWide && pPrinterInfo->ReverseLandscape();

		if (fWide)
		{
		/* Envelope is wide. Reverse the dimensions. */
			CFixed t = EnvelopeDims.x;
			EnvelopeDims.x = EnvelopeDims.y;
			EnvelopeDims.y = t;
			
			pPrinterInfo->OrientRect(&Margins, FALSE, TRUE);
		}

		BOOL fFlip = FALSE;

		if ((wPosition & ENVELOPE_FMASK) == ENVELOPE_FlapUp)
		{
			fFlip = !fFlip;
		}
		if (fReverseLandscape)
		{
			fFlip = !fFlip;
		}

		if (fFlip)
		{
		/*
		// Flap up reverses left and right.
		// 
		// (Cheat a little... we know this works with the current value scheme.)
		*/
			if (!(wPosition & ENVELOPE_Center))
			{
				wPosition ^= ENVELOPE_Right;
			}
		}

		CFixedPoint Org;

		switch (wPosition & ENVELOPE_HMASK)
		{
			case ENVELOPE_Center:
			{
				Org.x = (PaperDims.x - EnvelopeDims.x)/2;
				break;
			}
			case ENVELOPE_Right:
			{
				Org.x = PaperDims.x - EnvelopeDims.x;
				break;
			}
			default:
			{
				Org.x = 0;
				break;
			}
		}

		Org.y = 0;

		if (!fWide)
		{
		/*
		// This is not a wide printout. Landscape will be in effect.
		// Check the landscape direction to see if we should top or bottom align
		// the document.
		*/
			if (((CEnvelopeDoc*)pDoc)->EnvelopeOrientation(pPrinterInfo) == 90)
			{
				Org.y = PaperDims.y - EnvelopeDims.y;
			}
		}

		Org.x += Margins.Left-PaperPhysicalMargins.Left;
		Org.y += Margins.Top-PaperPhysicalMargins.Top;

		if (!fWide)
		{
		/* Printout is landscape. Reverse the origins. */
			CFixed t = Org.x;
			Org.x = Org.y;
			Org.y = t;
		}

		PPNT Delta;
		Delta.x = MulFixed(Org.x, PAGE_RESOLUTION);
		Delta.y = MulFixed(Org.y, PAGE_RESOLUTION);

		printer_rc.source_pbox.x0 = SourcePbox.x0 - Delta.x;
		printer_rc.source_pbox.y0 = SourcePbox.y0 - Delta.y;
		printer_rc.source_pbox.x1 = SourcePbox.x1 - Delta.x;
		printer_rc.source_pbox.y1 = SourcePbox.y1 - Delta.y;

		printer_rc.scaled_source_x0 = SourceOrigin.x
								- Delta.x*printer_rc.x_resolution;
		printer_rc.scaled_source_y0 = SourceOrigin.y
								- Delta.y*printer_rc.y_resolution;

	/*
	// Do the draw.
	*/

		if (fReverseLandscape)
		{
			FlipProject();
		}
		CPmwView::OnDraw(pDC);
		if (fReverseLandscape)
		{
			FlipProject();
		}

	/*
	// Restore the scaled source x0 from the saved values.
	*/

		printer_rc.source_pbox = SourcePbox;

		printer_rc.scaled_source_x0 = SourceOrigin.x;
		printer_rc.scaled_source_y0 = SourceOrigin.y;
	}
	else
	{
		// Go to base draw for normal OnIdle drawing.
		CPmwView::OnDraw(pDC);
	}
}

void CEnvelopeView::OnEnvelopeType()
{
	CPmwDoc* pDoc = GetDocument();

	CEnvelopeTypeDialog Dialog(pDoc->GetPaperInfo(), this);

	if (Dialog.DoModal() == IDOK)
	{
		CPaperInfo* pPaperInfo = Dialog.GetChosenEnvelope();

		if (pPaperInfo != NULL)
		{
		/* Get the paper info. */
			pDoc->AssignPaperInfo(*pPaperInfo);
			pDoc->SizeToInternalPaper();
			pDoc->reset_views();
		}
	}
}
