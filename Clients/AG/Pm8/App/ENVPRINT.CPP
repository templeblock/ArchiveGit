/*
// $Workfile: ENVPRINT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ENVPRINT.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:18   Fred
// Initial revision.
// 
//    Rev 1.1   30 Jul 1997 15:12:10   Fred
// Sets default reverse landscape
// 
//    Rev 1.0   14 Mar 1996 13:33:18   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 10:28:22   Jay
// Changes from 2.1
// 
//    Rev 1.8   23 Feb 1996 08:55:34   JAY
// Fixed activation problem in 16-bit version.
// 
//    Rev 1.7   20 Nov 1995 13:43:54   JAY
// 
//    Rev 1.6   20 Nov 1995 13:33:50   JAY
// Enables the "Default" button if the reverse landscape setting is not default.
// 
//    Rev 1.5   20 Nov 1995 11:18:10   JAY
// Nre reverse landscape check
// 
//    Rev 1.4   06 Oct 1995 11:59:56   JAY
// New print code, etc.
// 
//    Rev 1.3   10 Aug 1995 13:22:40   JAY
//  
// 
//    Rev 1.2   09 Aug 1995 13:11:26   JAY
// Misc changes.
// 
//    Rev 1.1   08 Aug 1995 11:01:40   JAY
// Envelope stuff.
// 
//    Rev 1.0   28 Jul 1995 12:29:38   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "envprvw.h"
#include "envprint.h"
#include "printer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePrintDialog dialog

CEnvelopePrintDialog::CEnvelopePrintDialog(CPmwView* pView)
	: CPmwPrint(pView, NULL, CEnvelopePrintDialog::IDD)
{
	//{{AFX_DATA_INIT(CEnvelopePrintDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CEnvelopePrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwPrint::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnvelopePrintDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEnvelopePrintDialog, CPmwPrint)
	//{{AFX_MSG_MAP(CEnvelopePrintDialog)
	ON_BN_CLICKED(IDC_POSITION, OnPosition)
	ON_BN_CLICKED(IDC_POSITION_PREVIEW, OnPosition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePrintDialog message handlers

BOOL CEnvelopePrintDialog::OnInitDialog()
{
	CPmwPrint::OnInitDialog();

	/*
	// Use GetDlgItem() to get our preview window. GetDlgItem normally returns
	// (or can return) a temporary object; it's not too smart to save a pointer
	// to it. However, in this case, we know the class auto-creates an MFC
	// object; the object physically exists as a C++ object, and we are safe
	// to save a pointer to it because we know it's not temporary.
	*/

	m_pPreview = (CEnvelopePreview*)GetDlgItem(IDC_POSITION_PREVIEW);
	UpdatePreview();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEnvelopePrintDialog::ChangedPrinter(void)
{
	UpdatePreview();
}

void CEnvelopePrintDialog::UpdatePreview(void)
{
	ASSERT(m_pPreview != NULL);
	if (m_pPreview != NULL)
	{
		CPrinterInfo* pInfo = GET_PMWAPP()->GetPrinterInfo(PORTRAIT);
		WORD wEnvelopePosition = pInfo->GetEnvelopePosition();

		if (m_pPreview->GetEnvelopePosition() != wEnvelopePosition)
		{
			m_pPreview->SetTractor(pInfo->Tractor() == TRUE);
			m_pPreview->SetEnvelopePosition(wEnvelopePosition);
			m_pPreview->Invalidate();
		}
	}
}

void CEnvelopePrintDialog::OnPosition()
{
	CPmwApp* pApp = GET_PMWAPP();
	CPrinterInfo* pInfo = pApp->GetPrinterInfo(PORTRAIT);

	CEnvelopePositionDialog Dialog;

	Dialog.SetTractor(pInfo->Tractor() == TRUE);
	Dialog.SetEnvelopePosition(m_pPreview->GetEnvelopePosition());
	Dialog.SetDefaultPosition(pInfo->GetDefaultEnvelopePosition());
	Dialog.SetReverseLandscape(pInfo->ReverseLandscape());
	Dialog.SetDefaultReverseLandscape(pInfo->GetDefaultReverseLandscape());

	if (Dialog.DoModal() == IDOK)
	{
		WORD wPosition = Dialog.GetEnvelopePosition();
		BOOL fReverseLandscape = Dialog.GetReverseLandscape();

		m_pPreview->SetEnvelopePosition(wPosition);
		m_pPreview->Invalidate();

		pInfo->SetEnvelopePosition(wPosition);
		pInfo->ReverseLandscape(fReverseLandscape);
		pInfo->WriteSettings();
		
		pApp->GetPrinterInfo(LANDSCAPE)->SetEnvelopePosition(wPosition);
		pApp->GetPrinterInfo(LANDSCAPE)->ReverseLandscape(fReverseLandscape);

		pApp->update_paper_info();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePositionDialog dialog

static struct
{
	WORD m_wID;
	WORD m_wBits;
} PreviewInfo[] =
{
	{ IDC_POSITION_PREVIEW_LT, ENVELOPE_Left | ENVELOPE_Tall},
	{ IDC_POSITION_PREVIEW_CT, ENVELOPE_Center | ENVELOPE_Tall},
	{ IDC_POSITION_PREVIEW_RT, ENVELOPE_Right | ENVELOPE_Tall},
	{ IDC_POSITION_PREVIEW_LW, ENVELOPE_Left | ENVELOPE_Wide},
	{ IDC_POSITION_PREVIEW_CW, ENVELOPE_Center | ENVELOPE_Wide},
	{ IDC_POSITION_PREVIEW_RW, ENVELOPE_Right | ENVELOPE_Wide}
};

CEnvelopePositionDialog::CEnvelopePositionDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CEnvelopePositionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnvelopePositionDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_wEnvelopeBase = 0;
	m_wEnvelopePosition = 0;
	m_nSelectedPreview = -1;
	m_wDefaultPosition = DEFAULT_ENVELOPE_POSITION;

	m_fReverseLandscape = -1;
	m_fDefaultReverseLandscape = FALSE;

	m_fTractor = FALSE;
	memset(m_pPreview, 0, sizeof(m_pPreview));
}

void CEnvelopePositionDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnvelopePositionDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEnvelopePositionDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CEnvelopePositionDialog)
	ON_BN_CLICKED(IDC_FLAP_DOWN, OnFlapDown)
	ON_BN_CLICKED(IDC_FLAP_UP, OnFlapUp)
	ON_BN_CLICKED(IDC_ROTATED, OnRotated)
	ON_BN_CLICKED(IDC_POSITION_PREVIEW_LT, OnClickedPreviewLT)
	ON_BN_CLICKED(IDC_POSITION_PREVIEW_CT, OnClickedPreviewCT)
	ON_BN_CLICKED(IDC_POSITION_PREVIEW_RT, OnClickedPreviewRT)
	ON_BN_CLICKED(IDC_POSITION_PREVIEW_LW, OnClickedPreviewLW)
	ON_BN_CLICKED(IDC_POSITION_PREVIEW_CW, OnClickedPreviewCW)
	ON_BN_CLICKED(IDC_POSITION_PREVIEW_RW, OnClickedPreviewRW)
	ON_BN_DOUBLECLICKED(IDC_POSITION_PREVIEW_LT, OnDblClickedPreviewLT)
	ON_BN_DOUBLECLICKED(IDC_POSITION_PREVIEW_CT, OnDblClickedPreviewCT)
	ON_BN_DOUBLECLICKED(IDC_POSITION_PREVIEW_RT, OnDblClickedPreviewRT)
	ON_BN_DOUBLECLICKED(IDC_POSITION_PREVIEW_LW, OnDblClickedPreviewLW)
	ON_BN_DOUBLECLICKED(IDC_POSITION_PREVIEW_CW, OnDblClickedPreviewCW)
	ON_BN_DOUBLECLICKED(IDC_POSITION_PREVIEW_RW, OnDblClickedPreviewRW)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	ON_BN_CLICKED(IDC_REVERSE_LANDSCAPE, OnReverseLandscape)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvelopePositionDialog message handlers

BOOL CEnvelopePositionDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	/*
	// Use GetDlgItem() to get our preview window. GetDlgItem normally returns
	// (or can return) a temporary object; it's not too smart to save a pointer
	// to it. However, in this case, we know the class auto-creates an MFC
	// object; the object physically exists as a C++ object, and we are safe
	// to save a pointer to it because we know it's not temporary.
	*/

	for (int i = 0; i < 6; i++)
	{
		m_pPreview[i] = (CEnvelopePreview*)GetDlgItem(PreviewInfo[i].m_wID);
	}

	ShowPosition();

	GetDlgItem(IDOK)->SetFocus();

	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CEnvelopePositionDialog::ShowPosition(void)
{
	/*
	// Strip out the orientation and horizontal alignment bits.
	// They get added back in on a per-preview basis.
	*/

	m_wEnvelopeBase = m_wEnvelopePosition & ~(ENVELOPE_OMASK | ENVELOPE_HMASK);

	UpdatePreview(FALSE);

	/*
	// Compute the preview to select.
	*/

	int nIndex = 0;
	if ((m_wEnvelopePosition & ENVELOPE_OMASK) == ENVELOPE_Wide)
	{
		nIndex += 3;
	}
	switch (m_wEnvelopePosition & ENVELOPE_HMASK)
	{
		case ENVELOPE_Right:
		{
			nIndex++;
		}
		case ENVELOPE_Center:
		{
			nIndex++;
		}
		default:
		{
			break;
		}
	}

	SelectPreview(nIndex);

	BOOL fFlapUp = (m_wEnvelopePosition & ENVELOPE_FMASK) == ENVELOPE_FlapUp;

	CheckDlgButton(IDC_FLAP_UP, fFlapUp);
	CheckDlgButton(IDC_FLAP_DOWN, !fFlapUp);

	CheckDlgButton(IDC_ROTATED,
						((m_wEnvelopePosition & ENVELOPE_RMASK) == ENVELOPE_RRotate));

	CheckDlgButton(IDC_REVERSE_LANDSCAPE, m_fReverseLandscape);
}

void CEnvelopePositionDialog::OnOK()
{
	CPmwDialog::OnOK();
}

void CEnvelopePositionDialog::UpdatePreview(BOOL fUpdatePosition /*=TRUE*/)
{
	for (int i = 0; i < 6; i++)
	{
		CEnvelopePreview* pPreview = m_pPreview[i];
		ASSERT(pPreview != NULL);
		if (pPreview != NULL)
		{
			pPreview->SetTractor(m_fTractor);
			WORD wEnvelopePosition = m_wEnvelopeBase | PreviewInfo[i].m_wBits;
			if (pPreview->GetEnvelopePosition() != wEnvelopePosition)
			{
				pPreview->SetEnvelopePosition(wEnvelopePosition);
				pPreview->Invalidate();
			}
		}
	}

	if (fUpdatePosition)
	{
		UpdatePosition();
	}
}

void CEnvelopePositionDialog::UpdatePosition()
{
	if (m_nSelectedPreview != -1)
	{
		m_wEnvelopePosition = m_pPreview[m_nSelectedPreview]->GetEnvelopePosition();
	}

	CWnd* pWnd;
	if ((pWnd = GetDlgItem(IDC_DEFAULT)) != NULL)
	{
		pWnd->EnableWindow(m_wEnvelopePosition != m_wDefaultPosition
								 || m_fReverseLandscape != m_fDefaultReverseLandscape);
	}
}

void CEnvelopePositionDialog::OnFlapDown()
{
	m_wEnvelopeBase &= ~ENVELOPE_FMASK;
	m_wEnvelopeBase |= ENVELOPE_FlapDown;
	UpdatePreview();
}

void CEnvelopePositionDialog::OnFlapUp()
{
	m_wEnvelopeBase &= ~ENVELOPE_FMASK;
	m_wEnvelopeBase |= ENVELOPE_FlapUp;
	UpdatePreview();
}

void CEnvelopePositionDialog::OnRotated()
{
	m_wEnvelopeBase ^= ENVELOPE_RMASK;
	UpdatePreview();
}

void CEnvelopePositionDialog::OnClickedPreviewLT()
{
	SelectPreview(0);
}

void CEnvelopePositionDialog::OnClickedPreviewCT()
{
	SelectPreview(1);
}

void CEnvelopePositionDialog::OnClickedPreviewRT()
{
	SelectPreview(2);
}

void CEnvelopePositionDialog::OnClickedPreviewLW()
{
	SelectPreview(3);
}

void CEnvelopePositionDialog::OnClickedPreviewCW()
{
	SelectPreview(4);
}

void CEnvelopePositionDialog::OnClickedPreviewRW()
{
	SelectPreview(5);
}

void CEnvelopePositionDialog::OnDblClickedPreviewLT()
{
	SelectPreview(0, TRUE);
}

void CEnvelopePositionDialog::OnDblClickedPreviewCT()
{
	SelectPreview(1, TRUE);
}

void CEnvelopePositionDialog::OnDblClickedPreviewRT()
{
	SelectPreview(2, TRUE);
}

void CEnvelopePositionDialog::OnDblClickedPreviewLW()
{
	SelectPreview(3, TRUE);
}

void CEnvelopePositionDialog::OnDblClickedPreviewCW()
{
	SelectPreview(4, TRUE);
}

void CEnvelopePositionDialog::OnDblClickedPreviewRW()
{
	SelectPreview(5, TRUE);
}

void CEnvelopePositionDialog::SelectPreview(int nPreview, BOOL fDblClick /*=FALSE*/)
{
	if (m_nSelectedPreview != nPreview)
	{
		if (m_nSelectedPreview != -1)
		{
			m_pPreview[m_nSelectedPreview]->Select(FALSE);
		}
		m_nSelectedPreview = nPreview;
		if (m_nSelectedPreview != -1)
		{
			m_pPreview[m_nSelectedPreview]->Select(TRUE);
		}

		UpdatePosition();
	}

	if (fDblClick)
	{
		OnOK();
	}
}

void CEnvelopePositionDialog::OnDefault()
{
	m_wEnvelopePosition = m_wDefaultPosition;
	m_fReverseLandscape = m_fDefaultReverseLandscape;
	ShowPosition();
}

void CEnvelopePositionDialog::OnReverseLandscape()
{
	m_fReverseLandscape = !m_fReverseLandscape;
	UpdatePreview();
}
