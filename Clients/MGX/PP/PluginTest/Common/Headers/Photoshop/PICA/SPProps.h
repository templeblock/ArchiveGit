/*
 * Name:
 *	SPProperties.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SP Properties Suite
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First Version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPProperties__
#define __SPProperties__


/*******************************************************************************
 **
 **	Imports
 **
 **/

#include "SPTypes.h"
#include "SPMdata.h"
#include "SPPiPL.h"

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 **
 ** Constants
 **
 **/

#define kSPPropertiesSuite				"SP Properties Suite"
#define kSPPropertiesSuiteVersion		2

#define kSPPropertiesCaller				"SP Properties"
#define kSPPropertiesAcquireSelector	"Acquire"
#define kSPPropertiesReleaseSelector	"Release"


/*******************************************************************************
 **
 ** Types
 **
 **/

typedef struct SPProperty *SPPropertyRef;
typedef struct SPPropertyList *SPPropertyListRef;
typedef struct SPPropertyListIterator *SPPropertyListIteratorRef;

typedef struct SPPropertiesMessage {

	SPMessageData d;

	PIType vendorID;
	PIType propertyKey;
	long propertyID;

	void *property;
	long refCon;
	long cacheable;

} SPPropertiesMessage;


/*******************************************************************************
 **
 ** Suite
 **
 **/

typedef struct SPPropertiesSuite {

	SPAPI SPErr (*AllocatePropertyList)( SPPropertyListRef *propertyList );
	SPAPI SPErr (*FreePropertyList)( SPPropertyListRef propertyList );
	
	SPAPI SPErr (*AddProperties)( SPPropertyListRef propertyList, PIPropertyList *pList, long refCon, long cacheable );
	SPAPI SPErr (*AddProperty)( SPPropertyListRef propertyList, PIType vendorID, PIType propertyKey, long propertyID, PIProperty *p, long refCon, long cacheable, SPPropertyRef *property );

	SPAPI SPErr (*FindProperty)( SPPropertyListRef propertyList, PIType vendorID, PIType propertyKey, long propertyID, SPPropertyRef *property );

	SPAPI SPErr (*NewPropertyListIterator)( SPPropertyListRef propertyList, SPPropertyListIteratorRef *iter );
	SPAPI SPErr (*NextProperty)( SPPropertyListIteratorRef iter, SPPropertyRef *property );
	SPAPI SPErr (*DeletePropertyListIterator)( SPPropertyListIteratorRef iter );

	SPAPI SPErr (*GetPropertyPIProperty)( SPPropertyRef property, PIProperty **p );
	SPAPI SPErr (*GetPropertyRefCon)( SPPropertyRef property, long *refCon );
	SPAPI SPErr (*GetPropertyCacheable)( SPPropertyRef property, long *cacheable );
	SPAPI SPErr (*GetPropertyAllocatedByPlugin)( SPPropertyRef property, long *allocatedByPlugin );

} SPPropertiesSuite;


SPAPI SPErr SPAllocatePropertyList( SPPropertyListRef *propertyList );
SPAPI SPErr SPFreePropertyList( SPPropertyListRef propertyList );

SPAPI SPErr SPAddProperties( SPPropertyListRef propertyList, PIPropertyList *pList, long refCon, long cacheable );
SPAPI SPErr SPAddProperty( SPPropertyListRef propertyList, PIType vendorID, PIType propertyKey, long propertyID, PIProperty *p, long refCon, long cacheable, SPPropertyRef *property );

SPAPI SPErr SPFindProperty( SPPropertyListRef propertyList, PIType vendorID, PIType propertyKey, long propertyID, SPPropertyRef *property );

SPAPI SPErr SPNewPropertyListIterator( SPPropertyListRef propertyList, SPPropertyListIteratorRef *iter );
SPAPI SPErr SPNextProperty( SPPropertyListIteratorRef iter, SPPropertyRef *property );
SPAPI SPErr SPDeletePropertyListIterator( SPPropertyListIteratorRef iter );

SPAPI SPErr SPGetPropertyPIProperty( SPPropertyRef property, PIProperty **p );
SPAPI SPErr SPGetPropertyRefCon( SPPropertyRef property, long *refCon );
SPAPI SPErr SPGetPropertyCacheable( SPPropertyRef property, long *cacheable );
SPAPI SPErr SPGetPropertyAllocatedByPlugin( SPPropertyRef property, long *allocatedByPlugin );


/*******************************************************************************
 **
 **	Errors
 **
 **/

#define kSPCorruptPiPLError					'CPPL'
#define kSPBadPropertyListIteratorError		'BPrI'


#ifdef __cplusplus
}
#endif

#endif
