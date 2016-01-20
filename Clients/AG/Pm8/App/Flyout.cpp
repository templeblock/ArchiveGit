// RFlyout.cpp : implementation file
//

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME


//#include "RenaissanceResource.h"
#include "Flyout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RFlyout dialog


RFlyout::RFlyout(UINT nNumRows, UINT nToolBarID, CWnd* pParent /*=NULL*/)
	: CDialog(RFlyout::IDD, pParent),
	m_nToolBarID(nToolBarID),
	m_toolBar(nNumRows)
{
	//{{AFX_DATA_INIT(RFlyout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void RFlyout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RFlyout)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RFlyout, CDialog)
	//{{AFX_MSG_MAP(RFlyout)
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RFlyout message handlers

BOOL RFlyout::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_toolBar.Create(this) && m_toolBar.LoadToolBar(m_nToolBarID))
	{
		m_toolBar.SetBarStyle(m_toolBar.GetBarStyle() | CBRS_FLOATING | 
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

		for(int i = 0; i < m_toolBar.GetCount(); i++)
		{
			m_toolBar.SetButtonStyle(i, TBBS_CHECKBOX | TBBS_CHECKGROUP );
		}
//		m_toolBar.GetToolBarCtrl().SetState(ID_CIRCLE, TBSTATE_CHECKED | TBSTATE_ENABLED );
		CSize szToolbar = m_toolBar.GetSize();
		CRect rcDlg;
		GetClientRect(&rcDlg);
		rcDlg.right = rcDlg.left + szToolbar.cx;
		rcDlg.bottom = rcDlg.top + szToolbar.cy - 2;
		MoveWindow(rcDlg);

		CRect rcToolBar(0,0, szToolbar.cx, szToolbar.cy);
		rcToolBar.OffsetRect(0,-2);
		m_toolBar.MoveWindow(rcToolBar, TRUE);
		ShowWindow(SW_SHOW);
		m_bDestroyWindow = TRUE;
	}
	else
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RFlyout::OnOK() 
{
	OnCancel();//destroy the dialog window
}

void RFlyout::OnCancel() 
{
	//has DestroyWindow already been called.  This is necessary because DestroyWindow
	//calls OnActivate which we have set up to call OnCancel which calls DestroyWindow.
	//We use OnActivate to dismiss the dialog if the user clicks outside the dialog.
	if( m_bDestroyWindow )
	{
		m_bDestroyWindow = FALSE;
		DestroyWindow();
	}
}

// @member <CBColorToolbar>  Function used to dismiss dialog if user clicks outside
// of client rect.  We used this instead of setting the capture because setting
// the capture caused too many other problems.(MFW 11/10/97)
void RFlyout::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	
	//this function is called with the state set to INACTIVE 
	//when the dialog loses focus.  This is necessary to dismiss the
	//dialog when the user clicks outside the dialog.
	if( nState == WA_INACTIVE )
		OnCancel();
		
}


void RFlyout::SetButtonState(UINT nID, UINT nState )
{
	CToolBarCtrl& toolBarCtrl = m_toolBar.GetToolBarCtrl();
	toolBarCtrl.SetState(nID, nState);
}

BOOL RFlyout::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// this traps Enter (1) and Escape (2) key messages
	if ( LOWORD(wParam) == 1 || LOWORD(wParam) == 2 )
	{
		OnCancel();
	}
	else if(m_pParentWnd)
	{
		m_pParentWnd->PostMessage(WM_COMMAND, wParam, lParam);
	}

	return TRUE;
}
