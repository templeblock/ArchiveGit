//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/PhPrjDlg.cpp 1     3/03/99 6:09p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/PhPrjDlg.cpp $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 4     2/24/99 9:14p Psasse
// fixed a memory leak
// 
// 3     2/09/99 9:13p Psasse
// No longer have description strings
// 
// 2     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     12/24/98 4:58p Psasse
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "project.h"
#include "PhPrjDlg.h"
#include "photoprj.h"
#include "printer.h"
#include "colordef.h"
#include "cwmf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NUM_SUBSECTIONS 3

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectsTypeDlg dialog


CPhotoProjectsTypeDlg::CPhotoProjectsTypeDlg(CPaperInfo* pOldInfo, CWnd* pParent /*=NULL*/, DWORD WizFlags, COLORREF cr /*INVALID_DIALOG_COLOR*/)
	: CPmwDialogColor(CPhotoProjectsTypeDlg::IDD, pParent, cr)
{
	//{{AFX_DATA_INIT(CPhotoProjectsTypeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	if (pOldInfo == NULL) // || pOldInfo->Class() == CPaperInfo::CLASS_PhotoProjects)
	{
		m_pOldInfo = pOldInfo;
	}
	else
	{
		m_pOldInfo = NULL;
	}
	m_pChosenPhotoProject = NULL;

	m_WizFlags = WizFlags;

}	

CPhotoProjectsTypeDlg::~CPhotoProjectsTypeDlg()
{
}

void CPhotoProjectsTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPhotoProjectsTypeDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPhotoProjectsTypeDlg, CPmwDialogColor)
	//{{AFX_MSG_MAP(CPhotoProjectsTypeDlg)
	ON_LBN_SELCHANGE(IDC_PHOTOPROJECTS_LIST, OnSelchangePhotoProjectsList)
	ON_LBN_DBLCLK(IDC_PHOTOPROJECTS_LIST, OnDblclkPhotoProjectsList)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhotoProjectsTypeDlg message handlers

void CPhotoProjectsTypeDlg::BuildList(void)
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
	CListBox* pList;

	if ((pList = (CListBox*)GetDlgItem(IDC_PHOTOPROJECTS_LIST)) != NULL)
	{
		pList->SetRedraw(FALSE);
		pList->ResetContent();
		int nPhotoPrjs = m_List.PhotoProjects();
		for (int nPhotoPrj = 0; nPhotoPrj < nPhotoPrjs; nPhotoPrj++)
		{
			CPhotoPrjData* pPhotoPrj = (CPhotoPrjData*)m_List.PhotoProject(nPhotoPrj);
			ASSERT(pPhotoPrj != NULL);

			int nIndex = pList->AddString(pPhotoPrj->m_pSubAreaData[0]->GetName());
			if (nIndex >= 0)
			{
				pList->SetItemData(nIndex, (DWORD)pPhotoPrj);

				if (pPhotoPrj->m_pSubAreaData[0]->GetName() == csOldName)
				{
					nNewSel = nIndex;
				}
			}
		}

	/*
	// Set the initial entry as necessary.
	*/

		if (pList->GetCount() > 0)
		{
			pList->SetCurSel(nNewSel);
			OnSelchangePhotoProjectsList();
		}
		pList->SetRedraw(TRUE);
	}


}

void CPhotoProjectsTypeDlg::OnOK()
{
	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_PHOTOPROJECTS_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			m_pChosenPhotoProject = (CPhotoPrjData*)pList->GetItemData(nCurSel);

			int nType = nCurSel;
			m_PhotoProjectType = (PHOTO_PROJECT_TYPE)nType;
		}
	}
	CPmwDialogColor::OnOK();
}


BOOL CPhotoProjectsTypeDlg::OnInitDialog()
{
	CPmwDialogColor::OnInitDialog();

	/*
	// Read the list.
	*/

	m_List.ReadList("PHOTOPRJ.DAT");


	// Build the list box.
	BuildList();

   CreateWizzardButtons ();
   EnableWizardButtons (m_WizFlags);

	CStatic* pDescription;
	if ((pDescription = (CStatic*)GetDlgItem(IDC_PREVIEW_DESCRIPTION)) != NULL)
		pDescription->SetWindowText("");

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CPhotoProjectsTypeDlg::OnSelchangePhotoProjectsList()
{

	CListBox* pList;
	if ((pList = (CListBox*)GetDlgItem(IDC_PHOTOPROJECTS_LIST)) != NULL)
	{
		int nCurSel = pList->GetCurSel();
		if (nCurSel != -1)
		{
			m_pChosenPhotoProject = (CPhotoPrjData*)pList->GetItemData(nCurSel);
		}
	}


	UpdatePreview();
}

void CPhotoProjectsTypeDlg::UpdatePreview(void)
{
	CWnd* pPreview = GetDlgItem(IDC_PREVIEW);
	if (pPreview != NULL)
	{
		pPreview->InvalidateRect(NULL);
	}
}

void CPhotoProjectsTypeDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_PREVIEW:
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


CPhotoLabelData* CPhotoProjectsTypeDlg::GetChosenPhotoProject(void)
{
	return m_pChosenPhotoProject->m_pSubAreaData[0];
}

void CPhotoProjectsTypeDlg::OnPhotoProjectTypeChanged()
{
	BuildList();
}

void CPhotoProjectsTypeDlg::OnDblclkPhotoProjectsList()
{
	OnOK();
}

void CPhotoProjectsTypeDlg::OnBack() 
{
   EndDialog (IDC_BACK);
}

void CPhotoProjectsTypeDlg::DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	BOOL fEraseBackground = TRUE;

	if (m_pChosenPhotoProject != NULL)
	{
		for (int x = 0; x < NUM_SUBSECTIONS; x++)
		{
		
			if((m_pChosenPhotoProject->m_pSubAreaData[x]->SlotsAcross() != 0) &&
				 (m_pChosenPhotoProject->m_pSubAreaData[x]->SlotsDown() != 0))
			{
				CDC dc;
				const int nGap = 2;

				dc.Attach(lpDrawItemStruct->hDC);
				CRect crPreview = lpDrawItemStruct->rcItem;
				CRect crDrawTo = crPreview;
				CRect crDrawn;

				CFixedPoint Dims = m_pChosenPhotoProject->m_pSubAreaData[x]->PaperDims();
				
				CFixedPoint pt;
					pt.x = 0;
					pt.y = 0; //x*50;

				m_pChosenPhotoProject->m_pSubAreaData[x]->OriginOffset(pt);

				// Note that we switch x and y for this since these are inherently landscape.
				CFixed Max = MakeFixed(11.5);
				
				int nAdjust, nAdjustY;
				if (m_pChosenPhotoProject->m_pSubAreaData[x]->Orientation() == CPaperInfo::PAPER_Landscape)
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

				if(x == 0)
					fEraseBackground = TRUE;
				else
					fEraseBackground = FALSE;

				if (m_pChosenPhotoProject->m_pSubAreaData[x]->Orientation() == CPaperInfo::PAPER_Landscape)
					m_pChosenPhotoProject->m_pSubAreaData[x]->DrawPage3D(&dc, crDrawTo, nGap, TRUE, &crDrawn, &crPreview, fEraseBackground);
				else
					m_pChosenPhotoProject->m_pSubAreaData[x]->DrawPage3D(&dc, crDrawTo, nGap, FALSE, &crDrawn, &crPreview, fEraseBackground);

				CWindowsMetaFile MetaFile;
				
				if (MetaFile.Init(AfxGetResourceHandle(), "PHOTOPROJECTWMF", "METAFILE"))
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

				dc.Detach();
		
			}
		}
	}
}

