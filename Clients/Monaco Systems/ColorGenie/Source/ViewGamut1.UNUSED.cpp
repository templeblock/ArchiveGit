// ViewGamut1.cpp : implementation file
//

#include "stdafx.h"
#include "ColorGenie.h"
#include "ViewGamut1.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewGamut1 dialog

// Message Map
BEGIN_MESSAGE_MAP(CViewGamut1, CDialog)
	//{{AFX_MSG_MAP(CViewGamut1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Event Map
BEGIN_EVENTSINK_MAP(CViewGamut1, CDialog)
    //{{AFX_EVENTSINK_MAP(CViewGamut1)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
CViewGamut1::CViewGamut1(CProfileDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CViewGamut1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewGamut1)
	m_pDoc = pDoc;
	m_pControl = NULL;
	m_ppMyself = NULL;
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
CViewGamut1::~CViewGamut1()
{
	if (m_pControl)
	{
		delete m_pControl;
		m_pControl = NULL;
	}

	if (m_ppMyself)
		*m_ppMyself = NULL;
}


/////////////////////////////////////////////////////////////////////////////
void CViewGamut1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewGamut1)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CViewGamut1 message handlers


/////////////////////////////////////////////////////////////////////////////
BOOL CViewGamut1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_pDoc)
		return TRUE;

	// Initialize stuff
	CenterWindow();
	ShowWindow(SW_NORMAL);
	UpdateWindow();

	RECT rect;
	GetClientRect(&rect);
	rect.bottom -= 40;

	m_pControl = new CQd3dControl(rect, this, 999/*id*/);
	if (m_pControl)
	{
		// Export the source color data as a way to pass it to the Qd3dGamut object
		if (!m_pDoc->SavePatch("DryjetColorData.txt", 0/*dest (DryJet)*/))
			Message( "Can't read the data." );

		m_pControl->SetGamutDataFile("DryjetColorData.txt");
		m_pControl->ShowWindow(SW_SHOW); 
		m_pControl->UpdateWindow();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
BOOL CViewGamut1::DoModeless(CWnd* pParent, CViewGamut1** ppMyself)
{
	m_ppMyself = ppMyself;
	return Create(IDD, pParent);
}


/////////////////////////////////////////////////////////////////////////////
void CViewGamut1::PostNcDestroy() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut1::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CViewGamut1::OnCancel() 
{
	DestroyWindow();
}
