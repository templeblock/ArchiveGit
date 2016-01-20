// $Workfile: ProgBar.h $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.

// Revision History:
//
// $Log: /PM8/App/ProgBar.h $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 4     5/20/98 5:19p Mwilson
// changed to diable and enable main window as necessary.
// 
// 3     12/09/97 3:30p Dennis
// Added History
// 

#if !defined(AFX_PROGBAR_H__339C12E0_6CB2_11D1_A666_00A024EA69C0__INCLUDED_)
#define AFX_PROGBAR_H__339C12E0_6CB2_11D1_A666_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProgBar.h : header file
//

class CPMWProgressBarStatus
   {
   public:
      CPMWProgressBarStatus()
         {
            m_bCancelHit = FALSE;
         }
      BOOL CancelHit()
         { return m_bCancelHit; }
      void SetCancel(BOOL bHasBeenHit)
         { m_bCancelHit = bHasBeenHit; }
   protected:
      BOOL  m_bCancelHit;
   };

/////////////////////////////////////////////////////////////////////////////
// CPMWProgressBar dialog

class CPMWProgressBar : public CDialog
{
// Construction
public:
	CPMWProgressBar(CWnd* pParent = NULL);   // standard constructor

   // Operations
   // GetStatus() and CancelHit() are useful for polling progress stats
   CPMWProgressBarStatus & GetStatus()
      { return m_status; }
   BOOL  CancelHit()
      { return m_status.CancelHit();}

   // SetCancelMethod is used to set a callback method when the cancel
   // button has been hit
   static void SetCancelMethod(void (*pCancelMethod)());

// Dialog Data
	//{{AFX_DATA(CPMWProgressBar)
	enum { IDD = IDD_PROGRESS_BAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPMWProgressBar)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPMWProgressBar)
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
   CPMWProgressBarStatus   m_status;
   // Static Data Members
   static void (*m_pCancelMethod)();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGBAR_H__339C12E0_6CB2_11D1_A666_00A024EA69C0__INCLUDED_)
