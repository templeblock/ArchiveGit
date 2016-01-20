// expirat.h : header file
//

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CExpirationDateDialog dialog

class CExpirationDateDialog : public CBaseDialog
{
// Construction
public:
	CExpirationDateDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CExpirationDateDialog)
	enum { IDD = IDD_EXPIRATION_DATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CExpirationDateDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
