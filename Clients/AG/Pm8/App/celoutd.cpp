// celoutd.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "celOutD.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCellOutlineDlg dialog

CCellOutlineDlg::CCellOutlineDlg(CTableCellFormatting* pFormatting, CWnd* pParent /*=NULL*/)
	: CDialog(CCellOutlineDlg::IDD, pParent)
{
	m_pFormatting = pFormatting;
	//{{AFX_DATA_INIT(CCellOutlineDlg)
	m_nLineWidth = 0;
	//}}AFX_DATA_INIT
	m_pOutlineControl = NULL;
}

void CCellOutlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCellOutlineDlg)
	DDX_Control(pDX, IDC_SPIN_LINEWIDTH, m_ctlSpinLine);
	DDX_Control(pDX, IDC_LINE_WIDTH, m_ctlLineWidth);
	DDX_Control(pDX, IDC_FILL_COLOR, m_FillColor);
	DDX_Control(pDX, IDC_LINE_COLOR, m_LineColor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCellOutlineDlg, CDialog)
	//{{AFX_MSG_MAP(CCellOutlineDlg)
	ON_BN_CLICKED(IDC_OUTLINE_CONTROL, OnEdgeSelect)
	ON_BN_CLICKED(IDC_FILL_COLOR, OnFillColor)
	ON_BN_CLICKED(IDC_LINE_COLOR, OnLineColor)
	ON_EN_CHANGE(IDC_LINE_WIDTH, OnChangeLineWidth)
	ON_EN_SETFOCUS(IDC_LINE_WIDTH, OnSetfocusLineWidth)
	ON_EN_KILLFOCUS(IDC_LINE_WIDTH, OnKillfocusLineWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCellOutlineDlg message handlers

BOOL CCellOutlineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pOutlineControl = (CCellOutlineControl*)GetDlgItem(IDC_OUTLINE_CONTROL);
	m_pOutlineControl->Init(m_pFormatting);

//	m_comboFillColor.SubclassDlgItem(IDC_FILL_COLOR, this);
	m_FillColor.SetColor( RColorFromColor( m_pFormatting->m_FillColor ));

//	m_comboLineColor.SubclassDlgItem(IDC_LINE_COLOR, this);

	m_ctlLineWidth.LimitText(2);

	m_ctlSpinLine.SetRange(0, 99);

	// Update the cell controls to match the current formatting.
	UpdateCellControls();

	return TRUE;
}

void CCellOutlineDlg::OnOK() 
{
	CDialog::OnOK();
}

void CCellOutlineDlg::OnEdgeSelect() 
{
	UpdateCellControls();
}

//
// Update the cell controls with the currently selected segment.
//

void CCellOutlineDlg::UpdateCellControls(void)
{
	// Update the parameters in the dialog to reflect the current segment.
	int nSegment = m_pOutlineControl->GetSelectedSegment();
	if (nSegment != CTableCellFormatting::SEGMENT_None)
	{
		CTableBorderSegment& Segment = m_pOutlineControl->GetSegment(nSegment);
		m_LineColor.SetColor( RColorFromColor( Segment.m_Color ));

		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_LINE_WIDTH);
		if (pEdit != NULL)
		{
			CString cs;
			if (Segment.m_bWidth != (BYTE)-1)
			{
				cs.Format("%d", Segment.m_bWidth);
			}
			pEdit->SetWindowText(cs);
		}
	}
}

void CCellOutlineDlg::OnFillColor() 
{
	RWinColorDlg dlg(this, kShowTransparent) ;
	dlg.SetColor( m_FillColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		RColor rColor = dlg.SelectedColor() ;
		m_FillColor.SetColor( rColor ) ;

		m_pOutlineControl->SetFillColor( ColorFromRColor( rColor ));
	}
}

void CCellOutlineDlg::OnLineColor() 
{
	RWinColorDlg dlg(this, kShowTransparent) ;
	dlg.SetColor( m_LineColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		RColor rColor = dlg.SelectedColor() ;
		m_LineColor.SetColor( rColor ) ;

		m_pOutlineControl->SetLineColor(ColorFromRColor( rColor ));
	}
}

void CCellOutlineDlg::OnChangeLineWidth() 
{
	if (m_ctlLineWidth.GetSafeHwnd() != NULL)
	{
		CString csText;
		m_ctlLineWidth.GetWindowText(csText);
		int nWidth;
		if (csText.IsEmpty())
		{
			if (GetFocus() == &m_ctlLineWidth)
			{
				nWidth = m_nLineWidthSave;
			}
			else
			{
				return;
			}
		}
		else
		{
			nWidth = atoi(csText);
		}
		m_pOutlineControl->SetLineWidth(nWidth);
	}
}

void CCellOutlineDlg::OnSetfocusLineWidth() 
{
	// Save the current line width for this segment.
	m_nLineWidthSave = m_pOutlineControl->GetLineWidth();
}

void CCellOutlineDlg::OnKillfocusLineWidth() 
{
	CString csText;
	m_ctlLineWidth.GetWindowText(csText);
	if (csText.IsEmpty())
	{
		// Revert back to what was there to begin with
		// (which may be blank anyway).
		m_pOutlineControl->SetLineWidth(m_nLineWidthSave);
		UpdateCellControls();
	}
}
