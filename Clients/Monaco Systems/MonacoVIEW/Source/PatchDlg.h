#if !defined(AFX_CPatchDlg_H__9C5E5F62_EDAD_11D0_9B8C_444553540000__INCLUDED_)
#define AFX_CPatchDlg_H__9C5E5F62_EDAD_11D0_9B8C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PatchDlg.h : header file
//

#include "ColorControl.h"
//j#include "DataConvert.h"

/////////////////////////////////////////////////////////////////////////////
// CPatchDlg dialog

class CPatchDlg : public CDialog
{
// Construction
public:
	CPatchDlg(double* pLab, double* pDeltaE, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	int m_iCurrent;
	double* m_lab; //[5][3];
	double* m_deltaE; //[5];
//j	MonitorData m_MonitorData;
//j	DataConvert m_DataConvert;
	//{{AFX_DATA(CPatchDlg)
	enum { IDD = IDD_VAL_PATCH };
	CButton m_radio1;
	CButton m_radio2;
	CButton m_radio3;
	CButton m_radio4;
	CButton m_radio5;
	CEdit m_editL;
	CEdit m_editA;
	CEdit m_editB;
	CEdit m_editDeltaE;
	CStatic	m_patch_warn[5];
	CColorControl m_patch[5];
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnChangeEditL();
	afx_msg void OnChangeEditA();
	afx_msg void OnChangeEditB();
	afx_msg void OnChangeEditDeltaE();
	afx_msg void OnPatch1();
	afx_msg void OnPatch2();
	afx_msg void OnPatch3();
	afx_msg void OnPatch4();
	afx_msg void OnPatch5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPatchDlg_H__9C5E5F62_EDAD_11D0_9B8C_444553540000__INCLUDED_)
