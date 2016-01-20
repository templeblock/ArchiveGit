#if !defined(AFX_EDITIMAGE_H__26149F90_19E4_11D3_91E8_00A02459C447__INCLUDED_)
#define AFX_EDITIMAGE_H__26149F90_19E4_11D3_91E8_00A02459C447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EditImageDlg.h : header file
//

#include "ImageN.h"

class CCalibrateDlg;
class CToneAdjustDlg;
class CTuningDlg;
class CImageN;

/////////////////////////////////////////////////////////////////////////////
// CEditImageDlg dialog

class CEditImageDlg : public CDialog
{
// Construction
public:
	CEditImageDlg(CString& szImageFile, CWnd* pParent = NULL);
	~CEditImageDlg();

// Dialog Data
	DWORD m_dwBackupSize;
	HGLOBAL m_pBackupBuffer;
	CString m_szImageFile;
	HGLOBAL m_hDib;
	CPoint m_ptMouseOnImage;
	CCalibrateDlg* m_pCalibrateDlg;
	CToneAdjustDlg* m_pToneAdjustDlg;
	CTuningDlg* m_pTuningDlg;

	//{{AFX_DATA(CEditImageDlg)
	enum { IDD = IDD_EDIT_IMAGE };
	CImageN	m_ctlImage;
	BOOL	m_bZoom;
	BOOL	m_bUnzoom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditImageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL GetImageFileName(CString& szFileName);
	void RestoreImage();
	void ModifyImage(bool bSplit);

	// Generated message map functions
	//{{AFX_MSG(CEditImageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCurve1();
	afx_msg void OnCurve2();
	afx_msg void OnTuning();
	afx_msg void OnPrint();
	afx_msg void OnHelp();
	afx_msg void OnZoom();
	afx_msg void OnUnzoom();
	afx_msg void OnOpenImage();
	afx_msg void OnImageMouseMove(short Button, short Shift, long X, long Y);
	afx_msg void OnImageClick();
	afx_msg void OnImageDoubleClick();
	afx_msg void OnImageMouseDown(short Button, short Shift, long X, long Y);
	afx_msg void OnImageMouseUp(short Button, short Shift, long X, long Y);
	afx_msg void OnImageViewStatusChanged(long ViewId, long TypeCode, long Data);
	afx_msg void OnImageBufStatusChanged(long BufId, long Status, long Data);
	afx_msg void OnImageTimerTick();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITIMAGE_H__26149F90_19E4_11D3_91E8_00A02459C447__INCLUDED_)
