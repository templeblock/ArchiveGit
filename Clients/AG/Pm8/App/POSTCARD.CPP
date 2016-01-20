/*
// $Workfile: POSTCARD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/POSTCARD.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 3     7/25/98 1:48p Jayn
// Made this compile?
// 
// 2     12/10/97 12:30p Jayn
// Fixed a bug in preview
// 
//    Rev 1.2   30 Sep 1997 11:18:06   johno
// Changes to DrawPreview() for different post card orientations
// 
//    Rev 1.1   26 Sep 1997 17:05:50   johno
// 
//    Rev 1.0   22 Sep 1997 10:49:14   johno
// Initial revision.
// 
*/

#include "stdafx.h"
#include "pmw.h"
#include "POSTCARD.h"
#include "cwmf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostCardTypeDialog dialog

// JRO Parameters in this order so old code compiles

CPostCardTypeDialog::CPostCardTypeDialog(CPaperInfo* pOldInfo, CWnd* pParent /*=NULL*/, DWORD WizFlags, COLORREF cr)
	: CPmwDialogColor (CPostCardTypeDialog::IDD, pParent, cr)
{
	//{{AFX_DATA_INIT(CPostCardTypeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_WizFlags = WizFlags;
   m_pOldInfo = pOldInfo;
}

void CPostCardTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPostCardTypeDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPostCardTypeDialog, CPmwDialogColor)
	//{{AFX_MSG_MAP(CPostCardTypeDialog)
	ON_LBN_SELCHANGE(IDC_POSTCARD_LIST, OnSelchangePostCardList)
	ON_WM_DRAWITEM()
	ON_LBN_DBLCLK(IDC_POSTCARD_LIST, OnDblclkPostCardList)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostCardTypeDialog message handlers

void CPostCardTypeDialog::BuildList(void)
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_POSTCARD_LIST)) != NULL)
	{
		/*
		// Get the name to match.
		*/

		CString csOldName;
		if (m_pOldInfo != NULL)
		{
			csOldName = m_pOldInfo->GetName();
		}

		int nNewSel = 0;

		pList->SetRedraw(FALSE);
		pList->ResetContent();
		int nLabels = m_List.Labels();
		for (int nLabel = 0; nLabel < nLabels; nLabel++)
		{
			CLabelData* pLabel = m_List.Label(nLabel);
			ASSERT(pLabel != NULL);

			if (pLabel != NULL)
			{
				int nIndex = pList->AddString(pLabel->GetName());
				if (nIndex >= 0)
				{
					pList->SetItemData(nIndex, (DWORD)pLabel);
				/*
				// If the label matches the name coming in,
				// remember its position.
				*/
					if (pLabel->GetName() == csOldName)
					{
						nNewSel = nIndex;
					}
				}
			}
		}
		// Always start with the first label.
		pList->SetCurSel(nNewSel);
		pList->SetRedraw(TRUE);

		OnSelchangePostCardList();				// Do this by hand.
	}
}

void CPostCardTypeDialog::OnOK()
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_POSTCARD_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			m_pChosenInfo = (CLabelData*)pList->GetItemData(nCurSel);
		}
	}

	CPmwDialogColor::OnOK();
}

BOOL CPostCardTypeDialog::OnInitDialog()
{
	CPmwDialogColor::OnInitDialog();

	m_List.ReadList("POSTCARD.DAT");

	BuildList();

   CreateWizzardButtons ();
   EnableWizardButtons (m_WizFlags);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPostCardTypeDialog::OnSelchangePostCardList()
{
	m_pChosenInfo = NULL;

	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_POSTCARD_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			m_pChosenInfo = (CLabelData*)pList->GetItemData(nCurSel);
		}
	}
	UpdatePreview();
}

CLabelData* CPostCardTypeDialog::GetChosenPostCard(void)
{
   return m_pChosenInfo;
}

void CPostCardTypeDialog::UpdatePreview(void)
{
	CWnd* pPreview = GetDlgItem(IDC_MAKE_PREVIEW);
	if (pPreview != NULL)
	{
		pPreview->InvalidateRect(NULL);
	}
}

void CPostCardTypeDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_MAKE_PREVIEW:
		{
			DrawPreview(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwDialogColor::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}

void CPostCardTypeDialog::DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_pChosenInfo != NULL)
	{
		CDC dc;
		const int nGap = 2;

		dc.Attach(lpDrawItemStruct->hDC);
		CRect crPreview = lpDrawItemStruct->rcItem;
		CRect crDrawTo = crPreview;
		CRect crDrawn;

		CFixedPoint Dims = m_pChosenInfo->PaperDims();

		// Note that we switch x and y for this since these are inherently landscape.
		CFixed Max = MakeFixed(11.5);
		
      int nAdjust, nAdjustY;
      if (m_pChosenInfo->Orientation() == CPaperInfo::PAPER_Landscape)
      {
		   nAdjust = (int)MulDivFixed(crDrawTo.Width(), Max-Dims.y, Max);
		   nAdjustY = (int)MulDivFixed(crDrawTo.Height(), Max-Dims.x, Max);
      }
      else
      {
         nAdjust = (int)MulDivFixed (crDrawTo.Width(), Max-Dims.x, Max);
         nAdjustY = (int)MulDivFixed (crDrawTo.Height(), Max-Dims.y, Max);
      }

		if (nAdjust > nAdjustY)
		{
			nAdjust = nAdjustY;
		}
		if (nAdjust < 0)
		{
			nAdjust = 0;
		}

		crDrawTo.InflateRect(-nAdjust/2, -nAdjust/2);

		if (m_pChosenInfo->Orientation() == CPaperInfo::PAPER_Landscape)
         m_pChosenInfo->DrawPage3D(&dc, crDrawTo, nGap, TRUE, &crDrawn, &crPreview);
      else
         m_pChosenInfo->DrawPage3D(&dc, crDrawTo, nGap, FALSE, &crDrawn, &crPreview);

		CWindowsMetaFile MetaFile;
		
      if (MetaFile.Init(AfxGetResourceHandle(), "POSTCARDEWMF", "METAFILE"))
		{
			const ALDUS_WMF_HEADER* pHeader = MetaFile.AldusHeader();
			if (pHeader != NULL)
			{
				// Erase the areas of the preview window that are not covered
				// by the actual preview.

				CRect crWmf = crDrawn;
				crWmf.InflateRect(-3,-3);

				// Set the viewport to be our rectangle.

				dc.SaveDC();
				dc.SetMapMode(MM_ANISOTROPIC);
				dc.SetViewportOrg(crWmf.left, crWmf.top);
				dc.SetViewportExt(crWmf.Width(), crWmf.Height());
				MetaFile.Play(&dc, FALSE);
				dc.RestoreDC(-1);
			}
		}

		CString csDescription = m_pChosenInfo->GetDescription();

		int nContext = dc.SaveDC();
		if (nContext > 0)
		{
			CRect crLabel(crDrawn);
									
			dc.SetTextAlign(TA_CENTER|TA_TOP);
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
									
			dc.ExtTextOut(
							  (crLabel.left+crLabel.right)/2,
							  crLabel.bottom+9,
							  0,
							  NULL,
							  csDescription,
							  csDescription.GetLength(),
							  NULL);
										
			dc.RestoreDC(nContext);
		}
		dc.Detach();
	}
}

void CPostCardTypeDialog::OnDblclkPostCardList()
{
	OnOK();
}

void CPostCardTypeDialog::OnBack() 
{
   EndDialog (IDC_BACK);
}
