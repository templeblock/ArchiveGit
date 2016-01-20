// thread.cpp : implementation file
//

#include "stdafx.h"
#include "..\Test ATL\Test ATL.h"
#include "thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// thread

IMPLEMENT_DYNCREATE(thread, CWinThread)

thread::thread()
{
}

thread::~thread()
{
}

BOOL thread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int thread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(thread, CWinThread)
	//{{AFX_MSG_MAP(thread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// thread message handlers
