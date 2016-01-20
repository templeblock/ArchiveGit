/*
// $Workfile: LBLPRINT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LBLPRINT.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:38:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:38   FRED
// Initial revision.
// 
//    Rev 1.5   04 Sep 1995 14:56:44   JAY
// New collate handling.
// 
//    Rev 1.4   19 Aug 1995 12:04:42   FRED
// Business card print dialogs
// 
//    Rev 1.3   10 Aug 1995 13:23:16   JAY
//  
// 
//    Rev 1.2   09 Aug 1995 13:11:24   JAY
// Misc changes.
// 
//    Rev 1.1   21 Jul 1995 17:20:56   JAY
// Label printing
// 
//    Rev 1.0   20 Jul 1995 16:26:40   JAY
//  
*/

class CLabelPrintDialog;
class CLabelStartDialog;
class CBusinessCardPrintDialog;
class CBusinessCardStartDialog;

#ifndef __LBLPRINT_H__
#define __LBLPRINT_H__

#include "pmwprint.h"
#include "pmwdlg.h"

class CLabelPreview;

/////////////////////////////////////////////////////////////////////////////
// CLabelPrintDialog dialog

class CLabelPrintDialog : public CPmwPrint
{
// Construction
public:
	CLabelPrintDialog(CPmwView* pView, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLabelPrintDialog)
	enum { IDD = IDD_LABEL_PRINT };
	BOOL	m_fCollate;
	BOOL	m_fPageBreak;
	int	m_nLabelCopies;
	//}}AFX_DATA

	int m_nTotalLabels;
	int m_nTotalPages;

	int m_nStartingLabel;
	void ComputeTotals(void);
	void UpdateStartingLabel(void);
	void UpdateCollate(void);

	virtual void update_names_chosen(void);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLabelPrintDialog)
	afx_msg void OnStartingLabel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeLabelCopies();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CLabelStartDialog dialog

class CLabelStartDialog : public CPmwDialog
{
// Construction
public:
	CLabelStartDialog(CPaperInfo* pPaperInfo, CWnd* pParent = NULL, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLabelStartDialog)
	enum { IDD = IDD_LABEL_START };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CPaperInfo* m_pPaperInfo;
	int m_nStartingLabel;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CLabelPreview* m_pPreview;

	// Generated message map functions
	//{{AFX_MSG(CLabelStartDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDoubleclickedPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBusinessCardPrintDialog dialog

class CBusinessCardPrintDialog : public CLabelPrintDialog
{
// Construction
public:
	CBusinessCardPrintDialog(CPmwView* pView, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBusinessCardPrintDialog)
	enum { IDD = IDD_BUSINESS_CARD_PRINT };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CBusinessCardPrintDialog)
	afx_msg void OnStartingLabel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBusinessCardStartDialog dialog

class CBusinessCardStartDialog : public CLabelStartDialog
{
// Construction
public:
	CBusinessCardStartDialog(CPaperInfo* pPaperInfo, CWnd* pParent = NULL, int nID = IDD);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBusinessCardStartDialog)
	enum { IDD = IDD_BUSINESS_CARD_START };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CBusinessCardStartDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
