/******************************************************************/
/* qd3d_share.c													  */
/* class that allows sharing of data and events between windows	  */
/* Feb 26, 1996												      */
/* James Vogh													  */
/******************************************************************/

#include "qd3d_share.h"
#include <QD3DGeometry.h>

QD3D_Shared::QD3D_Shared(MS_WindowTypes t)
{
priority = 3;
type = t;
//wins = new baseWin*[MaxNumWins];
num_wins = 0;
xr = 0;
yr = 0;
zr = 0;
scale = 0;
dx = 0;
dy = 0;
xrt = 0.01;
yrt = 0.012;
zrt =  0.008;
stopped = FALSE;
}

QD3D_Shared::~QD3D_Shared(void)
{
if (wins) delete wins;
}


McoStatus QD3D_Shared::AddWin(baseWin *win)
{
TQ3Matrix4x4	Rotation;

wins[num_wins] = win;

if (num_wins > 0) 
	{
	wins[0]->GetRotation(&Rotation);
	win->SetRotation(&Rotation);
	}
else win->DoTransform(WE_Rotate,xr,yr,zr);
win->DoTransform(WE_Scale,scale,0,0);
win->DoTransform(WE_Translate,dx,dy,0);

num_wins ++;
return MCO_SUCCESS;
}

Boolean QD3D_Shared::MyType(MS_WindowTypes t)
{
if (t == type) return TRUE;
if ((type == Gammut_Dialog1) && (t == Gammut_Dialog_Src)) return TRUE;
if ((type == Gammut_Dialog1) && (t == Gammut_Dialog_Dst)) return TRUE;
if ((type == Gammut_Dialog_Src) && (t == Gammut_Dialog1)) return TRUE;
if ((type == Gammut_Dialog_Src) && (t == Gammut_Dialog_Dst)) return TRUE;
if ((type == Gammut_Dialog_Dst) && (t == Gammut_Dialog1)) return TRUE;
if ((type == Gammut_Dialog_Dst) && (t == Gammut_Dialog_Src)) return TRUE;
return FALSE;
}

//void QD3DWin::DoTransform(int code, double v1, double v2);
	
McoStatus QD3D_Shared::ProcessMouseDown(Point ClickPoint,int code,baseWin *frontWin)
{
int i;
Point	where,last;
//double	xrt,yrt;
double 	scalet,dxt,dyt;

where = ClickPoint;
if (frontWin != NULL) SetPort(frontWin->dp);
GlobalToLocal(&where);
last = where;
if (code == WE_Rotate)
	{
	do {
		yrt = (where.h - last.h);
		yrt = yrt/100;
		xrt = (where.v - last.v);
		xrt = xrt/100;
		zrt = 0;
		xr += xrt;
		yr += yrt;
		for (i=0; i<num_wins; i++) 
			{
			wins[i]->DoTransform(code,xrt,yrt,0);
			wins[i]->updateWindowData(0);
			wins[i]->DrawWindow();
			}
		last = where;
		GetMouse ( &where );
		} while ( StillDown ( ) );
	}
else if (code == WE_Scale)
	{
	do {
		scalet = (where.v - last.v);
		scalet = scalet/200;
		scale += scalet;
		for (i=0; i<num_wins; i++) 
			{
			wins[i]->DoTransform(code,scalet,0,0);
			wins[i]->updateWindowData(0);
			wins[i]->DrawWindow();
			}
		last = where;
		GetMouse ( &where );
		} while ( StillDown ( ) );
	}
else if (code == WE_Translate)
	{
		do {
		dxt = (where.h - last.h);
		dxt = dxt/175;
		dyt = (where.v - last.v);
		dyt = dyt/175;
		dx += dxt;
		dy += dyt;
		for (i=0; i<num_wins; i++) 
			{
			wins[i]->DoTransform(code,dxt,dyt,0);
			wins[i]->updateWindowData(0);
			wins[i]->DrawWindow();
			}
		last = where;
		GetMouse ( &where );
		} while ( StillDown ( ) );
	}
else if (code == WE_Stop)
	{
	if (stopped)
		{
		xrt = 0.01;
		yrt = 0.012;
		zrt =  0.008;
		stopped = FALSE;
		}
	else
		{
		xrt = 0;
		yrt = 0;
		zrt = 0;
		stopped = TRUE;
		}
	}
return MCO_SUCCESS;
}



McoStatus QD3D_Shared::DoAlwaysUpdate(baseWin *frontWin)
{
int i;
//double	xrt,yrt,zrt;
//return MCO_SUCCESS;

//xrt = 0.01;
//yrt = 0.012;
//zrt =  0.008;
xr += xrt;
yr += yrt;
zr += zrt;
for (i=0; i<num_wins; i++) 
	{
	wins[i]->DoTransform(WE_Rotate,xrt,yrt,zrt);
	//wins[i]->updateWindowData();
	wins[i]->forceUpdateData();
	//wins[i]->DrawWindow();
	}
return MCO_SUCCESS;
}
