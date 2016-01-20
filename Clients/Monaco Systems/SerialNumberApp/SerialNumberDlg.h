// SerialNumberDlg.h : header file
//

#if !defined(AFX_SERIALNUMBERDLG_H__CA49A617_D314_11D2_8617_00A0241A89B6__INCLUDED_)
#define AFX_SERIALNUMBERDLG_H__CA49A617_D314_11D2_8617_00A0241A89B6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSerialNumberDlg dialog

class CSerialNumberDlg : public CDialog
{
// Construction
public:
	CSerialNumberDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSerialNumberDlg)
	enum { IDD = IDD_SERIALNUMBERAPP_DIALOG };
	CButton m_buttonSaveAs;
	CButton m_buttonCancel;
	CComboBox m_comboVendor;
	CEdit m_editPrefix;
	CListBox m_listboxList;
	CEdit m_editHowMany;
	CButton m_buttonGenerate;
	CComboBox m_comboApplication;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialNumberDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSerialNumberDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSaveAsClicked();
	afx_msg void OnApplicationSelchange();
	afx_msg void OnVendorSelchange();
	afx_msg void OnPrefixChange();
	afx_msg void OnHowManyChange();
	afx_msg void OnGenerateClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ScrambleTwoDigits(int iTwoDigits, int& iData, int& iKey);
	int UnscrambleTwoDigits(int iData, int iKey);
	int CheckSumString(CString szString, int iStopValue);
	int CheckSum(int iValue, int iStopValue);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALNUMBERDLG_H__CA49A617_D314_11D2_8617_00A0241A89B6__INCLUDED_)
