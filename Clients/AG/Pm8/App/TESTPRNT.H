/*
// $Workfile: TESTPRNT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/TESTPRNT.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:08   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:40   FRED
// Initial revision.
// 
//    Rev 1.4   06 Oct 1995 11:59:44   JAY
// New print code, etc.
// 
//    Rev 1.3   30 Aug 1995 11:39:32   FRED
// Test print
// 
//    Rev 1.2   28 Aug 1995 20:32:58   FRED
// Print alignment
// 
//    Rev 1.1   28 Aug 1995 15:28:34   FRED
//  
// 
//    Rev 1.0   26 Aug 1995 09:41:14   FRED
// Initial revision.
*/

class CPrinterAlignmentTestMeasurementEdit;
class CPrinterAlignmentTestDialog;

#ifndef __TESTPRNT_H__
#define __TESTPRNT_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPrinterAlignmentTestMeasurementEdit

class CPrinterAlignmentTestMeasurementEdit : public CEdit
{
// Construction
public:
	CPrinterAlignmentTestMeasurementEdit();
	virtual ~CPrinterAlignmentTestMeasurementEdit();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrinterAlignmentTestMeasurementEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPrinterAlignmentTestDialog dialog

class CPrinterAlignmentTestDialog : public CPmwDialog
{
// Construction
public:
	CPrinterAlignmentTestDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrinterAlignmentTestDialog)
	enum { IDD = IDD_PRINT_ALIGNMENT_TEST };
	double	m_dLeftToA;
	double	m_dBottomToA;
	double	m_dLeftToB;
	double	m_dTopToA;
	double	m_dTopToB;
	double	m_dRightToB;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPrinterAlignmentTestDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusMeasurement();
	afx_msg void OnPrintTestPage();
	afx_msg void OnReset();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnClickOrientation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void UpdatePreview(void);
	void GetCurrentMargins(void);
	void SetCurrentMargins(void);
	
protected:
	CPrinterAlignmentTestMeasurementEdit m_TopToB;
	CPrinterAlignmentTestMeasurementEdit m_TopToA;
	CPrinterAlignmentTestMeasurementEdit m_BottomToA;
	CPrinterAlignmentTestMeasurementEdit m_LeftToA;
	CPrinterAlignmentTestMeasurementEdit m_LeftToB;
	CPrinterAlignmentTestMeasurementEdit m_RightToB;
	
	enum
	{
		ORIENTATION_Uninitialized,
		ORIENTATION_Tall,
		ORIENTATION_Wide
	};

	int m_nSelectedOrientation;
	int m_nCurrentOrientation;
	CRect m_crTallPageBounds;
	CRect m_crWidePageBounds;
	
	int GetPageOrientation(void)
		{ return m_nSelectedOrientation == ORIENTATION_Tall ? PORTRAIT : LANDSCAPE; }
};

#endif
