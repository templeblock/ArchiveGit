// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/* samples CONTONE input pixels at the desired rate, and stuff the	*/
/* output buffer till the desired count is reached				*/

/************************************************************************/
void sample8( input, inskip, output, outskip, count, rate )
/************************************************************************/
LPTR	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
{
FIXED	offset;

offset = rate>>1;
input += inskip;

offset += ((long)outskip * rate);
count -= outskip;
while ( --count >= 0 )
	{
	*output++ = *( input + HIWORD( offset ) );
	offset += rate;
	}
}


/* samples CONTONE input pixels at the desired rate, and stuff the	*/
/* output buffer till the desired count is reached				*/
/* This routine checks a mask byte before it WRITEs pixels */

/************************************************************************/
void sample8MW( input, inskip, output, outskip, count, rate, mask )
/************************************************************************/
LPTR	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
LPTR	mask;
{
FIXED 	offset;

offset = rate>>1;
input += inskip;

offset += ((long)outskip * rate);
count -= outskip;
while ( --count >= 0 )
	{
	if ( *mask++ )
		*output++ = *( input + HIWORD( offset ) );
	else	*output++;
	offset += rate;
	}
}


/************************************************************************/
void sample16( input, inskip, output, outskip, count, rate )
/************************************************************************/
LPWORD	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
{
FIXED	offset;

offset = rate>>1;
input += inskip;

offset += ((long)outskip * rate);
count -= outskip;
while ( --count >= 0 )
	{
	*output++ = *( input + HIWORD( offset ) );
	offset += rate;
	}
}


/************************************************************************/
void sample16MW( input, inskip, output, outskip, count, rate, mask )
/************************************************************************/
LPWORD	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
LPTR	mask;
{
FIXED 	offset;

offset = rate>>1;
input += inskip;

offset += ((long)outskip * rate);
count -= outskip;
while ( --count >= 0 )
	{
	if ( *mask++ )
		*output++ = *( input + HIWORD( offset ) );
	else	output++;
	offset += rate;
	}
}


/************************************************************************/
void sample24( input, inskip, output, outskip, count, rate )
/************************************************************************/
LPTR	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
{
FIXED	offset;
LPTR pixel;

offset = rate>>1;
input += ((long)inskip*3L);
offset += ((long)outskip * rate);
count -= outskip;
while ( --count >= 0 )
	{
	pixel = input + (3L * HIWORD(offset));
	*output++ = *pixel++;
	*output++ = *pixel++;
	*output++ = *pixel;
	offset += rate;
	}
}


/************************************************************************/
void sample24MW( input, inskip, output, outskip, count, rate, mask )
/************************************************************************/
LPTR	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
LPTR	mask;
{
FIXED 	offset;
LPTR pixel;

offset = rate>>1;
input += ((long)inskip*3L);

offset += ((long)outskip * rate);
count -= outskip;
while ( --count >= 0 )
	{
	if ( *mask++ )
	    {
	    pixel = input + (3L * HIWORD(offset));
	    *output++ = *pixel++;
	    *output++ = *pixel++;
	    *output++ = *pixel;
		}
	else
	    output += 3;
	offset += rate;
	}
}


#ifdef UNUSED
/* samples LINEART input pixels at the desired rate, and stuffs the	*/
/* output buffer till the desired count is reached				*/
/************************************************************************/
void sample1( input, inskip, output, count, rate )
/************************************************************************/
LPTR	input, output;
int	inskip;
int	count;
FIXED	rate;
{
int 	offset;
USHORT 	bit;
BYTE	byte, outmask;

offset = (inskip & 7) << 16;
input += (inskip >> 3);
byte = 0;
outmask = 128;
while ( --count >= 0 )
	{
	bit = HIWORD( offset );
	if ( *(input + (bit >> 3)) & (128 >> (bit & 7)) );
		byte |= outmask;
	if ( !( outmask >>= 1 ) )
		{
		*output++ = byte;
		byte = 0;
		outmask = 128;
		}
	offset += rate;
	}

if ( outmask != 128 )
	*output = byte;
}


/************************************************************************/
void qsample8(lpIn, lpOut, x, count, XSample)
/************************************************************************/
LPTR lpIn;
LPTR lpOut;
int x;
int count;
LPINT XSample;
{
int xend;

xend = x + count - 1;
while (x <= xend)
    {
    *lpOut = lpIn[XSample[x++]];
    ++lpOut;
    }
}


/************************************************************************/
void qsample16(lpIn, lpOut, x, count, XSample)
/************************************************************************/
LPWORD lpIn;
LPWORD lpOut;
int x;
int count;
LPINT XSample;
{
int xend;

xend = x + count - 1;
while (x <= xend)
    {
    *lpOut = lpIn[XSample[x++]];
    ++lpOut;
    }
}


/************************************************************************/
void qsample24(lpIn, lpOut, x, count, XSample)
/************************************************************************/
LPTR lpIn;
LPTR lpOut;
int x;
int count;
LPINT XSample;
{
int xend;
LPTR pixel;

xend = x + count - 1;
while (x <= xend)
    {
    pixel = lpIn + (XSample[x++]*3L);
    *lpOut++ = *pixel++;
    *lpOut++ = *pixel++;
    *lpOut++ = *pixel;
    }
}


/************************************************************************/
void csample16( input, inskip, output, outskip, count, rate )
/************************************************************************/
LPWORD	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
{
FIXED	offset;

offset = rate>>1;
input += inskip;

offset += ((long)outskip * rate);
count -= outskip;
while ( --count >= 0 )
	{
	*output++ = *( input + HIWORD( offset ) );
	offset += rate;
	}
}
#endif
