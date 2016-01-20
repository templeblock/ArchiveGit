// PromtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "PromtDlg.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPromptDlg dialog


CPromptDlg::CPromptDlg(CString csTitle, CString csMessage, CWnd* pParent /*=NULL*/)
	: CDialog(CPromptDlg::IDD, pParent),
	m_csTitle(csTitle),
	m_csMessage(csMessage)
{
	//{{AFX_DATA_INIT(CPromptDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPromptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPromptDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPromptDlg, CDialog)
	//{{AFX_MSG_MAP(CPromptDlg)
	ON_MESSAGE(UM_CDROM_DETECTED, OnCDDetected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPromptDlg message handlers

LRESULT CPromptDlg::OnCDDetected(LPARAM lParam, WPARAM wParam)
{
	OnOK();
	return 0L;
}

BOOL CPromptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_csTitle);
	
	//We want to determine the diminsions of the dialog and then resize it and it's
	//controls as needed.

	//get the text extents  
	CDC* pDC = GetDC();
	CSize szTitle = pDC->GetTextExtent(m_csTitle);
	int nLineHeight = szTitle.cy;

	CSize szMessage(0,0);
	CString csTemp(m_csMessage);
	while(TRUE)
	{
		CString csSubString = csTemp.SpanExcluding("\n");
		CSize szTemp = pDC->GetTextExtent(csSubString);
		szMessage.cx = max(szMessage.cx, szTemp.cx);
		szMessage.cy += nLineHeight;
		int nNewLine = csTemp.Find("\n");
		if(nNewLine == -1)
			break;
		csTemp = csTemp.Mid(nNewLine + 1);

	}

	ReleaseDC(pDC);

	//get the center point of the dialog
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint ptCenter = rcWindow.CenterPoint();

	//get the dimensions of a button
	CWnd* pButtonOK = GetDlgItem(IDOK);
	CRect rcButton;
	if(pButtonOK)
	{
		pButtonOK->GetClientRect(rcButton);
	}

	//calculate the dialog dimensions
	int nXDim = max(max(szTitle.cx, szMessage.cx), 2 * rcButton.Width() + rcButton.Height() + 10) + 10;
	int nYDim = szTitle.cy + szMessage.cy + rcButton.Height() + 25;

	MoveWindow(ptCenter.x - nXDim / 2, ptCenter.y - nYDim / 2,
				nXDim, nYDim, TRUE);

	//now to resize the controls
	CRect rcClient;
	GetClientRect(rcClient);
	ptCenter = rcClient.CenterPoint();
	//calculate dims for static text
	CRect rcMessage(rcClient.left + 2, rcClient.top + 5,
					rcClient.right - 2, rcClient.top + szMessage.cy + 5);

	CWnd* pMessageWnd = GetDlgItem(IDC_MESSAGE);
	if(pMessageWnd != NULL)
	{
		pMessageWnd->MoveWindow(rcMessage.left, rcMessage.top, rcMessage.Width(),
								rcMessage.Height(), TRUE);

		pMessageWnd->SetWindowText(m_csMessage);
	}
	//calculate and move the location of the buttons
	int nSpacing;
	if(pButtonOK)
	{
		nSpacing = rcButton.Height() / 2;
		pButtonOK->MoveWindow(ptCenter.x - nSpacing - rcButton.Width(), 
							rcClient.bottom - rcButton.Height() - 5,
							rcButton.Width(), rcButton.Height(), TRUE);
	}
	CWnd* pButtonCancel = GetDlgItem(IDCANCEL);
	if(pButtonCancel)
	{
		pButtonCancel->MoveWindow(ptCenter.x + nSpacing,
									rcClient.bottom - rcButton.Height() - 5,
									rcButton.Width(), rcButton.Height(), TRUE);
	
	}

	if (AfxGetMainWnd()->IsWindowVisible())
	{
		GET_PMWAPP()->WakeWindow();  //Bring us to top
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
