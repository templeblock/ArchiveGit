/*
// $Workfile: PRINTWIZ.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PRINTWIZ.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:30   Fred
// Initial revision.
// 
//    Rev 1.3   16 Aug 1996 12:28:26   Fred
// Fixed 16-bit bug where choices were not recorded
// 
//    Rev 1.2   06 Aug 1996 14:37:32   Jay
//  
// 
//    Rev 1.1   01 Aug 1996 09:23:10   Fred
// More 16-bit changes
// 
//    Rev 1.0   24 Jul 1996 18:24:48   Jay
// Initial revision.
*/

class CDoubleSidedPrintButton;
class CDoubleSidedPrintIntroduction;
class CDoubleSidedPrintPrePrint1;
class CDoubleSidedPrintPostPrint1;
class CDoubleSidedPrintPrePrint2;
class CDoubleSidedPrintPostPrint2;
class CDoubleSidedPrintFinish;
class CDoubleSidedPrintWizard;

#ifndef __PRINTWIZ_H__
#define __PRINTWIZ_H__

#include "pmwwiz.h"
#include "btnwnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintButton - A special button that has a checked state.

class CDoubleSidedPrintButton : public CButtonWnd
{
public:
	CDoubleSidedPrintButton();

	void SetChecked(BOOL fChecked);
	BOOL GetChecked(void)
		{ return m_fChecked; }

protected:
	BOOL m_fChecked;
	virtual void PaintCustomize(CDC& dcButton, CRect crButton);
};

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintIntroduction dialog

class CDoubleSidedPrintIntroduction : public CPmwWizardPage
{
	INHERIT(CDoubleSidedPrintIntroduction, CPmwWizardPage)
//	DECLARE_DYNCREATE(CDoubleSidedPrintIntroduction)

// Construction
public:
	CDoubleSidedPrintIntroduction();
	~CDoubleSidedPrintIntroduction();

// Dialog Data
	//{{AFX_DATA(CDoubleSidedPrintIntroduction)
	enum { IDD = IDD_DOUBLE_SIDED_PRINT_INTRODUCTION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintIntroduction)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDoubleSidedPrintIntroduction)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPrePrint1 dialog

class CDoubleSidedPrintPrePrint1 : public CPmwWizardPage
{
	INHERIT(CDoubleSidedPrintPrePrint1, CPmwWizardPage)
//	DECLARE_DYNCREATE(CDoubleSidedPrintPrePrint1)

// Construction
public:
	CDoubleSidedPrintPrePrint1();
	~CDoubleSidedPrintPrePrint1();

// Dialog Data
	//{{AFX_DATA(CDoubleSidedPrintPrePrint1)
	enum { IDD = IDD_DOUBLE_SIDED_PRINT_PREPRINT1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintPrePrint1)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDoubleSidedPrintPrePrint1)
	afx_msg void OnClickedPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void CreateLeftFrame(CPmwView* pView, LPCSTR pszText);
};

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPostPrint1 dialog

class CDoubleSidedPrintPostPrint1 : public CPmwWizardPage
{
	INHERIT(CDoubleSidedPrintPostPrint1, CPmwWizardPage)
//	DECLARE_DYNCREATE(CDoubleSidedPrintPostPrint1)

// Construction
public:
	CDoubleSidedPrintPostPrint1();
	~CDoubleSidedPrintPostPrint1();

// Dialog Data
	//{{AFX_DATA(CDoubleSidedPrintPostPrint1)
	enum { IDD = IDD_DOUBLE_SIDED_PRINT_POSTPRINT1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintPostPrint1)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDoubleSidedPrintPostPrint1)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedPage1();
	afx_msg void OnClickedPage2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void UpdateButtons(void);
	void ResetPostPrint2(void);

public:
	CDoubleSidedPrintButton	m_Output1;
	CDoubleSidedPrintButton	m_Output2;
	int m_nChosen;
};

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPrePrint2 dialog

class CDoubleSidedPrintPrePrint2 : public CPmwWizardPage
{
	INHERIT(CDoubleSidedPrintPrePrint2, CPmwWizardPage)
//	DECLARE_DYNCREATE(CDoubleSidedPrintPrePrint2)

// Construction
public:
	CDoubleSidedPrintPrePrint2();
	~CDoubleSidedPrintPrePrint2();

// Dialog Data
	//{{AFX_DATA(CDoubleSidedPrintPrePrint2)
	enum { IDD = IDD_DOUBLE_SIDED_PRINT_PREPRINT2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintPrePrint2)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDoubleSidedPrintPrePrint2)
	afx_msg void OnClickedPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void CreateRightFrame(CPmwView* pView, LPCSTR pszText);
};

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintPostPrint2 dialog

class CDoubleSidedPrintPostPrint2 : public CPmwWizardPage
{
	INHERIT(CDoubleSidedPrintPostPrint2, CPmwWizardPage)
//	DECLARE_DYNCREATE(CDoubleSidedPrintPostPrint2)

// Construction
public:
	CDoubleSidedPrintPostPrint2();
	~CDoubleSidedPrintPostPrint2();

// Dialog Data
	//{{AFX_DATA(CDoubleSidedPrintPostPrint2)
	enum { IDD = IDD_DOUBLE_SIDED_PRINT_POSTPRINT2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintPostPrint2)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDoubleSidedPrintPostPrint2)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedPage1();
	afx_msg void OnClickedPage2();
	afx_msg void OnClickedPage3();
	afx_msg void OnClickedPage4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void UpdateButtons(void);
	void UpdateBitmaps(void);

public:
	CDoubleSidedPrintButton	m_Output1;
	CDoubleSidedPrintButton	m_Output2;
	CDoubleSidedPrintButton	m_Output3;
	CDoubleSidedPrintButton	m_Output4;

	int m_nChosen;
};

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintFinish dialog

class CDoubleSidedPrintFinish : public CPmwWizardPage
{
	INHERIT(CDoubleSidedPrintFinish, CPmwWizardPage)
//	DECLARE_DYNCREATE(CDoubleSidedPrintFinish)

// Construction
public:
	CDoubleSidedPrintFinish();
	~CDoubleSidedPrintFinish();

// Dialog Data
	//{{AFX_DATA(CDoubleSidedPrintFinish)
	enum { IDD = IDD_DOUBLE_SIDED_PRINT_FINISH };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintFinish)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDoubleSidedPrintFinish)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDoubleSidedPrintWizard - This is the main wizard (dialog).

class CDoubleSidedPrintWizard : public CPmwWizard
{
	INHERIT(CDoubleSidedPrintWizard, CPmwWizard)
//	DECLARE_DYNAMIC(CDoubleSidedPrintWizard)

// Construction
public:
	CDoubleSidedPrintWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDoubleSidedPrintWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

	virtual void InitDialog(void);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDoubleSidedPrintWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDoubleSidedPrintWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDoubleSidedPrintWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void CommonConstruct(void);

public:
	static BOOL Run(CWnd* pWnd);

public:
	virtual CBitmap* GetBitmap(void)
		{ return &m_Bitmap; }

	CDoubleSidedPrintIntroduction m_Introduction;
	CDoubleSidedPrintPrePrint1 m_PrePrint1;
	CDoubleSidedPrintPostPrint1 m_PostPrint1;
	CDoubleSidedPrintPrePrint2 m_PrePrint2;
	CDoubleSidedPrintPostPrint2 m_PostPrint2;
	CDoubleSidedPrintFinish m_Finish;

protected:
	CBitmap m_Bitmap;
};

#endif
