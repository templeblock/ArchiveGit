//-------------------------------------------------------------------------------
//
//	File:
//		FileUtilities.h
//
//	Copyright 1996-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains prototypes and macros
//		to simplify the use of extremely common file
//		functions for plug-ins. 
//
//	Use:
//		FileUtilities is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	6/1/1996	Created for Photoshop 4.0
//			Written by Andrew Coven
//
//		Version 1.0.1	4/8/1997	Updated for Photoshop 4.0.1
//			Comments fattened out.  Reorganized for clarity.
//
//-------------------------------------------------------------------------------

#ifndef __FileUtilities_H__			// Have we defined this yet?
#define __FileUtilities_H__			// Define once.

#include "PITypes.h"				// Standard Photoshop types.
#include "PIUtilities.h"			// Utilities header.
#include "PIDefines.h"				// Plug-in definitions.

//-------------------------------------------------------------------------------
//	Structures -- Platform specific structures.
//-------------------------------------------------------------------------------

#ifdef __PIWin__
	typedef Handle					PIPlatformFileHandle;
	typedef	HFILE					FileHandle;
	typedef Handle					AliasHandle;
	typedef struct
	{	
		char fileName [256];
	
	} SFReply;
	
	// Mac toolbox routines need to be implemented for Windows:

	OSErr FSWrite(int32 refNum,long *count,void *buffPtr); 
	OSErr FSRead(int32 refNum,long *count,void *buffPtr); 
	OSErr SetFPos(int32 refNum, short posMode, long posOff);
	#define fsFromStart	0

#elif defined(__PIMac__)

	#include <StandardFile.h>
	#include <LowMem.h>

	// On Macintosh systems, FileHandle is a typedef for RefNum
	typedef short			FileHandle;
	typedef AliasHandle		PIPlatformFileHandle;

#endif

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

Boolean PISetSaveDirectory (Handle alias, Str255 s, 
				   Boolean query,
				   short *rVRefNum);
				   
Boolean TestAndStoreResult (short *res, OSErr result);
#define TSR(x) TestAndStoreResult (&gResult, x)

Boolean TestAndStoreCancel (short *res, Boolean tocancel);
#define TSC(x) TestAndStoreCancel (&gResult, x)

Boolean PICreateFile (Str255 filename, 
					  short vRefNum,
					  const ResType creator,
					  const ResType type, 
					  FileHandle *fRefNum,
					  short *result);

Boolean PICloseFile (Str255 filename, 
					 short vRefNum,
					 FileHandle fRefNum, 
					 Boolean sameNames,
					 Boolean *dirty, 
					 AliasHandle *alias,
					 short *result);

void PICloseAndOpenWD(const short vRefNum, const long dirID, short *rVRefNum);

//-------------------------------------------------------------------------------

#endif // __FileUtilites_H__
