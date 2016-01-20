#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"


class CProcessExamApp : public CWinApp
{
public:
	CProcessExamApp();

public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
protected:
	int m_iExitCode;
};

extern CProcessExamApp theApp;
