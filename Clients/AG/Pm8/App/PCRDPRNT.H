#include "pmwprint.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPostCardPrintDialog dialog

class CPostCardPrintDialog : public CLabelPrintDialog
{
   INHERIT (CPostCardPrintDialog, CLabelPrintDialog)

// Construction
public:
	CPostCardPrintDialog(CPmwView* pView, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPostCardPrintDialog)
	enum { IDD = IDD_POST_CARD_PRINT };
	BOOL	m_fDoubleSided;
	int		m_nPrintWhat;
	//}}AFX_DATA

	void  UpdateStartingLabel(void);
	void SetPages(int nFromPage, int nToPage);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPostCardPrintDialog)
	afx_msg void OnStartingLabel();
	afx_msg void OnPrintBoth();
	afx_msg void OnPrintBack();
	afx_msg void OnPrintFront();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPostCardStartDialog dialog

class CPostCardStartDialog : public CLabelStartDialog
{
// Construction
public:
	CPostCardStartDialog(CPaperInfo* pPaperInfo, CWnd* pParent = NULL, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPostCardStartDialog)
	enum { IDD = IDD_POST_CARD_START };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPostCardStartDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
