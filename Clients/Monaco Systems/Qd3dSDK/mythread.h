#if !defined(AFX_MYTHREAD_H__3D384F54_D3EF_11D1_91E6_486C0DC10000__INCLUDED_)
#define AFX_MYTHREAD_H__3D384F54_D3EF_11D1_91E6_486C0DC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mythread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// mythread thread

class mythread : public CWinThread
{
	DECLARE_DYNCREATE(mythread)
protected:
	mythread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(mythread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~mythread();

	// Generated message map functions
	//{{AFX_MSG(mythread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTHREAD_H__3D384F54_D3EF_11D1_91E6_486C0DC10000__INCLUDED_)
