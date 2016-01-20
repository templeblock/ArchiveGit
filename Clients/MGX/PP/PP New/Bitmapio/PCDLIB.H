/*
 *	KODAK Photo CD Access Developer Toolkit
 *	Common version for Unix and Windows 32bit
 *
 *	» Copyright Eastman Kodak Company, 1994.
 *	All rights reserved.
 */

/*
 *	Sccs Information:
 *		MODULE ID			%M% %I%
 *		Checked out:		%T% %H%
 *		Last modified:		%U% %G%
 */

#ifndef	_pcdlib_H_
#define	_pcdlib_H_

#ifdef __sun
#pragma ident "%W% \t%G% COPYRIGHT 1994 EASTMAN KODAK COMPANY"
#endif

#ifdef KPWIN32
#define USE_PROTOTYPES
#endif

/*  Changes
 *
 *  27 Aug 92 - v1.9 - js - Fix mis-specified info-struct - made "number of
 *                          sessions" inaccessable.
 *  28 Sep 94 -        alm -combining pcdlib.h with Unix and Windows NT pcd needs to
 *                          common code. Win32 port.
 */

#ifdef KPSUN
/* disable the following line if you're not using X */
#define PCD_USE_X
#endif

#ifdef PCD_USE_X
#include <X11/Xlib.h>	/* for XColor typedef */
#include <X11/Xutil.h>	/* for XVisualInfo typedef */
#endif /* PCD_USE_X */

#ifdef KPWIN32
#include <windows.h>
typedef HBITMAP PCDbitmapHdl;
typedef HANDLE  PCDpaletteHdl;  /* alm*/
#ifndef PCDRAWDATA
typedef unsigned char *PCDrawData;
#define PCDRAWDATA  PCDrawData
#endif  /* for PCDRAWDATA for backward compatibility only */
#endif  /* if defined KPWIN32 */

typedef unsigned char **PCDlutHdl;  

enum {false,true};

typedef unsigned char Style;
typedef long Fixed;

typedef long Size;

typedef unsigned char PCDboolean;

typedef struct {
    short v;
    short h;
} PCDpoint;
 
typedef struct {
        short top;
        short left;
        short bottom;
        short right;
} PCDrect;

typedef enum {
	PCD_BASE_OVER_64 = 0,	/*   64 x    96 */
	PCD_BASE_OVER_16,		/*  128 x   192 */
	PCD_BASE_OVER_4,		/*  256 x   384 */
	PCD_BASE,				/*  512 x   768 */
	PCD_4BASE,				/* 1024 x  1536 */
	PCD_16BASE,				/* 2048 x  3072 */
	PCD_64BASE				/* 4096 x  6144 */
} PCDresolution;
#define	PCD_ABSOLUTE	PCD_64BASE

typedef enum {
	PCD_SINGLE = 10,		/* single component (grayscale) */
	PCD_RGB,				/* RGB color */
	PCD_YCC,				/* PhotoYCC color */
	PCD_PALETTE				/* 8-bit: valid for Windows only */
} PCDformat;

#define PCD_NO_TRANSFORM 	0
#define PCD_HORIZ_MIRROR	1	/* Mirror image about horizontal axis */
#define	PCD_VERT_MIRROR		2	/* Mirror image about vertical axis */
#define	PCD_90_ROTATION		4	/* 90 degree counterclockwise rotation */

typedef enum {
	PCD_ROTATE_0	= PCD_NO_TRANSFORM,
	PCD_ROTATE_90	= PCD_90_ROTATION,
	PCD_ROTATE_180	= PCD_VERT_MIRROR | PCD_HORIZ_MIRROR,
	PCD_ROTATE_270	= PCD_90_ROTATION | PCD_HORIZ_MIRROR | PCD_VERT_MIRROR,
	PCD_MIRROR_0	= PCD_VERT_MIRROR,
	PCD_MIRROR_90	= PCD_90_ROTATION | PCD_HORIZ_MIRROR,
	PCD_MIRROR_180	= PCD_HORIZ_MIRROR,
	PCD_MIRROR_270	= PCD_90_ROTATION | PCD_VERT_MIRROR
} PCDtransform;

typedef	short PCDstatus;

typedef struct {
	unsigned short	version;	 		/* specification version */
	unsigned short	piwVersion;			/* PIW version */
	unsigned short	magnification;		/* 100 * magnification descriptor */
	unsigned long	scanTime;			/* image scan time */
	unsigned long	modTime;			/* last modification time */
	char			mediaId;			/* original media ID: */
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
	char			copyright;			/* 0x1 = copyright restrictions:
											see copyrightFile for details,
										   0xFF = undefined copyright restrictions */
	char			copyrightFile[32];	/* name of file containing
											copyright restrictions */
} PCDpacInfoRec, *PCDpacInfoPtr, **PCDpacInfoHdl;

typedef struct {
	unsigned char *data;
	PCDrect bounds;
	unsigned short depth;
	unsigned short stride;
} PCDimageData;	

#if defined(KPSUN) && !defined(PCD_USE_X)
/* need to define XColor ourselves */
typedef struct {
    unsigned long pixel;
    unsigned short red, green, blue;
    char flags;  /* do_red, do_green, do_blue (unused by Toolkit) */
    char pad;
} XColor;
#endif

typedef struct {
    short        BrightButton;    /* Brightness button value */
    short        RedButton;        /* Red Color button value */
    short        GreenButton;    /* Green Color button value */
    short        BlueButton;        /* Blue Color button value */
    short        SatButton;        /* Saturation button value */
    short        OutOfGamut;        /* 1 = show, 0 = don't show */
    short        SubtractSBA;    /* 1 = subtract SBA, 0 = don't subtract SBA */
} PCDbuttonRec, *PCDbuttonPtr;

typedef struct PCDphotoRec **PCDphotoHdl;

/* these defines are for the values of the MediaID value */
#define	PCD_COLOR_NEGATIVE	0
#define	PCD_COLOR_REVERSAL	1
#define	PCD_COLOR_HARDCOPY	2
#define	PCD_THERMAL_HARDCOPY	3
#define	PCD_BW_NEGATIVE		4
#define	PCD_BW_REVERSAL		5
#define	PCD_BW_HARDCOPY		6
#define	PCD_INTERNEGATIVE	7
#define	PCD_SYNTHETIC_IMAGE	8

/*
 *	Disk info file data
 */
typedef struct pcdinfofilerec {
	char			signature[8];		/* "PHOTO_CD" */
	unsigned char	majorRevision;
	unsigned char	minorRevision;
	char			serialNumber[12];
	unsigned long	creationTime;
	unsigned long	modificationTime;
	unsigned short	nImages;
	unsigned char	interleaveRatio;
	unsigned char	resolution;
    unsigned char   leadOutStartTime[3];
	unsigned char	nSessions;
}	PCDinfoFileRec, *PCDinfoFilePtr, **PCDinfoFileHdl;


#if defined(c_plusplus) || defined(__cplusplus) || defined(__STDC__) || defined(USE_PROTOTYPES)
#ifdef	__cplusplus
extern "C" {
#endif

extern PCDstatus PCDopen(char *, PCDphotoHdl*);
extern PCDstatus PCDrefOpen(int, PCDphotoHdl*);
extern PCDstatus PCDclose(PCDphotoHdl);
extern PCDstatus PCDrefClose(PCDphotoHdl pcd);
extern PCDstatus PCDgetToolkitVersion(unsigned short*);
extern PCDstatus PCDgetBlock(PCDphotoHdl, PCDrect*, unsigned char*, long);
extern PCDstatus PCDgetBytesPerRow(PCDphotoHdl, short, long*);
extern PCDstatus PCDgetPlaneColumn(PCDphotoHdl, long*, short*);
extern PCDstatus PCDgetCount(PCDphotoHdl, unsigned long*, short*);
extern PCDstatus PCDgetFormat(PCDphotoHdl, PCDformat*);
extern PCDstatus PCDgetResolution(PCDphotoHdl, PCDresolution*);
extern PCDstatus PCDgetRotation(PCDphotoHdl, PCDtransform*);
extern PCDstatus PCDgetSize(PCDphotoHdl, PCDrect*);
extern PCDstatus PCDgetTransform(PCDphotoHdl, PCDtransform*);
extern PCDstatus PCDreadImageInfo(PCDphotoHdl, PCDpacInfoPtr);
extern PCDstatus PCDsetAbort(PCDphotoHdl, PCDboolean (*)(long), long);
extern PCDstatus PCDgetAbort(PCDphotoHdl, PCDboolean (**)(long), long*);
extern PCDstatus PCDsetFormat(PCDphotoHdl, PCDformat);
extern PCDstatus PCDsetPlaneColumn(PCDphotoHdl, long, short);
extern PCDstatus PCDsetProgress(PCDphotoHdl, void (*)(short, short, long), long);
extern PCDstatus PCDgetProgress(PCDphotoHdl, void (**)(short, short, long), long*);
extern PCDstatus PCDsetResolution(PCDphotoHdl, PCDresolution);
extern PCDstatus PCDsetTransform(PCDphotoHdl, PCDtransform);
extern PCDstatus PCDcoordMult(PCDresolution, short*);
extern PCDstatus PCDlistResolution(PCDresolution, short*, short*);
extern PCDstatus PCDpointToAbsolute(PCDpoint*, PCDresolution, PCDpoint*);
extern PCDstatus PCDpointToStep(PCDpoint*, PCDresolution, PCDpoint*);
extern PCDstatus PCDrectToAbsolute(PCDrect*, PCDresolution, PCDrect*);
extern PCDstatus PCDrectToStep(PCDrect*, PCDresolution, PCDrect*);
extern PCDstatus PCDsetAllowance(long);
extern PCDstatus PCDgetAllowance(long*);
extern PCDstatus PCDconvertYCCtoRGB(PCDphotoHdl, unsigned char*, long, short,
        unsigned char*, long, short, unsigned long);
extern PCDstatus PCDYCCtoRGB(unsigned char*, long, short, unsigned char*, long,
  short, unsigned long);
extern PCDstatus PCDgetColorButtons(PCDphotoHdl, PCDbuttonPtr);
extern PCDstatus PCDsetColorButtons(PCDphotoHdl, PCDbuttonPtr);
extern PCDstatus PCDreadInfoFile(char *, PCDinfoFilePtr);

#ifdef KPWIN32
extern PCDstatus PCDloadImage(PCDphotoHdl,PCDrect*,PCDbitmapHdl*);
extern PCDstatus PCDfreeBitmap(PCDbitmapHdl);
extern PCDstatus PCDgetPalette(PCDphotoHdl, PCDpaletteHdl* );
extern PCDstatus PCDsetPalette(PCDphotoHdl, PCDpaletteHdl );
extern PCDstatus PCDfreePalette(PCDpaletteHdl);
extern PCDstatus PCDconvertBitmap (PCDbitmapHdl, HDC, HBITMAP*);
extern PCDstatus PCDcreatePalette(unsigned char *, long, short, short, HANDLE,
                                  short, short, short, BOOL, BOOL, BOOL, PCDpaletteHdl *);
extern PCDstatus PCDRGBtoPalette (unsigned char *, long, short,
                                  unsigned char *, long, PCDpaletteHdl);
extern PCDstatus PCDYCCtoPalette (unsigned char *, long, short, unsigned char *,
                                  long, PCDpaletteHdl);
extern PCDstatus PCDgetLut(PCDphotoHdl, HANDLE *, short*);
extern PCDstatus PCDsetLut(PCDphotoHdl, HANDLE, short);
extern PCDstatus PCDapplyTransform(PCDbitmapHdl, PCDtransform, BOOL, PCDboolean (*)(long), long,
                                  void (*)(short, short, long), long, PCDbitmapHdl *);
		/**** the following APIs are unsupported in release 2.5 of Windows 32bit ***/
//extern PCDstatus PCDgetVolume (LPSTR, LPSTR);
//extern PCDstatus PCDsetDiscChanged(PCDphotoHdl, FARPROC, long);
//extern PCDstatus PCDgetDiscChanged(PCDphotoHdl, FARPROC *, long *);
#endif /* KPWIN32 */

#ifdef KPSUN
extern PCDstatus PCDgetDither(PCDboolean*);
extern PCDstatus PCDsetDither(PCDboolean);
extern PCDstatus PCDapplyTransform( PCDimageData *, PCDimageData *,
        PCDtransform, PCDboolean (*)(long), long,
        void (*)(short, short, long), long);
extern PCDstatus PCDdither(unsigned char*, PCDrect*, short, long, unsigned char*, long);
extern PCDstatus PCDsetColors(PCDphotoHdl, XColor *, unsigned short, unsigned char, unsigned char, unsigned char);
extern PCDstatus PCDgetColors(PCDphotoHdl, XColor **, unsigned short*, unsigned char*, unsigned char* , unsigned char* );
extern PCDstatus PCDreduceColor(PCDphotoHdl , unsigned char *, unsigned char *, unsigned short ,unsigned short);
extern PCDstatus PCDgetLutHandle(PCDphotoHdl, PCDlutHdl*, short *);
extern PCDstatus PCDsetLutHandle(PCDphotoHdl, PCDlutHdl, short);
extern PCDstatus PCDsetIPEpath(char *);
#ifdef PCD_USE_X
extern PCDstatus PCDloadImage(PCDphotoHdl, PCDrect*, Display *, XVisualInfo *, XImage **);
#endif /* PCD_USE_X */
#endif /* KPSUN */

#ifdef	__cplusplus
}
#endif

#else /* no prototypes */

extern PCDstatus PCDopen();
extern PCDstatus PCDrefOpen();
extern PCDstatus PCDclose();
extern PCDstatus PCDrefClose();
extern PCDstatus PCDgetToolkitVersion();
extern PCDstatus PCDgetBlock();
extern PCDstatus PCDgetBytesPerRow();
extern PCDstatus PCDgetPlaneColumn();
extern PCDstatus PCDgetCount();
extern PCDstatus PCDgetFormat();
extern PCDstatus PCDgetResolution();
extern PCDstatus PCDgetRotation();
extern PCDstatus PCDgetSize();
extern PCDstatus PCDgetTransform();
extern PCDstatus PCDreadImageInfo();
extern PCDstatus PCDsetAbort();
extern PCDstatus PCDgetAbort();
extern PCDstatus PCDsetFormat();
extern PCDstatus PCDsetPlaneColumn();
extern PCDstatus PCDsetProgress();
extern PCDstatus PCDgetProgress();
extern PCDstatus PCDsetResolution();
extern PCDstatus PCDsetTransform();
extern PCDstatus PCDcoordMult();
extern PCDstatus PCDlistResolution();
extern PCDstatus PCDpointToAbsolute();
extern PCDstatus PCDpointToStep();
extern PCDstatus PCDrectToAbsolute();
extern PCDstatus PCDrectToStep();
extern PCDstatus PCDsetAllowance();
extern PCDstatus PCDgetAllowance();
extern PCDstatus PCDconvertYCCtoRGB();
extern PCDstatus PCDYCCtoRGB();
extern PCDstatus PCDgetColorButtons();
extern PCDstatus PCDsetColorButtons();
extern PCDstatus PCDsetIPEpath();
extern PCDstatus PCDreadInfoFile();
extern PCDstatus PCDgetDither();
extern PCDstatus PCDsetDither();
extern PCDstatus PCDapplyTransform();
extern PCDstatus PCDdither();
extern PCDstatus PCDsetColors();
extern PCDstatus PCDgetColors();
extern PCDstatus PCDreduceColor();
extern PCDstatus PCDgetLutHandle();
extern PCDstatus PCDsetLutHandle();
#ifdef PCD_USE_X
extern PCDstatus PCDloadImage();
#endif /* PCD_USE_X */

#endif /* no prototypes */


/* Error return values */
#define noErr			0
#define	pcdSuccess		noErr		/* msw compatibility */
#define	pcdBadFmt		3000
#define	pcdBadParam		3001
#define	pcdBadVers		3002
#define pcdOpenErr		3003
#define pcdReadErr		3004
#define pcdCloseErr		3005
#define pcdSeekErr		3007
#define	pcdUserAbort	3006
#define	PCDtoolkitError(X)	(((X) >= pcdBadFmt) && ((X) <= pcdUserAbort))
#define pcdNoMem		5000
#define pcdErrXErr		5001
#define pcdInternalErr  5002
#define pcdNotAvailable 5003
#define pcdResNotFound		

 
/* Overview pac stuff */
/* The OPA is up to 1 sector-12 bytes in length */
#define	OPA_SIZE	2036

typedef struct PCDoviewRec **PCDoviewHdl;
	
#if defined(c_plusplus) || defined(__cplusplus) || defined(__STDC__) || defined(USE_PROTOTYPES)
#ifdef	__cplusplus
extern "C" {
#endif
extern PCDstatus PCDOopen(char *, PCDoviewHdl *);
extern PCDstatus PCDOrefOpen(int, PCDoviewHdl*);
extern PCDstatus PCDOclose(PCDoviewHdl);
extern PCDstatus PCDOrefClose(PCDoviewHdl);
extern PCDstatus PCDOgetBlock(PCDoviewHdl, unsigned char*, long);
extern PCDstatus PCDOgetBytesPerRow(PCDoviewHdl, short, long*);
extern PCDstatus PCDOgetCount(PCDoviewHdl, short*);
extern PCDstatus PCDOgetFormat(PCDoviewHdl, PCDformat*);
extern PCDstatus PCDOgetFriend(PCDoviewHdl, char*);
extern PCDstatus PCDOgetPlaneColumn(PCDoviewHdl, long*, short*);
extern PCDstatus PCDOgetResolution(PCDoviewHdl, PCDresolution*);
extern PCDstatus PCDOgetRotation(PCDoviewHdl, PCDtransform*);
extern PCDstatus PCDOgetSelect(PCDoviewHdl, short*);
extern PCDstatus PCDOgetSize(PCDoviewHdl, PCDrect*);
extern PCDstatus PCDOgetTransform(PCDoviewHdl, PCDtransform*);
extern PCDstatus PCDOsetFormat(PCDoviewHdl, PCDformat);
extern PCDstatus PCDOsetPlaneColumn(PCDoviewHdl, long, short);
extern PCDstatus PCDOsetResolution(PCDoviewHdl, PCDresolution);
extern PCDstatus PCDOsetSelect(PCDoviewHdl, short);
extern PCDstatus PCDOsetTransform(PCDoviewHdl, PCDtransform);
extern PCDstatus PCDOgetColorButtons(PCDoviewHdl, PCDbuttonPtr);
extern PCDstatus PCDOsetColorButtons(PCDoviewHdl, PCDbuttonPtr);
extern PCDstatus PCDOconvertYCCtoRGB(PCDoviewHdl, unsigned char *, long, short,
	unsigned char *, long, short, unsigned long);

#ifdef KPWIN32
extern PCDstatus PCDOgetPalette(PCDoviewHdl, PCDpaletteHdl*); 
extern PCDstatus PCDOsetPalette(PCDoviewHdl, PCDpaletteHdl);
extern PCDstatus PCDOgetLut(PCDoviewHdl, HANDLE *, short *); 
extern PCDstatus PCDOsetLut(PCDoviewHdl, HANDLE, short);
extern PCDstatus PCDOloadImage(PCDoviewHdl, PCDbitmapHdl *);
extern PCDstatus PCDOgetBlockR(PCDoviewHdl, PCDrect *, PCDRAWDATA, long);
extern PCDstatus PCDOgetQuadRect(PCDoviewHdl, int, PCDrect *);
		/**** the following APIs are unsupported in release 2.5 of Windows 32bit ***/
//extern PCDstatus PCDOsetDiscChanged(PCDoviewHdl, FARPROC, long);
//extern PCDstatus PCDOgetDiscChanged(PCDoviewHdl, FARPROC *, long *);
//extern PCDstatus PCDOloadImageR(PCDoviewHdl, PCDrect, PCDbitmapHdl *);
#endif /* KPWIN32 */

#ifdef KPSUN
extern PCDstatus PCDOsetColors(PCDoviewHdl, XColor *, unsigned short, unsigned char,
	unsigned char, unsigned char);
extern PCDstatus PCDOgetColors(PCDoviewHdl, XColor **, unsigned short *, 
	unsigned char *, unsigned char *, unsigned char *);
extern PCDstatus PCDOgetLutHandle(PCDoviewHdl, Handle *, short *);
extern PCDstatus PCDOsetLutHandle(PCDoviewHdl, Handle, short);
extern PCDstatus PCDOreduceColor(PCDoviewHdl , unsigned char *, unsigned char *,
    unsigned short, unsigned short);
#ifdef PCD_USE_X
extern PCDstatus PCDOloadImage(PCDoviewHdl, Display *, XVisualInfo *, XImage **);
#endif /* PCD_USE_X */
#endif /* KPSUN */

#ifdef	__cplusplus
}
#endif

#else /* no prototypes */

extern PCDstatus PCDOopen();
extern PCDstatus PCDOrefOpen();
extern PCDstatus PCDOclose();
extern PCDstatus PCDOrefClose();
extern PCDstatus PCDOgetBlock();
extern PCDstatus PCDOgetBytesPerRow();
extern PCDstatus PCDOgetCount();
extern PCDstatus PCDOgetFormat();
extern PCDstatus PCDOgetFriend();
extern PCDstatus PCDOgetPlaneColumn();
extern PCDstatus PCDOgetResolution();
extern PCDstatus PCDOgetRotation();
extern PCDstatus PCDOgetSelect();
extern PCDstatus PCDOgetSize();
extern PCDstatus PCDOgetTransform();
#ifdef PCD_USE_X
extern PCDstatus PCDOloadImage();
#endif
extern PCDstatus PCDOsetFormat();
extern PCDstatus PCDOsetPlaneColumn();
extern PCDstatus PCDOsetResolution();
extern PCDstatus PCDOsetSelect();
extern PCDstatus PCDOsetTransform();
extern PCDstatus PCDOgetColorButtons();
extern PCDstatus PCDOsetColorButtons();
extern PCDstatus PCDOsetColors();
extern PCDstatus PCDOgetColors();
extern PCDstatus PCDOgetLutHandle();
extern PCDstatus PCDOsetLutHandle();
extern PCDstatus PCDOreduceColor();
extern PCDstatus PCDOconvertYCCtoRGB();
#endif /* no prototypes */


#endif	/* _pcdlib_H_ */
