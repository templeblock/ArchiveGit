/******************************************************************************
 **									 										 **
 ** 	Module:		QTMLCustomElements.h									 **						
 **									 										 **
 **									 										 **
 ** 	Purpose: 	Custom Elements Interface File.							 **			
 **									 										 **
 **									 										 **
 ** 	Copyright (C) 1997 Apple Computer, Inc.  All rights reserved.		 **
 **									 										 **
 **									 										 **
 *****************************************************************************/
#ifndef QTMLCustomElements_h
#define QTMLCustomElements_h

#include "QD3D.h"

#if defined(PRAGMA_ONCE) && PRAGMA_ONCE
	#pragma once
#endif  /*  PRAGMA_ONCE  */ 
 
#if defined(OS_MACINTOSH) && OS_MACINTOSH

#if defined(__xlc__) || defined(__XLC121__)
	#pragma options enum=int
	#pragma options align=power
#elif defined(__MWERKS__)
	#pragma enumsalwaysint on
	#pragma options align=native
#elif defined(__MRC__) || defined(__SC__)
	#if __option(pack_enums)
		#define PRAGMA_ENUM_RESET_CUSTOMELEMENTS 1
	#endif
	#pragma options(!pack_enums)
	#pragma options align=power
#endif

#endif  /* OS_MACINTOSH */

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */


/******************************************************************************
 **																			 **
 **								Export Control								 **
 **																			 **
 *****************************************************************************/
 
#if defined(_MSC_VER)	/* Microsoft Visual C */
	#if defined(WIN32_EXPORTING)	/* define when building DLL */
		#define CE_EXPORT __declspec( dllexport )	 
		#define CE_CALL	
		#define CE_CALLBACK	
	#else
		#define CE_EXPORT
		#define CE_CALL	__cdecl
		#define CE_CALLBACK	__cdecl	
	#endif /* WIN32_EXPORTING */
#else
	#define CE_EXPORT
	#define CE_CALL	
	#define CE_CALLBACK	
#endif  /*  _MSC_VER  */


/******************************************************************************
 **																			 **
 **						Name Data Structure Definitions						 **
 **																			 **
 *****************************************************************************/

#define	CEcNameElementName	"Apple Computer, Inc.:NameElement"


/******************************************************************************
 **																			 **
 **						Custom Name Element Functions						 **
 **																			 **
 *****************************************************************************/

CE_EXPORT TQ3Status CE_CALL CENameElement_SetData(
	TQ3Object			object,
	const char			*name);

CE_EXPORT TQ3Status CE_CALL CENameElement_GetData(
	TQ3Object			object,
	char				**name);

CE_EXPORT TQ3Status CE_CALL CENameElement_EmptyData(
	char				**name);


/******************************************************************************
 **																			 **
 **							URL Data Structure Definitions					 **
 **																			 **
 *****************************************************************************/

#define	CEcUrlElementName		"Apple Computer, Inc.:URLElement"

typedef enum TCEUrlOptions {
	kCEUrlOptionNone,
	kCEUrlOptionUseMap
} TCEUrlOptions;

typedef struct TCEUrlData {
	char				*url;
	char				*description;
	TCEUrlOptions		options;
} TCEUrlData;


/******************************************************************************
 **																			 **
 **						Custom URL Element Functions						 **
 **																			 **
 *****************************************************************************/

CE_EXPORT TQ3Status CE_CALL CEUrlElement_SetData(
	TQ3Object			object,
	TCEUrlData			*urlData);

CE_EXPORT TQ3Status CE_CALL CEUrlElement_GetData(
	TQ3Object			object,
	TCEUrlData			**urlData);

CE_EXPORT TQ3Status CE_CALL CEUrlElement_EmptyData(
	TCEUrlData			**urlData);


#ifdef __cplusplus
}
#endif	/* __cplusplus */


#if defined(OS_MACINTOSH) && OS_MACINTOSH

#if defined(__xlc__) || defined(__XLC121__)
	#pragma options enum=reset
	#pragma options align=reset
#elif defined(__MWERKS__)
	#pragma enumsalwaysint reset
	#pragma options align=reset
#elif defined(__MRC__) || defined(__SC__)
	#if PRAGMA_ENUM_RESET_CUSTOMELEMENTS
		#pragma options(pack_enums)
		#undef PRAGMA_ENUM_RESET_CUSTOMELEMENTS
	#endif
	#pragma options align=reset
#endif

#endif  /* OS_MACINTOSH */

#endif	/* QTMLCustomElements_h */

