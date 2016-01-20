// Xrite408.h: interface for the CXrite408 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRITE408_H__6FAA7543_6AFC_11D1_A43E_000094794634__INCLUDED_)
#define AFX_XRITE408_H__6FAA7543_6AFC_11D1_A43E_000094794634__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DeviceControl.h"

class CXrite408 : public CDeviceControl  
{
public:
	CXrite408();
	virtual ~CXrite408();
	virtual BOOL	Calibrate() { return TRUE;}
	virtual BOOL	StartComm(void);
	virtual BOOL	SendRequest(long whichpatch, CWnd *wnd);
	virtual	BOOL	ProcessResult( CCommRequest *pRequest, double *data);

};

#endif // !defined(AFX_XRITE408_H__6FAA7543_6AFC_11D1_A43E_000094794634__INCLUDED_)
