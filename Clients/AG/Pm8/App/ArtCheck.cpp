// ArtCheck.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "ArtCheck.h"
#include "pmdirdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArtCheckUtil dialog


CArtCheckUtil::CArtCheckUtil(BOOL bSrcIsDir, CWnd* pParent /*=NULL*/)
	: CDialog(CArtCheckUtil::IDD, pParent),
	m_bSrcIsDir(bSrcIsDir)
{
	//{{AFX_DATA_INIT(CArtCheckUtil)
	m_csArtSource = _T("");
	m_bCheckArt = FALSE;
	m_bGenerateFileList = FALSE;
	m_csProjSource = _T("");
	//}}AFX_DATA_INIT
}


void CArtCheckUtil::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArtCheckUtil)
	DDX_Text(pDX, IDC_ART_SRC, m_csArtSource);
	DDX_Check(pDX, IDC_CHECK_ART, m_bCheckArt);
	DDX_Check(pDX, IDC_GENERATE_FILELIST, m_bGenerateFileList);
	DDX_Text(pDX, IDC_PROJ_SRC, m_csProjSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArtCheckUtil, CDialog)
	//{{AFX_MSG_MAP(CArtCheckUtil)
	ON_BN_CLICKED(IDC_SELECT_SRC_ART, OnSelectSrcArt)
	ON_BN_CLICKED(IDC_SELECT_SRC_PROJ, OnSelectSrcProj)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArtCheckUtil message handlers

void CArtCheckUtil::OnSelectSrcArt() 
{
	if(m_bSrcIsDir)
	{
		CChooseDirectoryDialog dlg;
		if(dlg.DoModal() == IDOK)
			m_csArtSource = dlg.m_csDirectory;
	}
	else
	{
		CFileDialog dlg(TRUE, ".txt", "*.txt",OFN_OVERWRITEPROMPT, "Text Files (*.txt)|All Files (*.*)|*.*||");
		if(dlg.DoModal() == IDOK)
			m_csArtSource = dlg.GetPathName();
	}
	UpdateData(FALSE);
}

void CArtCheckUtil::OnSelectSrcProj() 
{
	if(m_bSrcIsDir)
	{
		CChooseDirectoryDialog dlg;
		if(dlg.DoModal() == IDOK)
			m_csProjSource = dlg.m_csDirectory;
	}
	else
	{
		CFileDialog dlg(TRUE, ".txt", "*.txt",OFN_OVERWRITEPROMPT, "Text Files (*.txt)|All Files (*.*)|*.*||");
		if(dlg.DoModal() == IDOK)
			m_csProjSource = dlg.GetPathName();
	}
	UpdateData(FALSE);
}

BOOL CArtCheckUtil::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWnd* pWndProj = GetDlgItem(IDC_PROJ_SRC);
	CWnd* pWndArt = GetDlgItem(IDC_ART_SRC);
	CWnd* pWndCheck = GetDlgItem(IDC_CHECK_ART);
	
	if(pWndProj && pWndArt && pWndCheck)
	{
		if(m_bSrcIsDir)
		{
			pWndProj->SetWindowText("Select Project directory");
			pWndArt->SetWindowText("Select Art directory");
			pWndCheck->SetWindowText("Check for existence of art file");
		}
		else
		{
			pWndProj->SetWindowText("Select Project Spreadsheet");
			pWndArt->SetWindowText("Select Art Spreadsheet");		
			pWndCheck->SetWindowText("Check for art spreadsheet entry");		
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
