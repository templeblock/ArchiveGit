// CalPicD.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "CalPicD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarPictureDlg dialog


CCalendarPictureDlg::CCalendarPictureDlg(NEWINFO * ni, COLORREF cr, CWnd* pParent /*=NULL*/)
	: CPmwDialogColor(CCalendarPictureDlg::IDD, pParent, cr)
{

		switch (ni->m_PictureStyle)
		{
			case CAL_PICTURE_NONE:
			{
				m_iAbove = 0;
				m_iPicture = 1;
				break;
			}
			case CAL_PICTURE_TOP:
			{
				m_iAbove = 0;
				m_iPicture = 0;
				break;
			}
			case CAL_PICTURE_LEFT:
			{
				m_iAbove = 1;
				m_iPicture = 0;
				break;
			}
			case CAL_PICTURE_SAME:
			{
				m_iAbove = 2;
				m_iPicture = 0;
				break;
			}
		};
	//{{AFX_DATA_INIT(CCalendarPictureDlg)
	//}}AFX_DATA_INIT

		m_type = ni->m_pCalendarInfo->style.type;
		m_orientation = ni->orientation;
	
	m_WizFlags = WIZBUT_BACK | WIZBUT_NEXT | WIZBUT_CANCEL | WIZBUT_HELP;
}


void CCalendarPictureDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarPictureDlg)
	DDX_Radio(pDX, IDC_PICTURE_ABOVE, m_iAbove);
	DDX_Radio(pDX, IDC_PICTURE_YES, m_iPicture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalendarPictureDlg, CPmwDialogColor)
	//{{AFX_MSG_MAP(CCalendarPictureDlg)
		ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_PICTURE_ABOVE, OnPictureAbove)
	ON_BN_CLICKED(IDC_PICTURE_NO, OnPictureNo)
	ON_BN_CLICKED(IDC_PICTURE_SIDE, OnPictureSide)
	ON_BN_CLICKED(IDC_PICTURE_YES, OnPictureYes)
	ON_BN_CLICKED(IDC_PICTURE_SAME, OnPictureSame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarPictureDlg message handlers

BOOL CCalendarPictureDlg::OnInitDialog() 
{
	VERIFY (m_Preview.SubclassDlgItem (IDC_MAKE_PREVIEW, this));
	CPmwDialogColor::OnInitDialog();
	
	CreateWizzardButtons ();
  EnableWizardButtons (m_WizFlags);
	UpdateCBitmap();	
	FixControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCalendarPictureDlg::OnBack() 
{
   EndDialog (IDC_BACK);
}


void CCalendarPictureDlg::OnPictureAbove() 
{
	UpdateCBitmap();
	
}

void CCalendarPictureDlg::OnPictureNo() 
{
	UpdateCBitmap();
	FixControls();
}

void CCalendarPictureDlg::OnPictureSide() 
{
	UpdateCBitmap();
	
}

void CCalendarPictureDlg::OnPictureYes() 
{
	UpdateCBitmap();
	FixControls();
}


// UpdateCBitmap()
// creates resource name based on type of calendar, then loads it

void CCalendarPictureDlg::UpdateCBitmap()
{
	UpdateData(TRUE);

	// resource name / prefix
	CString MetaName = "CALENDAR";
	
	switch (m_type)
	{
		case MONTHLY:
			{
				MetaName += "M";
				break;
			}
		case WEEKLY:
			{
				MetaName += "W";
				break;
			}
		case YEARLY:
			{
				MetaName += "Y";
				break;
			}
	};

	if (m_iPicture == 0)
	{
		switch (m_iAbove)
		{
			case 0:
			{
				m_picture_style = CAL_PICTURE_TOP;
				MetaName += "T";
				break;
			}
			case 1: 
			{
				m_picture_style = CAL_PICTURE_LEFT;
				MetaName += "L";
				break;
			}
			case 2:
			{
				m_picture_style = CAL_PICTURE_SAME;
				MetaName += "S";
				break;
			}
		};
	}
	else
	{
		m_picture_style = CAL_PICTURE_NONE;
		MetaName += "N";
	}

	switch (m_orientation)
	{
		case PORTRAIT:
			{
				MetaName += "P";
				break;
			}
		case LANDSCAPE:
			{
				MetaName += "L";
				break;
			}
	};

	// add suffix
	MetaName += "_BM";

	m_Preview.UpdateCBitmap (MetaName);
	m_Preview.Invalidate();
}	



VOID CCalendarPictureDlg::enable_child(UINT id, BOOL enable)
{
	CWnd *child;

	if ((child = GetDlgItem(id)) != NULL)
	{
		//child->ShowWindow(enable ? SW_SHOW : SW_HIDE);
		child->EnableWindow(enable);
	}
}

void CCalendarPictureDlg::FixControls()
{
	UpdateData(TRUE);
	BOOL bEnable = m_iPicture == 0;
	enable_child(IDC_PICTURE_SIDE, bEnable);
	enable_child(IDC_PICTURE_ABOVE, bEnable);
	enable_child(IDC_PICTURE_SAME, bEnable);
}

void CCalendarPictureDlg::OnPictureSame() 
{
	UpdateCBitmap();	
}
