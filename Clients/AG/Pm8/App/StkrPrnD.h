#if !defined(AFX_STKRPRND_H__AB63BEE1_6CB9_11D1_90E0_00A0240C7400__INCLUDED_)
#define AFX_STKRPRND_H__AB63BEE1_6CB9_11D1_90E0_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StkrPrnD.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStickerPrintDialog dialog

#include "pmwprint.h"
#include "pmwdlg.h"


class CStickerPrintDialog : public CPmwPrint
{
// Construction
public:
	CStickerPrintDialog(CPmwView* pView, int nID = IDD);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStickerPrintDialog)
	enum { IDD = IDD_STICKER_PRINT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStickerPrintDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStickerPrintDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STKRPRND_H__AB63BEE1_6CB9_11D1_90E0_00A0240C7400__INCLUDED_)
