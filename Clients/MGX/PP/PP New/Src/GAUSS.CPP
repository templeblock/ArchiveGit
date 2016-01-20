// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

#define GAUSSLUTSIZE 256
#define SUMBUFMAXSIZE 0x00003fff	// 64K total

//********************************** typedefs ********************************
typedef void (*FILTERPROC) (LPTR,int);


//***************************Static vars***********************************
static LPPLFIXED16 lpLuts;	// An array of Lut pointers
static int diameter;			// number of luts in lpLuts
static int left, right;		// the breakup of the convolve mask;
static LPFRAME lpInFrame;	// source data for VConvolves
static LPINT lpInFrameMap;	// specifies which lines to use from lpInFrame
static LPTR lpHSrc;			// source data for HConvolves
static LPLFIXED16 lpSumBuffer;  // Buffer for running sum in vconvolve.  Init to HALF's!
static BOOL bVTemp = TRUE;
static BOOL bTemp = TRUE;

//***************************Static Prototypes*****************************
static int CreateSemiGaussMask(LFIXED sigma, int maxrad, int cutoff);
static void hconvolve8(LPTR lpDst, int iCount);
static void hconvolve24(LPTR lpDst, int iCount);
static void hconvolve32(LPTR lpDst, int iCount);
static void vconvolve(LPTR lpDst, int xPos, int depth, int iCount);
static BOOL MakeLut(LPPLFIXED16 lpLuts, LFIXED16 value);
static void FreeLuts(LPPLFIXED16 lpLuts, int num);

//*************************************************************************
//	Performs a gaussian blurr on lpPixmap.
//	has size maxsize*2+1 or smaller.
//	lpMask can be NULL.
//	MskOffX&Y are the offset of the pixmap from the mask.
// 	Uses LineBuffer!
// 	Only applies gaussian in lpArea, but may look at pixels outside lpArea.
//*************************************************************************
STATUS_CODE GaussPixmap(LPPIXMAP lpPixmap, LPMASK lpMask, int Edge, int maxsize, int MskOffX, int MskOffY,
						BOOL fAbortCheck, LPRECT lpArea)
//************************************************************************
{
	int depth, i, y, width, height, size;
	int newline, oldline;
	FILTERPROC pHConvolve;
	LPTR lpDst, lpDstBuffer, lpin, lpout;
	LPFRAME lpoutFrame;
	LPLFIXED16 lpSum;
	int edge;
	RECT rArea, rTemp;
	FRMTYPEINFO TypeInfo;
	STATUS_CODE StatusCode = SC_SUCCESS;
	
	lpLuts = NULL;
	lpInFrameMap = NULL;
	lpSumBuffer = NULL;
	lpDstBuffer = NULL;
	lpInFrame = NULL;
	
	ProgressBegin(1,0);
	lpoutFrame = PixmapFrame(lpPixmap, PMT_EDIT);

	ASSERT(lpoutFrame);
	
	edge = Edge;

	FrameGetTypeInfo(lpoutFrame, &TypeInfo);
	depth = FrameDepth(lpoutFrame);
	depth = Max(depth,1);
	// get right convolve procs
	switch(depth)
	{
		case 0:
		case 1:
			pHConvolve = hconvolve8;
		break;
		case 3:
			pHConvolve = hconvolve24;
		break;
		case 4:
			pHConvolve = hconvolve32;
		break;
		default:
			goto BadParms;
		break;
	}	
	// Create mask and lookup tables
//	if (!maxsize)
//		maxsize = (4*sigma)/FUNITY;
	diameter = CreateSemiGaussMask(0, maxsize, edge);
	if (diameter < 3)
		goto BadParms;
		
	// determine area to work on
	rArea.left = 0;
	rArea.top = 0;
	rArea.right = FrameXSize(lpoutFrame)-1;
	rArea.bottom = FrameYSize(lpoutFrame)-1;
	if (lpArea && !AstralIsRectEmpty(lpArea))
	{
		// expand for diameter
		rTemp = *lpArea;
		rTemp.left-=left+1;
		rTemp.right+=right+1;
		rTemp.top-=left+1;
		rTemp.bottom+=right+1;
		AstralIntersectRect(&rArea, &rArea, &rTemp);
	}
		
	width = RectWidth(&rArea);
	height = RectHeight(&rArea);
		
	// Create buffer frame (to do convolve in place)
	lpInFrame = FrameOpen(TypeInfo, width, diameter, 1);
	if (!lpInFrame) goto MemError;
	FrameMode(lpInFrame, RELEASE_NEWEST);
	// Create buffer for one output line (can not use lpInFrame due to caching
	lpDstBuffer =  Alloc(depth*width);
	if (!lpDstBuffer) goto MemError;
	lpHSrc = lpDstBuffer;
	// Create table of lines in lpInFrame to use
	lpInFrameMap = (LPINT)Alloc(diameter*sizeof(int));
	if (!lpInFrameMap) goto MemError;
	// Create & set buffer for VConvolves
	size = Min(SUMBUFMAXSIZE,width*depth);
	lpSum = lpSumBuffer = (LPLFIXED16)Alloc(size*sizeof(LPLFIXED16));
	if (!lpSumBuffer) goto MemError;
	for (i=0;i<size;i++) 
		*lpSum++ = HALF;
	// setup initial lines	for vconvolve
	for (i=0; i<diameter; i++)
		lpInFrameMap[i] = -1;
	for (i=0; i<right;i++)	  
	{
		if (i+rArea.top<height)
		{               
			// copy data from lpoutFrame to lpInFrame
			lpout = FramePointer(lpoutFrame, rArea.left, i+rArea.top, NO, width);
			lpin = FramePointer(lpInFrame, 0, i, YES, width);
			copy(lpout, lpin, width*depth);
			lpInFrameMap[i+left+1] = i;
		}
	}
	// loop	
	for (y=0; y<height;y++)
	{
		if (AstralClockCursor(y,height,YES))
		{
			if (!fAbortCheck || CheckAbortInPlaceEdit(lpPixmap))
				goto Cancelled;
		}

		// rotate buffered lines
		oldline = lpInFrameMap[0];
		if (oldline < 0)
			oldline = bound(lpInFrameMap[diameter-1]+1, 0, diameter-1);	// fill down initialy
		for (i=0;i<diameter-1;i++)
			lpInFrameMap[i] = lpInFrameMap[i+1];	
		newline = y+right+rArea.top;
		if (newline < height+rArea.top)
		{
			// copy data from lpoutFrame to lpInFrame
			lpout = FramePointer(lpoutFrame, rArea.left, newline , NO, width);
			lpin = FramePointer(lpInFrame, 0, oldline , YES, width);
			copy(lpout, lpin, width*depth);
		}
		else
			oldline = -1;
		lpInFrameMap[diameter-1] = oldline;
		// do Vcpnvolve!
		lpDst = lpDstBuffer;
		vconvolve(lpDst,0,depth,width);
		// do Hconvolve	on resulting line
		oldline = lpInFrameMap[0]; // use this line as temp storage
		if (oldline < 0)
			oldline = diameter-1;
		lpDst = FramePointer(lpInFrame, 0, oldline, YES);		
		if (lpHSrc && lpDst)
			(*pHConvolve)(lpDst, width); 
		// write out the data
		lpDst = FramePointer(lpInFrame, 0, oldline, YES);
		
		WriteLine(lpDst, lpPixmap,lpMask,y+rArea.top,MskOffX,MskOffY, rArea.left, width);
		
	}	// for y (Convolve)
	
	goto Exit;

Cancelled:
	StatusCode = SC_CANCELLED;
	goto Exit;

BadParms:
	StatusCode = SC_BADPARMS;
	goto Exit;

MemError:
	StatusCode = SC_MEMERROR;
	
Exit:
	FreeLuts(lpLuts, diameter);
	if (lpDstBuffer)
		FreeUp(lpDstBuffer);
	if (lpSumBuffer)
		FreeUp((LPTR)lpSumBuffer);
	if (lpInFrameMap)
		FreeUp((LPTR)lpInFrameMap);
	if (lpInFrame)
		FrameClose(lpInFrame);
	ProgressEnd();
	return(StatusCode);
	
}

//************************************************************************
//	Creates a seperated 1d gaussian.
//	if sigma==0 then maxrad is used to compute a new sigma.
//		The new sigma is computed so that the convolve is cutoff/255 at the edge.
// 	if sigma!=0 && maxrad==0 then maxrad is computed based on sigma.
//	Sets up lpLuts, diameter, left and right.
//	Caller should free lpLut memory.
//	Returns the resulting diameter or 0 for failure;
//	Uses LineBuffer
//************************************************************************
static int CreateSemiGaussMask(LFIXED sigma, int maxrad, int cutoff)
//************************************************************************
{
	LPLFIXED lpCurve;
	int r;
	LFIXED Val, sum;
	double val,scale, p, fsigma, foldsigma;
	double rad, delta;
	
	// GAUSS_CONST is sqrt(2*pi)
	#define GAUSS_CONST 2.506628275
	
	if (sigma != 0)
	{
		maxrad = FROUND(sigma*4);
		diameter = maxrad;
		left = right = diameter;
		diameter = left+right+1;
		if (diameter < 3)
			return(0);
		fsigma = DOUBLE(sigma);
	}
	else
	{
		maxrad = Min(maxrad, 51);
		cutoff = bound(cutoff, 1, (255/(2*maxrad)));
		diameter = maxrad;
		left = right = diameter;
		diameter = left+right+1;
		if (diameter < 3)
			return(0);
		// find sigma that will give us this
		foldsigma = 0.0;
		delta = fsigma = 51.0;
		rad = 59;
		while (delta >= .005)
		{
			foldsigma = fsigma;
			if (rad > maxrad)
				fsigma = foldsigma-delta/2;
			else
				fsigma = foldsigma+delta/2;
			delta = fabs(fsigma-foldsigma);
			rad = sqrt(2*fsigma*fsigma*fabs(log(GAUSS_CONST*fsigma*cutoff/255.0)));
			if (WITHIN(rad, maxrad,maxrad+.02))
				break; 
		}
	}	// end else
	// create basic curve //***FIX !!! LineBuffer
	lpCurve = (LPLFIXED)LineBuffer[0];
	// special case diameter = 3
	if (diameter == 3)
	{
		lpCurve[0] = lpCurve[2] = FUNITY/4;
		lpCurve[1] = FUNITY/2;
	}
	else
	{
		scale = 1.0/(GAUSS_CONST*fsigma);
		p = -1.0/(2*fsigma*fsigma);
		for(r=left; r>=0; r--)
		{
			val = scale*exp(r*r*p);
			Val = DBL2FIX(val);
			if (Val*255<FUNITY)
			{
				if (WITHIN(Val*255,FUNITY-DBL2FIX(.00024),FUNITY))
					Val = FUNITY/255;
				else
				{
					left--;right--;diameter-=2;
					if(diameter<3)
						return(0);
					continue;
				}
			}
		   	lpCurve[left-r] = Val;
		   	lpCurve[diameter-1-left+r] = Val;
		}
	}
	// scale curve to UNITY
	sum = 0L;
	for (r=0;r<diameter;r++)
		sum += lpCurve[r];
	for (r=0;r<diameter;r++)
		lpCurve[r] = DBL2FIX(DOUBLE(lpCurve[r])/DOUBLE(sum));
	// create luts
	lpLuts = (LPPLFIXED16)Alloc(diameter*sizeof(LPLFIXED16));
	if (!lpLuts)
		return(0);
	for (r=0;r<diameter;r++)
		if (!MakeLut(&lpLuts[r], GETFIXED16(lpCurve[r])))
		{
			FreeLuts(lpLuts, r);
			return(0);
		}
	return (diameter);
}

//************************************************************************
// Allocate memory and fills the lut. 
//************************************************************************
static BOOL MakeLut(LPPLFIXED16 lpLuts, LFIXED16 value)
//************************************************************************
{
	int i;
	LPLFIXED16 lpLut;
	
	lpLut = *lpLuts = (LPLFIXED16)Alloc(GAUSSLUTSIZE*sizeof(LFIXED16));
	if (!lpLut)
		return(FALSE);
	for(i=0;i<GAUSSLUTSIZE;i++)
		*lpLut++ = value*i;
	return(TRUE);		
}

//************************************************************************
//	Frees the memory allocated for the luts
//************************************************************************
static void FreeLuts(LPPLFIXED16 lpLuts, int num)
{
	int i;
	LPPLFIXED16 lpLutsBase;
	
	if (!lpLuts)
		return;
	lpLutsBase = lpLuts;
	for(i=0; i<num;i++)
	{
		if(*lpLuts)
			FreeUp((LPTR)*lpLuts);   
		lpLuts++;
	}
	FreeUp((LPTR)lpLutsBase);
}


//************************************************************************
//	Does a 1Xdiameter convolve on any depth data.
// 	iCount is in pixels
//	starts at xPos.
//	Gets source data from lpInFrame.
//	Uses lpInFrameMap[i] as an index into lpInFrame.
//	Puts result into lpDst.
//************************************************************************
static void vconvolve(LPTR lpDst, int xPos, int depth, int iCount)
//************************************************************************
{
	int i, size;
	LPLFIXED16 lpLut, lpSum, lpSumEnd;
	LFIXED16 div;
	long byteWidth;
	LPTR lpSrc;
	
	if (!bVTemp)
	{
		lpSrc = FramePointer(lpInFrame,xPos,lpInFrameMap[left],NO);
		copy(lpSrc, lpDst, iCount*depth);
		return;
	}
	byteWidth = iCount*depth;
	
	if (byteWidth > SUMBUFMAXSIZE)
	{
		//we have to break it into two because of lpSum size limits.
		while (iCount)
		{
			size = Min(SUMBUFMAXSIZE, iCount);
			vconvolve(lpDst,xPos,depth,size);
			lpDst+=size*depth;
			xPos+=size;
			iCount-=size;
		}
		return;
	}
	// just go through the data	a scan line at a time 
	// creating sum
	div = 0L;
	for (i=0; i<diameter;i++)
	{
		if (lpInFrameMap[i] >= 0)
		{
			lpSrc = FramePointer(lpInFrame,xPos,lpInFrameMap[i],NO);
			lpLut = lpLuts[i];
			div += lpLut[1];
			lpSum = lpSumBuffer;
			lpSumEnd = lpSum+byteWidth;
			while (lpSum < lpSumEnd)
				*lpSum++ += lpLut[*lpSrc++];
		}
	}
	lpSum = lpSumBuffer;
	lpSumEnd = lpSum+byteWidth;
	if (WITHIN(div, UNITY-2, UNITY+2))
		for (;lpSum<lpSumEnd;*lpSum++ = HALF)
			*lpDst++ = (*lpSum)>>16;
	else if (div>0)
		for (;lpSum<lpSumEnd;*lpSum++ = HALF)
			*lpDst++ = *lpSum/div;
	else
		for (;lpSum<lpSumEnd;*lpSum++ = HALF)
			*lpDst++ = 0;
}

//************************************************************************
//	Does a diameterX1 convolve on 8-bit data.
//	Gets source data from lpHSrc.
//	Puts result into lpDst.
//************************************************************************
static void hconvolve8(LPTR lpDst, int iCount)
//************************************************************************
{
	int x, count, i;
	LPTR lpRead, lpReadBase;
	LFIXED16 sum, div;
	
	if (iCount < diameter || !bTemp)
	{
		// gauusian is larger than the image
		copy(lpHSrc, lpDst, iCount);
		return;
	}	
	
	//do left edge
	// setup div for left edge
	div = 0L;
	for (i=left+1;i<diameter;i++)		 
		div+=lpLuts[i][1];
	for (x=0; x<left; x++)
	{
		sum = HALF;
		div += lpLuts[left-x][1];
		lpRead = lpHSrc;
		for(i=0; i<=x+right; i++)
			sum += lpLuts[left-x+i][*lpRead++];
		*lpDst++ = sum/div;
	}
	
	//do center
	count = iCount-right;
	lpReadBase=&lpHSrc[x-left];
	for (;x<count;x++)
	{
		lpRead = lpReadBase++;
		sum = HALF;
		for (i=0;i<diameter;i++)
			sum += lpLuts[i][*lpRead++];
		*lpDst++ = sum>>16;
	}
	
	//do right edge
	// setup div for left edge (should begin as UNITY-but just in case)
	div = 0L;
	for (i=0;i<diameter;i++)		 
		div+=lpLuts[i][1];
	for (;x<iCount;x++)
	{
		sum = HALF;
		div -= lpLuts[diameter-x+count-1][1];
		lpRead = lpReadBase++;
		for(i=x-left; i<iCount; i++)
			sum += lpLuts[i-x+left][*lpRead++];
		if (!div)
			*lpDst++ = 0;
		else
			*lpDst++ = sum/div;
	}
	// DONE!!
}

//************************************************************************
//	Does a diameterX1 convolve on 24-bit data.
//	Gets source data from lpHSrc.
//	Puts result into lpDst.
//************************************************************************
static void hconvolve24(LPTR lpDst, int iCount)
//************************************************************************
{
#define depth 3
	
	int x, count, i;
	LPTR lpRead, lpReadBase;
	LFIXED16 div;
	LFIXED16 sum[depth];
	
	if (iCount < diameter || !bTemp)
	{
		// gauusian is larger than the image
		copy(lpHSrc, lpDst, iCount*depth);
		return;
	}	
	
	//do left edge
	// setup div for left edge
	div = 0L;
	for (i=left+1;i<diameter;i++)		 
		div+=lpLuts[i][1];
	for (x=0; x<left; x++)
	{
		sum[0]=sum[1]=sum[2]= HALF;
		div += lpLuts[left-x][1];
		lpRead = lpHSrc;
		for(i=0; i<=x+right; i++)
		{
			sum[0] += lpLuts[left-x+i][*lpRead++];
			sum[1] += lpLuts[left-x+i][*lpRead++];
			sum[2] += lpLuts[left-x+i][*lpRead++];
		}
		*lpDst++ = sum[0]/div;
		*lpDst++ = sum[1]/div;
		*lpDst++ = sum[2]/div;
	}
	
	//do center
	count = iCount-right;
	lpReadBase=&lpHSrc[(x-left)*depth];
	for (;x<count;x++)
	{
		lpRead = lpReadBase;
		lpReadBase+=depth;
		sum[0]=sum[1]=sum[2]= HALF;
		for (i=0;i<diameter;i++)
		{
			sum[0] += lpLuts[i][*lpRead++];
			sum[1] += lpLuts[i][*lpRead++];
			sum[2] += lpLuts[i][*lpRead++];
		}
		*lpDst++ = sum[0]>>16;
		*lpDst++ = sum[1]>>16;
		*lpDst++ = sum[2]>>16;
	}
	
	//do right edge
	// setup div for left edge (should begin as UNITY-but just in case)
	div = 0L;
	for (i=0;i<diameter;i++)		 
		div+=lpLuts[i][1];
	for (;x<iCount;x++)
	{
		sum[0]=sum[1]=sum[2]= HALF;
		div -= lpLuts[diameter-x+count-1][1];
		lpRead = lpReadBase;
		lpReadBase+=depth;
		for(i=x-left; i<iCount; i++)
		{
			sum[0] += lpLuts[i-x+left][*lpRead++];
			sum[1] += lpLuts[i-x+left][*lpRead++];
			sum[2] += lpLuts[i-x+left][*lpRead++];
		}
		if (!div)
		{
			*lpDst++ = 0;
			*lpDst++ = 0;
			*lpDst++ = 0;
		}
		else
		{
			*lpDst++ = sum[0]/div;
			*lpDst++ = sum[1]/div;
			*lpDst++ = sum[2]/div;
		}
	}
	// DONE!!
#undef depth
}


//************************************************************************
//	Does a diameterX1 convolve on 24-bit data.
//	Gets source data from lpHSrc.
//	Puts result into lpDst.
//************************************************************************
static void hconvolve32(LPTR lpDst, int iCount)
//************************************************************************
{
#define depth 4
	
	int x, count, i;
	LPTR lpRead, lpReadBase;
	LFIXED16 div;
	LFIXED16 sum[depth];
	
	if (iCount < diameter || !bTemp)
	{
		// gauusian is larger than the image
		copy(lpHSrc, lpDst, iCount*depth);
		return;
	}	
	
	//do left edge
	// setup div for left edge
	div = 0L;
	for (i=left+1;i<diameter;i++)		 
		div+=lpLuts[i][1];
	for (x=0; x<left; x++)
	{
		sum[0]=sum[1]=sum[2]=sum[3]= HALF;
		div += lpLuts[left-x][1];
		lpRead = lpHSrc;
		for(i=0; i<=x+right; i++)
		{
			sum[0] += lpLuts[left-x+i][*lpRead++];
			sum[1] += lpLuts[left-x+i][*lpRead++];
			sum[2] += lpLuts[left-x+i][*lpRead++];
			sum[3] += lpLuts[left-x+i][*lpRead++];
		}
		*lpDst++ = sum[0]/div;
		*lpDst++ = sum[1]/div;
		*lpDst++ = sum[2]/div;
		*lpDst++ = sum[3]/div;
	}
	
	//do center
	count = iCount-right;
	lpReadBase=&lpHSrc[(x-left)*depth];
	for (;x<count;x++)
	{
		lpRead = lpReadBase;
		lpReadBase+=depth;
		sum[0]=sum[1]=sum[2]=sum[3]= HALF;
		for (i=0;i<diameter;i++)
		{
			sum[0] += lpLuts[i][*lpRead++];
			sum[1] += lpLuts[i][*lpRead++];
			sum[2] += lpLuts[i][*lpRead++];
			sum[3] += lpLuts[i][*lpRead++];
		}
		*lpDst++ = sum[0]>>16;
		*lpDst++ = sum[1]>>16;
		*lpDst++ = sum[2]>>16;
		*lpDst++ = sum[3]>>16;
	}
	
	//do right edge
	// setup div for left edge (should begin as UNITY-but just in case)
	div = 0L;
	for (i=0;i<diameter;i++)		 
		div+=lpLuts[i][1];
	for (;x<iCount;x++)
	{
		sum[0]=sum[1]=sum[2]=sum[3]= HALF;
		div -= lpLuts[diameter-x+count-1][1];
		lpRead = lpReadBase;
		lpReadBase+=depth;
		for(i=x-left; i<iCount; i++)
		{
			sum[0] += lpLuts[i-x+left][*lpRead++];
			sum[1] += lpLuts[i-x+left][*lpRead++];
			sum[2] += lpLuts[i-x+left][*lpRead++];
			sum[3] += lpLuts[i-x+left][*lpRead++];
		}
		if (!div)
		{
			*lpDst++ = 0;
			*lpDst++ = 0;
			*lpDst++ = 0;
			*lpDst++ = 0;
		}
		else
		{
			*lpDst++ = sum[0]/div;
			*lpDst++ = sum[1]/div;
			*lpDst++ = sum[2]/div;
			*lpDst++ = sum[3]/div;
		}
	}
	// DONE!!
	
#undef depth
}
