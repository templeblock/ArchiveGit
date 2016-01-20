// NumberEdit.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "NumberEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumberEdit

CNumberEdit::CNumberEdit():
	m_bNumOnly(TRUE)
{
}

CNumberEdit::~CNumberEdit()
{
}


BEGIN_MESSAGE_MAP(CNumberEdit, CEdit)
	//{{AFX_MSG_MAP(CNumberEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumberEdit message handlers

void CNumberEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(m_bNumOnly)
	{
		if(nChar >= '0' && nChar <= '9'	|| nChar == VK_BACK || nChar == '-')
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else
		CEdit::OnChar(nChar, nRepCnt, nFlags);

}
