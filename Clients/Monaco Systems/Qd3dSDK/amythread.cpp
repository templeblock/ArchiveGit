// amythread.cpp : implementation file
//

#include "stdafx.h"
#include "..\Test ATL\Test ATL.h"
#include "amythread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// amythread

IMPLEMENT_DYNCREATE(amythread, CWinThread)

amythread::amythread()
{
}

amythread::~amythread()
{
}

BOOL amythread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int amythread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(amythread, CWinThread)
	//{{AFX_MSG_MAP(amythread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// amythread message handlers
