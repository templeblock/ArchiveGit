/*
// $Workfile: NEWDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
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
// $Log: /PM8/App/NEWDLG.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 13    2/20/99 9:19p Psasse
// Support for Vertical Banners
// 
// 12    2/10/99 5:42p Johno
// 
// 11    2/10/99 4:06p Johno
// The Brand New dialog now sizes itself to avoid listbox scriolling.
// 
// 10    8/20/98 12:43p Hforman
// cleanup unused stuff
// 
// 9     6/04/98 4:47p Rlovejoy
// Show helpful hint.
// 
// 8     6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 7     5/27/98 6:07p Hforman
// remove SEND_WELCOME
// 
// 6     4/21/98 6:52p Fredf
// Fixed flashing problem.
// 
// 5     4/18/98 5:04p Fredf
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "newdlg.h"
#include "pmwtempl.h"
#include "colordef.h"
#include "mainfrm.h"
#include "hint.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define  ICON_GAP 2

#define  NDLG_ICONSIZEX()	(GetSystemMetrics(SM_CXICON))
#define  NDLG_ICONSIZEY()	(GetSystemMetrics(SM_CYICON))
   
#define  NDLG_ICONHANDLE(id) \
				(HICON)(LoadImage(AfxGetResourceHandle(), \
				MAKEINTRESOURCE(id), \
				IMAGE_ICON, \
				NDLG_ICONSIZEX(), \
				NDLG_ICONSIZEY(), \
				LR_DEFAULTCOLOR))

#define  NDLG_ICONDRAW(DCptr, l, t) \
				DrawIconEx((DCptr)->m_hDC, \
				(l)+ICON_GAP, \
				(t), \
				hIcon, \
				NDLG_ICONSIZEX(), \
				NDLG_ICONSIZEY(), \
				0, \
				NULL, \
				DI_NORMAL)

/////////////////////////////////////////////////////////////////////////////
CNewDlgListbox::CNewDlgListbox ()
{
   m_AllItemDataPtrsValid = FALSE;
}

CNewDlgListbox::~CNewDlgListbox ()
{
}

BEGIN_MESSAGE_MAP(CNewDlgListbox, COwnerDrawListBox)
   //{{AFX_MSG_MAP(CNewDlgListbox)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNewDlgListbox::DeleteItem(LPDELETEITEMSTRUCT pDeleteItemStruct)
{
}

void CNewDlgListbox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// Default values.
	lpMIS->itemWidth = ICON_GAP+NDLG_ICONSIZEX()+ICON_GAP;
	lpMIS->itemHeight = NDLG_ICONSIZEY()+ICON_GAP;

   ASSERT(m_AllItemDataPtrsValid);

   if (m_AllItemDataPtrsValid)
	{
		CSize czItem;
		// Run through all the items and find the largest one.
		for (int nItem = 0; nItem < GetCount(); nItem++)
		{
			CString csText;
			GetText(nItem, csText);

			CSize czItem(ICON_GAP+NDLG_ICONSIZEX()+ICON_GAP,NDLG_ICONSIZEY());

			CDC* pDC = GetDC();
			if (pDC != NULL)
			{
		      CSize czText = pDC->GetTextExtent(csText, csText.GetLength());
				pDC->LPtoDP(&czText);
				czItem.cx += czText.cx;
				if (czText.cy > czItem.cy)
				{
					czItem.cy = czText.cy;
				}

				ReleaseDC(pDC);
				pDC = NULL;
			}
			else
			{
				czItem.cx += 64;	// random guess
			}
			// Add in some spacing between columns.
//			czItem.cx += 12;
			// Update maximum item size.
			if ((UINT)czItem.cx > lpMIS->itemWidth)
			{
				lpMIS->itemWidth = (UINT)czItem.cx;
			}
			if ((UINT)czItem.cy > lpMIS->itemHeight)
			{
				lpMIS->itemHeight = (UINT)czItem.cy;
			}
		}
	}
}

void CNewDlgListbox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   CDC* pDC = CDC::FromHandle(lpDIS->hDC);
   
   pDC->SaveDC();

   CRect Rect(lpDIS->rcItem);

   if (lpDIS->itemState & ODS_SELECTED)
   {
      pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
      pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
   }
   else
   {
      pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
      pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
   }

   pDC->ExtTextOut(0, 0, ETO_CLIPPED | ETO_OPAQUE, &Rect, NULL, 0, NULL);

   if (m_AllItemDataPtrsValid)
   {
      CPmwDocTemplate* pTemplate = (CPmwDocTemplate*)lpDIS->itemData;

      if (pTemplate != NULL)
      {
         HICON hIcon;
         hIcon = NDLG_ICONHANDLE(pTemplate->IDResource());
         if (hIcon != NULL)
         {
				NDLG_ICONDRAW(pDC, Rect.left, Rect.top+(Rect.Height()-NDLG_ICONSIZEY())/2);
            Rect.left += ICON_GAP + NDLG_ICONSIZEX() + ICON_GAP;
         }
      }
   }
   
   CString csText;
   GetText(lpDIS->itemID, csText);

   pDC->DrawText(csText, -1, &Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

   pDC->RestoreDC(-1);
}

int CNewDlgListbox::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
{
   return 0;       // exact match
}

BOOL CNewDlgListbox::SubclassDlgItem(UINT nID, CWnd* pParent)
{
   m_AllItemDataPtrsValid = FALSE;

   BOOL b;

   if ((b = INHERITED::SubclassDlgItem (nID, pParent)) != FALSE)
	{
      FontMetrics(m_tm);
	}

   return b;
}
/////////////////////////////////////////////////////////////////////////////
// CNewDialog dialog

CNewDialog::CNewDialog(CPtrList* pList, NEWINFO_REF info, UINT id /*=0*/, CWnd* pParent /*=NULL*/)
   : CPmwDialogColor(id == 0 ? CNewDialog::IDD : id, pParent, BRAND_NEW_COLOR, FALSE)
{
   //{{AFX_DATA_INIT(CNewDialog)
   m_nIndex = 0;
   m_subtype = 0;
   m_orientation = info.orientation;
   m_nPanels = (info.type == PROJECT_TYPE_Brochure) ? info.subtype-2 : 0;
   //}}AFX_DATA_INIT
   m_subtype = (info.type == PROJECT_TYPE_Card
                     || info.type == PROJECT_TYPE_HalfCard
                     || info.type == PROJECT_TYPE_NoteCard)
                  ? info.subtype == CARD_TopFold ? 1 : 0
                  : 0;
   ASSERT(pList == NULL);

   m_pList = pList;
   m_nType = info.type;
}

void CNewDialog::DoDataExchange(CDataExchange* pDX)
{
   CPmwDialogColor::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CNewDialog)
   DDX_LBIndex(pDX, IDC_NEW_LIST, m_nIndex);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewDialog, CPmwDialogColor)
   //{{AFX_MSG_MAP(CNewDialog)
	ON_LBN_SELCHANGE(IDC_NEW_LIST, OnSelchangeNewList)
   ON_LBN_DBLCLK(IDC_NEW_LIST, OnOK)
	ON_BN_CLICKED(IDC_FINISH, OnFinish)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewDialog message handlers
BOOL CNewDialog::OnInitDialog()
{
	// Do the normal init dialog.
	CPmwDialogColor::OnInitDialog();

	VERIFY(m_NewListbox.SubclassDlgItem(IDC_NEW_LIST, this));
	// Fill with document templates in list.
	m_NewListbox.SetRedraw(FALSE);
	m_NewListbox.ResetContent();

	ASSERT(m_pList == NULL);
	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();
	// Add all the CDocTemplates in the list by name.
	while (pos != NULL)
	{
		CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		CString strTypeName;

		if (pTemplate->GetDocString(strTypeName, CDocTemplate::fileNewName) &&
			!strTypeName.IsEmpty())
		{
			// add it to the listbox
			int nIndex = m_NewListbox.AddString(strTypeName);
			if (nIndex == -1)
			{
				EndDialog(IDABORT);
				return FALSE;
			}
      
			m_NewListbox.SetItemDataPtr(nIndex, pTemplate);
			if (((CPmwDocTemplate*)pTemplate)->ProjectType() == m_nType)
			{
				m_nIndex = nIndex;
			}
		}
	}

	m_NewListbox.m_AllItemDataPtrsValid = TRUE;
	// This call to SizeItems() simulates the ownerdraw
	// sizing mechanism. We need to call it here because
	// the first WM_MEASUREITEM message occurred a long
	// time ago before we had populated the list box...
	// probably even before we had the list box subclassed.
	m_NewListbox.SizeItems();
	// Allow the box to redraw.
	m_NewListbox.SetRedraw(TRUE);
	// Select the one we want.
	m_NewListbox.SetCurSel(m_nIndex);

	CreateWizzardButtons();
	EnableWizardButtons(WIZBUT_NEXT | WIZBUT_CANCEL);
	// Do this in case the buttons need to be changed
	OnSelchangeNewList();

	SizeToListboxContent();

	GET_PMWAPP()->ShowHelpfulHint(PLACE_HINT, PLACE_BrandNew, this);

   return TRUE;
}

void
CNewDialog::SizeToListboxContent(void)
{
	int count = m_NewListbox.GetCount();
	if (count > 0)
	{
		CRect	RectList;
		// Find the needed listbox width
		m_NewListbox.GetClientRect(&RectList);
		// Get the size of the first item
		// (assumes all items the same size)
		CRect	RectItem;
		m_NewListbox.GetItemRect(0, &RectItem);
		int rows = RectList.Height() / RectItem.Height();
		//int cols = (int)ceil((double)count / (double)rows);
		int cols = count / rows;
		if (cols * rows < count)
			cols ++;
		int width = cols * RectItem.Width();
		// Add some pixels to avoid horizontal scrolling
		width += cols;
		// Does the listbox need to be wider?
		if (RectList.Width() < width)
		{
			// Find the new width for the listbox
			int dx = width - RectList.Width();
			// Move - widen dialog
			CRect	RectDialog;
			GetWindowRect(&RectDialog);	
			RectDialog.InflateRect(dx / 2, 0);
			MoveWindow(RectDialog);
			// Move - widen listbox
			m_NewListbox.ClientToScreen(RectList);
			RectList.right += dx;
			ScreenToClient(RectList);
			m_NewListbox.MoveWindow(RectList);
		}
	}
}

void CNewDialog::OnOK()
{
   UpdateData(TRUE);

   CListBox *pListBox = (CListBox*)GetDlgItem(IDC_NEW_LIST);
   m_template = (CDocTemplate *)pListBox->GetItemDataPtr(m_nIndex);

   m_nType = ((CPmwDocTemplate*)m_template)->ProjectType();
   CPmwDialogColor::OnOK();
}

void CNewDialog::OnSelchangeNewList()
{
   CPmwDocTemplate   *pTemplate;
   CListBox          *pListBox;

   UpdateData(TRUE);

   if ((pListBox = (CListBox*)GetDlgItem (IDC_NEW_LIST)) != NULL)
      pTemplate = (CPmwDocTemplate*)pListBox->GetItemDataPtr (m_nIndex);

   if (((pTemplate->ProjectType() == PROJECT_TYPE_Banner) && (GetConfiguration()->SupportsVerticalBanners() == FALSE))
	 || (pTemplate->ProjectType() == PROJECT_TYPE_TShirt))
	{
		EnableWizardButtons(WIZBUT_CANCEL | WIZBUT_FINISH);
	}
   else
	{
		EnableWizardButtons(WIZBUT_NEXT | WIZBUT_CANCEL);
	}
}

/*
// Return the template chosen (and the project type number).
*/

VOID CNewDialog::get_exit_info(NEWINFO_PTR info)
{
   info->type = m_nType;
   info->subtype = 0;         // As a default.
   info->orientation = m_orientation;
   info->pTemplate = m_template;

   // Handle any overrides.

   switch (info->type)
   {
/*
      case PROJECT_TYPE_BusinessCard:
      {
      // Business cards are actually reversed (make tall and wide right).

         info->orientation = (info->orientation == PORTRAIT)
                              ? LANDSCAPE
                              : PORTRAIT;
         break;
      }
*/
      case PROJECT_TYPE_BusinessCard:
      case PROJECT_TYPE_Banner:
      case PROJECT_TYPE_Envelope:
      case PROJECT_TYPE_PostCard:
      {
         info->orientation = LANDSCAPE;
         break;
      }
      case PROJECT_TYPE_Label:
      case PROJECT_TYPE_WebPub:
      case PROJECT_TYPE_TShirt:
      {
         info->orientation = PORTRAIT;
         break;
      }
      case PROJECT_TYPE_Card:
      case PROJECT_TYPE_HalfCard:
      case PROJECT_TYPE_NoteCard:
      {
         info->subtype = m_subtype ? CARD_TopFold : CARD_SideFold;
         info->orientation = PORTRAIT;
         break;
      }

      case PROJECT_TYPE_Brochure:
      {
         info->subtype = m_nPanels+2;
         info->orientation = PORTRAIT;
/*
         info->orientation = (info->orientation == PORTRAIT)
                              ? LANDSCAPE
                              : PORTRAIT;
*/
         break;
      }

      default:
      {
         info->orientation = PORTRAIT;
         break;
      }
   }
}


void CNewDialog::OnFinish() 
{
   OnOK();
}
