//////////////////////////////////////////////////////////////////////////////
// $Workfile: SAVEPROJ.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/SAVEPROJ.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 5     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 4     7/21/98 8:02p Hforman
// showing helpful hint; checking for maximum filename length
// 
// 3     4/16/98 6:57p Hforman
// add categories to dialog
// 
// 2     4/13/98 2:34p Hforman
// using Common Dialog, added categories, etc.
// 
//    Rev 1.0   14 Aug 1997 15:25:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:22   Fred
// Initial revision.
// 
//    Rev 1.4   09 May 1997 10:16:58   Jay
// Project Save
// 
//    Rev 1.3   07 May 1997 17:21:50   Jay
// More save project tweaks.
// 
//    Rev 1.2   07 May 1997 12:10:52   Jay
// User directory and collections
// 
//    Rev 1.1   06 May 1997 10:30:10   Jay
// New browser stuff.
// 
//    Rev 1.0   28 Mar 1997 17:11:06   Jay
// Ripped out albums, sound manager, side bar, etc.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "saveproj.h"
#include "pmwdoc.h"
#include "util.h"
#include "utils.h"
#include "pmdirdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSaveProjectDialog dialog

CSaveProjectDialog::CSaveProjectDialog(LPCTSTR szExt, LPCTSTR szFileName,
													LPCTSTR szFilter, LPCTSTR szInitialDir, 
													CPmwDoc* pDoc)
	: CFileDialog(FALSE, szExt, szFileName, NULL, szFilter)
{
	//{{AFX_DATA_INIT(CSaveProjectDialog)
	m_fAddToGallery = TRUE;
	m_fEmbedPictures = FALSE;
	m_csCategory = _T("");
	//}}AFX_DATA_INIT

	m_ofn.Flags |= (OFN_EXPLORER | OFN_ENABLETEMPLATE |
						OFN_HIDEREADONLY | OFN_NOREADONLYRETURN |
						OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST);
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD);
	m_ofn.lpstrInitialDir = szInitialDir;
}

CSaveProjectDialog::~CSaveProjectDialog()
{
}

void CSaveProjectDialog::DoDataExchange(CDataExchange* pDX)
{
   CFileDialog::DoDataExchange(pDX);

   //{{AFX_DATA_MAP(CSaveProjectDialog)
	DDX_Control(pDX, IDC_NEW_CATEGORY, m_btnNewCategory);
	DDX_Control(pDX, IDC_CATEGORY_COMBO, m_CategoryCombo);
	DDX_Check(pDX, IDC_ADD_TO_GALLERY, m_fAddToGallery);
	DDX_Check(pDX, IDC_EMBED_PICTURE, m_fEmbedPictures);
	DDX_CBString(pDX, IDC_CATEGORY_COMBO, m_csCategory);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveProjectDialog, CFileDialog)
	//{{AFX_MSG_MAP(CSaveProjectDialog)
	ON_BN_CLICKED(IDC_ADD_TO_GALLERY, OnAddToGallery)
	ON_BN_CLICKED(IDC_NEW_CATEGORY, OnNewCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSaveProjectDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	// init the category manager
	m_CategoryManager.Initialize(CPMWCollection::typeProjects);

	// fill the category combo with current categories
	CStringArray categories;
	m_CategoryManager.GetUserCategories(categories);
	for (int i = 0; i < categories.GetSize(); i++)
		m_CategoryCombo.AddString(categories[i]);
	m_CategoryCombo.SetCurSel(0);	
	UpdateData(TRUE);

	GET_PMWAPP()->ShowHelpfulHint("SaveDialog", this);
	return TRUE;
}

// This gets called after user presses "Save". We need to override this
// to call UpdateData, since it doesn't get called.
BOOL CSaveProjectDialog::OnFileNameOK()
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
	else
		return FALSE;	// assume filename is OK
}

void CSaveProjectDialog::OnAddToGallery() 
{
	UpdateData(TRUE);

	// enable/disable category controls
	m_CategoryCombo.EnableWindow(m_fAddToGallery);
	m_btnNewCategory.EnableWindow(m_fAddToGallery);
}

void CSaveProjectDialog::OnNewCategory() 
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
