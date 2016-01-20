#include "pp.h"
#include "ppafx.h"
#include "defaults.h"
#include "ppmodal.h"
#include "dlgjpeg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



CJPEGOptionsDlg::CJPEGOptionsDlg(CWnd* pParent)
	: CPPModalDlg(CJPEGOptionsDlg::IDD, pParent)
{

}

BOOL CJPEGOptionsDlg::PreDoModal(LPVOID lpVoid)
{
	m_CompressionPref = ::GetDefaultInt(JPEGCompressionFactor_Str, 50);
	m_SubsamplingPref = ::GetDefaultInt(JPEGSubsampling_Str, 1);
	m_bAlwaysPromptPref = ::GetDefaultInt(bJPEGPromptUser_Str, 1);
	return CPPModalDlg::PreDoModal();
}

int CJPEGOptionsDlg::DoModal()
{
	return CDialog::DoModal();

}


LPVOID CJPEGOptionsDlg::PostDoModal()
{
	//write preferences to file
	::PutDefaultInt(JPEGCompressionFactor_Str, m_CompressionPref);
	::PutDefaultInt(JPEGSubsampling_Str, m_SubsamplingPref);
	::PutDefaultInt(bJPEGPromptUser_Str, m_bAlwaysPromptPref);

	//write preferences to memory
	JPEGOptions.bJPEGPromptUser = m_bAlwaysPromptPref;
	JPEGOptions.nCompressionFactor = m_CompressionPref;
	JPEGOptions.nSubsampling = m_SubsamplingPref;

	return CPPModalDlg::PostDoModal();
}

void CJPEGOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SUBSAMPLING_COMBO, m_SubsamplingCombo);
	DDX_Control(pDX, IDC_COMPRESSION_SLIDER, m_CompressionSlider);
	DDX_Text(pDX, IDC_COMPRESSION_EDIT, m_CompressionEdit);
	DDX_Check(pDX, IDC_PROMPTFOROPTIONS, m_bAlwaysPromptPref);
}


BEGIN_MESSAGE_MAP(CJPEGOptionsDlg, CDialog)
	ON_EN_CHANGE(IDC_COMPRESSION_EDIT, OnChangeCompressionEdit)
	ON_CBN_SELCHANGE(IDC_SUBSAMPLING_COMBO, OnSelchangeSubsamplingCombo)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_PROMPTFOROPTIONS, OnPromptForOptions)
END_MESSAGE_MAP()



BOOL CJPEGOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	PreDoModal();

	
	m_CompressionEdit = m_CompressionPref;	
	m_SubsamplingCombo.AddString("YUV 4:1:1 (Low Resolution)");
	m_SubsamplingCombo.AddString("YUV 4:2:2 (Medium Resolution)");
	m_SubsamplingCombo.AddString("YUV 4:4:4 (High Resolution)");

	m_CompressionSlider.SetTicFreq(JPG_TICFREQ);
	m_CompressionSlider.SetRange(JPG_MINSLIDERPOS,JPG_MAXSLIDERPOS);
	m_CompressionSlider.SetPageSize(JPG_SLIDERPAGESIZE);
	m_CompressionSlider.SetPos(m_CompressionEdit);
	m_SubsamplingCombo.SetCurSel(m_SubsamplingPref);

	UpdateData(FALSE);


	return(TRUE);
}

void CJPEGOptionsDlg::OnChangeCompressionEdit() 
{
	LPTSTR str = new char[5];

	GetDlgItem(IDC_COMPRESSION_EDIT)->GetWindowText(str, 5);
	if(*str != '\0')
	{
	m_CompressionEdit = atoi((LPCTSTR)str);

	if(m_CompressionEdit < 1)
		{
		m_CompressionEdit = 1;
		UpdateData(FALSE);
		}
	else if(m_CompressionEdit > 100)
		{
		m_CompressionEdit = 100;
		UpdateData(FALSE);	
		}
	}
	m_CompressionSlider.SetPos(m_CompressionEdit);

}

void CJPEGOptionsDlg::OnSelchangeSubsamplingCombo() 
{
	
}

void CJPEGOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(m_CompressionSlider.GetPos())
		m_CompressionEdit = m_CompressionSlider.GetPos();
	else
		m_CompressionEdit = 1;

	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CJPEGOptionsDlg::OnOK()
{
	m_CompressionPref = m_CompressionEdit;
	m_SubsamplingPref = m_SubsamplingCombo.GetCurSel();
	PostDoModal();
	CPPModalDlg::OnOK();
}

void CJPEGOptionsDlg::OnPromptForOptions()
{
	m_bAlwaysPromptPref = !m_bAlwaysPromptPref;
}
