/*
 * Name:
 *	SPMessageData.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Define SP Message Data.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First Version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPMessageData__
#define __SPMessageData__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 ** Types
 **
 **/

/* If the message data associated with a call to a plug-in has come from *
 * SPSendMessage, or is prepared using SetupMessageData(), this constant *
 * will be in the first four bytes of the message, in SPCheck.			 */
#define kSPValidSPMessageData 'SPCk'

typedef struct SPMessageData {

	long SPCheck;			/* kSPValidSPMessageData if a valid SPMessage */
	struct SPPlugin *self;  /* SPPluginRef */
	void *globals;
	struct SPBasicSuite *basic;

} SPMessageData;


#ifdef __cplusplus
}
#endif

#endif
