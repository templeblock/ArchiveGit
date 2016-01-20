//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/SvBorder.cpp 1     3/03/99 6:12p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Log: /PM8/App/SvBorder.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 4     7/21/98 8:02p Hforman
// setting m_ofn.lpstrTitle; checking for maximum filename length
// 
// 3     4/16/98 7:06p Hforman
// add categories
// 
// 2     2/11/98 5:54p Hforman
// finished implementing
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "SvBorder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveBorderDlg

IMPLEMENT_DYNAMIC(CSaveBorderDlg, CFileDialog)

CSaveBorderDlg::CSaveBorderDlg(LPCTSTR szDefExt, LPCTSTR szFileName, LPCTSTR szFilter,
										 LPCTSTR lpInitDir, CWnd* pParentWnd)
 : CFileDialog(FALSE, szDefExt, szFileName, NULL, szFilter, pParentWnd)
{
   //{{AFX_DATA_INIT(CSaveBorderDlg)
	m_strCategory = _T("");
	m_fAddToGallery = TRUE;
	//}}AFX_DATA_INIT

	m_ofn.Flags |= (OFN_EXPLORER | OFN_ENABLETEMPLATE |
						OFN_HIDEREADONLY | OFN_NOREADONLYRETURN |
						OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST);
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD);
	m_ofn.lpstrInitialDir = lpInitDir;
	m_ofn.lpstrTitle = GET_PMWAPP()->GetResourceStringPointer(IDS_SAVE_BORDER);
}

void CSaveBorderDlg::DoDataExchange(CDataExchange* pDX)
{
   CFileDialog::DoDataExchange(pDX);

   //{{AFX_DATA_MAP(CSaveBorderDlg)
	DDX_Control(pDX, IDC_CATEGORY_COMBO, m_CategoryCombo);
	DDX_Control(pDX, IDC_NEW_CATEGORY, m_btnNewCategory);
	DDX_CBString(pDX, IDC_CATEGORY_COMBO, m_strCategory);
	DDX_Check(pDX, IDC_ADD_TO_GALLERY, m_fAddToGallery);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSaveBorderDlg, CFileDialog)
	//{{AFX_MSG_MAP(CSaveBorderDlg)
	ON_BN_CLICKED(IDC_ADD_TO_GALLERY, OnAddToGallery)
	ON_BN_CLICKED(IDC_NEW_CATEGORY, OnNewCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSaveBorderDlg::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	// fill the Categories combo with current categories
	m_CategoryManager.Initialize(CPMWCollection::typeArt);
	CStringArray categories;
	m_CategoryManager.GetUserCategories(categories);
	for (int i = 0; i < categories.GetSize(); i++)
		m_CategoryCombo.AddString(categories[i]);
	m_CategoryCombo.SetCurSel(0);	
	UpdateData(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

// This gets called after user presses "Save". We need to override this
// to call UpdateData, since OnOK() doesn't get called.
BOOL CSaveBorderDlg::OnFileNameOK()
{
	UpdateData(TRUE);

	CString strFolder = GetFolderPath();
	CString strFile = GetFileName();
	CString strExt = GetFileExt();
	if (strFolder.GetLength() + strFile.GetLength() + strExt.GetLength() >= (_MAX_PATH - 10))
	{
		AfxMessageBox(IDS_PATH_TOO_LONG);

		CEdit* pEdit = (CEdit*)GetParent()->GetDlgItem(edt1);
		if (pEdit)
		{
			pEdit->SetFocus();
			pEdit->SetSel(0, -1);
		}

		return TRUE;
	}

	return FALSE;	// assume filename is OK
}

void CSaveBorderDlg::OnAddToGallery() 
{
	// disable type-in fields if "Save To Gallery" turned off
	UpdateData(TRUE);
   
	m_CategoryCombo.EnableWindow(m_fAddToGallery);
	m_btnNewCategory.EnableWindow(m_fAddToGallery);
}

void CSaveBorderDlg::OnNewCategory() 
{
	CString strCat;
	if (m_CategoryManager.GetNewCategory(strCat))
	{
		int index = m_CategoryCombo.AddString(strCat);
		if (index != CB_ERR)
		{
			m_CategoryCombo.SetCurSel(index);
			UpdateData(TRUE);
		}
	}
}
