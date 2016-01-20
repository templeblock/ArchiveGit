/*
// $Workfile: IPROGRES.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IPROGRES.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     4/08/98 12:34p Fredf
// New explorer progress indicator.
// 
// 2     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
// 
//    Rev 1.0   14 Aug 1997 15:21:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:20   Fred
// Initial revision.
// 
//    Rev 1.2   20 Jun 1997 16:47:12   Fred
// Progress bar *is* in permanent table, so switched back to pointer
// 
//    Rev 1.1   20 Jun 1997 11:53:04   Fred
// Fixed bug with pointer to temporary CWnd
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

#ifndef _IPROGRES_H_
#define _IPROGRES_H_

#include "cprogbar.h"

/*
// ID's for important controls. These ID's must be used in the template.
*/

#define	IDC_PROGRESS_ACTION		(1015)
#define	IDC_PROGRESS_ITEM			(1016)
#define	IDC_PROGRESS_BAR			(1014)

/////////////////////////////////////////////////////////////////////////////
// CInstallProgressDialog dialog

class CInstallProgressDialog : public CDialog
{
// Construction
public:
	CInstallProgressDialog(int IDD, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallProgressDialog)
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallProgressDialog)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	virtual void FreeAllocatedData(void) {}
	
protected:
	CProgressBar* m_pProgressBar;
	CWnd* m_pAbort;
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
		{ return m_pProgressBar->GetMin(); }
	DWORD GetMax(void)
		{ return m_pProgressBar->GetMax(); }
	DWORD GetPos(void)
		{ return m_pProgressBar->GetPos(); }
	UINT GetPosPercentage(void)
		{ return m_pProgressBar->GetPosPercentage(); }
	void SetMin(DWORD dwMin)
		{ m_pProgressBar->SetMin(dwMin); }
	void SetMax(DWORD dwMax)
		{ m_pProgressBar->SetMax(dwMax); }
	void SetPos(DWORD dwPos)
		{ m_pProgressBar->SetPos(dwPos); }
	void SetPosPercentage(UINT uPercentage)
		{ m_pProgressBar->SetPosPercentage(uPercentage); }
	void SetMinMaxPos(DWORD dwMin, DWORD dwMax, DWORD dwPos)
		{ m_pProgressBar->SetMinMaxPos(dwMin, dwMax, dwPos); }
	void SetMinMaxPosPercentage(DWORD dwMin, DWORD dwMax, UINT uPercentage)
		{ m_pProgressBar->SetMinMaxPosPercentage(dwMin, dwMax, uPercentage); }
	void SetPercentageFormat(LPCSTR pszFormat)
		{ m_pProgressBar->SetPercentageFormat(pszFormat); }
	void HideProgress(void)
		{ m_pProgressBar->Hide(); }
	void ShowProgress(void)
		{ m_pProgressBar->Show(); }
	BOOL IsProgressVisible(void)
		{ return m_pProgressBar->IsVisible(); }
	void RedrawProgress(void)
		{ m_pProgressBar->Redraw(); }
	void HideAnimation(void);
	void ShowAnimation(LPCSTR pszResource);
	BOOL CheckForAbort(void)
		{ if (!m_fIsAborted) { Breathe(); } return m_fIsAborted; }
	void EnableAbort(BOOL fEnable)
		{ if (m_pAbort != NULL) m_pAbort->EnableWindow(fEnable); }
	void Breathe(void);
};

#endif
