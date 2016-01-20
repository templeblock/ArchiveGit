/*
 * Name:
 *	ADMIcon.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Standard Enviornment Configuration
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 8/1/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __ADMIcon__
#define __ADMIcon__

/*
 * Includes
 */
 
#ifndef __ADMTypes__
#include "ADMTypes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN

// Problematic Macintosh Headers #define GetItem.
#ifdef GetItem
#undef GetItem
#endif

/*
 * Constants
 */

#define kADMIconSuite				"ADM Icon Suite"

#define kADMIconSuiteVersion1		1
#define kADMIconSuiteVersion		kADMIconSuiteVersion1


typedef enum {
	// Mac types
	kCICN, kPICT, kIconSuite,
	// Windows types
	kWinIcon, kBMP,
	// Either type
	kUnknown
} ADMIconType;


/*
 *  Icon Suite
 */
typedef struct ADMIconSuite1
{
	ADMIconRef ASAPI (*GetFromResource)(SPPluginRef pluginRef, int iconID, int iconIndex);
	ADMIconRef ASAPI (*Create)(ADMIconType type, int width, int height, void *data);
	void ASAPI (*Destroy)(ADMIconRef icon);

	ADMIconType ASAPI (*GetType)(ADMIconRef icon);
	int ASAPI (*GetWidth)(ADMIconRef icon);
	int ASAPI (*GetHeight)(ADMIconRef icon);
	ASAPI void * (*GetData)(ADMIconRef icon);
	ASBoolean ASAPI (*IsFromResource)(ADMIconRef icon);

} ADMIconSuite1;

typedef ADMIconSuite1 ADMIconSuite;

#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif








