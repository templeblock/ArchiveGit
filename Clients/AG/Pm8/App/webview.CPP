/*
// $Workfile: webview.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/webview.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 60    2/19/99 11:31a Johno
// Moved OnWebSiteProperties to pmwview.cpp
// 
// 59    2/05/99 5:31p Johno
// Disabled master page stuff for web projects
// 
// 58    2/03/99 5:13p Johno
// Fixed web page print preview crash
// 
// 57    1/25/99 4:36p Johno
// Moved OnPageProperties() from here to PmwView
// 
// 56    1/21/99 6:13p Johno
// Changes for links, hotspots, and text links!
// 
// 55    1/04/99 5:12p Johno
// Moved CWebView / CWebDoc code into CPmwDoc / CPmwView.
// 
// 54    12/22/98 10:30a Johno
// Moved most HTML conversion stuff to pmwview.*
// 
// 53    10/19/98 10:50a Jayn
// More COM restructuring changes.
// 
// 52    10/15/98 3:47p Dennis
// New OCX Connection Manager
// 
// 51    10/13/98 5:07p Dennis
// Fixed settings retrieval bug
// 
// 50    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 49    7/22/98 10:02a Jayn
// Fixed the flashing red border.
// 
// 48    7/17/98 5:53p Johno
// Added WebPublishWarnGet(), QueryPublish() uses new CWebPublishQuery
// dialog
// 
// 47    7/17/98 3:13p Johno
// Avoid integer rounding problems in OnDraw() (print preview)
// 
// 46    7/14/98 11:10a Johno
// OnDraw over ride for print preview, for the case of a web project
// larger than the printer's paper.
// 
// 45    7/12/98 5:34p Jayn
// Fix for red flashing border.
// 
// 44    7/11/98 3:09p Johno
// Added QueryPublish()
// 
// 43    7/01/98 5:38p Johno
// Ignores disabled hyperlinks in SelectFullHyperlink()
// 
// 42    6/26/98 4:42p Johno
// Got rid of unused CTextStyle
// 
// 41    6/05/98 2:12p Jayn
// Overlapping object flashing rectangle(s) in web projects.
// 
// 40    6/04/98 2:19p Johno
// Changes for doing HTML format changes on grouped objects
// 
// 39    6/03/98 2:47p Johno
// Changes for "run design checker" menu option
// 
// 38    6/03/98 1:45p Johno
// Added DesignCheckState methods
// 
// 37    6/02/98 7:04p Johno
// Simplifications / changes / debugs to find HTML positioning bug
// 
// 36    6/02/98 12:00p Johno
// Tweaks
// 
// 35    5/31/98 5:27p Johno
// Functional HTML warning rectangle
// 
// 34    5/29/98 5:57p Johno
// HTML Format menu now enabled for multiple text object selection
// 
// 33    5/28/98 4:58p Johno
// Changes for HTML compliance groups
// 
// 32    5/28/98 10:20a Johno
// Compile update
// 
// 31    5/26/98 6:56p Johno
// 
// 30    5/26/98 2:00p Dennis
// Added call to GetConfiguration()->ReplaceText() so app. name can be
// substituted in string.
// 
// 29    5/22/98 4:45p Johno
// WebCheck INI file controlled
// 
// 28    5/20/98 12:14p Johno
// Changes for design checker
// 
// 27    5/19/98 11:08a Johno
// Disabled layer menu control for hotspots.
// 
// 26    5/18/98 4:28p Johno
// Oops - missed OnUpdateLayerToBack
// 
// 25    5/18/98 4:08p Johno
// Changes for layer menu enable and hotspots
// 
// 24    5/11/98 12:07p Johno
// Undo for web text publishing changes
// 
// 23    5/11/98 9:17a Johno
// Compile update
// 
// 22    5/09/98 1:57p Psasse
// added #include "progbar.h" for ConnMgr support
// 
// 21    5/07/98 4:53p Johno
// Added interface for control of conversion of text objects to HTML
// 
// 20    4/20/98 4:40p Johno
// Added scaling web pages to paper size for printing (OnPreparePrinting)
// 
// 19    4/13/98 4:54p Johno
// Undo logic in DoHyperlinkObject()
// 
// 18    4/13/98 9:59a Johno
// Initial search for hyper link in SelectFullHyperlink now only searches
// within current selection.
// If one is not found, the selection is not changed.
// 
// 17    4/07/98 4:27p Johno
// Fixed bugs in SelectFullHyperlink().
// Bugs - everywhere! Look - there's one now - over there!
// 
// 16    4/06/98 5:18p Johno
// SelectFullHyperlink now works - it changes text selection bounds to
// contain one, and only one, complete hyperlink
// 
// 15    4/03/98 4:43p Johno
// Changes for page links
// 
// 14    3/31/98 11:44a Jayn
// Hyperlink record reference count fixes.
// 
// 13    3/27/98 5:51p Johno
// Changes to DoHyperlinkText
// 
// 12    3/26/98 10:21a Johno
// Changed hyplnkdlg.h to hyplkdlg.h
// 
// 11    3/25/98 6:38p Johno
// Changes for text links
// 
// 10    3/24/98 5:25p Johno
// Split OnWebHyperlink into  DoHyperlinkText and  DoHyperlinkObject.
// Added  SelectFullHyperlink (incomplete);
// 
// 9     3/11/98 5:54p Johno
// Use HyperlinkData struct with CHyperlinkDialog
// 
// 8     3/10/98 5:24p Jayn
// Master page changes
// 
// 7     3/10/98 4:19p Johno
// Got rid of SetObjectHyperlink, changed OnWebHyperlink
// 
// 6     3/06/98 5:50p Johno
// Added CWebView::SetObjectHyperlink
// 
// 5     3/05/98 6:31p Johno
// Various changes for Hyperlink
// 
// 4     3/02/98 5:04p Johno
// Compile update
// 
// 3     2/24/98 9:24a Dennis
// Added WebEditDesktop to take control of background drawing.
// 
// 2     12/11/97 4:25p Dennis
// 
// 1     12/11/97 1:43p Dennis
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "webview.h"
#include "nwsprint.h"
#include "desktop.h"
#include "util.h"
#include "paper.h"      // For CPaperInfo class
#include "progbar.h"
#include "DlgURL.h"
#include "wpapi.h"      // For Microsoft Web Publishing SDK - Specifically, WpPost

#include "webdoc.h"

#include "textrec.h"
#include "commands.h"   // For CCmdChangeSelects in DoHyperlinkObject
#include "frameobj.h"
#include "grpobj.h"     // For GroupObject

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CWebView

IMPLEMENT_DYNCREATE(CWebView, CNewsletterView)

CWebView::CWebView()
{
}
			  
CWebView::~CWebView()
{
}

double
CWebView::GetPaperToProjectScale(void)
{
	double Scale = 100.0;
   
	CPmwDoc  *pDoc = INHERITED::GetDocument();
	if(pDoc != NULL)
	{
		CPmwApp *pApp = (CPmwApp *)AfxGetApp();
		CPaperInfo PaperInfo = *(pApp->GetPaperInfo(pDoc->get_orientation()));
		PPNT PaperDim = PaperInfo.GetProjectDims();
		PPNT DocDim = pDoc->get_dimensions();
		
		if (DocDim.x > PaperDim.x)
		{
			Scale = ((double)PaperDim.x / (double)DocDim.x) * 100;
		}

		if (DocDim.y > PaperDim.y)
		{
			double Scale2 = ((double)PaperDim.y / (double)DocDim.y) * 100;
			if (Scale2 < Scale)
				Scale = Scale2;
		}
	}

	return Scale;
}

void CWebView::OnDraw(CDC* pDC)
{
	if (pDC->IsPrinting())
	{
		if (printer_rc.is_print_preview)
		{
			ASSERT(m_pPreviewExtent != NULL);
			if (m_pPreviewExtent != NULL)	
			{
				double Scale = 100.0 / GetPaperToProjectScale();
				if (Scale > 1.0)
				{
					RedisplayContext	rcSave;
					rcSave = printer_rc;	// Save the rc - we are going to fake out print preview 

					printer_rc.source_pbox.x0 = (int)((double)printer_rc.source_pbox.x0 * Scale);
					printer_rc.source_pbox.x1 = (int)((double)printer_rc.source_pbox.x1 * Scale);
					printer_rc.source_pbox.y0 = (int)((double)printer_rc.source_pbox.y0 * Scale);
					printer_rc.source_pbox.y1 = (int)((double)printer_rc.source_pbox.y1 * Scale);

//					printer_rc.scaled_source_x0 *= iScale;
//					printer_rc.scaled_source_y0 *= iScale;

					printer_rc.x_resolution = (int)((double)printer_rc.x_resolution / Scale);
					printer_rc.y_resolution = (int)((double)printer_rc.y_resolution / Scale);
					// CPmwView::OnDraw() will use m_pPreviewExtent	in print preview
					PBOX	extent = m_pPreviewExtent->extent; 

					m_pPreviewExtent->extent.x0 = (int)((double)m_pPreviewExtent->extent.x0 * Scale);
					m_pPreviewExtent->extent.x1 = (int)((double)m_pPreviewExtent->extent.x1 * Scale);
					m_pPreviewExtent->extent.y0 = (int)((double)m_pPreviewExtent->extent.y0 * Scale);
					m_pPreviewExtent->extent.y1 = (int)((double)m_pPreviewExtent->extent.y1 * Scale);

					INHERITED::OnDraw(pDC);

					m_pPreviewExtent->extent = extent; 

					printer_rc = rcSave; 

					return;
				}
			}
		}
	}

	INHERITED::OnDraw(pDC);
}

BEGIN_MESSAGE_MAP(CWebView, CNewsletterView)
	//{{AFX_MSG_MAP(CWebView)
	ON_COMMAND(IDM_WEB_PMSITE, OnWebPmsite)
   ON_WM_TIMER()
   ON_UPDATE_COMMAND_UI(ID_GOTO_MASTER_PAGE, OnUpdateGotoMasterPage)
   ON_UPDATE_COMMAND_UI(ID_IGNORE_MASTER_ITEMS, OnUpdateIgnoreMasterItems)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebView interface
LPCSTR CWebView::GadgetMenuName(UINT uID, UINT uPart)
{
	LPCSTR pszMenuName = NULL;

	if (uID == ID_PAGE_GADGET)
	{
		pszMenuName = "Web Page";
	}
	else
	{
		pszMenuName = CNewsletterView::GadgetMenuName(uID, uPart);
	}
      	   
	return pszMenuName;
}
/////////////////////////////////////////////////////////////////////////////
// CWebView message handlers
CPmwPrint* CWebView::CreatePrintDialog(void)
{
	CNewsletterPrintDialog *pdlg = new CNewsletterPrintDialog(this);
	
	double Scale = GetPaperToProjectScale();
   pdlg->m_scale = Scale;
	return pdlg;
}
   	
SHORT CWebView::OnPrintDocument(PrintContext &pc,
													int32 aCurrentDocument, int32 aNumDocuments)
{
	return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}

extern BOOL DoHTML (HWND hwnd, LPCSTR url, BOOL GotoPage = FALSE);

void CWebView::OnWebPmsite() 
{
   CString csUrl;
   TRY
   {
      char     szPath[_MAX_PATH];
      CString  csUrlFile;

      if (::GetModuleFileName(AfxGetInstanceHandle(), szPath, sizeof(szPath)-1) > 0)
      {
         char *pEnd = strrchr(szPath, '\\');
         if (pEnd != NULL)
         {
            strcpy(pEnd, "\\PMW.URL");
            csUrlFile = szPath;
         }
      }

      GetPrivateProfileString("InternetShortcut", "URL", "http://www.printmaster.com/", csUrl.GetBuffer(256), 256-1, csUrlFile);
      csUrl.ReleaseBuffer();
   }
   END_TRY
/*

   if (DoHTML (m_hWnd, csUrl, TRUE) == TRUE)
      return;

   CDialogURL  Dlg (this);
   Dlg.DoModal ();
*/
   if (csUrl.Find(".htm") == -1)
   {
      if ((!csUrl.IsEmpty()) && (csUrl[csUrl.GetLength()-1] != '/'))
         csUrl += '/';
      csUrl += "index.html";
   }
   
   CURLProcessor        urlProcess;
   URLProcessorRetCode  rc;
   if ((rc = urlProcess.OpenURL(csUrl)) != enURLErrorNone)
      DoURLErrorDialog(rc);

}

/*
void
CWebView::DoTextPublishChange(DWORD Flags, CFrameObject *pObject, CWebDoc *pDoc)
{
   CCmdChangeSelects* pCommand;
   TRY
   {
      pCommand = new CCmdChangeSelects(IDCmd_FrameAttributes);
   }
   END_TRY

   if (pCommand != NULL)
   {
      if (!pCommand->Before(this))
      {
         delete pCommand;
         pCommand = NULL;
      }
   }

   pObject->SetAllExtraFlags(Flags); 
   pDoc->SetModifiedFlag();

   if (pCommand != NULL)
   {
      if (pCommand->After())
      {
         // Add the command for undoing later.
         pDoc->AddCommand(pCommand, FALSE);  // No execute!
      }
      else
      {
         delete pCommand;
      }
   }
}
*/
static void AddObjectRefreshes(CRefreshArray& Refreshes, PMGPageObject* pObject)
{
	while (pObject != NULL)
	{
		if (pObject->type() == OBJECT_TYPE_Group)
		{
			AddObjectRefreshes(Refreshes, (PMGPageObject*)((GroupObject*)pObject)->object_list()->first_object());
		}
		else
		{
			// Add the extent.
			REFRESH_EXTENT re;

			re.extent = pObject->get_bound();
			re.refresh_type = REFRESH_ALL;
			re.my_object = NULL;

			re.update_object = NULL;
			re.update_state = NULL;

			Refreshes.Add(&re);
		}
		pObject = (PMGPageObject*)pObject->next_object();
	}
}

struct COverlapStats
{
	BOOL m_fHasText;
	int m_nCount;
};

static void GetOverlapStats(const PBOX& Bound, PMGPageObject* pObject, COverlapStats& Stats)
{
	while (pObject != NULL)
	{
		// See if this text box touches the desired bound.
		PBOX ObjectBound = pObject->get_bound();
		if (IsBoxOnBox(&Bound, &ObjectBound))
		{
			// Another object in this extent.
			Stats.m_nCount++;

			switch (pObject->type())
			{
				case OBJECT_TYPE_Frame:
				{
					Stats.m_fHasText = TRUE;
					break;
				}
				case OBJECT_TYPE_Group:
				{
					// Nest into the group.
					GetOverlapStats(Bound, (PMGPageObject*)((GroupObject*)pObject)->object_list()->first_object(), Stats);
					Stats.m_nCount--;		 // Not us.
					break;
				}
				default:
				{
					break;
				}
			}
		}
		pObject = (PMGPageObject*)pObject->next_object();
	}
}
// A hook function to handle things after an object has changed position.
void CWebView::AfterPositionChange(void)
{
	// Get rid of any old ones.
	ResetOverlappingExtents();

	CPmwDoc* pDoc = GetDocument();
	// The key to this is selected objects.
	if (pDoc->something_selected())
	{
		// There is a selection.

		// Create an array of objects "pools", areas where objects overlap.
		// We use a RefreshArray to handle this. It handles merging objects
		// and the like.

		CRefreshArray Refreshes;
		AddObjectRefreshes(Refreshes, pDoc->objects());

		// Loop through all the object refreshes. They should all be disjoint.
		int nExtents = Refreshes.NumberOfExtents();
		for (int nExtent = 0; nExtent < nExtents; nExtent++)
		{
			REFRESH_EXTENT* pExtent = Refreshes.GetExtent(nExtent);

			// See if this intersects any selected objects.
			PMGPageObject* pObject = pDoc->selected_objects();
			while (pObject != NULL)
			{
				PBOX Bound = pObject->get_bound();
				if (IsBoxOnBox(&Bound, &pExtent->extent))
				{
					break;
				}
				pObject = pObject->next_selected_object();
			}

			if (pObject != NULL)
			{
				// This extent overlaps a selected object.
				// If there is a text box in this extent, we have an area to mark.
				COverlapStats Stats;
				Stats.m_fHasText = FALSE;
				Stats.m_nCount = 0;
				GetOverlapStats(pExtent->extent, pDoc->objects(), Stats);
				if (Stats.m_fHasText && Stats.m_nCount > 1)
				{
					// Add this extent to our array.
					m_OverlappingExtents.Add(&pExtent->extent);
				}
			}
		}
		// If we found an overlapping extents, show them.
		if (m_OverlappingExtents.GetSize() > 0)
		{
			ShowOverlappingExtents();
		}
	}
}

void CWebView::OnTimer(UINT nIDEvent)
{
   switch (nIDEvent)
   {
      case OVERLAPPING_EXTENTS_TIMER:
      {
			// Turn it off now.
			ResetOverlappingExtents();
         break;
      }
      default:
      {
			CPmwView::OnTimer(nIDEvent);
         break;
      }
   }
}
//
// Show the overlapping extents in the above array.
//
static BOOL m_fExtentOn;

void CWebView::ShowOverlappingExtents(void)
{
	if (m_OverlappingExtents.GetSize() > 0)
	{
		// Turn the extents on.
		ToggleOverlappingExtents();
		m_fExtentOn = TRUE;

		// Set the timer to show the extents for a second.
		SetTimer(OVERLAPPING_EXTENTS_TIMER, 750, NULL);

		// Add the toggle.
		rc.register_toggle(OverlappingExtentsToggle, this);
	}
}
//
// Hide the overlapping extents in the above array,
// and remove them from the array.
//
void CWebView::ResetOverlappingExtents(void)
{
	if (m_OverlappingExtents.GetSize() > 0)
	{
		// Kill the timer.
		KillTimer(OVERLAPPING_EXTENTS_TIMER);

		if (m_fExtentOn)
		{
			m_fExtentOn = FALSE;
			// Turn the extents off.
			ToggleOverlappingExtents();
			// Remove all the extents.
			m_OverlappingExtents.RemoveAll();
			// Remove the toggle function.
			rc.remove_toggle(OverlappingExtentsToggle);
		}
		else
		{
			// It is currently turned off by a toggle function.
			// Try again later.
			SetTimer(OVERLAPPING_EXTENTS_TIMER, 0, NULL);
		}
	}
}
// Toggle all the overlapping extents.
// There is room for a passed HDC to allow for the doc toggle case.
void CWebView::ToggleOverlappingExtents(CDC* pInDC /*=NULL*/, RedisplayContext* rc /*=NULL*/)
{
	// Setup the CDC we will be using. There are two cases.
	CDC* pDC;
	if ((pDC = pInDC) == NULL)
	{
		pDC = GetDC();
	}
	if (rc == NULL)
	{
		rc = get_rc();
	}
	ASSERT(rc->IsScreen());
	int nExtents = m_OverlappingExtents.GetSize();
	for (int nExtent = 0; nExtent < nExtents; nExtent++)
	{
		PBOX Extent = *(PBOX*)(m_OverlappingExtents.GetAt(nExtent));
		rc->pbox_to_screen(&Extent, TRUE);
		CRect crBox;
		if (rc->convert_pbox(&Extent, &crBox, NULL))
		{
			// This is our pen width.
			int nWidth = 4;
			// Draw the outline outside the box.
			crBox.InflateRect(nWidth, nWidth);

			// Create the pen to use for framing.
			COLORREF clrRect = RGB(255, 0, 0);		// Red!
			CPen Pen;
			if (Pen.CreatePen(PS_SOLID, nWidth, clrRect ^ RGB(255, 255, 255)))
			{
				// We want an empty middle.
				CBrush Brush;
				if (Brush.CreateStockObject(NULL_BRUSH))
				{
					// Select the pen...
					CPen* pOldPen = pDC->SelectObject(&Pen);
					if (pOldPen != NULL)
					{
						// ... and select the brush.
						CBrush* pOldBrush = pDC->SelectObject(&Brush);
						if (pOldBrush != NULL)
						{
							// Set ROP2 to XOR our pen.
							int nOldROP2 = pDC->SetROP2(R2_XORPEN);
							// Do the rectangle draw.
							pDC->Rectangle(crBox);
							// Set ROP2 back.
							pDC->SetROP2(nOldROP2);
							// Select the old brush back in.
							pDC->SelectObject(pOldBrush);
						}
						// Select the old pen back in.
						pDC->SelectObject(pOldPen);
					}
				}
			}
		}
	}

	if (pDC != pInDC)
	{
		ReleaseDC(pDC);
	}
}

BOOL CWebView::OverlappingExtentsToggle(RedisplayContextPtr rc, BOOL turn_on, LPRECT clip, VOIDPTR data)
{
	m_fExtentOn = turn_on;
	// Setup the DC to use.
	CDC dc;
	dc.Attach(rc->destination_hdc);
	// Toggle the selects.
	((CWebView*)data)->ToggleOverlappingExtents(&dc, rc);
	// Clean up the DC.
	dc.Detach();

	return TRUE;
}

void 
CWebView::OnUpdateGotoMasterPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void 
CWebView::OnUpdateIgnoreMasterItems(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
