//{{AFX_INCLUDES()
#include "imagen.h"
//}}AFX_INCLUDES
#if !defined(AFX_CROP_H__F09703D8_364F_11D1_BAE5_200604C10000__INCLUDED_)
#define AFX_CROP_H__F09703D8_364F_11D1_BAE5_200604C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Crop.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCrop dialog

class CCrop : public CDialog
{
// Construction
public:
	CCrop(CString szImageFile, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCrop)
	enum { IDD = IDD_CROP };
	CString	m_szImageFile;
	CImageN	m_ctlImage;
	CPoint	m_ptMouseOnImage;
	CPoint	m_ptCornerUR;
	CPoint	m_ptCornerUL;
	CPoint	m_ptCornerLR;
	CPoint	m_ptCornerLL;
	CPoint	m_ptOutputUR;
	CPoint	m_ptOutputUL;
	CPoint	m_ptOutputLR;
	CPoint	m_ptOutputLL;
	HGLOBAL m_hDib;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrop)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HGLOBAL GetDib(void);
	CWnd* GetImageWindow(int idControl);
	void UndrawMark(int iType, POINT pt);
	void DrawMark(int iType, POINT pt, BOOL bDrawText);
	void DrawAllMarks(BOOL bDrawText);
	// Generated message map functions
	//{{AFX_MSG(CCrop)
	virtual BOOL OnInitDialog();
	afx_msg void OnUnzoom();
	afx_msg void OnCropOK();
	afx_msg void OnImageMouseMove(short Button, short Shift, long X, long Y);
	afx_msg void OnImageClick();
	afx_msg void OnImageDoubleClick();
	afx_msg void OnImageMouseDown(short Button, short Shift, long X, long Y);
	afx_msg void OnImageMouseUp(short Button, short Shift, long X, long Y);
	afx_msg void OnImageViewStatusChanged(long ViewId, long TypeCode, long Data);
	afx_msg void OnImageBufStatusChanged(long BufId, long Status, long Data);
	afx_msg void OnImageTimerTick();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CROP_H__F09703D8_364F_11D1_BAE5_200604C10000__INCLUDED_)
