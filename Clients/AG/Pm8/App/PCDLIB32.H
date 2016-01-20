//{{NO_DEPENDENCIES}}
/*
 *	KODAK Photo CD Access Developer Toolkit						 
 *	Common version for Unix, Windows 32bit and the Mac
 *
 *  developed by the Eastman Kodak PCD Toolkit Swat team 
 *
 *	Copyright Eastman Kodak Company, 1995.
 *	All rights reserved.
 */

/*
 *	SCCS information
 *	MODULE ID		pcdlib.h	1.11
 *	Checked out:		09:47:39	5/1/95
 *	Last Modified:		16:22:54	4/11/95
 */
#ifdef IDENT
#ident "@(#)pcdlib.h	1.11 \t4/11/95	COPYRIGHT 1995 EASTMAN KODAK COMPANY"
#endif

#ifndef	_pcdlib32_H_
#define	_pcdlib32_H_

/* if not using the KEPS compatability libraries, define your system type */
#if !defined(KPMAC) & !defined(KPWIN32) & !defined(KPSUN)
#define KPMAC
#undef KPWIN32
#undef KPSUN
#endif

#if defined(KPWIN32) | defined(KPMAC)
#define USE_PROTOTYPES
#endif

/*  Changes
 *
 *  27 Aug 92 - v1.9 - js - Fix mis-specified info-struct - made "number of
 *                          sessions" inaccessable.
 *  28 Sep 94 -        alm -combining pcdlib.h with Unix and Windows NT pcd needs to
 *                          common code. Win32 port.
 */
 
#ifdef KPMAC
#include <Types.h>
#include <Quickdraw.h>
#include <QDOffscreen.h>
#endif /* KPMAC amjs 12-26-94  */

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
typedef HANDLE PCDbitmapHdl;   /* 2-1-95 changed from incorrectly defined HBITMAP
								  back to HANDLE */
typedef HANDLE  PCDpaletteHdl;  
#ifndef PCDRAWDATA
typedef unsigned char *PCDrawData;
#define PCDRAWDATA  PCDrawData
#endif  /* for PCDRAWDATA for backward compatibility only */
#endif  /* if defined KPWIN32 */

#ifndef KPMAC
//TODO:MAC - do we need these typedef's?
#ifndef _MAC
#define nil ((void *)0)
typedef unsigned char *Ptr;
typedef Ptr *Handle;
#endif // !_MAC
#else 
#include <memory.h>
#endif /* 12-20-94  */

/* 10 is the length of an locking key for a locked image, \0 (NULL) terminated. */
#define KEY_STR_LEN	11
typedef unsigned char PCDKeyStr[KEY_STR_LEN];

typedef unsigned char PCDboolean;

#ifdef KPMAC
#define PCDpoint Point
#else
typedef struct {
    short v;
    short h;
} PCDpoint;
#endif

#ifdef KPMAC
#define PCDrect Rect
#else 
typedef struct {
        short top;
        short left;
        short bottom;
        short right;
} PCDrect;
#endif

typedef enum {
	PCD_BASE_OVER_64 = 0,	/*   64 x    96 */
	PCD_BASE_OVER_16,	/*  128 x   192 */
	PCD_BASE_OVER_4,	/*  256 x   384 */
	PCD_BASE,		/*  512 x   768 */
	PCD_4BASE,		/* 1024 x  1536 */
	PCD_16BASE,		/* 2048 x  3072 */
	PCD_64BASE		/* 4096 x  6144 */
} PCDresolution;
#define	PCD_ABSOLUTE	PCD_64BASE

/* All non-16bit Windows software developed now should be using the first */
/* enum set for PCDformat for upward compatibility purposes. The second   */
/* definition here is strictly for backward compatibility of older 16bit  */
/* Windows applications.                                                  */

#if defined (KPWIN32) || defined (KPSUN) || defined (KPMAC)

typedef enum {
	PCD_SINGLE = 10,		/* single component (grayscale) */
	PCD_RGB,			/* RGB color */
	PCD_YCC,			/* PhotoYCC color */
	PCD_PALETTE			/* 8-bit: valid for Windows only */
} PCDformat;

#elif defined(_WINDOWS) || defined(_Windows)
typedef enum {
	PCD_SINGLE,		/* single component (8-bit grayscale) */
	PCD_YCC,		/* PhotoYCC */
	PCD_RGB,		/* RGB */
	PCD_PALETTE,		/* mapped to given palette values */
}	PCDformat;

#endif


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

/* amjs 12-26-94 */ 
#if defined(powerc) || defined(__powerc)
#pragma options align=mac68k
#endif

typedef struct {
    unsigned short    version;             /* specification version */
    unsigned short    piwVersion;          /* PIW version */
    unsigned short    magnification;       /* 100 * magnification descriptor */
    unsigned long     scanTime;            /* image scan time */
    unsigned long     modTime;             /* last modification time */
    char              mediaId;             /* original media ID: */
    char              mediaType[20];       /* original media product type */
    char              scannerVendor[20];   /* scanner vendor */
    char              scannerProdID[16];   /* scanner type */
    char              scannerFirmRev[4];   /* scanner SCSI firmware rev # */
    char              scannerFirmDate[8];  /* scanner SCSI firmware rev date */
    char              scannerSerial[20];   /* scanner serial number */
    unsigned char     scannerSize[2];      /* pixel size: 
                                              byte 0 = microns
                                              byte 1 = fraction (right of decimal point) */
    char              piwEquipment[20];    /* PIW equipment manufacturer */
    char              nameCharSet;         /* char set of name (below) */
#ifdef KPMAC    
    char              pad2;                /* padding to even byte boundary */
#endif /* KPMAC */
    char              nameEscapes[32];     /* escape sequences for name (below) */
    char              photofinisher[60];   /* name of photofinisher */
    char              SBAdata[106];        /* Scene Balance Algorithm data */
    char              copyright;           /* 0x1 = copyright restrictions:
                                              see copyrightFile for details,
                                              0xFF = undefined copyright restrictions */
#ifdef KPMAC
    char              pad3;                /* padding to even byte boundary */
#endif /* KPMAC amjs 12-26-94 */
    char              copyrightFile[32];   /* name of file containing
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

/* amjs 12-26-94 */
/* used in encryption feature set */
typedef struct {
    unsigned long enc_mask;
    unsigned long wrc_mask;
} IPEattrib;

/* reset byte alignment for ppc */
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif
/* amjs 12-26-94 */

typedef struct PCDphotoRec **PCDphotoHdl;

/* ----Macro section---- */

/* The following 2 macros are for Windows programmers convenience */

#define makePCDrect(winRect, pcdRect)\
	{\
		(pcdRect->top)    = (short)(winRect.top);\
		(pcdRect->left)   = (short)(winRect.left);\
		(pcdRect->bottom) = (short)(winRect.bottom);\
		(pcdRect->right)  = (short)(winRect.right);\
	}

#define makeRECTfromPCDrect(pcdRect, winRect)\
	{\
		(winRect->top)    = (long)(pcdRect.top);\
		(winRect->left)   = (long)(pcdRect.left);\
		(winRect->bottom) = (long)(pcdRect.bottom);\
		(winRect->right)  = (long)(pcdRect.right);\
	}



/* amjs 12-26-94 */
/* definitions for the PCDprogress function */

#ifdef KPMAC
typedef pascal void (*PCDprogressPtr)(short num, short denom, long data);
#else
typedef void (*PCDprogressPtr)(short num, short denom, long data);
#endif

#if USESROUTINEDESCRIPTORS
enum {
	uppPCDprogressInfo = kPascalStackBased
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short)))
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short)))
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(long*)))
};
typedef UniversalProcPtr PCDprogressUPP;

#define BUILD_PCDPROGRESS(userRoutine)	\
		 BUILD_ROUTINE_DESCRIPTOR(uppPCDprogressInfo, (ProcPtr) userRoutine)
#define CallPCDprogress(userRoutine, num, denom, data)		\
		CallUniversalProc((UniversalProcPtr)(userRoutine), uppPCDprogressInfo, (num), (denom), (data))
#define NewPCDprogress(userRoutine)		\
		(PCDprogressUPP) NewRoutineDescriptor((ProcPtr)(userRoutine), uppPCDprogressInfo, GetCurrentISA())
#else
typedef PCDprogressPtr PCDprogressUPP;

#define CallPCDprogress(userRoutine, num, denom, data)		\
		(*(userRoutine))((num), (denom), (data))
#define NewPCDprogress(userRoutine)		\
		(PCDprogressUPP)(userRoutine)
#endif

/* definitions for the PCDabort function */
#ifdef KPMAC
typedef pascal Boolean (*PCDabortPtr)(long data);
#else
typedef PCDboolean (*PCDabortPtr)(long data);
#endif

#if USESROUTINEDESCRIPTORS
enum {
	uppPCDabortInfo = kPascalStackBased
		 | RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long*)))
};
typedef UniversalProcPtr PCDabortUPP;
#define BUILD_PCDABORT(userRoutine)	\
		 BUILD_ROUTINE_DESCRIPTOR(uppPCDabortInfo, (ProcPtr) userRoutine)
#define CallPCDabort(userRoutine, data)		\
		CallUniversalProc((UniversalProcPtr)(userRoutine), uppPCDabortInfo, (data))
#define NewPCDabort(userRoutine)		\
		(PCDabortUPP) NewRoutineDescriptor((ProcPtr)(userRoutine), uppPCDabortInfo, GetCurrentISA())
#else
typedef PCDabortPtr PCDabortUPP;

#define CallPCDabort(userRoutine, data)		\
		(*(userRoutine))((data))
#define NewPCDabort(userRoutine)		\
		(PCDabortUPP)(userRoutine)
#endif
/* amjs 12-26-94 */

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

#ifdef KPMAC
#if defined(powerc) || defined(__powerc)
#pragma options align=mac68k
#endif
#endif /* KPMAC amjs 12-26-94 */

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

#ifdef KPMAC
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif
#endif /* KPMAC amjs 12-26-94 */


#if defined(c_plusplus) || defined(__cplusplus) || defined(__STDC__) || defined(USE_PROTOTYPES)
#ifdef	__cplusplus
extern "C" {
#endif


/* The following are APIs that are common by Sun, Mac and Windows */
  
extern PCDstatus PCDrefOpen(short, PCDphotoHdl*);
extern PCDstatus PCDclose(PCDphotoHdl);
extern PCDstatus PCDrefClose(PCDphotoHdl pcd);
extern PCDstatus PCDreadImageInfo(PCDphotoHdl, PCDpacInfoPtr);
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
extern PCDstatus PCDsetFormat(PCDphotoHdl, PCDformat);
extern PCDstatus PCDsetPlaneColumn(PCDphotoHdl, long, short);

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
/* PCDprogressUPP are just void (*) for Sun and Windows */
extern PCDstatus PCDsetProgress(PCDphotoHdl, PCDprogressUPP, long);
extern PCDstatus PCDgetProgress(PCDphotoHdl, PCDprogressUPP*, long*);
extern PCDstatus PCDsetAbort(PCDphotoHdl, PCDabortUPP, long);
extern PCDstatus PCDgetAbort(PCDphotoHdl, PCDabortUPP*, long*);
extern PCDstatus PCDsetFullChroma(PCDphotoHdl, PCDboolean);
extern PCDstatus PCDgetFullChroma(PCDphotoHdl, PCDboolean *);
/* New feature set 1-4-95 amjs */


/* Decryption prototypes */
#ifdef KPMAC
extern PCDstatus PCDsetKey(PCDphotoHdl, Str255);
extern PCDstatus PCDgetKey(PCDphotoHdl, Str255);
#else
extern PCDstatus PCDsetKey(PCDphotoHdl, char *);
extern PCDstatus PCDgetKey(PCDphotoHdl, PCDKeyStr *);
#endif
/* This char** might look strange, but the returned C string is up to 2048 long */
extern PCDstatus PCDgetImageIdStr(PCDphotoHdl, char *, short);
extern PCDstatus PCDclearKey(PCDphotoHdl);
extern PCDstatus PCDgetAttributes(PCDphotoHdl, IPEattrib *);
/* End of common API section */


/* Sun and Windows prototypes */
#ifndef KPMAC
extern PCDstatus PCDopen(char *, PCDphotoHdl*);
extern PCDstatus PCDreadInfoFile(char *, PCDinfoFilePtr);
#endif /* end Sun and Windows prototypes  */

/* Sun and Mac */
#ifndef KPWIN32
extern PCDstatus PCDgetDither(PCDboolean*);
extern PCDstatus PCDsetDither(PCDboolean);
extern PCDstatus PCDdither(unsigned char*, PCDrect*, short, long, unsigned char*, long);
#endif


/* Windows unique APIs */
#ifdef KPWIN32
extern PCDstatus PCDloadImage(PCDphotoHdl, PCDrect *, PCDbitmapHdl *);
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

extern PCDstatus PCDapplyTransform(PCDbitmapHdl, PCDtransform, BOOL, PCDabortUPP, long,
			PCDprogressUPP, long, PCDbitmapHdl *);

extern PCDstatus PCDgetVolume (char *, char *);
#endif /* KPWIN32 */

/* Sun only */
#ifdef KPSUN
extern PCDstatus PCDapplyTransform(PCDimageData *, PCDimageData *,
        PCDtransform, PCDabortUPP, long, PCDprogressUPP, long);

extern PCDstatus PCDsetColors(PCDphotoHdl, XColor *, unsigned short, unsigned char, unsigned char, unsigned char);
extern PCDstatus PCDgetColors(PCDphotoHdl, XColor **, unsigned short*, unsigned char*, unsigned char* , unsigned char* );
extern PCDstatus PCDreduceColor(PCDphotoHdl , unsigned char *, unsigned char *, unsigned short ,unsigned short);
extern PCDstatus PCDgetLutHandle(PCDphotoHdl, Handle*, short *);
extern PCDstatus PCDsetLutHandle(PCDphotoHdl, Handle, short);
#ifdef PCD_USE_X
extern PCDstatus PCDloadImage(PCDphotoHdl, PCDrect *, Display *, XVisualInfo *, XImage **);
#endif /* PCD_USE_X */
#endif /* KPSUN */

/* Mac only */
#ifdef KPMAC
/* Mac's PCDopen expects the name, vol and directory */
extern PCDstatus PCDopen(Str255, short, long, PCDphotoHdl *);
extern PCDstatus PCDreadInfoFile(Str255, short, long, PCDinfoFilePtr);
extern PCDstatus PCDgetToolkitString(short, Str255);
extern PCDstatus PCDgetPhotoCD( Str255, short, short*, long*, long*);
extern void PCDfixToolkitSuffix(StringPtr, short, long);
extern PCDstatus PCDapplyTransform(PixMapHandle, PCDtransform,
			Boolean, PCDabortUPP, long, PCDprogressUPP, long, PixMapHandle*);
extern PCDstatus PCDdisposePixMap(PixMapHandle);
extern PCDstatus PCDnoAllowance(void);
extern void PCDsetYCCTableLock(PCDboolean);
extern PCDstatus PCDloadImage(PCDphotoHdl, PCDrect *, GWorldPtr, GDHandle, PixMapHandle);
extern PCDstatus PCDgetLutHandle(PCDphotoHdl, Handle*);
extern PCDstatus PCDsetLutHandle(PCDphotoHdl, Handle);
extern PCDstatus PCDcloseToolkit(void);
#endif /* KPMAC amjs 12-26-94 */



#ifdef	__cplusplus
}
#endif

#else /* no prototypes */

#ifdef KPMAC
extern PCDstatus PCDgetToolkitString();
extern PCDstatus PCDgetPhotoCD();
extern PCDstatus PCDHopen();
extern PCDstatus PCDapplyTransform();
#endif /* KPMAC amjs 12-26-94 */

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
/* amjs 12-26-94 */
/* Encryption prototypes */
extern PCDstatus PCDsetKey();
extern PCDstatus PCDgetKey();
extern PCDstatus PCDclearKey();
extern PCDstatus PCDgetImageIdStr();
extern PCDstatus PCDgetAttributes();
/* amjs 12-26-94 */
#endif /* no prototypes */


/* Error return values */
#define noErr			0
#define	pcdSuccess		noErr		/* msw compatibility */
#define	pcdBadFmt		3000
#define	pcdBadParam		3001
#define	pcdBadVers		3002
#define	pcdUserAbort		3003
#define	pcdReentry		3004  /* maintained for backward compatibility */
#define	pcdWrongVolume		3005  /* maintained for backward compatibility */
#define pcdNotAvailable 	3006  /* used to be 3003 for Windows but that was incorrect */
#define pcdReadErr		3007  /* added enhancement for versions 2.5 dev drop and 3.0 rel */
#define pcdCloseErr		3008  /* added enhancement for versions 2.5 dev drop and 3.0 rel */
#define pcdSeekErr		3009  /* added enhancement for versions 2.5 dev drop and 3.0 rel */
#define pcdOpenErr		3010  /* added enhancement for versions 2.5 dev drop and 3.0 rel */
#define	PCDtoolkitError(X)	(((X) >= pcdBadFmt) && ((X) <= pcdResNotFound))
/* amjs 12-26-94 */
/* Encryption errors added for versions 2.5 dev drop and 3.0 rel */
#define pcdBadKey		3011
#define pcdKeyRequired		3012
#define pcdEncryptErr		3013
#define pcdNotInterleaved	3014
#define pcdUnknownAttribute 	3015
/* amjs 12-26-94  */
#define pcdNoMem		3016
#define pcdErrXErr		3017
#define pcdResNotFound		3019	

 
/* Overview pac stuff */
/* The OPA is up to 1 sector-12 bytes in length */
#define	OPA_SIZE	2036

typedef struct PCDoviewRec **PCDoviewHdl;

	
#if defined(c_plusplus) || defined(__cplusplus) || defined(__STDC__) || defined(USE_PROTOTYPES)
#ifdef	__cplusplus
extern "C" {
#endif

#ifndef KPMAC
extern PCDstatus PCDOopen(char *, PCDoviewHdl *);
#endif
extern PCDstatus PCDOrefOpen(short, PCDoviewHdl*);
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

/* Windows only */
#ifdef KPWIN32
extern PCDstatus PCDOgetPalette(PCDoviewHdl, PCDpaletteHdl*); 
extern PCDstatus PCDOsetPalette(PCDoviewHdl, PCDpaletteHdl);
extern PCDstatus PCDOgetLut(PCDoviewHdl, HANDLE *, short *); 
extern PCDstatus PCDOsetLut(PCDoviewHdl, HANDLE, short);
extern PCDstatus PCDOloadImage(PCDoviewHdl, PCDbitmapHdl *);
#endif /* KPWIN32 */


/* Sun only */
#ifdef KPSUN
extern PCDstatus PCDOsetColors(PCDoviewHdl, XColor *, unsigned short, unsigned char,
	unsigned char, unsigned char);
extern PCDstatus PCDOgetColors(PCDoviewHdl, XColor **, unsigned short *, 
	unsigned char *, unsigned char *, unsigned char *);
extern PCDstatus PCDOreduceColor(PCDoviewHdl , unsigned char *, unsigned char *,
    unsigned short, unsigned short);
extern PCDstatus PCDOgetLutHandle(PCDoviewHdl, Handle *, short *);
extern PCDstatus PCDOsetLutHandle(PCDoviewHdl, Handle, short);
#ifdef PCD_USE_X
extern PCDstatus PCDOloadImage(PCDoviewHdl, Display *, XVisualInfo *, XImage **);
#endif /* PCD_USE_X */
#endif /* KPSUN */

	
#ifdef KPMAC
extern PCDstatus PCDOopen(Str255, short, long, PCDoviewHdl *);
extern PCDstatus PCDOloadImage(PCDoviewHdl, GWorldPtr, GDHandle, PixMapHandle);
extern PCDstatus PCDOgetLutHandle(PCDoviewHdl, Handle *);
extern PCDstatus PCDOsetLutHandle(PCDoviewHdl, Handle);
#endif /* KPMAC amjs 12-26-94 */


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

#ifdef KPMAC
/*extern PCDstatus PCDOgetBlockR(); */
extern PCDstatus PCDOloadImage();
/*extern PCDstatus PCDOloadImageR(); */
extern PCDstatus PCDOgetLutHandle();
extern PCDstatus PCDOsetLutHandle();
#endif /* KPMAC amjs 12-26-94 */

#endif /* no prototypes */

#ifdef KPMAC

/*
 *	Toolkit Macintosh resources
 */
#define	PCDyccLutType		'PCDl'	/* lookup tables for YCC/RGB conversion */
#define PCDdefaultLutID		1004	/* resource ID of default LUT table */

#define	PCDgammaLutType		'PCDg'	/* gamma correction for loadImage(SINGLE) */
#define PCDdefaultGammaID	0		/* resource ID of default gamma table */

#define	PCDfilenameStrsID	6500	/* strings for file names */
#define	PCDerrorTextID		6501	/* error texts */

/* String indices in STR# PCDfilenameStrsID */
#define	PCDfileStringCD			1	/* name of the CD volume */
#define PCDfileStringImages		2	/* name of the images directory */
#define	PCDfileStringIpac		3	/* template for an image pac */
#define	PCDfileStringOvw		4	/* name of the overview pac */
#define	PCDfileStringInfo		5	/* name of the info file */
#define	PCDfileStringStartup	6	/* name of the startup image */
#define	PCDfileStringRights		7	/* name of the copyright info directory */
#define	PCDfileStringRightsFile	8	/* name of the default copyright info File */
#define	PCDfileStringRightsApdx	9	/* appendix to copyright info File name */
#define PCDfileStringIPEDir			10
#define PCDfileStringIPEBase16Dir	11
#define PCDfileStringIPEBase4Dir	12
#define PCDfileStringIPEBaseDir		13
#define PCDfileStringIPE4BaseDir	14
#define PCDfileStringIPE16BaseDir	15
#define PCDfileStringIPE64BaseDir	16
#define PCDfileStringIPE256BaseDir	17
#define PCDfileStringIPE1024BaseDir	18
#define PCDfileStringInfoIPEFile	19
#define PCDfileStringInfoICFile		20
#define PCDfileStringRevExt			21
#endif /* KPMAC amjs 12-26-94 */

#endif	/* _pcdlib32_H_ */
