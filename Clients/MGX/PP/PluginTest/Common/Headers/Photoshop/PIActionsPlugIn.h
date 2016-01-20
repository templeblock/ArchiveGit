//-------------------------------------------------------------------------------
//
//	File:
//		PIActionsPlugIn.h	
//
//	Copyright 1991-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Distribution:
//		PUBLIC
//
//	Description:
//		This file describes version 1.0 of Photoshop's actions plug-in module
//		interface.
//
//	Use:
//		Include in Photoshop Action plug-ins.
//		
//
//	Version history:
//		Version 1.0.0	1/15/98		CM		Initial compilation.
//		
//		
//
//-------------------------------------------------------------------------------

#ifndef __PIActionsPlugIn__
#define __PIActionsPlugIn__

#include "PIActions.h"
#include "PIGeneral.h"
#include "SPBasic.h"
#include "SPMData.h"

/********************************************************************************/


typedef struct PSActionsPlugInMessage
	{
	SPMessageData			d;
	PIActionParameters		*actionParameters;	/* For recording and playback */
	} PSActionsPlugInMessage;

#endif // __PIActionsPlugIn_h__
