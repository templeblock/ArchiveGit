// GretagScan.h: interface for the CGretagScan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRETAGSCAN_H__6FAA7544_6AFC_11D1_A43E_000094794634__INCLUDED_)
#define AFX_GRETAGSCAN_H__6FAA7544_6AFC_11D1_A43E_000094794634__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DeviceControl.h"
#include "tablepc.h"
#include "patchlayout.h"

class CGretagScan : public CDeviceControl 
{
private:

protected:
	PatchLayout	m_patchLayout;
	TableRect	m_tableRect;
	Table		m_table;

	BOOL	TurnOnLine();
	BOOL	TurnOffLine();
	BOOL	MoveToPatch(int patchnum, int ref);
	BOOL	GetPatchPos(int patchnum, double *pos);
	BOOL	SetPatchPos(int patchnum, double *pos);
	BOOL	GetHeadLocation(int *position, int refrence);

	BOOL	MoveToHome();

	BOOL	PutHeadUp();
	BOOL	PutHeadDown();

	BOOL	AlignTable();
	BOOL	PostPositionDialog(int which);

	BOOL	CheckStart(char *data);
	BOOL	CheckError(int *code);
	BOOL	CheckErrorChart(int *code);

	BOOL	GetLab(CCommRequest *pRequest, double *data);
	BOOL	GetDensity(CCommRequest *pRequest, double *data);

public:
	CGretagScan();
	virtual ~CGretagScan();
	virtual BOOL	Calibrate();
	virtual BOOL	StartComm(void);
	virtual BOOL	SendRequest(long whichpatch, CWnd *wnd);
	virtual	BOOL	ProcessResult( CCommRequest *request, double *data);

};

#endif // !defined(AFX_GRETAGSCAN_H__6FAA7544_6AFC_11D1_A43E_000094794634__INCLUDED_)
