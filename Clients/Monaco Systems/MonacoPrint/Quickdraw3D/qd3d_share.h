/******************************************************************/
/* qd3d_share.h													  */
/* class that allows sharing of data and events between windows	  */
/* Feb 26, 1996												      */
/* James Vogh													  */
/******************************************************************/

#ifndef IN_QD3D_SHARE
#define IN_QD3D_SHARE

#include "share_data.h"

// The following defines shared parameters for wiindows
class QD3D_Shared:public Shared_Data{
private:
protected:
	double	xrt,yrt,zrt;	//rotation parameters
	double	xr,yr,zr;	//rotation parameters
	double	scale;	//scale
	double  dx,dy;  //translation
	Boolean stopped;
public:	
	
	QD3D_Shared(MS_WindowTypes type);
	~QD3D_Shared(void);
	
	Boolean MyType(MS_WindowTypes t);
	McoStatus AddWin(baseWin *win);
	McoStatus ProcessMouseDown(Point where,int code,baseWin *frontWin);
	McoStatus DoAlwaysUpdate(baseWin *frontWin);
	
};
	
	
#endif