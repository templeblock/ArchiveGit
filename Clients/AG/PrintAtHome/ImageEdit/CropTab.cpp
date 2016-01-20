#include "stdafx.h"
#include "CropTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CCropTab, CBaseTab)
BEGIN_MESSAGE_MAP(CCropTab, CBaseTab)
	ON_BN_CLICKED(IDC_CROP_APPLY, OnCropApply)
	ON_BN_CLICKED(IDC_CROP_PORTRAIT, OnCropPortrait)
	ON_BN_CLICKED(IDC_CROP_LANDSCAPE, OnCropLandscape)
	ON_CBN_SELCHANGE(IDC_CROP_ASPECTRATIO, &CCropTab::OnCbnSelchangeCropAspectratio)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CCropTab::CCropTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CCropTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CCropTab::~CCropTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCropTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	CheckRadioButton(IDC_CROP_PORTRAIT, IDC_CROP_LANDSCAPE, IDC_CROP_PORTRAIT);

	m_comboAspectRatio.AddString(_T("None"));
	m_comboAspectRatio.AddString(_T("3 x 4"));
	m_comboAspectRatio.AddString(_T("3 x 5"));
	m_comboAspectRatio.AddString(_T("4 x 6"));
	m_comboAspectRatio.AddString(_T("5 x 7"));
	m_comboAspectRatio.AddString(_T("8 x 10"));

	int iCurSel = 0;
	m_comboAspectRatio.SetCurSel(iCurSel);
	EnableControl(IDC_CROP_PORTRAIT, iCurSel != 0);
	EnableControl(IDC_CROP_LANDSCAPE, iCurSel != 0);

	EnableControl(IDC_CROP, false);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CROP_ASPECTRATIO, m_comboAspectRatio);
	DDX_Control(pDX, IDC_CROP_LEFT, m_editLeft);
	DDX_Control(pDX, IDC_CROP_TOP, m_editTop);
	DDX_Control(pDX, IDC_CROP_RIGHT, m_editRight);
	DDX_Control(pDX, IDC_CROP_BOTTOM, m_editBottom);
	DDX_Control(pDX, IDC_CROP_ORIGINAL, m_staticOriginalSize);
	DDX_Control(pDX, IDC_CROP_NEW, m_staticNewSize);
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropApply()
{
	if (!DoCommand(_T("Crop"), _T("-1")))
		return;

	EnableControl(IDC_CROP_APPLY, false);
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropPortrait()
{
	CheckRadioButton(IDC_CROP_PORTRAIT, IDC_CROP_LANDSCAPE, IDC_CROP_PORTRAIT);
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCropLandscape()
{
	CheckRadioButton(IDC_CROP_PORTRAIT, IDC_CROP_LANDSCAPE, IDC_CROP_LANDSCAPE);
}

/////////////////////////////////////////////////////////////////////////////
void CCropTab::OnCbnSelchangeCropAspectratio()
{
	int iCurSel = m_comboAspectRatio.GetCurSel();
	EnableControl(IDC_CROP_PORTRAIT, iCurSel != 0);
	EnableControl(IDC_CROP_LANDSCAPE, iCurSel != 0);
}
