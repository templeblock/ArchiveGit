//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

static BOOL bSavedShield;

/***********************************************************************/
void Shields( BOOL bOn )
/***********************************************************************/
{
ColorMask.On = bOn;
ColorMask.Mask = ColorMask.On;
}

/***********************************************************************/
void ShieldsSave( void )
/***********************************************************************/
{
bSavedShield = ColorMask.On;
}

/***********************************************************************/
void ShieldsRestore( void )
/***********************************************************************/
{
ColorMask.On = bSavedShield;
ColorMask.Mask = ColorMask.On;
}

/************************************************************************/
void Shield(LPTR pm, LPTR pd, WORD wCount, int depth)
/************************************************************************/
{
switch (depth)
	{
	case 0:
	case 1:
		Shield8(pm, pd, wCount);
	break;

	case 3:
		Shield24(pm, (LPRGB)pd, wCount);
	break;

	case 4:
		Shield32(pm, (LPCMYK)pd, wCount);
	break;
	}
}

/************************************************************************/
void Shield8(LPTR pm, LPTR pd, WORD wCount)
/************************************************************************/
{
if ( !wCount )
	return;

while ( wCount-- != 0 )
	{
	if ( *pd++ )
		pm++; // not black
	else
		*pm++ = 0; // black
	}
}

/************************************************************************/
void Shield24(LPTR pm, LPRGB pd, WORD wCount)
/************************************************************************/
{
if ( !wCount )
	return;

while ( wCount-- != 0 )
	{
	if ( pd->red || pd->green || pd->blue )
		pm++; // not black
	else
		*pm++ = 0; // black
	pd++;
	}
}

/************************************************************************/
void Shield32(LPTR pm, LPCMYK pd, WORD wCount)
/************************************************************************/
{
if ( !wCount )
	return;

while ( wCount-- != 0 )
	{
	if ( pd->k != 255 || pd->c || pd->m || pd->y )
		pm++; // not black
	else
		*pm++ = 0; // black
	pd++;
	}
}
