/*
// $Workfile: ENVTYPE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVTYPE.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:20   Fred
// Initial revision.
// 
//    Rev 1.6   17 Apr 1997 16:07:36   johno
// I killed the little guy!
// 
// 
//    Rev 1.5   17 Apr 1997 15:15:32   johno
// Changes for the lil' PrintMaster guy
// 
// 
//    Rev 1.4   15 Apr 1997 17:33:08   johno
// 
//    Rev 1.3   15 Apr 1997 16:55:44   johno
// Changes for "New Project" path
// 
// 
//    Rev 1.2   10 Apr 1997 15:54:46   johno
// Now a CPmwDialogColor
// 
//    Rev 1.1   07 May 1996 12:43:42   Jay
// From PMW2
// 
//    Rev 1.2   03 May 1996 15:20:18   JAY
// New label reading code
// 
//    Rev 1.1   17 Aug 1995 07:49:28   JAY
// Added double-click handler for the list.
// 
//    Rev 1.0   16 Aug 1995 18:03:00   JAY
// Envelope type.
*/

#include "stdafx.h"
#include "pmw.h"
#include "envtype.h"
#include "cwmf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeTypeDialog dialog

// JRO Parameters in this order so old code compiles

CEnvelopeTypeDialog::CEnvelopeTypeDialog(CPaperInfo* pOldInfo, CWnd* pParent /*=NULL*/, DWORD WizFlags, COLORREF cr)
	: CPmwDialogColor (CEnvelopeTypeDialog::IDD, pParent, cr)
{
	//{{AFX_DATA_INIT(CEnvelopeTypeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_WizFlags = WizFlags;
   m_pOldInfo = pOldInfo;
}

void CEnvelopeTypeDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnvelopeTypeDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEnvelopeTypeDialog, CPmwDialogColor)
	//{{AFX_MSG_MAP(CEnvelopeTypeDialog)
	ON_LBN_SELCHANGE(IDC_ENVELOPE_LIST, OnSelchangeEnvelopeList)
	ON_WM_DRAWITEM()
	ON_LBN_DBLCLK(IDC_ENVELOPE_LIST, OnDblclkEnvelopeList)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvelopeTypeDialog message handlers

void CEnvelopeTypeDialog::BuildList(void)
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_ENVELOPE_LIST)) != NULL)
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

		OnSelchangeEnvelopeList();				// Do this by hand.
	}
}

void CEnvelopeTypeDialog::OnOK()
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_ENVELOPE_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			m_pChosenInfo = (CLabelData*)pList->GetItemData(nCurSel);
		}
	}

	CPmwDialogColor::OnOK();
}

BOOL CEnvelopeTypeDialog::OnInitDialog()
{
	CPmwDialogColor::OnInitDialog();

	m_List.ReadList("ENVELOPE.DAT");

	BuildList();

   CreateWizzardButtons ();
   EnableWizardButtons (m_WizFlags);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEnvelopeTypeDialog::OnSelchangeEnvelopeList()
{
	m_pChosenInfo = NULL;

	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_ENVELOPE_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			m_pChosenInfo = (CLabelData*)pList->GetItemData(nCurSel);
		}
	}
	UpdatePreview();
}

CPaperInfo* CEnvelopeTypeDialog::GetChosenEnvelope(void)
{
	return m_pChosenInfo;
}

void CEnvelopeTypeDialog::UpdatePreview(void)
{
	CWnd* pPreview = GetDlgItem(IDC_MAKE_PREVIEW);
	if (pPreview != NULL)
	{
		pPreview->InvalidateRect(NULL);
	}
}

void CEnvelopeTypeDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
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

void CEnvelopeTypeDialog::DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct)
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
		
		int nAdjust = (int)MulDivFixed(crDrawTo.Width(), Max-Dims.y, Max);
		int nAdjustY = (int)MulDivFixed(crDrawTo.Height(), Max-Dims.x, Max);
		if (nAdjust > nAdjustY)
		{
			nAdjust = nAdjustY;
		}
		if (nAdjust < 0)
		{
			nAdjust = 0;
		}

		crDrawTo.InflateRect(-nAdjust/2, -nAdjust/2);

		m_pChosenInfo->DrawPage3D(&dc, crDrawTo, nGap, TRUE, &crDrawn, &crPreview);

		CWindowsMetaFile MetaFile;
		
		if (MetaFile.Init(AfxGetResourceHandle(), "ENVELOPEWMF", "METAFILE"))
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

void CEnvelopeTypeDialog::OnDblclkEnvelopeList()
{
	OnOK();
}

void CEnvelopeTypeDialog::OnBack() 
{
   EndDialog (IDC_BACK);
}
