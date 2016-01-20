/*
 * Name:
 *	SPFiles.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Define the SP Files Suite.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPFiles__
#define __SPFiles__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPProps.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kSPFilesSuite				"SP Files Suite"
#define kSPFilesSuiteVersion		3

#define kSPRuntimeFileList			((SPFileListRef)NULL)

#define kFileDoesNotHavePiPL		(SPPropertyListRef)0xffffffff
#define kFileHasMulitplePiPLs		NULL

/*******************************************************************************
 **
 ** Types
 **
 **/
#define kMaxPathLength 300

typedef struct SPFile *SPFileRef;
typedef struct SPFileList *SPFileListRef;
typedef struct SPFileListIterator *SPFileListIteratorRef;

#ifdef MAC_ENV
#ifdef __MWERKS__
#pragma options align=mac68k
#endif

typedef struct SPPlatformFileSpecification_t {  /* identical to FSSpec */
	short vRefNum;
	long parID;
	unsigned char name[64];
} SPPlatformFileSpecification;

#ifdef __MWERKS__
#pragma options align=reset
#endif

typedef struct {
	unsigned long attributes;
	unsigned long creationDate;
	unsigned long modificationDate;
	unsigned long finderType;
	unsigned long finderCreator;
	unsigned short finderFlags;
} SPPlatformFileInfo;
#endif


#ifdef WIN_ENV
typedef struct SPPlatformFileSpecification_t {
	char path[kMaxPathLength];
} SPPlatformFileSpecification;

typedef struct {
	unsigned long attributes;
	unsigned long lowCreationTime;
	unsigned long highCreationTime;
	unsigned long lowModificationTime;
	unsigned long highModificationTime;
	char *extension;
} SPPlatformFileInfo;
#endif

typedef SPBoolean (*SPAddPiPLFilterProc)( SPPlatformFileInfo *info );

/*******************************************************************************
 **
 ** Suite
 **
 **/

typedef struct SPFilesSuite {

	SPAPI SPErr (*AllocateFileList)( SPFileListRef *fileList );
	SPAPI SPErr (*FreeFileList)( SPFileListRef fileList );

	SPAPI SPErr (*AddFiles)( SPFileListRef fileList, SPPlatformFileSpecification *file );

	SPAPI SPErr (*NewFileListIterator)( SPFileListRef fileList, SPFileListIteratorRef *iter );
	SPAPI SPErr (*NextFile)( SPFileListIteratorRef iter, SPFileRef *file );
	SPAPI SPErr (*DeleteFileListIterator)( SPFileListIteratorRef iter );

	SPAPI SPErr (*GetFileSpecification)( SPFileRef file, SPPlatformFileSpecification *fileSpec );
	SPAPI SPErr (*GetFileInfo)( SPFileRef file, SPPlatformFileInfo *info );

	SPAPI SPErr (*GetIsAPlugin)( SPFileRef file, SPBoolean *isAPlugin );
	SPAPI SPErr (*SetIsAPlugin)( SPFileRef file, SPBoolean isAPlugin );

	SPAPI SPErr (*GetFilePropertyList)( SPFileRef file, SPPropertyListRef *propertList );
	SPAPI SPErr (*SetFilePropertyList)( SPFileRef file, SPPropertyListRef propertList );
	
} SPFilesSuite;


SPAPI SPErr SPAllocateFileList( SPFileListRef *fileList );
SPAPI SPErr SPFreeFileList( SPFileListRef fileList );
SPAPI SPErr SPAddFiles( SPFileListRef fileList, SPPlatformFileSpecification *file );

SPAPI SPErr SPNewFileListIterator( SPFileListRef fileList, SPFileListIteratorRef *iter );
SPAPI SPErr SPNextFile( SPFileListIteratorRef iter, SPFileRef *file );
SPAPI SPErr SPDeleteFileListIterator( SPFileListIteratorRef iter );

SPAPI SPErr SPGetFileSpecification( SPFileRef file, SPPlatformFileSpecification *fileSpec );
SPAPI SPErr SPGetFileInfo( SPFileRef file, SPPlatformFileInfo *info );

SPAPI SPErr SPGetIsAPlugin( SPFileRef file, SPBoolean *isAPlugin );
SPAPI SPErr SPSetIsAPlugin( SPFileRef file, SPBoolean isAPlugin );

SPAPI SPErr SPGetFilePropertyList( SPFileRef file, SPPropertyListRef *propertList );
SPAPI SPErr SPSetFilePropertyList( SPFileRef file, SPPropertyListRef propertList );

SPAPI SPErr SPAddFilePiPLs( SPFileListRef fileList, SPAddPiPLFilterProc filter );

/*******************************************************************************
 **
 **	Errors
 **
 **/

#define kSPTroubleAddingFilesError	'TAdd'
#define kSPBadFileListIteratorError	'BFIt'


#ifdef __cplusplus
}
#endif

#endif
