// Xrite408.cpp: implementation of the CXrite408 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Xrite408.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXrite408::CXrite408():CDeviceControl()
{

}

CXrite408::~CXrite408()
{

}

BOOL	CXrite408::StartComm(void)
{
	//init the port
	Write("\r", 1);
	Write("1j\r", 3);
	Write("g\r", 2);

	return TRUE;
}

BOOL	CXrite408::SendRequest(long whichpatch, CWnd	*wnd)
{
	if(!RequestRead( 40, wnd)){	 //24
		//show error message
		return FALSE;
	}

	return TRUE;
}

BOOL	CXrite408::ProcessResult( CCommRequest *pRequest, double *data)
{
	char	junk[4];

	//no error, then put the data
	if(pRequest->m_dwRequestType == REQUEST_READ){

		sscanf(pRequest->m_lpszResult, "%1s%lf%1s%lf%1s%lf%1s%lf", 
				junk,&data[3],junk,&data[0],junk,&data[1],junk,&data[2]);
	}

	return TRUE;	
}
