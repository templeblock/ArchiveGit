/*
// $Workfile: tblctl.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/tblctl.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 3     6/17/98 11:09a Jayn
// 
// 2     6/02/98 3:00p Jayn
// 
// 1     6/01/98 9:58a Jayn
*/ 

#ifndef __TBLCTL_H__
#define __TBLCTL_H__

#include "tbldef.h"
#include "redisp.h"

class CTableObject;

/////////////////////////////////////////////////////////////////////////////
// CTableFormatControl window

class CTableFormatControl : public CWnd
{
public:
// Construction
public:
	CTableFormatControl();

// Implementation
public:
	virtual ~CTableFormatControl();

	void Invalidate(void)
		{ InvalidateRect(NULL); }

	// Initialize the control.
	void Init(void);

	CTableObject* GetTable(void) const
		{ return m_pTable; }
protected:
	// Generated message map functions
	//{{AFX_MSG(CTableFormatControl)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }

protected:	
	// Our client rect.
	CRect m_crClient;

	// Our redisplay context.
	RedisplayContext m_rc;

	// Our table object.
	CTableObject* m_pTable;

	// The document used for previewing.
	CPmwDoc* m_pDoc;

private:
	static LONG CALLBACK AFX_EXPORT FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	BOOL m_fCreatedByClassName;
	virtual void PostNcDestroy();
};

/////////////////////////////////////////////////////////////////////////////

#endif
