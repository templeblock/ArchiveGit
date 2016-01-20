// ordfail.h : header file
//

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// COrderFailedDialog dialog

class COrderFailedDialog : public CBaseDialog
{
// Construction
public:
	COrderFailedDialog(LPCSTR pszReason, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(COrderFailedDialog)
	enum { IDD = IDD_ORDER_FAILED };
	CString	m_csReason;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(COrderFailedDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
