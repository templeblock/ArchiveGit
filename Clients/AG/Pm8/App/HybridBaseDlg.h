#if !defined(AFX_HybridBaseDlg_H__E028DC56_BB96_11D2_B996_00A02416AB46__INCLUDED_)
#define AFX_HybridBaseDlg_H__E028DC56_BB96_11D2_B996_00A02416AB46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// HybridBaseDlg.h : header file
//
#include "PaintCap.h"
#include "dbbld.h"
#include "incompdb.h"

#include "TransBtn.h"

class CPControlInfo;
class CPImageControl;
typedef CTypedPtrList<CObList, CPImageControl*> ImgCtrlList;
typedef CTypedPtrList<CObList, CWnd*> WndCtrlList;
typedef CTypedPtrList<CObList, CPControlInfo*> CtrlInfoList;

/////////////////////////////////////////////////////////////////////////////
// CHybridBaseDlg dialog

class CHybridBaseDlg : public CDialog
{

// Construction and destruction
public:
	CHybridBaseDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL, int nBorderAdjust = 0, int nTitleAdjust = 0);
	~CHybridBaseDlg();


	void DrawBkgnd(CDC* pDC);

// Dialog Data
	//{{AFX_DATA(CHybridBaseDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHybridBaseDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
private:

	void DrawBorder(CDC* pDC, CRect& theRect);
	void DrawCornerPieces(CDC* pDC, CRect& theRect);
	void DrawSidePieces(CDC* pDC, CRect& theRect);


	void ValidatePoint(UINT message, POINT pt);
	
	BOOL LoadDlogBkImages();
	BOOL CreateButton(HWND hWnd, CPControlInfo* pInfo);

protected:

	// Generated message map functions
	//{{AFX_MSG(CHybridBaseDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnSetText(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	static BOOL CALLBACK CustomizeChildWindow(HWND hWindow, LPARAM lData);
	BOOL CreateCustomCtrl(HWND childHWnd, int nCtrlType);

protected:

	CString	m_strBkgdImageName;
	BOOL		m_fTileBkgdImage;

private:
	
	BOOL		m_fCustomLook;

	CPUIImageComponentManagerIndexedDB* m_pAppImageManager;

	CCaptionPainter	m_capp;

	int					m_nBdrAdjust;
	int					m_nTitleAdjust;
	CSize					m_czBrdThickness;
		
	ImgCtrlList			m_listImageControls;	// list of image controls
	WndCtrlList			m_listControls;		// list of window controls

	CPCompPage			m_Page;
	CPImageControl*	m_pBkgdImageCtrl;		// background

	CPImageControl*	m_pBorderImageCtrl;
	CPImageControl*	m_pCornerImageCtrl;

	CFont*				m_pDlogFont;
	CRect					m_rcBkgndImage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HybridBaseDlg_H__E028DC56_BB96_11D2_B996_00A02416AB46__INCLUDED_)
