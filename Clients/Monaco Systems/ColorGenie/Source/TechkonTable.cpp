// TechkonTable.cpp: implementation of the CTechkonTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TechkonTable.h"
#include "ErrorMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTechkonTable::CTechkonTable():CDeviceControl()
{

}

CTechkonTable::~CTechkonTable()
{

}

BOOL	CTechkonTable::Calibrate() 
{ 
	return TRUE;

}

BOOL	CTechkonTable::StartComm(void)
{

	ErrorMessage(ERR_NODEVICE);
	return FALSE;
}

BOOL	CTechkonTable::SendRequest(long whichpatch, CWnd *wnd)
{

	ErrorMessage(ERR_NODEVICE);
	return FALSE;
}

BOOL	CTechkonTable::ProcessResult( CCommRequest *pRequest, double *data)
{
	
	ErrorMessage(ERR_NODEVICE);
	return FALSE;
}

