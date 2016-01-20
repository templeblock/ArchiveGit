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

/************************************************************************/
long sharpsmooth8(Type, xc, buf1, buf2, buf3, strength)
/************************************************************************/
int Type;
int xc;
LPTR buf1, buf2, buf3;
int strength;
{
int x;
long sum, cpix;

sum = 0;
for (x = xc-1; x <= xc+1; ++x)
    sum += (long)buf1[x];
for (x = xc-1; x <= xc+1; ++x)
    sum += (long)buf2[x];
for (x = xc-1; x <= xc+1; ++x)
    sum += (long)buf3[x];
cpix = (long)buf2[xc];
        
sum -= cpix;

if (Type == IDC_SHARP)
    {
    if (strength == IDC_PRESSLIGHT)
        cpix = (24*cpix - sum + 8 ) >> 4; /* 1.5*cpix - sum/16 */
    if (strength == IDC_PRESSMEDIUM)
	cpix = (16*cpix - sum + 4 ) >> 3; /* 2*cpix - sum/8 */
    if (strength == IDC_PRESSHEAVY)
        cpix = (12*cpix - sum + 1 ) >> 2; /* 3*cpix - sum/4 */
    }
else	
    { /* smooth */
    if (strength == IDC_PRESSLIGHT)
	cpix = (24*cpix + sum + 16 ) >> 5; /* .75*cpix + sum/32 */
    if (strength == IDC_PRESSMEDIUM)
	cpix = (8*cpix + sum + 8 ) >> 4; /* .5*cpix + sum/16 */
    if (strength == IDC_PRESSHEAVY)
	cpix = (         sum + 4 ) >> 3; /* 0*cpix + sum/8 */
    }
cpix = bound((int)cpix, 0, 255);
return(cpix);
}

/************************************************************************/
long sharpsmooth16(Type, xc, buf1, buf2, buf3, strength)
/************************************************************************/
int Type;
int xc;
LPWORD buf1, buf2, buf3;
int strength;
{
int x, i;
long sum, cpix;
RGBS rgb;

for (i = 0; i < 3; ++i)
    {
    sum = 0;
    if (i == 0)
        {
        for (x = xc-1; x <= xc+1; ++x)
            {
            sum += MaxiR(buf1[x]);
            sum += MaxiR(buf2[x]);
            sum += MaxiR(buf3[x]);
            }
        cpix = MaxiR(buf2[xc]);
        }
    else if (i == 1)
        {
        for (x = xc-1; x <= xc+1; ++x)
            {
            sum += MaxiG(buf1[x]);
            sum += MaxiG(buf2[x]);
            sum += MaxiG(buf3[x]);
            }
        cpix = MaxiG(buf2[xc]);
        }
    else
        {
        for (x = xc-1; x <= xc+1; ++x)
            {
            sum += MaxiB(buf1[x]);
            sum += MaxiB(buf2[x]);
            sum += MaxiB(buf3[x]);
            }
        cpix = MaxiB(buf2[xc]);
        }
    sum -= cpix;

    if (Type == IDC_SHARP)
        {
        if (strength == IDC_PRESSLIGHT)
            cpix = (24*cpix - sum + 8 ) >> 4; /* 1.5*cpix - sum/16 */
        if (strength == IDC_PRESSMEDIUM)
	        cpix = (16*cpix - sum + 4 ) >> 3; /* 2*cpix - sum/8 */
        if (strength == IDC_PRESSHEAVY)
            cpix = (12*cpix - sum + 1 ) >> 2; /* 3*cpix - sum/4 */
        }
    else	
        { /* smooth */
        if (strength == IDC_PRESSLIGHT)
	        cpix = (24*cpix + sum + 16 ) >> 5; /* .75*cpix + sum/32 */
        if (strength == IDC_PRESSMEDIUM)
	        cpix = (8*cpix + sum + 8 ) >> 4; /* .5*cpix + sum/16 */
        if (strength == IDC_PRESSHEAVY)
	        cpix = (         sum + 4 ) >> 3; /* 0*cpix + sum/8 */
        }
    if (i == 0)
        rgb.red = (BYTE)bound(cpix, 0, 255);
    else if (i == 1)
        rgb.green = (BYTE)bound(cpix, 0, 255);
    else
        rgb.blue = (BYTE)bound(cpix, 0, 255);
    }
cpix = RGBtoMiniRGB(&rgb);
return(cpix);
}

/************************************************************************/
long sharpsmooth16L(Type, xc, buf1, buf2, buf3, strength)
/************************************************************************/
int Type;
int xc;
LPWORD buf1, buf2, buf3;
int strength;
{
int x;
long sum, cpix;
HSLS hsl1;

sum = 0;
for (x = xc-1; x <= xc+1; ++x)
    sum += MiniRGBtoL(buf1[x]);
for (x = xc-1; x <= xc+1; ++x)
    sum += MiniRGBtoL(buf2[x]);
for (x = xc-1; x <= xc+1; ++x)
    sum += MiniRGBtoL(buf3[x]);
MiniRGBtoHSL(buf2[xc], (LPHSL)&hsl1);
cpix = hsl1.lum;
        
sum -= cpix;

if (Type == IDC_SHARP)
    {
    if (strength == IDC_PRESSLIGHT)
        cpix = (24*cpix - sum + 8 ) >> 4; /* 1.5*cpix - sum/16 */
    if (strength == IDC_PRESSMEDIUM)
	cpix = (16*cpix - sum + 4 ) >> 3; /* 2*cpix - sum/8 */
    if (strength == IDC_PRESSHEAVY)
        cpix = (12*cpix - sum + 1 ) >> 2; /* 3*cpix - sum/4 */
    }
else	
    { /* smooth */
    if (strength == IDC_PRESSLIGHT)
	cpix = (24*cpix + sum + 16 ) >> 5; /* .75*cpix + sum/32 */
    if (strength == IDC_PRESSMEDIUM)
	cpix = (8*cpix + sum + 8 ) >> 4; /* .5*cpix + sum/16 */
    if (strength == IDC_PRESSHEAVY)
	cpix = (         sum + 4 ) >> 3; /* 0*cpix + sum/8 */
    }
hsl1.lum = (BYTE)bound(cpix, 0, 255);
cpix = HSLtoMiniRGB((LPHSL)&hsl1);
return(cpix);
}

/************************************************************************/
long sharpsmooth24(Type, xc, startBuf1, startBuf2, startBuf3, strength)
/************************************************************************/
int Type;
int xc;
LPTR startBuf1, startBuf2, startBuf3;
int strength;
{
int x, i;
long sum, cpix, color, x3;
RGBS rgb;
LPTR buf1, buf2, buf3;

for (i = 0; i < 3; ++i)
    {
    buf1 = startBuf1++;
    buf2 = startBuf2++;
    buf3 = startBuf3++;
    sum = 0;
    x3 = (xc-1) * 3L;
    for (x = xc-1; x <= xc+1; ++x, x3 += 3)
        {
        sum += buf1[x3];
        sum += buf2[x3];
        sum += buf3[x3];
        }
    cpix = (long)buf2[xc*3L];
    sum -= cpix;
    
    if (Type == IDC_SHARP)
        {
        if (strength == IDC_PRESSLIGHT)
            cpix = (24*cpix - sum + 8 ) >> 4; /* 1.5*cpix - sum/16 */
        if (strength == IDC_PRESSMEDIUM)
	        cpix = (16*cpix - sum + 4 ) >> 3; /* 2*cpix - sum/8 */
        if (strength == IDC_PRESSHEAVY)
            cpix = (12*cpix - sum + 1 ) >> 2; /* 3*cpix - sum/4 */
        }
    else	
        { /* smooth */
        if (strength == IDC_PRESSLIGHT)
	        cpix = (24*cpix + sum + 16 ) >> 5; /* .75*cpix + sum/32 */
        if (strength == IDC_PRESSMEDIUM)
	        cpix = (8*cpix + sum + 8 ) >> 4; /* .5*cpix + sum/16 */
        if (strength == IDC_PRESSHEAVY)
	        cpix = (         sum + 4 ) >> 3; /* 0*cpix + sum/8 */
        }
    if (i == 0)
        rgb.red = bound(cpix, 0, 255);
    else if (i == 1)
        rgb.green = bound(cpix, 0, 255);
    else
        rgb.blue = bound(cpix, 0, 255);
    }
CopyRGB(&rgb, &color);
return(color);
}

/************************************************************************/
long sharpsmooth24L(Type, xc, buf1, buf2, buf3, strength)
/************************************************************************/
int Type;
int xc;
LPTR buf1, buf2, buf3;
int strength;
{
int x;
long sum, cpix, color, x3;
HSLS hsl1;
RGBS rgb;
LPRGB lpRGB;

sum = 0;
for (x = xc-1; x <= xc+1; ++x)
    {
    x3 = x * 3L;
    sum += RGBtoL((LPRGB)&buf1[x3]);
    sum += RGBtoL((LPRGB)&buf2[x3]);
    sum += RGBtoL((LPRGB)&buf3[x3]);
    }
lpRGB = (LPRGB)&buf2[xc*3L];
RGBtoHSL(lpRGB->red, lpRGB->green, lpRGB->blue, (LPHSL)&hsl1);
cpix = hsl1.lum;
        
sum -= cpix;

if (Type == IDC_SHARP)
    {
    if (strength == IDC_PRESSLIGHT)
        cpix = (24*cpix - sum + 8 ) >> 4; /* 1.5*cpix - sum/16 */
    if (strength == IDC_PRESSMEDIUM)
	cpix = (16*cpix - sum + 4 ) >> 3; /* 2*cpix - sum/8 */
    if (strength == IDC_PRESSHEAVY)
        cpix = (12*cpix - sum + 1 ) >> 2; /* 3*cpix - sum/4 */
    }
else	
    { /* smooth */
    if (strength == IDC_PRESSLIGHT)
	cpix = (24*cpix + sum + 16 ) >> 5; /* .75*cpix + sum/32 */
    if (strength == IDC_PRESSMEDIUM)
	cpix = (8*cpix + sum + 8 ) >> 4; /* .5*cpix + sum/16 */
    if (strength == IDC_PRESSHEAVY)
	cpix = (         sum + 4 ) >> 3; /* 0*cpix + sum/8 */
    }
hsl1.lum = (BYTE)bound(cpix, 0, 255);
HSLtoRGB(hsl1.hue, hsl1.sat, hsl1.lum, &rgb);
CopyRGB(&rgb, &color);
return(color);
}

