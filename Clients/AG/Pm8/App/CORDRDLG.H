/*
// $Workfile: CORDRDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CORDRDLG.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:32   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:12   JAY
// Reverted. New series.
// 
//    Rev 1.5   11 Nov 1994 16:52:32   FRED
//  
// 
//    Rev 1.4   08 Nov 1994 18:28:20   FRED
//  
*/

#ifndef __CORDRDLG_H__
#define __CORDRDLG_H__

// cordrdlg.h : header file
//

class COrderFormDialog;

/////////////////////////////////////////////////////////////////////////////
// COrderFormDialog dialog

class COrderFormDialog : public CItemDialog
{
// Construction
public:
	COrderFormDialog();	// standard constructor

// Dialog Data
	//{{AFX_DATA(COrderFormDialog)
	enum { IDD = IDD_ORDERFORM };
	CStatic	m_StaticTotalHighlight;
	CStatic	m_StaticTaxHighlight;
	CStatic	m_StaticGraphicsHighlight;
	CStatic	m_StaticFontsHighlight;
	CStatic	m_StaticAllHighlight;
	CEdit	m_EditTotal;
	CEdit	m_EditTaxTotal;
	CEdit	m_EditGraphicsTotal;
	CEdit	m_EditFontsTotal;
	CEdit	m_EditAllTotal;
	BOOL	m_fAllChecked;
	BOOL	m_fFontsChecked;
	BOOL	m_fGraphicsChecked;
	BOOL	m_fTaxChecked;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(COrderFormDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnAll();
	afx_msg void OnFonts();
	afx_msg void OnGraphics();
	afx_msg void OnSalestax();
	afx_msg void OnOrderbymail();
	afx_msg void OnOrderbyphone();
	afx_msg void OnInstall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void UpdateDialog(void);
	void UpdateItem(BOOL fSelected, CWnd& wndPrice, CWnd& wndHighlight, long& lTotal, long lPrice);
	void SetPrice(long lPrice, CWnd* pWnd, BOOL fDollarSign = TRUE, BOOL fBlankIfZero = FALSE);
	void PriceString(CString& csPrice, long lPrice, BOOL fDollarSign = TRUE, BOOL fBlankIfZero = FALSE);
};

#endif
