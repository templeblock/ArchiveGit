// DongleUpgradeDlg.cpp : implementation file
//

#define MESSAGE_UPGRADE "\
In order to upgrade this application, please \r\n\
call the Monaco Systems sales office from \r\n\
9AM to 5PM EST at 978-749-9944.  \r\n\
Our sales representatives will be happy to give \r\n\
you the current pricing and process your order.  \r\n\
Once your order has been processed, the sales \r\n\
rep will give you a purchase number.  \r\n\
Enter the purchase number in the space below, \r\n\
and press the 'Upgrade' button.  \r\n\
Your Monaco Systems Dongle will be activated, \r\n\
so that you may access all of the features of \r\n\
this application.\r\n\r\n\
Thank you!"

#include "stdafx.h"
#include "DongleUpgradeDlg.h"
#include "Message.h"
#include "stdlib.h" // for srand() and rand()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDD_DongleUpgrade_DIALOG			8000
#define IDC_DongleUpgrade_MESSAGE			9000
#define IDC_DongleUpgrade_APPLICATION		9001
#define IDC_DongleUpgrade_VERSION			9002
#define IDC_DongleUpgrade_ORDER_NUMBER		9003
#define IDC_DongleUpgrade_PURCHASE_NUMBER	9004

/////////////////////////////////////////////////////////////////////////////
// CDongleUpgradeDlg dialog

BEGIN_MESSAGE_MAP(CDongleUpgradeDlg, CDialog)
	//{{AFX_MSG_MAP(CDongleUpgradeDlg)
	ON_EN_CHANGE(IDC_DongleUpgrade_ORDER_NUMBER, OnChangeOrderNumber)
	ON_EN_CHANGE(IDC_DongleUpgrade_PURCHASE_NUMBER, OnChangePurchaseNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CDongleUpgradeDlg::CDongleUpgradeDlg(WORD wSerialNumber, WORD wApplication, WORD wVersion, WORD wFeature, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DongleUpgrade_DIALOG, pParent)
{
	m_wSerialNumber = wSerialNumber;
	m_wApplication = wApplication;
	m_wVersion = wVersion;
	m_wFeature = 0;

	//{{AFX_DATA_INIT(CDongleUpgradeDlg)
	m_Application = "";
	m_Version = "";
	m_OrderNumber = "";
	m_PurchaseNumber = "";
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CDongleUpgradeDlg::~CDongleUpgradeDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDongleUpgradeDlg)
	DDX_Control(pDX, IDC_DongleUpgrade_MESSAGE, m_Message);
	DDX_Text(pDX, IDC_DongleUpgrade_APPLICATION, m_Application);
	DDX_Text(pDX, IDC_DongleUpgrade_VERSION, m_Version);
	DDX_Text(pDX, IDC_DongleUpgrade_ORDER_NUMBER, m_OrderNumber);
	DDX_Text(pDX, IDC_DongleUpgrade_PURCHASE_NUMBER, m_PurchaseNumber);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDongleUpgradeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit* pEditPurchase = (CEdit*)GetDlgItem(IDC_DongleUpgrade_PURCHASE_NUMBER);
	CEdit* pEditOrder = (CEdit*)GetDlgItem(IDC_DongleUpgrade_ORDER_NUMBER);

	if (!m_wSerialNumber && !m_wApplication && !m_wVersion && !m_wFeature)
	{ // This class is being called by the Administrators app
		m_Application = "";
		m_Version = "";
		m_PurchaseNumber = "";
		m_OrderNumber = "";
		if (pEditPurchase)
			pEditPurchase->SetReadOnly(true);
		if (pEditOrder)		
			pEditOrder->SetFocus();
	}
	else
	{ // This class is being called by the application
		m_Application = AppNames[m_wApplication];
		m_Version = AppVersions[m_wApplication][m_wVersion];
		m_PurchaseNumber = "";
		GenerateOrderNumber(m_OrderNumber);
		if (pEditOrder)
			pEditOrder->SetReadOnly(true);
		if (pEditPurchase)
			pEditPurchase->SetFocus();
	}

	CRect rect;
	m_Message.GetClientRect(rect);
	m_Message.SetRect(rect);
	m_Message.FmtLines(true);
	m_Message.SetWindowText(MESSAGE_UPGRADE);

	UpdateData(false);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::OnChangeOrderNumber() 
{
	UpdateData(true);

	WORD OrderData[3];
	if (UnscrambleNumber(m_OrderNumber, OrderData))
	{
		m_wSerialNumber	= HIBYTE(OrderData[0]);
		m_wApplication	= LOBYTE(OrderData[0]);
		m_wVersion		= LOBYTE(OrderData[1]);
		m_wFeature		= HIBYTE(OrderData[1]);
		if (ValidateData(OrderData, false/*bPurchaseNumber*/))
		{
			m_Application = AppNames[m_wApplication];
			m_Version = AppVersions[m_wApplication][m_wVersion];
			GeneratePurchaseNumber(OrderData, m_PurchaseNumber);

			UpdateData(false);
			return;
		}
	}

	m_Application = "";
	m_Version = "";
	m_PurchaseNumber = "";

	UpdateData(false);
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::OnChangePurchaseNumber() 
{
	UpdateData(true);
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::OnOK() 
{
	m_PurchaseNumber.MakeUpper();
	if (m_PurchaseNumber == "ANDOVER")
	{
		CDialog::OnOK();
		return;
	}

	WORD PurchaseData[3];
	if (UnscrambleNumber(m_PurchaseNumber, PurchaseData))
	{
		if (ValidateData(PurchaseData, true/*bPurchaseNumber*/))
		{
			CDialog::OnOK();
			return;
		}
	}

	Message("The purchase number is invalid");
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::OnCancel() 
{
	CDialog::OnCancel();
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::GetDay(SYSTEMTIME& SystemTime, int iDayOffset)
{
	CTimeSpan DeltaDays(iDayOffset/*days*/, 0/*hrs*/, 0/*min*/, 0/*sec*/);
	CTime TheDate = CTime::GetCurrentTime() + DeltaDays;

	SystemTime.wYear		 = TheDate.GetYear();
	SystemTime.wMonth		 = TheDate.GetMonth();
	SystemTime.wDay			 = TheDate.GetDay();
	SystemTime.wHour		 = TheDate.GetHour();
	SystemTime.wMinute		 = TheDate.GetMinute();
	SystemTime.wSecond		 = TheDate.GetSecond();
	SystemTime.wDayOfWeek	 = TheDate.GetDayOfWeek();
	SystemTime.wMilliseconds = 0;
}

/////////////////////////////////////////////////////////////////////////////
WORD CDongleUpgradeDlg::MakeDate(bool bPurchaseNumber, int iDayOffset)
{
	SYSTEMTIME SystemTime; 
	GetDay(SystemTime, iDayOffset);

	if (SystemTime.wDay > 31)
		SystemTime.wDay = 0;		// illegal days become 0

	if (SystemTime.wMonth > 12)
		SystemTime.wMonth = 0;		// illegal months become 0

	// Create a 9 bit date field
	WORD wDate = (
		(SystemTime.wDay << 4) |	// day is 5 bits wide
		(SystemTime.wMonth));		// month is 4 bits wide

	// Be sure to use the 9 bit date as a random seed, so that it stays the same all day long
	WORD wSeed = wDate;

	// If this is for a PurchaseNumber, modified the random seed, but keep it the same all day long
	if (bPurchaseNumber)
		wSeed ^= 0x55;

	// Create a random number 7 bits wide
	srand(wSeed);
	WORD wRandom = rand() & 0x7F;

	// Create the final date field, including the random piece
	wDate = (
		(wDate << 7) |				// the 9 bit date
		(wRandom));					// the 7 bit random piece

	return wDate;
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::GenerateOrderNumber(CString& OrderNumber)
{
	WORD OrderData[3];
	OrderData[0] = MAKEWORD((BYTE)m_wApplication, (BYTE)m_wSerialNumber);
	OrderData[1] = MAKEWORD((BYTE)m_wVersion, (BYTE)m_wFeature);
	OrderData[2] = MakeDate(false/*bPurchaseNumber*/, 0/*iDayOffset*/);

	// Scramble with the date as a pseudo-random key
	ScrambleData(OrderData, OrderNumber);
}

/////////////////////////////////////////////////////////////////////////////
bool CDongleUpgradeDlg::GeneratePurchaseNumber(WORD* pOrderData, CString& PurchaseNumber)
{
	if (!pOrderData)
	{
		PurchaseNumber = "";
		return false;
	}

	if (!ValidateData(pOrderData, false/*bPurchaseNumber*/))
	{
		PurchaseNumber = "";
		return false;
	}

	// Rescramble with a modified date as a pseudo-random key
	pOrderData[2] = MakeDate(true/*bPurchaseNumber*/, 0/*iDayOffset*/);
	ScrambleData(pOrderData, PurchaseNumber);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDongleUpgradeDlg::UnscrambleNumber(CString szNumber, WORD* pOrderData)
{
	pOrderData[0] = pOrderData[1] = pOrderData[2] = 0;

	szNumber.MakeUpper();

	int j = 0;
	BYTE ByteData[6];
	for (int i=0; i<szNumber.GetLength(); i++)
	{
		BYTE c = szNumber.GetAt(i);
		bool bIsAlpha = (c >= 'A' && c <= 'F');
		bool bIsDigit = (c >= '0' && c <= '9');
		if (bIsAlpha || bIsDigit)
		{
			if (j < 12)
			{
				if (bIsAlpha) c = (c - 'A' + 10);
				if (bIsDigit) c = (c - '0');
				if (j & 1) // if index is odd...
				{
					ByteData[j/2] |= c;
				}
				else
					ByteData[j/2] = (c << 4);
			}
			j++;
		}
	}

	if (j != 12)
		return false; // Didn't get the right number of hex digits

	pOrderData[0] = MAKEWORD(ByteData[1], ByteData[0]);
	pOrderData[1] = MAKEWORD(ByteData[3], ByteData[2]);
	pOrderData[2] = MAKEWORD(ByteData[5], ByteData[4]);

	// Unscramble the data
	CString Dummy;
	ScrambleData(pOrderData, Dummy);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDongleUpgradeDlg::ValidateData(WORD* pData, bool bPurchaseNumber)
{
	bool bDateValid = 
		(pData[2] == MakeDate(bPurchaseNumber,  0/*today*/))     ||
		(pData[2] == MakeDate(bPurchaseNumber, -1/*yesterday*/)) ||
		(pData[2] == MakeDate(bPurchaseNumber,  1/*tomorrow*/));

	if (!bDateValid)
		return false;

	if (pData[0] != MAKEWORD((BYTE)m_wApplication, (BYTE)m_wSerialNumber) ||
		pData[1] != MAKEWORD((BYTE)m_wVersion, (BYTE)m_wFeature))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDongleUpgradeDlg::ScrambleData(WORD* pOrderData, CString& ScrambleString)
{
	WORD wDate = pOrderData[2];

	int iRandomBit = 6; // use only the random, lower 7 bits of the date to scramble the data
	for (int i=0; i<=2; i++)
	{
		// pOrderData[0] and pOrderData[1] are process fully
		// pOrderData[2] is processed up to the lower 7 random bits
		int iLimit = ((i == 2) ? 7 : 0);
		for (int iDataBit=15; iDataBit>=iLimit; iDataBit--)
		{
			if (wDate & (1<<iRandomBit))
				pOrderData[i] ^= (1<<iDataBit);
			if (--iRandomBit < 0)
				iRandomBit = 6;
		}
	}

	ScrambleString.Format("%04x-%04x-%04x", pOrderData[0], pOrderData[1], pOrderData[2]);
	ScrambleString.MakeUpper();
}
