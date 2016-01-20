/***************************************************************************/
/*                                                                         */
/* MODULE:  FPXAPI.h - FlashPix extension API include file                	*/
/*                                                                         */
/*                                                                         */
/* DATE CREATED:  10/21/1996                                               */
/*                                                                         */
/* $Date:: 3/03/99 6:00p  $                                                */
/* $Revision:: 1    $                                                      */
/*                                                                         */
/* Copyright (c) 1996, AccuSoft Corporation.  All rights reserved.         */
/*                                                                         */
/***************************************************************************/

#ifndef __FPXAPI_A_H__
#define __FPXAPI_A_H__

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************/
/* Compile time version macros                                             */
/***************************************************************************/

#define GXT_FPX_REV_MAJOR		1 
#define GXT_FPX_REV_MINOR		2
#define GXT_FPX_REV_UPDATE		0

/***************************************************************************/
/* FlashPix private AT_MODE constants                                      */
/***************************************************************************/

#define	IG_FPX_LOAD_DEFAULT						0
#define  IG_FPX_LOAD_NOLESS						1
#define  IG_FPX_LOAD_NOGREATER					2

#define  IG_FPX_RESOLUTION_INCHES				0
#define  IG_FPX_RESOLUTION_METERS				1
#define  IG_FPX_RESOLUTION_CENTIMETERS			2
#define  IG_FPX_RESOLUTION_MILLIMETERS			3

/***************************************************************************/
/* FlashPix private error codes                                            */
/***************************************************************************/

/***************************************************************************/
/* FlashPix data exchange tags															*/
/***************************************************************************/

/* Transforms:											201xx											.			*/

#define IGTAG_FPX_ROI                  		20101	/*	ARRAY OF AT_REAL [4]						*/
#define IGTAG_FPX_FILTERING_VALUE				20102	/*				AT_REAL							*/
#define IGTAG_FPX_AFFINE_MATRIX					20103	/*	ARRAY OF AT_REAL [4]						*/
#define IGTAG_FPX_ASPECT_RATIO					20104	/* 			AT_REAL							*/
#define IGTAG_FPX_COLORTWIST_MATRIX				20105	/*	ARRAY OF AT_REAL [4]						*/
#define IGTAG_FPX_CONTRAST_ADJUSTMENT			20106	/* 			AT_REAL							*/


/* Summary information group:						202xx														*/

#define IGTAG_FPX_TITLE                		20201	/* 			string							*/		
#define IGTAG_FPX_SUBJECT              		20202	/* 			string                     */
#define IGTAG_FPX_AUTHOR               		20203	/* 			string                     */
#define IGTAG_FPX_KEYWORDS             		20204	/* 			string                     */
#define IGTAG_FPX_COMMENTS             		20205	/* 			string                     */
#define IGTAG_FPX_LAST_AUTHOR          		20206	/* 			string                     */
#define IGTAG_FPX_REV_NUMBER           		20207	/* 			string                     */
#define IGTAG_FPX_EDIT_TIME            		20208	/* 			FILETIME				         */
#define IGTAG_FPX_LAST_PRINTED         		20209	/* 			FILETIME				         */
#define IGTAG_FPX_CREATE_DTM           		20210	/* 			FILETIME				         */
#define IGTAG_FPX_LAST_SAVE_DTM        		20211	/* 			FILETIME				         */
#define IGTAG_FPX_THUMBNAIL_CLIPFMT      		20212	/* 			ULONG								*/
#define IGTAG_FPX_THUMBNAIL_BITMAP      		20213	/* 			BITMAP							*/
#define IGTAG_FPX_APPNAME              		20214	/* 			string							*/


/* File source group...								203xx														*/

#define IGTAG_FPX_FILE_SOURCE                20301 /*          AT_MODE (AT_FPX_SOURCE)		*/
#define IGTAG_FPX_SCENE_TYPE                 20302 /*          AT_MODE (AT_FPX_SCENE)		*/
#define IGTAG_FPX_CREATION_PATH              20303 /* ARRAY OF LONG								*/
#define IGTAG_FPX_NAME_MAN_RELEASE           20304 /*          wide string						*/
#define IGTAG_FPX_USER_DEFINED_ID            20305 /*          wide string						*/
#define IGTAG_FPX_ORIGINAL_SHARPNESS_APPROXIMATION 20306 /*    AT_REAL							*/


/* Intellectual property group...				204xx														*/

#define IGTAG_FPX_COPYRIGHT								20401 /*    wide string						*/
#define IGTAG_FPX_LEGAL_BROKER_FOR_ORIG_IMAGE      20402 /*    wide string						*/
#define IGTAG_FPX_LEGAL_BROKER_FOR_DIGITAL_IMAGE   20403 /*    wide string						*/
#define IGTAG_FPX_AUTHORSHIP                       20404 /*    wide string						*/
#define IGTAG_FPX_INTELLECTUAL_PROP_NOTES          20405 /*    wide string						*/
						

/* Content description group						205xx														*/	

#define IGTAG_FPX_TEST_TARGET_IN_THE_IMAGE	20501	/*				AT_MODE (AT_FPX_TEST_TARGET)	*//*!!*/
#define IGTAG_FPX_GROUP_CAPTION              20502 /*				wide string						*/
#define IGTAG_FPX_CAPTION_TEXT               20503 /*          wide string						*/
#define IGTAG_FPX_PEOPLE_IN_THE_IMAGE			20504	/*	ARRAY OF wide string 					*/
#define IGTAG_FPX_THINGS_IN_IMAGE				20505	/* ARRAY OF wide string						*/
#define IGTAG_FPX_DATE_OF_ORIGINAL_IMAGE		20506	/*			   FILETIME				*/
#define IGTAG_FPX_EVENTS_IN_THE_IMAGE			20507	/*	ARRAY OF wide string						*/
#define IGTAG_FPX_PLACES_IN_THE_IMAGE			20508	/*	ARRAY OF wide string						*/
#define IGTAG_FPX_CONTENT_DESCRIPTION_NOTES  20509 /*          wide string 					*/


/*Camera description group...*/

#define IGTAG_FPX_CAMERA_MANUFACTURER_NAME   20601 /*          wide string						*/
#define IGTAG_FPX_CAMERA_MODEL_NAME          20602 /*          wide string						*/
#define IGTAG_FPX_CAMERA_SERIAL_NUMBER       20603 /*          wide string 					*/


/*	Per Picture Camera Settings Group */

#define IGTAG_FPX_CAPTURE_DATE					20701	/* 			FILETIME        							*/
#define IGTAG_FPX_EXPOSURE_TIME					20702	/* 			AT_REAL              					*/
#define IGTAG_FPX_F_NUMBER							20703	/* 			AT_REAL              					*/
#define IGTAG_FPX_EXPOSURE_PROGRAM				20704	/* 			AT_MODE	(AT_FPX_EXPOSURE_PROGRAM)	*/
#define IGTAG_FPX_BRIGHTNESS_VALUE				20705	/* ARRAY OF AT_REAL       								*/
#define IGTAG_FPX_EXPOSURE_BIAS_VALUE			20706	/* 			AT_REAL              					*/
#define IGTAG_FPX_SUBJECT_DISTANCE				20707	/* ARRAY OF AT_REAL       								*/
#define IGTAG_FPX_METERING_MODE					20708	/* 			AT_MODE	(AT_FPX_METERING_MODE) 		*/
#define IGTAG_FPX_SCENE_ILLUMINANT				20709	/* 			AT_MODE	(AT_FPX_SCENE_ILLUMINANT)	*/
#define IGTAG_FPX_FOCAL_LENGTH					20710	/* 			AT_REAL										*/
#define IGTAG_FPX_MAXIMUM_APERTURE_VALUE		20711	/* 			AT_REAL										*/
#define IGTAG_FPX_FLASH								20712	/* 			AT_MODE	(AT_FPX_FLASH)					*/
#define IGTAG_FPX_FLASH_ENERGY					20713	/* 			AT_REAL              					*/
#define IGTAG_FPX_FLASH_RETURN					20714	/* 			AT_MODE	(AT_FPX_FLASH_RETURN 		*/
#define IGTAG_FPX_BACK_LIGHT						20715	/*				AT_MODE	(AT_FPX_BACK_LIGHT   		*/
#define IGTAG_FPX_SUBJECT_LOCATION				20716	/*	ARRAY OF AT_REAL       								*/
#define IGTAG_FPX_EXPOSURE_INDEX					20717	/*				AT_REAL										*/
#define IGTAG_FPX_FX_OPTICAL_FILTER				20718	/*				AT_MODE	(AT_FPX_FX_OPTICAL_FILTER) */
#define IGTAG_FPX_PER_PICTURE_NOTES				20719	/*				wide string         						*/


/*	Digital Camera Characterization Group																				*/

#define IGTAG_FPX_SENSING_METHOD					20801	/*				AT_MODE	(AT_FPX_SENSINGMETHOD)		*/						
#define IGTAG_FPX_FOCAL_PLANE_X_RESOLUTION	20802	/*				AT_REAL             						*/			
#define IGTAG_FPX_FOCAL_PLANE_Y_RESOLUTION	20803	/*				AT_REAL             						*/			
#define IGTAG_FPX_FOCAL_PLANE_RESOLUTION_UNIT	20804	/*			AT_MODE  (AT_FPX_RESOLUTION_UNIT)	*/				
#define IGTAG_FPX_SPACIAL_FREQUENCY				20805	/*	ARRAY OF structure (not available in level A)*/
#define IGTAG_FPX_CFA_PATTERN						20806	/*	ARRAY OF structure (not available in level A)*/
#define IGTAG_FPX_SPECTRAL_SENSITIVITY			20807	/*				string				           			*/	
#define IGTAG_FPX_ISO_SPEED_RATINGS				20808	/*	ARRAY OF SHORT     								   */
#define IGTAG_FPX_OECF								20809	/*	ARRAY OF structure (not available in level A)*/


/* Film Description Group;	*/

#define IGTAG_FPX_FILM_BRAND			         20901 /*          wide string                */
#define IGTAG_FPX_FILM_CATEGORY		         20902 /*          AT_MODE (AT_FPX_FILM_CATEGORY) */
#define IGTAG_FPX_FILM_SIZE			         20903 /*          structure				         */
#define IGTAG_FPX_FILM_SIZE_X			         20904 /*          AT_REAL				         */
#define IGTAG_FPX_FILM_SIZE_Y			         20905 /*          AT_REAL 				         */
#define IGTAG_FPX_FILM_SIZE_UNIT		         20906 /*          AT_MODE (AT_FPX_RESOLUTION_UNIT 		)*/
#define IGTAG_FPX_FILM_ROLL_NUMBER	         20907 /*          USHORT			            */
#define IGTAG_FPX_FILM_FRAME_NUMBER	         20908 /*          USHORT			            */


/* Original Document Scan Description Group;*/

#define IGTAG_FPX_SCANNED_IMAGE_SIZE			21001 /*				structure (not available in level A, use next three fields) */
#define IGTAG_FPX_SCANNED_IMAGE_XSIZE			21002 /*				AT_REAL							*/
#define IGTAG_FPX_SCANNED_IMAGE_YSIZE			21003 /*				AT_REAL							*/
#define IGTAG_FPX_SCANNED_IMAGE_UNIT			21004 /*          AT_MODE (AT_FPX_RESOLUTION_UNIT 		)*/
#define IGTAG_FPX_DOCUMENT_SIZE					21005 /*				structure (not available in level A, use next three fields) */
#define IGTAG_FPX_DOCUMENT_XSIZE					21006 /*				AT_REAL							*/
#define IGTAG_FPX_DOCUMENT_YSIZE					21007 /*				AT_REAL							*/
#define IGTAG_FPX_DOCUMENT_UNIT					21008 /*          AT_MODE (AT_FPX_RESOLUTION_UNIT 		)*/
#define IGTAG_FPX_ORIGINAL_MEDIUM				21009 /*				AT_MODE	(AT_FPX_ORIGINAL_MEDIUM)				*/
#define IGTAG_FPX_TYPE_OF_REFLECTION_ORIGINAL	21010 /*			AT_MODE	(AT_TYPE_OF_REFLECTION_ORIGINAL)		*/						


/*	Scan Device Property Group */

#define IGTAG_FPX_SCANNER_MANUFACTURER_NAME	21101	/*				wide string						*/	
#define IGTAG_FPX_SCANNER_MODEL_NAME			21102	/*				wide string			         */			
#define IGTAG_FPX_SCANNER_SERIAL_NUMBER		21103	/*				string							*/			
#define IGTAG_FPX_SCAN_SOFTWARE					21104	/*				wide string						*/			
#define IGTAG_FPX_SCAN_SOFTWARE_REVISION_DATE	21105	/*			DATE        					*/			
#define IGTAG_FPX_SERVICE_BUREAU_ORG_NAME		21106	/*				wide string						*/			
#define IGTAG_FPX_SCAN_OPERATOR_ID				21107	/*				wide string						*/			
/*
#define IGTAG_FPX_SBA_DATA							21108	
#define IGTAG_FPX_SBA_VERSION_MAJOR				21109	
#define IGTAG_FPX_SBA_VERSION_MINOR				21110	
#define IGTAG_FPX_SBA_NEUTRAL_ON					21111	
#define IGTAG_FPX_SBA_COLOR_ON					21112	
#define IGTAG_FPX_SBA_SBA_DATA					21113	
*/
#define IGTAG_FPX_SCAN_DATE						21114	/*				FILETIME							*/			
#define IGTAG_FPX_LAST_MODIFIED_DATE			21115	/*				FILETIME							*/			
#define IGTAG_FPX_SCANNER_PIXEL_SIZE			21116	/*				AT_REAL							*/	

/* FPXExtensionDescription;				*/

/* ICC Profile									*/
#define IGTAG_FPX_ICC_PROFILE						21201	/*				ARRAY of RAWBYTES					*/	



#define	IG_EXT_FLASHPIX	0

						
/***************************************************************************/
/* Extension Names																			*/
/***************************************************************************/



#ifdef WIN32
	#ifndef AM_EXTENSION_PROGOLD /*!!*/
/*		#define IG_EXT_NAME "\\accusoft\\gear\\work\\extensns\\fpx\\StdDbg\\igfpx32s.dll" */
		#define IG_EXT_NAME "igfpx32s.dll" 
	#else
/*		#define IG_EXT_NAME "\\accusoft\\gear\\work\\extensns\\fpx\\GldDbg\\igfpx32p.dll" */
		#define IG_EXT_NAME "igfpx32p.dll"
	#endif
	/* no 16-bit support */
#endif


/***************************************************************************/
/* FLASHPIX image control 																	*/
/***************************************************************************/

#define	IG_CONTROL_FPX_SAVE_FROM_DIB					(IG_FORMAT_FPX|0x0100)
#define	IG_CONTROL_FPX_DISABLETRANSFORMS				(IG_FORMAT_FPX|0x0200)
#define	IG_CONTROL_FPX_PAGEBACKGROUND					(IG_FORMAT_FPX|0x0500)
#define	IG_CONTROL_FPX_PAGEWIDTH						(IG_FORMAT_FPX|0x0600)
#define	IG_CONTROL_FPX_PAGEHEIGHT						(IG_FORMAT_FPX|0x0700)
#define	IG_CONTROL_FPX_QUALITY							(IG_FORMAT_FPX|0x0800)
#define  IG_CONTROL_FPX_KEEP_ALPHA			         (IG_FORMAT_FPX|0x0900)  
#define	IG_CONTROL_FPX_LOAD_OPTION						(IG_FORMAT_FPX|0x0a00) 
	/* AT_MODE constant: IG_FPX_LOAD_DEFAULT, IG_FPX_LOAD_NOLESS, IG_FPX_LOAD_NOGREATER */
#define	IG_CONTROL_FPX_SAVE_SET_SCANNED_IMG_SIZE	(IG_FORMAT_FPX|0x0b00) 
   /* BOOL, set to true to set the SCANNED_IMAGE_SIZE tag according to the */
	/* resolution (DPI) of the image being saved */
	/* Default is FALSE */

/***************************************************************************/
/* Private extension access functions													*/
/***************************************************************************/

AT_ERRCOUNT CACCUAPI IG_ext_call_function(

	LPSTR					lpszExtension,
	UINT					nExtIndex,
	UINT					nFuncIndex,
	...
	);


#ifdef AM_EXTENSION_PROGOLD

/****************************************************************************/
/* Extension API function offset															 */
/****************************************************************************/

/* offset 0 is reserved for internal use */

#define IG_EXT_FPX_LOAD_DOCFILE							1 
#define IG_EXT_FPX_LOAD_STORAGE							2
#define IG_EXT_FPX_SAVE_DOCFILE							3	
#define IG_EXT_FPX_SAVE_STORAGE							4		
#define IG_EXT_FPX_CHECK									5 						
#define IG_EXT_FPX_DISABLE									6 
#define IG_EXT_FPX_ENABLE									7 
#define IG_EXT_FPX_UPDATE_FROM_DIB						8 
#define IG_EXT_FPX_TRANSFORMS_DISABLE					9 
#define IG_EXT_FPX_TRANSFORMS_ENABLE					10
#define IG_EXT_FPX_TRANSFORMS_ENABLED_CHECK			11
#define IG_EXT_FPX_ICC_PROFILE_GET						12								
#define IG_EXT_FPX_ICC_PROFILE_SET						13				
#define IG_EXT_FPX_ROI_GET									14	 						
#define IG_EXT_FPX_ROI_SET									15							
#define IG_EXT_FPX_FILTERING_VALUE_GET					16											
#define IG_EXT_FPX_FILTERING_VALUE_SET					17											
#define IG_EXT_FPX_AFFINE_MATRIX_GET					18											
#define IG_EXT_FPX_AFFINE_MATRIX_SET					19											
#define IG_EXT_FPX_ASPECT_RATIO_GET						20							
#define IG_EXT_FPX_ASPECT_RATIO_SET						21				
#define IG_EXT_FPX_COLOR_TWIST_GET						22	
#define IG_EXT_FPX_COLOR_TWIST_SET						23	
#define IG_EXT_FPX_CONTRAST_ADJUST_GET					24
#define IG_EXT_FPX_CONTRAST_ADJUST_SET					25							
#define IG_EXT_FPX_ROTATE									26
#define IG_EXT_FPX_SHEAR									27
#define IG_EXT_FPX_FLIP										28
#define IG_EXT_FPX_TRANSLATE								29	
#define IG_EXT_FPX_SCALE									30
#define IG_EXT_FPX_OUTLINE_RECTANGLE_GET				31			
#define IG_EXT_FPX_VIEW_SETTINGS_GET					32
#define IG_EXT_FPX_VIEW_SETTINGS_SET					33

#define IG_EXT_FPX_INFO_ITEM_CHECK						34
#define IG_EXT_FPX_INFO_ITEM_GET							35
#define IG_EXT_FPX_INFO_ITEM_SET							36
#define IG_EXT_FPX_INFO_ARRAY_LENGTH_GET				37
#define IG_EXT_FPX_INFO_ARRAY_LENGTH_SET				38
																	
/****************************************************************************/
/* Extension API data types																 */
/****************************************************************************/

/* Structures to access the FlashPix image info */

#define IG_FPX_MAX_ITEM_DEPTH	2

typedef struct tagAT_LEVEL
{
	AT_MODE	nItemID;
	UINT		nArrayIndex;
}  AT_LEVEL;
typedef AT_LEVEL FAR* LPAT_LEVEL;


typedef struct tagAT_ITEMADDRESS 
{
	int			nLevelCount;
	AT_LEVEL		AddressLevel[IG_FPX_MAX_ITEM_DEPTH];
} AT_ITEMADDRESS;

typedef AT_ITEMADDRESS FAR*	LPAT_ITEMADDRESS;


/****************************************************************************/
/* Extension API function prototypes (macros)										 */
/****************************************************************************/

#ifndef AM_EXTENSION

/* AT_ERRCOUNT IG_FPX_load_docfile(
		 const LPSTR lpszFileName,
	    const LPSTR lpszPathInFile,
		 LPHIGEAR    lphIGear       
   );         
*/

#define IG_FPX_load_docfile(lpszFileName, lpszPathInFile, lphIGear)				\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_LOAD_DOCFILE,								\
										 (const LPSTR)lpszFileName,							\
										 (const LPSTR)lpszPathInFile,							\
										 (LPHIGEAR)lphIGear))


/* AT_ERRCOUNT IG_FPX_load_storage(
	   IStorage    *lpStorage,     
	   const LPSTR lpszPathInFile, 
	   LPHIGEAR    lphIGear        
	   );         
*/

#define IG_FPX_load_storage(lpStorage,  lpszPathInFile, lphIGear)					\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_LOAD_STORAGE,								\
										 (IStorage*)lpStorage,									\
										 (const LPSTR)lpszPathInFile,							\
										 (LPHIGEAR)lphIGear))

/*AT_ERRCOUNT IG_FPX_save_docfile(
		HIGEAR      hIGear           
	   const LPSTR lpszFileName,    
	   const LPSTR lpszPathInFile,  
   );         
*/

#define IG_FPX_save_docfile(hIGear, lpszFileName, lpszPathInFile)					\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_SAVE_DOCFILE,								\
										 (HIGEAR)hIGear											\
										 (const LPSTR )lpszFileName,							\
										 (const LPSTR )lpszPathInFile))

/*
AT_ERRCOUNT IG_FPX_save_storage(
   HIGEAR      hIGear           
   IStorage    *lpStorage,      
   const LPSTR lpszPathInFile,  
   );         
*/

#define IG_FPX_save_storage(hIGear, lpStorage, lpszPathInFile)						\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_SAVE_STORAGE,								\
										 (HIGEAR)hIGear											\
										 (IStorage*)lpStorage,									\
										 (const LPSTR) lpszPathInFile))


/****************************************************************************/


/*BOOL IG_FPX_check(HIGEAR hIGear) */

#define IG_FPX_check(hIGear)																	\
	((BOOL)IG_ext_call_function(IG_EXT_NAME,												\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_CHECK,											\
										 (HIGEAR)hIGear))

/*AT_ERRCOUNT IG_FPX_disable(HIGEAR hIGear) */

#define IG_FPX_disable(hIGear)																\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_DISABLE,										\
										 (HIGEAR)hIGear))

/*AT_ERRCOUNT IG_FPX_enable(HIGEAR hIGear) */

#define IG_FPX_enable(hIGear)																	\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_ENABLE,										\
										 (HIGEAR)hIGear))


/*AT_ERRCOUNT IG_FPX_update_from_DIB(HIGEAR hIGear) */

#define IG_FPX_update_from_DIB(hIGear)														\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_UPDATE_FROM_DIB,							\
										 (HIGEAR)hIGear))

/*AT_ERRCOUNT IG_FPX_transforms_disable(HIGEAR hIGear) */

#define IG_FPX_transforms_disable(hIGear)													\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_TRANSFORMS_DISABLE,						\
										 (HIGEAR)hIGear))

/*AT_ERRCOUNT IG_FPX_transforms_enable(HIGEAR hIGear) */

#define IG_FPX_transforms_enable(hIGear)													\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_TRANSFORMS_ENABLE,							\
										 (HIGEAR)hIGear))

/*BOOL IG_FPX_transforms_enabled_check(HIGEAR hIGear) */

#define IG_FPX_transforms_enabled_check(hIGear)											\
	((BOOL)IG_ext_call_function(IG_EXT_NAME,												\
										 IG_EXT_FLASHPIX,											\
										 IG_EXT_FPX_TRANSFORMS_ENABLED_CHECK,				\
										 (HIGEAR)hIGear))

/****************************************************************************/
/****************************************************************************/

/*AT_ERRCOUNT IG_FPX_ROI_set(HIGEAR hIGear, LPAT_REAL lpMatrix) */

#define IG_FPX_ROI_set(hIGear, lpROI)														\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_ROI_SET,							\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpROI))




/*AT_ERRCOUNT IG_FPX_ROI_get(HIGEAR hIGear, LPAT_REAL lpROI) */

#define IG_FPX_ROI_get(hIGear, lpROI)														\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_ROI_GET,							\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpROI))

/*AT_ERRCOUNT IG_FPX_ROI_set(HIGEAR hIGear, LPAT_REAL lpMatrix) */

#define IG_FPX_ROI_set(hIGear, lpROI)														\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_ROI_SET,							\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpROI))


/*AT_ERRCOUNT IG_FPX_filtering_value_get(HIGEAR hIGear, LPAT_REAL lpValue) */

#define IG_FPX_filtering_value_get(hIGear, lpValue)				\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_FILTERING_VALUE_GET,			\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpValue))


/*AT_ERRCOUNT IG_FPX_filtering_value_set(HIGEAR hIGear, AT_REAL Value) */

#define IG_FPX_filtering_value_set(hIGear, Value)				\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_FILTERING_VALUE_SET,			\
												  (HIGEAR)hIGear,									\
												  (*((LPDWORD)&(AT_REAL)Value))))

/*AT_ERRCOUNT IG_FPX_affine_matrix_get(HIGEAR hIGear, LPAT_REAL lpMatrix) */

#define IG_FPX_affine_matrix_get(hIGear, lpMatrix)				\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_AFFINE_MATRIX_GET,				\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpMatrix))

/*AT_ERRCOUNT IG_FPX_affine_matrix_set(HIGEAR hIGear, LPAT_REAL lpMatrix) */

#define IG_FPX_affine_matrix_set(hIGear, lpMatrix)										\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_AFFINE_MATRIX_SET,				\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpMatrix))


/*AT_ERRCOUNT IG_FPX_result_aspect_ratio_get(HIGEAR hIGear, LPAT_REAL lpValue) */

#define IG_FPX_result_aspect_ratio_get(hIGear, lpValue)								\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_ASPECT_RATIO_GET,				\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpValue))


/*AT_ERRCOUNT IG_FPX_result_aspect_ratio_set(HIGEAR hIGear, AT_REAL Value) */

#define IG_FPX_result_aspect_ratio_set(hIGear, Value)									\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_ASPECT_RATIO_SET,				\
												  (HIGEAR)hIGear,									\
												  (*((LPDWORD)&(AT_REAL)Value))))

/*AT_ERRCOUNT IG_FPX_color_twist_matrix_get(HIGEAR hIGear, LPAT_REAL lpMatrix) */

#define IG_FPX_color_twist_matrix_get(hIGear, lpMatrix)										\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_COLOR_TWIST_GET,					\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpMatrix))

/*AT_ERRCOUNT IG_FPX_affine_matrix_set(HIGEAR hIGear, LPAT_REAL lpMatrix) */

#define IG_FPX_color_twist_matrix_set(hIGear, lpMatrix)										\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_COLOR_TWIST_SET,					\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpMatrix))


/*AT_ERRCOUNT IG_FPX_contrast_adjustment_get(HIGEAR hIGear, LPAT_REAL lpValue) */

#define IG_FPX_contrast_adjustment_get(hIGear, lpValue)				\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_CONTRAST_ADJUST_GET,			\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)lpValue))


/*AT_ERRCOUNT IG_FPX_contrast_adjustment_set(HIGEAR hIGear, AT_REAL Value) */

#define IG_FPX_contrast_adjustment_set(hIGear, Value)				\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_CONTRAST_ADJUST_SET,			\
												  (HIGEAR)hIGear,									\
												  (*((LPDWORD)&(AT_REAL)Value))))

/* AT_ERRCOUNT IG_FPX_rotate(HIGEAR hIGear, AT_REAL tetta, AT_MODE clipping_mode ); */

#define IG_FPX_rotate(hIGear, tetta, clipping_mode)									\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_ROTATE,								\
												  (HIGEAR)hIGear,									\
												  (*((LPDWORD)&(AT_REAL)tetta)),				\
												  (AT_MODE)clipping_mode))

/* AT_ERRCOUNT IG_FPX_shear(HIGEAR hIGear, AT_REAL shearRatio, AT_MODE direction, AT_MODE clipping_mode); */

#define IG_FPX_shear(hIGear, shearRatio, direction, clipping_mode)										\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_SHEAR,								\
												  (HIGEAR)hIGear,									\
												  (*((LPDWORD)&(AT_REAL)shearRatio)),		\
												  (AT_MODE)direction,							\
												  (AT_MODE)clipping_mode))

/* AT_ERRCOUNT IG_FPX_flip(HIGEAR hIGear, AT_MODE direction); */

#define IG_FPX_flip(hIGear, direction)														\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_FLIP,								\
												  (HIGEAR)hIGear,									\
												  (AT_MODE)direction))

/* AT_ERRCOUNT IG_FPX_translate(HIGEAR hIGear, AT_REAL dx, AT_REAL dy);*/
   
#define IG_FPX_translate(hIGear, dx, dy)													\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_TRANSLATE,							\
												  (HIGEAR)hIGear,									\
												  (*((LPDWORD)&(AT_REAL)dx)),					\
												  (*((LPDWORD)&(AT_REAL)dy))))

/* AT_ERRCOUNT IG_FPX_scale(HIGEAR hIGear, AT_REAL x, AT_REAL y);	*/

#define IG_FPX_scale(hIGear, x, y)															\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_SCALE,								\
												  (HIGEAR)hIGear,									\
												  (*((LPDWORD)&(AT_REAL)x)),					\
												  (*((LPDWORD)&(AT_REAL)y))))
/*
AT_ERRCOUNT IG_FPX_outline_rectangle_get(
   HIGEAR     hIGear,          
   LPAT_REAL  x0,              
   LPAT_REAL  y0,              
   LPAT_REAL  x1,              
   LPAT_REAL  y1               
   );
*/

#define IG_FPX_outline_rectangle_get(hIGear, x0, y0, x1, y1)															\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_OUTLINE_RECTANGLE_GET,			\
												  (HIGEAR)hIGear,									\
												  (LPAT_REAL)x0,									\
												  (LPAT_REAL)y0,									\
												  (LPAT_REAL)x1,									\
												  (LPAT_REAL)y1))


/*
AT_ERRCOUNT	IG_FPX_view_settings_get(
   HIGEAR     			hIGear,				
	LPUINT				lpTotalPages,
	LPUINT				lpPageNumber,
	LPAT_DIMENSION		lpWidth,				
	LPAT_DIMENSION		lpHeight, 			
	LPAT_MODE			lpColorspace,		
	LPBOOL				lpUseAlpha, 		
	LPAT_RGBQUAD		lpBackground)
*/

#define IG_FPX_view_settings_get(hIGear, lpTotalPages, lpPageNumber,			\
											lpWidth,	lpHeight, 								\
											lpColorspace,	lpUseAlpha,							\
											lpBackground)											\
	((AT_ERRCOUNT)IG_ext_call_function(	IG_EXT_NAME,									\
													IG_EXT_FLASHPIX,								\
													IG_EXT_FPX_VIEW_SETTINGS_GET,				\
													(HIGEAR)hIGear,								\
													(LPUINT)lpTotalPages,						\
													(LPUINT)lpPageNumber,						\
													(LPAT_DIMENSION)lpWidth,					\
													(LPAT_DIMENSION)lpHeight, 					\
													(LPAT_MODE)lpColorspace,					\
													(LPBOOL)lpUseAlpha, 							\
													(LPAT_RGBQUAD)lpBackground))				\


/*
AT_ERRCOUNT	IG_FPX_view_settings_set(
   HIGEAR     		hIGear,				
	UINT				nTotalPages,
	UINT				nPageNumber,
	AT_DIMENSION	width,				
	AT_DIMENSION	height, 			
	AT_MODE			nColorspace,		
	BOOL				bUseAlpha, 		
	LPAT_RGBQUAD	rgbBackground)
*/

#define IG_FPX_view_settings_set(hIGear, nTotalPages, nPageNumber,				\
											width, height, 										\
											nColorspace,	bUseAlpha,							\
											rgbBackground)											\
	((AT_ERRCOUNT)IG_ext_call_function(	IG_EXT_NAME,									\
													IG_EXT_FLASHPIX,								\
													IG_EXT_FPX_VIEW_SETTINGS_SET,				\
													(HIGEAR)hIGear,								\
													(LPUINT)nTotalPages,							\
													(LPUINT)nPageNumber,							\
													(LPAT_DIMENSION)width,						\
													(LPAT_DIMENSION)height, 					\
													(LPAT_MODE)nColorspace,						\
													(LPBOOL)bUseAlpha, 							\
													(LPAT_RGBQUAD)rgbBackground))				\


/*****************************************************************************/
/*****************************************************************************/
/*
AT_ERRCODE    IG_FPX_info_item_check(
   HIGEAR				hIGear,        
	LPAT_ITEMADDRESS	lpItemAddress	
);
*/

#define IG_FPX_info_item_check(hIGear, lpItemAddress, lpSize)						\
	((AT_ERRCODE)IG_ext_call_function(IG_EXT_NAME,										\
												 IG_EXT_FLASHPIX,									\
												 IG_EXT_FPX_INFO_ITEM_CHECK,					\
												 (HIGEAR)hIGear,									\
												 (LPAT_ITEMADDRESS)lpItemAddress,			\
												 (LPDWORD)lpSize))

/*
AT_ERRCOUNT   IG_FPX_info_item_get(
   HIGEAR			   hIGear,			
	LPAT_ITEMADDRESS	lpItemAddress,	
   LPVOID				lpBuffer,      
   DWORD					dwBufSize      
   );
*/

#define IG_FPX_info_item_get(hIGear, lpItemAddress, lpBuffer, dwBufSize)									\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_INFO_ITEM_GET,					\
												  (HIGEAR)hIGear,									\
												  (LPAT_ITEMADDRESS)lpItemAddress,			\
												  (LPVOID)lpBuffer,								\
												  (DWORD)dwBufSize))

/*
AT_ERRCOUNT   IG_FPX_info_item_set(
   HIGEAR			   hIGear,			
	LPAT_ITEMADDRESS	lpItemAddress,	
   LPVOID				lpBuffer,       
   DWORD					dwBufSize      
   );
*/

#define IG_FPX_info_item_set(hIGear,lpItemAddress,lpBuffer, dwBufSize)			\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_INFO_ITEM_SET,					\
												  (HIGEAR)hIGear,									\
												  (LPAT_ITEMADDRESS)lpItemAddress,			\
												  (LPVOID)lpBuffer,								\
												  (DWORD)dwBufSize))
/*
AT_ERRCOUNT   IG_FPX_info_array_length_get
   HIGEAR			   hIGear,				
	LPAT_ITEMADDRESS	lpItemAddress,		
   LPDWORD    lpLength						 
   );
*/

#define IG_FPX_info_array_length_get(hIGear, lpItemAddress,	lpLength)									\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_INFO_ARRAY_LENGTH_GET,			\
												  (HIGEAR)hIGear,									\
												  (LPAT_ITEMADDRESS)lpItemAddress,			\
												  (LPDWORD)lpLength))

/*
AT_ERRCOUNT   IG_FPX_info_array_length_set
   HIGEAR			   hIGear,				
	LPAT_ITEMADDRESS	lpItemAddress,		
   DWORD					dwLength						 
   );
*/

#define IG_FPX_info_array_length_set(hIGear, lpItemAddress,	lpLength)									\
	((AT_ERRCOUNT)IG_ext_call_function(IG_EXT_NAME,										\
												  IG_EXT_FLASHPIX,								\
												  IG_EXT_FPX_INFO_ARRAY_LENGTH_SET,			\
												  (HIGEAR)hIGear,									\
												  (LPAT_ITEMADDRESS)lpItemAddress,			\
												  (LPDWORD)lpLength))

/* #ifndef AM_EXTENSION */
#endif

#endif /* AM_EXTENSION_PROGOLD */

#ifdef __cplusplus
}
#endif


/* #ifndef __FPXAPI_H__ */
#endif


