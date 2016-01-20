/*
	File: PIGeneral.h

	Copyright 1992 by Adobe Systems, Inc.  All rights reserved.

	This file describes the elements common to all plug-in interfaces.
*/

#ifndef __PIGeneral__
#define __PIGeneral__

#if Macintosh

#include <Types.h>

#endif

/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif


typedef struct PlugInInfo
	{

	short	version;			/* The version number of the interface supported. */
	short	subVersion; 		/* The sub-version number.						  */
	short	priority;			/* The plug-in's priority.                        */
	short	generalInfoSize;	/* The size of the general info.				  */
	short	typeInfoSize;		/* The size of the type specific info.			  */
	short	supportsMode;		/* A bit mask indicating supported image modes.   */
	OSType	requireHost;		/* A required host if any.						  */

	}
PlugInInfo;

typedef struct PSBuffer *BufferID;

#define kPIGeneralInfoSize 16

/* Image modes */

#define plugInModeBitmap	   0
#define plugInModeGrayScale    1
#define plugInModeIndexedColor 2
#define plugInModeRGBColor	   3
#define plugInModeCMYKColor    4
#define plugInModeHSLColor	   5
#define plugInModeHSBColor	   6
#define plugInModeMultichannel 7
#define plugInModeDuotone	   8
#define plugInModeLabColor	   9

/* The Window procID for a Moveable Modal Dialog window that can be used
 *	under System 6.  WDEF(128) is part of Photoshop.
 */
 
#define PSmovableDBoxProc	(128*16+5)

/* Other commonly used structures */

typedef short PlaneMap [16];

typedef struct PlugInMonitor
	{
	Fixed gamma;
	Fixed redX;
	Fixed redY;
	Fixed greenX;
	Fixed greenY;
	Fixed blueX;
	Fixed blueY;
	Fixed whiteX;
	Fixed whiteY;
	Fixed ambient;
	}
PlugInMonitor;

typedef struct PSPixelMap
	{
	int32 version;
	VRect bounds;
	int32 imageMode;
	int32 rowBytes;
	int32 colBytes;
	int32 planeBytes;
	void *baseAddr;
	}
PSPixelMap;

#ifdef __cplusplus
extern "C"
	{
#endif

	typedef MACPASCAL Boolean (*TestAbortProc) (void);

	typedef MACPASCAL void (*ProgressProc) (int32 done, int32 total);

	typedef MACPASCAL void (*HostProc) (int16 selector, int32 *data);

	typedef MACPASCAL int32 (*SpaceProc) (void);

	typedef MACPASCAL Handle (*NewPIHandleProc) (int32 size);

	typedef MACPASCAL void (*DisposePIHandleProc) (Handle h);
	
	typedef MACPASCAL int32 (*GetPIHandleSizeProc) (Handle h);
	
	typedef MACPASCAL OSErr (*SetPIHandleSizeProc) (Handle h, int32 newSize);
	
	typedef MACPASCAL Ptr (*LockPIHandleProc) (Handle h, Boolean moveHigh);
	
	typedef MACPASCAL void (*UnlockPIHandleProc) (Handle h);

	typedef MACPASCAL OSErr (*AllocateBufferProc) (int32 size, BufferID *bufferID);

	typedef MACPASCAL Ptr (*LockBufferProc) (BufferID bufferID, Boolean moveHigh);

	typedef MACPASCAL void (*UnlockBufferProc) (BufferID bufferID);

	typedef MACPASCAL void (*FreeBufferProc) (BufferID bufferID);
	
	typedef MACPASCAL int32 (*BufferSpaceProc) (void);
	
	typedef MACPASCAL int16 (*CountPIResourcesProc) (ResType type);
	
	typedef MACPASCAL Handle (*GetPIResourceProc) (ResType type, int16 index);
	
	typedef MACPASCAL void (*DeletePIResourceProc) (ResType type, int16 index);
	
	typedef MACPASCAL OSErr (*AddPIResourceProc) (ResType type, Handle data);
	
	typedef MACPASCAL void (*ProcessEventProc) (void *event);
	
	typedef MACPASCAL OSErr (*DisplayPixelsProc) (const PSPixelMap *source,
												  const VRect *srcRect,
												  int32 dstRow,
												  int32 dstCol,
												  unsigned32 platformContext);

#ifdef __cplusplus
	};
#endif

#define kCurrentBufferProcsVersion 2

typedef struct BufferProcs
	{

	int16 bufferProcsVersion;
	
	int16 numBufferProcs;

	AllocateBufferProc allocateProc;

	LockBufferProc lockProc;

	UnlockBufferProc unlockProc;

	FreeBufferProc freeProc;
	
	BufferSpaceProc spaceProc;

	}
BufferProcs;

#define kCurrentBufferProcsCount \
	((sizeof(BufferProcs) - offsetof(BufferProcs, allocateProc)) / sizeof(void *)) 

#define kCurrentResourceProcsVersion 3

typedef struct ResourceProcs
	{
	
	int16 resourceProcsVersion;
	
	int16 numResourceProcs;
	
	CountPIResourcesProc countProc;
	
	GetPIResourceProc getProc;
	
	DeletePIResourceProc deleteProc;
	
	AddPIResourceProc addProc;
	
	}
ResourceProcs;

#define kCurrentResourceProcsCount \
	((sizeof(ResourceProcs) - offsetof(ResourceProcs, countProc)) / sizeof(void *)) 

#define kCurrentHandleProcsVersion 1

typedef struct HandleProcs
	{
	
	int16 handleProcsVersion;
	
	int16 numHandleProcs;
	
	NewPIHandleProc newProc;
	
	DisposePIHandleProc disposeProc;
	
	GetPIHandleSizeProc getSizeProc;
	
	SetPIHandleSizeProc setSizeProc;
	
	LockPIHandleProc lockProc;
	
	UnlockPIHandleProc unlockProc;
	
	}
HandleProcs;

#define kCurrentHandleProcsCount \
	((sizeof(HandleProcs) - offsetof(HandleProcs, newProc)) / sizeof(void *)) 


/* turn off the byte align pragma back to its original state; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a.
#endif

#endif

