//
// $Workfile: prnaddlg.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/prnaddlg.cpp $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 12    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 11    5/01/98 12:19p Johno
// Got rid of extra '\' in INI file path
// 
// 10    2/19/98 5:06p Johno
// moved most of CJComboBox to Util as CComboFileNames
// 
// 9     2/17/98 12:37p Johno
// 
// 8     2/12/98 12:10p Johno
// File name sans extension used if friendly name not found
// 
// 7     2/11/98 5:56p Johno
// Support for friendly name INI file
// 
// 6     2/09/98 3:16p Johno
// Ignore print range in OnOK if "all" is selected.
// 
// 5     2/05/98 3:43p Johno
// Quick! Before the power goes out!
// 
// 4     1/30/98 11:13a Johno
// Compile update
// 
// 3     1/27/98 11:50a Johno
// For a version that will compile
// 
// 2     1/26/98 12:22p Johno
// Print Address book dialog code
// 
// 1     1/26/98 11:56a Johno
// 
// prnaddlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"

#include "prnaddlg.h"

#include "inifile.h" // For CJComboBox::GetFriendlyName

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CJComboBox::CJComboBox(void)
{
   mINIFileName = GetGlobalPathManager()->ExpandPath("[[A]]\\addrname.ini");
}

CString
CJComboBox::GetFriendlyName (LPCSTR istr)
{
   CIniFile INIFile (mINIFileName);
   CString cstr = INIFile.GetString ("FRIENDLY NAMES", istr, NULL);
   // If a friendly name found, return it
   if (!cstr.IsEmpty ())
      return cstr;
   // Friendly name not found...
   //  make a friendly name by stripping the extention from input name
   cstr = CComboFileNames::GetFriendlyName (istr);
   return cstr;
}
////////////////////////////////////////////////////////////////////////////
// CPrintAddressBookDlg dialog


CPrintAddressBookDlg::CPrintAddressBookDlg(CPmwDoc* pDoc, UINT nRecs)
	: CPmwPrint(NULL, pDoc, IDD)
{
	//{{AFX_DATA_INIT(CPrintAddressBookDlg)
	m_Copies = 1;
	m_FormatValue = _T("");
	mFrom = 1;
	mTo = nRecs;
	m_PrintAll = 0;
	//}}AFX_DATA_INIT
   
   m_pd.Flags &= ~PD_USEDEVMODECOPIES;     // Always want copies.

   m_pd.Flags |= PD_ENABLEPRINTTEMPLATE;
   m_pd.lpPrintTemplateName = MAKEINTRESOURCE(IDD);
#ifndef WIN32
   m_pd.Flags |= PD_ENABLESETUPTEMPLATE;
   m_pd.lpSetupTemplateName = MAKEINTRESOURCE(IDD_PRINT_SETUP);
#endif

   m_pd.hInstance = AfxGetResourceHandle();

/*
// Initialize some variables in the dialog from the printer rc.
// (Be sure the printer rc is initialized itself first.)
*/

   if (!printer_rc.is_printing)
   {
   // Not set yet! Initialize now.
   // If the variable becomes tristate in the dialog (as I plan for
   // someday), it should default to TRISTATE_SELECTED.
      printer_rc.is_printing = TRUE;
      printer_rc.smoothing = TRISTATE_ALL;
      printer_rc.coloring_book = TRISTATE_SELECTED;
   }

   //m_pd.nMinPage = 1;
   //m_pd.nMaxPage = (USHORT)(pView == NULL ? pDoc : pView->GetDocument())->Num

   m_pd.Flags &= ~PD_NOPAGENUMS;
   m_pd.nMinPage = 1;
   m_pd.nMaxPage = nRecs;

   m_pd.nFromPage = m_pd.nMinPage;
   m_pd.nToPage = m_pd.nMaxPage;

   mAborted = FALSE;
}

void CPrintAddressBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintAddressBookDlg)
	DDX_Control(pDX, 1153, mToControl);
	DDX_Control(pDX, 1152, mFromControl);
	DDX_Control(pDX, IDC_ADDRESS_BOOK_FORMAT, m_Format);
	DDX_Text(pDX, IIDC_PRINT_ADDRESS_BOOK_COPIES, m_Copies);
	DDX_CBString(pDX, IDC_ADDRESS_BOOK_FORMAT, m_FormatValue);
	DDX_Text(pDX, 1152, mFrom);
	DDX_Text(pDX, 1153, mTo);
	DDX_Radio(pDX, IDC_PRINT_ALL, m_PrintAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintAddressBookDlg, CPmwPrint)
	//{{AFX_MSG_MAP(CPrintAddressBookDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintAddressBookDlg message handlers

BOOL CPrintAddressBookDlg::OnInitDialog() 
{
	CPmwPrint::OnInitDialog();
	
   // Windows 95 changes this string. We set it back.
   SetDlgItemText(IDC_PRINT_ALL, GET_PMWAPP()->GetResourceStringPointer(IDS_PrintAll));
	
   mSelectedName = _T("");
   m_Format.ResetContent();
   // Get all names of PM posters representing print templates
   mPath = GetGlobalPathManager()->ExpandPath("[[A]]");
   
   CString  Path = mPath;
   Path += "*.sig";
   if (m_Format.Fill(Path) == FALSE)
   {
      mAborted = TRUE;   // Indicate what happened, and kill self
      Util::PostWmCommand(this, IDCANCEL, NULL, 0);
   }
   else mAborted = FALSE;

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}  
// Do our own print range validation here
void CPrintAddressBookDlg::OnOK() 
{
   UpdateData(TRUE);

   if (m_PrintAll != 0)
   {
      if ((mFrom > m_pd.nMaxPage) || (mFrom > mTo) || (mFrom < m_pd.nMinPage))
      {
         Util::MessageBox(MB_OK, -1, MAKEINTRESOURCE(IDS_ENTER_VAL_D), m_pd.nMinPage, m_pd.nMaxPage);
         mFromControl.SetFocus();
         return;
      }
      else
      if (mTo > m_pd.nMaxPage)
      {
         Util::MessageBox(MB_OK, -1, MAKEINTRESOURCE(IDS_ENTER_VAL_D), m_pd.nMinPage, m_pd.nMaxPage);
         mToControl.SetFocus();
         return;
      }
   }
/*
   mSelectedName.Empty();
   int ComboIndex;
   if ((ComboIndex = m_Format.FindStringExact(-1, m_FormatValue)) != CB_ERR)
   {   
      if ((ComboIndex = m_Format.GetItemData(ComboIndex)) != CB_ERR)
      mSelectedName = m_Format.GetFilename(ComboIndex);
   }
*/
   mSelectedName = m_Format.GetFullName(m_FormatValue);

   CPmwPrint::OnOK();
}
