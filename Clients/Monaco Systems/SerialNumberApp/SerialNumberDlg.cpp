// SerialNumberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SerialNumberApp.h"
#include "SerialNumberDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSerialNumberDlg, CDialog)
	//{{AFX_MSG_MAP(CSerialNumberDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnSaveAsClicked)
	ON_CBN_SELCHANGE(IDC_APPLICATION, OnApplicationSelchange)
	ON_CBN_SELCHANGE(IDC_VENDOR, OnVendorSelchange)
	ON_EN_CHANGE(IDC_PREFIX, OnPrefixChange)
	ON_EN_CHANGE(IDC_HOWMANY, OnHowManyChange)
	ON_BN_CLICKED(IDC_GENERATE, OnGenerateClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CSerialNumberDlg::CSerialNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSerialNumberDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSerialNumberDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSerialNumberDlg)
	DDX_Control(pDX, IDOK, m_buttonSaveAs);
	DDX_Control(pDX, IDCANCEL, m_buttonCancel);
	DDX_Control(pDX, IDC_VENDOR, m_comboVendor);
	DDX_Control(pDX, IDC_PREFIX, m_editPrefix);
	DDX_Control(pDX, IDC_LIST, m_listboxList);
	DDX_Control(pDX, IDC_HOWMANY, m_editHowMany);
	DDX_Control(pDX, IDC_GENERATE, m_buttonGenerate);
	DDX_Control(pDX, IDC_APPLICATION, m_comboApplication);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSerialNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_comboApplication.SetCurSel(0);
	m_comboVendor.SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CSerialNumberDlg::OnPaint() 
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
HCURSOR CSerialNumberDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnSaveAsClicked() 
{
	int iCount = m_listboxList.GetCount();
	if (!iCount)
		return;

 	CFileDialog FileDialog(
		false,		//BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		"txt",		//LPCTSTR lpszDefExt = NULL,
		"Untitled",	//LPCTSTR lpszFileName = NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, //DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Text Files (.txt)|*.txt|All Files|*.*||", //LPCTSTR lpszFilter = NULL,
		this);		//CWnd* pParentWnd = NULL);

	int iResult = FileDialog.DoModal();
	if (iResult == IDCANCEL)
		return;

	CString szFileName = FileDialog.GetPathName();
	CStdioFile File(szFileName, CFile::modeWrite | CFile::modeCreate);

	CString szItem;
	szItem.Format("%d Monaco Systems serial numbers\n\n", iCount);
	File.WriteString(szItem);

	for (int i=0; i<iCount; i++)
	{
		m_listboxList.GetText(i, szItem);
		szItem += "\n";
		File.WriteString(szItem);
	}

	File.Close();
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnApplicationSelchange() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnVendorSelchange() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnPrefixChange() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnHowManyChange() 
{
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::OnGenerateClicked() 
{
	BeginWaitCursor();

	m_listboxList.ResetContent();

	CString szBuffer;
	m_editHowMany.GetWindowText(szBuffer);
	long lCount = atol(szBuffer);
	if (lCount > 10000)
	{
		EndWaitCursor();
		AfxMessageBox("Make sure the count is not greater than 10000");
		return;
	}

	CString szPrefix;
	m_editPrefix.GetWindowText(szPrefix);

	int iPrefixCheckSum = CheckSumString(szPrefix, 99);
	int iApplication = m_comboApplication.GetCurSel();
	int iVendor = m_comboVendor.GetCurSel();

	srand((unsigned)time(NULL));
	for (int i = 0; i < lCount; i++)
	{
		int data1, data2, data3, key1, key2, key3;
		ScrambleTwoDigits(iPrefixCheckSum, data1, key1);
		ScrambleTwoDigits(iApplication, data2, key2);
		ScrambleTwoDigits(iVendor, data3, key3);
		
		CString szItem;
		szItem.Format("%s%02d%02d%02d%02d%02d%02d",
			szPrefix, data1, data2, data3, key1, key2, key3);
		szItem.MakeUpper();

		m_listboxList.AddString(szItem);
	}

	EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
void CSerialNumberDlg::ScrambleTwoDigits(int iTwoDigits, int& iData, int& iKey)
{
	iKey = rand() % 99;

	if (iKey > iTwoDigits)
	{
		iData = iKey - iTwoDigits;
		// iKey is guaranteed to be greater than iData:
		// unscramble as follows: iTwoDigits = iKey - iData
	}
	else
	{
		iData = iTwoDigits - iKey;
		// Swap if iKey is greater than iData:
		// unscramble as follows: iTwoDigits = iKey + iData
		if (iKey > iData)
		{
			int iTemp = iKey;
			iKey = iData;
			iData = iTemp;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
int CSerialNumberDlg::UnscrambleTwoDigits(int iData, int iKey)
{
	if (iKey > iData)
		return iKey - iData;
	else
		return iKey + iData;
}

/////////////////////////////////////////////////////////////////////////////
int CSerialNumberDlg::CheckSumString(CString szString, int iStopValue)
{
	// Add the ascii values in the string
	szString.MakeUpper();
	int iCheckSum = 0;
	int iLocation = szString.GetLength() - 1;
	while (iLocation >= 0)
	{
		TCHAR c = szString.GetAt(iLocation--);
		iCheckSum += c;
	}

	return CheckSum(iCheckSum, iStopValue);
}

/////////////////////////////////////////////////////////////////////////////
int CSerialNumberDlg::CheckSum(int iValue, int iStopValue)
{
	if (iValue <= iStopValue)
		return iValue;

	int iCheckSum = 0;
	while (iValue > 0)
	{
		iCheckSum += (iValue % 10);
		iValue /= 10;
	}

	return CheckSum(iCheckSum, iStopValue);
}
