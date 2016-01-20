// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "border.h"
#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSettings

void CSettings::TransferSettings(BOOL aRegions[])
{
	// Transfer settings
	m_pBEObj->SetTiling(m_bForceTile, m_nTiles, m_nMaxTiles);
	if (m_bSizeChanged)
	{
#ifdef USE_CENTIMETRES
		m_pBEObj->SetSizing(m_dEdgeSize/2.54, aRegions);
#else
		m_pBEObj->SetSizing(m_dEdgeSize, aRegions);
#endif
		m_pBEObj->AdjustOffsets(TRUE);
		m_pBEObj->ComputeGridCoords();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CBEObjectD* pBEObject, CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	m_Settings.m_pBEObj = pBEObject;
	m_Settings.m_bSizeChanged = FALSE;
	m_bIgnoreSizeChange = FALSE;
	m_bIgnoreMaxChange = FALSE;

	//{{AFX_DATA_INIT(CSettingsDlg)
	m_nTiles = 1;
	m_dEdgeSize = 1.0;
	m_nMaxTiles = 0;
	//}}AFX_DATA_INIT
#ifdef USE_CENTIMETRES
//	m_dEdgeSize = 2.0;
#endif
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Text(pDX, IDC_ED_N_TILES, m_nTiles);
	DDV_MinMaxInt(pDX, m_nTiles, 1, 255);
	DDX_Text(pDX, IDC_ED_SIZE, m_dEdgeSize);
	DDV_MinMaxDouble(pDX, m_dEdgeSize, 0., 999.);
	DDX_Text(pDX, IDC_ED_MAX_TILES, m_nMaxTiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_BN_CLICKED(IDC_USE_TILING, OnUseTiling)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_ED_SIZE, OnChangeEdSize)
	ON_EN_CHANGE(IDC_ED_MAX_TILES, OnChangeEdMaxTiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

BOOL CSettingsDlg::OnInitDialog() 
{

	// Set up spin controls
	CSpinButtonCtrl* pTiles = (CSpinButtonCtrl*)GetDlgItem(IDC_TILE_SPIN);
	pTiles->SetBuddy(GetDlgItem(IDC_ED_N_TILES));
	pTiles->SetRange(1, 255);
	
	CSpinButtonCtrl* pMaxTiles = (CSpinButtonCtrl*)GetDlgItem(IDC_MAX_TILE_SPIN);
	pMaxTiles->SetBuddy(GetDlgItem(IDC_ED_MAX_TILES));
	pMaxTiles->SetRange(1, UD_MAXVAL);
	
	// Disable everything if only corners are selected.
	// May change later to have a corners & edges worksheet, or
	// disable Settings in the menu.
	if (m_Settings.m_pBEObj->IsEdgeSelected())
	{
		// Gather settings and enable/disable appropriate controls
		m_Settings.m_pBEObj->GetTiling(m_Settings.m_bForceTile, m_nTiles, m_nMaxTiles);
#ifdef USE_CENTIMETRES
		double temp = m_dEdgeSize/2.54;
		m_bValidSize = m_Settings.m_pBEObj->GetSizing(temp);
#else
		m_bValidSize = m_Settings.m_pBEObj->GetSizing(m_dEdgeSize);
#endif
		m_bValidMaxTiles = (m_nMaxTiles != -1);

		// Handle the tiling boxes
		CheckDlgButton(IDC_USE_TILING, m_Settings.m_bForceTile);
		GetDlgItem(IDC_N_TILES)->EnableWindow(m_Settings.m_bForceTile);
		GetDlgItem(IDC_ED_N_TILES)->EnableWindow(m_Settings.m_bForceTile);
		GetDlgItem(IDC_MAX_TILES)->EnableWindow(!m_Settings.m_bForceTile);
		GetDlgItem(IDC_ED_MAX_TILES)->EnableWindow(!m_Settings.m_bForceTile);
		if (!m_Settings.m_bForceTile)
		{
			m_nTiles = 1;
		}

		// Limit sizing text
		((CEdit*)GetDlgItem(IDC_ED_SIZE))->SetLimitText(5);	// x.xxx
	}
	else
	{
		// Disable everything
		GetDlgItem(IDC_TILING)->EnableWindow(FALSE);
		GetDlgItem(IDC_USE_TILING)->EnableWindow(FALSE);
		GetDlgItem(IDC_N_TILES)->EnableWindow(FALSE);
		GetDlgItem(IDC_ED_N_TILES)->EnableWindow(FALSE);
		GetDlgItem(IDC_MAX_TILES)->EnableWindow(FALSE);
		GetDlgItem(IDC_ED_MAX_TILES)->EnableWindow(FALSE);
		GetDlgItem(IDC_REGION_SIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_ED_SIZE)->EnableWindow(FALSE);
		pTiles->EnableWindow(FALSE);
	}

	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::OnOK() 
{
	// If edge size is invalid, just set it to 0
	if (!m_bValidSize)
	{
		m_bIgnoreSizeChange = TRUE;
		GetDlgItem(IDC_ED_SIZE)->SetWindowText("0.");
	}

	// Need a value
	if (!m_bValidMaxTiles)
	{
		m_bIgnoreMaxChange = TRUE;
		GetDlgItem(IDC_ED_MAX_TILES)->SetWindowText("-1");
	}

	CDialog::OnOK();

	// Transfer to settings
	m_Settings.m_nTiles = m_nTiles;
	m_Settings.m_nMaxTiles = m_nMaxTiles;
	m_Settings.m_dEdgeSize = m_dEdgeSize;
}

void CSettingsDlg::OnUseTiling() 
{
	m_Settings.m_bForceTile = IsDlgButtonChecked(IDC_USE_TILING);

	// Enable or disable the edit boxes
	GetDlgItem(IDC_N_TILES)->EnableWindow(m_Settings.m_bForceTile);
	GetDlgItem(IDC_ED_N_TILES)->EnableWindow(m_Settings.m_bForceTile);
	GetDlgItem(IDC_TILE_SPIN)->EnableWindow(m_Settings.m_bForceTile);
	GetDlgItem(IDC_MAX_TILES)->EnableWindow(!m_Settings.m_bForceTile);
	GetDlgItem(IDC_ED_MAX_TILES)->EnableWindow(!m_Settings.m_bForceTile);
	GetDlgItem(IDC_MAX_TILE_SPIN)->EnableWindow(!m_Settings.m_bForceTile);

	// Appears to be a bug where window is not updated---
	GetDlgItem(IDC_TILE_SPIN)->Invalidate();

}

void CSettingsDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (!m_bValidSize)
	{
		m_bIgnoreSizeChange = TRUE;
		GetDlgItem(IDC_ED_SIZE)->SetWindowText("");
	}

	if (m_nMaxTiles == -1)
	{
		m_bIgnoreMaxChange = TRUE;
		GetDlgItem(IDC_ED_MAX_TILES)->SetWindowText("");
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CSettingsDlg::OnChangeEdSize() 
{
	if (!m_bIgnoreSizeChange)
	{
		m_Settings.m_bSizeChanged = m_bValidSize = TRUE;
	}
	m_bIgnoreSizeChange = FALSE;
}


void CSettingsDlg::OnChangeEdMaxTiles() 
{	
	CString szText;
	GetDlgItem(IDC_ED_MAX_TILES)->GetWindowText(szText);
	if (!m_bIgnoreMaxChange)
	{
		m_bValidMaxTiles = TRUE;
	}
	if (szText.IsEmpty())
	{
		m_bValidMaxTiles = FALSE;
	}
	m_bIgnoreMaxChange = FALSE;
	
}
