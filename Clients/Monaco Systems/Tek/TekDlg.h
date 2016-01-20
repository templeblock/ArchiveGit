// TekDlg.h : header file
//

#if !defined(AFX_TEKDLG_H__26149F88_19E4_11D3_91E8_00A02459C447__INCLUDED_)
#define AFX_TEKDLG_H__26149F88_19E4_11D3_91E8_00A02459C447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CTekDlg dialog

class CTekDlg : public CDialog
{
// Construction
public:
	CTekDlg(CWnd* pParent = NULL);
	~CTekDlg();

// Dialog Data
	//{{AFX_DATA(CTekDlg)
	enum { IDD = IDD_LAUNCHER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTekDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CDialog* m_pEditProfileDlg;

	// Generated message map functions
	//{{AFX_MSG(CTekDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEditProfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEKDLG_H__26149F88_19E4_11D3_91E8_00A02459C447__INCLUDED_)
