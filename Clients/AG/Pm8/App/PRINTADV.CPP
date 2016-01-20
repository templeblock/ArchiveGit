// printadv.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "printadv.h"
#include "printer.h"
#include "testprnt.h"

#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDlg dialog

CAdvancedDlg::CAdvancedDlg(int outline_gamma, int bitmap_gamma, BOOL fPrintAsBitmap, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CAdvancedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvancedDlg)
	m_fPrintAsBitmap = FALSE;
	//}}AFX_DATA_INIT
	m_outline_gamma = outline_gamma;
	m_bitmap_gamma = bitmap_gamma;
	m_fPrintAsBitmap = fPrintAsBitmap;
}

void CAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedDlg)
	DDX_Control(pDX, IDC_OUTLINE_CONTRAST_NUMBER, m_outline_contrast);
	DDX_Control(pDX, IDC_CONTRAST_NUMBER, m_bitmap_contrast);
	DDX_Check(pDX, IDC_PRINT_AS_BITMAP_CHECK, m_fPrintAsBitmap);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAdvancedDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CAdvancedDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_PRINTER_ALIGNMENT, OnPrinterAlignment)
	ON_BN_CLICKED(IDC_PRINT_AS_BITMAP_CHECK, OnPrintAsBitmapCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDlg message handlers

VOID CAdvancedDlg::update_bitmap_contrast()
{
	char buffer[20];

	itoa(m_bitmap_gamma, buffer, 10);
	m_bitmap_contrast.SetWindowText(buffer);
}

VOID CAdvancedDlg::update_outline_contrast()
{
	char buffer[20];

	itoa(m_outline_gamma, buffer, 10);
	m_outline_contrast.SetWindowText(buffer);
}

BOOL CAdvancedDlg::OnInitDialog()
{
/* Setup the scroll bar. */

	CScrollBar *scroll;

	if ((scroll = (CScrollBar *)GetDlgItem(IDC_PRINT_CONTRAST)) != NULL)
	{
		scroll->SetScrollRange(100, 1200, FALSE);
		scroll->SetScrollPos(m_bitmap_gamma, FALSE);
	}

	if ((scroll = (CScrollBar *)GetDlgItem(IDC_PRINT_OUTLINE_CONTRAST)) != NULL)
	{
		scroll->SetScrollRange(100, 1200, FALSE);
		scroll->SetScrollPos(m_outline_gamma, FALSE);
	}

	CPmwDialog::OnInitDialog();

	update_bitmap_contrast();
	update_outline_contrast();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAdvancedDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int cur_pos;
	int new_pos = cur_pos = pScrollBar->GetScrollPos();
	int min, max;
	pScrollBar->GetScrollRange(&min, &max);

	switch (nSBCode)
	{
		case SB_TOP:
		{
			new_pos = min;
			break;
		}

		case SB_BOTTOM:
		{
			new_pos = max;
			break;
		}
		
		case SB_LINEUP:
		{
			new_pos--;
			break;
		}

		case SB_LINEDOWN:
		{
			new_pos++;
			break;
		}

		case SB_PAGEUP:
		{
			new_pos -= 50;
			break;
		}

		case SB_PAGEDOWN:
		{
			new_pos += 50;
			break;
		}

#if 1
		case SB_THUMBTRACK:
		{
			new_pos = nPos;
			break;
		}
#endif

		default:        // ignore other notifications
		{
			return;
		}
	}

	if (new_pos != cur_pos)
	{
		pScrollBar->SetScrollPos(new_pos);
	/* Re-read the scroll position in case it got clipped to limits. */
		switch (pScrollBar->GetDlgCtrlID())
		{
			case IDC_PRINT_CONTRAST:
			{
				m_bitmap_gamma = pScrollBar->GetScrollPos();
				update_bitmap_contrast();
				break;
			}
			case IDC_PRINT_OUTLINE_CONTRAST:
			{
				m_outline_gamma = pScrollBar->GetScrollPos();
				update_outline_contrast();
				break;
			}
			default:
			{
				break;
			}
		}
	}
//	od("Code: %d, nPos:%d, new:%d (%lx)\r\n", nSBCode, nPos, new_pos, pScrollBar);
	CPmwDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAdvancedDlg::OnPrinterAlignment()
{
	CPrinterAlignmentTestDialog Dialog(this);
	
	if (Dialog.DoModal() == IDOK)
	{
		CPmwApp* pApp = GET_PMWAPP();
		if (pApp != NULL)
		{
			CPrinterInfo* pPrinterInfo;
			if ((pPrinterInfo = pApp->GetPrinterInfo(PORTRAIT)) != NULL)
			{
				pPrinterInfo->WriteSettings();
			}
			if ((pPrinterInfo = pApp->GetPrinterInfo(LANDSCAPE)) != NULL)
			{
				pPrinterInfo->WriteSettings();
			}
			pApp->update_paper_info();
		}
	}
}

void CAdvancedDlg::OnPrintAsBitmapCheck() 
{
	CPmwApp* pApp = GET_PMWAPP();
	if (pApp != NULL)
	{
		CPrinterInfo* pPrinterInfo;
		
		if ((pPrinterInfo = pApp->GetPrinterInfo(PORTRAIT)) != NULL)
		{
			pPrinterInfo->WriteSettings();
		}
		if ((pPrinterInfo = pApp->GetPrinterInfo(LANDSCAPE)) != NULL)
		{
			pPrinterInfo->WriteSettings();
		}
		pApp->update_paper_info();
	}
	
}
