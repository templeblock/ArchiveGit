/***************************************************************************
*  FILE:        PREFONL.CPP                                                *
*  SUMMARY:     Property Page for Online configuration.                    *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
//
// $Log: /PM8/ConnMgr/prefonl.cpp $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 1     10/19/98 11:18a Dennis
// 
// 10    8/29/98 8:23a Dennis
// Messages and saving of online settings only when visiting that dialog
// 
// 9     8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
// 8     8/12/98 1:58p Mwilson
// hide checkboxes for web publishing for AG projects
// 
// 7     7/17/98 5:49p Johno
// Support "Web Publish Warning" checkbox in CPreferencesOnlinePage
// 
// 6     7/14/98 6:34p Jayn
// 
// 5     6/03/98 1:42p Johno
// Added web page design checker control, log
// 
*  1/27/98  DGP      Created                                               *
***************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "prefonl.h"
#include "ConnSet.h"
#include "util.h"
#include "inifile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDV_MinChars(CDataExchange* pDX, CString const &value, int nMinChars)
{
	if (pDX->m_bSaveAndValidate && value.GetLength() < nMinChars)
	{
      CString csMessage, csResource;
      csResource.LoadString(IDS_DDV_MINCHARS);
      ASSERT(!csResource.IsEmpty());
      csMessage.Format(csResource, nMinChars);
      AfxMessageBox(csMessage);
      csResource.Empty();   // exception prep
      csMessage.Empty();   // exception prep
      pDX->Fail();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPreferencesOnlinePage dialog

CPreferencesOnlinePage::CPreferencesOnlinePage()
	: CDialog(CPreferencesOnlinePage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesOnlinePage)
	m_nCurDialup = -1;
	//}}AFX_DATA_INIT

   m_pConnSettings = NULL;
   m_bOwnSettings = FALSE;
}


CPreferencesOnlinePage::~CPreferencesOnlinePage()
{
   DeInit();
}

void CPreferencesOnlinePage::DeInit() 
{
   if(m_bOwnSettings)
   {
      delete m_pConnSettings;
      m_pConnSettings = NULL;
      m_bOwnSettings = FALSE;
   }
}

void CPreferencesOnlinePage::InitSettings(CConnectionSettings * pConnSettings)
{
   m_pConnSettings = pConnSettings;
}

#ifndef IS_PROPERTY_PAGE
int  CPreferencesOnlinePage::DoModal(CConnectionSettings * pConnSettings)
{
   InitSettings(pConnSettings);
   return INHERITED::DoModal();
}
#endif

/*
int CPreferencesOnlinePage::DoModal()
{
   int nRetCode;
   nRetCode = CDialog::DoModal();
   DeInit();
   return nRetCode;
}
*/
void CPreferencesOnlinePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesOnlinePage)
	DDX_Control(pDX, IDC_CONNECTION_MODEM, m_btnModem);
	DDX_Control(pDX, IDC_CONNECTION_DIRECT, m_btnDirect);
	DDX_Control(pDX, IDC_DIALUP_CONNECTIONS, m_comboDialups);
	DDX_Control(pDX, IDC_CONNECTION_MODEM_PROPERTIES, m_btnAdvanced);
	DDX_CBIndex(pDX, IDC_DIALUP_CONNECTIONS, m_nCurDialup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreferencesOnlinePage, CDialog)
	//{{AFX_MSG_MAP(CPreferencesOnlinePage)
	ON_BN_CLICKED(IDC_CONNECTION_MODEM_PROPERTIES, OnConnectionModemProperties)
	ON_BN_CLICKED(IDC_CONNECTION_MODEM, OnConnectionModem)
	ON_BN_CLICKED(IDC_CONNECTION_DIRECT, OnConnectionDirect)
	ON_CBN_SELCHANGE(IDC_DIALUP_CONNECTIONS, OnSelchangeDialupConnections)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesOnlinePage message handlers

void CPreferencesOnlinePage::OnConnectionModem() 
{
   if(m_btnModem.GetCheck())
      {
         m_comboDialups.EnableWindow(TRUE);
         m_btnAdvanced.EnableWindow(TRUE);
      }
}

void CPreferencesOnlinePage::OnConnectionDirect() 
{
   if(m_btnDirect.GetCheck())
      {
         m_comboDialups.EnableWindow(FALSE);
         m_btnAdvanced.EnableWindow(FALSE);
      }
}

void CPreferencesOnlinePage::OnSelchangeDialupConnections() 
{
   BOOL        bEnableAdvanced = FALSE;
   CComboBox   *pDialupsCombo;
   CConnectionSettings::Type  connectType = CConnectionSettings::typeNone;

   UpdateData(TRUE);
   pDialupsCombo = (CComboBox*)  GetDlgItem(IDC_DIALUP_CONNECTIONS);
   connectType = (CConnectionSettings::Type) pDialupsCombo->GetItemData(m_nCurDialup);
   if(connectType != CConnectionSettings::typeModemDialup)
      bEnableAdvanced = TRUE;
   m_btnAdvanced.EnableWindow(bEnableAdvanced);
   m_pConnSettings->SetType(connectType);
   m_pConnSettings->GetSettings();
}

BOOL CPreferencesOnlinePage::OnInitDialog() 
{
   CComboBox   *pDialupsCombo;
   CButton     *pBtnModem, *pBtnDirect;
   CString     csResource;

   m_bSetActiveOnce = FALSE;

	CDialog::OnInitDialog();

   m_bVisitedThisPage = FALSE;

#if 0
	BOOL bAG = GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings;
	if(bAG)
	{
		CWnd* pWndDesign = GetDlgItem(IDC_DESIGN_CHECK);
		if(pWndDesign )
			pWndDesign->ShowWindow(SW_HIDE);

		CWnd* pWndCheck = GetDlgItem(IDC_USE_PROMPT);
		if(pWndCheck )
			pWndCheck->ShowWindow(SW_HIDE);
	}
#endif

   m_bOwnSettings = FALSE;
   DeInit();
   if(m_pConnSettings == NULL)
      {
        m_bOwnSettings = TRUE;
        m_pConnSettings = new CConnectionSettings;
        m_pConnSettings->Update(FALSE, FALSE);
      }

   pBtnModem   = (CButton *)     GetDlgItem(IDC_CONNECTION_MODEM);
   ASSERT(pBtnModem);
   pBtnDirect  = (CButton *)     GetDlgItem(IDC_CONNECTION_DIRECT);
   ASSERT(pBtnDirect);
   if(m_pConnSettings->GetType() != CConnectionSettings::typeDirect)
      {
         if(pBtnModem)
            pBtnModem->SetCheck(1);
         m_comboDialups.EnableWindow(TRUE);
         m_btnAdvanced.EnableWindow(TRUE);
      }
   else
      {
         if(pBtnDirect)
            pBtnDirect->SetCheck(1);
         m_comboDialups.EnableWindow(FALSE);
         m_btnAdvanced.EnableWindow(FALSE);
      }
   pDialupsCombo = (CComboBox*)  GetDlgItem(IDC_DIALUP_CONNECTIONS);
   if(pDialupsCombo)
      {
         CDialupConnections   *pDialups;
         int                  nItemIndex=-1;
         int                  nModemAOLIndex, nCompuServeIndex;
         int                  nModemDialupIndex, nDirectIndex, nModemCustomIndex;
         BOOL                 bEnableAdvanced;

         nModemAOLIndex = nModemDialupIndex = nDirectIndex = nModemCustomIndex = -1;
         if(m_pConnSettings->IsAOL95Installed())
            {
               csResource.LoadString(IDS_AOL);
               ASSERT(!csResource.IsEmpty());
               if(!csResource.IsEmpty())
                  nModemAOLIndex = pDialupsCombo->AddString(csResource);
               ASSERT(nModemAOLIndex >= 0);
               if(nModemAOLIndex >= 0)
                  pDialupsCombo->SetItemData(
                     nModemAOLIndex,
                     (DWORD) CConnectionSettings::typeModemAOL);
            }
         if(m_pConnSettings->IsCompuServeInstalled())
            {
               csResource.LoadString(IDS_COMPUSERVE);
               ASSERT(!csResource.IsEmpty());
               if(!csResource.IsEmpty())
                  nCompuServeIndex = pDialupsCombo->AddString(csResource);
               ASSERT(nCompuServeIndex >= 0);
               if(nCompuServeIndex >= 0)
                  pDialupsCombo->SetItemData(
                     nCompuServeIndex,
                     (DWORD) CConnectionSettings::typeModemCompuServe);
            }
         pDialups = m_pConnSettings->GetDialups();
         ASSERT(pDialups);
         if(pDialups->GetEntryCount())
            {
               csResource.LoadString(IDS_DIALUP);
               ASSERT(!csResource.IsEmpty());
               if(!csResource.IsEmpty())
                  nModemDialupIndex = pDialupsCombo->AddString(csResource);
               ASSERT(nModemDialupIndex >= 0);
               if(nModemDialupIndex >= 0)
                  pDialupsCombo->SetItemData(
                     nModemDialupIndex,
                     (DWORD) CConnectionSettings::typeModemDialup);
            }

         csResource.LoadString(IDS_CUSTOM_CONNECTION);
         ASSERT(!csResource.IsEmpty());
         if(!csResource.IsEmpty())
            nModemCustomIndex = pDialupsCombo->AddString(csResource);
         ASSERT(nModemCustomIndex >= 0);
         if(nModemCustomIndex >= 0)
            pDialupsCombo->SetItemData(
               nModemCustomIndex,
               (DWORD) CConnectionSettings::typeModemCustom);


         bEnableAdvanced = FALSE;
         switch(m_pConnSettings->GetType())
            {
               case CConnectionSettings::typeModemAOL:
                  nItemIndex = nModemAOLIndex;
                  break;
               case CConnectionSettings::typeModemCompuServe:
                  nItemIndex = nCompuServeIndex;
                  break;
               case CConnectionSettings::typeModemDialup:
                  nItemIndex =  nModemDialupIndex;
                  break;
               case CConnectionSettings::typeModemCustom:
                  nItemIndex = nModemCustomIndex;
                  if(nItemIndex >= 0)
                     bEnableAdvanced = TRUE;
                  break;
               case CConnectionSettings::typeDirect:
                  nItemIndex = nDirectIndex;
                  break;
               case CConnectionSettings::typeNone:
               default:
                  nItemIndex = -1;
                  break;
            }
         if(m_pConnSettings->GetType() != CConnectionSettings::typeDirect)
            m_btnAdvanced.EnableWindow(bEnableAdvanced);
         if(nItemIndex < 0)
            nItemIndex = 0;
         m_nCurDialup = nItemIndex;
         UpdateData(FALSE);
         // Simulate SelChange(), Why doesn't UpdateData() do this?
         OnSelchangeDialupConnections();
         m_pConnSettings->GetSettings();
      }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesOnlinePage::OnOK() 
{
   CComboBox   *pDialupsCombo;
   CButton     *pBtnModem;
   int         nCurSel;
   CString     csName;
   CConnectionSettings::Type  connectType = CConnectionSettings::typeNone;

#ifdef IS_PROPERTY_PAGE
   if(!m_bVisitedThisPage)
      return;
#endif

   UpdateData(TRUE); // Update Member variables

   pBtnModem = (CButton *)       GetDlgItem(IDC_CONNECTION_MODEM);
   if(pBtnModem->GetCheck())
      {
         pDialupsCombo = (CComboBox*)  GetDlgItem(IDC_DIALUP_CONNECTIONS);
         nCurSel = pDialupsCombo->GetCurSel();
         ASSERT(nCurSel >= 0);
         if(nCurSel >= 0)
            {
               pDialupsCombo->GetLBText(nCurSel, csName);
               connectType = (CConnectionSettings::Type) pDialupsCombo->GetItemData(nCurSel);
            }
      }
   else
      {
         csName.LoadString(IDS_DIRECT_CONNECTION);
         ASSERT(!csName.IsEmpty());
         connectType = CConnectionSettings::typeDirect;
      }

   if(connectType != CConnectionSettings::typeModemCustom)
      m_pConnSettings->SetName(csName);
   else if(m_comboDialups.IsWindowEnabled())
      {
            CConnectionSettings::Type  connectType = CConnectionSettings::typeNone;
            CComboBox   *pDialupsCombo;
            CString     csFileName;
            BOOL        bValidData;

            pDialupsCombo = (CComboBox*)  GetDlgItem(IDC_DIALUP_CONNECTIONS);
            connectType = (CConnectionSettings::Type) pDialupsCombo->GetItemData(m_nCurDialup);
            csFileName = m_pConnSettings->GetAppFileName();
            if(!Util::FileExists(csFileName))
            {
               CString  csResource;
               csResource.LoadString(IDS_ONLINE_MISSING_APPPATH);
               ASSERT(!csResource.IsEmpty());
               AfxMessageBox(csResource);
               csResource.Empty();
               csFileName.Empty();
               bValidData = FALSE;
            }
            if(!bValidData)
               return;
      }
   m_pConnSettings->SetType(connectType);
   m_pConnSettings->SaveSettings();

	CDialog::OnOK();  // End dialog
}

void CPreferencesOnlinePage::OnConnectionModemProperties() 
{
   int         nRetVal;
   CString     csAppFileName;
   CConnectionSettingsAdvanced	dlgAdvancedSettings;

   nRetVal = dlgAdvancedSettings.DoModal(m_pConnSettings);
   return;
}

#ifdef IS_PROPERTY_PAGE
BOOL CPreferencesOnlinePage::OnSetActive()
   {
      BOOL bSetActive = INHERITED::OnSetActive();

      if(!m_bSetActiveOnce)
         m_bSetActiveOnce = TRUE;
      else if(!m_bVisitedThisPage)
         {
            m_pConnSettings->Update(FALSE, TRUE);
            m_bVisitedThisPage = TRUE;
         }

      return bSetActive;
   }

BOOL CPreferencesOnlinePage::OnKillActive()
   {
      BOOL bValidData = TRUE;
      INHERITED::OnKillActive();

      if(m_bVisitedThisPage)
         {
            CConnectionSettings::Type  connectType = CConnectionSettings::typeNone;
            CComboBox   *pDialupsCombo;
            CString     csFileName;

            pDialupsCombo = (CComboBox*)  GetDlgItem(IDC_DIALUP_CONNECTIONS);
            connectType = (CConnectionSettings::Type) pDialupsCombo->GetItemData(m_nCurDialup);
            if(m_comboDialups.IsWindowEnabled() &&
			      connectType == CConnectionSettings::typeModemCustom)
            {
               csFileName = m_pConnSettings->GetAppFileName();
               if(!Util::FileExists(csFileName))
               {
                  CString  csResource;
				      LoadConfigurationString(IDS_ONLINE_MISSING_APPPATH, csResource);
                  ASSERT(!csResource.IsEmpty());
                  AfxMessageBox(csResource);
                  csResource.Empty();
                  csFileName.Empty();
                  bValidData = FALSE;
               }
            }
         }
      return bValidData;
   }
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectionSettingsAdvanced dialog


CConnectionSettingsAdvanced::CConnectionSettingsAdvanced(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectionSettingsAdvanced::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectionSettingsAdvanced)
	m_dwFirstCheck = 0;
	m_dwTimeOut = 0;
	m_csAppTitle = _T("");
	m_csAppPathLabel = _T("");
	m_bUsesDialup = FALSE;
	m_bRestoreAutodial = FALSE;
	//}}AFX_DATA_INIT

   m_csFileName = "*.exe";
}

void CConnectionSettingsAdvanced::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectionSettingsAdvanced)
	DDX_Control(pDX, IDC_CONNECTION_RESTORE_AUTODIAL, m_btnRestoreAutodial);
	DDX_Control(pDX, IDC_CONNECTION_PROGRAM, m_btnAppPath);
	DDX_Text(pDX, IDC_CONNECTION_FIRSTCHECK, m_dwFirstCheck);
	DDV_MinMaxDWord(pDX, m_dwFirstCheck, 1, 120);
	DDX_Text(pDX, IDC_CONNECTION_TIMEOUT, m_dwTimeOut);
	DDV_MinMaxDWord(pDX, m_dwTimeOut, 1, 9999);
	DDX_Text(pDX, IDC_CONNECTION_APPTITLE, m_csAppTitle);
	DDV_MaxChars(pDX, m_csAppTitle, 30);
	DDX_Text(pDX, IDC_CONNECTION_APPPATH_LABEL, m_csAppPathLabel);
	DDX_Check(pDX, IDC_CONNECTION_USES_DIALUP, m_bUsesDialup);
	DDX_Check(pDX, IDC_CONNECTION_RESTORE_AUTODIAL, m_bRestoreAutodial);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_CONNECTION_APPTITLE, m_csAppTitle);
	DDV_MinChars(pDX, m_csAppTitle, 1);
}

BEGIN_MESSAGE_MAP(CConnectionSettingsAdvanced, CDialog)
	//{{AFX_MSG_MAP(CConnectionSettingsAdvanced)
	ON_BN_CLICKED(IDC_CONNECTION_PROGRAM, OnConnectionProgram)
	ON_BN_CLICKED(IDC_CONNECTION_USES_DIALUP, OnConnectionUsesDialup)
   ON_UPDATE_COMMAND_UI(IDC_CONNECTION_USES_DIALUP, OnUpdateAutodial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionSettingsAdvanced message handlers

void CConnectionSettingsAdvanced::SetAppTitle(LPCSTR szAppTitle)
   {
      m_csAppTitle = szAppTitle;
   }

LPCSTR CConnectionSettingsAdvanced::GetAppTitle() const
   {
      return m_csAppTitle;
   }

void CConnectionSettingsAdvanced::SetAppFileName(LPCSTR szFileName)
   {
      m_csFileName = szFileName;
      m_csAppPathLabel = m_csFileName;
   }

LPCSTR CConnectionSettingsAdvanced::GetAppFileName() const
   {
      return m_csFileName;
   }

int CConnectionSettingsAdvanced::DoModal(CConnectionSettings *pConnSettings)
   {
      int      nRetVal;

      ASSERT(pConnSettings);
      if(pConnSettings == NULL)
         return IDCANCEL;

      m_pConnSettings = pConnSettings;
      SetAppTitle(m_pConnSettings->GetName());
      SetAppFileName(m_pConnSettings->GetAppFileName());
      SetTimeOut(m_pConnSettings->GetTimeOut());
      SetFirstCheck(m_pConnSettings->GetFirstCheck());
      m_bUsesDialup = m_pConnSettings->WatchForDialup();
      m_bRestoreAutodial = m_pConnSettings->RestoreAutodial();

      nRetVal = CDialog::DoModal();

      if(nRetVal == IDOK)
         {
            m_pConnSettings->SetTimeOut(GetTimeOut());
            m_pConnSettings->SetFirstCheck(GetFirstCheck());
            m_pConnSettings->SetName(GetAppTitle());
            m_pConnSettings->SetAppFileName(GetAppFileName());
            m_pConnSettings->SetWatchForDialup(m_bUsesDialup);
            m_pConnSettings->SetRestoreAutodial(m_bRestoreAutodial);
            m_pConnSettings->SaveSettings();
         }
      return nRetVal;
   }

BOOL CConnectionSettingsAdvanced::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   if(m_pConnSettings->GetType() == CConnectionSettings::typeModemAOL)
      {
         CEdit    *pEditAppTitle;

         m_btnAppPath.EnableWindow(FALSE);
         pEditAppTitle = (CEdit *)  GetDlgItem(IDC_CONNECTION_APPTITLE);
         ASSERT(pEditAppTitle);
         if(pEditAppTitle)
            pEditAppTitle->EnableWindow(FALSE);
      }
   m_btnRestoreAutodial.EnableWindow(!m_bUsesDialup);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConnectionSettingsAdvanced::OnConnectionProgram() 
{
   CString        csPath, csResource;

   Util::SplitPath(m_csFileName, &csPath,NULL);
   csPath += "*.exe";
   
   CFileDialog    cfDialog(TRUE, "exe", csPath, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXTENSIONDIFFERENT, "Program Files (*.exe)", this);

   csResource.LoadString(IDS_CONNECTION_ADV_SELECTAPP);
   ASSERT(!csResource.IsEmpty());
   cfDialog.m_ofn.lpstrTitle = csResource;
   if(cfDialog.DoModal() == IDOK)
      {
         if( (cfDialog.m_ofn.Flags & OFN_EXTENSIONDIFFERENT ) == 0)
         {
            SetAppFileName(cfDialog.GetPathName());
            UpdateData(FALSE);
         }
         else
         {
            csResource.LoadString(IDS_CONNECTION_ADV_NOTAPP);
            ASSERT(!csResource.IsEmpty());
            AfxMessageBox(csResource);
         }
      }
}


void CConnectionSettingsAdvanced::OnConnectionUsesDialup() 
{
   UpdateData(TRUE);
   m_btnRestoreAutodial.EnableWindow(!m_bUsesDialup);
}

void CConnectionSettingsAdvanced::OnUpdateAutodial(CCmdUI* pCmdUI)
{
   UpdateData(TRUE);
   pCmdUI->Enable(!m_bUsesDialup);
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
   CString     csFileName;
   CIniFile    IniFile;

	CDialog::OnInitDialog();
	
   CenterWindow();

   // TODO-replace with registry functions
   Util::GetWindowsDirectory(csFileName);
   csFileName += "\\ConnMgr.ini";
   IniFile.Name(csFileName);
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
   CIniFile    IniFile;
   CString     csFileName;

   Util::GetWindowsDirectory(csFileName);
   csFileName += "\\ConnMgr.ini";
   IniFile.Name(csFileName);
   UpdateData(TRUE); // Update Member variables

   IniFile.WriteInteger("Configuration", "ShowOnlineConfirm", !m_bDontShowMe);
	
	CDialog::OnOK();
}


