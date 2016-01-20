#if !defined(AFX_POSITIONDIALOG_H__4AEA5242_709A_11D1_A43E_000094794634__INCLUDED_)
#define AFX_POSITIONDIALOG_H__4AEA5242_709A_11D1_A43E_000094794634__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PositionDialog.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CPositionDialog dialog

class CPositionDialog : public CDialog
{
// Construction
public:
	CPositionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPositionDialog)
	enum { IDD = IDD_POSITION_DLG };
	CStatic	m_ctlbitMap;
	//}}AFX_DATA

	int	m_type;
	int	m_position;
	CBitmap	m_bitMap;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPositionDialog)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPositionDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSITIONDIALOG_H__4AEA5242_709A_11D1_A43E_000094794634__INCLUDED_)
