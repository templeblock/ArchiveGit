//-------------------------------------------------------------------------------
//
//	File:
//		PIAbout.h
//
//	Copyright 1992-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Distribution:
//		PUBLIC
//
//	Description:
//		This file describes version 4.0 of Photoshop's plug-in module interface
//		for the about box selector.
//
//	Use:
//		Use in all types of Photoshop plug-ins.
//
//	Version history:
//		1.0.0	10/9/1997	Ace		Initial compilation.
//		
//-------------------------------------------------------------------------------

#ifndef __PIAbout__
#define __PIAbout__

#include "SPBasic.h"
#include "SPMData.h"

/******************************************************************************/
/* Pragma to byte align structures; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a-
#endif

/******************************************************************************/
#define plugInSelectorAbout 	 0

typedef struct AboutRecord 
	{

	void *		platformData;		/* Platform specific information. */
	
	SPBasicSuite *sSPBasic;			/* SuitePea basic suite */
	void		*plugInRef;			/* plugin reference used by SuitePea*/

	char		reserved [244]; 	/* Set to zero */

	}
AboutRecord, *AboutRecordPtr;

/******************************************************************************/
/* turn off the byte align pragma back to its original state; only for Borland C */

#if defined(__BORLANDC__)
#pragma option -a.
#endif

/******************************************************************************/

#endif // __PIAbout_h__ 
