#include "stdafx.h"
#include "EditEx.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CEditEx, CEdit)
BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_MESSAGE(OCM_CTLCOLOREDIT, CtlColorEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CEditEx::CEditEx()
{
}

/////////////////////////////////////////////////////////////////////////////
CEditEx::~CEditEx()
{
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CEditEx::CtlColorEdit(WPARAM wParam, LPARAM lParam) 
{
	return DefWindowProc(WM_CTLCOLOREDIT, wParam, lParam);
//	case WM_CTLCOLOREDIT:
//	lResult = GetParent().SendMessage(uMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	//set text color
//	pDC->SetTextColor(m_crText);
	//set the text's background color
//	pDC->SetBkColor(m_crBackGnd);

	//return the brush used for background this sets control background
//	return m_brBackGnd;
   	return CEdit::OnCtlColor(pDC, this, nCtlColor);
}
