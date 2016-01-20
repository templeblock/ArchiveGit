// DlgProxy.h: header file
//

#pragma once

class CPAHStudioDlg;


// CPAHStudioDlgAutoProxy command target

class CPAHStudioDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CPAHStudioDlgAutoProxy)

	CPAHStudioDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CPAHStudioDlg* m_pDialog;

// Operations
public:

// Overrides
	public:
	virtual void OnFinalRelease();

// Implementation
protected:
	virtual ~CPAHStudioDlgAutoProxy();

	// Generated message map functions

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CPAHStudioDlgAutoProxy)

	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

