// fd.h : main header file for the FD application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

//	Global Equates
#define FULL_CX			640
#define FULL_CY			400
#define FRAME_WIDTH		320
#define FRAME_HEIGHT	200

/////////////////////////////////////////////////////////////////////////////
// CFdApp:
// See fd.cpp for the implementation of this class
//

class CFdApp : public CWinApp
{
public:
	CFdApp();

// Overrides
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(long iCount);
	virtual int Run(void);
	virtual	CDocument* OpenDocumentFile(LPCSTR lpszFileName);

// Implementation

	//{{AFX_MSG(CFdApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
