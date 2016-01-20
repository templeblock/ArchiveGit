// FontsInUseDlg.h : header file
//

#if !defined(AFX_FONTSINUSEDLG_H__37890166_0BFF_11D6_979D_00B0D0609817__INCLUDED_)
#define AFX_FONTSINUSEDLG_H__37890166_0BFF_11D6_979D_00B0D0609817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFontsInUseDlg dialog

class CFontsInUseDlg : public CDialog
{
// Construction
public:
	CFontsInUseDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFontsInUseDlg)
	enum { IDD = IDD_FONTSINUSE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontsInUseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	void FontsInUse();
protected:
	HICON m_hIcon;
	int m_dx;
	int m_dy;

	// Generated message map functions
	//{{AFX_MSG(CFontsInUseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTSINUSEDLG_H__37890166_0BFF_11D6_979D_00B0D0609817__INCLUDED_)
