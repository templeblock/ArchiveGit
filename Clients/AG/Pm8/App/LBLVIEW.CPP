/*
// $Workfile: LBLVIEW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLVIEW.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 8     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 7     7/28/98 9:40a Jayn
// Fixed a crash when no names selected for postcard print.
// 
// 6     7/24/98 10:29p Jayn
// Post card printing. Label count adjustment for name list.
// 
// 4     4/24/98 6:33p Fredf
// Fixed bug with simultaneous print previews. Moved preview refresh
// extent pointer into CPmwView... it was a global variable.
// 
// 3     3/10/98 5:24p Jayn
// Master page changes
// 
// 2     3/06/98 3:59p Jayn
// Post card print code (double-sided).
// 
//    Rev 1.3   02 Oct 1997 15:20:46   johno
// Various changes for post card printing
// 
//    Rev 1.2   26 Sep 1997 16:57:12   johno
// Added PostCardView
// 
//    Rev 1.1   19 Sep 1997 17:35:40   johno
// Changes for new Post Card project type
// (preliminary)
// 
// 
//    Rev 1.0   14 Aug 1997 15:22:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:02   Fred
// Initial revision.
// 
//    Rev 1.5   26 Mar 1997 14:51:56   Jay
// Interruptable preview
// 
//    Rev 1.4   24 Mar 1997 10:35:26   Jay
// Print preview for labels and envelopes
// 
//    Rev 1.3   16 Jul 1996 17:31:56   Fred
// More address/sender book stuff
// 
//    Rev 1.2   16 Jul 1996 08:53:00   Fred
// Hooked up address/sender book
// 
//    Rev 1.1   15 May 1996 08:46:08   Jay
// From PMW2
// 
//    Rev 1.19   13 May 1996 14:27:00   JAY
// Won't attempt to bind sender values if address book is not supported.
// 
//    Rev 1.18   24 Jan 1996 10:03:28   JAY
// New string management in Configuration
// 
//    Rev 1.17   06 Oct 1995 11:59:50   JAY
// New print code, etc.
// 
//    Rev 1.15   05 Sep 1995 10:18:36   JAY
// Always correctly reflows macros after printing.
// 
//    Rev 1.14   31 Aug 1995 10:04:14   FRED
// Miscellaneous fixes.
// 
//    Rev 1.13   23 Aug 1995 11:18:20   JAY
// Now handles landscape printing correctly.
// 
//    Rev 1.12   19 Aug 1995 12:04:56   FRED
// Business card print dialogs
// 
//    Rev 1.11   16 Aug 1995 16:23:36   JAY
//  
// 
//    Rev 1.10   15 Aug 1995 14:11:14   JAY
// Fixed banding/placement problem with printing.
// 
//    Rev 1.9   11 Aug 1995 09:37:08   JAY
// Fixed an error in nSlots computation for printing page.
// 
//    Rev 1.8   09 Aug 1995 13:12:52   JAY
// Misc changes.
// 
//    Rev 1.7   08 Aug 1995 12:10:34   FRED
// Open databases read-only
// 
//    Rev 1.6   04 Aug 1995 10:47:06   FRED
// Sender fields.
// 
//    Rev 1.5   02 Aug 1995 14:56:30   JAY
//  
// 
//    Rev 1.4   02 Aug 1995 07:47:58   JAY
// 
//    Rev 1.3   31 Jul 1995 10:27:32   JAY
// New projects.
// 
//    Rev 1.2   21 Jul 1995 17:20:40   JAY
// Label printing
// 
//    Rev 1.1   20 Jul 1995 16:27:12   JAY
//  
// 
//    Rev 1.0   20 Jul 1995 12:52:18   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "lblview.h"
#include "labeldlg.h"
#include "desktop.h"
#include "lblprint.h"
#include "macrostd.h"
#include "namerec.h"
#include "addrbook.h"
#include "printer.h"
#include "pmwcfg.h"
#include "pcrdprnt.h"
#include "postcard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BOOL NEAR UserAbort;

/////////////////////////////////////////////////////////////////////////////
// CLabelView

IMPLEMENT_DYNCREATE(CLabelView, CPmwView)

CLabelView::CLabelView()
{
}

CLabelView::~CLabelView()
{
}

BEGIN_MESSAGE_MAP(CLabelView, CPmwView)
	//{{AFX_MSG_MAP(CLabelView)
	ON_COMMAND(ID_LABEL_TYPE, OnLabelType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLabelView message handlers

void CLabelView::OnLabelType()
{
	CPmwDoc* pDoc = GetDocument();

	CLabelTypeDialog Dialog(pDoc->GetPaperInfo(), this);

	if (Dialog.DoModal() == IDOK)
	{
		CLabelData* pLabel = Dialog.GetChosenLabel();

		if (pLabel != NULL)
		{
		/* Get the paper info. */
			pDoc->AssignPaperInfo(*pLabel);
			pDoc->SizeToInternalPaper();
			pDoc->reset_views();
		}
	}
}

/*
// Create the edit desktop for a label.
*/

void CLabelView::CreateEditDesktop(void)
{
	m_pDesktop = new CLabelEditDesktop(this);
}

CPmwPrint* CLabelView::CreatePrintDialog(void)
{
	return new CLabelPrintDialog(this);
}

int CLabelView::GrindOutPrintJob(PrintContext& pc)
{
/*
// Print our document(s).
*/

	CLabelPrintDialog* pDialog = (CLabelPrintDialog*)pc.m_pPD;

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

int CLabelView::PrintLabelsFront(PrintContext& pc)
{
	// No error yet.
	int nError = 0;

	if ((pc.m_wDoubleSidedType & DBLPRN_FRONTDIRMASK) == DBLPRN_FrontForward)
	{
		// Print in the forward direction.
		nError = PrintLabelsForward(pc, 0);
	}
	else
	{
		// Print in the reverse direction.
		nError = PrintLabelsBackward(pc, 0);
	}

	return nError;
}

int CLabelView::PrintLabelsBack(PrintContext& pc)
{
	// No error yet.
	int nError = 0;

	if ((pc.m_wDoubleSidedType & DBLPRN_BACKDIRMASK) == DBLPRN_BackForward)
	{
		// Print in the forward direction.
		nError = PrintLabelsForward(pc, 1);
	}
	else
	{
		// Print in the reverse direction.
		nError = PrintLabelsBackward(pc, 1);
	}

	return nError;
}

int CLabelView::PrintLabelsForward(PrintContext &pc, int nSide)
{
	CPmwDoc* pDoc = GetDocument();

	CLabelPrintDialog* pDialog = (CLabelPrintDialog*)pc.m_pPD;
	int nTotalPages = pDialog->m_nTotalPages;

	// Set the side.
	DWORD dwCurrentPage = pDoc->CurrentPageIndex();
	if (nSide != (int)dwCurrentPage)
	{
		pDoc->GotoPage((DWORD)nSide, FALSE);
	}

	// Start at requested first label for first page.
	int nError = 0;

	// Run through all the pages.
	for (int nPage = 0; nPage < nTotalPages; nPage++)
	{
		pc.m_nStartingLabel = (nPage == 0) ? pDialog->m_nStartingLabel : 0;
		if ((nError = OnPrintLabels(pc, nPage, nTotalPages)) != 0)
		{
			// Aack! We got an error. Leave now.
			break;
		}
	}

	// Reset the page.
	if (dwCurrentPage != pDoc->CurrentPageIndex())
	{
		pDoc->GotoPage(dwCurrentPage, FALSE);
	}

	return nError;
}

int CLabelView::PrintLabelsBackward(PrintContext &pc, int nSide)
{
	CPmwDoc* pDoc = GetDocument();

	CLabelPrintDialog* pDialog = (CLabelPrintDialog*)pc.m_pPD;
	int nTotalPages = pDialog->m_nTotalPages;

	// Set the side.
	DWORD dwCurrentPage = pDoc->CurrentPageIndex();
	if (nSide != (int)dwCurrentPage)
	{
		pDoc->GotoPage((DWORD)nSide, FALSE);
	}

	// Start at requested first label for first page.
	int nError = 0;

	// Run through all the pages.
	for (int nPage = nTotalPages; nPage-- > 0;)
	{
		pc.m_nStartingLabel = (nPage == 0) ? pDialog->m_nStartingLabel : 0;
		SetLabelVariables(pc, nPage);
		if ((nError = OnPrintLabels(pc, nPage, nTotalPages)) != 0)
		{
			// Aack! We got an error. Leave now.
			break;
		}
	}

	// Reset the page.
	if (dwCurrentPage != pDoc->CurrentPageIndex())
	{
		pDoc->GotoPage(dwCurrentPage, FALSE);
	}

	return nError;
}

/*
// Set the variables for a label printout.
// This is currently used only for the reverse print (labels printed backward
// during double-sided printing of post cards).
// The current code assumes that m_fPageBreak is FALSE. I could write code to
// support it, but since I don't have to, I won't right now.
*/

void CLabelView::SetLabelVariables(PrintContext& pc, int nPage)
{
	CLabelPrintDialog* pDialog = (CLabelPrintDialog*)pc.m_pPD;
	ASSERT(!pDialog->m_fPageBreak);
	ASSERT(nPage < pDialog->m_nTotalPages);

	// Compute how many labels have been printed to this point.
	CPmwDoc* pDoc = GetDocument();
	CPaperInfo* pLabelInfo = pDoc->GetPaperInfo();
	int nStartingLabel = pDialog->m_nStartingLabel;
	int nSlots = pLabelInfo->Slots();

	// This is how many labels have been printed so far.
	int nLabels = nPage*nSlots - nStartingLabel;

	// Compute the new name index and copy count.
	// Get the name list.
	CStdMacroServer* pMacroServer = pDoc->GetMacroServer();
	NameListRecord* pNameList = pMacroServer->GetNameList();
	int nNames = pNameList->Names();

	if (nNames == 0)
	{
		pc.m_nCurrentName = -1;
		pc.m_nCurrentCopy = nLabels;
	}
	else
	{
		if (pDialog->m_fCollate)
		{
			pc.m_nCurrentName = nLabels % nNames;
			pc.m_nCurrentCopy = nLabels / nNames;
		}
		else
		{
			int nCopies = pDialog->m_nLabelCopies;
			ASSERT(nCopies > 0);
			pc.m_nCurrentName = nLabels / nCopies;
			pc.m_nCurrentCopy = nLabels % nCopies;
		}
	}
}

/*
// Streamlined version of OnPrintDocument for labels.
// Does not require tiling, cut marks, etc.
*/

int CLabelView::OnPrintLabels(PrintContext &pc, int nCurrentPage, int nNumPages)
{
	CPmwDoc* pDoc = GetDocument();
	PPNT dims = pDoc->get_dimensions();

	int nError = 0;

	int nPrintCopies = pc.m_copies;

	while (nError == 0 && nPrintCopies-- > 0)
	{
	// Source will always be all of the label.

		pc.m_source.x0 = 0;
		pc.m_source.y0 = 0;
		pc.m_source.x1 = dims.x;
		pc.m_source.y1 = dims.y;

		OnPrepareDC(&pc.m_dcPrint, pc.m_pInfo);

	// Set up drawing rect to entire page (in logical coordinates)

		pc.m_pInfo->m_rectDraw.SetRect(0, 0,
							pc.m_dcPrint.GetDeviceCaps(HORZRES),
							pc.m_dcPrint.GetDeviceCaps(VERTRES));
		pc.m_dcPrint.DPtoLP(&pc.m_pInfo->m_rectDraw);

		CString strFmt;
		TRY
			LoadConfigurationString(IDS_PRINTPAGENUM, strFmt);
		END_TRY
		char szBuf[80];
		wsprintf(szBuf, strFmt, (int32)nCurrentPage+1, (int32)nNumPages);
		pc.m_dlgstatus->SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);

		if (!pc.m_fBanding)
		{
			VERIFY(pc.m_dcPrint.StartPage());
		}
		nError = OnPrint(&pc, pc.m_pInfo) || UserAbort;
		if (!pc.m_fBanding)
		{
			if (pc.m_dcPrint.EndPage() < 0)
				nError = TRUE;
		}
	}
	return nError;
}

/*
// This is the OnDraw called at print time.
//
// We need to generate multiple images of the label into the appropriate
// places on the page (i.e. the printout).
*/

void CLabelView::OnDraw(PrintContext* pc)
{
	CPmwDoc* pDoc = GetDocument();
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

/* Get the document to work with. */
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

/* Get the name list. */
	CStdMacroServer* pMacroServer = pDoc->GetMacroServer();
	NameListRecord* pNameList = pMacroServer->GetNameList();

/* Compute the number of slots we need to print. */
	int nSlot = pc->m_nStartingLabel;
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
//				case -1:
//				case 270:
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

	/*
	// Do the draw.
	*/

		CPmwView::OnDraw(&pc->m_dcPrint);

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
		nSlot++;
	}

/*
// Restore the scaled source x0 from the saved values.
*/

	printer_rc.source_pbox = SourcePbox;
	printer_rc.scaled_source_x0 = SourceOrigin.x;
	printer_rc.scaled_source_y0 = SourceOrigin.y;
}

/*
// This is the normal OnDraw.
// This exists only to handle print preview. Non-preview draws are passed
// through to the parent class to handle the on-idle drawing.
//
// We need to generate multiple images of the label into the appropriate
// places on the page (i.e. the preview).
*/

void CLabelView::OnDraw(CDC* pDC)
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
//				case -1:
//				case 270:
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
			// Do the draw.
			INHERITED::OnDraw(pDC);

			if (m_pPreviewExtent->update_object != NULL)
			{
				m_pPreviewExtent->m_nPreviewSlot = nSlot;
				m_pPreviewExtent->extent = InExtent;
				break;
			}
		}

	/*
	// Update the parameters for the next label to draw.
	*/

		nSlot++;
	}

/*
// Restore the scaled source x0 from the saved values.
*/

	printer_rc.source_pbox = SourcePbox;
	printer_rc.scaled_source_x0 = SourceOrigin.x;
	printer_rc.scaled_source_y0 = SourceOrigin.y;
}

/////////////////////////////////////////////////////////////////////////////
// CBizCardView

IMPLEMENT_DYNCREATE(CBizCardView, CLabelView)

CBizCardView::CBizCardView()
{
}

CBizCardView::~CBizCardView()
{
}

CPmwPrint* CBizCardView::CreatePrintDialog(void)
{
	return new CBusinessCardPrintDialog(this);
}

BEGIN_MESSAGE_MAP(CBizCardView, CLabelView)
	//{{AFX_MSG_MAP(CBizCardView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPostCardView

IMPLEMENT_DYNCREATE(CPostCardView, CLabelView)

CPostCardView::CPostCardView()
{
}

CPostCardView::~CPostCardView()
{
}

CPmwPrint* CPostCardView::CreatePrintDialog(void)
{
	return new CPostCardPrintDialog(this);
}

BEGIN_MESSAGE_MAP(CPostCardView, CLabelView)
	//{{AFX_MSG_MAP(CPostCardView)
	ON_COMMAND(ID_POSTCARD_VIEWFRONT, OnViewFront)
	ON_UPDATE_COMMAND_UI(ID_POSTCARD_VIEWFRONT, OnUpdateViewFront)
	ON_COMMAND(ID_POSTCARD_VIEWBACK, OnViewBack)
	ON_UPDATE_COMMAND_UI(ID_POSTCARD_VIEWBACK, OnUpdateViewBack)
	ON_COMMAND(ID_POSTCARD_TYPE, OnPostCardType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostCardView gadget handlers

BOOL CPostCardView::GadgetVisible(UINT uID)
{
	return (uID == ID_PAGE_GADGET) || CPmwView::GadgetVisible(uID);
}

void CPostCardView::GadgetText(UINT uID, CString& csText)
{
	switch (uID)
	{
		case ID_PAGE_GADGET:
		{
			CMlsStringArray PanelNames(IDS_POSTCARD_PANEL_NAMES);

			TRY
			{
				int nPanel = (int)GetDocument()->CurrentPageIndex();
				if ((nPanel < 0) || (nPanel >= (int)GetDocument()->NumberOfPages()))
				{
					csText.Empty();
				}
				else
				{
					csText = PanelNames.GetAt(nPanel);
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

LPCSTR CPostCardView::GadgetMenuName(UINT uID, UINT uPart)
{
	LPCSTR pszMenuName = NULL;

	if (uID == ID_PAGE_GADGET)
	{
		// JRO pszMenuName = "Brochure Page";
      pszMenuName = GET_PMWAPP()->GetResourceStringPointer (IDS_POSTCARD_PAGE);
	}
	else
	{
		pszMenuName = CPmwView::GadgetMenuName(uID, uPart);
	}

	return pszMenuName;
}

void CPostCardView::OnViewFront()
{
	GetDocument()->GotoPage(0);
}

void CPostCardView::OnUpdateViewFront(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->CurrentPageIndex() != 0);
}

void CPostCardView::OnViewBack()
{
	GetDocument()->GotoPage(1);
}

void CPostCardView::OnUpdateViewBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->CurrentPageIndex() != 1);
}

void CPostCardView::OnDraw(PrintContext* pc)
{
   INHERITED::OnDraw (pc);
}

SHORT CPostCardView::OnPrintDocument(PrintContext &pc,
													int32 aCurrentDocument, int32 aNumDocuments)
{
	return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}

void CPostCardView::OnPostCardType()
{
	CPmwDoc* pDoc = GetDocument();

	CPostCardTypeDialog Dialog(pDoc->GetPaperInfo(), this);

	if (Dialog.DoModal() == IDOK)
	{
		CLabelData* pLabel = Dialog.GetChosenPostCard();

		if (pLabel != NULL)
		{
		/* Get the paper info. */
			pDoc->AssignPaperInfo(*pLabel);
			pDoc->SizeToInternalPaper();
			pDoc->reset_views();
		}
	}
}
