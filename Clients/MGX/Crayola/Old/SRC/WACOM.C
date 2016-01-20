//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "tablet.h"

static int iMax, iMin;
static FARPROC lpSetTabletParms;
typedef BOOL (_far *GETDATAPROC)(LPTDATA);
static GETDATAPROC lpGetTabletData;
typedef BOOL (_far *GETPARMSPROC)(LPTPARMS);
static GETPARMSPROC lpGetTabletParms;

/***********************************************************************/
BOOL WacStart()
/***********************************************************************/
{
HMODULE hMouse;

if ( !(hMouse = GetModuleHandle( "MOUSE" )) )  // see if already resident
	return(FALSE);

lpGetTabletData = (GETDATAPROC)GetProcAddress( hMouse, MAKEINTRESOURCE(5) );
lpGetTabletParms = (GETPARMSPROC)GetProcAddress( hMouse, MAKEINTRESOURCE(6) );
lpSetTabletParms = GetProcAddress( hMouse, MAKEINTRESOURCE(7) );
if (lpGetTabletData && lpGetTabletParms && lpSetTabletParms)
	return(Control.UseWacom);
else
	return(FALSE);
}

/***********************************************************************/
void WacEnd()
/***********************************************************************/
{
}

/***********************************************************************/
BOOL WacInit()
/***********************************************************************/
{
TPARMS Tablet_Parms;
char t;

if (!lpGetTabletParms)
	return(FALSE);
if (!(*lpGetTabletParms)((LPTPARMS)&Tablet_Parms))
	return(FALSE);
// 32 is max pressure for tablet
iMax = 32;
// 7-bit signed data
// sensitivity is the threshold for changing from
// mouse down to up and up to down
t = Tablet_Parms.sensitivity;
if (t & 0x40)
	t |= 0x80;
iMin = t;
iMin += 2;
if (iMin > iMax)
	iMin = iMax;
// iMax now equal our maximum possible value
iMax -= iMin;
return(Tablet_Parms.pressure != 0);
}

/***********************************************************************/
int WacPressure()
/***********************************************************************/
{
TDATA Tablet_Data;
int iPressure;
char p;

if (!(*lpGetTabletData)((LPTDATA)&Tablet_Data))
	return(0);
if (Tablet_Data.contact)
	{
	// 7-bit signed data
	p = Tablet_Data.pres_val;
	if (p & 0x40)
		p |= 0x80;
	iPressure = p;
	// subtract off minimum value
	iPressure -= iMin;
	}
else
	{
	iPressure = 0;
	}
// scale pressure from -32 to 32 to 0 to 100
// taking into account the threshold, because we
// will only get pressures from the threshold to 32
iPressure = (iPressure*100)/iMax;

//	PrintStatus("iPressure = %d iThreshold = %d p = %d Button = %d", iPressure, iMin, (int)p, Button);
iPressure = bound(iPressure, 0, 100);
return(iPressure);
}
