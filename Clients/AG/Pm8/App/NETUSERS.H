// netusers.h : header file
//

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNetworkUsersDialog dialog

class CNetworkUsersDialog : public CPmwDialog
{
// Construction
public:
	CNetworkUsersDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNetworkUsersDialog)
	enum { IDD = IDD_NETWORK_USERS };
	CListBox	m_user_list;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNetworkUsersDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
