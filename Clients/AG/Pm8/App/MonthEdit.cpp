// MonthEdit.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "MonthEdit.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonthEdit

CMonthEdit::CMonthEdit()
{
}

CMonthEdit::~CMonthEdit()
{
}


BEGIN_MESSAGE_MAP(CMonthEdit, CEdit)
	//{{AFX_MSG_MAP(CMonthEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonthEdit message handlers

void CMonthEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMonthEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CEdit::OnChar(nChar, nRepCnt, nFlags);
}


void CMonthEdit::OnChange() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEdit::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	CString strText;
	GetWindowText(strText);
	int nMonth = atoi(strText);
	if(nMonth)
	{
		CString strTemp;
#ifdef LOCALIZE
		strTemp = CurrentLocale.AbbrevMonthName((MONTH)(nMonth-1));
		strTemp.MakeUpper();
#else
		strTemp.LoadString(IDS_MONTH1 + nMonth - 1);
#endif
		SetWindowText(strTemp);
	}

}
