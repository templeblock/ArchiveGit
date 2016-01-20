// OnAdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "OnAdDlg.h"
#include "pdlgclr.h"
#include "ddb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DLGBORDER 0
#define DLGBUTTONBORDER 6

extern CPalette *pOurPal;


/////////////////////////////////////////////////////////////////////////////
// COnlineAdvertisementDlg dialog


COnlineAdvertisementDlg::COnlineAdvertisementDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnlineAdvertisementDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COnlineAdvertisementDlg)
	m_strAdText = _T("");
	//}}AFX_DATA_INIT
	m_pddb = NULL;
}

COnlineAdvertisementDlg::~COnlineAdvertisementDlg()
{
}

void COnlineAdvertisementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnlineAdvertisementDlg)
	DDX_Control(pDX, IDC_ADIMAGE, m_ctrlPicture);
	DDX_Control(pDX, IDC_ADTEXT, m_ctrlAdText);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Text(pDX, IDC_ADTEXT, m_strAdText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COnlineAdvertisementDlg, CDialog)
	//{{AFX_MSG_MAP(COnlineAdvertisementDlg)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnlineAdvertisementDlg message handlers

void COnlineAdvertisementDlg::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}

void COnlineAdvertisementDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

BOOL COnlineAdvertisementDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_strAdText);
	m_ctrlAdText.ShowWindow(SW_HIDE);
	m_ctrlAdText.EnableWindow(FALSE);  //FOR NOW
	
	m_ctrlPicture.ShowWindow(SW_HIDE);

	if(!m_fURLLink)
		m_btnOK.ShowWindow(SW_HIDE);

	char s[MAX_PATH];
	lstrcpy(s, (LPCTSTR)m_strbm);


	// Initialize the picture preview
	m_cppAdImage.Initialize(this, IDC_ADIMAGE, NULL);
	
   //m_cppAdImage.NewGraphicPreview(GetGlobalPathManager()->LocatePath(m_strbm));
   m_cppAdImage.NewGraphicPreview(m_strbm);
	
	ChangeDimensions();

	// Initialize the picture preview
	m_cppAdImage.Initialize(this, IDC_ADIMAGE, NULL);
	
   //m_cppAdImage.NewGraphicPreview(GetGlobalPathManager()->LocatePath(m_strbm));
   m_cppAdImage.NewGraphicPreview(m_strbm);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COnlineAdvertisementDlg::ChangeDimensions()
{
	PBOX Bounds;

	if (m_cppAdImage.GetImageSize(&Bounds))
	{

		ResizeDlg(&Bounds);

	}
}

void COnlineAdvertisementDlg::ResizeDlg(PBOX* pbx)
{
	RECT rDlg;

	CSize cs(0, 0);
	GetWindowRect(&rDlg);

	CPoint ptBorder(DLGBORDER, DLGBORDER);
	CPoint ptButtonBorder(DLGBUTTONBORDER, DLGBUTTONBORDER);

	CRect OKRect;
	m_btnOK.GetWindowRect(&OKRect);
	ScreenToClient(&OKRect);

	CRect CancelRect;
	m_btnCancel.GetWindowRect(&CancelRect);
	ScreenToClient(&CancelRect);

	CRect PictureRect;
	m_ctrlPicture.GetWindowRect(&PictureRect);
	ScreenToClient(&PictureRect);

	CDC* pDC = GetDC();
	
	cs.cx = pbx->Width() *pDC->GetDeviceCaps(LOGPIXELSX)/1800;
	cs.cy = pbx->Height() *pDC->GetDeviceCaps(LOGPIXELSY)/1800 + (2 * ptButtonBorder.y) + OKRect.Height();

	ReleaseDC(pDC);

	//define minimum size of our sizing window
	int minWidth = CancelRect.right - OKRect.left + (2 * ptButtonBorder.x);
	int minHeight = OKRect.bottom - OKRect.top + (2 * ptButtonBorder.y);

	//make it wrap the bitmap if larger than minimum size
	int DlgWidth = max(minWidth, cs.cx + (2 * (GetSystemMetrics(SM_CXFIXEDFRAME ) + ptBorder.x)));
	int DlgHeight = max(minHeight, cs.cy + (2 * ptBorder.y) - (2 * GetSystemMetrics(SM_CYBORDER)) + (GetSystemMetrics(SM_CYMAXIMIZED) - GetSystemMetrics(SM_CYFULLSCREEN)));

	//wrap the window around the bitmap
	MoveWindow(rDlg.left, rDlg.top, DlgWidth, DlgHeight, TRUE);

	//Get CLIENT width
	int DlgClientWidth = DlgWidth - (2 * (GetSystemMetrics(SM_CXFIXEDFRAME ) + ptBorder.x));
	int DlgClientHeight = DlgHeight - (2 * ptBorder.y) + (2 * GetSystemMetrics(SM_CYBORDER)) - (GetSystemMetrics(SM_CYMAXIMIZED) - GetSystemMetrics(SM_CYFULLSCREEN));

	//wrap the window around the bitmap
	if((pbx->Width() *pDC->GetDeviceCaps(LOGPIXELSX)/1800) < minWidth)
		m_ctrlPicture.MoveWindow(((DlgClientWidth - (pbx->Width() *pDC->GetDeviceCaps(LOGPIXELSX)/1800)) / 2), 0, pbx->Width() *pDC->GetDeviceCaps(LOGPIXELSX)/1800, pbx->Height() *pDC->GetDeviceCaps(LOGPIXELSY)/1800, TRUE);
	else
		m_ctrlPicture.MoveWindow(0, 0, pbx->Width() *pDC->GetDeviceCaps(LOGPIXELSX)/1800, pbx->Height() *pDC->GetDeviceCaps(LOGPIXELSY)/1800, TRUE);

	//center the buttons
	int leftOrigin = 0;
	if(!m_fURLLink)
	{
		//only a Cancel button to center
		leftOrigin = ((DlgClientWidth - (CancelRect.Width())) / 2);
		m_btnCancel.MoveWindow(leftOrigin, cs.cy - ptButtonBorder.y - CancelRect.Height(), CancelRect.Width(), CancelRect.Height());
	}
	else
	{
		//center the Cancel and Continue button
		leftOrigin = ((DlgClientWidth - (CancelRect.right - OKRect.left)) / 2);
		m_btnOK.MoveWindow(leftOrigin, cs.cy - ptButtonBorder.y - OKRect.Height(), OKRect.Width(), OKRect.Height());

		leftOrigin += (CancelRect.left - OKRect.right) + OKRect.Width();
		m_btnCancel.MoveWindow(leftOrigin, cs.cy - ptButtonBorder.y - CancelRect.Height(), CancelRect.Width(), CancelRect.Height());
	}
}

void COnlineAdvertisementDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{

	if (nIDCtl == IDC_ADIMAGE)
   {
		m_cppAdImage.DrawPreview(lpDrawItemStruct);
   
	}
   else
   	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

