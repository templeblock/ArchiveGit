// printadv.h : header file
//

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvancedDlg dialog

class CAdvancedDlg : public CPmwDialog
{
// Construction
public:
	CAdvancedDlg(int outline_gamma, int bitmap_gamma, BOOL fPrintAsBitmap, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAdvancedDlg)
	enum { IDD = IDD_PRINT_ADVANCED };
	CStatic	m_outline_contrast;
	CStatic	m_bitmap_contrast;
	BOOL	m_fPrintAsBitmap;
	//}}AFX_DATA
	int m_outline_gamma;
	int m_bitmap_gamma;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	VOID update_bitmap_contrast(VOID);
	VOID update_outline_contrast(VOID);

	// Generated message map functions
	//{{AFX_MSG(CAdvancedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPrinterAlignment();
	afx_msg void OnPrintAsBitmapCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
