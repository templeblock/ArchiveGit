/*
// $Workfile: matchset.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/matchset.cpp $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 1     12/01/98 9:12a Mwilson
// 
/ 
// MatchSet.cpp : implementation file
//
*/

#include "stdafx.h"
#include "catdata.h"
#include "pmw.h"
#include "Matchset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;  // PrintMaster Palette

/////////////////////////////////////////////////////////////////////////////
// CMatchingSetsDlg dialog

CMatchingSetsDlg::CMatchingSetsDlg(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CMatchingSetsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMatchingSetsDlg)
	//}}AFX_DATA_INIT

}


CMatchingSetsDlg::~CMatchingSetsDlg()
{
}

void CMatchingSetsDlg::DoDataExchange(CDataExchange* pDX)
{
	INHERITED::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatchingSetsDlg)
	DDX_Control(pDX, IDC_SUBCAT_LIST, m_cSubCatList);
	DDX_Control(pDX, IDC_PROJ_LIST, m_cProjList);
	DDX_Control(pDX, IDC_CAT_LIST, m_cCatList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMatchingSetsDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CMatchingSetsDlg)
	ON_CBN_SELCHANGE(IDC_CAT_LIST, OnSelChangeCatList)
	ON_CBN_SELCHANGE(IDC_SUBCAT_LIST, OnSelChangeSubCatList)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMatchingSetsDlg message handlers

BOOL CMatchingSetsDlg::OnInitDialog() 
{
	INHERITED::OnInitDialog();
	CString csDestFile("cparty.dat");

	TRY
	{
		//read in the data
		CFile cf(csDestFile, CFile::modeRead);
		CArchive ar(&cf, CArchive::load | CArchive::bNoFlushOnDelete);
		ar.m_bForceFlat = FALSE;
		m_cplCategoryList.Serialize(ar);
		ar.Close();
	}
	CATCH_ALL(e)
	{
		AfxMessageBox("Serialize failed");
		return FALSE;
	}
	END_CATCH_ALL

	m_cppPackPreview.Initialize(this, IDC_MATCHING_SETS_PREVIEW, NULL);

	//init category list
	POSITION pos = m_cplCategoryList.GetHeadPosition();
	while(pos != NULL)
	{
		CCatData* pCatData = m_cplCategoryList.GetNext(pos);
		int nIndex = m_cCatList.AddString(pCatData->m_csCategoryName);
		if(nIndex >= 0)
		{
			m_cCatList.SetItemDataPtr(nIndex, pCatData);
		}
	}

	m_cCatList.SetCurSel(0);
	OnSelChangeCatList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMatchingSetsDlg::OnSelChangeCatList() 
{
	int nIndex = m_cCatList.GetCurSel();
	if(nIndex >= 0)
	{
		m_cSubCatList.ResetContent();
		int nSubCatIndex = -1;
		//get category data
		CCatData* pData = (CCatData*)m_cCatList.GetItemDataPtr(nIndex);
		POSITION pos = pData->m_cplSubCatData.GetHeadPosition();
		//add subcategories to list
		while(pos != NULL)
		{
			CSubCategoryData* pSubCatData = pData->m_cplSubCatData.GetNext(pos);
			nSubCatIndex = m_cSubCatList.AddString(pSubCatData->m_csSubCategoryName);
			if(nSubCatIndex >= 0)
			{
				m_cSubCatList.SetItemDataPtr(nSubCatIndex, pSubCatData);
			}
			
		}

		m_cSubCatList.SetCurSel(0);
		//init subcategory list
		OnSelChangeSubCatList();
	}
}

void CMatchingSetsDlg::OnSelChangeSubCatList() 
{
	int nIndex = m_cSubCatList.GetCurSel();
	if(nIndex >= 0)
	{
		m_cProjList.ResetContent();
		//get subcategory data
		CSubCategoryData* pData = (CSubCategoryData*)m_cSubCatList.GetItemDataPtr(nIndex);
		POSITION pos = pData->m_cplProjData.GetHeadPosition();
		int nProjIndex = -1;
		//add projects to listbox
		while(pos != NULL)
		{
			CProjectData* pProjData = pData->m_cplProjData.GetNext(pos);
			nProjIndex = m_cProjList.AddString(pProjData->m_csProjectType);
			if(nProjIndex >= 0)
			{
				m_cProjList.SetItemDataPtr(nProjIndex, pProjData);
			}
		}
		//create a preview
		m_cppPackPreview.NewGraphicPreview(NULL, NULL, 
							(BITMAPINFO*)(pData->m_pBitmap + sizeof(BITMAPFILEHEADER)));
	}
	
}

void CMatchingSetsDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (nIDCtl == IDC_MATCHING_SETS_PREVIEW)
   {
		//draw the preview
		m_cppPackPreview.DrawPreview(lpDrawItemStruct);
   }
   else
   	INHERITED::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMatchingSetsDlg::OnOK()
{
	//get the selected projects paths
	for(int i = 0; i < m_cProjList.GetCount(); i++)
	{
		if(m_cProjList.GetCheck(i) == 1)
		{
			CProjectData* pProjData = (CProjectData*)m_cProjList.GetItemDataPtr(i);
			m_csaSelectedProjects.Add(pProjData->m_csFilePath);
		}
	}
	INHERITED::OnOK();
}

