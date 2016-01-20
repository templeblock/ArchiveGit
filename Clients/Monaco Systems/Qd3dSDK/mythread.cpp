// mythread.cpp : implementation file
//

#include "stdafx.h"
#include "..\Test ATL\Test ATL.h"
#include "mythread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// mythread

IMPLEMENT_DYNCREATE(mythread, CWinThread)

mythread::mythread()
{
}

mythread::~mythread()
{
}

BOOL mythread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int mythread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(mythread, CWinThread)
	//{{AFX_MSG_MAP(mythread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// mythread message handlers
