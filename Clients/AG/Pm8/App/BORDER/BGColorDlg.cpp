// BGColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "border.h"
#include "BGColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBGColorDlg dialog


CBGColorDlg::CBGColorDlg(CBEObjectD* pBEObject, CBECallbackInterface* pCallbackInterface, CWnd* pParent /*=NULL*/)
	: CDialog(CBGColorDlg::IDD, pParent), 
	m_ptTopLeft(180, 28)
{
	const BOX_WIDTH = 40;
	const BOX_HEIGHT = 40;

	m_pBEObj = pBEObject;
	m_pCallbackInterface = pCallbackInterface;

	// Compute other points from anchor
	m_ptTopRight = m_ptTopLeft + CPoint(BOX_WIDTH, 0);
	m_ptBotRight = m_ptTopLeft + CPoint(BOX_WIDTH, BOX_HEIGHT);
	m_ptBotLeft = m_ptTopLeft + CPoint(0, BOX_HEIGHT);

	//{{AFX_DATA_INIT(CBGColorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBGColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBGColorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBGColorDlg, CDialog)
	//{{AFX_MSG_MAP(CBGColorDlg)
	ON_BN_CLICKED(IDC_DISPLAY_BG_COLOR, OnDisplayBgColor)
	ON_BN_CLICKED(IDC_CHOOSE_COLOR, OnChooseColor)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBGColorDlg message handlers

BOOL CBGColorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Retrieve current BG color and enable/disable controls as necessary
	m_bUseBGColor = m_pBEObj->GetBGColor(m_colBGColor);

	CheckDlgButton(IDC_DISPLAY_BG_COLOR, m_bUseBGColor);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBGColorDlg::OnDisplayBgColor() 
{
	m_bUseBGColor = IsDlgButtonChecked(IDC_DISPLAY_BG_COLOR);
}

void CBGColorDlg::OnChooseColor() 
{
	CColorDialog ColorDlg(m_colBGColor, 0, this);		// Windows color picker dialog

	if (ColorDlg.DoModal() == IDOK) {
		m_bUseBGColor = TRUE;
		m_colBGColor = ColorDlg.GetColor();
		Invalidate(FALSE);

		// Close the dialog
		OnOK();
	}
	
}

void CBGColorDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CPen *pOldPen;
	CPen BoxPen(PS_SOLID, 1, RGB(128, 128, 128));

	// Draw the box surrounding the color box
	dc.MoveTo(m_ptBotLeft);
	pOldPen = (CPen *)dc.SelectObject(&BoxPen); 
	dc.LineTo(m_ptTopLeft);
	dc.LineTo(m_ptTopRight);
	dc.SelectStockObject(WHITE_PEN);	
	dc.LineTo(m_ptBotRight);
	dc.LineTo(m_ptBotLeft);
	dc.SelectObject(pOldPen);
	
	// Draw the color box
	if (m_colBGColor != -1L)
	{
		CBEDrawInfo stDrawInfo = {
			dc.m_hDC,				// m_hdc
			{m_ptTopLeft.x+1,
			 m_ptTopLeft.y+1,
			 m_ptBotRight.x-1,
			 m_ptBotRight.y-1},		// m_Bounds
			NULL,					// m_pClip
			0.,						// m_dAngle
			FALSE,					// m_fXFlipped
			FALSE,					// m_fYFlipped
			m_colBGColor,			// m_clBackground
			this->m_hWnd,			// m_hWnd
			TRUE,					// m_fAllowInterrupt
			NULL,					// m_pBEDrawState
			NULL					// m_pClientData
		};
		m_pCallbackInterface->DrawGraphic(0, m_pBEObj->GetDatabase(), &stDrawInfo);
	}

	// Do not call CDialog::OnPaint() for painting messages
}

