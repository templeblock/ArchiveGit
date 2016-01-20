// TechkonTable.h: interface for the CTechkonTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TECHKONTABLE_H__6FAA7545_6AFC_11D1_A43E_000094794634__INCLUDED_)
#define AFX_TECHKONTABLE_H__6FAA7545_6AFC_11D1_A43E_000094794634__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DeviceControl.h"

class CTechkonTable : public CDeviceControl  
{
public:
	CTechkonTable();
	virtual ~CTechkonTable();
	virtual BOOL	Calibrate();
	virtual BOOL	StartComm(void);
	virtual BOOL	SendRequest(long whichpatch, CWnd *wnd);
	virtual	BOOL	ProcessResult( CCommRequest *pRequest, double *data);

};

#endif // !defined(AFX_TECHKONTABLE_H__6FAA7545_6AFC_11D1_A43E_000094794634__INCLUDED_)
