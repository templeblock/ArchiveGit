// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

/* ems control variables */
static LPTR	lpEMS;
static HANDLE	Handles[16];
static int val;

/************************************************************************
	EMS pointer format:   0xFhpppppp
	1st nibble is hex F
	2nd nibble is the EMS handle
	Next 6 nibbles are the memory offset - access to 16MB
************************************************************************/

/************************************************************************/
int EMSstatus( wpTotal, wpAvail )
/************************************************************************/
LPWORD	wpTotal, wpAvail;
{
int val;

*wpTotal = 0;
*wpAvail = 0;
lpEMS = NULL;
if ( !ems_installed() )			/* Check for EMS installed */
	return( 0 );
if ( val = ems_pfba( &lpEMS ) )		/* Get the page frame address */
	{
//	Print("pfba error %d %lx", val, lpEMS );
//	return( 0 );
	}
if ( val = ems_gpc( wpTotal, wpAvail ) ) /* Get the 16K page counts */
	{
//	Print("gpc error %d %d/%d", val, *wpAvail, *wpTotal );
//	return( 0 );
	}

*wpTotal *= 16; // Pass back Kbytes
*wpAvail *= 16;

return( *wpAvail );
}

/************************************************************************/
LPTR EMSmap(lpMemory)
/************************************************************************/
LPTR lpMemory;
{
//TRYIT static ULONG lLastUpper = 0xFFFF;
ULONG lEMSaddr, lUpper;
WORD wLogicalPage, wLower;
HANDLE hMemory;

/* If it isn't an EMS pointer, return it unmodified */
if ( !IsEMS( lpMemory ) || !lpEMS )
	return( lpMemory );

lEMSaddr = (unsigned long)lpMemory;
//TRYIT lUpper = (lEMSaddr & 0xFFFF0000);
wLower = (lEMSaddr & 0xFFFF);

//TRYIT if ( lUpper != lLastUpper )
	{
//TRYIT	lLastUpper = lUpper;
	/* Calculate the logical page */
	wLogicalPage = (lEMSaddr & 0x00FF0000) >> 14; /* Divide by 16K */
	/* Get the EMS handle by shifting out the 6 offset nibbles */
	hMemory = (lEMSaddr >> 24) & 0xF;
/*	Print("EMSmap: ptr=%lx, slot=%d, handle=%d",
		lpMemory, hMemory, Handles[hMemory] );*/
	/* If hMemory is NULL at this point, we're in trouble */
	/* Map the logical page and the next three into the page frame */
	/* The ems_map calls could fail if there aren't that many pages */
	if ( hMemory = Handles[ hMemory ] )
		{
		if ( ems_map( hMemory, wLogicalPage, 0) )
			;/*Print("EMSmap0 failed");*/
		if ( ems_map( hMemory, wLogicalPage+1, 1) )
			;/*Print("EMSmap1 failed");*/
		if ( ems_map( hMemory, wLogicalPage+2, 2) )
			;/*Print("EMSmap2 failed");*/
		if ( ems_map( hMemory, wLogicalPage+3, 3) )
			;/*Print("EMSmap3 failed");*/
		}
	}
/*Print("EMSmap: seg=%lx, page=%x, ptr=%lx",
	lpEMS, wLogicalPage, lpEMS + wLower);*/
return( lpEMS + wLower );
}


/************************************************************************/
void EMSclose()
/************************************************************************/
{
int i;

for ( i=0; i<16; i++ )
	{
	if ( Handles[i] )
		{
		ems_deallocate( Handles[i] );
		Handles[i] = 0;
		}
	}
}


/************************************************************************/
LPTR EMSalloc( lCount )
/************************************************************************/
long	lCount;
{
HANDLE	hMemory;
LPTR	lpMemory;
WORD	wPages;
int	i;

if ( !lpEMS )
	return( NULL );
/* Allocate the memory from the expanded memory manager - NOT limited to 64K */
lCount += 16383L; /* Rounding */
wPages = (lCount / 16384L);

/* Get the EMS memory handle */
if ( ems_allocate( wPages, &hMemory ) )
	return( NULL );

/* Find a slot in the handles array to store the new handle */
for ( i=0; i<16; i++ )
	{
	if ( !Handles[i] )
		{
		Handles[i] = hMemory;
		break;
		}
	if ( i == 15 )
		{
		ems_deallocate( hMemory );
		return( NULL );
		}
	}

/* Build the EMS pointer */
lpMemory = (LPTR)((ULONG)0xF0000000L | (ULONG)i<<24);
return( lpMemory );
}


/************************************************************************/
VOID EMSfree( lpMemory )
/************************************************************************/
LPTR	lpMemory;
{
int	iSlot;
HANDLE	hMemory;

/* If it isn't an EMS pointer, return without doing anything */
if ( IsEMS( lpMemory ) )
	{ /* Shift out the 6 offset nibbles to get to the EMS handle */
	iSlot = ((ULONG)lpMemory >> 24) & 0xF;
	if ( hMemory = Handles[ iSlot ] )
		{
		ems_deallocate( hMemory );
		Handles[ iSlot ] = 0;
		}
	}
}
