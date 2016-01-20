/*
// $Workfile: LINEWID.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LINEWID.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:04   Fred
// Initial revision.
// 
//    Rev 1.1   06 Nov 1996 13:49:04   johno
// Moved strings to resource DLL
// 
//    Rev 1.0   14 Mar 1996 13:32:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:16   FRED
// Initial revision.
// 
//    Rev 1.2   28 Aug 1995 15:26:04   FRED
// None/NoNone cases for line width selection.
// 
//    Rev 1.1   19 Aug 1995 17:49:30   FRED
// Maximum line sizes
// 
//    Rev 1.0   11 May 1995 08:51:54   FRED
// Initial revision.
*/ 


#include "stdafx.h"
#include "pmw.h"
#include "linewid.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define IDM_VALIDATE_LINE_WIDTH	(WM_USER+100)

/////////////////////////////////////////////////////////////////////////////
// CChooseLineWidth dialog


CChooseLineWidthDialog::CChooseLineWidthDialog(int nLineWidth, int nMaximumLineWidth, BOOL fAllowNone /*=TRUE*/, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CChooseLineWidthDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseLineWidthDialog)
	//}}AFX_DATA_INIT
	
	m_nLineWidth = nLineWidth;
	m_nMaximumLineWidth = nMaximumLineWidth;
	m_fLineWidthChanged = FALSE;
	m_fAllowNone = fAllowNone;
}

void CChooseLineWidthDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseLineWidthDialog)
	DDX_Control(pDX, IDC_LINE_WIDTH, m_LineWidthEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChooseLineWidthDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CChooseLineWidthDialog)
	ON_WM_ACTIVATE()
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_LINE_WIDTH, OnChangeLineWidth)
	ON_EN_KILLFOCUS(IDC_LINE_WIDTH, OnKillfocusLineWidth)
	ON_MESSAGE(IDM_VALIDATE_LINE_WIDTH, OnValidateLineWidth)
	ON_EN_SETFOCUS(IDC_LINE_WIDTH, OnSetfocusLineWidth)
	ON_WM_PARENTNOTIFY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChooseLineWidth message handlers

void CChooseLineWidthDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
}

void CChooseLineWidthDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nWidth = m_nLineWidth;

	if (nSBCode == SB_LINEUP)
	{
		nWidth++;
	}

	if (nSBCode == SB_LINEDOWN)
	{
		nWidth--;
	}

	if (nWidth < -1)
	{
		nWidth = -1;
	}

	if (nWidth > m_nMaximumLineWidth)
	{
		nWidth = m_nMaximumLineWidth;
	}

	if (m_nLineWidth != nWidth)
	{
		Width(nWidth);

		if (GetFocus() == GetDlgItem(IDC_LINE_WIDTH))
		{
			m_LineWidthEdit.SetSel(0, -1);
		}
	}

	CPmwDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CChooseLineWidthDialog::OnParentNotify(UINT message, LPARAM lParam)
{
	CPmwDialog::OnParentNotify(message, lParam);

	if (message == WM_LBUTTONDOWN)
	{
		CRect crCancelButton;

		((CButton *)GetDlgItem(IDCANCEL))->GetWindowRect(&crCancelButton);

		CPoint pCursor((DWORD)lParam);
		ClientToScreen(&pCursor);

		if (crCancelButton.PtInRect(pCursor))
		{
			m_fLineWidthChanged = FALSE;
		}
	}
}

void CChooseLineWidthDialog::OnOK()
{
	m_fLineWidthChanged = FALSE;
	
	if (OnValidateLineWidth(0, 0L))
	{
		CPmwDialog::OnOK();
	}
}

void CChooseLineWidthDialog::OnChangeLineWidth()
{
	m_fLineWidthChanged = TRUE;
}

BOOL CChooseLineWidthDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

	m_LineWidthEdit.LimitText(8);
	Width(m_nLineWidth);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChooseLineWidthDialog::OnKillfocusLineWidth()
{
	if (m_fLineWidthChanged)
	{
		PostMessage(IDM_VALIDATE_LINE_WIDTH, 0, 0L);
	}
}

LRESULT CChooseLineWidthDialog::OnValidateLineWidth(WPARAM wParam, LPARAM lParam)
{
	CString csWidth;
	CString csNone;
	CString csHairline;
	CString csFormat;
	CString csMessage;

	LRESULT lResult = 0;

	TRY
	{
		BOOL fIsValid = TRUE;
		
		// JRO
      //csNone = "None";
		//csHairline = "Hairline";
      csNone = GET_PMWAPP()->GetResourceStringPointer (IDS_NONE);
		csHairline = GET_PMWAPP()->GetResourceStringPointer (IDS_HAIRLINE);
		
		// get the value that the user entered
		
		m_LineWidthEdit.GetWindowText(csWidth);
		
		if (m_fAllowNone && (csWidth.CompareNoCase(csNone.Left(csWidth.GetLength())) == 0))
		{
			m_nLineWidth = -1;
		}
		else if (csWidth.CompareNoCase(csHairline.Left(csWidth.GetLength())) == 0)
		{
			m_nLineWidth = 0;
		}
		else
		{
			fIsValid = Util::ValidUnsignedNumber(csWidth, 1, m_nMaximumLineWidth);
			m_nLineWidth = atoi(csWidth);
			
			if (!fIsValid)
			{
				if (m_nLineWidth < 1)
				{
					m_nLineWidth = 1;
				}
					
				if (m_nLineWidth > m_nMaximumLineWidth)
				{
					m_nLineWidth = m_nMaximumLineWidth;
				}
			}
		}
		
		if (!fIsValid)
		{
			// the number entered was NOT valid
			AfxMessageBox(Util::FormatString(csMessage, MAKEINTRESOURCE(m_fAllowNone ? IDS_ErrInvalidLineWidth : IDS_ErrInvalidLineWidthNoNone), m_nMaximumLineWidth), MB_OK|MB_ICONEXCLAMATION);
		}
		else
		{
			// the number entered was valid
			lResult = 1;
		}
	
		Width(m_nLineWidth);
	}
	CATCH_ALL(e)
	{
		csWidth.Empty();
		csNone.Empty();
		csHairline.Empty();

		THROW_LAST();
	}
	END_CATCH_ALL

	return lResult;
}

void CChooseLineWidthDialog::Width(int nWidth)
{
	m_nLineWidth = nWidth;
	
	if (nWidth < 0)
	{
		// JRO m_LineWidthEdit.SetWindowText("None");
      m_LineWidthEdit.SetWindowText(GET_PMWAPP()->GetResourceStringPointer (IDS_NONE));
	}
	else if (m_nLineWidth == 0)
	{
		// JRO m_LineWidthEdit.SetWindowText("Hairline");
      m_LineWidthEdit.SetWindowText(GET_PMWAPP()->GetResourceStringPointer (IDS_HAIRLINE));
	}
	else
	{
		char szWidth[10];
		_itoa(nWidth, szWidth, 10);
		m_LineWidthEdit.SetWindowText(szWidth);
	}
	
	m_LineWidthEdit.Invalidate();
}


void CChooseLineWidthDialog::OnSetfocusLineWidth()
{
	m_LineWidthEdit.SetSel(0, -1);
}
