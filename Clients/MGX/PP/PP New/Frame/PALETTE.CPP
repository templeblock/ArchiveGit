//®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "mgxframe.h"
#include "frameint.h"

LOCAL BYTE best_neighbor(LPRGB lpRGBmap, LPINT neighbor, int num, int r, int g, int b);
LOCAL LPTR CreatePaletteQLut15(LPRGB lpRGBmap, int nCubes, LPTR lpOutLut, STATUSCALLBACKPROC lpStatusCallback);
LOCAL LPTR OldCreatePaletteLut15(LPRGB lpRGBmap, int nCubes, LPTR lpLut15, STATUSCALLBACKPROC lpStatusCallback);

//************************************************************************
void CreateErrorLut15(LPRGB lpRGBmap, LPTR lpTable, LPTR lpErrorTable, BOOL bDither) 
//************************************************************************
{
WORD wRGB, wIndex;
int r, g, b;
RGBS rgb;

for ( wRGB=0; wRGB<=INT_MAX; wRGB++ )
	{
	// get the original index
	wIndex = lpTable[wRGB];

	if ( bDither )
		{
		// get desired color for this entry
		r = MaxiR(wRGB);
		g = MaxiG(wRGB);
		b = MaxiB(wRGB);

		// add in the error to the desired color; error=desired-actual
		r += (r - (int)lpRGBmap[wIndex].red);
		g += (g - (int)lpRGBmap[wIndex].green);
		b += (b - (int)lpRGBmap[wIndex].blue);

		// bound color value
		rgb.red   = mbound( r, 0, 255 );
		rgb.green = mbound( g, 0, 255 );
		rgb.blue  = mbound( b, 0, 255 );

		// convert to MiniRGB to create a new table index
		wIndex = lpTable[ RGBtoMiniRGB(&rgb) ];
		}
	lpErrorTable[wRGB] = ( char )wIndex;
	}
}



#define hindex(r,g,b)	(((((b)<<5L) | (g))<<5L) | (r))

//************************************************************************
// This is an optimized version of CreatePaletteQLut15.  
// It uses pointers instrad of doing indexes.
// It also checks fewer neighbors (with little or no apparent loss of quality. 
//************************************************************************
LPTR CreatePaletteLut15(LPRGB lpRGBmap, int nCubes, LPTR lpOutLut, STATUSCALLBACKPROC lpStatusCallback) 
//************************************************************************
{
	LPTR lpLut, lpVisit;
 	LPTR Line000, Line010, Line001; // Line011;
 	LPTR Visit000, Visit010, Visit001; // Visit011;
	int neighbor[8];
	int num;
	long index, myindex;
	int item;
	int r,g,b;
	
//	DWORD t1, t2;
//	STRING szTime;

//	t1 = GetTickCount();
	#ifdef UNUSED
   	if (SHIFT)
	{
		lpLut = OldCreatePaletteLut15(lpRGBmap,nCubes,lpOutLut,lpStatusCallback);
//		t2 = GetTickCount();
//		wsprintf(szTime, "%ld", (LONG)(t2-t1));
//		MessageBox(NULL, szTime, NULL, MB_OK);
		return(lpLut);
	}
   	else if (CONTROL)
	{
		lpLut = CreatePaletteQLut15(lpRGBmap,nCubes,lpOutLut,lpStatusCallback);
//		t2 = GetTickCount();
//		wsprintf(szTime, "%ld", (LONG)(t2-t1));
//		MessageBox(NULL, szTime, NULL, MB_OK);
		return(lpLut);
	}
	#endif
	
	
	if (!(lpLut = lpOutLut))
	{
		lpLut = Alloc(32768L);
		if (!lpLut)
			return(NULL);
	}
	lpVisit = Alloc(32768L);
	if (!lpVisit)
	{
		if (!lpOutLut)
			FreeUp(lpLut);
		return(NULL);
	}
	clr(lpVisit, 32768L);
	
	// set all base points
	for (item = 0 ;item<nCubes;item++)
	{
		r = lpRGBmap[item].red>>3;
		g = lpRGBmap[item].green>>3;
		b = lpRGBmap[item].blue>>3;
		lpVisit[index = hindex(r,g,b)] = TRUE;
		lpLut[index] = item;
	}

#define ADDA(roff, gbindex) \
{\
	if(*(Visit##gbindex - roff)){\
		item = *(Line##gbindex - roff);\
		for(index=0;index<num;index++){\
			if(item==neighbor[index]) break;}\
		if(index>=num){neighbor[num]=item;num++;}\
	}\
}\


	
	// go through from 0,0,0 to 31,31,31
	Line010 =  Line001 /* =Line011*/ = NULL;
	Visit010 = Visit001 /*=Visit011*/ = NULL;
	for (b=0;b<32;b++)	
	{
		// setup r,g,b-1 line
		if (b>0)
		{
			Visit001 = &lpVisit[myindex = hindex(0,0,b-1)];
			Line001  = &lpLut[myindex];
		}
		for (g=0;g<32;g++)	
		{
			// setup lines
			if (g>0)
			{
				// setup r,g-1,b
				Visit010 = &lpVisit[myindex = hindex(0,g-1,b)];
				Line010  = &lpLut[myindex];
//				if (b>0)
//				{
//					// setup r,g-1,b-1
//					Visit011 = &lpVisit[myindex = hindex(0,g-1,b-1)];
//					Line011  = &lpLut[myindex];
//				}
//				else
//					Line011 = NULL;
				
			}
			else
			{
				Line010 = NULL;
//				Line011 = NULL;
			}
			Visit000 = &lpVisit[myindex = hindex(0,g,b)];
			Line000  = &lpLut[myindex];
			for (r=0;r<32;r++)	
			{
				// get all valid neighbors 
				// add will mess with: num, neighbor, item, index;
				num = 0;
				// leave original points alone
				if (!*Visit000)
				{

					// check neighbors
					if (Line001)
						ADDA(0, 001);  	//(r,g,b-1)
					if (Line010)
						ADDA(0, 010);	//(r,g-1,b)
//					if (Line011) 
//						ADDA(0, 011);	//(r,g-1,b-1)
					if (r>0)
					{
						ADDA(1, 000);		//(r-1,g,b)
//						if (Line001)
//							ADDA(1, 001);	//(r-1,g,b-1)
//						if (Line010)
//							ADDA(1, 010);	//(r-1,g-1,b)
//						if (Line011)
//							ADDA(1, 011);	//(r-1,g-1,b-1)
					}
					if (num >= 1)
					{
						*Visit000 = TRUE;
						if (num == 1)
							*Line000 = neighbor[0];
						else
						   	*Line000 = best_neighbor(lpRGBmap, neighbor, num, r, g, b);
					}
				}
				Line000++;
				Visit000++;
				if (Line010){Line010++;Visit010++;}
				if (Line001){Line001++;Visit001++;}
//				if (Line011){Line011++;Visit011++;}
			}
		}
		if (lpStatusCallback)
			(*lpStatusCallback)((int)b, (int)64, NO);
	}

#define ADDB(roff, gbindex) \
{\
	item = *(Line##gbindex + roff);\
	for(index=0;index<num;index++){\
		if(item==neighbor[index]) break;}\
	if(index>=num){neighbor[num]=item;num++;}\
}\

	Line010 =  Line001 /*=Line011*/ = NULL;
	// go through from 31,31,31 to 0,0,0
	for (b=31;b>=0;b--)	
	{
		// setup r,g,b+1 line
		if (b<31)
			Line001 = &lpLut[hindex(31,31,b+1)];
		for (g=31;g>=0;g--)	
		{
			// setup lines
			if (g<31)
			{
				// setup r,g+1,b
				Line010 = &lpLut[hindex(31,g+1,b)];
				// setup r,g+1,b+1
//				if (b<31)
//					Line011 = &lpLut[hindex(31,g+1,b+1)];
//				else
//					Line011 = NULL;
			}
			else
			{
				Line010 = NULL;
//				Line011 = NULL;
			}
			Line000 = &lpLut[hindex(31,g,b)];
			for (r=31;r>=0;r--)	
			{
				// get all valid neighbors 
				num = 0;
				// check neighbors
				ADDB(0, 000);		//(r,g,b)
				if (Line001)
					ADDB(0, 001);  	//(r,g,b+1)
				if (Line010)
					ADDB(0, 010);	//(r,g+1,b)
//				if (Line011) 
//					ADDB(0, 011);	//(r,g+1,b+1)
				if (r<31)
				{
					ADDB(1, 000);		//(r+1,g,b)
//					if (Line001)
//						ADDB(1, 001);	//(r+1,g,b+1)
//					if (Line010)
//						ADDB(1, 010);	//(r+1,g+1,b)
//					if (Line011)
//						ADDB(1, 011);	//(r+1,g+1,b+1)
				}
				if (num == 1)
					*Line000 = neighbor[0];
				else
					*Line000 = best_neighbor(lpRGBmap, neighbor, num, r, g, b);
				Line000--;
				if (Line010){Line010--;}
				if (Line001){Line001--;}
//				if (Line011){Line011--;}
			}
		}
		if (lpStatusCallback)
			(*lpStatusCallback)((int)64-b, (int)64, NO);
	}
//Done:
	FreeUp(lpVisit);
// 	t2 = GetTickCount();
//	wsprintf(szTime, "%ld", (LONG)(t2-t1));
//	MessageBox(NULL, szTime, NULL, MB_OK);
	return(lpLut);
}

LOCAL BYTE best_neighbor(LPRGB lpRGBmap, LPINT neighbor, int num, int r, int g, int b)
{
	long best, dist, delta;
	int index, found; 
	
	r = (r<<3) + 4;
	g = (g<<3) + 4;
	b = (b<<3) + 4;
	
	best = LONG_MAX;
	
	while(num--)
	{	
		index = neighbor[num];
		dist = lpRGBmap[index].red-r;
		dist *= dist;
		if (dist > best)
			continue;
		delta = lpRGBmap[index].green-g;
		dist += delta*delta;
		if (dist > best)
			continue;
		delta = lpRGBmap[index].blue-b;
		dist += delta*delta;
		if (dist > best)
			continue;
		best = dist;
		found = index;
	}
	return(found);
}


//************************************************************************
// This is the alogorithm that CreatePaletteLut15 is based on.
// 1: We set the starting palette enties in the table.
// 2: We run through the cube from 0,0,0 to 31,31,31,
//		looking at out neighbors 'below' (in the r,g,& b directions).
// 		We set ourself to the 'closest' palette point found in a neighbor.
// 3: We run through the cube from 31,31,31 to 0,0,0, doing the same.
//************************************************************************
LOCAL LPTR CreatePaletteQLut15(LPRGB lpRGBmap, int nCubes, LPTR lpOutLut, STATUSCALLBACKPROC lpStatusCallback) 
//************************************************************************
{
	LPTR lpLut, lpVisit;
	int neighbor[8];
	int num;
	long index, myindex;
	int item;
	int r,g,b;
	
	if (!(lpLut = lpOutLut))
	{
		lpLut = Alloc(32768L);
		if (!lpLut)
			return(NULL);
	}
	lpVisit = Alloc(32768L);
	if (!lpVisit)
	{
		if (!lpOutLut)
			FreeUp(lpLut);
		return(NULL);
	}
	clr(lpVisit, 32768L);
	
	// set all base points
	for (item = 0 ;item<nCubes;item++)
	{
		r = lpRGBmap[item].red>>3;
		g = lpRGBmap[item].green>>3;
		b = lpRGBmap[item].blue>>3;
		lpVisit[index = hindex(r,g,b)] = TRUE;
		lpLut[index] = item;
	}

#define ADD1(a,b,c) \
if ((a)>=0 && (b)>=0 && (c)>=0){\
	if(lpVisit[index = hindex((a),(b),(c))]){\
		item = lpLut[index];\
		for(index=0;index<num;index++){\
			if(item==neighbor[index]) break;}\
		if(index>=num){neighbor[num]=item;num++;}\
	}\
}\


	
	// go through from 0,0,0 to 31,31,31
	for (b=0;b<32;b++)	
	{
		for (g=0;g<32;g++)	
		{
			for (r=0;r<32;r++)	
			{
				// get all valid neighbors 
				// add will mess with: num, neighbor, item, index;
				num = 0;
				// leave original points alone
				if(lpVisit[myindex = hindex(r,g,b)])
					continue;
				ADD1(r,g,b-1);
				ADD1(r,g-1,b);
				ADD1(r,g-1,b-1);
				ADD1(r-1,g,b);
				ADD1(r-1,g,b-1);
				ADD1(r-1,g-1,b);
				ADD1(r-1,g-1,b-1);
				if (num < 1)
					continue;
				lpVisit[myindex] = TRUE;
				if (num == 1)
				{
					lpLut[myindex] = neighbor[0];
					continue;
				}
				lpLut[myindex] = best_neighbor(lpRGBmap, neighbor, num, r, g, b);
			}
		}
		if (lpStatusCallback)
			(*lpStatusCallback)((int)b, (int)64, NO);
	}

#define ADD2(a,b,c) \
if ((a)<32 && (b)<32 && (c)<32){\
	item = lpLut[hindex((a),(b),(c))];\
	for(index=0;index<num;index++){\
		if(item==neighbor[index]) break;}\
	if(index>=num){neighbor[num]=item;num++;}\
}\

	// go through from 31,31,31 to 0,0,0
	for (b=31;b>=0;b--)	
	{
		for (g=31;g>=0;g--)	
		{
			for (r=31;r>=0;r--)	
			{
				// get all valid neighbors 
				num = 0;
				ADD2(r,g,b);
				ADD2(r,g,b+1);
				ADD2(r,g+1,b);
				ADD2(r,g+1,b+1);
				ADD2(r+1,g,b);
				ADD2(r+1,g,b+1);
				ADD2(r+1,g+1,b);
				ADD2(r+1,g+1,b+1);
				if (num == 1)
				{
					lpLut[hindex(r,g,b)] = neighbor[0];
					continue;
				}
				lpLut[hindex(r,g,b)] = best_neighbor(lpRGBmap, neighbor, num, r, g, b);
			}
		}
		if (lpStatusCallback)
			(*lpStatusCallback)((int)64-b, (int)64, NO);
		
	}
//Done:
	FreeUp(lpVisit);
	return(lpLut);
}





//THIS CREATES THE PALETEE LOOKUP USING THE OLD METHOD.... SLOW & POOR
//************************************************************************
LOCAL LPTR OldCreatePaletteLut15(LPRGB lpRGBmap, int nCubes, LPTR lpLut15, STATUSCALLBACKPROC lpStatusCallback) 
//************************************************************************
{
int j, i;
RGBS rgb1, rgb2;
LPTR lpLut, lpVisit, lpTmpVisit;
BYTE Grow[256];
WORD wCount, wRGB;
BYTE x, y, z;

if (!(lpLut = lpLut15))
	{
	lpLut = Alloc(32768L);
	if (!lpLut)
		return(NULL);
	}
lpVisit = Alloc(32768L);
if (!lpVisit)
	{
	if (!lpLut15)
		FreeUp(lpLut);
	return(NULL);
	}
clr(lpVisit, 32768L);


wCount = 32768; // the number of table entries left to set
j = -1; // the amount of growth around each palette color
for (i = 0; i < nCubes; ++i)
	Grow[i] = YES;
// Loop until we have no more entries to set
while ( wCount )
	{
	// Seed the table with the palette we just created; grow a cube
	// around each color it until all 32k values are set
	j++;
	for ( i=0; i<nCubes; i++ )
		{
		if (!Grow[i])
			continue;

		rgb2 = lpRGBmap[i];

		rgb2.red >>= 3;
		rgb1.red = bound( (int)rgb2.red - j, 0, 31 );
		rgb2.red = bound( (int)rgb2.red + j, 0, 31 );

		rgb2.green >>= 3;
		rgb1.green = bound( (int)rgb2.green - j, 0, 31 );
		rgb2.green = bound( (int)rgb2.green + j, 0, 31 );

		rgb2.blue >>= 3;
		rgb1.blue = bound( (int)rgb2.blue - j, 0, 31 );
		rgb2.blue = bound( (int)rgb2.blue + j, 0, 31 );

		Grow[i] = NO;
		for ( x=rgb1.red;   x<=rgb2.red;   x++ )
		for ( y=rgb1.green; y<=rgb2.green; y++ )
		for ( z=rgb1.blue;  z<=rgb2.blue;  z++ )
			{
			wRGB = ToMiniRGB(x, y, z);
			lpTmpVisit = lpVisit + wRGB;
			if (*lpTmpVisit)
				continue;
			Grow[i] = YES;
			*lpTmpVisit = YES;
			// if we haven't visited this entry yet...
			lpLut[wRGB] = i;
			if ( !(--wCount) )
				goto Done;
			if (lpStatusCallback && !(wCount&0x00fff))
				{
					(*lpStatusCallback)((int)(16384-(wCount>>2L)), (int)16384, NO);
				}
			}
		}
	}
Done:
FreeUp(lpVisit);
return(lpLut);
}
#ifdef UNUSED

THIE FOLLOWING STUFF CREATES THE PALETEE LOOKUP USING THE JPEG METHOD
IT GIVES HIGH QUALITY, AND IS FASTER FOR SMALL PALETTES, BUT GETS SLOWER FOR LARGE ONES
WARNING!! THIS USES 0 as a flag entry.  SO IT ONLY WORKS FOR 255 or less colors.  

typedef unsigned char JSAMPLE;
#define GETJSAMPLE(value)  (value)
#define BITS_IN_JSAMPLE   8
#define MAXJSAMPLE	255
#define CENTERJSAMPLE	128

#define MAXNUMCOLORS  (MAXJSAMPLE+1) /* maximum size of colormap */

#define Y_SCALE 1		/* scale Y distances up by this much */
#define HIST_C_BITS  5		/* bits of precision in Cb/Cr xhistogram */
#define HIST_Y_BITS  5		/* bits of precision in Cb/Cr xhistogram */
#define C_SHIFT  (BITS_IN_JSAMPLE-HIST_C_BITS)

#define HIST_Y_ELEMS  (1<<HIST_Y_BITS) /* # of elements along xhistogram axes */
#define HIST_C_ELEMS  (1<<HIST_C_BITS)

#define BOX_Y_LOG  (HIST_C_BITS-3) /* log2(hist cells in update box, Y axis) */
#define BOX_C_LOG  (HIST_C_BITS-3) /* log2(hist cells in update box, C axes) */

#define BOX_Y_ELEMS  (1<<BOX_Y_LOG) /* # of hist cells in update box */
#define BOX_C_ELEMS  (1<<BOX_C_LOG)

#define BOX_Y_SHIFT  (C_SHIFT + BOX_Y_LOG)
#define BOX_C_SHIFT  (C_SHIFT + BOX_C_LOG)




typedef JSAMPLE histcell;	/* xhistogram cell; MUST be an unsigned type */
typedef histcell FAR * histptr;	/* for pointers to xhistogram cells */

//typedef histcell hist1d[HIST_C_ELEMS]; /* typedefs for the array */
//typedef hist1d hist2d[HIST_C_ELEMS];	/* type for the Y-level pointers */
//typedef hist2d hist3d[HIST_Y_ELEMS];	/* type for top-level pointer */

//#define getxhistogram(r,g,b)	xhistogram[(b)*(long)HIST_C_ELEMS*HIST_C_ELEMS + (g)*(long)HIST_C_ELEMS + (r)]
#define getxhistogram(r,g,b)	xhistogram[((((b)<<HIST_C_BITS) | (g))<<HIST_C_BITS) | (r)]

static LPRGB my_colormap;
//static histgram[HIST_C_ELEMS][HIST_C_ELEMS][HIST_C_ELEMS];

LOCAL LPTR color_quant_init ();
LOCAL int find_nearby_colors (int actualnumcolors, int minc0, int minc1, int minc2, JSAMPLE colorlist[]);
LOCAL void find_best_colors (int minc0, int minc1, int minc2, int numcolors, JSAMPLE colorlist[], JSAMPLE bestcolor[]);
LOCAL void fill_inverse_cmap (LPTR xhistogram, int actualnumcolors, int c0, int c1, int c2);


LPTR CreatePaletteLut15NEW1(LPRGB lpRGBmap, int nCubes, LPTR lpOutLut, STATUSCALLBACKPROC lpStatusCallback) 
{
	int r,g,b;
	LONG l;
  	histptr cachep;
	LPTR xhistogram;

	my_colormap = lpRGBmap; 
	// allocate buffers
	if (lpOutLut)
	{
		xhistogram = lpOutLut;
		clr(lpOutLut, (long)HIST_C_ELEMS*HIST_C_ELEMS*HIST_Y_ELEMS*sizeof(histcell));
	}
	else
	{
		xhistogram = color_quant_init ();
	}
	for (r = 0; r < HIST_C_ELEMS;r++)
	{
		for (g = 0; g < HIST_C_ELEMS;g++)
		{
			for (b = 0; b < HIST_C_ELEMS;b++)
			{
    			  /* If we have not seen this color before, find nearest colormap entry */
    			  /* and update the cache */
    			  if (getxhistogram(r,g,b) == 0)
						fill_inverse_cmap(xhistogram, nCubes, b,g,r);
				  if (getxhistogram(r,g,b) == 0)
				  		MessageBeep(-1);
			}
		}
	}
	
	// make all indexes real
	l = (LONG)HIST_Y_ELEMS*HIST_C_ELEMS*HIST_C_ELEMS;
	cachep = &getxhistogram(0,0,0);
	while(l--)
		(*cachep++)--;
		
	return(xhistogram);
}



/*
 * The next three routines implement inverse colormap filling.  They could
 * all be folded into one big routine, but splitting them up this way saves
 * some stack space (the mindist[] and bestdist[] arrays need not coexist)
 * and may allow some compilers to produce better code by registerizing more
 * inner-loop variables.
 */

//************************************************************************
LOCAL LPTR color_quant_init ()
//************************************************************************
{
	LPTR xhistogram;	/* pointer to the xhistogram */
//  	int i;
//	LPTR ptr;

  	/* Allocate and zero the xhistogram */
	xhistogram = (histptr)AllocX((long)HIST_C_ELEMS*HIST_C_ELEMS*HIST_Y_ELEMS*sizeof(histcell), GMEM_ZEROINIT);
//  	ptr = AllocX(HIST_Y_ELEMS*HIST_C_ELEMS*HIST_C_ELEMS*SIZEOF(histcell),
//  				 GMEM_ZEROINIT);
//	xhistogram = (hist3d*)ptr;
//	ptr += HIST_Y_ELEMS * SIZEOF(hist2d);
//  	for (i = 0; i < HIST_Y_ELEMS; i++) 
//  	{
//  		xhistogram[i] = (hist2d)ptr;
//		ptr += HIST_C_ELEMS*HIST_C_ELEMS*SIZEOF(histcell);
//  	}
	return(xhistogram);
}



//************************************************************************
/* Locate the colormap entries close enough to an update box to be candidates
 * for the nearest entry to some cell(s) in the update box.  The update box
 * is specified by the center coordinates of its first cell.  The number of
 * candidate colormap entries is returned, and their colormap indexes are
 * placed in colorlist[].
 * This routine uses Heckbert's "locally sorted search" criterion to select
 * the colors that need further consideration.
 */
//************************************************************************
LOCAL int find_nearby_colors (int actualnumcolors, int minc0, int minc1, int minc2,
		    JSAMPLE colorlist[])
//************************************************************************
{
//  int numcolors = cinfo->actual_number_of_colors;
  int maxc0, maxc1, maxc2;
  int centerc0, centerc1, centerc2;
  int i, x, ncolors, numcolors;
  INT32 minmaxdist, min_dist, max_dist, tdist;
  INT32 mindist[MAXNUMCOLORS];	/* min distance to colormap entry i */
	
	numcolors =  actualnumcolors;
  /* Compute true coordinates of update box's upper corner and center.
   * Actually we compute the coordinates of the center of the upper-corner
   * xhistogram cell, which are the upper bounds of the volume we care about.
   * Note that since ">>" rounds down, the "center" values may be closer to
   * min than to max; hence comparisons to them must be "<=", not "<".
   */
  maxc0 = minc0 + ((1 << BOX_C_SHIFT) - (1 << C_SHIFT));
  centerc0 = (minc0 + maxc0) >> 1;
  maxc1 = minc1 + ((1 << BOX_C_SHIFT) - (1 << C_SHIFT));
  centerc1 = (minc1 + maxc1) >> 1;
  maxc2 = minc2 + ((1 << BOX_C_SHIFT) - (1 << C_SHIFT));
  centerc2 = (minc2 + maxc2) >> 1;

  /* For each color in colormap, find:
   *  1. its minimum squared-distance to any point in the update box
   *     (zero if color is within update box);
   *  2. its maximum squared-distance to any point in the update box.
   * Both of these can be found by considering only the corners of the box.
   * We save the minimum distance for each color in mindist[];
   * only the smallest maximum distance is of interest.
   * Note we have to scale Y to get correct distance in scaled space.
   */
  minmaxdist = 0x7FFFFFFFL;

  for (i = 0; i < numcolors; i++) {
    /* We compute the squared-c0-distance term, then add in the other two. */
    x = GETJSAMPLE(my_colormap[i].blue);
    if (x < minc0) {
      tdist = (x - minc0) * Y_SCALE;
      min_dist = tdist*tdist;
      tdist = (x - maxc0) * Y_SCALE;
      max_dist = tdist*tdist;
    } else if (x > maxc0) {
      tdist = (x - maxc0) * Y_SCALE;
      min_dist = tdist*tdist;
      tdist = (x - minc0) * Y_SCALE;
      max_dist = tdist*tdist;
    } else {
      /* within cell range so no contribution to min_dist */
      min_dist = 0;
      if (x <= centerc0) {
	tdist = (x - maxc0) * Y_SCALE;
	max_dist = tdist*tdist;
      } else {
	tdist = (x - minc0) * Y_SCALE;
	max_dist = tdist*tdist;
      }
    }

    x = GETJSAMPLE(my_colormap[i].green);
    if (x < minc1) {
      tdist = x - minc1;
      min_dist += tdist*tdist;
      tdist = x - maxc1;
      max_dist += tdist*tdist;
    } else if (x > maxc1) {
      tdist = x - maxc1;
      min_dist += tdist*tdist;
      tdist = x - minc1;
      max_dist += tdist*tdist;
    } else {
      /* within cell range so no contribution to min_dist */
      if (x <= centerc1) {
	tdist = x - maxc1;
	max_dist += tdist*tdist;
      } else {
	tdist = x - minc1;
	max_dist += tdist*tdist;
      }
    }

    x = GETJSAMPLE(my_colormap[i].red);
    if (x < minc2) {
      tdist = x - minc2;
      min_dist += tdist*tdist;
      tdist = x - maxc2;
      max_dist += tdist*tdist;
    } else if (x > maxc2) {
      tdist = x - maxc2;
      min_dist += tdist*tdist;
      tdist = x - minc2;
      max_dist += tdist*tdist;
    } else {
      /* within cell range so no contribution to min_dist */
      if (x <= centerc2) {
	tdist = x - maxc2;
	max_dist += tdist*tdist;
      } else {
	tdist = x - minc2;
	max_dist += tdist*tdist;
      }
    }

    mindist[i] = min_dist;	/* save away the results */
    if (max_dist < minmaxdist)
      minmaxdist = max_dist;
  }

  /* Now we know that no cell in the update box is more than minmaxdist
   * away from some colormap entry.  Therefore, only colors that are
   * within minmaxdist of some part of the box need be considered.
   */
  ncolors = 0;
  for (i = 0; i < numcolors; i++) {
    if (mindist[i] <= minmaxdist)
      colorlist[ncolors++] = (JSAMPLE) i;
  }
  return ncolors;
}

//************************************************************************
/* Find the closest colormap entry for each cell in the update box,
 * given the list of candidate colors prepared by find_nearby_colors.
 * Return the indexes of the closest entries in the bestcolor[] array.
 * This routine uses Thomas' incremental distance calculation method to
 * find the distance from a colormap entry to successive cells in the box.
 */
//************************************************************************
LOCAL void find_best_colors (int minc0, int minc1, int minc2,
		  int numcolors, JSAMPLE colorlist[], JSAMPLE bestcolor[])
//************************************************************************
{
  int ic0, ic1, ic2;
  int i, icolor;
  INT32 * bptr;	/* pointer into bestdist[] array */
  JSAMPLE * cptr;		/* pointer into bestcolor[] array */
  INT32 dist0, dist1;		/* initial distance values */
  INT32 dist2;		/* current distance in inner loop */
  INT32 xx0, xx1;		/* distance increments */
  INT32 xx2;
  INT32 inc0, inc1, inc2;	/* initial values for increments */
  /* This array holds the distance to the nearest-so-far color for each cell */
  INT32 bestdist[BOX_Y_ELEMS * BOX_C_ELEMS * BOX_C_ELEMS];

  /* Initialize best-distance for each cell of the update box */
  bptr = bestdist;
  for (i = BOX_Y_ELEMS*BOX_C_ELEMS*BOX_C_ELEMS-1; i >= 0; i--)
    *bptr++ = 0x7FFFFFFFL;
  
  /* For each color selected by find_nearby_colors,
   * compute its distance to the center of each cell in the box.
   * If that's less than best-so-far, update best distance and color number.
   * Note we have to scale Y to get correct distance in scaled space.
   */
  
  /* Nominal steps between cell centers ("x" in Thomas article) */
#define STEP_Y  ((1 << C_SHIFT) * Y_SCALE)
#define STEP_C  (1 << C_SHIFT)
  
  for (i = 0; i < numcolors; i++) {
    icolor = GETJSAMPLE(colorlist[i]);
    /* Compute (square of) distance from minc0/c1/c2 to this color */
    inc0 = (minc0 - (int) GETJSAMPLE(my_colormap[icolor].blue)) * Y_SCALE;
    dist0 = inc0*inc0;
    inc1 = minc1 - (int) GETJSAMPLE(my_colormap[icolor].green);
    dist0 += inc1*inc1;
    inc2 = minc2 - (int) GETJSAMPLE(my_colormap[icolor].red);
    dist0 += inc2*inc2;
    /* Form the initial difference increments */
    inc0 = inc0 * (2 * STEP_Y) + STEP_Y * STEP_Y;
    inc1 = inc1 * (2 * STEP_C) + STEP_C * STEP_C;
    inc2 = inc2 * (2 * STEP_C) + STEP_C * STEP_C;
    /* Now loop over all cells in box, updating distance per Thomas method */
    bptr = bestdist;
    cptr = bestcolor;
    xx0 = inc0;
    for (ic0 = BOX_Y_ELEMS-1; ic0 >= 0; ic0--) {
      dist1 = dist0;
      xx1 = inc1;
      for (ic1 = BOX_C_ELEMS-1; ic1 >= 0; ic1--) {
	dist2 = dist1;
	xx2 = inc2;
	for (ic2 = BOX_C_ELEMS-1; ic2 >= 0; ic2--) {
	  if (dist2 < *bptr) {
	    *bptr = dist2;
	    *cptr = (JSAMPLE) icolor;
	  }
	  dist2 += xx2;
	  xx2 += 2 * STEP_C * STEP_C;
	  bptr++;
	  cptr++;
	}
	dist1 += xx1;
	xx1 += 2 * STEP_C * STEP_C;
      }
      dist0 += xx0;
      xx0 += 2 * STEP_Y * STEP_Y;
    }
  }
}


//************************************************************************
/* Fill the inverse-colormap entries in the update box that contains */
/* xhistogram cell c0/c1/c2.  (Only that one cell MUST be filled, but */
/* we can fill as many others as we wish.) */
//************************************************************************
LOCAL void fill_inverse_cmap (LPTR xhistogram, int actualnumcolors, int c0, int c1, int c2)
//************************************************************************
{
  int minc0, minc1, minc2;	/* lower left corner of update box */
  int ic0, ic1, ic2;
  JSAMPLE * cptr;	/* pointer into bestcolor[] array */
  LPTR cachep;	/* pointer into main cache array */
  /* This array lists the candidate colormap indexes. */
  JSAMPLE colorlist[MAXNUMCOLORS];
  int numcolors;		/* number of candidate colors */
  /* This array holds the actually closest colormap index for each cell. */
  JSAMPLE bestcolor[BOX_Y_ELEMS * BOX_C_ELEMS * BOX_C_ELEMS];

  /* Convert cell coordinates to update box ID */
  c0 >>= BOX_Y_LOG;
  c1 >>= BOX_C_LOG;
  c2 >>= BOX_C_LOG;

  /* Compute true coordinates of update box's origin corner.
   * Actually we compute the coordinates of the center of the corner
   * xhistogram cell, which are the lower bounds of the volume we care about.
   */
  minc0 = (c0 << BOX_C_SHIFT) + ((1 << C_SHIFT) >> 1);
  minc1 = (c1 << BOX_C_SHIFT) + ((1 << C_SHIFT) >> 1);
  minc2 = (c2 << BOX_C_SHIFT) + ((1 << C_SHIFT) >> 1);
  
  /* Determine which colormap entries are close enough to be candidates
   * for the nearest entry to some cell in the update box.
   */
  numcolors = find_nearby_colors(actualnumcolors, minc0, minc1, minc2, colorlist);

  /* Determine the actually nearest colors. */
  find_best_colors(minc0, minc1, minc2, numcolors, colorlist,
		   bestcolor);

  /* Save the best color numbers (plus 1) in the main cache array */
  c0 <<= BOX_Y_LOG;		/* convert ID back to base cell indexes */
  c1 <<= BOX_C_LOG;
  c2 <<= BOX_C_LOG;
  cptr = bestcolor;
  for (ic0 = 0; ic0 < BOX_Y_ELEMS; ic0++) {
    for (ic1 = 0; ic1 < BOX_C_ELEMS; ic1++) {
      cachep = & getxhistogram(c2, c1+ic1, c0+ic0,);
      for (ic2 = 0; ic2 < BOX_C_ELEMS; ic2++) {
		*cachep++ = (histcell) (GETJSAMPLE(*cptr++) + 1);
      }
    }
  }
}

#endif


