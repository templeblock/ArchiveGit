// ActiveXHostDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "atlcoll.h"
#include "TextEditor.h"

// CActiveXHostDlg dialog
class CActiveXHostDlg : public CDialog
{
// Construction
public:
	CActiveXHostDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ACTIVEXHOST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	float m_fWidth;
	float m_fHeight;
	USHORT m_iFontSize;
	CTexteditor2::STR_TYPES m_StrType;

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void SetDrawString();
	void SetCurrentLine(CString& newStr, int nLine);
	bool GetCurrentLine(CString& str, int nLine);

	CAtlList<CString, CStringElementTraits<CString> > m_CurrStrList;
public:
	CEdit EditWidth;
	CEdit EditHeight;
	CEdit EditFontSize;
	CEdit Writingbox;
	CButton IncrLineSpacing;
	CButton DecrLineSpacing;
	CButton RadioBezier;
	CComboBox SweepMode;
	CButton ImageBkgd;

	afx_msg void OnEnChangeEditbox();
	afx_msg void OnEnChangeEditWidth();
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnEnChangeEditFontsize();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedBtnIncrcs();
	afx_msg void OnBnClickedBtnDecrcs();
	afx_msg void OnBnClickedRadioBezier();
	afx_msg void OnBnClickedRadioCircle();
	afx_msg void OnBnClickedRadioEllipse();
	afx_msg void OnBnClickedRadioSemicircle();
	afx_msg void OnBnClickedRadioSpiral();
	afx_msg void OnBnClickedRadioVertical();
	afx_msg void OnBnClickedRadioHorizontal();
	afx_msg void OnBnClickedRadioInvbezier();
	afx_msg void OnBnClickedRadioInvsemicircle();
	afx_msg void OnBnClickedBtnClr1();
	afx_msg void OnBnClickedBtnClr2();
	afx_msg void OnBnClickedBtnPrvbk();
	afx_msg void OnBnClickedBtnImgbk();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnCbnSelchangeComboSweepmode();
	afx_msg void OnBnClickedCheckImageTrans();
	CTexteditor2 TextEditor;
};
