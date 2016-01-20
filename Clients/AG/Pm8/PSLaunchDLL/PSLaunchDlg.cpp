// PSLaunchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
//#include "PSLaunchDLL.h"

#include "PSLaunchDlg.h"
#include "ProjectCategoryDlg.h"
#include "CommonTypes.h"
#include "PSButton.h"

#include "LaunchInterfaces.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const UINT kMainScreenTransitions[][2] =
{
	//  Command ID				Project Category Dialog
	//
	{	IDC_CARDSHOP,			IDD_CATEGORY_CARD_SHOP			},
	{	IDC_PHOTO_PROJECTS,		IDD_CATEGORY_PHOTO_PROJECTS		},
	{	IDC_CREATIVE_CORNER,	IDD_CATEGORY_CREATIVE_CORNER	},
	{	IDC_CATEGORY_CLASSROOM,	IDD_CATEGORY_CLASSROOM			},
	{	IDC_CATEGORY_WORKPLACE,	IDD_CATEGORY_WORKPLACE			},
	{	IDC_PROJECTS_INTERNET,	IDD_CATEGORY_INTERNETWORLD		},
	{	IDC_PROJECTS_ALL,		IDD_CATEGORY_ALL				},
	{	IDC_CATEGORY_OPEN,		IDD_CATEGORY_OPEN				}
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchDlg dialog

CPSLaunchDlg::CPSLaunchDlg(CWnd* pParent /*=NULL*/)	
	: CPSBaseDialog(CPSLaunchDlg::IDD, pParent)
	, m_uiMouseOverId( 0 )
	, m_nItem( -1 )
{
	//{{AFX_DATA_INIT(CPSLaunchDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPSLaunchDlg::~CPSLaunchDlg()
{
	for (int i = m_cDlgStack.GetUpperBound(); i >= 0; i--)
	{
		m_cDlgStack[i]->DestroyWindow();
		delete m_cDlgStack[i];
	}
}

void CPSLaunchDlg::DoDataExchange(CDataExchange* pDX)
{
	CPSBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPSLaunchDlg)
	DDX_Control(pDX, IDC_INFO_TEXT, m_cInfoText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPSLaunchDlg, CPSBaseDialog)
	//{{AFX_MSG_MAP(CPSLaunchDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( UM_PUSH_DIALOG, OnPushDialog )
	ON_MESSAGE( UM_POP_DIALOG, OnPopDialog )
	ON_MESSAGE( UM_REMOVE_UNUSED, OnRemoveUnused )
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_WM_CTLCOLOR()
	ON_MESSAGE( UM_MOUSEMOVE, OnUmMouseMove )
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchDlg message handlers

BOOL CPSLaunchDlg::OnInitDialog()
{
	CPSBaseDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPSLaunchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CPSBaseDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPSLaunchDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPSBaseDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPSLaunchDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CPSLaunchDlg::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect( rect );
	rect.InflateRect( 0, 0, 1, 1 );
	pDC->FillSolidRect( rect, RGB( 255, 255, 255 ) );
	
	return TRUE;
//	return CPSBaseDialog::OnEraseBkgnd( pDC );
}

BOOL CPSLaunchDlg::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	if (BN_CLICKED == HIWORD( wParam ))
	{
		// Search through table to find matching command.
		for (int i = 0; i < NumElements( kMainScreenTransitions ); i++)
		{
			if (kMainScreenTransitions[i][0] == LOWORD( wParam ))
			{
				for (int j = m_cDlgStack.GetUpperBound(); j >= 0; j--)
				{
					m_cDlgStack[j]->DestroyWindow();
					delete m_cDlgStack[j];
				}

				m_cDlgStack.RemoveAll();
				m_nItem = -1;

				CDialog* pDialog = new CProjectCategoryDlg( kMainScreenTransitions[i][1], this );
				SendMessage( UM_PUSH_DIALOG, kMainScreenTransitions[i][1], (LPARAM) pDialog );

				return TRUE;
			}
		}
	}

	return CPSBaseDialog::OnCommand(wParam, lParam);
}

LONG CPSLaunchDlg::OnPushDialog( UINT wParam, LONG lParam )
{
	ASSERT( lParam && wParam );

	CRect rect;
	CWnd* pWnd = GetDlgItem( IDC_STATIC_WORKAREA );
	pWnd->GetWindowRect( rect );
	ScreenToClient( rect );

	CPSBaseDialog* pDialog =  (CPSBaseDialog *) lParam;

	if (m_cDlgStack.GetUpperBound() > m_nItem)
	{
		// Check to see if the user is moving back to
		// one that has already been displayed.
		CPSBaseDialog *pNextDlg = (CPSBaseDialog *) m_cDlgStack[m_nItem + 1];

		if (pDialog->GetTemplateID() == pNextDlg->GetTemplateID())
		{
			// Same dialog, so delete the one passed
			// in, and use the one that is already
			// created.
			delete pDialog;
			pDialog = pNextDlg;
			pDialog->SendMessage( UM_UPDATE_DATA );
			pDialog->ShowWindow( SW_SHOW );

			goto _found_dialog;
		}
		else
		{
			// Pop off all the dialogs above the current
			// point, as the user has selected a different
			// path.
			OnRemoveUnused( 0, 0L );
		}
	}

	if (!pDialog->Create( wParam, this ))
		ASSERT( FALSE );

	pDialog->SetWindowPos( pWnd, rect.left + 3, rect.top + 3, 
		0, 0, SWP_NOSIZE | SWP_SHOWWINDOW );

	m_cDlgStack.Add( pDialog );

_found_dialog:
	
	// Advance counter to 
	// next item in list.
	m_nItem++;

	// Hide the previous dialog
	//
	if (m_nItem > 0)
	{
		m_cDlgStack[m_nItem - 1]->ShowWindow( SW_HIDE );
	}
	else
	{
		ASSERT( GetDlgItem( IDC_WORKSPACE_SPLASH ) );
		GetDlgItem( IDC_WORKSPACE_SPLASH )->ShowWindow( SW_HIDE );
	}

	return 0L;
}

LONG CPSLaunchDlg::OnPopDialog( UINT, LONG )
{
	ASSERT( m_cDlgStack.GetUpperBound() > 0 );

//	int nItem = m_cDlgStack.GetUpperBound();

	m_nItem--;

	if (m_nItem >= 0)
	{
		// Display the previous window
		CWnd* pWnd = GetDlgItem( IDC_STATIC_WORKAREA );
		m_cDlgStack[m_nItem]->SetWindowPos( pWnd, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
	}
	else
	{
		ASSERT( GetDlgItem( IDC_WORKSPACE_SPLASH ) );
		GetDlgItem( IDC_WORKSPACE_SPLASH )->ShowWindow( SW_SHOW );
	}

	// Hide the previously displayed window
	if (m_nItem < m_cDlgStack.GetUpperBound())
		m_cDlgStack[m_nItem + 1]->ShowWindow( SW_HIDE );

	// Get rid of the top window
//	m_cDlgStack[nItem]->DestroyWindow();
//	delete m_cDlgStack[nItem];

//	m_cDlgStack.RemoveAt( nItem );

	return 0L;
}

LONG CPSLaunchDlg::OnRemoveUnused( UINT, LONG )
{
	// Pop off all the dialogs above the current
	// point, as the user has selected a different
	// path.
	for (int j = m_cDlgStack.GetUpperBound(); j > m_nItem; j--)
	{
		m_cDlgStack[j]->DestroyWindow();
		delete m_cDlgStack[j];

		m_cDlgStack.RemoveAt( j );
	}

	return 0L;
}

void CPSLaunchDlg::OnOK() 
{
	CPSBaseDialog::OnOK();

	// TODO: Write out configuration file

}

void CPSLaunchDlg::OnOpen() 
{
	EndDialog( kResultOpen );
}

HBRUSH CPSLaunchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (IDC_INFO_TEXT == pWnd->GetDlgCtrlID())
	{
		HBRUSH hbr = ::CreateSolidBrush( RGB( 181, 214, 255 ) );
		pDC->SetBkMode( TRANSPARENT );

		return hbr;
	}
	
	return CPSBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

LONG CPSLaunchDlg::OnUmMouseMove( WPARAM wParam, LPARAM lParam )
{
	CWnd* pWnd = GetFocus();

	// Make sure one of our controls has focus.  If no control
	// has focus, then we probably aren't an active window.

	if (pWnd && wParam != m_uiMouseOverId)
	{
		if (m_uiMouseOverId)
		{
			// Notify the control that the mouse has left the building
			CWnd* pWnd = GetDlgItem( m_uiMouseOverId );
			if (pWnd) pWnd->PostMessage( UM_MOUSEEXIT );
		}

		// Tell the control that it is ok to track the mouse move
		// as we will notify it when the mouse leaves
		CPSButton* pButton = (CPSButton *) CWnd::FromHandle( (HWND) lParam );

		if (pButton)
		{
			CString strInfoText;
			if (!pButton->GetInfoText( strInfoText ))
				strInfoText.LoadString( IDS_INFOTEXT_DEFAULT );

			m_cInfoText.SetWindowText( strInfoText );

			pButton->SendMessage( UM_MOUSEENTER );
		}

		m_uiMouseOverId = wParam;
	}

	return 0L;
}

void CPSLaunchDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd* pWnd = GetFocus();

	// Make sure one of our controls has focus.  If no control
	// has focus, then we probably aren't an active window.

	if (pWnd && m_uiMouseOverId)
	{
		// Notify the control that the mouse has left the building
		CWnd* pWnd = GetDlgItem( m_uiMouseOverId );
		if (pWnd) pWnd->PostMessage( UM_MOUSEEXIT );

		m_uiMouseOverId = 0;
	}
	
	CPSBaseDialog::OnMouseMove(nFlags, point);
}
