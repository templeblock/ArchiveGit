//
// $Workfile: WebDlg.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/WebDlg.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 16    1/21/99 6:13p Johno
// Changes for links, hotspots, and text links!
// 
// 15    1/20/99 5:03p Johno
// Changes for paneled HTML
// 
// 14    10/13/98 5:09p Dennis
// Moved web publishing options into properties dialog
// 
// 13    7/28/98 9:41a Jayn
// 
// 12    7/24/98 9:35p Johno
// Fixed browser terminate focus problem, "Apply" / "Apply to all Pages"
// properties update problem. Oh, It's 10 PM - do a diff if you really
// want to know.
// 
// 11    7/15/98 12:09p Dennis
// Added online program confirm dialog
// 
// 10    5/13/98 4:30p Johno
// New "center output" option
// 
// 9     4/30/98 5:22p Johno
// Eliminated body text color option
// 
// 8     4/30/98 4:25p Johno
// Changes to apply page properties across all pages
// 
// 7     4/16/98 5:34p Johno
// Fixed URL problem, more behavior / visibility of buttons.
// 
// 6     4/15/98 11:28a Johno
// Shows complete filename as user modifies name / ext
//
    
#include "stdafx.h"
#include "pmw.h"
#include "WebDlg.h"
#include "util.h"
#include "colordef.h"
#include "htmlconv.h"   // For SetHTMLHomeName, etc.
#include "pagerec.h"
#include "pmwcfg.h"
#include "inifile.h"
#include "webdef.h"     // For DesignCheckState

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebDialog dialog


CWebDialog::CWebDialog(CPaperInfo* pPaperInfo, CWnd* pParent /*=NULL*/)
	: CPmwDialogColor(CWebDialog::IDD, pParent, BRAND_NEW_COLOR)
{
	//{{AFX_DATA_INIT(CWebDialog)
	m_dHeight = 0.0;
	m_dWidth = 0.0;
	m_csWidthDesc = _T("");
	//}}AFX_DATA_INIT

   ASSERT(pPaperInfo);
   m_pPaperInfo = pPaperInfo;
}

void CWebDialog::SetNewPageSize(void)
{
   CString     csResource;
   CPaperInfo* pPaperInfo = m_pPaperInfo;

   CFixedPoint cfpPaperSize, cfpOffset = {0,0};
   CFixedRect  cfrMargins;

   cfpPaperSize.x = MakeFixed(m_dWidth);
   cfpPaperSize.y = MakeFixed(m_dHeight);
   cfrMargins.Left = cfrMargins.Right = CFixed(0);
   cfrMargins.Top = cfrMargins.Bottom = CFixed(0);

   pPaperInfo->InitAsSheet(
      cfpPaperSize,           // Paper Size  (CFixedPoint &)
      cfpOffset,              // Offset      (CFixedPoint &)
      cfrMargins,             // Margins     (CFixedRect &)
      PAPER_TYPE_CUT_SHEET);  // Paper Type  
      
   CFixedRect MinimumMargins;
   MinimumMargins.Left = 
      MinimumMargins.Right =
      MinimumMargins.Top =
      MinimumMargins.Bottom = 0;
//      MinimumMargins.Bottom = MakeFixed(1)/8;

   pPaperInfo->MinimumMargins(MinimumMargins);
   pPaperInfo->ComputeMargins();
   csResource.LoadString(IDS_WEB_PROJECT_PAPER_NAME);
   ASSERT(!csResource.IsEmpty());
   pPaperInfo->SetName(csResource);
}

void CWebDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebDialog)
	DDX_Control(pDX, IDC_WEB_PAGE_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_WEB_PAGE_HEIGHT, m_editHeight);
	DDX_Control(pDX, IDC_WEB_PAGE_CUSTOM, m_btnCustomWidth);
	DDX_Control(pDX, IDC_WEB_PAGE_WIDE, m_btnWideWidth);
	DDX_Control(pDX, IDC_WEB_PAGE_STANDARD, m_btnStandardWidth);
	DDX_Text(pDX, IDC_WEB_PAGE_HEIGHT, m_dHeight);
	DDV_MinMaxDouble(pDX, m_dHeight, 1., 72.);
	DDX_Text(pDX, IDC_WEB_PAGE_WIDTH, m_dWidth);
	DDV_MinMaxDouble(pDX, m_dWidth, 1., 72.);
	DDX_Text(pDX, IDC_WEB_PAGE_WIDTH_DESC, m_csWidthDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebDialog, CPmwDialogColor)
	//{{AFX_MSG_MAP(CWebDialog)
	ON_BN_CLICKED(IDC_WEB_PAGE_CUSTOM, OnWebPageCustom)
	ON_BN_CLICKED(IDC_WEB_PAGE_STANDARD, OnWebPageStandard)
	ON_BN_CLICKED(IDC_WEB_PAGE_WIDE, OnWebPageWide)
   ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_FINISH, OnFinish)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_EN_KILLFOCUS(IDC_WEB_PAGE_HEIGHT, OnKillfocusWebPageHeight)
	ON_EN_KILLFOCUS(IDC_WEB_PAGE_WIDTH, OnKillfocusWebPageWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebDialog message handlers

BOOL CWebDialog::OnEraseBkgnd(CDC* pDC)
{
   CWnd        *pCWndPreview;
   CRect       crPreview;

   INHERITED::OnEraseBkgnd(pDC);
   // Draw preview
   pCWndPreview = GetDlgItem(IDC_WEB_PAGE_SETUP_PREVIEW);
   if(pCWndPreview)
   {
      COLORREF colorrefHighlight = GetSysColor(COLOR_BTNHILIGHT);
      COLORREF colorrefShadow = GetSysColor(COLOR_BTNSHADOW);

      pCWndPreview->GetWindowRect(&crPreview);
      ScreenToClient(crPreview);
      crPreview.InflateRect(-16, -16);    // Margins
      crPreview.top += 4;                 // Offset for text

      CFixedPoint       cfpPaperSize;

      cfpPaperSize.x = MakeFixed(m_dWidth);
      cfpPaperSize.y = MakeFixed(m_dHeight);

		CFixed Max = max(cfpPaperSize.x, cfpPaperSize.y);

      int nAdjustX, nAdjustY;
		nAdjustX = (int)MulDivFixed(crPreview.Width(), Max-cfpPaperSize.x, Max);
		nAdjustY = (int)MulDivFixed(crPreview.Height(), Max-cfpPaperSize.y, Max);

      crPreview.InflateRect(-nAdjustX/2, -nAdjustY/2);
      // Draw Shadow
      CRect crShadow = crPreview;

      crShadow.OffsetRect(4, 4);
      CPen *pOldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
      CBrush *pOldBrush = (CBrush *) pDC->SelectStockObject(BLACK_BRUSH);
      pDC->Rectangle(crShadow);          
      pDC->SelectObject(pOldPen);
      pDC->SelectObject(pOldBrush);
      // Draw Border
      pOldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
      pOldBrush = (CBrush *) pDC->SelectStockObject(WHITE_BRUSH);
      pDC->Rectangle(crPreview);          
      pDC->SelectObject(pOldPen);
      pDC->SelectObject(pOldBrush);
   }

   return TRUE;
}

void CWebDialog::UpdatePreview(void)
{
   CWnd *pCWndPreview= GetDlgItem(IDC_WEB_PAGE_SETUP_PREVIEW);
	if (pCWndPreview != NULL)
   {
      CRect crPreview;
      pCWndPreview->GetWindowRect(&crPreview);
      ScreenToClient(crPreview);
		InvalidateRect(crPreview);
   }
}

void CWebDialog::OnWebPageStandard() 
{
   m_editWidth.EnableWindow(FALSE);
   m_dWidth = 6;
   m_dHeight = 10;
   m_csWidthDesc.LoadString(IDS_WEB_PROJECT_STANDARD_DESC);
   ASSERT(!m_csWidthDesc.IsEmpty());
   UpdateData(FALSE);   // Send to Dialog
   UpdatePreview();
}

void CWebDialog::OnWebPageWide() 
{
   m_editWidth.EnableWindow(FALSE);
   m_dWidth = 7.5;
   m_dHeight = 12.5;
   m_csWidthDesc.LoadString(IDS_WEB_PROJECT_WIDE_DESC);
   ASSERT(!m_csWidthDesc.IsEmpty());
   UpdateData(FALSE);   // Send to Dialog
   UpdatePreview();
}

void CWebDialog::OnWebPageCustom() 
{
   UpdateData(TRUE);    // Get Dialog Data
   m_editWidth.EnableWindow(TRUE);
   m_editHeight.EnableWindow(TRUE);
   m_csWidthDesc.LoadString(IDS_WEB_PROJECT_CUSTOM_DESC);
   ASSERT(!m_csWidthDesc.IsEmpty());
   UpdateData(FALSE);   // Send back to Dialog
   UpdatePreview();
}

BOOL CWebDialog::OnInitDialog() 
{
	CPmwDialogColor::OnInitDialog();

	OnWebPageStandard();
   m_btnStandardWidth.SetCheck(1);

   CreateWizzardButtons ();
   EnableWizardButtons (WIZBUT_BACK | WIZBUT_FINISH | WIZBUT_CANCEL | WIZBUT_HELP);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWebDialog::OnOK() 
{
   UpdateData(TRUE);    // Transfer dialog data to members

   SetNewPageSize();
	
	CPmwDialogColor::OnOK();
}

void CWebDialog::OnFinish() 
{
   OnOK ();   
}

void CWebDialog::OnBack() 
{
   EndDialog (IDC_BACK);
}

void CWebDialog::OnKillfocusWebPageHeight() 
{
   UpdateData(TRUE);    // Transfer dialog data to members
   UpdatePreview();
}

void CWebDialog::OnKillfocusWebPageWidth() 
{
   UpdateData(TRUE);    // Transfer dialog data to members
   UpdatePreview();
}

/////////////////////////////////////////////////////////////////////////////
// CWebSitePropertiesDialog dialog

CWebSitePropertiesDialog::CWebSitePropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWebSitePropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebSitePropertiesDialog)
	m_csExtension = _T("");
	m_csFileName = _T("");
	m_bUseDesignChecker = FALSE;
	m_bDisplayWarning = FALSE;
	//}}AFX_DATA_INIT
   m_bUseDesignChecker = DesignCheckStateGet();
	m_bDisplayWarning = WebPublishWarnGet();
}

static void AFXAPI DDV_IsAlpha(CDataExchange* pDX, CString const &csToCheck)
{
	if (pDX->m_bSaveAndValidate)
	{
      BOOL  bIsAlpha = TRUE;

      for(int i=0;i < csToCheck.GetLength() && bIsAlpha;i++)
      {
         if(isalpha(csToCheck[i]) == 0)
            bIsAlpha = FALSE;
      }
      if(!bIsAlpha)
      {
         CString csResource;
         csResource.LoadString(IDS_DDV_NOTALPHA);
         AfxMessageBox(csResource);
         csResource.Empty();   // exception prep
         pDX->Fail();
      }
	}
}

void CWebSitePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebSitePropertiesDialog)
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_NameAndExt);
	DDX_Control(pDX, IDC_WEB_SITE_EXTENSION, m_comboExtension);
	DDX_Control(pDX, IDC_WEB_SITE_FILENAME, m_comboFileName);
	DDX_CBString(pDX, IDC_WEB_SITE_EXTENSION, m_csExtension);
	DDV_MaxChars(pDX, m_csExtension, 4);
	DDX_CBString(pDX, IDC_WEB_SITE_FILENAME, m_csFileName);
	DDV_MaxChars(pDX, m_csFileName, 30);
	DDX_Check(pDX, IDC_DESIGN_CHECK, m_bUseDesignChecker);
	DDX_Check(pDX, IDC_USE_PROMPT, m_bDisplayWarning);
	//}}AFX_DATA_MAP

   // Custom DDV
	DDX_Text(pDX, IDC_WEB_SITE_FILENAME, m_csFileName);
	DDV_IsAlpha(pDX, m_csFileName);
}


BEGIN_MESSAGE_MAP(CWebSitePropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CWebSitePropertiesDialog)
	ON_CBN_SELCHANGE(IDC_WEB_SITE_EXTENSION, OnSelchangeWebSiteExtension)
	ON_CBN_EDITUPDATE(IDC_WEB_SITE_FILENAME, OnEditupdateWebSiteFilename)
	ON_CBN_CLOSEUP(IDC_WEB_SITE_FILENAME, OnCloseupWebSiteFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWebSitePropertiesDialog::OnSelchangeWebSiteExtension() 
{
   OnNameChanged();
}

void CWebSitePropertiesDialog::OnEditupdateWebSiteFilename() 
{
   OnNameChanged();
}

void CWebSitePropertiesDialog::OnCloseupWebSiteFilename() 
{
   OnNameChanged();
}

void CWebSitePropertiesDialog::OnNameChanged() 
{
   UpdateData(TRUE);
   
   int   i;
   if ((i = m_comboFileName.GetCurSel()) != CB_ERR)
      m_comboFileName.GetLBText(i, m_csFileName);
   CString  Temp = m_csFileName;
   Temp += '.';
   Temp += m_csExtension;
   m_NameAndExt.SetWindowText(Temp);	
TRACE1("%s\n", Temp);
//   UpdateData(FALSE);
}

BOOL CWebSitePropertiesDialog::OnInitDialog() 
{
   CString           csHomePage, csFileExt, Temp;
   int               nHomePageSel = 0, nFileExtSel = 0, nSelFound;

	CDialog::OnInitDialog();

   CHTMLDocConverter::GetHTMLFileName(0, Temp);
   Util::SplitPath(Temp, NULL, NULL, &csHomePage, &csFileExt);
   
   if(!csHomePage.IsEmpty())
   {
      nSelFound = m_comboFileName.FindStringExact(-1, csHomePage);
      if(nSelFound >= 0)
         nHomePageSel = nSelFound;
      else
         nHomePageSel = m_comboFileName.AddString(csHomePage);
   }

   Temp.Empty();
   for (int i = 0; i < csFileExt.GetLength(); i ++)
   {
      TCHAR c;
      if ((c = csFileExt[i]) != '.')
         Temp += c;
   }
    
   if(!Temp.IsEmpty())
   {
      nSelFound = m_comboExtension.FindStringExact(-1, Temp);
      if(nSelFound >= 0)
         nFileExtSel = nSelFound;
      // Extension combo should be drop down list type (no edit)
      ASSERT(nSelFound >= 0);
   }

   m_comboFileName.SetCurSel(nHomePageSel);
   m_comboExtension.SetCurSel(nFileExtSel);
	
	OnNameChanged();
   
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWebSitePropertiesDialog::OnOK() 
{
   // Get Dialog Data
   if(!UpdateData(TRUE))   
      return;              // DDV failed!

   DesignCheckStatePut(m_bUseDesignChecker);
	WebPublishWarnPut(m_bDisplayWarning);

	CHTMLDocConverter::SetHTMLHomeName(m_csFileName);
   CHTMLDocConverter::SetHTMLFileExt(m_csExtension);
   CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CWebPageProperties dialog

CWebPageProperties::CWebPageProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CWebPageProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebPageProperties)
	m_AllPages = FALSE;
	m_Center = FALSE;
	//}}AFX_DATA_INIT
   m_pDoc = NULL;
}


void CWebPageProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebPageProperties)
	DDX_Check(pDX, IDC_ALL_PAGES, m_AllPages);
	DDX_Check(pDX, IDC_CENTER, m_Center);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebPageProperties, CDialog)
	//{{AFX_MSG_MAP(CWebPageProperties)
	ON_BN_CLICKED(IDC_WEB_PAGE_BROWSE_TEXTURE, OnWebPageBrowseTexture)
	ON_BN_CLICKED(IDC_WEB_PAGE_NO_TEXTURE, OnWebPageNoTexture)
	ON_BN_CLICKED(IDC_WEB_PAGE_APPLY, OnWebPageApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebPageProperties message handlers

BOOL CWebPageProperties::OnInitDialog() 
{
   CPageProperties      *pPageProp = NULL;

	CDialog::OnInitDialog();
   // Subclass combobox controls
	m_comboColorBackground.SubclassDlgItem(IDC_WEB_PAGE_BKG_COLOR_COMBO, this);
//	m_comboColorBodyText.SubclassDlgItem(IDC_WEB_PAGE_BODY_TEXT_COLOR_COMBO, this);
	m_comboColorNVHLink.SubclassDlgItem(IDC_WEB_PAGE_NVHLINK_COLOR_COMBO, this);
	m_comboColorVHLink.SubclassDlgItem(IDC_WEB_PAGE_VHLINK_COLOR_COMBO, this);
	m_comboColorAHLink.SubclassDlgItem(IDC_WEB_PAGE_ACTIVE_HLINK_COLOR_COMBO, this);

   if(m_pDoc)
      pPageProp = m_pDoc->GetPageProperties();

   if(pPageProp)
   {
      m_comboColorBackground.SetColor(pPageProp->GetBackgroundColor());
      m_dbrecTexture = pPageProp->GetTextureRecNum();
      
      m_comboColorNVHLink.SetColor(pPageProp->GetNonVisitedLinkColor());
//      m_comboColorBodyText.SetColor(pPageProp->GetBodyTextColor());
      m_comboColorVHLink.SetColor(pPageProp->GetVisitedLinkColor());
      m_comboColorAHLink.SetColor(pPageProp->GetActiveLinkColor());
      m_Center = pPageProp->IsCenteredHTML();
      UpdateData(FALSE);   // Send to Dialog
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CWebPageProperties::DoModal(CPmwDoc *pDoc)
{
   m_pDoc = pDoc;
   ASSERT(pDoc);
   if(pDoc == NULL)
      return IDABORT;

   return CDialog::DoModal();
}

void CWebPageProperties::OnOK() 
{
   UpdateDocView();

	CDialog::OnOK();
}

void
CWebPageProperties::GetProperties(CPageProperties *pPageProp)
{
   pPageProp->SetBackgroundColor(m_comboColorBackground.GetColor());
   pPageProp->SetTextureRecNum(m_dbrecTexture);

   pPageProp->SetNonVisitedLinkColor(m_comboColorNVHLink.GetColor());
//   pPageProp->SetBodyTextColor(m_comboColorBodyText.GetColor());
   pPageProp->SetVisitedLinkColor(m_comboColorVHLink.GetColor());
   pPageProp->SetActiveLinkColor(m_comboColorAHLink.GetColor());
   pPageProp->CenteredHTML(m_Center);
}

void CWebPageProperties::UpdateDocView()
{
   CPageProperties      *pPageProp;
   // Send dialog data to data members
   UpdateData(TRUE);

   if(m_pDoc)
   {
      if (m_AllPages)
      {
/*
         DB_RECORD_NUMBER lCurrentPage = m_pDoc->CurrentPageRecord();
         DWORD dwPages = m_pDoc->NumberOfPages();
	      for (DWORD dwPage = 0; dwPage < dwPages; dwPage++)
	      {
            DB_RECORD_NUMBER lPage = m_pDoc->GetPage(dwPage);
		      if (lPage != 0)
		      {
               if (lPage != m_pDoc->CurrentPageRecord())
                  m_pDoc->LoadPage(lPage, TRUE, FALSE);
               pPageProp = m_pDoc->GetPageProperties();
               GetProperties(pPageProp);
               m_pDoc->UpdatePageProperties();
            }
         }
         
         if (lCurrentPage != m_pDoc->CurrentPageRecord())
            m_pDoc->LoadPage(lCurrentPage, TRUE, FALSE);
*/
	      DWORD dwPages = m_pDoc->NumberOfPages();
         DocumentRecord* pDocrec = m_pDoc->DocumentRecord();
         if (pDocrec != NULL)
         {
            for (DWORD dwPage = 0; dwPage < dwPages; dwPage++)
            {
               DB_RECORD_NUMBER  Page = pDocrec->GetPage(dwPage);
               PageRecord  *pPage = (PageRecord*)(m_pDoc->get_database()->get_record(Page, NULL, RECORD_TYPE_Page));
               if (pPage != NULL)
               {
                  ObjectListPtr pObjects = pPage->objects();
                  if (pObjects != NULL)
                  {
                     pPageProp = pObjects->GetPageProperties();
                     GetProperties(pPageProp);
                  }   
                  pPage->release();
               }
            }
         }
      }
      else
      {
         pPageProp = m_pDoc->GetPageProperties();
         GetProperties(pPageProp);
      }
      //m_pDoc->UpdatePageProperties();
		m_pDoc->UpdatePageProperties(pPageProp);
      m_pDoc->UpdateAllViews(NULL);
      m_pDoc->SetModifiedFlag();       // Document is dirty
   }
}

void CWebPageProperties::OnWebPageBrowseTexture() 
{
	int            retval = 0;
	PMGDatabasePtr pPMGDataBase;

   m_dbrecTexture = 0;
   ASSERT(m_pDoc);
   if(m_pDoc == NULL)
      return;

   pPMGDataBase = m_pDoc->get_database();
//	if (m_pDoc->ChoosePicture(NULL, NULL, AfxGetMainWnd()) == IDOK)
	if (m_pDoc->ChoosePicture() == IDOK)
	{
		GRAPHIC_CREATE_STRUCT_PTR p_gcs = m_pDoc->get_gcs();
		m_dbrecTexture = pPMGDataBase->new_graphic_record(p_gcs);
	}
}

void CWebPageProperties::OnWebPageNoTexture() 
{
   m_dbrecTexture = 0;
}

void CWebPageProperties::OnWebPageApply() 
{
   UpdateDocView();
}




/////////////////////////////////////////////////////////////////////////////
// COnlineProgramConfirmStart dialog


COnlineProgramConfirmStart::COnlineProgramConfirmStart(CWnd* pParent /*=NULL*/)
	: CDialog(COnlineProgramConfirmStart::IDD, pParent)
{
	//{{AFX_DATA_INIT(COnlineProgramConfirmStart)
	m_bDontShowMe = FALSE;
	//}}AFX_DATA_INIT
}


void COnlineProgramConfirmStart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineProgramConfirmStart)
	DDX_Check(pDX, IDC_ONLINE_CONFIRM_START_SHOW, m_bDontShowMe);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COnlineProgramConfirmStart, CDialog)
	//{{AFX_MSG_MAP(COnlineProgramConfirmStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnlineProgramConfirmStart message handlers

BOOL COnlineProgramConfirmStart::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   CenterWindow();
   GetConfiguration()->ReplaceDialogText(this);
   CIniFile    IniFile(GET_PMWAPP()->m_pszProfileName);

   BOOL bShowMe = (BOOL)IniFile.GetInteger("Configuration", "ShowOnlineConfirm", 1);
   if(!bShowMe)
      EndDialog(IDOK);
	
   m_bDontShowMe = FALSE;
   UpdateData(FALSE);   // Send Member variables to dialog

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineProgramConfirmStart::OnOK() 
{
   CIniFile    IniFile(GET_PMWAPP()->m_pszProfileName);

   UpdateData(TRUE); // Update Member variables

   IniFile.WriteInteger("Configuration", "ShowOnlineConfirm", !m_bDontShowMe);
	
	CDialog::OnOK();
}

