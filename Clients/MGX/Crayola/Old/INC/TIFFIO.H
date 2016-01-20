//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _TIFFIO_H_
#define _TIFFIO_H_

typedef struct {
	// Generic bitmap values
	short npix, nlin, bpp, InPlanes, InDepth, OutDepth, FileBPL;
	short bNegate, bCompressed, bColormap, bOKtoPreload;
	long lResolution;
	RGBS RGBmap[256];
	LPTR lpFileLine;
	LPTR lpConvertLine;
	void (FAR *lpExpandProc)(LPTR lpIn, int iInByteCount);
	void (FAR *lpSampleProc)( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
	// TIFF Specific values
	void (FAR *lpPredictorProc)( LPTR lpImage, int iCount );
	long StripsPerImage;
	LPLONG StripByteCounts;
	LPLONG StripOffsets;
	long ImagePos;
} TIFFHEADER, _far * LPTIFFHEADER;

/************************************************************************/
/* Tag structure definition						*/
/************************************************************************/
typedef struct _tag
	{
	short	tagno;
	short	type;
	long	length;
	long	value;
} TAG;
typedef TAG _far *LPTAG;

#endif // _TIFFIO_H_
