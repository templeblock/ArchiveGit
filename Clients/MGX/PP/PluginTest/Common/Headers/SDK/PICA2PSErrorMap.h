//-------------------------------------------------------------------------------
//
//	File:
//		PICA2PSErrorMap.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Distribution:
//		PUBLIC
//
//	Description:
//		This file contains all the API error codes for Photoshop and
//		PICA (Sweet Pea).  It maps all the PICA error codes to their
//		Photoshop equivalents.
//
//	Version history:
//		Version 1.0.0	10/9/1997	Ace		Initial compilation.
//			Updated for new headers for Photoshop 5.0.  Broke out
//			ResDefines into separate file for PIGeneral.h and PIGeneral.r.
//
//-------------------------------------------------------------------------------

#ifndef __PICA2PSErrorMap_h__ // Already defined?
#define __PICA2PSErrorMap_h__

//-------------------------------------------------------------------------------
//	PICA headers.
//-------------------------------------------------------------------------------
#include "SPAccess.h"
#include "SPAdapts.h"
#include "SPBckDbg.h"
#include "SPBlocks.h"
#include "SPFiles.h"
#include "SPHost.h"
#include "SPInterf.h"
#include "SPPlugs.h"
#include "SPProps.h"
#include "SPSuites.h"
#include "ADMDialogGroup.h"
#include "ASHelp.h"
#include "ASTypes.h"

//-------------------------------------------------------------------------------
//	Photoshop headers.
//-------------------------------------------------------------------------------
#include "PIAcquire.h"
#include "PIParser.h"
#include "PISelection.h"
#include "PIPicker.h"
#include "PIParser.h"
#include "PIGeneral.h"
#include "PIFormat.h"
#include "PIExtension.h"
#include "PIExport.h"
#include "PIActions.h"
#include "PSErrorDefines.h"
#include "PSErrors.h"

#define noMap -1

typedef struct SPErrorMapEntry
	{
	
	OSType	suitePeaErrorCode;
	OSErr	photoshopErrorCode;
	
	};
	
static SPErrorMapEntry spErrorMap [] =
	{
		kSPCantAcquirePluginError	/* SPAccess.h: '!Acq' */,
			noMap	/* -12345 */,
		kSPCantReleasePluginError	/* SPAccess.h: '!Rel' */,
			noMap	/* -12345 */,
		kSPPluginAlreadyReleasedError	/* SPAccess.h: 'AlRl' */,
			noMap	/* -12345 */,
		kSPAdapterAlreadyExistsError	/* SPAdapts.h: 'AdEx' */,
			noMap	/* -12345 */,
		kSPBadAdapterListIteratorError	/* SPAdapts.h: 'BdAL' */,
			noMap	/* -12345 */,
		kSPCantChangeBlockDebugNowError	/* SPBlckDbg.h: '!Now' */,
			noMap	/* -12345 */,		
		kSPBlockDebugNotEnabledError	/* SPBlckDbg.h: '!Nbl' */,
			noMap	/* -12345 */,
		kSPOutOfMemoryError	/* SPBlocks.h: 0xFFFFFF6cL (-108) */,
			noMap	/* -12345 */,
		kSPBlockSizeOutOfRangeError	/* SPBlocks.h: 'BkRg' */,
			noMap	/* -12345 */,
		kSPTroubleAddingFilesError	/* SPFiles.h: 'TAdd' */,
			noMap	/* -12345 */,
		kSPBadFileListIteratorError	/* SPFiles.h: 'BFIt' */,
			noMap	/* -12345 */,
		kSPTroubleInitializingError	/* SPHost.h: 'TIni' */,
			noMap	/* -12345 */,
		kHostCanceledStartupPluginsError	/* SPHost.h: 'H!St' */,
			noMap	/* -12345 */,
		kSPNotASweetPeaPluginError	/* SPInterf.h: 'NSPP' */,
			noMap	/* -12345 */,
		kSPAlreadyInSPCallerError	/* SPInterf.h: 'AISC' */,
			noMap	/* -12345 */,
		kSPUnknownAdapterError	/* SPPlugs.h: '?Adp' */,
			noMap	/* -12345 */,
		kSPBadPluginListIteratorError	/* SPPlugs.h: 'PiLI' */,
			noMap	/* -12345 */,
		kSPBadPluginHost	/* SPPlugs.h: 'PiH0' */,
			errPlugInHostInsufficient	/* PIGeneral.h: -30900 */,
		kSPCantAddHostPluginError	/* SPPlugs.h: 'AdHo' */,
			noMap	/* -12345 */,
		kSPPluginNotFound	/* SPPlugs.h: 'P!Fd' */,
			errPlugInHostInsufficient	/* PIGeneral.h: -30900 */,
		kSPCorruptPiPLError	/* SPPlugs.h: 'CPPL' */,
			noMap	/* -12345 */,
		kSPBadPropertyListIteratorError	/* SPPlugs.h: 'BPrI' */,
			noMap	/* -12345 */,
		kSPSuiteNotFoundError	/* SPSuites.h: 'S!Fd' */,
			errPlugInHostInsufficient	/* PIGeneral.h: -30900 */,
		kSPSuiteAlreadyExistsError	/* SPSuites.h: 'SExi' */,
			noMap	/* -12345 */,
		kSPSuiteAlreadyReleasedError	/* SPSuites.h: 'SRel' */,
			noMap	/* -12345 */,
		kSPBadSuiteListIteratorError	/* SPSuites.h: 'SLIt' */,
			noMap	/* -12345 */,
		kSPBadSuiteInternalVersionError	/* SPSuites.h: 'SIVs' */,
			noMap	/* -12345 */,
		kDockHostConflictError	/* ADMDialogGroup.h: 'DOCK' */,
			noMap	/* -12345 */,
		kTabGroupNotFoundError	/* ADMDialogGroup.h: 'T!FD' */,
			noMap	/* -12345 */,
		kAlreadyDockedError		/* ADMDialogGroup.h: 'DCKD' */,
			noMap	/* -12345 */,
		kHelpError	/* ASHelp.h: '!Hlp' */,
			noMap	/* -12345 */,
		kNoErr	/* ASTypes.h: 0 */,
			noErr	/* PITypes.h: 0 */,
		kOutOfMemoryErr	/* ASTypes.h: '!MEM' */,
			memFullErr	/* PITypes.h: -108 */,
		kBadParameterErr	/* ASTypes.h: 'PARM' */,
			paramErr	/* PITypes.h: -50 */,
		kNotImplementedErr	/* ASTypes.h: '!IMP' */,
			errPlugInHostInsufficient	/* PIGeneral.h: -30900 */,
		kCantHappenErr	/* ASTypes.h: 'CANT' */,
			errPlugInPropertyUndefined	/* PIGeneral.h: -30901 */,
		kUserCancel	/* PIUBasic.h: 'STOP' */,
			userCanceledErr	/* PITypes.h: -128 */,
	};
	
#undef noMap

//-------------------------------------------------------------------------------

#endif // __PICA2PSErrorMap.h__
