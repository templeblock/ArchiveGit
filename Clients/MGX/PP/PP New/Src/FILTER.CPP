//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

/************************************************************************/

#define SUMNEIGHBORS(sum, xc, depth) \
{\
	xc  -= depth; \
	sum  = buf1[xc] + buf2[xc] + buf3[xc];\
	xc  += depth;\
	sum += buf1[xc] + buf3[xc];\
	xc  += depth;\
	sum += buf1[xc] + buf2[xc] + buf3[xc];\
	xc  -= depth; \
}

#define SHARPLEVEL0( sum, center, depth )\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((72 * buf2[center]) - sum + 32) >> 6;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SHARPLEVEL1( sum, center, depth )\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((40 * buf2[center]) - sum + 16) >> 5;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SHARPLEVEL2( sum, center, depth )\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((24 * buf2[center]) - sum + 8) >> 4;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SHARPLEVEL3( sum, center, depth )\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((16 * buf2[center]) - sum + 4) >> 3;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SHARPLEVEL4( sum, center, depth )\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((12 * buf2[center]) - sum + 2) >> 2;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SHARPLEVEL5( sum, center, depth )\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((10 * buf2[center]) - sum + 1) >> 1;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SHARPLEVEL6( sum, center, depth )\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((9 * buf2[center]) - sum + 0) >> 0;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SMOOTHLEVEL0( sum, center, depth)\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((120 * buf2[center]) + sum + 32) >> 7;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SMOOTHLEVEL1( sum, center, depth)\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((56 * buf2[center]) + sum + 32) >> 6;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SMOOTHLEVEL2( sum, center, depth)\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = ((24 * buf2[center]) + sum + 16) >> 5;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SMOOTHLEVEL3(sum, center, depth)\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = (( 8 * buf2[center]) + sum +  8) >> 4;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}

#define SMOOTHLEVEL4( sum, center, depth)\
{\
	SUMNEIGHBORS( sum, center, depth );\
	sum = (sum + 4) >> 3;\
	if (sum > 255) sum = 255;\
	if (sum <   0) sum = 0;\
}


/************************************************************************/

void sharp8_level0( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                     LPTR lpDst )
{
	register int sum;

	// Get the output value
	SHARPLEVEL0( sum, xc, 1);

	*lpDst = sum;
}

/************************************************************************/

void sharp8_level1( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                     LPTR lpDst )
{
	register int sum;

	// Get the output value
	SHARPLEVEL1( sum, xc, 1);

	*lpDst = sum;
}

/************************************************************************/

void sharp8_level2( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                     LPTR lpDst )
{
	register int sum;

	// Get the output value
	SHARPLEVEL2( sum, xc, 1);

	*lpDst = sum;
}

/************************************************************************/

void sharp8_level3( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                     LPTR lpDst )
{
	register int sum;

	// Get the output value
	SHARPLEVEL3( sum, xc, 1);

	*lpDst = sum;
}

/************************************************************************/

void sharp8_level4( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                     LPTR lpDst )
{
	register int sum;

	// Get the output value
	SHARPLEVEL4( sum, xc, 1);

	*lpDst = sum;
}

/************************************************************************/

void sharp8_level5( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                     LPTR lpDst )
{
	register int sum;

	// Get the output value
	SHARPLEVEL5( sum, xc, 1);

	*lpDst = sum;
}

/************************************************************************/

void sharp8_level6( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                     LPTR lpDst )
{
	register int sum;

	// Get the output value
	SHARPLEVEL6( sum, xc, 1);

	*lpDst = sum;
}

/************************************************************************/

void smooth8_level0( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;

	// Get the output value
	SMOOTHLEVEL0( sum, xc, 1 );

	*lpDst = sum;
}

void smooth8_level1( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;

	// Get the output value
	SMOOTHLEVEL1( sum, xc, 1 );

	*lpDst = sum;
}

void smooth8_level2( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;

	// Get the output value
	SMOOTHLEVEL2( sum, xc, 1 );

	*lpDst = sum;
}

void smooth8_level3( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;

	// Get the output value
	SMOOTHLEVEL3( sum, xc, 1 );

	*lpDst = sum;
}

void smooth8_level4( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;

	// Get the output value
	SMOOTHLEVEL4( sum, xc, 1 );

	*lpDst = sum;
}

/************************************************************************/

SHARPSMOOTHPROC GetSharpSmooth8(int Type, int Pressure)
{
	if (Type == IDC_SHARP)
	{ /* sharpen */
		// map 0-255 to range 0-6
		Pressure = (7 * Pressure) / 256;

		switch(Pressure)
		{
			case 0	: return(sharp8_level0);
			case 1	: return(sharp8_level1);
			case 2	: return(sharp8_level2);
			case 3	: return(sharp8_level3);
			case 4	: return(sharp8_level4);
			case 5	: return(sharp8_level5);
			case 6	: return(sharp8_level6);
		}
	return(sharp8_level0);
	}
	else	
	{ /* smooth */
		// map 0-255 to range 0-4
		Pressure = (5 * Pressure) / 256;
		switch(Pressure)
		{
			case 0	: return(smooth8_level0);
			case 1	: return(smooth8_level1);
			case 2	: return(smooth8_level2);
			case 3	: return(smooth8_level3);
			case 4	: return(smooth8_level4);
		}
	return(smooth8_level0);
	}
}

/************************************************************************/

void sharp24_level0( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SHARPLEVEL0(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SHARPLEVEL0(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SHARPLEVEL0(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp24_level1( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SHARPLEVEL1(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SHARPLEVEL1(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SHARPLEVEL1(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp24_level2( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SHARPLEVEL2(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SHARPLEVEL2(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SHARPLEVEL2(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp24_level3( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SHARPLEVEL3(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SHARPLEVEL3(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SHARPLEVEL3(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp24_level4( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SHARPLEVEL4(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SHARPLEVEL4(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SHARPLEVEL4(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp24_level5( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SHARPLEVEL5(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SHARPLEVEL5(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SHARPLEVEL5(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp24_level6( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SHARPLEVEL6(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SHARPLEVEL6(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SHARPLEVEL6(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth24_level0( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SMOOTHLEVEL0(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SMOOTHLEVEL0(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SMOOTHLEVEL0(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth24_level1( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SMOOTHLEVEL1(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SMOOTHLEVEL1(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SMOOTHLEVEL1(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth24_level2( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SMOOTHLEVEL2(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SMOOTHLEVEL2(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SMOOTHLEVEL2(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth24_level3( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SMOOTHLEVEL3(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SMOOTHLEVEL3(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SMOOTHLEVEL3(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth24_level4( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 3;

	// Get the RED output value
	SMOOTHLEVEL4(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the GREEN output value
	SMOOTHLEVEL4(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLUE output value
	SMOOTHLEVEL4(sum, xc, 3);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

SHARPSMOOTHPROC GetSharpSmooth24(int Type, int Pressure)
{
	if (Type == IDC_SHARP)
	{ /* sharpen */
		// map 0-255 to range 0-6
		Pressure = (7 * Pressure) / 256;
		switch(Pressure)
		{
			case 0	: return(sharp24_level0);
			case 1	: return(sharp24_level1);
			case 2	: return(sharp24_level2);
			case 3	: return(sharp24_level3);
			case 4	: return(sharp24_level4);
			case 5	: return(sharp24_level5);
			case 6	: return(sharp24_level6);
		}
	return(sharp24_level0);
	}
	else	
	{ /* smooth */
		// map 0-255 to range 0-4
		Pressure = (5 * Pressure) / 256;
		switch(Pressure)
		{
			case 0	: return(smooth24_level0);
			case 1	: return(smooth24_level1);
			case 2	: return(smooth24_level2);
			case 3	: return(smooth24_level3);
			case 4	: return(smooth24_level4);
		}
	return(smooth24_level0);
	}
}

/************************************************************************/

void sharp32_level0( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SHARPLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SHARPLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SHARPLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SHARPLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp32_level1( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SHARPLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SHARPLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SHARPLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SHARPLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp32_level2( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SHARPLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SHARPLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SHARPLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SHARPLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp32_level3( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SHARPLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SHARPLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SHARPLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SHARPLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp32_level4( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SHARPLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SHARPLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SHARPLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SHARPLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp32_level5( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SHARPLEVEL5(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SHARPLEVEL5(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SHARPLEVEL5(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SHARPLEVEL5(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void sharp32_level6( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SHARPLEVEL6(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SHARPLEVEL6(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SHARPLEVEL6(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SHARPLEVEL6(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth32_level0( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SMOOTHLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SMOOTHLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SMOOTHLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SMOOTHLEVEL0(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth32_level1( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SMOOTHLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SMOOTHLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SMOOTHLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SMOOTHLEVEL1(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth32_level2( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SMOOTHLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SMOOTHLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SMOOTHLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SMOOTHLEVEL2(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth32_level3( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst ) 
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SMOOTHLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SMOOTHLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SMOOTHLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SMOOTHLEVEL3(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

void smooth32_level4( register int xc, LPTR buf1, LPTR buf2, LPTR buf3,
                        LPTR lpDst )
{
	register int sum;
	xc *= 4;

	// Get the CYAN output value
	SMOOTHLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the MAGENTA output value
	SMOOTHLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the YELLOW output value
	SMOOTHLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
	xc++;

	// Get the BLACK output value
	SMOOTHLEVEL4(sum, xc, 4);

	// Store the result
	*lpDst++ = sum;
}

/************************************************************************/

SHARPSMOOTHPROC GetSharpSmooth32(int Type, int Pressure)
{

	if (Type == IDC_SHARP)
	{ /* sharpen */
		// map 0-255 to range 0-6
		Pressure = (7 * Pressure) / 256;
		switch(Pressure)
		{
			case 0	: return(sharp32_level0);
			case 1	: return(sharp32_level1);
			case 2	: return(sharp32_level2);
			case 3	: return(sharp32_level3);
			case 4	: return(sharp32_level4);
			case 5	: return(sharp32_level5);
			case 6	: return(sharp32_level6);
		}
	return(sharp32_level0);
	}
	else	
	{ /* smooth */
		// map 0-255 to range 0-4
		Pressure = (5 * Pressure) / 256;
		switch(Pressure)
		{
			case 0	: return(smooth32_level0);
			case 1	: return(smooth32_level1);
			case 2	: return(smooth32_level2);
			case 3	: return(smooth32_level3);
			case 4	: return(smooth32_level4);
		}
	return(smooth32_level0);
	}
}


/************************************************************************/

