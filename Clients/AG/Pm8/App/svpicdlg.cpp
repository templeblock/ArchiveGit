// $Workfile: svpicdlg.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
// 
//
// $Log: /PM8/App/svpicdlg.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 17    7/21/98 8:02p Hforman
// setting m_ofn.lpstrTitle; checking for maximum filename length
// 
// 16    4/13/98 6:54p Hforman
// more category stuff
// 
// 15    4/13/98 2:33p Hforman
// add Category, remove Friendly Name and Description
// 
// 14    1/29/98 8:13p Hforman
// add support for "pmo" file type; other cleanup
// 
// 13    1/12/98 1:46p Johno
// Added Keywords edit box
// 
// 12    1/09/98 5:26p Johno
// Changes for "Save in Art Gallery" option with "Save as Graphic File".
// 
// 11    12/05/97 4:26p Johno
// Minor changes
// 
// 10    12/04/97 5:15p Johno
// Moved filter string stuff from here
// 
// 9     12/04/97 5:12p Johno
// 
// 
// 8     12/03/97 5:30p Johno
// Added filter string to resources
// Constructor now takes as parameter
// 
// 7     11/21/97 5:21p Johno
// General update
// 
// 6     11/20/97 10:37a Johno
// Use CPmwDoc::CDIBInfo::PrepareData
// 
// 5     11/19/97 2:38p Johno
// Re - sync to Source Safe
// 
// 4     11/18/97 9:48a Johno
// Corrected filter strings
// 
// 3     11/14/97 11:58a Johno
// 
// 2     11/10/97 2:52p Johno
// Split Save2pic.cpp into:
// save2pic.cpp, svpicdlg.cpp, and svpicdlg.h
// 
// 7     11/07/97 5:27p Johno
// 2 sets of output sizes - selected objects and all objects
// enum for graphic type
// 
// 6     11/07/97 3:19p Johno
// Dialog size options
// 
// 5     11/06/97 5:50p Johno
// 
// 4     11/06/97 5:48p Johno
// Makes fixed size GIF!
// 
// 3     11/05/97 5:29p Johno
// CFileDialog derived custom dialog
// 
// 2     11/05/97 2:08p Johno
// 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "svpicdlg.h"


BOOL  CFileSaveAsGraphicDlg::m_Aspect = TRUE;

/////////////////////////////////////////////////////////////////////////////
// CFileSaveAsGraphicDlg

IMPLEMENT_DYNAMIC(CFileSaveAsGraphicDlg, CFileDialog)

CFileSaveAsGraphicDlg::CFileSaveAsGraphicDlg(BOOL fSelection, LPCSTR Filter,
															LPCSTR defExt, LPCSTR initialDir, CWnd* pParentWnd)
 : CFileDialog(FALSE, defExt, NULL, NULL, Filter, pParentWnd)
{
   //{{AFX_DATA_INIT(CFileSaveAsGraphicDlg)
	m_Width = MIN_WIDTH;
	m_Height = MIN_HEIGHT;
	m_SaveAll = (fSelection == TRUE) ? 1 : 0;
	m_fAddToGallery = TRUE;
	m_Aspect = TRUE;
	m_strCategory = _T("");
	//}}AFX_DATA_INIT

	m_ofn.Flags |=	(OFN_EXPLORER | OFN_ENABLETEMPLATE |
						 OFN_HIDEREADONLY | OFN_NOREADONLYRETURN |
						 OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST);

	m_ofn.lpTemplateName = MAKEINTRESOURCE (IDD);
	m_ofn.hInstance = AfxGetResourceHandle ();
	m_ofn.lpstrInitialDir = initialDir;
	m_ofn.lpstrTitle = GET_PMWAPP()->GetResourceStringPointer(IDS_EXPORT_AS);

	m_nGraphicIndex = -1;
}

void CFileSaveAsGraphicDlg::DoDataExchange(CDataExchange* pDX)
{
   CFileDialog::DoDataExchange(pDX);

   if (pDX->m_bSaveAndValidate == FALSE)
   {
      m_Width = (UINT)m_dWidth;
      m_Height = (UINT)m_dHeight;
   }

   //{{AFX_DATA_MAP(CFileSaveAsGraphicDlg)
	DDX_Control(pDX, IDC_NEW_CATEGORY, m_btnNewCategory);
	DDX_Control(pDX, IDC_CATEGORY_COMBO, m_CategoryCombo);
	DDX_Control(pDX, IDC_SAVE_2_PICT_ASPECT, m_btnAspect);
	DDX_Control(pDX, IDC_SAVE_2_PICT_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_SAVE_2_PICT_HEIGHT, m_editHeight);
   DDX_Text (pDX, IDC_SAVE_2_PICT_WIDTH, m_Width);
   DDV_MinMaxInt (pDX, m_Width, MIN_WIDTH, MAX_WIDTH); 
   DDX_Text (pDX, IDC_SAVE_2_PICT_HEIGHT, m_Height);
   DDV_MinMaxInt (pDX, m_Height, MIN_HEIGHT, MAX_HEIGHT); 
   DDX_Radio (pDX, IDC_SAVE_2_ALL, m_SaveAll);
	DDX_Check(pDX, IDC_ADD_TO_GALLERY, m_fAddToGallery);
   DDX_Check (pDX, IDC_SAVE_2_PICT_ASPECT, m_Aspect);
	DDX_CBString(pDX, IDC_CATEGORY_COMBO, m_strCategory);
	DDV_MaxChars(pDX, m_strCategory, 40);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileSaveAsGraphicDlg, CFileDialog)
	//{{AFX_MSG_MAP(CFileSaveAsGraphicDlg)
	ON_EN_KILLFOCUS(IDC_SAVE_2_PICT_HEIGHT, OnKillFocusHeight)
	ON_EN_KILLFOCUS(IDC_SAVE_2_PICT_WIDTH, OnKillFocusWidth)
	ON_BN_CLICKED(IDC_SAVE_2_ALL, OnSave2All)
	ON_BN_CLICKED(IDC_SAVE_2_SEL, OnSave2Sel)
	ON_BN_CLICKED(IDC_ADD_TO_GALLERY, OnAddToGallery)
	ON_BN_CLICKED(IDC_NEW_CATEGORY, OnNewCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPoint CFileSaveAsGraphicDlg::PboxToScreen (PBOX *pb, int LogPixX, int LogPixY)
{
   CPoint   p;

   p.x = MulDiv ((pb->x1 - pb->x0), LogPixX, PAGE_RESOLUTION);
   p.y = MulDiv ((pb->y1 - pb->y0), LogPixY, PAGE_RESOLUTION);

   return p;
}

void CFileSaveAsGraphicDlg::SetDimensions (PBOX *pbAll, PBOX *pbSell, int LogPixX, int LogPixY)
{
   CPoint   pa, ps;
   pa = PboxToScreen (pbAll, LogPixX, LogPixY);
   ps = PboxToScreen (pbSell, LogPixX, LogPixY);

   SetDimensions (&pa, &ps);
}

void CFileSaveAsGraphicDlg::SetDimensions (CPoint *pAll, CPoint *pSell)
{
   DimensionsAll = *pAll;
   DimensionsSel = *pSell;
   
   SelectDimensionSet ();
}

CPoint CFileSaveAsGraphicDlg::GetSelectedDimensionSet()
{
   if (IsSaveSelected () == TRUE)
      return DimensionsSel;
   else
      return DimensionsAll;
}

void CFileSaveAsGraphicDlg::SelectDimensionSet()
{
   CPoint p = GetSelectedDimensionSet ();
   
   m_dWidth = (double)p.x;
   m_dHeight = (double)p.y;
   
   LimitDimensions ();
   CalcHeight ();
   CalcWidth ();
}

void CFileSaveAsGraphicDlg::CalcWidth()
{
   LimitDimensions ();
   CPoint p = GetSelectedDimensionSet ();
   m_dWidth = (m_dHeight * ((double)p.x / (double)p.y));
}

void CFileSaveAsGraphicDlg::CalcHeight()
{
   LimitDimensions ();
   CPoint p = GetSelectedDimensionSet ();
   m_dHeight = (m_dWidth * ((double)p.y / (double)p.x));
}

void CFileSaveAsGraphicDlg::LimitDimensions()
{
   if (m_dWidth > MAX_WIDTH)
      m_dWidth = MAX_WIDTH;
   else if (m_dWidth < MIN_WIDTH)
      m_dWidth = MIN_WIDTH;

   if (m_dHeight > MAX_HEIGHT)
      m_dHeight = MAX_HEIGHT;
   else if (m_Height < MIN_HEIGHT)
      m_dHeight = MIN_HEIGHT;
}

/////////////////////////////////////////////////////////////////////////////
// CFileSaveAsGraphicDlg message handlers

BOOL CFileSaveAsGraphicDlg::OnInitDialog()
{
   CFileDialog::OnInitDialog ();

	// don't allow editing of Image Size fields for "pmo" file type
	// ASSUMPTION: "*.pmo" is the default
	m_btnAspect.EnableWindow(FALSE);
	m_editWidth.EnableWindow(FALSE);
	m_editHeight.EnableWindow(FALSE);

	m_CategoryManager.Initialize(CPMWCollection::typeArt);

	// fill the Categories combo with current categories
	CStringArray categories;
	m_CategoryManager.GetUserCategories(categories);
	for (int i = 0; i < categories.GetSize(); i++)
		m_CategoryCombo.AddString(categories[i]);
	m_CategoryCombo.SetCurSel(0);	
	UpdateData(TRUE);

   if (!IsSelectedAvailable())
   {
      CButton  *pBtn = (CButton*)GetDlgItem(IDC_SAVE_2_SEL);
      if (pBtn)
         pBtn->EnableWindow(FALSE);
   }

   return TRUE;
}

void CFileSaveAsGraphicDlg::OnTypeChange()
{
	CFileDialog::OnTypeChange();

	// if type is ".pmo", gray out Image Size options
	// ASSUMPTION: "*.pmo" is first in the list
	BOOL bAllowSize = (m_ofn.nFilterIndex != 1);
	m_btnAspect.EnableWindow(bAllowSize);
	m_editWidth.EnableWindow(bAllowSize);
	m_editHeight.EnableWindow(bAllowSize);
}

BOOL CFileSaveAsGraphicDlg::OnFileNameOK ()
{
	UpdateData(TRUE);

	ASSERT (m_ofn.nFilterIndex > 0);
	m_nGraphicIndex = m_ofn.nFilterIndex;

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

	return FALSE;
}

void CFileSaveAsGraphicDlg::OnKillFocusHeight () 
{
   if ((UpdateData (TRUE) != FALSE) && (m_Aspect == TRUE))
   {
      if (m_Height != (UINT)m_dHeight)
      {
         m_dHeight = (double)m_Height; 
         CalcWidth ();
         UpdateData (FALSE);
      }
   }
}

void CFileSaveAsGraphicDlg::OnKillFocusWidth() 
{
   if ((UpdateData (TRUE) != FALSE) && (m_Aspect == TRUE))
   {
      if (m_Width != (UINT)m_dWidth)
      {
         m_dWidth = (double)m_Width;
         CalcHeight ();
         UpdateData (FALSE);
      }
   }
}

void CFileSaveAsGraphicDlg::OnSave2All() 
{
   UpdateData (TRUE);
   SelectDimensionSet ();
   UpdateData (FALSE);
}

void CFileSaveAsGraphicDlg::OnSave2Sel() 
{
   UpdateData (TRUE);
   SelectDimensionSet ();
   UpdateData (FALSE);
}

void CFileSaveAsGraphicDlg::OnAddToGallery() 
{
	// disable type-in fields if "Save To Gallery" turned off
   UpdateData (TRUE);
   
	m_CategoryCombo.EnableWindow(m_fAddToGallery);
	m_btnNewCategory.EnableWindow(m_fAddToGallery);
}

void CFileSaveAsGraphicDlg::OnNewCategory() 
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
