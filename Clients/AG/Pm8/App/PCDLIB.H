/*
 *	Photo CD Access Developer Toolkit
 *
 *	pcdlib.h
 *	Public definitions for the toolkit
 *
 *	(c) Copyright Eastman Kodak Company, 1991.
 *	All rights reserved.
 */

#ifndef	_PCDLIB_H_

#define ENABLE_CATALOG_CALLS 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define	_PCDLIB_H_

#ifndef	PCDRAWDATA
typedef	unsigned char _huge *PCDrawData;
#define	PCDRAWDATA	PCDrawData
#endif

/* Information contained in each Image Pac */
typedef struct {
	unsigned short	version;	 		/* specification version */
	unsigned short	piwVersion;			/* PIW version */
	unsigned short	magnification;		/* 100 * magnification descriptor */
	unsigned long	scanTime;			/* image scan time */
	unsigned long	modTime;			/* last modification time */
	char			mediaId;			/* original media ID: */
#define				PCD_COLOR_NEGATIVE		0
#define				PCD_COLOR_REVERSAL		1
#define				PCD_COLOR_HARDCOPY		2
#define				PCD_THERMAL_HARDCOPY	3
#define				PCD_BW_NEGATIVE			4
#define				PCD_BW_REVERSAL			5
#define				PCD_BW_HARDCOPY			6
#define				PCD_INTERNEGATIVE		7
#define				PCD_SYNTHETIC_IMAGE		8
	char			mediaType[20];		/* original media product type */
	char			scannerVendor[20];	/* scanner vendor */
	char			scannerProdID[16];	/* scanner type */
	char			scannerFirmRev[4];	/* scanner SCSI firmware rev # */
	char			scannerFirmDate[8];	/* scanner SCSI firmware rev date */
	char			scannerSerial[20];	/* scanner serial number */
	unsigned char	scannerSize[2];		/* pixel size: 
						 byte 0 = microns
						 byte 1 = fraction (right of decimal point) */
	char			piwEquipment[20];	/* PIW equipment manufacturer */
	char			nameCharSet;		/* char set of name (below) */
	char			nameEscapes[32];	/* escape sequences for name (below) */
	char			photofinisher[60];	/* name of photofinisher */
	char			SBAdata[106];		/* Scene Balance Algorithm data */
	char			copyright;			/* 1 = copyright restrictions:
											see copyrightFile for details */
	char			copyrightFile[30];	/* name of optional file containing
											copyright restrictions */
} PCDpacInfoRec, FAR *PCDpacInfoPtr;
typedef HANDLE PCDpacInfoHdl;


/* 
 * Per-disc information. Found in \PHOTO_CD\INFO.PCD; 
 * retrieved with PCDreadInfoFile.
 */

#define PCD_INFO_FILE_NAME "INFO.PCD"

typedef struct {
	char			signature[8];		/* "PHOTO_CD" */
	unsigned char	majorRevision;
	unsigned char	minorRevision;
	char			serialNumber[12];
	unsigned long	creationTime;
	unsigned long	modificationTime;
	unsigned short	nImages;
	unsigned char	interleaveRatio;
	unsigned char	resolution;
	unsigned char 	leadOutStartTime[3];
	unsigned char	nSessions;
}	PCDinfoFileRec, FAR *PCDinfoFilePtr;


// 
// used by PCDgetColorButtons(), PCDsetColorButtons(), and 
// PCDOgetColorButtons()
//
typedef struct 
{
	short		BrightButton;	// Brightness button value
	short		RedButton;		// Red Color button value
	short		GreenButton;	// Green Color button value
	short		BlueButton;		// Blue Color button value
	short		SatButton;		// Saturation button value
	short		OutOfGamut;		// 1 = show, 0 = don't show
	short		SubtractSBA;	// 1 = subtract SBA, 0 = don't subtract SBA

} PCDbuttonRec, FAR *PCDbuttonPtr;


  
typedef HANDLE	PCDpaletteHdl;

/* Resolutions for retrieval */
typedef enum 
{
	PCD_BASE_OVER_64 = 0,	/*   64 x    96 */
	PCD_BASE_OVER_16,		/*  128 x   192 */
	PCD_BASE_OVER_4,		/*  256 x   384 */
	PCD_BASE,				/*  512 x   768 */
	PCD_4BASE,				/* 1024 x  1536 */
	PCD_16BASE,				/* 2048 x  3072 */
	PCD_64BASE,				/* 4096 x  6144 */
	PCD_256BASE,			/* 8192 x  12288 */
	PCD_1024BASE			/* 16384 x  24576 */
} PCDresolution;

#define	PCD_ABSOLUTE	PCD_64BASE

/* Color formats */
typedef enum {
	PCD_SINGLE,			/* single component (8-bit grayscale) */
	PCD_YCC,			/* PhotoYCC */
	PCD_RGB,			/* RGB */
	PCD_PALETTE,		/* mapped to given palette values */
}	PCDformat;

typedef	HANDLE			PCDbitmapHdl;
#define	PCDBMEXTRABYTES	(sizeof(long) * 3)

/* Maximum number of images in an overview pac */
#define	PCDMAXOVIEWS	2045

/* Length of string returned by PCDgetVolume */
#define	PCDVOLSIZE		35

/* Maximum size of the name of any file on a disc */
#define	PCD_MAX_FILE_LENGTH	12

/* 
 * Definitions for image transformations.
 */

#define PCD_NO_TRANSFORM 	0
#define PCD_HORIZ_MIRROR	1		/* Mirror image about vertical axis */
#define	PCD_VERT_MIRROR		2		/* Mirror image about horizontal axis */
#define	PCD_90_ROTATION		4		/* 90 degree counterclockwise rotation */

/* 
 * Image transformation API.
 */

typedef enum {
	PCD_ROTATE_0	= PCD_NO_TRANSFORM,
	PCD_ROTATE_90	= PCD_90_ROTATION,
	PCD_ROTATE_180	= PCD_VERT_MIRROR | PCD_HORIZ_MIRROR,
	PCD_ROTATE_270	= PCD_90_ROTATION | PCD_HORIZ_MIRROR | PCD_VERT_MIRROR,
	PCD_MIRROR_0	= PCD_VERT_MIRROR ,
	PCD_MIRROR_90	= PCD_90_ROTATION | PCD_HORIZ_MIRROR,
	PCD_MIRROR_180	= PCD_HORIZ_MIRROR,
	PCD_MIRROR_270	= PCD_90_ROTATION | PCD_VERT_MIRROR
}	PCDtransform;

typedef HANDLE PCDphotoHdl;
typedef	HANDLE PCDoviewHdl;

/* File suffixes */
#define	PCD_IMAGEPACK	".pcd"
#define	PCD_PCXEXPORT	".pcx"
#define	PCD_DIBEXPORT	".bmp"
#define	PCD_TIFFEXPORT	".tif"
#define	PCD_RIFFEXPORT	".rdi"
#define	PCD_EPSFEXPORT	".eps"

/* Return values */
typedef short	PCDstatus;
#define	pcdSuccess		   0
#define	pcdBadFmt		3000
#define	pcdBadParam		3001
#define	pcdBadVers		3002
#define	pcdNotAvailable	3003
#define	pcdReentry		3004
#define	pcdWrongVolume	3005
#define	pcdUserAbort	3006
#define	PCDtoolkitError(X)	(((X) >= pcdBadFmt) && ((X) <= pcdUserAbort))

/* Toolkit function prototypes */

PCDstatus FAR PASCAL	PCDopen (LPSTR lpPath, PCDphotoHdl FAR *lphPCD);
PCDstatus FAR PASCAL	PCDrefOpen (int fd, PCDphotoHdl FAR *lphPCD);
PCDstatus FAR PASCAL	PCDclose (PCDphotoHdl);
PCDstatus FAR PASCAL	PCDrefClose (PCDphotoHdl);
PCDstatus FAR PASCAL	PCDsetResolution (PCDphotoHdl, PCDresolution);
PCDstatus FAR PASCAL	PCDgetResolution (PCDphotoHdl, PCDresolution FAR *);
PCDstatus FAR PASCAL	PCDgetRotation (PCDphotoHdl hPcd, 
									PCDtransform FAR *lpTransform);
PCDstatus FAR PASCAL	PCDsetFormat (PCDphotoHdl, PCDformat);
PCDstatus FAR PASCAL	PCDgetFormat (PCDphotoHdl, PCDformat FAR *);
PCDstatus FAR PASCAL	PCDgetCount (PCDphotoHdl pcd, unsigned long FAR *lpRes, 
									short FAR *lpResCount);
PCDstatus FAR PASCAL	PCDgetVolume (LPSTR lpszDrive, LPSTR lpszVolInfo);
PCDstatus FAR PASCAL	PCDgetSize(PCDphotoHdl pcd, LPRECT lpBoundRect);
PCDstatus FAR PASCAL	PCDgetPlaneColumn (PCDphotoHdl, long FAR *, short FAR *);
PCDstatus FAR PASCAL	PCDsetPlaneColumn (PCDphotoHdl pcd, 
								  long lPlaneBytes, short nColumnBytes);

PCDstatus FAR PASCAL	PCDsetPalette (PCDphotoHdl pcd, 
									  PCDpaletteHdl hPal);
PCDstatus FAR PASCAL	PCDgetPalette (PCDphotoHdl pcd, 
									  PCDpaletteHdl FAR *lphPal);
PCDstatus FAR PASCAL	PCDOsetPalette (PCDoviewHdl pcd, 
									  PCDpaletteHdl hPal);
PCDstatus FAR PASCAL	PCDOgetPalette (PCDoviewHdl pcd, 
									  PCDpaletteHdl FAR *lphPal);
									
PCDstatus FAR PASCAL	PCDsetTransform (PCDphotoHdl pcd, PCDtransform op);
PCDstatus FAR PASCAL	PCDgetTransform (PCDphotoHdl pcd, PCDtransform FAR*lpOp);
PCDstatus FAR PASCAL	PCDgetBytesPerRow (PCDphotoHdl pcd, short width,
										  long FAR *lpdwRowBytes);
PCDstatus FAR PASCAL	PCDgetBlock (PCDphotoHdl pcd, LPRECT lpBoundRect, 
							PCDRAWDATA hpBuffer,  long lStride);
PCDstatus FAR PASCAL	PCDloadImage (PCDphotoHdl pcd, LPRECT lpBoundRect,
							 PCDbitmapHdl FAR *lphPCDbitmap);
PCDstatus FAR PASCAL	PCDconvertBitmap (PCDbitmapHdl hPCDbitmap, 
										 HDC hDisplayContext,
										 HBITMAP FAR *lphBitmap);
PCDstatus FAR PASCAL	PCDfreeBitmap (PCDbitmapHdl hPCDbitmap);
PCDstatus FAR PASCAL    PCDsetProgress(PCDphotoHdl hPcd, 
										FARPROC lpfnCheckProc,
										long lData);
PCDstatus FAR PASCAL    PCDgetProgress (PCDphotoHdl hPcd,
										FARPROC FAR *lplpfnCheckProc,
										long FAR *lplData);
PCDstatus FAR PASCAL	PCDsetAbort (PCDphotoHdl hPcd,
										FARPROC lpfnAbortProc,
										long lData);
PCDstatus FAR PASCAL	PCDgetAbort (PCDphotoHdl hPcd,
										FARPROC FAR *lplpfnAbortProc,
										long FAR *lplData);
PCDstatus FAR PASCAL 	PCDsetDiscChanged (PCDphotoHdl hPcd,
										FARPROC lpfnChangeProc,
										long lData);
PCDstatus FAR PASCAL 	PCDgetDiscChanged (PCDphotoHdl hPcd,
										FARPROC FAR *lplpfnChangeProc,
										long FAR *lplData);
PCDstatus FAR PASCAL	PCDapplyTransform (PCDbitmapHdl hBmap, 
								 PCDtransform op, BOOL inplace, 
								 FARPROC lpAbort, long lAbortData,
								 FARPROC lpProgress, long lProgressData,
								 PCDbitmapHdl FAR *lphResult);
PCDstatus FAR PASCAL	PCDYCCtoRGB (PCDRAWDATA hpYCCdata, 
							long yPlane, short yCol,
							PCDRAWDATA hpRGBdata, 
							long rPlane, short rCol, long count);
PCDstatus FAR PASCAL	PCDYCCtoPalette (PCDRAWDATA hpYCCdata, 
							long yPlane, short yCol, 
							PCDRAWDATA hpRGBdata, 
							long count, PCDpaletteHdl hPal);
PCDstatus FAR PASCAL	PCDreadImageInfo (PCDphotoHdl, PCDpacInfoPtr);
PCDstatus FAR PASCAL	PCDlistResolution (PCDresolution pcd, 
										  short FAR *lpX, short FAR *lpY);
PCDstatus FAR PASCAL	PCDexport (PCDphotoHdl pcd, LPRECT lpBoundRect, 
							LPSTR lpszExport, LPSTR lpszOutputFile);
PCDstatus FAR PASCAL	PCDcreatePalette (unsigned char FAR *lpPal, 
							long lPlaneBytes, short nColumnBytes, short nColors,
							HANDLE hLUT,
							short nC1bits, short nC2bits, short nC3bits,
							BOOL bRGBquant, BOOL bRGBpalette, BOOL bDoErrDiff,
							PCDpaletteHdl FAR *lphPal);

PCDstatus FAR PASCAL	PCDfreePalette (PCDpaletteHdl);

PCDstatus FAR PASCAL	PCDRGBtoPalette (PCDRAWDATA hpRGB, 
								long planeBytes, short colBytes, 
								PCDRAWDATA hpPalVals, 
								long nPixels,
								PCDpaletteHdl hPal);

PCDstatus FAR PASCAL PCDpointToAbsolute (LPPOINT lpSrc, 
										PCDresolution res, LPPOINT lpDst);
PCDstatus FAR PASCAL PCDpointToStep (LPPOINT lpSrc, 
										PCDresolution res, LPPOINT lpDst);
PCDstatus FAR PASCAL PCDrectToAbsolute (LPRECT lpSrc, 
										PCDresolution res, LPRECT lpDst);
PCDstatus FAR PASCAL PCDrectToStep (LPRECT lpSrc, 
										PCDresolution res, LPRECT lpDst);
PCDstatus FAR PASCAL PCDcoordMult (PCDresolution res, short FAR *lpMult);
PCDstatus FAR PASCAL PCDgetToolkitVersion (unsigned short FAR *);
PCDstatus FAR PASCAL PCDOclose (PCDoviewHdl hPcdo);
PCDstatus FAR PASCAL PCDOrefClose (PCDoviewHdl hPcdo);
PCDstatus FAR PASCAL PCDOgetFriend (PCDoviewHdl hPcdo, LPSTR lpPath);
PCDstatus FAR PASCAL PCDOgetBytesPerRow (PCDoviewHdl pcd, short width,
										  long FAR *lplRowBytes);
PCDstatus FAR PASCAL PCDOgetCount (PCDoviewHdl hPcdo, short FAR *lpImgCount);
PCDstatus FAR PASCAL PCDOgetFormat (PCDoviewHdl hPcdo, 
									PCDformat FAR *lpFormat);
PCDstatus FAR PASCAL	PCDOgetBlock (PCDoviewHdl pcd, PCDRAWDATA buf, 
							long lStride);
PCDstatus FAR PASCAL	PCDOloadImage (PCDphotoHdl pcd,
							 PCDbitmapHdl FAR *lphPCDbitmap);
PCDstatus FAR PASCAL PCDOgetPlaneColumn (PCDoviewHdl hPcdo, 
									long FAR *lpPlane, short FAR *lpColumn);
PCDstatus FAR PASCAL PCDOgetResolution (PCDoviewHdl hPcdo, 
									PCDresolution FAR *lpResolution);
PCDstatus FAR PASCAL PCDOgetRotation (PCDoviewHdl hPcdo, 
									PCDtransform FAR *lpTransform);
PCDstatus FAR PASCAL PCDOgetSelect (PCDoviewHdl hPcdo, 
									short FAR *lpwImgNo);
PCDstatus FAR PASCAL PCDOgetSize (PCDoviewHdl hPcdo, 
									LPRECT lpRect);
PCDstatus FAR PASCAL PCDOgetTransform (PCDoviewHdl hPcdo, 
									PCDtransform FAR *lpTransform);
PCDstatus FAR PASCAL PCDOopen (LPSTR lpPath, PCDoviewHdl FAR *lphPcdo);
PCDstatus FAR PASCAL PCDOrefOpen (int fd, PCDoviewHdl FAR *lphPcdo);
PCDstatus FAR PASCAL PCDOsetSelect (PCDoviewHdl hPcdo, short wImgNo);
PCDstatus FAR PASCAL PCDOsetFormat (PCDoviewHdl hPcdo, PCDformat format);
PCDstatus FAR PASCAL PCDOsetPlaneColumn (PCDoviewHdl hPcdo, 
									long planeBytes, short columnBytes);
PCDstatus FAR PASCAL PCDOsetResolution (PCDoviewHdl hPcdo, 
									PCDresolution resolution);
PCDstatus FAR PASCAL PCDOsetTransform (PCDoviewHdl hPcdo, 
									PCDtransform transform);
PCDstatus FAR PASCAL 	PCDOsetDiscChanged (PCDoviewHdl hPcd,
										FARPROC lpfnChangeProc,
										long lData);
PCDstatus FAR PASCAL 	PCDOgetDiscChanged (PCDoviewHdl hPcd,
										FARPROC FAR *lplpfnChangeProc,
										long FAR *lplData);

PCDstatus FAR PASCAL	PCDgetAllowance (long FAR *lplMaxBytes);
PCDstatus FAR PASCAL	PCDsetAllowance (long lMaxBytes);
PCDstatus FAR PASCAL 	PCDreadInfoFile (LPSTR, PCDinfoFilePtr);


#ifdef ENABLE_CATALOG_CALLS
PCDstatus FAR PASCAL PCDOloadImageR(PCDoviewHdl hPcd, 
                                    LPRECT lpRect, 
                                    PCDbitmapHdl FAR *lphPCDbitmap);
PCDstatus FAR PASCAL PCDOgetQuadRect(PCDoviewHdl pcdo, 
                                     int quadrant, 
                                     LPRECT theRectPtr);
PCDstatus FAR PASCAL PCDOgetBlockR(PCDoviewHdl hpcd, 
                                    LPRECT lprect,
                                    PCDRAWDATA buffer, 
                                    long stride);
#endif



// new lut get/set routines
PCDstatus FAR PASCAL	PCDgetLut(PCDphotoHdl  hpcd,  HANDLE FAR *pHandle, short FAR *sLutSize);
PCDstatus FAR PASCAL	PCDsetLut(PCDphotoHdl  hpcd,  HANDLE hLut, short sLutSize);
PCDstatus FAR PASCAL	PCDOgetLut(PCDoviewHdl hpcdo, HANDLE FAR *pHandle, short FAR *sLutSize);
PCDstatus FAR PASCAL	PCDOsetLut(PCDoviewHdl hpcdo, HANDLE hLut, short sLutSize);

PCDstatus FAR PASCAL PCDgetColorButtons(PCDphotoHdl hpcd, PCDbuttonPtr buttonValues);
PCDstatus FAR PASCAL PCDsetColorButtons(PCDphotoHdl hpcd, PCDbuttonPtr buttonValues);
PCDstatus FAR PASCAL PCDOgetColorButtons(PCDoviewHdl hpcdo, PCDbuttonPtr buttonValues);
PCDstatus FAR PASCAL PCDOsetColorButtons(PCDoviewHdl hpcdo, PCDbuttonPtr buttonValues);

PCDstatus FAR PASCAL PCDconvertYCCtoRGB( PCDphotoHdl hpcd, 
				PCDRAWDATA  y, 
                long        yPlane, 
                short       yCol, 
                PCDRAWDATA  r, 
                long        rPlane, 
                short       rCol, 
                long        count );

PCDstatus FAR PASCAL PCDOconvertYCCtoRGB( PCDphotoHdl hpcdo, 
				PCDRAWDATA  y, 
                long        yPlane, 
                short       yCol, 
                PCDRAWDATA  r, 
                long        rPlane, 
                short       rCol, 
                long        count );

#ifdef __cplusplus
} 
#endif /* __cplusplus */
#endif	/* _PCDLIB_H_ */

