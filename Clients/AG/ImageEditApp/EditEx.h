#pragma once

class CEditEx : public CEdit
{
public:
	CEditEx();
	virtual ~CEditEx();

protected:
	DECLARE_DYNAMIC(CEditEx)
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT CtlColorEdit(WPARAM wParam, LPARAM lParam); 
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor); 
};
