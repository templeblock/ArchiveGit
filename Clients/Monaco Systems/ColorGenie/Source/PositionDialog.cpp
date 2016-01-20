// PositionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "colorgenie.h"
#include "PositionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPositionDialog dialog


CPositionDialog::CPositionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPositionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPositionDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPositionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPositionDialog)
	DDX_Control(pDX, IDC_STATIC_POSITION, m_ctlbitMap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPositionDialog, CDialog)
	//{{AFX_MSG_MAP(CPositionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPositionDialog message handlers


BOOL CPositionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int bitmap[3];

	if(m_type == 1)
	{
		bitmap[0] = IDB_LAB1;
		bitmap[1] = IDB_LAB2;
		bitmap[2] = IDB_LAB3;
	}
	else
	{
		bitmap[0] = IDB_DENSITY1;
		bitmap[1] = IDB_DENSITY2;
		bitmap[2] = IDB_DENSITY3;
	}

	switch(m_position){
	case(0):
		if(!m_bitMap.LoadBitmap(bitmap[0]))
			return FALSE;
		break;

	case(1):
		if(!m_bitMap.LoadBitmap(bitmap[1]))
			return FALSE;
		break;

	case(2):
		if(!m_bitMap.LoadBitmap(bitmap[2]))
			return FALSE;
		break;

	default:
		if(!m_bitMap.LoadBitmap(bitmap[0]))
			return FALSE;	
		break;
	}

	m_ctlbitMap.SetBitmap( HBITMAP(m_bitMap) );
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
