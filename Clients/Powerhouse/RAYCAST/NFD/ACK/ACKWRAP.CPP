/******************* ( Animation Construction Kit 3D ) ******************/
/*			    Wrapup Routines				     																	*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5	 John Lundy															     		*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

/****************************************************************************
** This routine **MUST** be called by the application prior to exiting to  **
** allow XMS memory to be returned to the system. Failure to do so will	   **
** cause the XMS memory to become unavailable to other applications.	  	 **
****************************************************************************/
int AckWrapUp(ACKENG *ae)
{
int		i;

if (hLongTanTable)
{
	GlobalUnlock(hLongTanTable);
	GlobalFree(hLongTanTable);
	hLongTanTable = NULL;
}

if (hLongInvTanTable)
{
	GlobalUnlock(hLongInvTanTable);
	GlobalFree(hLongInvTanTable);
	hLongInvTanTable = NULL;
}

if (hCosTable)
{
	GlobalUnlock(hCosTable);
	GlobalFree(hCosTable);
	hCosTable = NULL;
}

if (hSinTable)
{
	GlobalUnlock(hSinTable);
	GlobalFree(hSinTable);
	hSinTable = NULL;
}

if (hInvSinTable)
{
	GlobalUnlock(hInvSinTable);
	GlobalFree(hInvSinTable);
	hInvSinTable = NULL;
}

if (hInvCosTable)
{
	GlobalUnlock(hInvCosTable);
	GlobalFree(hInvCosTable);
	hInvCosTable = NULL;
}

if (hArcCosTable)
{
	GlobalUnlock(hArcCosTable);
	GlobalFree(hArcCosTable);
	hArcCosTable = NULL;
}

if (hArcSinTable)
{
	GlobalUnlock(hArcSinTable);
	GlobalFree(hArcSinTable);
	hArcSinTable = NULL;
}

if (hxNextTable)
{
	GlobalUnlock(hxNextTable);
	GlobalFree(hxNextTable);
	hxNextTable = NULL;
}

if (hyNextTable)
{
	GlobalUnlock(hyNextTable);
	GlobalFree(hyNextTable);
	hyNextTable = NULL;
}

if (hViewCosTable)
{
	GlobalUnlock(hViewCosTable);
	GlobalFree(hViewCosTable);
	hViewCosTable = NULL;
}

if (hAdjustTable)
{
	GlobalUnlock(hAdjustTable);
	GlobalFree(hAdjustTable);
	hAdjustTable = NULL;
}

if (hBkgdBuffer)
{
	GlobalUnlock(hBkgdBuffer);
	GlobalFree(hBkgdBuffer);
	hBkgdBuffer = NULL;
}

if (hDeflectTable)
{
	GlobalUnlock(hDeflectTable);
	GlobalFree(hDeflectTable);
	hDeflectTable = NULL;
}

for (i=0 ; i < MAX_WALLBMPS ; i++)
{
	if (hbMaps[i])
	{
		GlobalUnlock(hbMaps[i]);
		GlobalFree(hbMaps[i]);
		hbMaps[i] = NULL;
	}
}

for (i=0 ; i < MAX_OBJBMPS ; i++)
{
	if (hoMaps[i])
	{
		GlobalUnlock(hoMaps[i]);
		GlobalFree(hoMaps[i]);
		hoMaps[i] = NULL;
	}
}

if (hBmScrn)
{
	GlobalUnlock(hBmScrn);
	GlobalFree(hBmScrn);
	hBmScrn = NULL;
}

return(0);
}

