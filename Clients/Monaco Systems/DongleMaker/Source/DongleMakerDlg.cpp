// DongleMakerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AboutDlg.h"
#include "SerialNumber.h"
#include "Dongle.h"
#include "DongleMaker.h"
#include "DongleMakerDlg.h"
#include "DongleUpgradeDlg.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TREE_LEVEL_APP				0x01000000
#define TREE_LEVEL_VERSIONS_HEADER	0x02000000
#define TREE_LEVEL_VERSIONS			0x04000000
#define TREE_LEVEL_FEATURES_HEADER	0x08000000
#define TREE_LEVEL_FEATURES			0x10000000

/////////////////////////////////////////////////////////////////////////////
// CDongleMakerDlg dialog

BEGIN_MESSAGE_MAP(CDongleMakerDlg, CDialog)
	//{{AFX_MSG_MAP(CDongleMakerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_READ, OnReadDongle)
	ON_BN_CLICKED(IDC_PROGRAM, OnProgramDongle)
	ON_BN_CLICKED(IDC_UNLIMITED, OnUnlimited)
	ON_BN_CLICKED(IDC_LIMITED_USES, OnLimitedUses)
	ON_BN_CLICKED(IDC_LIMITED_DAYS, OnLimitedDays)
	ON_EN_CHANGE(IDC_COUNT, OnChangeCount)
	ON_NOTIFY(TVN_SELCHANGED, IDC_APPTREE, OnAppTreeSelChanged)
	ON_NOTIFY(NM_DBLCLK, IDC_APPTREE, OnAppTreeDblClk)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_TEST_DONGLE, OnTestDongle)
	ON_BN_CLICKED(IDC_TEST_SERIALNUMBER, OnTestSerialNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CDongleMakerDlg::CDongleMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDongleMakerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDongleMakerDlg)
	m_serialNumber = _T("");
	m_AppName = _T("");
	m_count = 0;
	m_iSavedCount = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////////////
CDongleMakerDlg::~CDongleMakerDlg()
{
	if (m_pDongle)
	{
		delete m_pDongle;
		m_pDongle = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDongleMakerDlg)
	DDX_Control(pDX, IDC_APPTREE, m_AppTree);
	DDX_Text(pDX, IDC_SN, m_serialNumber);
	DDX_Text(pDX, IDC_APPNAME, m_AppName);
	DDX_Text(pDX, IDC_VERSIONS, m_versions);
	DDX_Text(pDX, IDC_FEATURES, m_features);
	DDX_Control(pDX, IDC_UNLIMITED, m_unlimited);
	DDX_Control(pDX, IDC_LIMITED_USES, m_limited_uses);
	DDX_Control(pDX, IDC_LIMITED_DAYS, m_limited_days);

	// All this checking just so we don't get an assert on a null string entry
	CString szString;
	GetDlgItem(IDC_COUNT)->GetWindowText(szString);
	if (pDX->m_bSaveAndValidate && !szString.GetLength())
		m_count = 0;
	else
		DDX_Text(pDX, IDC_COUNT, m_count);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDongleMakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Create an imagelist and put it on tree
	m_imagelist.Create(IDB_IMAGELIST, 16, 1, CLR_NONE);
	m_AppTree.SetImageList(&m_imagelist, TVSIL_NORMAL);
	//m_appTree.Create(TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP | TVS_HASBUTTONS | TVS_HASLINES);
	m_selected = 0;

	// Create a Dongle object
	m_pDongle = new CDongle();

	// Read the Dongle
	OnReadDongle();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnReadDongle() 
{
	// Check the attached dongle and get the serial number
	unsigned short ssn;
	BOOL hasKey = m_pDongle->GetSerialNumber(&ssn);
	if (!hasKey)
	{
		AfxMessageBox(IDS_NOKEY);
		return;
	}

	m_serialNumber.Format("%d", (int)ssn);
 
	BeginWaitCursor();
	m_AppTree.DeleteAllItems();
	for (int i=0; i<MAX_PROTECTED_APP; i++)
	{
		AppLock* applock = m_pDongle->GetAppLock(i);
		BOOL bOk = m_pDongle->ReadApp(applock);
		AddToTree(applock); 
	}
	m_AppTree.SelectItem(m_AppTree.GetRootItem());
	EndWaitCursor();

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDongleMakerDlg::AddToTree(AppLock* applock) 
{		
	// If not assigned, then don't need to add
	if (!applock || !applock->assigned)
		return false;

	int which = applock->number;

	TV_INSERTSTRUCT tvinsert;
	tvinsert.hParent = NULL;
	tvinsert.hInsertAfter = TVI_LAST;
	tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE | TVIF_PARAM;
	tvinsert.item.hItem = NULL;
	tvinsert.item.state = applock->active;
	tvinsert.item.stateMask = 0xFFFFFFFF;
	tvinsert.item.lParam = TREE_LEVEL_APP | which;
	tvinsert.item.cchTextMax = 30;
	tvinsert.item.iSelectedImage = tvinsert.item.state;
	tvinsert.item.iImage = tvinsert.item.state;

	tvinsert.item.pszText = AppNames[which];
	HTREEITEM hTreeItem = m_AppTree.InsertItem(&tvinsert);

	// Add the Versions bits
	tvinsert.hParent = hTreeItem;
	tvinsert.item.lParam = TREE_LEVEL_VERSIONS_HEADER | which;
	tvinsert.item.pszText = "Versions";
	tvinsert.item.state = 0;
	tvinsert.item.iSelectedImage = 2;
	tvinsert.item.iImage = 2;
	HTREEITEM hTreeSubItem = m_AppTree.InsertItem(&tvinsert);

	for (int i = 0; i < 16; i++)
	{
		tvinsert.hParent = hTreeSubItem;
		tvinsert.item.lParam = TREE_LEVEL_VERSIONS | (i<<16) | which;
		tvinsert.item.pszText = AppVersions[which][i];
		tvinsert.item.state = !!(applock->versions & (1 << i));
		tvinsert.item.iSelectedImage = tvinsert.item.state;
		tvinsert.item.iImage = tvinsert.item.state;
		if (*tvinsert.item.pszText)
			m_AppTree.InsertItem(&tvinsert);
	}

	// Add the Features bits
	tvinsert.hParent = hTreeItem;
	tvinsert.item.lParam = TREE_LEVEL_FEATURES_HEADER | which;
	tvinsert.item.pszText = "Features";
	tvinsert.item.state = 0;
	tvinsert.item.iSelectedImage = 2;
	tvinsert.item.iImage = 2;
	hTreeSubItem = m_AppTree.InsertItem(&tvinsert);

	for (i = 0; i < 16; i++)
	{
		tvinsert.hParent = hTreeSubItem;
		tvinsert.item.lParam = TREE_LEVEL_FEATURES | (i<<16) | which;
		tvinsert.item.pszText = AppFeatures[which][i];
		tvinsert.item.state = !!(applock->features & (1 << i));
		tvinsert.item.iSelectedImage = tvinsert.item.state;
		tvinsert.item.iImage = tvinsert.item.state;
		if (*tvinsert.item.pszText)
			m_AppTree.InsertItem(&tvinsert);
	}

	m_AppTree.SelectItem(hTreeItem);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnProgramDongle() 
{
	// Check key attached and get serial number
	unsigned short ssn;
	BOOL hasKey = m_pDongle->GetSerialNumber(&ssn);
	if (!hasKey)
	{
		AfxMessageBox(IDS_NOKEY);
		return;
	}

	BeginWaitCursor();
	for (int i=0; i<MAX_PROTECTED_APP; i++)
	{
		AppLock* applock = m_pDongle->GetAppLock(i);
		BOOL bOk = m_pDongle->ProgramApp(applock);
		if (!bOk)
			AfxMessageBox(IDS_PROGRAM_ERR);
	}

	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnAppTreeDblClk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pnmtv = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hSelected = m_AppTree.GetSelectedItem();

	// Returning a result of true will prevent normal processing,
	// therefore the tree will not expand and contract when doubleclicked
	if (pResult)
//j		*pResult = true;
		*pResult = false;

	DWORD dwParam;
	DWORD dwState;
	AppLock* applock = GetSelectedApp(hSelected, &dwState, &dwParam);
	if (!applock)
		return;

	if (dwParam & TREE_LEVEL_APP)
	{ // Double click on an app to select/deselect it
		applock->active = !applock->active;
		DWORD algorithm = m_pDongle->GetAlgorithm(applock);
		UpdateData(FALSE);

		CheckTreeItem(hSelected, applock->active);

//j		// Collapse this app
//j		if (!applock->active)
//j			m_AppTree.Expand(hSelected, TVE_COLLAPSE);
//j
//j		// Collapse the entire tree
//j		HTREEITEM hItem = m_AppTree.GetRootItem();
//j		while (hItem)
//j		{ 
//j			m_AppTree.Expand(hItem, TVE_COLLAPSE);
//j			hItem = m_AppTree.GetNextItem()
//j		}
	}
	else
	if (dwParam & TREE_LEVEL_VERSIONS_HEADER)
	{ // Double click on the version header to clear all version bits
		HTREEITEM hChild = m_AppTree.GetChildItem(hSelected);
		while (hChild)
		{
			CheckTreeItem(hChild, false);
			hChild = m_AppTree.GetNextSiblingItem(hChild);
		}
		applock->versions = 0;
		m_versions.Format("0x%04lx", applock->versions);
		m_versions.MakeLower();
		UpdateData(FALSE);
	}
	else
	if (dwParam & TREE_LEVEL_FEATURES_HEADER)
	{ // Double click on the feature header to clear all feature bits
		HTREEITEM hChild = m_AppTree.GetChildItem(hSelected);
		while (hChild)
		{
			CheckTreeItem(hChild, false);
			hChild = m_AppTree.GetNextSiblingItem(hChild);
		}
		applock->features = 0;
		m_features.Format("0x%04lx", applock->features);
		m_features.MakeLower();
		UpdateData(FALSE);
	}
	else
	if (dwParam & TREE_LEVEL_VERSIONS)
	{ // Double click on a version to select/deselect it
		int iBit = (dwParam & 0x00FF0000) >> 16;
		bool bOn = !(applock->versions & (1 << iBit));
		if (!bOn)
			applock->versions &= ~(1 << iBit);
		else
			applock->versions |=  (1 << iBit);

		m_versions.Format("0x%04lx", applock->versions);
		m_versions.MakeLower();
		UpdateData(FALSE);

		CheckTreeItem(hSelected, bOn);
	}
	else
	if (dwParam & TREE_LEVEL_FEATURES)
	{ // Double click on a feature to select/deselect it
		int iBit = (dwParam & 0x00FF0000) >> 16;
		bool bOn = !(applock->features & (1 << iBit));
		if (!bOn)
			applock->features &= ~(1 << iBit);
		else
			applock->features |=  (1 << iBit);

		m_features.Format("0x%04lx", applock->features);
		m_features.MakeLower();
		UpdateData(FALSE);

		CheckTreeItem(hSelected, bOn);
	}

	m_AppTree.Invalidate(false);
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::CheckTreeItem(HTREEITEM hItem, BOOL bOn)
{
	if (!hItem)
		return;

	TV_ITEM item;
	item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE;
	item.hItem = hItem;
	item.state = bOn;
	item.stateMask = 0xFFFFFFFF;
	item.iSelectedImage = item.state;
	item.iImage = item.state;
	m_AppTree.SetItem(&item);
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnAppTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hSelected = pNMTreeView->itemNew.hItem;

	if (pResult)
		*pResult = 0;

	AppLock* applock = GetSelectedApp(hSelected, NULL, NULL);
	if (!applock)
		return;

	if (m_selected != hSelected)
	{
		m_selected = hSelected;
		m_versions.Format("0x%04lx", applock->versions);
		m_versions.MakeLower();
		m_features.Format("0x%04lx", applock->features);
		m_features.MakeLower();
		m_count = applock->count;
		DWORD algorithm = m_pDongle->GetAlgorithm(applock);
		DWORD apassword = m_pDongle->GetPassword(applock);

		SetupLimitCountUI(applock);
		UpdateData(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::SetupLimitCountUI(AppLock* applock)
{
	if (!applock)
		return;

	bool bUnlimited = (applock->count >= 100);
	bool bLimitedUses = !applock->date && !bUnlimited;
	bool bLimitedDays = !!applock->date && !bUnlimited;
	m_unlimited.SetCheck(bUnlimited);
	m_limited_uses.SetCheck(bLimitedUses);
	m_limited_days.SetCheck(bLimitedDays);

	CWnd* pWnd = GetDlgItem(IDC_COUNT);
	if (pWnd)
		pWnd->EnableWindow(!bUnlimited);
}

/////////////////////////////////////////////////////////////////////////////
AppLock* CDongleMakerDlg::GetSelectedApp(HTREEITEM hItem, LPDWORD pdwState, LPDWORD pdwParam)
{
	if (!hItem)
		return NULL;

	TV_ITEM item;
	item.mask = TVIF_HANDLE | TVIF_STATE | TVIF_PARAM;
	item.hItem = hItem;
	item.state = 0;
	item.stateMask = 0xFFFFFFFF;
	item.lParam = 0;
	m_AppTree.GetItem(&item);

	if (pdwState)
		*pdwState = item.state;
	if (pdwParam)
		*pdwParam = item.lParam;

	int i = item.lParam & 0x0000FFFF;
	m_AppName = AppNames[i];
	AppLock* applock = m_pDongle->GetAppLock(i);
	return applock;
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnUnlimited() 
{
	CWnd* pWnd = GetDlgItem(IDC_COUNT);
	if (pWnd)
		pWnd->EnableWindow(false);

	m_iSavedCount = m_count;
	m_count = 9999;
	UpdateData(FALSE);

	AppLock* applock = GetSelectedApp(m_selected, NULL, NULL);
	if (applock)
	{
		applock->count = (unsigned)m_count;
		applock->date = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnLimitedUses() 
{
	CWnd* pWnd = GetDlgItem(IDC_COUNT);
	if (pWnd)
		pWnd->EnableWindow(true);

	if (m_iSavedCount)
	{
		m_count = m_iSavedCount;
		m_iSavedCount = 0;
		UpdateData(FALSE);
	}

	AppLock* applock = GetSelectedApp(m_selected, NULL, NULL);
	if (applock)
	{
		applock->count = (unsigned)m_count;
		applock->date = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnLimitedDays() 
{
	CWnd* pWnd = GetDlgItem(IDC_COUNT);
	if (pWnd)
		pWnd->EnableWindow(true);

	if (m_iSavedCount)
	{
		m_count = m_iSavedCount;
		m_iSavedCount = 0;
		UpdateData(FALSE);
	}

	AppLock* applock = GetSelectedApp(m_selected, NULL, NULL);
	if (applock)
	{
		applock->count = (unsigned)m_count;
		applock->date = 1;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnChangeCount() 
{
	UpdateData(TRUE);

	AppLock* applock = GetSelectedApp(m_selected, NULL, NULL);
	if (applock)
		applock->count = (unsigned)m_count;	
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnTestDongle() 
{
	// For test purposes only
	WORD wVersions = 0;
	WORD wFeatures = 0;
	if (CDongle::IsMonacoPRINT(&wVersions, &wFeatures))
	{
		// Re-Read the dongle in case it was upgraded
		OnReadDongle();
		Message("MonacoPrint will run");
	}
	else
		Message("MonacoPrint will NOT run");
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnTestSerialNumber() 
{
	// For test purposes only
	if (CSerialNumber::IsMonacoPRINT())
		Message("MonacoPrint will run");
	else
		Message("MonacoPrint will NOT run");
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnOrder() 
{
	// Allow the administrator to accept an order number and generate a purchase number
	CDongleUpgradeDlg dlg(0/*wSerialNumber*/, 0/*wApplication*/, 0/*Version*/, 0/*Feature*/);
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
		;
}

/////////////////////////////////////////////////////////////////////////////
void CDongleMakerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CDongleMakerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDongleMakerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
