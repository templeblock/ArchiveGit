//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
/*=======================================================================*\

	QMUL.C - Quick Pixel Multiplication routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifdef C_CODE
#include <windows.h>
#include <stdio.h>
#include "framelib.h"

void quick_average24(LPTR lpBuf1, LPTR lpBuf2, long area1, long area2,
						long area3, long area4, LPTR lpDst)
{
lpDst[0] = (
	lpBuf1[0] * area1 +
	lpBuf1[3] * area2 +
	lpBuf2[0] * area3 +
 	lpBuf2[3] * area4) >> 16;

lpDst[1] = (
	lpBuf1[1] * area1 +
	lpBuf1[4] * area2 +
	lpBuf2[1] * area3 +
	lpBuf2[4] * area4) >> 16;

lpDst[2] = (
	lpBuf1[2] * area1 +
	lpBuf1[5] * area2 +
	lpBuf2[2] * area3 +
	lpBuf2[5] * area4) >> 16;
}

void quick_average32(LPTR lpBuf1, LPTR lpBuf2, long area1, long area2,
						long area3, long area4, LPTR lpDst)
{
lpDst[0] = (
	lpBuf1[0] * area1 +
	lpBuf1[4] * area2 +
	lpBuf2[0] * area3 +
	lpBuf2[4] * area4) >> 16;

lpDst[1] = (
	lpBuf1[1] * area1 +
	lpBuf1[5] * area2 +
	lpBuf2[1] * area3 +
	lpBuf2[5] * area4) >> 16;

lpDst[2] = (
	lpBuf1[2] * area1 +
	lpBuf1[6] * area2 +
	lpBuf2[2] * area3 +
	lpBuf2[6] * area4) >> 16;

lpDst[3] = (
	lpBuf1[3] * area1 +
	lpBuf1[7] * area2 +
	lpBuf2[3] * area3 +
	lpBuf2[7] * area4) >> 16;
}
#endif

