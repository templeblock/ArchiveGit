/******************* ( Animation Construction Kit 3D ) ******************/
/*			  Background Routines				     																*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5  John Lundy															     		*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

void AckCeilFloor(UCHAR far *,int,int,int,int,int,int);

/****************************************************************************
** This routine attempts to build a shaded background. It is not very	   **
** successful and should probably not be used, but it is here for the	   **
** application to use if needed.					   **
**									   **
****************************************************************************/
int AckBuildBackground(ACKENG *ae)
{

/* Let the assembly routine do all the hard work */

AckCeilFloor(BkgdBuffer,
		     ae->LightFlag,
		     ae->TopColor,
		     ae->BottomColor,
		     ae->WinStartY,
		     ae->WinEndY,
		     ae->CenterRow);


return(0);
}

