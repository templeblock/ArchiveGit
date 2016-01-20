#include "stdafx.h"
#include "BordersTab.h"
#include "MainDlg.h"
#include "Image.h"
#include <atldlgs.h>
#include "Utility.h"

/////////////////////////////////////////////////////////////////////////////
CBordersTab::CBordersTab()
{
	m_strClipArtInfo.Empty();
}

/////////////////////////////////////////////////////////////////////////////
CBordersTab::~CBordersTab()
{
	m_bFadeStretch = true;
	m_iFadeSize = 100;
	m_iFadeSoftness = 50;
}

/////////////////////////////////////////////////////////////////////////////
void CALLBACK CBordersTab::MyArtGalleryCallback(HWND hwnd, UINT uMsgId, LPARAM lParam, LPARAM lArtGalleryPtr, LPCTSTR szValue)
{
	CBordersTab * pBordersTab = (CBordersTab*)lParam;
	if (!pBordersTab)
		return;

	if (WM_ARTGALLERY_DOWNLOADCOMPLETE == uMsgId)
	{
		CString strFileName(szValue);
		if (!strFileName.IsEmpty())
		{
			CArtGalleryDialog* pArtGal = (CArtGalleryDialog*)lArtGalleryPtr;
			pBordersTab->GetMainDlg().SetArtGalleryDialog(pArtGal);
			pBordersTab->LoadBorderImage(strFileName);

		}
	}
	else if (WM_CLOSE == uMsgId)
	{
		pBordersTab->GetMainDlg().SetArtGalleryDialog(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CBordersTab::LoadBorderImage(LPCTSTR szFileName)
{
	CString strName(szFileName);
	if (strName.IsEmpty())
		return false;
	
	m_strBorderImageFileName = strName;
	OnBorderImage(0, 0, 0);

	CArtGalleryDialog* pArtGal = GetMainDlg().GetArtGalleryDialog();
	if (pArtGal)
	{
		m_strClipArtInfo = pArtGal->GetClipArtDownloadInfo();
		pArtGal->ArtAddComplete(_T("success"), false/*bDeleteFile*/); // pass "success" or "error"
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBordersTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	m_FadeSizeSlider.SetRange(0, 100);
	m_FadeSizeSlider.SetTicFreq(10);
	m_FadeSizeSlider.SetPageSize(100/10);

	m_FadeSoftnessSlider.SetRange(0, 100);
	m_FadeSoftnessSlider.SetTicFreq(10);
	m_FadeSoftnessSlider.SetPageSize(100/10);

	m_strBorderImageFileName.Empty();

//j	StandardizeHyperLink(m_btnChoose);

	m_comboFadeType.AddString(_T("Rectangle"));
	m_comboFadeType.AddString(_T("Rounded Rectangle"));
	m_comboFadeType.AddString(_T("Oval"));
	m_comboFadeType.AddString(_T("Heart"));
	m_comboFadeType.AddString(_T("Star1"));
	m_comboFadeType.AddString(_T("Star2"));
	m_comboFadeType.AddString(_T("Star3"));
	m_comboFadeType.AddString(_T("Star4"));
	m_comboFadeType.AddString(_T("Triangle"));
	m_comboFadeType.AddString(_T("Rounded Triangle"));
	m_comboFadeType.AddString(_T("Diamond"));
	m_comboFadeType.AddString(_T("Starburst1"));
	m_comboFadeType.AddString(_T("Starburst2"));
	m_comboFadeType.AddString(_T("Starburst3"));
	m_comboFadeType.AddString(_T("Starburst4"));
	m_comboFadeType.AddString(_T("Egg"));
	m_comboFadeType.AddString(_T("Arch"));
	m_comboFadeType.AddString(_T("Bevel"));
	m_comboFadeType.AddString(_T("Jewel"));
	m_comboFadeType.AddString(_T("Top"));
	m_comboFadeType.AddString(_T("Bottom"));
	m_comboFadeType.AddString(_T("Left"));
	m_comboFadeType.AddString(_T("Right"));
//j	m_comboFadeType.AddString(_T("Circle/Ellipse"));
//j	m_comboFadeType.AddString(_T("Line"));
//j	m_comboFadeType.AddString(_T("Shape1"));
//j	m_comboFadeType.AddString(_T("Shape2"));
//j	m_comboFadeType.AddString(_T("Circle Mask"));
//j	m_comboFadeType.AddString(_T("Circle Frame"));
//j	m_comboFadeType.AddString(_T("Square Frame"));
//j	m_comboFadeType.AddString(_T("House"));
//j	m_comboFadeType.AddString(_T("House2"));
//j	m_comboFadeType.AddString(_T("Rounded Corner"));
//j	m_comboFadeType.AddString(_T("Skull"));
//j	m_comboFadeType.AddString(_T("Cross1"));
//j	m_comboFadeType.AddString(_T("Cross2"));
//j	m_comboFadeType.AddString(_T("Cross3"));
//j	m_comboFadeType.AddString(_T("Cross4"));
//j	m_comboFadeType.AddString(_T("Cross5"));
//j	m_comboFadeType.AddString(_T("Crown"));
//j	m_comboFadeType.AddString(_T("Cup"));
//j	m_comboFadeType.AddString(_T("Flame"));
//j	m_comboFadeType.AddString(_T("G Clef"));
//j	m_comboFadeType.AddString(_T("Quarter Note"));
//j	m_comboFadeType.AddString(_T("Gear"));
//j	m_comboFadeType.AddString(_T("Horseshoe"));
//j	m_comboFadeType.AddString(_T("Hand"));
//j	m_comboFadeType.AddString(_T("Leaf"));
//j	m_comboFadeType.AddString(_T("Phone"));
//j	m_comboFadeType.AddString(_T("Plane"));
//j	m_comboFadeType.AddString(_T("Arrow1"));
//j	m_comboFadeType.AddString(_T("Arrow2"));
//j	m_comboFadeType.AddString(_T("Arrow3"));
//j	m_comboFadeType.AddString(_T("Arrow4"));
//j	m_comboFadeType.AddString(_T("Arrow5"));
//j	m_comboFadeType.AddString(_T("Arrow6"));
//j	m_comboFadeType.AddString(_T("Arrow7"));
//j	m_comboFadeType.AddString(_T("Arrow8"));
//j	m_comboFadeType.AddString(_T("Bowling Pin"));
//j	m_comboFadeType.AddString(_T("Heart"));
//j	m_comboFadeType.AddString(_T("Flower"));
//j	m_comboFadeType.AddString(_T("Lightning Bolt"));
//j	m_comboFadeType.AddString(_T("Badge1"));
//j	m_comboFadeType.AddString(_T("Badge2"));
//j	m_comboFadeType.AddString(_T("Badge3"));
//j	m_comboFadeType.AddString(_T("Banner1"));
//j	m_comboFadeType.AddString(_T("Banner2"));

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnEnterTab(bool bFirstTime)
{
	m_strClipArtInfo.Empty();
	
	OnBorderNone(0, 0, 0);
	m_comboFadeType.SetCurSel(0);

	m_bFadeStretch = true;
	m_checkFadeStretch.SetCheck(m_bFadeStretch ? BST_CHECKED : BST_UNCHECKED);

	m_iFadeSize = 100;
	m_FadeSizeSlider.SetPos(m_iFadeSize);

	m_iFadeSoftness = 50;
	m_FadeSoftnessSlider.SetPos(m_iFadeSoftness);

	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_START));
	EnableButtons(/*Apply*/false, /*Reset*/false);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_END));
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_APPLY));
	
	if (bOK && !m_strClipArtInfo.IsEmpty())
		GetMainDlg().SetClipArtInfo(m_strClipArtInfo, false/*bInitialState*/);

	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_RESET));
	
	OnEnterTab();
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderNone(UINT uCode, int nID, HWND hwndCtrl)
{
	CheckRadioButton(IDC_IME_BORDER_NONE, IDC_IME_BORDER_FADE, IDC_IME_BORDER_NONE);

	m_btnChoose.EnableWindow(false);
	m_comboFadeType.EnableWindow(false);
	m_checkFadeStretch.EnableWindow(false);
	m_FadeSizeSlider.EnableWindow(false);
	m_FadeSoftnessSlider.EnableWindow(false);

	bool bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_RESET));
	EnableButtons(/*Apply*/false, /*Reset*/false);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderImage(UINT uCode, int nID, HWND hwndCtrl)
{
	CheckRadioButton(IDC_IME_BORDER_NONE, IDC_IME_BORDER_FADE, IDC_IME_BORDER_IMAGE);

	m_btnChoose.EnableWindow(true);
	m_comboFadeType.EnableWindow(false);
	m_checkFadeStretch.EnableWindow(false);
	m_FadeSizeSlider.EnableWindow(false);
	m_FadeSoftnessSlider.EnableWindow(false);

	bool bEnableButtons = false;
	bool bOK = false;
	if (!m_strBorderImageFileName.IsEmpty() && (bOK = DoCommand(_T("BorderImage"), m_strBorderImageFileName)))
		bEnableButtons = true;
	else
		bOK = DoCommand(_T("BordersTab"), Str(_T("%d"), EDIT_RESET));

	EnableButtons(/*Apply*/bEnableButtons, /*Reset*/bEnableButtons);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderFade(UINT uCode, int nID, HWND hwndCtrl)
{
	CheckRadioButton(IDC_IME_BORDER_NONE, IDC_IME_BORDER_FADE, IDC_IME_BORDER_FADE);

	m_btnChoose.EnableWindow(false);
	m_comboFadeType.EnableWindow(true);
	m_checkFadeStretch.EnableWindow(true);
	m_FadeSizeSlider.EnableWindow(true);
	m_FadeSoftnessSlider.EnableWindow(true);

	int iCurSel = m_comboFadeType.GetCurSel();
	CString strValue;
	m_comboFadeType.GetLBText(iCurSel, strValue);
	bool bOK = DoCommand(_T("BorderType"), strValue);
	bOK = DoCommand(_T("BorderStretch"), Str(_T("%d"), m_bFadeStretch));
	bOK = DoCommand(_T("BorderSize"), Str(_T("%d"), m_iFadeSize));
	bOK = DoCommand(_T("BorderSoftness"), Str(_T("%d"), m_iFadeSoftness));
	bOK = DoCommand(_T("BordersTab"), _T("0"));
	EnableButtons(/*Apply*/true, /*Reset*/true);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderImageChoose(UINT uCode, int nID, HWND hwndCtrl)
{
	#define SHIFT (GetAsyncKeyState(VK_SHIFT)<0)
	if (!SHIFT)
	{
		GetMainDlg().ArtSetup(CBordersTab::MyArtGalleryCallback, (LPARAM)this);
		return;
	}

	#define FILTER_WITH_GDIPLUS    _T("All Supported Files (*.bmp, *.gif, *.jpg, *.tif, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.tif*;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0TIF Files (*.tif)\0*.tif*\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0")
	#define FILTER_WITHOUT_GDIPLUS _T("All Supported Files (*.bmp, *.gif, *.jpg, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0")
	LPCTSTR pstrFilter = (CImage::IsGdiPlusInstalled() ? FILTER_WITH_GDIPLUS : FILTER_WITHOUT_GDIPLUS);

	CFileDialog FileDialog(true, _T("jpg"), NULL/*pszFileName*/, 
		OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		pstrFilter, m_hWnd/*hWndParent*/);

	FileDialog.m_ofn.lpstrTitle = _T("Open an Image File");
	if (FileDialog.DoModal() == IDCANCEL)
		return;

	m_strBorderImageFileName = FileDialog.m_ofn.lpstrFile;
	// Save the path in the registry
//j	if (regkey.m_hKey)
//j		bool bOK = (regkey.SetStringValue(REGVAL_MRU_PICTURE_PATH, StrPath(m_strBorderImageFileName)) == ERROR_SUCCESS);

	OnBorderImage(0, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderTypeSelect(UINT uCode, int nID, HWND hwndCtrl)
{
	int iCurSel = m_comboFadeType.GetCurSel();
	CString strValue;
	m_comboFadeType.GetLBText(iCurSel, strValue);
	bool bOK = DoCommand(_T("BorderType"), strValue);
	bOK = DoCommand(_T("BordersTab"), _T("0"));
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnBorderStretch(UINT uCode, int nID, HWND hwndCtrl)
{
	m_bFadeStretch = !m_bFadeStretch;
	m_checkFadeStretch.SetCheck(m_bFadeStretch ? BST_CHECKED : BST_UNCHECKED);
	bool bOK = DoCommand(_T("BorderStretch"), Str(_T("%d"), m_bFadeStretch));
	bOK = DoCommand(_T("BordersTab"), _T("0"));
}

/////////////////////////////////////////////////////////////////////////////
void CBordersTab::OnHScroll(int nSBCode, short nPos, HWND hWnd)
{
	CTrackBarCtrl pSliderCtrl(hWnd);
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_ENDTRACK:
		{
			int iValue = pSliderCtrl.GetPos();

			if (pSliderCtrl.m_hWnd == m_FadeSizeSlider.m_hWnd)
			{
				if (m_iFadeSize == iValue)
					return;

				m_iFadeSize = iValue;
				bool bOK = DoCommand(_T("BorderSize"), Str(_T("%d"), iValue));
			}
			else
			if (pSliderCtrl.m_hWnd == m_FadeSoftnessSlider.m_hWnd)
			{
				if (m_iFadeSoftness == iValue)
					return;

				m_iFadeSoftness = iValue;
				bool bOK = DoCommand(_T("BorderSoftness"), Str(_T("%d"), iValue));
			}

			bool bOK = DoCommand(_T("BordersTab"), _T("0"));
			break;
		}
	}
}
