/*
// $Workfile: PROJGRPH.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROJGRPH.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     5/19/98 11:35a Dennis
// Added progress bar and methods to control it.
// 
// 2     2/27/98 2:45p Jayn
// Added Extract Project
// 
//    Rev 1.0   14 Aug 1997 15:25:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Jul 1997 11:19:30   Fred
// Initial revision.
*/

class CFindProjectsWithGraphics;

#ifndef __PROJGRPH_H__
#define __PROJGRPH_H__

/////////////////////////////////////////////////////////////////////////////
// Find projects with graphics class.

class CFindProjectsWithGraphics
{
public:
	CFindProjectsWithGraphics();
	virtual ~CFindProjectsWithGraphics();

public:
	// Run the show.
	void Run(void);
};

#define	IDC_PROGRESS_MESSAGE_LIST		(666)

/////////////////////////////////////////////////////////////////////////////
// Extract content class.

class CExtractContent
{
public:
	CExtractContent();
	virtual ~CExtractContent();

public:
	// Run the show.
	void Run(void);
};

/////////////////////////////////////////////////////////////////////////////
// CFindProjectsWithGraphicsProgressDialog dialog

class CFindProjectsWithGraphicsProgressDialog : public CDialog
{
// Construction
public:
	CFindProjectsWithGraphicsProgressDialog(int IDD, CWnd* pParent = NULL);	// standard constructor
	virtual ~CFindProjectsWithGraphicsProgressDialog();

// Dialog Data
	//{{AFX_DATA(CFindProjectsWithGraphicsProgressDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CFindProjectsWithGraphicsProgressDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	BOOL m_fIsAborted;
	BOOL m_fIsDone;
	CListBox* m_pMessageList;
	CWnd* m_pParent;
   CProgressCtrl *m_pProgressBar;
	
public:
	void AddMessage(LPCSTR pszAction);
	BOOL CheckForAbort(void)
		{ if (!m_fIsAborted) { Breathe(); } return m_fIsAborted; }
   void SetProgressRange(int nStart, int nEnd);
   void SetProgressStep(int nStep=1);
   void StepProgress();
	void WaitForDone(void);
	void Finish(void);
	void Breathe(void);
};


#endif
