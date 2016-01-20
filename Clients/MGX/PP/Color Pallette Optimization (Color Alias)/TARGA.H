#ifndef __targa
#define __targa

/* TARGA file description */

/* dataBits values */
#define TYPE_8		8
#define TYPE_16		16
#define TYPE_24		24
#define TYPE_32		32
#define TYPE_M8		-8

typedef struct _tgahdr {
    TCHAR	textSize;
    TCHAR	mapType;
    TCHAR	dataType;
    short	mapOrig;
    short	mapLength;
    TCHAR	CMapBits;
    short	XOffset;
    short	YOffset;
    short	x;
    short	y;
    TCHAR	dataBits;
    TCHAR	imType;
} TGAHDR;

typedef struct _tgastate {
	CFile *pFile;
	 BOOL     bCompressed;
	 int      PixelsPerScanline;
	 int      BytesPerScanline;
    LPRGB    lpRGBmap;	  /* pointer to color map */
    int	   inputBytes;
    int	   cnt;		  /* number of pixels remaining in packet */
    BYTE     DataBuf[4]; /* pixel being repeated (run-length packet) */
    BOOL	  rawPacket;  /* raw packet flag */
	LPTR		lpBuffer;
} TGASTATE;

#endif
