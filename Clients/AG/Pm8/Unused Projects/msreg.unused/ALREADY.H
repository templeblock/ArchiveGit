// already.h : header file
//

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAlreadyRegisteredDialog dialog

class CAlreadyRegisteredDialog : public CBaseDialog
{
// Construction
public:
	CAlreadyRegisteredDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlreadyRegisteredDialog)
	enum { IDD = IDD_ALREADY_REGISTERED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlreadyRegisteredDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlreadyRegisteredDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
