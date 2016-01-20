#include "stdafx.h"
#include "MainTab.h"
#include "TabControl.h"
#include "MainDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CMainTab, CBaseTab)
BEGIN_MESSAGE_MAP(CMainTab, CBaseTab)
	ON_BN_CLICKED(IDC_MAIN_BORDERS, OnBorders)
	ON_BN_CLICKED(IDC_MAIN_COLOR, OnColor)
	ON_BN_CLICKED(IDC_MAIN_CONTRAST_BRIGHTNESS, OnContrastBrightness)
	ON_BN_CLICKED(IDC_MAIN_CROP, OnCrop)
	ON_BN_CLICKED(IDC_MAIN_EFFECTS, OnEffects)
	ON_BN_CLICKED(IDC_MAIN_FILE, OnFile)
	ON_BN_CLICKED(IDC_MAIN_FLIP_ROTATE, OnFlipRotate)
	ON_BN_CLICKED(IDC_MAIN_RED_EYE, OnRedEye)
	ON_BN_CLICKED(IDC_MAIN_RECOLOR, OnRecolor)
	ON_BN_CLICKED(IDC_MAIN_RESIZE, OnResize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CMainTab::CMainTab(CWnd* pParent/*=NULL*/)
	: CBaseTab(CMainTab::IDD, pParent)
{
}

/////////////////////////////////////////////////////////////////////////////
CMainTab::~CMainTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainTab::OnInitDialog()
{
	// Initialize the resize dialog base class
	CBaseTab::OnInitDialog();

	StandardizeHyperLink(m_Borders);
	StandardizeHyperLink(m_Color);
	StandardizeHyperLink(m_ContrastBrightness);
	StandardizeHyperLink(m_Crop);
	StandardizeHyperLink(m_Effects);
	StandardizeHyperLink(m_File);
	StandardizeHyperLink(m_FlipRotate);
	StandardizeHyperLink(m_RedEye);
	StandardizeHyperLink(m_Recolor);
	StandardizeHyperLink(m_Resize);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::StandardizeHyperLink(CHyperLink& HyperLink, HICON hIcon)
{
	HyperLink.SetFont(_T("Arial"), 16, FW_NORMAL, false/*bItalic*/);
	HyperLink.UnderlineAlways(false);
	HyperLink.IsLink(false);
	HyperLink.SetColors(RGB(0,0,255)/*crLinkUp*/, RGB(0,0,0)/*crLinkHover*/, RGB(100,100,100)/*crLinkDown*/, GetSysColor(COLOR_BTNFACE)/*crBackGround*/);
	if (hIcon)
		HyperLink.SetIcons(hIcon/*Up*/, hIcon/*Hover*/, hIcon/*Down*/, CHyperLink::SI_ICONUP_ON|CHyperLink::SI_ICONHOVER_ON|CHyperLink::SI_ICONDOWN_ON/*uShowIcons*/);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::DoDataExchange(CDataExchange* pDX)
{
	CBaseTab::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_BORDERS, m_Borders);
	DDX_Control(pDX, IDC_MAIN_COLOR, m_Color);
	DDX_Control(pDX, IDC_MAIN_CONTRAST_BRIGHTNESS, m_ContrastBrightness);
	DDX_Control(pDX, IDC_MAIN_CROP, m_Crop);
	DDX_Control(pDX, IDC_MAIN_EFFECTS, m_Effects);
	DDX_Control(pDX, IDC_MAIN_FILE, m_File);
	DDX_Control(pDX, IDC_MAIN_FLIP_ROTATE, m_FlipRotate);
	DDX_Control(pDX, IDC_MAIN_RED_EYE, m_RedEye);
	DDX_Control(pDX, IDC_MAIN_RECOLOR, m_Recolor);
	DDX_Control(pDX, IDC_MAIN_RESIZE, m_Resize);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnBorders()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CBordersTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnColor()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CColorsTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnContrastBrightness()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CContrastBrightnessTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnCrop()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CCropTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnEffects()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CEffectsTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnFile()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CFileTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnFlipRotate()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CLayoutTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnRecolor()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CRecolorTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnRedEye()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CRedEyeTab::IDD);
}

/////////////////////////////////////////////////////////////////////////////
void CMainTab::OnResize()
{
	CTabControl* pParent = (CTabControl*)GetParent();
	if (pParent) pParent->SetCurrentTab(CResizeTab::IDD);
}
