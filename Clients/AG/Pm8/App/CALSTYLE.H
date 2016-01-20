// calstyle.h : header file
//

#include "calinfo.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarStyleDlg dialog

class CCalendarStyleDlg : public CPmwDialog
{
// Construction
public:
	CCalendarStyleDlg(CALENDAR_INFO_REF info, CPmwDoc *pDoc, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCalendarStyleDlg)
	enum { IDD = IDD_CALENDAR_STYLE };
	int		m_shadow;
	int		m_lines;
	int		m_spelling;
	int		m_background;
	BOOL	m_outline;
	//}}AFX_DATA

	CALENDAR_TYPE m_type;
	COLOR m_color;
	CPmwDoc *m_document;

// Implementation

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void vars_from_info(CALENDAR_INFO_REF info);

	// Generated message map functions
	//{{AFX_MSG(CCalendarStyleDlg)
	afx_msg void OnCalstyleDefault();
	afx_msg void OnCalstyleColor();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL get_exit_info(CALENDAR_INFO_REF info);
};
