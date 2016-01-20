#ifndef _TGARDR_H_
#define _TGARDR_H_

// TGARDR prototypes
PDIB ReadTarga(LPCSTR lpFileName);

/* TARGA file description */

/* dataBits values */
#define TYPE_8		8
#define TYPE_16		16
#define TYPE_24		24
#define TYPE_32		32
#define TYPE_M8		-8

typedef struct _tgahdr {
    BYTE	textSize;
    BYTE	mapType;
    BYTE	dataType;
    short	mapOrig;
    short	mapLength;
    BYTE	CMapBits;
    short	XOffset;
    short	YOffset;
    short	x;
    short	y;
    BYTE	dataBits;
    BYTE	imType;
} TGAHDR, FAR * LPTGAHDR;

typedef struct _tgastate {
	HFILE	pFile;
	BOOL    bCompressed;
	int     PixelsPerScanline;
	int     BytesPerScanline;
    LPRGB   lpRGBmap;	/* pointer to color map */
    int	    cnt;		/* number of pixels remaining in packet */
    BYTE    DataBuf[4]; /* pixel being repeated (run-length packet) */
    BOOL	rawPacket;	/* raw packet flag */
	HPTR	lpBuffer;
} TGASTATE, FAR * LPTGASTATE;

#endif

