// calview.cpp : implementation file


//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "calview.h"
#include "desktop.h"
#include "caldoc.h"
#include "prntcald.h"
#include "pmwcfg.h"
#include "paper.h"
#include "printer.h"
#include "utils.h"

extern BOOL UserAbort;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarView

IMPLEMENT_DYNCREATE(CCalendarView, CPmwView)

CCalendarView::CCalendarView()
{
}

CCalendarView::~CCalendarView()
{
}

BEGIN_MESSAGE_MAP(CCalendarView, CPmwView)
	//{{AFX_MSG_MAP(CCalendarView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarView drawing

void CCalendarView::OnDraw(CDC* pDC)
{
	

		// Print preview comes in as printing. Printing case does not come here.
	// So, IsPrinting() flag determines preview.
	if (!pDC->IsPrinting())
	{
		CPmwView::OnDraw(pDC);
		return;
	}

	if (printer_rc.is_print_preview)
  {
		if (m_pPreviewExtent == NULL)
		{
			// We need our preview extent.
			ASSERT(FALSE);
			return;
		}

		PBOX old_source = printer_rc.source_pbox;
		PBOX old_extent = m_pPreviewExtent->extent;

		((CCalendarDoc*)GetDocument())->CalcPagePrintSourceBox(
			&printer_rc.source_pbox,
			m_pPreviewExtent->m_nPreviewPage);
	
		PPNT SourceOrigin;
		SourceOrigin.x = printer_rc.source_pbox.x0;
		SourceOrigin.y = printer_rc.source_pbox.y0;
	
			printer_rc.scaled_source_x0 = SourceOrigin.x
								* printer_rc.x_resolution;
			printer_rc.scaled_source_y0 = SourceOrigin.y
								* printer_rc.y_resolution;

		((CCalendarDoc*)GetDocument())->CalcPagePrintSourceBox(
			&m_pPreviewExtent->extent,
			m_pPreviewExtent->m_nPreviewPage);

		CPmwView::OnDraw(pDC);

		m_pPreviewExtent->extent = old_extent;
		printer_rc.source_pbox = old_source;
	}
	else
	{
 // Normal print.
         // Update all the objects into the DC.
         REFRESH_EXTENT re;

         // Build the refresh extent.

         re.extent = printer_rc.source_pbox;
         re.refresh_type = REFRESH_ALL;
         re.my_object = NULL;
         re.update_object = NULL;
         re.update_state = NULL;

         // Draw all objects into this extent.
         GetDocument()->draw_objects(&re, &printer_rc);
  
	}
	return; 
}

/////////////////////////////////////////////////////////////////////////////
// CCalendarView message handlers


/////////////////////////////////////////////////////////////////////////////
// CCalendarView gadget handlers

BOOL CCalendarView::GadgetVisible(UINT uID)
{
	return (uID == ID_PAGE_GADGET) || CPmwView::GadgetVisible(uID);
}

void CCalendarView::GadgetText(UINT uID, CString& csText)
{
	switch (uID)
	{
		case ID_PAGE_GADGET:
		{
			TRY
			{
				DWORD page = GetDocument()->CurrentPageIndex();
				short picture = ((CCalendarDoc*)GetDocument())->GetPictureStyle();
				if (picture == CAL_PICTURE_NONE || picture == CAL_PICTURE_SAME)
				{
					// TODO: Move string to resource file.
					csText.Format("%ld", page+1);
				}
				else
				{
					
					if (page == 0) 
					{
						csText.Format("%ld", page + 1);
					}
					else if (page == GetDocument()->NumberOfPages()-1)
					{
						page *= 2;
						csText.Format("%ld", page);
					}
					else
					{
						page *= 2;
						csText.Format("%ld - %ld", page, page+1);
					}
				}

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

LPCSTR CCalendarView::GadgetMenuName(UINT uID, UINT uPart)
{
	LPCSTR pszMenuName = NULL;

	if (uID == ID_PAGE_GADGET)
	{
		pszMenuName = "Calendar Page";
	}
	else
	{
		pszMenuName = CPmwView::GadgetMenuName(uID, uPart);
	}

	return pszMenuName;
}

/*
// Create the edit desktop for a card.
*/

void CCalendarView::CreateEditDesktop(void)
{
	m_pDesktop = new CCalendarEditDesktop(this);
}


SHORT CCalendarView::OnPrintDocument(PrintContext &pc,
                     int32 aCurrentDocument, int32 aNumDocuments)
{
	if (pc.m_print_scale > 1.0 || pc.m_print_scale < 1.0)
		return CPmwView::OnPrintDocument(pc, aCurrentDocument, aNumDocuments);
	else
		return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}

CPmwPrint* CCalendarView::CreatePrintDialog(void)
{
	return new CPrintCalendarDlg(this);
}

int CCalendarView::PrintCurrentPage(PrintContext& pc, int nPage, BOOL fMakeBlank /*=FALSE*/)
{
	int nError = 0;

	OnPrepareDC(&pc.m_dcPrint, pc.m_pInfo);

	// Set up drawing rect to entire page (in logical coordinates)

	pc.m_pInfo->m_rectDraw.SetRect(0, 0,
						pc.m_dcPrint.GetDeviceCaps(HORZRES),
					   pc.m_dcPrint.GetDeviceCaps(VERTRES));

	pc.m_dcPrint.DPtoLP(&pc.m_pInfo->m_rectDraw);

	CString strFmt;
	if (!fMakeBlank)
	{
		TRY
			LoadConfigurationString(nPage == -1 ? IDS_PRINTKEYPAGE : IDS_PRINTMULTIPAGENUM, strFmt);
		END_TRY
	}
	char szBuf[80];
	wsprintf(szBuf, strFmt, (long)nPage);
	pc.m_dlgstatus->SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);
	ASSERT(pc.m_dlgstatus->IsWindowEnabled());

	if (!pc.m_fBanding)
	{
		VERIFY(pc.m_dcPrint.StartPage());
	}
	if (!fMakeBlank)
	{
		// This is not a blank page. Print the contents now.

		BOOL bEven = !(nPage % 2); // if even, print top half of page

		CPmwView::generate_print_vars(&pc, pc.m_pInfo->m_rectDraw);
		CPmwView::OnDraw(&pc);

		nError = 0;
//		nError = OnPrint(&pc, pc.m_pInfo);
		if (nError == 0 && UserAbort)
		{
			nError = SP_USERABORT;
		}
	}
	if (!pc.m_fBanding)
	{
		int nThisError = pc.m_dcPrint.EndPage();
		if (nError == 0 && nThisError < 0)
		{
			nError = nThisError;
		}
	}

	return nError;
}

int CCalendarView::PrintSingleSided (int nFromPage, int nToPage, PrintContext& pc)
{
	CCalendarDoc *pDoc = (CCalendarDoc*)GetDocument();

	

/*	PPNT dims = pDoc->get_dimensions();
	pc.m_source.x0 = 0;
	pc.m_source.y0 = 0;
	pc.m_source.x1 = dims.x;
	pc.m_source.y1 = dims.y;
*/
	//pDoc->CalcCalendarSize (&pc.m_source);

	int nError = 0;
	// Single sided. Print out all the pages in the desired range.
	for (int nPage = nFromPage; nError == 0 && nPage <= nToPage; nPage++)
	{
	//	BOOL bEven = !(nPage % 2);
		
		pDoc->CalcPagePrintSourceBox (&pc.m_source, nPage);
		
		short picture = pDoc->GetPictureStyle();
		if (picture == CAL_PICTURE_NONE || picture == CAL_PICTURE_SAME)
		{
			DB_RECORD_NUMBER lPage = pDoc->GetPage(nPage-1);

			if (lPage == 0)
			{
				nError = -1;
			}
		
			pDoc->LoadPage(lPage, TRUE, FALSE);
			nError = PrintCurrentPage(pc, nPage);
		}
		else
		{
			DB_RECORD_NUMBER lPage = pDoc->GetPage(nPage / 2);
		
			if (lPage == 0)
			{
				nError = -1;
			}
		
			pDoc->LoadPage(lPage, TRUE, FALSE);
			nError = PrintCurrentPage(pc, nPage);
		}
	}
	return nError;
}

int CCalendarView::PrintDocumentForward(PrintContext& pc, int nSide)
{
	CCalendarDoc * pDoc = (CCalendarDoc*)GetDocument();
	int nError = 0;
	int nFromPage = pc.m_pInfo->GetFromPage();
	int nToPage = pc.m_pInfo->GetToPage();

	for (int nPage = nSide + nFromPage; nPage <= nToPage; nPage += 2)
	{
		pDoc->CalcPagePrintSourceBox (&pc.m_source, nPage);
		
		short ps = pDoc->GetPictureStyle();
		if ( ps == CAL_PICTURE_NONE || ps == CAL_PICTURE_SAME)
		{
			DB_RECORD_NUMBER lPage = pDoc->GetPage(nPage-1);

			if (lPage == 0)
			{
				nError = -1;
				break;
			}
		
			pDoc->LoadPage(lPage, TRUE, FALSE);
			nError = PrintCurrentPage(pc, nPage);
		}
		else
		{
			DB_RECORD_NUMBER lPage = pDoc->GetPage(nPage / 2);
		
			if (lPage == 0)
			{
				nError = -1;
				break;
			}
	
			pDoc->LoadPage(lPage, TRUE, FALSE);
			nError = PrintCurrentPage(pc, nPage);
		}
	}
	return nError;
}

int CCalendarView::PrintDocumentBackward(PrintContext& pc, int nSide)
{
	
	CCalendarDoc * pDoc = (CCalendarDoc*)GetDocument();
	int nError = 0;

	// Get the from and to pages.
	int nFromPage = pc.m_pInfo->GetFromPage();
	int nToPage = pc.m_pInfo->GetToPage();
/*
	// Normalize the page numbers.
	int nPageOrigin = nFromPage;
	nToPage -= nFromPage;
	nFromPage = 0;

	// Compute the page to start from.
	if (nSide == 0)
	{
		nToPage &= ~1;
	}
	else
	{
		if (!(nToPage & 1))
		{
			// Back sides of odd number of pages. Print a blank one.
			if ((nError = PrintBlankPage(pc)) != 0)
			{
				return nError;
			}
			nToPage--;
		}
	}
*/
	FLAGS flag;
	DB_RECORD_NUMBER lPage;
	// Now, print the regular pages.
	for (int nPage = nToPage; nPage >= nFromPage; nPage -= 2)
	{
		flag = pDoc->CalcPagePrintSourceBox (&pc.m_source, nPage);
		short ps = pDoc->GetPictureStyle();		
		if (ps == CAL_PICTURE_NONE || ps == CAL_PICTURE_SAME)
		{
			lPage = pDoc->GetPage(nPage-1);

			if (lPage == 0)
			{
				nError = -1;
				break;
			}
		
	//		pDoc->LoadPage(lPage, TRUE, FALSE);
	//		nError = PrintCurrentPage(pc, nPage);
		}
		else
		{
			lPage = pDoc->GetPage(nPage / 2);
		
			if (lPage == 0)
			{
				nError = -1;
				break;
			}
		
//			pDoc->LoadPage(lPage, TRUE, FALSE);
			// flip page
//			if (nSide == 1)
//				pDoc->FlipPage(pc.m_source, flag);
//			nError = PrintCurrentPage(pc, nPage);
			// flip it back
//			if (nSide == 1)
//				pDoc->FlipPage(pc.m_source, flag);
		}
		pDoc->LoadPage(lPage, TRUE, FALSE);
		// flip page
		if (nSide == 1)
			pDoc->FlipPage(pc.m_source, flag);
		nError = PrintCurrentPage(pc, nPage);
		// flip it back
		if (nSide == 1)
			pDoc->FlipPage(pc.m_source, flag);
	}
	return nError;
}

int CCalendarView::PrintPreviewPageCount(void)
{
   return ((CCalendarDoc*)GetDocument())->NumberOfPrinterPages();
}


int CCalendarView::SetPrintPreviewPage(int nPage)
{
	CCalendarDoc* pDoc = (CCalendarDoc*)GetDocument();

	DWORD dwDesiredPage;
	short style = pDoc->GetPictureStyle();
	switch (style)
	{
		case CAL_PICTURE_NONE:
		case CAL_PICTURE_SAME:
		{
			dwDesiredPage = nPage - 1;
			break;
		}
		case CAL_PICTURE_TOP:
		case CAL_PICTURE_LEFT:
		{
			dwDesiredPage = (nPage / 2);
			break;
		}
	};
	
	DWORD dwCurrentPage = pDoc->CurrentPageIndex();

	if (dwDesiredPage != dwCurrentPage)
	{
		pDoc->GotoPage(dwDesiredPage, FALSE);
	}

	return (int)dwCurrentPage+1;
}