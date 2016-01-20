/*
 * Name:
 *	PIHandleSuite.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Photoshop Handle Suite for Plug-ins.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1998	JF	First Version.
 *		Created by Josh Freeman.
 */

#ifndef __PIHandleSuite__
#define __PIHandleSuite__

#include "PIGeneral.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

#define kPSHandleSuite 			"Photoshop Handle Suite for Plug-ins"

//-------------------------------------------------------------------------------
//	Handle suite typedefs
//-------------------------------------------------------------------------------

typedef MACPASCAL void (*SetPIHandleLockProc) (Handle h, Boolean lock, Ptr *address, Boolean *oldLock);

//-------------------------------------------------------------------------------
//	Handle suite version 1
//-------------------------------------------------------------------------------

#define kPSHandleSuiteVersion1		1

typedef struct
{
	NewPIHandleProc New;
	DisposePIHandleProc Dispose;
	SetPIHandleLockProc SetLock;
	GetPIHandleSizeProc GetSize;
	SetPIHandleSizeProc SetSize;
	RecoverSpaceProc RecoverSpace;
	
} PSHandleSuite1;

/******************************************************************************/

#ifdef __cplusplus
}
#endif


#endif	// PIHandleSuite
