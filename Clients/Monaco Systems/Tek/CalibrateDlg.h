#if !defined(AFX_CALIBRATE_H__26149F97_19E4_11D3_91E8_00A02459C447__INCLUDED_)
#define AFX_CALIBRATE_H__26149F97_19E4_11D3_91E8_00A02459C447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CalibrateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalibrateDlg dialog

class CCurveWindow;
class CProfileDoc;

class CCalibrateDlg : public CDialog
{
// Construction
public:
	CCalibrateDlg(CWnd* pParent = NULL);
	~CCalibrateDlg();

// Implementation
protected:
	BOOL m_bShowCurveCyan;
	BOOL m_bShowCurveMagenta;
	BOOL m_bShowCurveYellow;
	BOOL m_bShowCurveBlack;
	CCurveWindow* m_pCurveWindow;

// Dialog Data
public:
	//{{AFX_DATA(CCalibrateDlg)
	enum { IDD = IDD_CALIBRATE };
	CEdit m_editLevelIn;
	CEdit m_editLevelOut;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCalibrateDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetDataFromDoc(CProfileDoc* pDoc);
protected:
	void ClearData(); 
	BOOL HasData();
	
	// Generated message map functions
	//{{AFX_MSG(CCalibrateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBlack();
	afx_msg void OnCyan();
	afx_msg void OnMagenta();
	afx_msg void OnYellow();
	afx_msg void OnReset();
	afx_msg BOOL OnCurveChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBRATE_H__26149F97_19E4_11D3_91E8_00A02459C447__INCLUDED_)
