#if !defined(AFX_ONADDLG_H__7DF66F40_FEF5_11D1_9909_00A0240C7400__INCLUDED_)
#define AFX_ONADDLG_H__7DF66F40_FEF5_11D1_9909_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OnAdDlg.h : header file
//
#include "pictprvw.h"


class CDDB;
struct PBOX;

/////////////////////////////////////////////////////////////////////////////
// COnlineAdvertisementDlg dialog

class COnlineAdvertisementDlg : public CDialog
{
// Construction
public:
	COnlineAdvertisementDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineAdvertisementDlg();
	
// Dialog Data
	//{{AFX_DATA(COnlineAdvertisementDlg)
	enum { IDD = IDD_ONLINE_ADVERTISEMENT };
	CButton	m_ctrlPicture;
	CStatic	m_ctrlAdText;
	CButton	m_btnOK;
	CButton	m_btnCancel;
	CString	m_strAdText;
	//}}AFX_DATA
	
	CBitmap m_Bitmap;
	CDDB* m_pddb;
	CString m_strbm;
	BOOL m_fURLLink;
   CPicturePreview   m_cppAdImage;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineAdvertisementDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineAdvertisementDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ChangeDimensions();
	void ResizeDlg(PBOX*);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ONADDLG_H__7DF66F40_FEF5_11D1_9909_00A0240C7400__INCLUDED_)
