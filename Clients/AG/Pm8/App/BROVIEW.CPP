/*
// $Workfile: BROVIEW.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BROVIEW.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     3/10/98 5:23p Jayn
// Master page changes
// 
//    Rev 1.0   14 Aug 1997 15:18:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:14   Fred
// Initial revision.
// 
//    Rev 1.8   06 Nov 1996 10:01:36   johno
// Moved strings to resource DLL
// 
//    Rev 1.7   01 Nov 1996 17:12:16   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.6   06 Aug 1996 14:36:38   Jay
//  
// 
//    Rev 1.5   06 Aug 1996 11:17:08   Jay
//  
// 
//    Rev 1.4   28 Jul 1996 13:37:50   Jay
//  
// 
//    Rev 1.3   28 Jul 1996 11:28:40   Jay
// Brochure stuff
// 
//    Rev 1.2   27 Jul 1996 10:10:08   Jay
// New brochure print dialog.
// 
//    Rev 1.1   25 Jul 1996 11:16:58   Jay
// More page stuff
// 
//    Rev 1.0   23 Jul 1996 10:39:36   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "broview.h"
#include "broprint.h"
#include "desktop.h"
#include "sortarry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrochureView

IMPLEMENT_DYNCREATE(CBrochureView, CPmwView)

CBrochureView::CBrochureView()
{
}

CBrochureView::~CBrochureView()
{
}

BEGIN_MESSAGE_MAP(CBrochureView, CPmwView)
	//{{AFX_MSG_MAP(CBrochureView)
	ON_COMMAND(ID_BROCHURE_VIEWFRONT, OnViewFront)
	ON_UPDATE_COMMAND_UI(ID_BROCHURE_VIEWFRONT, OnUpdateViewFront)
	ON_COMMAND(ID_BROCHURE_VIEWBACK, OnViewBack)
	ON_UPDATE_COMMAND_UI(ID_BROCHURE_VIEWBACK, OnUpdateViewBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrochureView drawing

void CBrochureView::OnDraw(CDC* pDC)
{
	CPmwView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CBrochureView gadget handlers

BOOL CBrochureView::GadgetVisible(UINT uID)
{
	return (uID == ID_PAGE_GADGET) || CPmwView::GadgetVisible(uID);
}

void CBrochureView::GadgetText(UINT uID, CString& csText)
{
	switch (uID)
	{
		case ID_PAGE_GADGET:
		{
			CMlsStringArray PanelNames(IDS_BROCHURE_PANEL_NAMES);

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

LPCSTR CBrochureView::GadgetMenuName(UINT uID, UINT uPart)
{
	LPCSTR pszMenuName = NULL;

	if (uID == ID_PAGE_GADGET)
	{
		// JRO pszMenuName = "Brochure Page";
      pszMenuName = GET_PMWAPP()->GetResourceStringPointer (IDS_BROCHURE_PAGE);
	}
	else
	{
		pszMenuName = CPmwView::GadgetMenuName(uID, uPart);
	}

	return pszMenuName;
}

/////////////////////////////////////////////////////////////////////////////
// CBrochureView message handlers

CPmwPrint* CBrochureView::CreatePrintDialog(void)
{
	return new CBrochurePrintDialog(this);
}

void CBrochureView::CreateEditDesktop(void)
{
	m_pDesktop = new CBrochureEditDesktop(this);
}

void CBrochureView::OnViewFront()
{
	GetDocument()->GotoPage(0);
}

void CBrochureView::OnUpdateViewFront(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->CurrentPageIndex() != 0);
}

void CBrochureView::OnViewBack()
{
	GetDocument()->GotoPage(1);
}

void CBrochureView::OnUpdateViewBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->CurrentPageIndex() != 1);
}

SHORT CBrochureView::OnPrintDocument(PrintContext &pc,
													int32 aCurrentDocument, int32 aNumDocuments)
{
	return OnPrintMultiPageDocument(pc, aCurrentDocument, aNumDocuments);
}
