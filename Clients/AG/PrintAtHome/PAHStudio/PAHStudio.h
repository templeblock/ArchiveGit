// PAHStudio.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "PAHStudio_i.h"
#include "singleinstance.h"

// CPAHStudioApp:
// See PAHStudio.cpp for the implementation of this class
//

class CPAHStudioApp : public CWinApp,
					  public CSingleInstance
{
public:
	CPAHStudioApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual void WakeUp ( LPCTSTR aCommandLine ) const ;


// Implementation

	DECLARE_MESSAGE_MAP()
public:
	BOOL ExitInstance(void);
	LPCTSTR GetHomeFolder();

};

extern CPAHStudioApp theApp;