//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/UsrCtgry.cpp 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
// User Category Management.
//
//
// $Log: /PM8/App/UsrCtgry.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 7     9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 6     7/26/98 7:05p Hforman
// changed baseclass from CDialog to CPmwDialog for palette handling
// 
// 5     7/23/98 10:39a Hforman
// not allowing names with just spaces
// 
// 4     5/19/98 2:34p Hforman
// checking that category doesn't already exist
// 
// 3     4/30/98 6:45p Hforman
// fix maximum category name length
// 
// 2     4/16/98 6:59p Hforman
// add default category string
// 
// 1     4/13/98 11:11a Hforman
// initial version
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "UsrCtgry.h"


CUserCategoryManager::CUserCategoryManager()
{
	m_fInitialized = FALSE;
}

CUserCategoryManager::~CUserCategoryManager()
{
}

BOOL CUserCategoryManager::Initialize(CPMWCollection::CollectionTypes type)
{
	// Note: this could be called multiple times with different collection types

	m_fInitialized = FALSE;
	m_CollType = type;

	// get a pointer to the proper user collection
	m_pUserCollection = GetGlobalCollectionManager()->GetUserCollection(m_CollType);
	if (m_pUserCollection == NULL)
	{
		ASSERT(m_pUserCollection);
		return FALSE;
	}

	// Attempt to fill our array with the current user categories.
	// We always have at least the one default category.
	CString strDefault;
	if (m_CollType == CPMWCollection::typeProjects)
		strDefault.LoadString(IDS_DEF_PROJECT_CATEGORY);
	else
		strDefault.LoadString(IDS_DEF_ART_CATEGORY);

	m_CategoryArray.Add(strDefault);

	// Open up a stream for reading category information.
	CCategoryDatabase* pDatabase;
	if (m_pUserCollection->NewCategoryStream(&pDatabase) == ERRORCODE_None)
	{
		// get the category directory
		CCategoryDir* pCategoryDir = pDatabase->GetDir();
		ASSERT(pCategoryDir);
		if (pCategoryDir)
		{
			// add categories to our array
			int nCategory = 0;
			while (nCategory >= 0)
			{
				CCategoryEntry Category;
				CString strName;
				if (m_CollType == CPMWCollection::typeProjects)
				{
					// project categories are 1 level below "project type"
					int nSubCategory = pCategoryDir->GetFirstChild(nCategory);
					while (nSubCategory >= 0)
					{
						if (pCategoryDir->GetItem(nSubCategory, &Category) == ERRORCODE_None)
						{
							Category.GetName(&strName);
							if (!Util::IsStrInArray(m_CategoryArray, strName))
								m_CategoryArray.Add(strName);
						}
						nSubCategory = pCategoryDir->GetNextSibling(nSubCategory);
					}
				}
				else
				{
					if (pCategoryDir->GetItem(nCategory, &Category) == ERRORCODE_None)
					{
						Category.GetName(&strName);
						if (!Util::IsStrInArray(m_CategoryArray, strName))
							m_CategoryArray.Add(strName);
					}
				}

				nCategory = pCategoryDir->GetNextSibling(nCategory);
			}
	
			m_fInitialized = TRUE;
		}

		m_pUserCollection->ReleaseCategoryStream(pDatabase);
	}

	return m_fInitialized;
}

void CUserCategoryManager::GetUserCategories(CStringArray& catArray)
{
	catArray.Copy(m_CategoryArray);
}

BOOL CUserCategoryManager::GetNewCategory(CString& strCat)
{
	CAddUserCategory dlg(m_CategoryArray);
	if (dlg.DoModal() == IDOK)
	{
		strCat = dlg.m_strCategory;
		m_CategoryArray.Add(strCat);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAddUserCategory dialog

CAddUserCategory::CAddUserCategory(const CStringArray& catArray, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CAddUserCategory::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddUserCategory)
	m_strCategory = _T("");
	//}}AFX_DATA_INIT

	m_CategoryArray.Append(catArray);
}


void CAddUserCategory::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddUserCategory)
	DDX_Control(pDX, IDC_EDIT_CATEGORY, m_editCategory);
	DDX_Text(pDX, IDC_EDIT_CATEGORY, m_strCategory);
	DDV_MaxChars(pDX, m_strCategory, MAX_CATEGORY_NAME_LEN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddUserCategory, CPmwDialog)
	//{{AFX_MSG_MAP(CAddUserCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddUserCategory message handlers

void CAddUserCategory::OnOK() 
{
	UpdateData(TRUE);

	m_strCategory.TrimRight();
	if (m_strCategory.IsEmpty())
	{
		UpdateData(FALSE);
		m_editCategory.SetFocus();
		return;
	}

	if (Util::IsStrInArray(m_CategoryArray, m_strCategory))
	{
		AfxMessageBox(IDS_CATEGORY_EXISTS);
		m_editCategory.SetFocus();
		m_editCategory.SetSel(0, -1);
		return;
	}
	
	CPmwDialog::OnOK();
}
