// AddrInfoPage.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "AddrInfoPage.h"
#include "util.h"
#include "bmptiler.h"
#include "rmagcom.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette* pOurPal;  // PrintMaster Palette

#define BKG_IMAGE IDB_GALLERY_PROJ_SIDEBAR


/////////////////////////////////////////////////////////////////////////////
// CAddrInfoPage property page

IMPLEMENT_DYNCREATE(CAddrInfoPage, CPropertyPage)

CAddrInfoPage::CAddrInfoPage() : CPropertyPage(CAddrInfoPage::IDD)
{
	//{{AFX_DATA_INIT(CAddrInfoPage)
	m_nDay = 1;
	m_nMonth = 1;
	m_nYear = 1998;
	m_csReturnAddress = _T("");
	m_csName = _T("");
	m_csTempName = _T("");
	m_csAddress = _T("");
	m_csTempAddress = _T("");
	m_csCity = _T("");
	m_csTempCity = _T("");
	m_csState = _T("");
	m_csZip = _T("");
	m_csTempZip = _T("");
	m_csCountry = _T("");
	m_bInternational = 0;
	//}}AFX_DATA_INIT
}

CAddrInfoPage::~CAddrInfoPage()
{
}

void CAddrInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddrInfoPage)
	DDX_Control(pDX, IDC_STATIC_CHARACTER, m_cImage);
	DDX_Control(pDX, IDC_DATE, m_cDate);
	DDX_Control(pDX, IDC_NAME, m_cName);
	DDX_Control(pDX, IDC_ADDRESS, m_cAddress);
	DDX_Control(pDX, IDC_CITY, m_cCity);
	DDX_Control(pDX, IDC_ZIP, m_cZip);
	DDX_Control(pDX, IDC_COUNTRY, m_cCountry);
	DDX_Control(pDX, IDC_STATIC_STATE, m_cStaticState);
	DDX_Control(pDX, IDC_STATIC_ZIP, m_cStaticCode);
	DDX_Control(pDX, IDC_STATIC_COUNTRY, m_cStaticCountry);
	DDX_Control(pDX, IDC_STATIC_CITY, m_cStaticCity);
	DDX_Text(pDX, IDC_SENDER_ADDRESS, m_csReturnAddress);
	DDX_Radio(pDX, IDC_USA, m_bInternational);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DATE, m_cDate);
	DDX_Date(pDX, IDC_DATE, m_csDate);
	DDV_Date(pDX);

	DDX_Text(pDX, IDC_NAME, m_csName);
	DDV_Empty(pDX, IDC_NAME, m_csName);

	DDX_Text(pDX, IDC_ADDRESS, m_csAddress);
	DDV_Empty(pDX, IDC_ADDRESS, m_csAddress);

	DDX_Text(pDX, IDC_CITY, m_csCity);
	DDV_Empty(pDX, IDC_CITY, m_csCity);

	DDX_Control(pDX, IDC_STATE, m_cState);
	DDX_CBString(pDX, IDC_STATE, m_csState);
	if(!m_bInternational)
		DDV_State(pDX, m_csState);

	DDX_Text(pDX, IDC_ZIP, m_csZip);
	if(!m_bInternational)
		DDV_Zip(pDX, IDC_ZIP);
	
	DDX_Text(pDX, IDC_COUNTRY, m_csCountry);
	if(m_bInternational)
		DDV_Empty(pDX, IDC_COUNTRY, m_csCountry);
}


BEGIN_MESSAGE_MAP(CAddrInfoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAddrInfoPage)
	ON_BN_CLICKED(IDC_INTERNATIONAL, OnInternational)
	ON_BN_CLICKED(IDC_USA, OnUsa)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddrInfoPage message handlers

BOOL CAddrInfoPage::OnEraseBkgnd(CDC* pDC) 
{
	GetParent()->SendMessage(WM_ERASEBKGND, (WPARAM)pDC->GetSafeHdc());
	// title...
	if ((HBITMAP)m_bmpTitle)
	{
		BITMAP BitmapInfo;
		m_bmpTitle.GetBitmap(&BitmapInfo);
		CRect rcTitle(0,0, BitmapInfo.bmWidth, BitmapInfo.bmHeight);
		// get position from static control in dialog
		CWnd* pWnd = GetDlgItem(IDC_TITLE);
		if (pWnd)
		{
			CRect rcClient;
			pWnd->GetClientRect(&rcClient);
			if(rcClient != rcTitle)
			{
				pWnd->SetWindowText("");
				pWnd->SetWindowPos(&wndTop, rcTitle.left, rcTitle.right, 
							rcTitle.Width(), rcTitle.Height(), SWP_NOZORDER); 
			}
		}
		// Use our global PrintMaster palette.
		CPalette* pOldPalette = pDC->SelectPalette(pOurPal, FALSE);
		pDC->RealizePalette();
		Util::TransparentBltBitmap(*pDC, rcTitle, m_bmpTitle, NULL, NULL, 0, 0);
		// Restore the previous DC settings.
		if (pOldPalette)
			pDC->SelectPalette(pOldPalette, FALSE);
	}
	return TRUE;
}

void CAddrInfoPage::DDV_Empty(CDataExchange* pDX, int nIDC, CString csText )
{
	if(pDX->m_bSaveAndValidate )
	{
		if(csText.GetLength() == 0)
		{
			UINT nID;
			switch(nIDC)
			{
				case IDC_NAME:
					nID = IDS_MISSING_DEST_NAME;
					break;
				case IDC_ADDRESS:
					nID = IDS_MISSING_DEST_ADDR;
					break;
				case IDC_CITY:
					nID = IDS_MISSING_DEST_CITY;
					break;
				case IDC_COUNTRY:
					nID = IDS_MISSING_DEST_COUNTRY;
					break;
				default:
					nID = IDS_REQUIRED_FIELD;
					break;


			}

			AfxMessageBox(nID);
			pDX->Fail();	
		}
	}
	
}

void CAddrInfoPage::DDV_State(CDataExchange* pDX, CString csState)
{

	if(pDX->m_bSaveAndValidate )
	{
		int nSel = m_cState.GetCurSel();
		if(nSel == -1)
		{
			csState.MakeUpper();
			nSel = m_cState.FindString(0, csState);
			if(nSel == CB_ERR)
			{
				AfxMessageBox(IDS_INVALID_STATE);
				pDX->Fail();	
			}
		}
	}
}

void CAddrInfoPage::DDX_Date(CDataExchange* pDX, int nIDC, CString& csExpDate)
{
	if(pDX->m_bSaveAndValidate )
	{
		CTimeStamp ts = m_cDate.GetSetTime();
		SYSTEMTIME sysTime;
		FILETIME ft = ts.GetTime();
		FileTimeToSystemTime(&ft, &sysTime);
		csExpDate.Format("%.2d%.2d%.4d", sysTime.wMonth, sysTime.wDay, sysTime.wYear);	
	}
}


void CAddrInfoPage::DDV_Date(CDataExchange* pDX)
{
	if(pDX->m_bSaveAndValidate )
	{
		CTimeStamp* pTime = m_cDate.GetInitialTimeStamp();
		CTimeStamp tmCheck = *pTime;
		CTimeStampSpan tmSpan(365,0,0,0);
		tmCheck += tmSpan;

		CTimeStamp setTime = m_cDate.GetSetTime();

		if(setTime > tmCheck)
		{
			AfxMessageBox(IDS_INVALID_DELIVERY_DATE);
			pDX->Fail();	
		}
	}
}

void CAddrInfoPage::DDV_Zip(CDataExchange* pDX, int nIDC)
{
	if(pDX->m_bSaveAndValidate )
	{
		int nLength = m_csZip.GetLength();
		BOOL bFail = TRUE;
	
		CString csTemp;
		for(int i = 0; i < nLength; i++)
		{
			char c = m_csZip[i];
			if( c >= '0' && c <= '9')
				csTemp += m_csZip[i];
		}
		nLength = csTemp.GetLength();
		if(nLength == 5 || nLength == 9)
			bFail = FALSE;

		if(bFail)
		{
			AfxMessageBox(IDS_INVALID_ZIP);
			pDX->Fail();
		}
	}
}

BOOL CAddrInfoPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	Util::LoadResourceBitmap(m_bmpTitle, MAKEINTRESOURCE(IDB_ADDR_INFO_TITLE), pOurPal);

	//added brackets so the resource loader goes out of scope when done.
	{
		CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());
		CBitmap bmCharacter;
		if(rlProductSpecific.LoadBitmap(bmCharacter, CSharedIDResourceManager::riCharacterLarge, pOurPal))
		{
			m_cImage.SetBitmap(bmCharacter);
			bmCharacter.Detach();
		}
		else
			m_cImage.ShowWindow(SW_HIDE);
	}



	m_cDate.SetWindowText("");
	m_cDate.Init(TRUE);

	char states[51][3] = {	{"AK"}, {"AL"}, {"AR"}, {"AZ"}, {"CA"}, {"CO"}, {"CT"}, {"DC"}, 
							{"DE"}, {"FL"}, {"GA"}, {"HI"}, {"IA"}, {"ID"}, {"IL"}, {"IN"}, 
							{"KS"}, {"KY"}, {"LA"}, {"MA"}, {"MD"}, {"ME"}, {"MI"}, {"MN"}, 
							{"MO"}, {"MS"}, {"MT"}, {"NC"}, {"ND"}, {"NE"}, {"NH"}, {"NM"}, 
							{"NJ"}, {"NV"}, {"NY"}, {"OH"}, {"OK"}, {"OR"}, {"PA"}, {"RI"}, 
							{"SC"}, {"SD"}, {"TN"}, {"TX"}, {"UT"}, {"VA"}, {"VT"}, {"WA"}, 
							{"WI"}, {"WV"}, {"WY"}};
						 						 						  					
	for(int i = 0; i < 51; i++)
	{
		m_cState.AddString(states[i]);
	}
	m_cState.SetCurSel(0);
	m_cCountry.ShowWindow(SW_HIDE);
	m_cStaticCountry.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddrInfoPage::OnInternational() 
{
	//International is already selected.
	if(m_cCountry.IsWindowVisible())
		return;

	CString csTemp;
	m_cName.GetWindowText(csTemp);
	m_cName.SetWindowText(m_csTempName);
	m_csTempName = csTemp;

	m_cAddress.GetWindowText(csTemp);
	m_cAddress.SetWindowText(m_csTempAddress);
	m_csTempAddress = csTemp;
	
	m_cCity.GetWindowText(csTemp);
	m_cCity.SetWindowText(m_csTempCity);
	m_csTempCity = csTemp;
	
	m_cZip.GetWindowText(csTemp);
	m_cZip.SetWindowText(m_csTempZip);
	m_csTempZip = csTemp;
	
	csTemp.LoadString(IDS_PROVINCE);
	m_cStaticCity.SetWindowText(csTemp);

	csTemp.LoadString(IDS_POSTAL_CODE);
	m_cStaticCode.SetWindowText(csTemp);
	CRect rcClient;
	m_cStaticCode.GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);
	InvalidateRect(rcClient);

	m_cCountry.ShowWindow(SW_SHOW);
	m_cStaticCountry.ShowWindow(SW_SHOW);
	m_cState.ShowWindow(SW_HIDE);
	m_cStaticState.ShowWindow(SW_HIDE);

	m_cZip.NumbersOnly(FALSE);
}

void CAddrInfoPage::OnUsa() 
{
	//USA is already selected.
	if(m_cState.IsWindowVisible())
		return;

	CString csTemp;
	m_cName.GetWindowText(csTemp);
	m_cName.SetWindowText(m_csTempName);
	m_csTempName = csTemp;

	m_cAddress.GetWindowText(csTemp);
	m_cAddress.SetWindowText(m_csTempAddress);
	m_csTempAddress = csTemp;
	
	m_cCity.GetWindowText(csTemp);
	m_cCity.SetWindowText(m_csTempCity);
	m_csTempCity = csTemp;
	
	m_cZip.GetWindowText(csTemp);
	m_cZip.SetWindowText(m_csTempZip);
	m_csTempZip = csTemp;

	csTemp.LoadString(IDS_CITY);
	m_cStaticCity.SetWindowText(csTemp);

	csTemp.LoadString(IDS_ZIP_CODE);
	m_cStaticCode.SetWindowText(csTemp);

	m_cCountry.ShowWindow(SW_HIDE);
	m_cStaticCountry.ShowWindow(SW_HIDE);
	m_cState.ShowWindow(SW_SHOW);
	m_cStaticState.ShowWindow(SW_SHOW);

	m_cZip.NumbersOnly(TRUE);
}


BOOL CAddrInfoPage::OnSetActive() 
{
	BOOL bResult = CPropertyPage::OnSetActive();
	if(bResult)
		((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT);	
	
	return bResult;
}

HBRUSH CAddrInfoPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = NULL;
	
	if(nCtlColor == CTLCOLOR_STATIC || 
		(nCtlColor == CTLCOLOR_BTN && pWnd->GetStyle() & BS_RADIOBUTTON))
	{
        // set background color, text mode, and text font
        //
        pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
        pDC->SetBkMode(TRANSPARENT);


        hbr = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

	}
	else
		hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}
