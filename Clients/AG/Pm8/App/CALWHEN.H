// calwhen.h : header file
//

#include "pdlgclr.h"

/////////////////////////////////////////////////////////////////////////////
// CCalendarWhenDlg dialog

class CCalendarWhenDlg : public CPmwDialogColor
{
private:
	VOID enable_child(UINT id, BOOL enable);
	VOID update_week_selection(BOOL type_change);
	VOID update_number_selection ();
// Construction
public:
	CCalendarWhenDlg(CALENDAR_INFO_REF info, BOOL full_dialog, DWORD WizFlags = WIZBUT_CANCEL | WIZBUT_OK, COLORREF cr = INVALID_DIALOG_COLOR, CWnd* pParent = NULL);	// standard constructor
	CCalendarWhenDlg(BOOL full_dialog, DWORD WizFlags = WIZBUT_CANCEL | WIZBUT_OK, COLORREF cr = INVALID_DIALOG_COLOR, CWnd* pParent = NULL);	                        // Constructor for static info

// Dialog Data
	//{{AFX_DATA(CCalendarWhenDlg)
	enum { IDD = IDD_CALENDAR_WHEN };
	CSpinButtonCtrl	m_ctlSpinNum;
	int		m_month;
	int		m_year;
	int		m_type;
	int		m_week;
	int		m_numMonths;
	//}}AFX_DATA

   static CALENDAR_INFO StaticInfo;
	CALENDAR_TYPE        type;
	BOOL		            m_full_dialog;

// Implementation
protected:
	void
   Afx_Data_Init (void);

   DWORD                m_WizFlags;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void vars_from_info(CALENDAR_INFO_REF info);

	// Generated message map functions
	//{{AFX_MSG(CCalendarWhenDlg)
	afx_msg void OnCalwhenNow();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedType();
	afx_msg void OnMonthChange();
	afx_msg void OnBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL get_exit_info(CALENDAR_INFO_REF info);
};

extern CHAR *suffix_for_day(SHORT day);
