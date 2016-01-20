/*
// $Workfile: NEWSVIEW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/NEWSVIEW.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 7     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 6     8/17/98 5:36p Mwilson
// changed Tshirts to be based off of newsletters instead of posters
// 
// 5     8/06/98 4:32p Mwilson
// added new craft project type
// 
// 4     3/19/98 2:32p Jayn
// Finish up master pages.
// 
// 3     3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 2     3/10/98 5:24p Jayn
// Master page changes
// 
//    Rev 1.0   14 Aug 1997 15:22:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:22   Fred
// Initial revision.
// 
//    Rev 1.6   21 Mar 1997 15:48:54   Jay
// More preview changes.
// 
//    Rev 1.5   06 Aug 1996 14:36:38   Jay
//  
// 
//    Rev 1.4   06 Aug 1996 11:17:06   Jay
//  
// 
//    Rev 1.3   25 Jul 1996 18:21:28   Fred
//  
// 
//    Rev 1.2   25 Jul 1996 13:52:12   Jay
// Gadget stuff.
// 
//    Rev 1.1   24 Jul 1996 07:53:46   Jay
// Added print dialog
// 
//    Rev 1.0   23 Jul 1996 10:39:56   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "newsview.h"
#include "nwsprint.h"
#include "util.h"
#include "desktop.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewsletterView

IMPLEMENT_DYNCREATE(CNewsletterView, CPmwView)

CNewsletterView::CNewsletterView()
{
}

CNewsletterView::~CNewsletterView()
{
}


BEGIN_MESSAGE_MAP(CNewsletterView, CPmwView)
	//{{AFX_MSG_MAP(CNewsletterView)
   ON_COMMAND(ID_GOTO_MASTER_PAGE, OnGotoMasterPage)
   ON_UPDATE_COMMAND_UI(ID_GOTO_MASTER_PAGE, OnUpdateGotoMasterPage)
   ON_COMMAND(ID_IGNORE_MASTER_ITEMS, OnIgnoreMasterItems)
   ON_UPDATE_COMMAND_UI(ID_IGNORE_MASTER_ITEMS, OnUpdateIgnoreMasterItems)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewsletterView drawing

void CNewsletterView::OnDraw(CDC* pDC)
{
	CPmwView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CNewsletterView gadget handlers

BOOL CNewsletterView::GadgetVisible(UINT uID)
{
	return (uID == ID_PAGE_GADGET) || CPmwView::GadgetVisible(uID);
}

void CNewsletterView::GadgetText(UINT uID, CString& csText)
{
	CPmwDoc* pDoc = GetDocument();
	CString csFormat;
	switch (uID)
	{
		case ID_PAGE_GADGET:
		{
			TRY
			{
				csText.Empty();
				DWORD dwPage = pDoc->CurrentPageIndex()+1;
				if (dwPage == 0)
				{
					// Not on a regular page.
					dwPage = pDoc->MasterPageIndex();
					if (dwPage != (DWORD)-1)
					{
						// On a master page.
						int nSided = pDoc->GetSided();
						ASSERT(nSided != DocumentRecord::NotSided);
						UINT uID;
						if (nSided == DocumentRecord::SingleSided)
						{
							// Single-sided. Only one master page.
							uID = IDS_MasterPageGadget;
						}
						else
						{
							// Double-sided. Determine which master page.
							uID = (dwPage == 0) ? IDS_LeftMasterPageGadget : IDS_RightMasterPageGadget;
						}
						// Load the correct string.
						Util::LoadString(uID, csText);
					}
				}
				else
				{
					// "%ld of %ld"
					Util::LoadString(IDS_PageNum, csFormat);
					csText.Format(csFormat, dwPage, GetDocument()->NumberOfPages());
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

LPCSTR CNewsletterView::GadgetMenuName(UINT uID, UINT uPart)
{
	LPCSTR pszMenuName = NULL;

	if (uID == ID_PAGE_GADGET)
	{
		pszMenuName = "Newsletter Page";
	}
	else
	{
		pszMenuName = CPmwView::GadgetMenuName(uID, uPart);
	}

	return pszMenuName;
}

/////////////////////////////////////////////////////////////////////////////
// CNewsletterView message handlers

CPmwPrint* CNewsletterView::CreatePrintDialog(void)
{
	return new CNewsletterPrintDialog(this);
}

SHORT CNewsletterView::OnPrintDocument(PrintContext &pc,
													int32 aCurrentDocument, int32 aNumDocuments)
{
	return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}

void CNewsletterView::OnGotoMasterPage()
{
	CPmwDoc* pDoc = GetDocument();

	// Compute which page to go to.
	if (pDoc->EditingMasterPage())
	{
		// Go back to the document page (the one we came from).
		if (m_dwPushedPageIndex >= pDoc->NumberOfPages())
		{
			m_dwPushedPageIndex = pDoc->NumberOfPages()-1;
		}
      BeforePageChange();
		pDoc->GotoPage(m_dwPushedPageIndex);
	}
	else
	{
		// On a normal page. Go to the master page.
		DWORD dwIndex;
		DocumentRecord* pDocRec = pDoc->DocumentRecord();

		// Remember what page we were on.
		m_dwPushedPageIndex = pDoc->CurrentPageIndex();

		// Figure out which page to go to.
		if (pDocRec->GetSided() == DocumentRecord::SingleSided)
		{
			dwIndex = 1;		// Right master page for single-sided documents.
		}
		else
		{
			dwIndex = (m_dwPushedPageIndex + 1) & 1;
		}

		// Load the master page.
		if (dwIndex < pDocRec->NumberOfMasterPages())
		{
         BeforePageChange(TRUE);
			pDoc->GotoMasterPage(dwIndex);
		}
	}
}

void CNewsletterView::OnUpdateGotoMasterPage(CCmdUI* pCmdUI)
{
	CPmwDoc* pDoc = GetDocument();
	CString csText;
	BOOL fEnable = TRUE;
	// If we are on a normal page, set it to "go to master page".
	if (pDoc->EditingMasterPage())
	{
		Util::LoadString(IDS_ReturnToPage, csText);
	}
	else
	{
		Util::LoadString(IDS_GotoMasterPage, csText);
	}
	pCmdUI->SetText(csText);
	pCmdUI->Enable(fEnable);
}

void CNewsletterView::OnIgnoreMasterItems()
{
	CPageProperties* pProperties = GetDocument()->GetPageProperties();
	int nHideMasterItems = pProperties->GetHideMasterItems();
	if (nHideMasterItems != -1)
	{
		RefreshMasterItems();
		pProperties->SetHideMasterItems(!nHideMasterItems);
		RefreshMasterItems();
	}
}

void CNewsletterView::OnUpdateIgnoreMasterItems(CCmdUI* pCmdUI)
{
	BOOL fCheck;
	BOOL fEnable;
	int nHideMasterItems = GetDocument()->GetPageProperties()->GetHideMasterItems();
	switch (nHideMasterItems)
	{
		case -1:
		{
			// On the master page. Disabled.
			fCheck = FALSE;
			fEnable = FALSE;
			break;
		}
		case FALSE:
		case TRUE:
		{
			fCheck = nHideMasterItems;
			fEnable = TRUE;
			break;
		}
		default:
		{
			ASSERT(FALSE);
			return;
		}
	}
	pCmdUI->SetCheck(fCheck);
	pCmdUI->Enable(fEnable);
}

void CNewsletterView::CreateEditDesktop(void)
{
   m_pDesktop = new CNewsletterEditDesktop(this);
}

void CNewsletterView::RefreshMasterItems(void)
{
	CPmwDoc* pDoc = GetDocument();

	// Get the list of master page objects for this page.
	ObjectList* pMasterItems = pDoc->GetMasterItems();

	// If we have objects, refresh all of them.
	if (pMasterItems != NULL)
	{
		for (PMGPageObject* pObject = (PMGPageObject*)pMasterItems->first_object();
			  pObject != NULL;
			  pObject = (PMGPageObject*)pObject->next_object())
		{
			pDoc->refresh_object(pObject);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTShirtView

IMPLEMENT_DYNCREATE(CTShirtView, CNewsletterView)

CTShirtView::CTShirtView()
{
}

CTShirtView::~CTShirtView()
{
}


BEGIN_MESSAGE_MAP(CTShirtView, CNewsletterView)
	//{{AFX_MSG_MAP(CTShirtView)
   ON_UPDATE_COMMAND_UI(ID_ADD_PAGE, OnUpdateAddPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTShirtView message handlers

CPmwPrint* CTShirtView::CreatePrintDialog(void)
{
	if(GetDocument()->NumberOfPages() > 1)
		return new CTShirtPrintDialog(this);
	else
		return CPmwView::CreatePrintDialog();
}

SHORT CTShirtView::OnPrintDocument(PrintContext &pc,
													int32 aCurrentDocument, int32 aNumDocuments)
{
	if(GetDocument()->NumberOfPages() > 1)
		return CNewsletterView::OnPrintDocument(pc, aCurrentDocument, aNumDocuments);
	else
		return CPmwView::OnPrintDocument(pc, aCurrentDocument, aNumDocuments);
}



BOOL CTShirtView::GadgetVisible(UINT uID)
{
	return (uID == ID_PAGE_GADGET && GetDocument()->NumberOfPages() > 1) || CPmwView::GadgetVisible(uID);
}

void CTShirtView::OnUpdateAddPage(CCmdUI* pCmdUI)
{
	if(GetDocument()->NumberOfPages() > 1)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CCraftView

IMPLEMENT_DYNCREATE(CCraftView, CTShirtView)

CCraftView::CCraftView()
{
}

CCraftView::~CCraftView()
{
}

BEGIN_MESSAGE_MAP(CCraftView, CTShirtView)
	//{{AFX_MSG_MAP(CCraftView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPmwPrint* CCraftView::CreatePrintDialog(void)
{
	if(GetDocument()->NumberOfPages() > 1)
		return CNewsletterView::CreatePrintDialog();
	else
		return CPmwView::CreatePrintDialog();
}

