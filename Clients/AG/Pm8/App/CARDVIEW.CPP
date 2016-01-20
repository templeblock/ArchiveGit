/*
// $Workfile: CARDVIEW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
c//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CARDVIEW.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 21    2/09/99 5:24p Psasse
// added PMGraphicServer account initialization to Online Greetings
// 
// 20    2/08/99 8:48p Psasse
// New improved scheduling of Online Greetings
// 
// 19    1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 18    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 17    10/15/98 3:44p Dennis
// New OCX Connection Manager
// 
// 16    9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 15    8/27/98 5:56p Dennis
// Changed no-standard Font check to YESNO messagebox prompt
// 
// 14    8/26/98 3:25p Mwilson
// added windows system fonts to allowable fonts
// 
// 13    8/26/98 10:36a Mwilson
// restore the doc state after personal delivery
// 
// 12    8/20/98 11:03a Jayn
// No transparency in GIF creation. Adds PPN to INI file sent. Overridable
// path in configuration.
// 
// 11    7/21/98 11:18a Mwilson
// fixes for personal delivery
// 
// 10    7/16/98 3:16p Mwilson
// changed path to temp project file
// 
// 9     7/08/98 3:28p Mwilson
// added check for unsupported project types for personal delivery
// 
// 8     6/17/98 5:05p Mwilson
// added check for pm fonts when doing personal delivery
// 
// 7     6/12/98 4:36p Mwilson
// moved handler for personal delivery from doc to view
// 
// 6     5/09/98 1:57p Psasse
// added #include "progbar.h" for ConnMgr support
// 
// 5     4/30/98 7:18p Psasse
// 
// 4     3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 3     1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 2     12/08/97 9:46a Dennis
// Changed return code check from BuildPanelDIB to check from Boolean
// value to ERRORCODE type.  Online greeting wasn't working.
// 
//    Rev 1.1   22 Oct 1997 11:29:36   dennisp
// Added error check when creating inside of card in OnCardServer() method.
// 
//    Rev 1.0   14 Aug 1997 15:18:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:24   Fred
// Initial revision.
// 
//    Rev 1.28   05 Aug 1997 16:57:36   Jay
// Removed message map entry for the text tool (was NOT right).
// 
//    Rev 1.27   24 Jul 1997 15:25:52   Jay
// Uses main frame as parent of progress dialog (not view).
// 
//    Rev 1.26   15 Jul 1997 16:21:54   Jay
// Check for WinSock DLL. SetErrorMode to prevent open error message box.
// 
//    Rev 1.25   10 Jul 1997 15:17:36   Jay
// Bad error message.
// 
//    Rev 1.24   10 Jul 1997 12:32:08   Jay
// Card server, etc.
// 
//    Rev 1.23   03 Jul 1997 10:48:30   Jay
// Online greetings now checks the configuration.
// 
//    Rev 1.22   30 Jun 1997 12:56:28   Jay
// New Card Server GIF dimensions.
// 
//    Rev 1.21   30 May 1997 12:43:20   Jay
// PasteSentiment
// 
//    Rev 1.20   23 May 1997 16:15:04   Jay
// Disclaimer dialog
// 
//    Rev 1.19   14 May 1997 10:16:42   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.18   07 May 1997 12:10:46   Jay
// User directory and collections
// 
//    Rev 1.17   05 May 1997 09:04:04   Jay
// Error message for Card Server
// 
//    Rev 1.16   05 May 1997 08:53:42   Jay
// Added DLL. Better error handling.
// 
//    Rev 1.15   18 Apr 1997 17:11:34   Jay
// Card server stuff
// 
//    Rev 1.14   17 Apr 1997 15:59:42   Jay
*/

#include "stdafx.h"

#include "mainfrm.h"

#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "carddoc.h"
#include "cardview.h"
#include "commands.h"
#include "desktop.h"
#include "sortarry.h"
#include "pmwcfg.h"
#include "broprint.h"
#include "gif.h"
#include <afxdtctl.h>
#include "cardsrvr.h"
#include "util.h"
#include "progbar.h"
#include "AddrInfoPage.h"
#include "BillingInfoPage.h"
#include "PDPropSt.h"
#include "fontrec.h"
#include "pmgfont.h"
#include "textrec.h"
#include "splshtxt.h"
#include "rmagcom.h"    // For CResourceManager
#include "docdib.h"		// For CDIBInfo
#include <sys/timeb.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//list of windows system fonts
static LPCSTR s_szaWindows95StandardFontName[] = 
{
   "Arial",
   "Courier New",
   "Symbol",
   "Times New Roman",
   "WingDings",
   NULL        // Last entry must be NULL which indicates end of list
};

extern CPalette* pOurPal;  // PrintMaster Palette

/////////////////////////////////////////////////////////////////////////////
// CCardView

IMPLEMENT_DYNCREATE(CCardView, CPmwView)

CCardView::CCardView()
{
}

CCardView::~CCardView()
{
}

BEGIN_MESSAGE_MAP(CCardView, CPmwView)
   //{{AFX_MSG_MAP(CCardView)
   ON_COMMAND(ID_CARD_VIEWBACK, OnCardViewback)
   ON_UPDATE_COMMAND_UI(ID_CARD_VIEWBACK, OnUpdateCardViewback)
   ON_COMMAND(ID_CARD_VIEWCARD, OnCardViewcard)
   ON_UPDATE_COMMAND_UI(ID_CARD_VIEWCARD, OnUpdateCardViewcard)
   ON_COMMAND(ID_CARD_VIEWFRONT, OnCardViewfront)
   ON_UPDATE_COMMAND_UI(ID_CARD_VIEWFRONT, OnUpdateCardViewfront)
   ON_COMMAND(ID_CARD_VIEWINSIDE, OnCardViewinside)
   ON_UPDATE_COMMAND_UI(ID_CARD_VIEWINSIDE, OnUpdateCardViewinside)
   ON_UPDATE_COMMAND_UI(ID_PICTURES_SELECT, OnUpdatePicturesSelect)
   ON_UPDATE_COMMAND_UI(ID_TEXT_SELECT, OnUpdateTextSelect)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_SPECIAL, OnUpdateEditPasteSpecial)
   ON_UPDATE_COMMAND_UI(ID_OLE_INSERT_NEW, OnUpdateInsertObject)
   ON_COMMAND(ID_FILE_CARDSERVER, OnCardServer)
   ON_UPDATE_COMMAND_UI(ID_FILE_CARDSERVER, OnUpdateCardServer)
   ON_UPDATE_COMMAND_UI(ID_PICTURES_IMPORT, OnUpdatePicturesSelect)
   ON_UPDATE_COMMAND_UI(ID_PICTURES_DRAWING_OBJECT, OnUpdatePicturesSelect)
	ON_COMMAND(ID_PERSONAL_DELIVERY, OnPersonalDelivery)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardView drawing

void CCardView::OnDraw(CDC* pDC)
{
   if (pDC->IsPrinting())
   {
      CCardDoc *pDoc = GetCardDocument();

   /*
   // Flip around for correct printing and show all objects.
   // This does nothing if current panel is already -1.
   */

      SHORT panel_save = pDoc->get_current_panel();
      BOOL fAddRefreshes = pDoc->AddRefreshes();

      pDoc->AddRefreshes(FALSE);
      pDoc->set_panel(-2);
      pDoc->AddRefreshes(fAddRefreshes);

      CPmwView::OnDraw(pDC);

      pDoc->AddRefreshes(FALSE);
      pDoc->set_panel(panel_save);
      pDoc->AddRefreshes(fAddRefreshes);
   }
   else
   {
   /* Just draw. */
      CPmwView::OnDraw(pDC);
   }
}

void CCardView::OnInitialUpdate()
{
   CPmwApp *app = (CPmwApp *)AfxGetApp();

////  GetCardDocument()->set_panel(-1);      /* Full card view. */
   GetCardDocument()->set_panel(0);    /* Front card view. */

   CPmwView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CCardView operations


/*
// Set the card panel.
// This uses a CCmdCardPanel command to do the change.
*/

static WORD IDCmd_Panel[] =
{
   IDCmd_ViewCard,            /* -1 */
   IDCmd_ViewFront,           /*  0 */
   IDCmd_ViewInside,          /*  1 */
   IDCmd_ViewBack             /*  2 */
};

VOID CCardView::set_card_panel_view(SHORT new_panel)
{
/*
// If this is a demo card back, then we don't allow editing.
*/
   if ((new_panel == CARD_PANEL_Back) && !GetConfiguration()->SupportsCardBack())
   {
      return;
   }

#if 1
   BeforePageChange();
   SetPanel(new_panel);
#else
/*
// Get the document and build the undo/redo command for this.
*/

   CPmwDoc* pDoc = GetDocument();

   CCmdCardPanel* pCommand = new CCmdCardPanel(IDCmd_Panel[new_panel+1]);

   if (pCommand->Snapshot(this, pDoc->get_current_panel(), new_panel))
   {
      pDoc->AddCommand(pCommand);
   }
   else
   {
      delete pCommand;

   /* Fall through. */
//    SetPanel(new_panel);
   }
#endif
}

/*
// Actually set the card panel.
// This will change the view to that panel.
*/

void CCardView::SetPanel(int new_panel, BOOL fUpdateMenu /*=TRUE*/)
{
   CCardDoc *pDoc = GetCardDocument();

   if (new_panel != pDoc->get_current_panel())
   {
      if (pDoc->set_panel(new_panel))
      {
      /* Don't leave old objects selected as we change panels. */
         pDoc->deselect_all(fUpdateMenu);

         pDoc->reset_views();
         pDoc->UpdateAllViews(NULL);
      }
   }
}

VOID CCardView::check_card_panel(CCmdUI* pCmdUI, SHORT panel)
{
   pCmdUI->SetCheck(GetCardDocument()->get_current_panel() == panel);
}

/////////////////////////////////////////////////////////////////////////////
// CCardView message handlers

void CCardView::OnCardViewback()
{
   set_card_panel_view(CARD_PANEL_Back);
}

void CCardView::OnCardViewcard()
{
   set_card_panel_view(-1);
}

void CCardView::OnCardViewfront()
{
   set_card_panel_view(CARD_PANEL_Front);
}

void CCardView::OnCardViewinside()
{
   set_card_panel_view(CARD_PANEL_Inside);
}

void CCardView::OnUpdateCardViewback(CCmdUI* pCmdUI)
{
   check_card_panel(pCmdUI, CARD_PANEL_Back);
}

void CCardView::OnUpdateCardViewcard(CCmdUI* pCmdUI)
{
   check_card_panel(pCmdUI, -1);
}

void CCardView::OnUpdateCardViewfront(CCmdUI* pCmdUI)
{
   check_card_panel(pCmdUI, CARD_PANEL_Front);
}

void CCardView::OnUpdateCardViewinside(CCmdUI* pCmdUI)
{
   check_card_panel(pCmdUI, CARD_PANEL_Inside);
}

void CCardView::OnUpdateBackgroundSelect(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->get_current_panel() != -1);
}

void CCardView::OnUpdatePicturesSelect(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->get_current_panel() != -1);
}

void CCardView::OnUpdateTextSelect(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->get_current_panel() != -1);
}

void CCardView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
   if (GetDocument()->get_current_panel() == -1)
   {
      pCmdUI->Enable(FALSE);
   }
   else
   {
      CPmwView::OnUpdateEditPaste(pCmdUI);
   }
}

void CCardView::OnUpdateEditPasteSpecial(CCmdUI* pCmdUI)
{
   if (GetDocument()->get_current_panel() == -1)
   {
      pCmdUI->Enable(FALSE);
   }
   else
   {
      CPmwView::OnUpdateEditPasteSpecial(pCmdUI);
   }
}

void CCardView::OnUpdateInsertObject(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(GetDocument()->get_current_panel() != -1);
}

void CCardView::OnCardServer()
{
	// Some configurations do not support online greetings ("card server").
	if (!GetConfiguration()->SupportsCardServer())
	{
		return;
	}

	// Get our document.
	CPmwDoc* pDoc = GetDocument();

	if(GetGlobalContentManager()->CheckAccountStatus())
	{
		CString strID = GetGlobalContentManager()->GetAccountID();
		if(strID != "")
		{
			// Put up the card server dialog.

			CCardServerDialog Dialog(this);

			Dialog.m_ctTime = CTime::GetCurrentTime();
			if (Dialog.DoModal() == IDOK)
			{
				// Put up the disclaimer dialog.
				CCardServerDisclaimer Disclaimer(this);

				if (Disclaimer.DoModal() == IDOK)
				{
					// Create the directory we want to use as our FTP source.
					CString csDir = pDoc->GetPathManager()->ExpandPath("[[U]]\\NETCARD");

					if (!Util::MakeDirectory(csDir))
					{
						// Not able to make the directory.
						// LPCSTR pFormat = "Can't create the directory\n%s";
						LPCSTR pFormat = GET_PMWAPP()->GetResourceStringPointer(IDS_ErrCreateDirectory);
						CString csMessage;
						csMessage.Format(pFormat, (LPCSTR)csDir);
						AfxMessageBox(csMessage);
						return;
					}

					// Construct the text file.
					CString csTextFile;
					Util::ConstructPath(csTextFile, csDir, "INFO.INI");

					CFile cfText;
					CString csVersion;
					CString csPPN;
					TRY
					{
						CFileException e;
						//if (!cfText.Open(csTextFile,
						//					  CFile::modeCreate
						//							| CFile::modeWrite,
						//					  &e))
						//{
						//	AfxThrowFileException(e.m_cause, e.m_lOsError);
						//}

						//do the time stuff 
						
						//time zone difference from PST
						struct _timeb tstruct;
						_ftime( &tstruct );
						int timecorrection = PacificTimeCorrection(tstruct.timezone);
						
						//get date of delivery - include current time
						struct tm when;   
						CString strDate;
						CTime testtime = CTime::GetCurrentTime();
						when = *testtime.GetLocalTm(NULL);

						SYSTEMTIME sysTime = *Dialog.GetTime();
						
						if((when.tm_mday != sysTime.wDay) || (when.tm_mon != (sysTime.wMonth - 1)))
						{
							when.tm_mday = sysTime.wDay;
							when.tm_mon = sysTime.wMonth - 1;

							ConvertToPST(&when, timecorrection);

							CString str_ampm = "AM";
						  
							if( when.tm_hour > 12 )        /* Set up extension. */
									 str_ampm = "PM";
							if( when.tm_hour > 12 )        /* Convert from 24-hour */
									 when.tm_hour -= 12;    /*   to 12-hour clock.  */
							if( when.tm_hour == 0 )        /*Set hour to 12 if midnight. */
									 when.tm_hour = 12;
							
							strDate.Format("%d/%d/%d %d:%d:%d%s", 
													(int)((int)when.tm_mon + 1),when.tm_mday,when.tm_year,when.tm_hour,when.tm_min,when.tm_sec,str_ampm);   
						}
						else
						{
							strDate = "";
						}
						
						CString strPSTCorrection;
						strPSTCorrection.Format("%d", timecorrection);   

						// Construct the version string.
						CString csEol = "\r\n";
						csVersion = GetConfiguration()->ReplacementText('T');
						csVersion += ' ';
						csVersion += GET_PMWAPP()->GetVersion();

						csPPN = GET_PMWAPP()->GetParentPartNumber();

						WritePrivateProfileString( "UserData", "UserId",
													GetGlobalContentManager()->GetAccountID(), csTextFile);
						
						WritePrivateProfileString( "UserData", "Description",
													csVersion, csTextFile);

						WritePrivateProfileString( "UserData", "SenderEmail",
													Dialog.m_csFrom, csTextFile);

						WritePrivateProfileString( "UserData", "RecipientEmail",
													Dialog.m_csTo, csTextFile);

						WritePrivateProfileString( "UserData", "sku",
													csPPN, csTextFile);


						WritePrivateProfileString( "CardData", "Title",
													Dialog.m_csTitle, csTextFile);

						WritePrivateProfileString( "CardData", "Date",
													strDate, csTextFile);

						WritePrivateProfileString( "CardData", "Offset",
													strPSTCorrection, csTextFile);


		#if 0 
						// Write version
						cfText.Write(csVersion, csVersion.GetLength());
						cfText.Write(csEol, csEol.GetLength());
						// Write "from" email address
						cfText.Write(Dialog.m_csFrom, Dialog.m_csFrom.GetLength());
						cfText.Write(csEol, csEol.GetLength());
						// Write "to" email address
						cfText.Write(Dialog.m_csTo, Dialog.m_csTo.GetLength());
						cfText.Write(csEol, csEol.GetLength());
						// Write Parent Part Number
						cfText.Write(csPPN, csPPN.GetLength());
						cfText.Write(csEol, csEol.GetLength());
		#endif

						//cfText.Close();
					}
					CATCH_ALL(e)
					{
						// Could not write the text file. Complain.
						// LPCSTR pFormat = "Can't create the file\n%s";
						LPCSTR pFormat = GET_PMWAPP()->GetResourceStringPointer(IDS_ErrCreateFile);
						CString csMessage;
						csMessage.Format(pFormat, (LPCSTR)csTextFile);
						AfxMessageBox(csMessage);
						return;
					}
					END_CATCH_ALL

					// Figure out how big we want the panels to be.
					CPoint cpFrontDims;
					CPoint cpInsideDims;

					PBOX World;
					pDoc->get_panel_world(&World, CARD_PANEL_Front);

					// Fit the front panel into a 425x425 square.
					PPNT FrontDims;
					FrontDims.x = World.x1 - World.x0;
					FrontDims.y = World.y1 - World.y0;

					double dScale = 425.0/(double)FrontDims.x;
					double dYScale = 425.0/(double)FrontDims.y;
					if (dScale > dYScale) dScale = dYScale;

					cpFrontDims.x = (int)((double)FrontDims.x*dScale);
					cpFrontDims.y = (int)((double)FrontDims.y*dScale);

					pDoc->get_panel_world(&World, CARD_PANEL_Inside);

					cpInsideDims.x = (int)((double)(World.x1-World.x0)*dScale);
					cpInsideDims.y = (int)((double)(World.y1-World.y0)*dScale);

					TRACE("Front: %d, %d; Inside: %d, %d\n",
							cpFrontDims.x, cpFrontDims.y,
							cpInsideDims.x, cpInsideDims.y);

					BOOL fSuccess = FALSE;

					CCardServerProgressDialog ProgressDialog(AfxGetMainWnd());

					CString csName;
					Util::ConstructPath(csName, csDir, "front.gif");
					ProgressDialog.SetStatus(IDS_BuildingFront);
					if (DumpPanel(CARD_PANEL_Front, csName, cpFrontDims))
					{
						ProgressDialog.SetStatus(IDS_BuildingInside);
						Util::ConstructPath(csName, csDir, "inside.gif");
						if(DumpPanel(CARD_PANEL_Inside, csName, cpInsideDims))
   						fSuccess = TRUE;
					}

					if (!fSuccess)
					{
						// "Can't create card pictures."
						AfxMessageBox(GET_PMWAPP()->GetResourceStringPointer(IDS_ErrWriteCardPictures));
						return;
					}

					// Now, invoke the DLL to write the data.
					CCardServerDLL DLL;
		#ifdef WIN32
					CString csDLLName = pDoc->GetPathManager()->ExpandPath("FTP32.DLL");
		#else
					CString csDLLName = pDoc->GetPathManager()->ExpandPath("FTP16.DLL");
		#endif
					// Startup the dialog.
					if (DLL.Startup(csDLLName))
					{
						if(CheckForValidConnection()){
							
							// Put up our progress dialog.
							// Send the files.
							int nResult = DLL.SendFiles(csDir, ProgressDialog);

							// Take down the progress dialog now.
							ProgressDialog.DestroyWindow();

							// Shut down the DLL.
							DLL.Shutdown();

							CPmwDialog Dialog(nResult == 0 ? IDD_CARD_SERVER_SUCCESS : IDD_CARD_SERVER_FAILURE);
							Dialog.DoModal();
						}
					}
					else
					{
						// Not able to load the DLL. Complain.
						// LPCSTR pFormat = "The file\n%s\nis missing or bad.";
						LPCSTR pFormat = GET_PMWAPP()->GetResourceStringPointer(IDS_ErrMissingDLL);
						CString csMessage;
						csMessage.Format(pFormat, (LPCSTR)csDLLName);
						AfxMessageBox(csMessage);
					}
				}
			}
		}
	}
}

int CCardView::PacificTimeCorrection(short zone)
{
	int PSTDifference = 0;

	int ZoneHours = zone / 60;

	if(ZoneHours <= 12)
		PSTDifference = 8 - ZoneHours;
	else
		PSTDifference = 24 - ZoneHours + 8;

	return PSTDifference;
}

BOOL CCardView::ConvertToPST(tm* when, int timecorrection)
{
	BOOL result = FALSE;
	
	when->tm_hour = when->tm_hour + 8; //timecorrection;
   if( (result = mktime( when )) != (time_t)-1 )
		return TRUE;
	else
		return FALSE;

}

BOOL CCardView::DumpPanel(int nPanel, LPCSTR pszGIFName, POINT MaxDims)
{
	extern const RGBQUAD* GetIdentityPalette(void);
   CCardDoc* pDoc = GetCardDocument();

   int nOldPanel = pDoc->get_current_panel();

   // Set the panel we want.
   pDoc->set_panel(nPanel);

   BOOL fRet = FALSE;

   // Build the preview.

   CDIBInfo Info;
   if (pDoc->BuildPanelDIB(Info, MaxDims, FALSE) == ERRORCODE_None)
   {
      StorageFile File(pszGIFName);
      if (File.initialize() == ERRORCODE_None)
      {
         // We have a destination file.
         // Setup the data sink.
         CGIFDataSinkDevice Sink(&File);
         CDIBtoGIFWriter Writer;

         TRY
         {
            Writer.CreateGIF(Info.BitmapInfo(), Info.BitmapData(), &Sink, FALSE);
            fRet = TRUE;
         }
         CATCH_ALL(e)
         {
            // Some sort of extension.
            fRet = FALSE;     // Make sure...
         }
         END_CATCH_ALL
      }
   }

   // Restore the old panel.
   pDoc->set_panel(nOldPanel);

   return fRet;
}

void CCardView::OnUpdateCardServer(CCmdUI* pCmdUI)
{
	// Make sure we can load the WinSock dll.
	// We only check this once. You will need to restart
	// PrintMaster if Internet support suddenly becomes available.
	static BOOL fHasWinSock = -1;		// Not checked yet.

	if (fHasWinSock == -1)
	{
		// Determine if the WinSock DLL is available.
		UINT wOldSem = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
#ifdef WIN32
		HINSTANCE hDLL = ::LoadLibrary("WSOCK32.DLL");
		if (hDLL == NULL)
#else
		HINSTANCE hDLL = ::LoadLibrary("WINSOCK.DLL");
		if (hDLL < HINSTANCE_ERROR)
#endif
		{
			fHasWinSock = FALSE;
		}
		else
		{
			fHasWinSock = TRUE;
			::FreeLibrary(hDLL);
		}
		::SetErrorMode(wOldSem);
	}

	// Enabled only if we know that the WinSock DLL is available.
	pCmdUI->Enable(fHasWinSock == TRUE);
}

/*
// Create the edit desktop for a card.
*/

void CCardView::CreateEditDesktop(void)
{
   m_pDesktop = new CCardEditDesktop(this);
}

BOOL CCardView::CanInsertPictures(void)
{
   return CPmwView::CanInsertPictures() && (GetDocument()->get_current_panel() != -1);
}

// Gadget interface.
BOOL CCardView::GadgetVisible(UINT uID)
{
   return (uID == ID_PAGE_GADGET) || CPmwView::GadgetVisible(uID);
}

BOOL CCardView::GadgetEnabled(UINT uID, UINT uPart)
{
   BOOL fResult = FALSE;

   switch (uID)
   {
      case ID_PAGE_GADGET:
      {
         int nMinPanel = 0;
         int nMaxPanel = 2;
         int nCurPanel = GetDocument()->get_current_panel();
         switch (uPart)
         {
            case ID_GADGET_BACK:
            {
               fResult = nCurPanel > nMinPanel;
               break;
            }
            case ID_GADGET_TEXT:
            {
               fResult = TRUE;
               break;
            }
            case ID_GADGET_NEXT:
            {
               fResult = nCurPanel < nMaxPanel;
               break;
            }
         }
         break;
      }
      default:
      {
         fResult = CPmwView::GadgetEnabled(uID, uPart);
      }
   }

   return fResult;
}

void CCardView::GadgetClicked(UINT uID, UINT uPart, CWnd* pGadget)
{
   switch (uID)
   {
      case ID_PAGE_GADGET:
      {
         switch (uPart)
         {
            case ID_GADGET_BACK:
            {
               int nMinPanel = 0;
               int nCurPanel = GetDocument()->get_current_panel();
               if (nCurPanel > nMinPanel)
               {
                  set_card_panel_view(nCurPanel-1);
               }
               break;
            }
            case ID_GADGET_NEXT:
            {
               int nMaxPanel = 2;
               int nCurPanel = GetDocument()->get_current_panel();
               if (nCurPanel < nMaxPanel)
               {
                  set_card_panel_view(nCurPanel+1);
               }
               break;
            }
            default:
            {
               CPmwView::GadgetClicked(uID, uPart, pGadget);
               break;
            }
         }
         break;
      }
      default:
      {
         CPmwView::GadgetClicked(uID, uPart, pGadget);
         break;
      }
   }
}

void CCardView::GadgetText(UINT uID, CString& csText)
{
   switch (uID)
   {
      case ID_PAGE_GADGET:
      {
         CMlsStringArray PanelNames(IDS_CARD_PANEL_NAMES);

         TRY
         {
            int nPanel = GetDocument()->get_current_panel();
            if ((nPanel < 0) || (nPanel >= PanelNames.GetSize()))
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

LPCSTR CCardView::GadgetMenuName(UINT uID, UINT uPart)
{
   LPCSTR pszMenuName = NULL;

   switch (uID)
   {
      case ID_PAGE_GADGET:
      {
         pszMenuName = "Card Page";
         break;
      }
      default:
      {
         pszMenuName = CPmwView::GadgetMenuName(uID, uPart);
         break;
      }
   }

   return pszMenuName;
}

//
// Sentiment support.
//

void CCardView::PasteSentiment(const CMlsStringArray& csaSentiment)
{
	CCardDoc* pDoc = GetCardDocument();
	// Get the current panel.
	int nPanel = pDoc->get_current_panel();

	// Prepare!
	set_arrow_tool();
	GetDocument()->deselect_all();

	// Paste front if we have one.
	if (!csaSentiment.GetAt(0).IsEmpty())
	{
		// Paste centered on front.
		pDoc->set_panel(CARD_PANEL_Front);
      CPmwView::PasteSentiment(csaSentiment.GetAt(0), -1, FALSE, nPanel == CARD_PANEL_Front);
	}
	// Paste inside if we have one.
	if (!csaSentiment.GetAt(1).IsEmpty())
	{
		pDoc->set_panel(CARD_PANEL_Inside);
		// Paste right/bottom of inside.
		CPmwView::PasteSentiment(csaSentiment.GetAt(1), 1, TRUE, nPanel == CARD_PANEL_Inside);
	}
	// Make sure we are back to square one.
	pDoc->set_panel(nPanel);
}

UINT CCardView::CheckForValidConnection()
{
	IConnectionManager* pConnectionManager = GetGlobalConnectionManager();
	if (pConnectionManager == NULL)
	{
		// 0 means not connected? Perhaps a BOOL return code would be better...
		return 0;
	}

	// Try to establish connection
	if(pConnectionManager->Connect() != CInternetConnectStatus::statusConnected)
	{
		AfxMessageBox(IDS_PGS_NO_INTERNET_CONNECTION);
		return 0;
	}

	return 1;
}

//check to see if the only fonts used are printmaster fonts
BOOL CCardView::CheckFonts(CCardDoc* pDoc)
{
	//create a mapping of all printmaster fonts the first time through
	static CMapStringToPtr map;
	if(map.IsEmpty())
	{
		TRY
		{
			CString csLine;
			CStdioFile cfInput;
			CString csInput = GetDocument()->GetPathManager()->ExpandPath("Pmw.ffl");
			//open printmaster font file
			if (!cfInput.Open(csInput, CFile::modeRead|CFile::shareDenyNone|CFile::typeText))
			{
				return FALSE;
			}
			//skip the first few lines.  font lines start with a 1 or 0
			while(TRUE)
			{
				Util::ReadString(&cfInput, csLine);
				if(!csLine.IsEmpty())
				{
					if(csLine[0] == '0' || csLine[0] == '1')
						break;
				}
			}
			//value for mapping
			DWORD mapValue = 0;
			do
			{
				//get the face name
				int nCharIndex;
				CString csFont = csLine.Right(csLine.GetLength() - 1 - csLine.ReverseFind(' '));
				//convert | to spaces
				while((nCharIndex = csFont.Find('|')) != -1)
					csFont.SetAt(nCharIndex, ' ');
				
				//add string to map
				map.SetAt(csFont, (void*)mapValue);
				mapValue++;

			}while(Util::ReadString(&cfInput, csLine));
			
			//add windows system fonts to map
			for(int i=0;s_szaWindows95StandardFontName[i] != NULL ;i++, mapValue++)
			{
				//get the face name
				CString csFont = s_szaWindows95StandardFontName[i];
		
				//add string to map
				map.SetAt(csFont, (void*)mapValue);
			}

		}
		CATCH_ALL(e)
		{
			return FALSE;
		}
		END_CATCH_ALL
	}

	//check the project fonts
	CTextRecord       *pTextRecord;
	CTextIterator     textIterator;
	CTextStyle        textStyle;
	CHARACTER_INDEX   charIndex;
	CString csPrevFace;
	csPrevFace.Empty();

	DocumentRecord* pDocRec = pDoc->DocumentRecord();
	if(pDocRec == NULL)
	{
		ASSERT(pDocRec);
		return FALSE;
	}
	PMGDatabase* pDatabase = pDoc->get_database();
	DWORD dwStories = pDocRec->NumberOfStories();
	//loop through stories
	for (DWORD dwStoryIndex = 0; dwStoryIndex < dwStories; dwStoryIndex++)
	{
		DB_RECORD_NUMBER lStory = pDocRec->GetStory(dwStoryIndex);

		// Lock the text record.
		pTextRecord = (CTextRecord*)(pDatabase->get_record(lStory, NULL, RECORD_TYPE_Text));
		if (pTextRecord != NULL)
		{
			textIterator.SetRecord(pTextRecord);
			PMGFontServer* pFontServer = (PMGFontServer*)(pDatabase->get_font_server());
			charIndex = 0;
			//check each story.  
			do
			{
				//set position to next style change
				textIterator.SetPosition(charIndex);
				textStyle = textIterator.Style();
				DB_RECORD_NUMBER recNum = textStyle.Font();
				FontDataRecord* pFontRec = (FontDataRecord*)pDatabase->get_record(recNum, NULL, RECORD_TYPE_FontData);
				if(pFontRec)
				{
					CString csFace = pFontRec->get_face_name();
					//if it is not the same font check the map to see if it is a pm font
					if(csPrevFace.IsEmpty() || csPrevFace != csFace)
					{
						void* p;
						if(!map.Lookup(csFace, p))
							return FALSE;
					}
					csPrevFace = csFace;
					pFontRec->release();
				}

				// Get next style change
				charIndex = textIterator.NextChangeStart();
			}while(charIndex != -1);
			pTextRecord->release();
		}  
	}
	return TRUE;
}
void CCardView::OnPersonalDelivery() 
{
	CCardDoc* pCurDoc = (CCardDoc*)GetDocument();
	ProjectInfo info = pCurDoc->GetProjectInfo();
	if(info.project_type == PROJECT_TYPE_Card)
	{
		if(info.project_sub_type == CARD_SideFold)
		{
			if(pCurDoc->get_orientation() == LANDSCAPE)
			{	
				AfxMessageBox(PERSONAL_DEL_NOT_SUPPORTED);
				return;
			}
		}
		else
		{
			if(pCurDoc->get_orientation() == PORTRAIT)
			{	
				AfxMessageBox(PERSONAL_DEL_NOT_SUPPORTED);
				return;
			}
		}
	}
	BeginWaitCursor();
	CString csSplashText;
	csSplashText.LoadString(IDS_PD_WAIT);
	CBitmap cbSplashBkgnd;
	{
		CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());
		rlProductSpecific.LoadBitmap(cbSplashBkgnd, CSharedIDResourceManager::riArtGallerySidebar, pOurPal);
	}
	CSplashText splash(csSplashText, cbSplashBkgnd, GetSysColor(COLOR_BACKGROUND));
	splash.Create(IDD_SPLASH_TEXT);
	if(!CheckFonts(pCurDoc))
	{
		int nRetVal = AfxMessageBox(IDS_PERSONAL_DELIV_FONT_PROMPT, MB_YESNO);
      if(nRetVal != IDYES)
   		return;
	}

	BOOL bNeedToSave = FALSE;
	int nResult = IDCANCEL;
	//save the modified flag
	BOOL bModified = pCurDoc->IsModified();

	SHORT nFileType = pCurDoc->get_file_type();
	CString csBrowserPath = pCurDoc->GetBrowserPath();
	//create the path for a temp file
	CDocTemplate* pTemplate = pCurDoc->GetDocTemplate();
	CString csExt;
	pTemplate->GetDocString(csExt, CDocTemplate::filterExt);
	CString csTempFileName = pCurDoc->GetPathManager()->ExpandPath("@@Temp");
	csTempFileName += csExt;
	//save the project off as a temp file
	if(pCurDoc->SaveToFile(csTempFileName, FALSE) == FALSE)
	{
		AfxMessageBox(IDS_ERR_SAVE_PD);
		return;	
	}
	//open up the saved temp file
	CCardDoc* pDoc = (CCardDoc*)GET_PMWAPP()->OpenHiddenDocument(csTempFileName);
	if(pDoc == NULL)
	{
		ASSERT(0);
		AfxMessageBox(IDS_ERR_CREATE_PD_DOC);
		return;
	}
	//resave the file
	if(pDoc->SaveToFile(csTempFileName, FALSE, TRUE) == FALSE)
	{
		AfxMessageBox(IDS_ERR_SAVE_PD);
		return;
	}
	pDoc->OnCloseDocument();

	pCurDoc->set_file_type(nFileType);
	if(!csBrowserPath.IsEmpty())
		pCurDoc->SetBrowserPath(csBrowserPath);

	//make sure the original docs modified flag is set right
	pCurDoc->SetModifiedFlag(bModified);
	//bring up the personal delivery dialog
	CPDPropSheet dlg(IDS_PD_TITLE);
	CAddrInfoPage page1;
	CBillingInfoPage page2(csTempFileName, pCurDoc->get_project_type());

	CString csPath = pCurDoc->GetPathManager()->ExpandPath("[[U]]\\PDInfo.dat");
	int nLength = 0;
	BYTE* pData = NULL;
	TRY
	{

		//get persistant data.
		CFile pdInfoFile(csPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite);
		nLength = pdInfoFile.GetLength();
		if(nLength)
		{
			pData = new BYTE[nLength];
			pdInfoFile.Read(pData, nLength);
		}

		char seps[] = "\\\\";
		if(pData != NULL)
		{
			char* pToken = strtok((char*)pData, seps);
			page1.m_csReturnAddress = pToken;
			pToken = strtok(NULL, seps);
			page2.m_csBillingName = pToken;
			pToken = strtok(NULL, seps);
			page2.m_csBillingAddr = pToken;
		}
		if(pData)
			delete pData;

		dlg.AddPage(&page1);
		dlg.AddPage(&page2);
		dlg.SetWizardMode();
		splash.DestroyWindow();
		if(dlg.DoModal() == ID_WIZFINISH)
		{
			//write persistant data.
			pdInfoFile.SeekToBegin();
			pdInfoFile.Write(page1.m_csReturnAddress, page1.m_csReturnAddress.GetLength());
			pdInfoFile.Write(seps, strlen(seps));
			pdInfoFile.Write(page2.m_csBillingName, page2.m_csBillingName.GetLength());
			pdInfoFile.Write(seps, strlen(seps));
			pdInfoFile.Write(page2.m_csBillingAddr, page2.m_csBillingAddr.GetLength());
			pdInfoFile.Write(seps, strlen(seps));
		}
		
		
		pdInfoFile.Close();
		CFile::Remove(csTempFileName);

		EndWaitCursor();
	}
	CATCH(CFileException, e)
	{
		AfxMessageBox(IDS_PD_FILE_ERROR);
	}
	END_CATCH
	
}


/////////////////////////////////////////////////////////////////////////////
// CHalfCardView

IMPLEMENT_DYNCREATE(CHalfCardView, CCardView)

CHalfCardView::CHalfCardView()
{
}

CHalfCardView::~CHalfCardView()
{
}

BEGIN_MESSAGE_MAP(CHalfCardView, CCardView)
   //{{AFX_MSG_MAP(CHalfCardView)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

SHORT CHalfCardView::OnPrintDocument(PrintContext &pc,
                     int32 aCurrentDocument, int32 aNumDocuments)
{
   return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}

CPmwPrint* CHalfCardView::CreatePrintDialog(void)
{
   return new CBrochurePrintDialog(this);
}

/////////////////////////////////////////////////////////////////////////////
// CNoteCardView

IMPLEMENT_DYNCREATE(CNoteCardView, CCardView)

CNoteCardView::CNoteCardView()
{
}

CNoteCardView::~CNoteCardView()
{
}

BEGIN_MESSAGE_MAP(CNoteCardView, CCardView)
   //{{AFX_MSG_MAP(CNoteCardView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



