#ifndef	DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include "CommDevice.h"

class CDeviceControl:public CCommDevice{
private:
protected:
public:
	int m_type;	//measure type

	CDeviceControl():CCommDevice(){;}
	~CDeviceControl(){;}
	virtual BOOL	Calibrate() { return FALSE; }
	virtual BOOL	StartComm(void){ return FALSE; }
	virtual BOOL	SendRequest(long whichpatch, CWnd *wnd){ return FALSE;}
	virtual	BOOL	ProcessResult( CCommRequest *request, double *data){ return FALSE;}

};

#endif	//DEVICE_CONTROL_H
