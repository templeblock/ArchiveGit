//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CollSel.cpp 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/CollSel.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 6     1/28/99 2:07p Gbeddow
// support for NOT displaying "web art" in the Art Gallery and NOT
// displaying "art" in the Web Art Gallery (yet still displaying both in
// the
// stand-alone ClickArt Gallery browser); collection building support for
// matching the order of graphic types used in current collection building
// spreadsheets
// 
// 5     9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "CollSel.h"
#include "srchart.h"
#include "util.h"
#include "pmwcoll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCollectionSelectionDialog dialog


CCollectionSelectionDialog::CCollectionSelectionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCollectionSelectionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCollectionSelectionDialog)
	//}}AFX_DATA_INIT
	m_pCollectionManager = GetGlobalCollectionManager();
}

void CCollectionSelectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCollectionSelectionDialog)
	DDX_Control(pDX, IDC_INSTALLED_COLLECTION_LIST, m_clcCollections);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCollectionSelectionDialog, CDialog)
	//{{AFX_MSG_MAP(CCollectionSelectionDialog)
	ON_BN_CLICKED(IDC_INSTALL_COLLECTION, OnInstallCollection)
	ON_BN_CLICKED(IDC_REMOVE_ALL_BUILD_COLLECTIONS, OnRemoveAllBuildCollections)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollectionSelectionDialog message handlers

void CCollectionSelectionDialog::OnInstallCollection() 
{
   CString        csSourceFile, csFileTitle, csPath, csEntry;
   CFileDialog    cfdSourceFile(TRUE, ".cdt", "*.cdt",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Builder Art Files (*.ini)|All Files (*.*)|*.*||");

   if(cfdSourceFile.DoModal() != IDOK)
      return;

   csSourceFile = cfdSourceFile.GetPathName();
   csFileTitle = cfdSourceFile.GetFileTitle();
   csFileTitle.MakeUpper();
   Util::SplitPath(csSourceFile, &csPath, NULL);

   if(m_pCollectionManager->FindCollection(csFileTitle) != NULL)
      {
         AfxMessageBox("Collection with the same base name is Already Installed");
         return;
      }

   CIniFile    IniFile(GetGlobalPathManager()->ExpandPath("[[S]]\\BUILD.INI"));
   IniFile.WriteString("CONTENTS", csFileTitle, "");
   IniFile.WriteString(csFileTitle, "CD Directory", csPath);
   IniFile.WriteString(csFileTitle, "Friendly Name", csFileTitle);
   IniFile.WriteString(csFileTitle, "Type", "Art");
   csEntry = csFileTitle + ".ico";
   IniFile.WriteString(csFileTitle, "Icon", csEntry);
   IniFile.WriteInteger(csFileTitle, "Version", 1);

   GET_PMWAPP()->GetSearchCollections()->Add(csFileTitle);
	GET_PMWAPP()->ReopenCollectionManager();

   UpdateCollectionList();
}

BOOL CCollectionSelectionDialog::OnInitDialog() 
{
   int      nColumnIndex = 0;

	CDialog::OnInitDialog();

   LV_COLUMN   lvColumnData;

   lvColumnData.mask =  LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
   lvColumnData.iSubItem = nColumnIndex;
   lvColumnData.pszText = "Friendly Name";
   lvColumnData.cx = 170;            // Pixels
   lvColumnData.fmt = LVCFMT_LEFT;
	m_clcCollections.InsertColumn(nColumnIndex++, &lvColumnData);

   lvColumnData.mask =  LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
   lvColumnData.iSubItem = nColumnIndex;
   lvColumnData.pszText = "Type";
   lvColumnData.cx = 80;            // Pixels
   lvColumnData.fmt = LVCFMT_LEFT;
	m_clcCollections.InsertColumn(nColumnIndex++, &lvColumnData);

   lvColumnData.mask =  LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
   lvColumnData.iSubItem = nColumnIndex;
   lvColumnData.pszText = "Base Name";
   lvColumnData.cx = 80;            // Pixels
   lvColumnData.fmt = LVCFMT_LEFT;
	m_clcCollections.InsertColumn(nColumnIndex++, &lvColumnData);

   lvColumnData.mask =  LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
   lvColumnData.iSubItem = nColumnIndex;
   lvColumnData.pszText = "Path";
   lvColumnData.cx = 220;            // Pixels
   lvColumnData.fmt = LVCFMT_LEFT;
	m_clcCollections.InsertColumn(nColumnIndex++, &lvColumnData);

   lvColumnData.mask =  LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
   lvColumnData.iSubItem = nColumnIndex;
   lvColumnData.pszText = "Searchable";
   lvColumnData.cx = 80;            // Pixels
   lvColumnData.fmt = LVCFMT_LEFT;
	m_clcCollections.InsertColumn(nColumnIndex++, &lvColumnData);

   UpdateCollectionList();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCollectionSelectionDialog::UpdateCollectionList()
   {
   m_clcCollections.DeleteAllItems();

   int nNumCollections = m_pCollectionManager->NumberOfCollections(CPMWCollection::typeArt, CPMWCollection::subTypeNone);
   for(int i=0;i<nNumCollections;i++)
      {
      	CPMWCollection* pCollection;
         CString        csCollectionType = "Art";
         
         pCollection = m_pCollectionManager->GetCollection(i, CPMWCollection::typeArt, CPMWCollection::subTypeNone);
         ASSERT(pCollection);
         if(pCollection)
            {
               CString  csLBItem, csSearchable;
               int      nColumnIndex = 1;

		         m_clcCollections.InsertItem(i,
                  (LPCSTR) pCollection->GetFriendlyName());

#if 0
               LV_ITEM  lvItem;

		         lvItem.mask = LVIF_TEXT;
		         lvItem.iItem = i;
		         lvItem.iSubItem = 0;
		         lvItem.pszText = "Art";
		         m_clcCollections.SetItemText(&lvItem);
#endif
		         m_clcCollections.SetItemText(
                  i,
                  nColumnIndex++,
                  csCollectionType);

		         m_clcCollections.SetItemText(
                  i,
                  nColumnIndex++,
                  (LPCSTR) pCollection->GetBaseName());

		         m_clcCollections.SetItemText(
                  i,
                  nColumnIndex++,
                  (LPCSTR) (GetGlobalPathManager()->ExpandPath(pCollection->GetCDDir())));

               if(GET_PMWAPP()->GetSearchCollections()->Find(pCollection->GetBaseName()) >= 0)
                  csSearchable = "Yes";
               else
                  csSearchable = "No";
		         m_clcCollections.SetItemText(
                  i,
                  nColumnIndex++,
                  (LPCSTR) csSearchable);
            }
      }
   }


void CCollectionSelectionDialog::OnRemoveAllBuildCollections() 
{
   CString  csBuildFile(GetGlobalPathManager()->ExpandPath("[[S]]\\BUILD.INI"));
   CString  csSearchArtFile(GetGlobalPathManager()->ExpandPath("[[S]]\\SRCHART.INI"));

   remove(csBuildFile);
   remove(csSearchArtFile);

	GET_PMWAPP()->ReopenCollectionManager();
   UpdateCollectionList();
}
