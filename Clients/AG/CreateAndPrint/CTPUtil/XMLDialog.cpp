// XMLDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CTPUtil.h"
#include "XMLDialog.h"
#include ".\xmldialog.h"
#include <shlobj.h>

#define REGKEY_APPROOT				HKEY_CURRENT_USER
#define REGKEY_APP					"Software\\American Greetings\\Create And Print Utility"
#define REGVAL_CTPUTIL_SRC_PATH		"MRUCtpUtilSrcPath"
#define REGVAL_CTPUTIL_DST_PATH		"MRUCtpUtilDstPath"

struct OPENFILENAMEX
{
	OPENFILENAME of;
	void* pvReserved;
	DWORD dwReserved;
	DWORD FlagsEx;
};

// CXMLDialog dialog

IMPLEMENT_DYNAMIC(CXMLDialog, CDialog)

////////////////////////////////////////////////////////////////////////////
CXMLDialog::CXMLDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CXMLDialog::IDD, pParent)
	, m_FileCount(50)
	, m_bValidateXml(TRUE)
	, m_bUnpackFiles(TRUE)
{
}

////////////////////////////////////////////////////////////////////////////
CXMLDialog::~CXMLDialog()
{
}

////////////////////////////////////////////////////////////////////////////
void CXMLDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_SOURCE, m_edtSrc);
	DDX_Control(pDX, IDC_EDT_DEST, m_edtDest);
	DDX_Text(pDX, IDC_EDT_FILE_COUNT, m_FileCount);
	DDV_MinMaxUInt(pDX, m_FileCount, 0, 99999);
	DDX_Check(pDX, IDC_VALIDATE_XML, m_bValidateXml);
	DDX_Control(pDX, IDC_EDT_SCHEMA, m_edtSchPath);
	DDX_Check(pDX, IDC_UNPACK_FILES, m_bUnpackFiles);
}


BEGIN_MESSAGE_MAP(CXMLDialog, CDialog)
	ON_BN_CLICKED(IDC_SRC_BROWSE, OnBnClickedSrcBrowse)
	ON_BN_CLICKED(IDC_DST_BROWSE, OnBnClickedDstBrowse)
	ON_BN_CLICKED(IDC_SCH_BROWSE, OnBnClickedSchBrowse)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_UNPACK_FILES, OnBnClickedUnpackFiles)
END_MESSAGE_MAP()


// CXMLDialog message handlers
////////////////////////////////////////////////////////////////////////////
CString CXMLDialog::GetInitialDir()
{
#define CString ATL::CAtlString
	CString strDir;
	bool bOK = GetSpecialFolderLocation(strDir, CSIDL_COMMON_DESKTOPDIRECTORY);
	if (!bOK)
		bOK = GetSpecialFolderLocation(strDir, CSIDL_DESKTOPDIRECTORY);
#undef CString
	return strDir;
}

////////////////////////////////////////////////////////////////////////////
CString CXMLDialog::GetInitialDir(CString strPath)
{
	CString strTargetDir;
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		char szFilePath[MAX_PATH];
		szFilePath[0] = 0;
		DWORD nChars = MAX_PATH;
		bool bOK = (regkey.QueryStringValue(strPath, szFilePath, &nChars) == ERROR_SUCCESS);
		if (bOK)
		{
			szFilePath[nChars] = 0;
			strTargetDir = szFilePath;
		}
	}
	return strTargetDir;
}

////////////////////////////////////////////////////////////////////////////
void CXMLDialog::OnBnClickedSrcBrowse()
{
	// TODO: Add your control notification handler code here
	//m_strSrcPath = GetInitialDir(REGVAL_CTPUTIL_SRC_PATH);
	//m_strSrcPath = GetInitialDir();
	GetPath(m_strSrcPath, "Select the Root Directory of Projects");
	m_edtSrc.SetWindowText(m_strSrcPath);

	if (!m_strSrcPath.IsEmpty())
	{
		CRegKey regkey;
		if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
			bool bOK = (regkey.SetStringValue(REGVAL_CTPUTIL_SRC_PATH, m_strSrcPath) == ERROR_SUCCESS);
	}
}

////////////////////////////////////////////////////////////////////////////
void CXMLDialog::OnBnClickedDstBrowse()
{
	// TODO: Add your control notification handler code here
	//m_strDstPath = GetInitialDir(REGVAL_CTPUTIL_DST_PATH);
	//m_strDstPath = GetInitialDir();
	GetPath(m_strDstPath, "Select the Target Directory for Xml files");
	if (!m_strDstPath.IsEmpty())
	{
		m_edtDest.SetWindowText(m_strDstPath);

		// Schema Path
		m_strSchPath = m_strDstPath;
		m_edtSchPath.SetWindowText(m_strSchPath);

		CRegKey regkey;
		if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
			bool bOK = (regkey.SetStringValue(REGVAL_CTPUTIL_DST_PATH, m_strDstPath) == ERROR_SUCCESS);
	}
}

////////////////////////////////////////////////////////////////////////////
void CXMLDialog::OnBnClickedSchBrowse()
{
	// TODO: Add your control notification handler code here
	GetPath(m_strSchPath, "Select Directory for Schema files");
	//m_strSchFile = GetInitialDir();
	//GetFileName(m_strSchFile, "Open a Create & Print Schema");
	if (!m_strSchPath.IsEmpty())
		m_edtSchPath.SetWindowText(m_strSchPath);
}

////////////////////////////////////////////////////////////////////////////
void CXMLDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_edtSrc.GetWindowText(m_strSrcPath);
	m_edtDest.GetWindowText(m_strDstPath);
	m_edtSchPath.GetWindowText(m_strSchPath);

	OnOK();
}

////////////////////////////////////////////////////////////////////////////
void CXMLDialog::GetFileName(CString& strFile, CString strTitle)
{
	char szFileName[MAX_PATH];
	szFileName[0] = 0;

	OPENFILENAMEX ofx;
	::ZeroMemory(&ofx, sizeof(ofx));
	ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx.of));
	ofx.of.hwndOwner = NULL;
	ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
	ofx.of.lpstrFilter = "Create & Print Schemas Files (*.xsd)\0*.*xsd\0\0";
	ofx.of.lpstrTitle = strTitle;
	ofx.of.lpstrDefExt = "xsd";
	ofx.of.lpstrInitialDir = strFile;
	ofx.of.lpstrFile = szFileName;
	ofx.of.nMaxFile = MAX_PATH;

	if (!::GetOpenFileName(&ofx.of))
		return;

	strFile = szFileName;
}

////////////////////////////////////////////////////////////////////////////
void CXMLDialog::GetPath(CString& strPath, CString strTitle)
{
	LPITEMIDLIST lpItemList = NULL;
	LPSHELLFOLDER pDesktopFolder;
	if (!strPath.IsEmpty())
	{
		OLECHAR chOlePath[255];
		ULONG   pchEaten;
		ULONG   dwAttributes;

		HRESULT hr = SHGetDesktopFolder(&pDesktopFolder);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strPath.GetBuffer(MAX_PATH), -1,
	                         chOlePath, MAX_PATH);
		strPath.ReleaseBuffer (-1);

		hr = pDesktopFolder->ParseDisplayName(NULL,
                                            NULL,
                                            chOlePath,
                                            &pchEaten,
                                            &lpItemList,
                                            &dwAttributes);
	}

	BROWSEINFO binfo;
	binfo.hwndOwner = NULL;
	binfo.pidlRoot = lpItemList; //NULL;
	binfo.pszDisplayName = NULL;
	binfo.lpszTitle = (LPCTSTR)strTitle;
	binfo.ulFlags = BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS|BIF_NONEWFOLDERBUTTON;
	binfo.lpfn = NULL;
	binfo.lParam = 0;
	binfo.iImage = 0;

	LPITEMIDLIST pItemID = SHBrowseForFolder(&binfo);	
	if (!pItemID)
		return;

	TCHAR szPath[MAX_PATH];
	*szPath = _T('\0');
	SHGetPathFromIDList(pItemID, szPath);
	strPath = szPath;
}

////////////////////////////////////////////////////////////////////////////
BOOL CXMLDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CXMLDialog::OnBnClickedUnpackFiles()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_bValidateXml = m_bUnpackFiles;

	UpdateData(FALSE);
}
