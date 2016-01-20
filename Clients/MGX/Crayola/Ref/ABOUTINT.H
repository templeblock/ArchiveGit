/*  $Header:   L:/dev/img/win32/lib/pvcs/aboutint.h_v   1.0   13 Jul 1993 12:31:20   tedr  $     */
/*  $Log:   L:/dev/img/win32/lib/pvcs/aboutint.h_v  $
 * 
 *    Rev 1.0   13 Jul 1993 12:31:20   tedr
 * Initial revision.
 * 
 *    Rev 1.0   09 May 1993 16:22:54   michaelb
 * Initial revision.
 * 
 *    Rev 1.2   09 Mar 1992 11:17:20   peterm
 * No change.
 * 
 *    Rev 1.1   17 Feb 1992 12:43:28   peterm
 * No change.
 * 
 *    Rev 1.0   10 Feb 1992 17:09:48   peterm
 * Initial revision.
 * 
 *    Rev 1.0   07 Feb 1992 09:57:02   peterm
 * Initial revision.
*/
/*
	File: AboutInterface.h

	Copyright 1992 by Adobe Systems, Inc.

	This file describes version 4.0 of Photoshop's plug-in module interface
	for the about box selector.
*/
#ifndef __ABOUTINTERFACE_H
#define __ABOUTINTERFACE_H

/* Operation selectors */

#define plugInSelectorAbout 	 0

typedef struct AboutRecord {

	PlatformData 	*platform;		/* Platform specific information. */

	char		reserved [252]; 	/* Set to zero */

	} AboutRecord, *AboutRecordPtr;

#endif
