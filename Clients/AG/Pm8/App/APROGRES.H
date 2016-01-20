/*
// $Workfile: APROGRES.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/APROGRES.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:40   Fred
// Initial revision.
// 
//    Rev 1.4   30 Jul 1997 16:53:52   johno
// Added 2nd progress bar
// 
//    Rev 1.3   19 Jun 1997 18:08:22   johno
// 
//    Rev 1.2   18 Jun 1997 16:42:02   johno
// 
//    Rev 1.1   18 Jun 1997 09:20:50   Fred
// Moved ID's back into .H file
// 
//    Rev 1.0   17 Jun 1997 14:06:24   johno
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 07:47:28   Fred
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:40   JAY
// Reverted. New series.
// 
//    Rev 1.0   16 Jun 1994 18:16:12   FRED
// Initial revision.
*/ 

#ifndef _APROGRES_H_
#define _APROGRES_H_

#include "cprogbar.h"

/*
// ID's for important controls. These ID's must be used in the template.
*/

#define	IDC_PROGRESS_ACTION		(1015)
#define	IDC_PROGRESS_ITEM			(1016)
#define	IDC_PROGRESS_BAR			(1014)
#define	IDC_PROGRESS_BAR2			(1017)

/////////////////////////////////////////////////////////////////////////////
// CAddonProgressDialog dialog

class CAddonProgressDialog : public CDialog
{
// Construction
public:
	CAddonProgressDialog(int IDD, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAddonProgressDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CAddonProgressDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	virtual void FreeAllocatedData(void) {}
	
protected:
	//CProgressBar *m_pProgressBar;
	BOOL m_fIsAborted;
 	
public:
	// pass through methods to progress bar
	void ShowAction(const CString &csAction)
		{ SetDlgItemText(IDC_PROGRESS_ACTION, csAction); }
	
   void HideAction(void)
		{ SetDlgItemText(IDC_PROGRESS_ACTION, ""); }
	
   void ShowItem(const CString &csItem)
		{ SetDlgItemText(IDC_PROGRESS_ITEM, csItem); }
	
   void HideItem(void)
		{ SetDlgItemText(IDC_PROGRESS_ITEM, ""); }

	DWORD GetMin(void)
	{
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return 0;
      
      return pProgressBar->GetMin();
   }
	
   DWORD GetMax(void)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return 0;
      
      return pProgressBar->GetMax(); 
   }
	
   DWORD GetPos(void)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return 0;
      
      return pProgressBar->GetPos(); 
   }
	
   UINT GetPosPercentage(void)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return 0;
      
      return pProgressBar->GetPosPercentage(); 
   }
	
   void SetMin(DWORD dwMin)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->SetMin(dwMin); 
   }
	
   void SetMax(DWORD dwMax)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->SetMax(dwMax); 
   }
	
   void SetPos(DWORD dwPos)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->SetPos(dwPos); 
   }
	
   void SetPosPercentage(UINT uPercentage)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->SetPosPercentage(uPercentage); 
   }
	
   void SetMinMaxPos(DWORD dwMin, DWORD dwMax, DWORD dwPos)
	{
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->SetMinMaxPos(dwMin, dwMax, dwPos); 
   }
	
   void SetMinMaxPosPercentage(DWORD dwMin, DWORD dwMax, UINT uPercentage)
   { 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->SetMinMaxPosPercentage(dwMin, dwMax, uPercentage); 
   }
	
   void HideProgress(void)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->Hide(); 
   }
	
   void ShowProgress(void)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) != NULL)
         pProgressBar->Show(); 
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR2)) != NULL)
         pProgressBar->Show(); 
   }
	
   BOOL IsProgressVisible(void)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return FALSE;
      
      return pProgressBar->IsVisible(); 
   }
	
   void RedrawProgress(void)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR)) == NULL)
         return;
      
      pProgressBar->Redraw();
   }
	
   BOOL CheckForAbort(void)
	{ 
      if (!m_fIsAborted) { Breathe(); } return m_fIsAborted; 
   }
	
   void Breathe(void);

   // Progress bar II
   void SetMinMaxPos2 (DWORD dwMin, DWORD dwMax, DWORD dwPos)
	{
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR2)) == NULL)
         return;
      
      pProgressBar->SetMinMaxPos(dwMin, dwMax, dwPos); 
   }
	
   void SetPos2 (DWORD dwPos)
	{ 
      CProgressBar *pProgressBar;
      if ((pProgressBar = (CProgressBar *)GetDlgItem(IDC_PROGRESS_BAR2)) == NULL)
         return;
      
      pProgressBar->SetPos(dwPos); 
   }
};

#endif
