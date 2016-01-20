#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "stkrview.h"
#include "stkrprnd.h"
#include "stkdsktp.h"
#include "lblprint.h"
#include "macrostd.h"
#include "namerec.h"
#include "addrbook.h"
#include "printer.h"
#include "pmwcfg.h"
#include "pcrdprnt.h"
#include "stkrdoc.h"
#include "labeldlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BOOL NEAR UserAbort;

/////////////////////////////////////////////////////////////////////////////
// CStickerView

IMPLEMENT_DYNCREATE(CStickerView, CPmwView)

CStickerView::CStickerView()
{
}

CStickerView::~CStickerView()
{
}

BEGIN_MESSAGE_MAP(CStickerView, CPmwView)
	//{{AFX_MSG_MAP(CStickerView)
	ON_COMMAND(ID_LABEL_TYPE, OnStickerType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStickerView message handlers

void CStickerView::OnStickerType()
{
	CPmwDoc* pDoc = GetDocument();

	CLabelTypeDialog Dialog(pDoc->GetPaperInfo(), this);

	if (Dialog.DoModal() == IDOK)
	{
		CLabelData* pSticker = Dialog.GetChosenLabel();

		if (pSticker != NULL)
		{
		/* Get the paper info. */
			pDoc->AssignPaperInfo(*pSticker);
			pDoc->size_to_paper();
			pDoc->reset_views();
		}
	}
}

/*
// Create the edit desktop for a Sticker.
*/

void CStickerView::CreateEditDesktop(void)
{
	m_pDesktop = new CStickerEditDesktop(this);
}

CPmwPrint* CStickerView::CreatePrintDialog(void)
{
	return new CStickerPrintDialog(this);
}


// This is the normal OnDraw.
// This exists only to handle print preview. Non-preview draws are passed
// through to the parent class to handle the on-idle drawing.
//
// We need to generate multiple images of the Sticker into the appropriate
// places on the page (i.e. the preview).


void CStickerView::OnDraw(CDC* pDC)
{
	//INHERITED::OnDraw(pDC);
	//return;

	// Print preview comes in as printing. Printing case does not come here.
	// So, IsPrinting() flag determines preview.
	if (!pDC->IsPrinting())
	{
		INHERITED::OnDraw(pDC);
		return;
	}

	if (m_pPreviewExtent == NULL)
	{
		// We need our preview extent.
		ASSERT(FALSE);
		return;
	}

	PBOX SourcePbox = printer_rc.source_pbox;

	PPNT SourceOrigin;
	SourceOrigin.x = printer_rc.scaled_source_x0;
	SourceOrigin.y = printer_rc.scaled_source_y0;

/* Get the document to work with. */
	CPmwDoc* pDoc = GetDocument();
	int nPrintOrientation = pDoc->get_orientation();
	BOOL fLandscape = (nPrintOrientation == LANDSCAPE);

/* Get the App and its info. */
	CPmwApp* pApp = GET_PMWAPP();
	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(nPrintOrientation);
	CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(nPrintOrientation);
	//CFixedRect PaperPhysicalMargins = pPaperInfo->PhysicalMargins();
	
/* Get the paper info. */
	CPaperInfo* pLabelInfo = pDoc->GetPaperInfo();
	CFixedRect Margins = pLabelInfo->Margins();
	
	// If this is landscape, rotate the margins correctly.

	pPrinterInfo->OrientRect(&Margins);

		
		PPNT Delta;
		Delta.x = MulFixed(Margins.Left, PAGE_RESOLUTION);
		Delta.y = MulFixed(Margins.Top, PAGE_RESOLUTION);
		
		printer_rc.source_pbox.x0 = SourcePbox.x0 + Delta.x;
		printer_rc.source_pbox.y0 = SourcePbox.y0 + Delta.y;
		printer_rc.source_pbox.x1 = SourcePbox.x1 - Delta.x;
		printer_rc.source_pbox.y1 = SourcePbox.y1 - Delta.y;

		printer_rc.scaled_source_x0 = /*SourceOrigin.x +*/ Delta.x * printer_rc.x_resolution;
		printer_rc.scaled_source_y0 = /*SourceOrigin.y +*/ Delta.y * printer_rc.y_resolution;

	/*
	// Do the draw.
	*/

		CPmwView::OnDraw(pDC);

/*
// Restore the scaled source x0 from the saved values.
*/

	printer_rc.source_pbox = SourcePbox;
	printer_rc.scaled_source_x0 = SourceOrigin.x;
	printer_rc.scaled_source_y0 = SourceOrigin.y;


}

LPCSTR CStickerView::GadgetMenuName(UINT uID, UINT uPart)
{
	LPCSTR pszMenuName = NULL;

	if (uID == ID_PAGE_GADGET)
	{
		pszMenuName = "Sticker Page";
	}
	else
	{
		pszMenuName = CPmwView::GadgetMenuName(uID, uPart);
	}

	return pszMenuName;
}

void CStickerView::GadgetText(UINT uID, CString& csText)
{
	switch (uID)
	{
		case ID_PAGE_GADGET:
		{
			TRY
			{
				DWORD page = GetDocument()->CurrentPageIndex();
				DWORD pages = GetDocument()->NumberOfPages();
				// TODO: Move string to resource file.
				csText.Format("%ld of %ld", page+1, pages);
			}
			END_TRY

			break;
		}
		default:
		{
			CPmwView::GadgetText(uID, csText);
			break;
		}
	}
}

BOOL CStickerView::GadgetVisible(UINT uID)
{
	return (uID == ID_PAGE_GADGET) || CPmwView::GadgetVisible(uID);
}


SHORT CStickerView::OnPrintDocument(PrintContext &pc,
                     int32 aCurrentDocument, int32 aNumDocuments)
{
	if (pc.m_print_scale > 1.0 || pc.m_print_scale < 1.0)
		return CPmwView::OnPrintDocument(pc, aCurrentDocument, aNumDocuments);
	else
		return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}


/*
// This is the OnDraw called at print time.
//
// We need to generate multiple images of the label into the appropriate
// places on the page (i.e. the printout).
*/

void CStickerView::OnDraw(PrintContext* pc)
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
	int nPrintOrientation = pDoc->get_orientation();
	BOOL fLandscape = (nPrintOrientation == LANDSCAPE);

/* Get the App and its info. */
	CPmwApp* pApp = GET_PMWAPP();
	CPaperInfo* pPaperInfo = pApp->GetPaperInfo(nPrintOrientation);
	CPrinterInfo* pPrinterInfo = pApp->GetPrinterInfo(nPrintOrientation);
	CFixedRect PaperPhysicalMargins = pPaperInfo->PhysicalMargins();
	
/* Get the paper info. */
	CPaperInfo* pLabelInfo = pDoc->GetPaperInfo();
	CFixedRect Margins = pLabelInfo->Margins();
	
	// If this is landscape, rotate the margins correctly.

	pPrinterInfo->OrientRect(&Margins);

/* Get the label print dialog. */
	CLabelPrintDialog* pDialog = (CLabelPrintDialog*)pc->m_pPD;


		
		PPNT Delta;
		Delta.x = MulFixed(PaperPhysicalMargins.Left, PAGE_RESOLUTION);
		Delta.y = MulFixed(PaperPhysicalMargins.Top, PAGE_RESOLUTION);
		
		printer_rc.source_pbox.x0 = SourcePbox.x0 + Delta.x;
		printer_rc.source_pbox.y0 = SourcePbox.y0 + Delta.y;
		printer_rc.source_pbox.x1 = SourcePbox.x1 - Delta.x;
		printer_rc.source_pbox.y1 = SourcePbox.y1 - Delta.y;

		printer_rc.scaled_source_x0 = /*SourceOrigin.x +*/ Delta.x * printer_rc.x_resolution;
		printer_rc.scaled_source_y0 = /*SourceOrigin.y +*/ Delta.y * printer_rc.y_resolution;

	/*
	// Do the draw.
	*/

		CPmwView::OnDraw(&pc->m_dcPrint);

/*
// Restore the scaled source x0 from the saved values.
*/

	printer_rc.source_pbox = SourcePbox;
	printer_rc.scaled_source_x0 = SourceOrigin.x;
	printer_rc.scaled_source_y0 = SourceOrigin.y;
}
