#pragma once

#ifndef GRETAG_SCAN_DENSITY_H
#define GRETAG_SCAN_DENSITY_H

#include "DeviceControl.h"
#include "tablepc.h"
#include "patchlayout.h"

class CGretagScanDensity : public CDeviceControl 
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

public:
	CGretagScanDensity();
	virtual ~CGretagScanDensity();
	virtual BOOL	Calibrate();
	virtual BOOL	StartComm(void);
	virtual BOOL	SendRequest(long whichpatch, CWnd *wnd);
	virtual	BOOL	ProcessResult( CCommRequest *request, double *data);
};

#endif
