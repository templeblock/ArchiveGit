/*
// $Header: /PM8/App/PARADLG.H 1     3/03/99 6:08p Gbeddow $
//
// Paragraph format dialog
//
// $Log: /PM8/App/PARADLG.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     8/24/98 5:38p Mwilson
// enabled more helpful hints
// 
//    Rev 1.0   14 Aug 1997 15:23:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:58   Fred
// Initial revision.
// 
//    Rev 1.3   18 Jul 1996 11:12:04   Jay
// Text Wrap
// 
//    Rev 1.2   21 Jun 1996 17:50:44   Jay
//  
// 
//    Rev 1.1   18 Jun 1996 16:15:16   Jay
//  
// 
//    Rev 1.0   17 Jun 1996 12:03:30   Jay
// Initial revision.
*/

#ifndef __PARADLG_H__
#define __PARADLG_H__

#include "pmwdlg.h"
class CTextStyle;

/////////////////////////////////////////////////////////////////////////////
// CParagraphFormatDialog dialog

class CParagraphFormatDialog : public CPmwDialog
{
// Construction
public:
	CParagraphFormatDialog(CTextStyle* pStyle, CWnd* pParent = NULL);   // standard constructor
	void GetExitStyle(CTextStyle* pStyle);

// Dialog Data
	//{{AFX_DATA(CParagraphFormatDialog)
	enum { IDD = IDD_PARAGRAPH_FORMAT };
	//}}AFX_DATA
	double	m_dLeftMargin;
	double	m_dRightMargin;
	double	m_dFirstLineIndent;
	double	m_dLeading;
	double	m_dSpaceBefore;
	double	m_dSpaceAfter;
	int		m_nAlignment;
	int		m_nLeadingType;

	BOOL		m_fLeftMargin;
	BOOL		m_fRightMargin;
	BOOL		m_fFirstLineIndent;
	BOOL		m_fLeading;
	BOOL		m_fSpaceBefore;
	BOOL		m_fSpaceAfter;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParagraphFormatDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParagraphFormatDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTextWrapDialog dialog

class CTextWrapDialog : public CPmwDialog
{
// Construction
public:
	CTextWrapDialog(int nFlow, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextWrapDialog)
	enum { IDD = IDD_TEXT_WRAP };
	int		m_nFlow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextWrapDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextWrapDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif					// #ifndef __PARADLG_H__
