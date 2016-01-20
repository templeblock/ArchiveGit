// $Workfile: hyplkdlg.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/hyplkdlg.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 18    2/01/99 9:37a Johno
// Disable page link button for paneled HTML
// 
// 17    1/22/99 11:23a Johno
// Disable page link control for paneled HTML documents
// (they don't really have pages)
// 
// 16    9/28/98 12:07p Johno
// ARTSTORE define to compile version without WININET
// 
// 15    9/28/98 10:54a Johno
// Use pmwinet.h in place of afxinet.h
// 
// 14    7/11/98 2:46p Jayn
// Changed "link to page" control.
// 
// 13    7/09/98 2:58p Johno
// Manual DDX for m_EditNumValue to avoid unwanted empty string warning.
// 
// 12    7/01/98 4:21p Johno
// Ensure hyperlink enabled in GetData().
// 
// 11    5/14/98 12:36p Johno
// Changed SetHotProperties() to to PMGPageObject::MakeInvisible(void)
// 
// 10    4/28/98 5:05p Johno
// Changes reflecting resource changes (tab order)
// 
// 9     4/21/98 2:39p Johno
// Fixed page bug in GetPageRecordNumber()
// 
// 8     4/16/98 6:20p Johno
// Yet more behavior / visibility of buttons.
// 
// 7     4/16/98 5:41p Johno
// Fixed URL problem, more behavior / visibility of buttons.
// 
// 6     4/16/98 4:26p Johno
// Changed behavior / visibility of buttons.
// Added validation of URLs.
// 
// 5     4/06/98 11:44a Johno
// Dialog now selects only "First Page" or "Specific Page" when editing
// existing hyperlink.
// 
// 4     4/03/98 4:35p Johno
// Changes for page links
// 
// 3     3/26/98 2:41p Johno
// Remove button only enabled with a valid hyperlink
// 
// 2     3/25/98 6:29p Johno
// DOS 8.3 name
// 
// 1     3/25/98 6:06p Johno
// 
// 6     3/16/98 9:33a Johno
// Validate file name if link is that type.
// 
// 5     3/13/98 10:35a Johno
// Added browse button handler
// 
// 4     3/11/98 5:50p Johno
// Use HyperLinkData struct, implement page number UI, etc.
// 
// 3     3/06/98 5:46p Johno
// Fixed OnOK, etc
// 
// 2     3/05/98 6:27p Johno
// Many changes, per design spec
// 
// HyplnkDlg.cpp : implementation file
//
// For input of "hot spot" properties, and a function for setting
// a RectangleObject - as - hotspot's properties from member variables

#include "stdafx.h"
#include "pmw.h"
#include "HyplkDlg.h"
#include "pmgobj.h"
#include "rectobj.h"
#include "util.h"

#include "pmwdoc.h"  // For GetPageRecordNumber and SetPageNumber
#include "pmwinet.h" // For AfxParseURL()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  LINK_TYPE_URL     0
#define  LINK_TYPE_LOCAL   2
#define  LINK_TYPE_MAIL    1
#define  LINK_TYPE_FILE    3

#define  PAGE_TYPE_FIRST   0
#define  PAGE_TYPE_PREV    1
#define  PAGE_TYPE_NEXT    2
#define  PAGE_TYPE_SPEC    3
/////////////////////////////////////////////////////////////////////////////
// CHyperlinkDialog dialog
// This order must match control tab order
int CHyperlinkDialog::HyperTypeTable[] =
{
   TYPE_URL,
   TYPE_EmailAddress,
   TYPE_PageURL,
	TYPE_FilePath,
};

CHyperlinkDialog::CHyperlinkDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CHyperlinkDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHyperlinkDialog)
	m_LinkType = LINK_TYPE_URL;
	m_PageType = PAGE_TYPE_FIRST;
	m_EditNumValue = 1;
	m_EditTemp = _T("");
	//}}AFX_DATA_INIT

   m_HData.FilePathOrURL = _T("");
   m_pDoc = NULL;
   m_AllowRemove = FALSE;
   mCurrentPageIndex = -1;
}

void CHyperlinkDialog::DoDataExchange(CDataExchange* pDX)
{
   CPmwDialog::DoDataExchange(pDX);

   if(!pDX->m_bSaveAndValidate)
      m_EditTemp = m_HData.FilePathOrURL;

	//{{AFX_DATA_MAP(CHyperlinkDialog)
	DDX_Control(pDX, IDC_HYPER_REMOVE, m_Remove);
	DDX_Control(pDX, IDC_HYPER_BROWSE, m_Browse);
	DDX_Control(pDX, IDC_HYPERLINK_STATIC, m_Static);
	DDX_Control(pDX, IDC_HYPER_EDIT_NUMBER, m_EditNumControl);
	DDX_Control(pDX, IDC_HYPER_EDIT_PATH, m_EditPathControl);
	DDX_Control(pDX, IDC_HYPER_PAGE_1, m_PageControl1);
	DDX_Control(pDX, IDC_HYPER_PAGE_PREV, m_PageControl2);
	DDX_Control(pDX, IDC_HYPER_PAGE_NEXT, m_PageControl3);
	DDX_Control(pDX, IDC_HYPER_PAGE_SPEC, m_PageControl4);
	DDX_Control(pDX, IDC_HYPER_LOCALDOC, m_LocalDoc);
	DDX_Control(pDX, IDC_HYPER_LOCALDOC, m_LocalDoc);
	DDX_Radio(pDX, IDC_HYPER_NETDOC, m_LinkType);
	DDX_Radio(pDX, IDC_HYPER_PAGE_1, m_PageType);
	DDX_Text(pDX, IDC_HYPER_EDIT_PATH, m_EditTemp);
	//}}AFX_DATA_MAP

   if(pDX->m_bSaveAndValidate)
   {
      m_HData.StringType = ((m_LinkType >= 0) && (m_LinkType <= 4)) ? HyperTypeTable[m_LinkType] : HyperTypeTable[0];
      m_HData.FilePathOrURL = m_EditTemp;
		
		CString	Temp;
		m_EditNumControl.GetWindowText(Temp);
		Temp.TrimLeft();
		m_EditNumValue = atoi(Temp);
   }
	else
	{
		if (m_EditNumValue != 0)
			DDX_Text(pDX, IDC_HYPER_EDIT_NUMBER, m_EditNumValue);
	}	
}


BEGIN_MESSAGE_MAP(CHyperlinkDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CHyperlinkDialog)
	ON_BN_CLICKED(IDC_HYPER_NETDOC, OnHyperNetdoc)
	ON_BN_CLICKED(IDC_HYPER_LOCALFILE, OnHyperLocalfile)
	ON_BN_CLICKED(IDC_HYPER_LOCALDOC, OnHyperLocaldoc)
	ON_BN_CLICKED(IDC_HYPER_EMAIL, OnHyperEmail)
	ON_BN_CLICKED(IDC_HYPER_PAGE_1, OnHyperPage1)
	ON_BN_CLICKED(IDC_HYPER_PAGE_NEXT, OnHyperPageNext)
	ON_BN_CLICKED(IDC_HYPER_PAGE_PREV, OnHyperPagePrev)
	ON_BN_CLICKED(IDC_HYPER_PAGE_SPEC, OnHyperPageSpec)
	ON_BN_CLICKED(IDC_HYPER_REMOVE, OnHyperRemove)
	ON_BN_CLICKED(IDC_HYPER_BROWSE, OnHyperBrowse)
	ON_EN_CHANGE(IDC_HYPER_EDIT_NUMBER, OnChangeHyperEditNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void
CHyperlinkDialog::GetData(HyperlinkData* p)
{
   p->Copy(m_HData);
	p->Enable();
}

void
CHyperlinkDialog::SetData(HyperlinkData* p, CPmwDoc* pDoc)
{
   m_pDoc = pDoc;
   
   if (m_pDoc != NULL)
      mCurrentPageIndex = m_pDoc->CurrentPageIndex();
   else
      mCurrentPageIndex = -1;

   m_HData.Copy(*p);
   SetPageNumber(m_HData.PageRecordNumber);
   // Set the link type radios
   int i;
   for (i = 0; ; i ++)
   {
      if (i == 4) // I give up
      {
         m_LinkType = LINK_TYPE_URL;
         break;
      }
      if (HyperTypeTable[i] == m_HData.StringType)
      {
         m_LinkType = i;
         m_AllowRemove = TRUE;
         break;
      }
   }

   if ((m_LinkType == LINK_TYPE_URL) && (m_EditNumValue > 0))
      m_PageType = PAGE_TYPE_SPEC;   
   else
      m_PageType = PAGE_TYPE_FIRST;   
}
/* Changed to PMGPageObject::MakeInvisible(void)
void
CHyperlinkDialog::SetHotProperties(RectangleObject* rp)
{
   ASSERT(rp != NULL);
   if (rp != NULL)
   {
      CObjectProperties Props;

      Props.SetFillForegroundColor(TRANSPARENT_COLOR, FALSE);
      Props.SetFillGradientBlendColor(TRANSPARENT_COLOR, FALSE);
      Props.SetFillType(FillFormatV2::FillNone);
      Props.SetOutlineForegroundColor(TRANSPARENT_COLOR, FALSE);

      rp->SetObjectProperties(Props);
   }
}
*/
void
CHyperlinkDialog::UpdateControlVisibility(void)
{
   UpdateData(TRUE);
   // Set the over - lapping dialog element visibility
   // according to the type selected
   BOOL     EnableFirst, EnablePrev, EnableNext, EnableSpec;
   int      ShowPage, ShowPath, ShowBrowse;
   CString  Static;
   
   ShowPage = ShowPath = ShowBrowse = SW_HIDE;

   TRY
   {
      switch(m_LinkType)
      {
         case LINK_TYPE_URL:  // URL
         ShowPath = SW_SHOW;
         Static.LoadString(IDS_WEB_LINK_NETDOC);
         break;

         case LINK_TYPE_LOCAL:  // Local link
         ShowPage = SW_SHOW;
         EnableFirst = EnablePrev = EnableNext = EnableSpec = FALSE;   
         DWORD np;
			EnableSpec = TRUE;		// Always (JN)
			EnableFirst = TRUE;		// Always (JN)
         if ((np = GetNumberOfPages()) > 1)
         {
            if (mCurrentPageIndex > 0)
               EnablePrev = TRUE;   

            if (mCurrentPageIndex + 1 < np)
               EnableNext = TRUE;   
         }
         Static.LoadString(IDS_WEB_LINK_LOCALDOC);
         break;

         case LINK_TYPE_MAIL:  // Email
         ShowPath = SW_SHOW;
         Static.LoadString(IDS_WEB_LINK_MAIL);
         break;

         case LINK_TYPE_FILE:  // File
         ShowPath = SW_SHOW;
         ShowBrowse = SW_SHOW;
         Static.LoadString(IDS_WEB_LINK_LOCALFILE);
         break;
      }
   }
   END_TRY
   
   m_PageControl1.ShowWindow(ShowPage);
   m_PageControl2.ShowWindow(ShowPage);
   m_PageControl3.ShowWindow(ShowPage);
   m_PageControl4.ShowWindow(ShowPage);
   m_EditNumControl.ShowWindow(ShowPage);
   if (ShowPage == SW_SHOW)
   {
      m_PageControl1.EnableWindow(EnableFirst);
      m_PageControl2.EnableWindow(EnablePrev);
      m_PageControl3.EnableWindow(EnableNext);
      m_PageControl4.EnableWindow(EnableSpec);
      m_EditNumControl.EnableWindow(EnableSpec);

      int CheckSet;
      CheckSet = EnableSpec == TRUE ? 1 : 0;
      m_PageControl4.SetCheck(CheckSet);
      CheckSet = CheckSet = 1 ? 0 : EnableNext == TRUE ? 1 : 0;
      m_PageControl3.SetCheck(CheckSet);
      CheckSet = CheckSet = 1 ? 0 : EnablePrev == TRUE ? 1 : 0;
      m_PageControl2.SetCheck(CheckSet);
      CheckSet = CheckSet = 1 ? 0 : EnableFirst == TRUE ? 1 : 0;
      m_PageControl1.SetCheck(CheckSet);
   }
   m_EditPathControl.ShowWindow(ShowPath);
   m_Browse.ShowWindow(ShowBrowse);
   m_Static.SetWindowText(Static);
}

DocumentRecord*
CHyperlinkDialog::GetDocumentRecord(void)
{
   DocumentRecord *pDocRec = NULL;
   
   if (m_pDoc != NULL)
   {
      pDocRec = m_pDoc->DocumentRecord();
   }
   ASSERT(pDocRec != NULL);

   return pDocRec;
}

DB_RECORD_NUMBER
CHyperlinkDialog::GetPageRecordNumber(int PageIndex)
{
   DB_RECORD_NUMBER  ret = 0;

   DocumentRecord *pDocRec = GetDocumentRecord();
   if (pDocRec != NULL)
   {
      if ((PageIndex >= 0) && (PageIndex < (int)GetNumberOfPages()))
         ret = pDocRec->GetPage(PageIndex);
   }
   
   return ret;
}

DWORD
CHyperlinkDialog::GetNumberOfPages(void)
{
   DWORD np = 0;

   DocumentRecord *pDocRec = GetDocumentRecord();
   if (pDocRec != NULL)
      np = pDocRec->NumberOfPages();

   return np;
}

void
CHyperlinkDialog::SetPageNumber(DB_RECORD_NUMBER rn)
{
   m_EditNumValue = 1;
   
   if (m_pDoc == NULL)
      return;   
   
   DocumentRecord *pDocRec;

   if ((pDocRec = m_pDoc->DocumentRecord()) == NULL)
      return;   

   int   PageIndex;
   
   if ((PageIndex = pDocRec->IndexOfPage(rn)) < 0)
      return;

   m_EditNumValue = PageIndex + 1;         
}

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkDialog message handlers

void CHyperlinkDialog::OnHyperNetdoc() 
{
   UpdateControlVisibility();
}

void CHyperlinkDialog::OnHyperLocalfile() 
{
   UpdateControlVisibility();
}

void CHyperlinkDialog::OnHyperLocaldoc() 
{
   UpdateControlVisibility();
}

void CHyperlinkDialog::OnHyperEmail() 
{
   UpdateControlVisibility();
}

void CHyperlinkDialog::OnHyperPage1() 
{
   UpdateData(TRUE);
   m_EditNumValue = 1;
   UpdateData(FALSE);
}

void CHyperlinkDialog::OnHyperPageNext() 
{
   UpdateData(TRUE);  
   m_EditNumValue = mCurrentPageIndex + 2; 
   UpdateData(FALSE);
}

void CHyperlinkDialog::OnHyperPagePrev() 
{
   UpdateData(TRUE);
   m_EditNumValue = mCurrentPageIndex; 
   UpdateData(FALSE);
}

void CHyperlinkDialog::OnHyperPageSpec() 
{

}

void CHyperlinkDialog::OnChangeHyperEditNumber() 
{
   UpdateData(TRUE);
   m_PageType = PAGE_TYPE_SPEC;
   UpdateData(FALSE);
}

BOOL CHyperlinkDialog::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();
	
   UpdateControlVisibility();
	
   m_Remove.EnableWindow(m_AllowRemove);

	if ((m_pDoc == NULL) || (m_pDoc->NormalHTML() == FALSE))
	{
		m_LocalDoc.EnableWindow(FALSE);
	}

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHyperlinkDialog::OnOK() 
{
#ifndef ARTSTORE
   UpdateData(TRUE);
   // Validate input here
   CString Label;
	if (m_LinkType != LINK_TYPE_LOCAL)
   {
      if(m_HData.FilePathOrURL.IsEmpty())
         m_Static.GetWindowText(Label);
      else
      if ((m_LinkType == LINK_TYPE_FILE) && (Util::FileExists(m_HData.FilePathOrURL) == FALSE))
      {
         m_Static.GetWindowText(Label);
      }
      else
      if ((m_LinkType == LINK_TYPE_URL) || (m_LinkType == LINK_TYPE_MAIL))
      {
         DWORD          dw;
         INTERNET_PORT  Port;
         CString        Server, Object;
         DocumentRecord *pDocRec = GetDocumentRecord();

         if (pDocRec != NULL)
         {
            CString  URL;
            pDocRec->AppendFullURLString(m_HData, URL);
            BOOL b = AfxParseURL(URL, dw, Server, Object, Port);
            // If the user forgot "http://"
            if ((b == FALSE) && (dw == AFX_INET_SERVICE_UNK) && (m_LinkType != LINK_TYPE_MAIL))
            {
               CString Temp = "http://";
               Temp += URL;
               b = AfxParseURL(Temp, dw, Server, Object, Port);
               if ((b != FALSE) && (dw == AFX_INET_SERVICE_HTTP))
               {
                  m_HData.FilePathOrURL = Temp;
                  UpdateData(FALSE);   // Reset this; CPmwDialog::OnOK() will do UpdateData(TRUE)
               }
            }
            
            if ((b == FALSE) || (dw == AFX_INET_SERVICE_UNK))
               m_Static.GetWindowText(Label);
         }
      }
   }
   else
   {
      int   PageIdx;

      switch (m_PageType)
      {
         case 0:
         PageIdx = 0;
         break;
         
         case 1:
         PageIdx = mCurrentPageIndex - 1;
         break;
         
         case 2:
         PageIdx = mCurrentPageIndex + 1;
         break;
         
         case 3:
         PageIdx = m_EditNumValue - 1;
         break;
      }

      m_HData.PageRecordNumber = GetPageRecordNumber(PageIdx);
      if (m_HData.PageRecordNumber == 0)
         m_PageControl4.GetWindowText(Label);
   }

   if (!Label.IsEmpty())
   {
      CString Format, Complain;
      Format.LoadString(IDS_HLINK_EMPTY_FIELD);
      Complain.Format(Format, Label);
      AfxMessageBox(Complain);
      return;
   }
#endif
   CPmwDialog::OnOK();
}


void CHyperlinkDialog::OnHyperRemove() 
{
   EndDialog(IDC_HYPER_REMOVE);	
}

void CHyperlinkDialog::OnHyperBrowse() 
{
   CFileDialog Dialog(TRUE, "*.*", "", OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST, "All Files (*.*)|*.*||", this);
	//Dialog.m_ofn.lpstrInitialDir = (LPCSTR)csDirectory;
   Dialog.m_ofn.lpstrInitialDir = NULL;
	if (Dialog.DoModal() == IDOK)
	{
      m_HData.FilePathOrURL = Dialog.GetPathName();
      UpdateData(FALSE);
	}
}

