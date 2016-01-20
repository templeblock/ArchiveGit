// DeviceSetupPage.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "DeviceSetupPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceSetupPage property page

IMPLEMENT_DYNCREATE(CDeviceSetupPage, CPropertyPage)

CDeviceSetupPage::CDeviceSetupPage() : CPropertyPage(CDeviceSetupPage::IDD)
{
	//{{AFX_DATA_INIT(CDeviceSetupPage)
	m_baudRate = -1;
	m_port = -1;
	//}}AFX_DATA_INIT
	m_deviceType = -1;
	for(int i = 0; i < 10; i++)
		m_deviceList[i] = FALSE;	
}

CDeviceSetupPage::~CDeviceSetupPage()
{
}

void CDeviceSetupPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeviceSetupPage)
	DDX_Control(pDX, IDC_DEVICE_COMBO, m_comboDevice);
	DDX_CBIndex(pDX, IDC_BAUTRATE, m_baudRate);
	DDX_CBIndex(pDX, IDC_PORT, m_port);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeviceSetupPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDeviceSetupPage)
	ON_CBN_SELCHANGE(IDC_DEVICE_COMBO, OnSelchangeDeviceCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceSetupPage message handlers

BOOL CDeviceSetupPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	char *devicename[] =
	{
		"Gretag SpectroScan",
		"Techkon TCR",
		"Xrite408",
		"Nothing",
		"Nothing"
	};

	int i, j = 0;
	for(i = 0; i < MAX_NUM_DEVICES; i++)
		if(m_deviceList[i])
		{
			m_comboDevice.InsertString(j, devicename[i]);
			j++;
		}

	// TODO: Add extra initialization here
	UpdateData(FALSE);

	j = 0;
	for(i = 0; i < m_deviceType; i++)
		if(m_deviceList[i]) j++;

	m_comboDevice.SetCurSel(j);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeviceSetupPage::OnSelchangeDeviceCombo() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int selected;
	int sum = -1;
	selected = m_comboDevice.GetCurSel();
	
	for(int i = 0; i < MAX_NUM_DEVICES; i++)
	{
		if(m_deviceList[i])
			sum++;
		if(selected == sum)
		{
			m_deviceType = i;
			return;
		}
	}			
}
