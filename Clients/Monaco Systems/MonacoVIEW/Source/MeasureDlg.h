#if !defined(AFX_MEASUREDLG_H__9C5E5F63_EDAD_11D0_9B8C_444553540000__INCLUDED_)
#define AFX_MEASUREDLG_H__9C5E5F63_EDAD_11D0_9B8C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Measure.h : header file
//

#include "SipCal.h"

/////////////////////////////////////////////////////////////////////////////
// CMeasureDlg dialog

class CMeasureDlg : public CDialog
{
// Construction
public:
	CMeasureDlg(COLORREF* pRGBlist, dYxy* pYxyMeasure, int iCount, CWnd* pParent = NULL);   // standard constructor
	~CMeasureDlg();

	void DisplayColorPatch(CWnd* pWnd, COLORREF lColor);
	double TakeMeasurement(dYxy* pYxyMeasure);

protected:
	COLORREF* m_pRGBlist;
	dYxy* m_pYxyMeasure;
	int m_iCount;

// Dialog Data
	//{{AFX_DATA(CMeasureDlg)
	enum { IDD = IDD_MEASURE };
	CButton	m_btnBegin;
	//}}AFX_DATA

// Flag for begin/done button
	BOOL	m_state;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeasureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMeasure();
	afx_msg void OnMeasureFrequency();
	afx_msg void OnSetIntegrationTime();
	afx_msg void OnSetNumPeriods();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASUREDLG_H__9C5E5F63_EDAD_11D0_9B8C_444553540000__INCLUDED_)
