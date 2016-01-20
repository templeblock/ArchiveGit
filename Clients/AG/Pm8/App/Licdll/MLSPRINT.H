/*
// $Workfile: MLSPRINT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MLSPRINT.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:26   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:42   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MLSPRINT.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:52   Fred
// Initial revision.
// 
//    Rev 1.1   27 Mar 1996 17:03:42   JAY
// Sets orientation during printing. Sets parent correctly.
// 
//    Rev 1.1   27 Mar 1996 16:59:12   FRED
// Forces portrait
// 
//    Rev 1.0   11 Mar 1996 15:55:20   FRED
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:06   JAY
// Initial revision.
// 
//    Rev 1.4   06 Dec 1994 13:38:44   FRED
//  
// 
//    Rev 1.3   11 Nov 1994 09:09:44   JAY
// 
//    Rev 1.2   09 Nov 1994 18:25:48   FRED
//  
// 
//    Rev 1.1   09 Nov 1994 17:45:12   FRED
// 
//    Rev 1.0   09 Nov 1994 17:15:06   FRED
// Initial revision.
*/ 

struct CMlsPrintInfo;
class CMlsPrintDialog;
class CMlsPrintingDialog;

#ifndef __MLSPRINT_H__
#define __MLSPRINT_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMlsPrintInfo

struct CMlsPrintInfo
{
	CMlsPrintInfo(CWnd* pParent = NULL);
	~CMlsPrintInfo();

	CPrintDialog* m_pPD;     // pointer to print dialog
	CDC* m_pDC;

	BOOL m_bContinuePrinting;// set to FALSE to prematurely end printing
	UINT m_nCurPage;         // Current page
	LPVOID m_lpUserData;     // pointer to user created struct
	CRect m_rectDraw;        // rectangle defining current usable page area
	
	int m_nOrientation;		 // desired orientation

	void SetMinPage(UINT nMinPage)
		{ m_pPD->m_pd.nMinPage = nMinPage; }
	void SetMaxPage(UINT nMaxPage)
		{ m_pPD->m_pd.nMaxPage = nMaxPage; }
	UINT GetMinPage() const
		{ return m_pPD->m_pd.nMinPage; }
	UINT GetMaxPage() const
		{ return m_pPD->m_pd.nMaxPage; }
	UINT GetFromPage() const
		{ return m_pPD->m_pd.nFromPage; }
	UINT GetToPage() const
		{ return m_pPD->m_pd.nToPage; }
	void SetOrientation(int nOrientation)
		{ m_nOrientation = nOrientation; }
	int GetOrientation(void)
		{ return m_nOrientation; }
};

/////////////////////////////////////////////////////////////////////////////
// CMlsPrintDialog

class CMlsPrintingDialog : public CBaseDialog
{
public:
	CMlsPrintingDialog(int nID, CWnd* pParent);
	virtual ~CMlsPrintingDialog();

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual BOOL DestroyWindow();

protected:

	//{{AFX_MSG(CMlsPrintingDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	CWnd* m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
// CMlsPrint

class CMlsPrint
{
public:

	// Implementation
	CMlsPrint();
	virtual ~CMlsPrint();

// Operations
public:
	virtual BOOL Print(CMlsPrintInfo* pInfo, CWnd* pParent);
	virtual CMlsPrintInfo* Info(void)
		{ return m_pInfo; }

// Overridables
public:

protected:

	// Printing support
	virtual BOOL OnPreparePrinting();
	virtual void OnPrepareDC();
	virtual void OnBeginPrinting();
	virtual void OnPrint();
	virtual void OnEndPrinting();
	virtual void OnCreatePrintingDialog();
	virtual void OnReleasePrintingDialog();
	virtual void OnUpdatePrintingDialog();
	virtual void GetTitle(CString& csTitle);
	
	CMlsPrintInfo* m_pInfo;
	CWnd* m_pParent;
	CMlsPrintingDialog* m_pPrintingDialog;
	CDC* m_pDC;
};

#endif
