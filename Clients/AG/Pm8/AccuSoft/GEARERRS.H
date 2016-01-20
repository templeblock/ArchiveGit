/***************************************************************************/
/*                                                                         */
/* MODULE:    GearErrs.h -  AccuSoft ERRor code definition file            */
/*                                                                         */
/*                                                                         */
/* Date created:        01/19/1996 DF                                      */
/*                                                                         */
/*    $Date:: 3/03/99 6:00p  $                                             */
/*    $Revision:: 1    $                                                   */
/*                                                                         */
/* Copyright (c) 1996-97, AccuSoft Corporation.  All rights reserved.      */
/*                                                                         */
/***************************************************************************/


#ifndef __GEARERRS_H__
#define __GEARERRS_H__


/***************************************************************************/
/* For now add error codes here and give them any value you want           */
/* Also, please add an English descriptive sentence as a comment           */
/* following the error code - these will be placed in an RC file           */
/***************************************************************************/


/***************************************************************************/
/* Misc. error codes                                                       */
/***************************************************************************/
#define  IGE_ALPHA_NOT_PRESENT				-2600

#define  IGE_SUCCESS                                0 /* No errors -- Success          */
#define  IGE_FAILURE                               -1 /* General error - Failure       */
#define  IGE_NOT_DONE_YET                        -100 /* For internal reference of areas to return to                                                       */
#define  IGE_NOT_IMPLEMENTED                     -200 /* For internal reference of areas to return to                                                       */
#define	IGE_PRO_GOLD_FEATURE							 -300	/* An attempt was made to use a Pro Gold feature in the standard product. */
#define	IGE_NOT_LITE_FEATURE							 -301	/* An attempt was made to use a Non IG-Lite feature in the IG-Lite product. */
#define	IGE_NOT_SUPPORTED_BY_PLATFORM				 -350	/* The last function used is not supported in 16-bit pro gold products	  */
#define  IGE_ERROR_COMPRESSION                   -400
#define	IGE_EXTENSION_NOT_LOADED					 -500	/* the ImageGear extension was not present or couldn't be loaded			  */
#define	IGE_INVALID_CONTROL_OPTION					 -600	/* invalid image control option ID													  */
#define	IGE_INVALID_EXTENSION_MODULE				 -700	/* the specified ImageGear extension file was not a valid extension file  */
#define	IGE_EXTENSION_INITIALIZATION_FAILED		 -800	/* the specified ImageGear extension was unable to initialize.				  */
#define	IGE_FUNCTIONALITY_NOT_SUPPORTED			 -900	/* the ImageGear functionality is not supported under this platform.		  */
#define  IGE_OUT_OF_MEMORY                      -1000 /* No more global memory is available for allocation, reduced used resources                          */
#define  IGE_EVAL_DLL_TIMEOUT_HAS_EXPIRED       -1003 /* The DLL is an Evaluation copy and as timed out - contact ACCUSOFT to purchase a release copy       */
#define  IGE_INVALID_STANDARD_KERNEL            -1004 /* The kernel expected should be one of the predefined ones, your could not be found                  */
#define  IGE_INTERNAL_ERROR                     -1005 /* An internal error has occured, contact ACCUSOFT techical support                                   */
#define  IGE_INVALID_RECTANGLE                  -1007 /* Occurs when a rectangle's left >= right or top >= bottom                                           */
#define	IGE_NO_CLIPBOARD_IMAGE_AVAILABLE			-1008 /* No image is available for a clipboard paste																			*/
#define	IGE_CLIPBOARD_OPEN_FAILED					-1009 /* Could not open the clipboard																								*/
#define  IGE_SETCLIPBOARDDATA_FAILED				-1010 /* Could not put data into the clipboard																					*/
#define  IGE_COULD_NOT_GET_DDB_DIMENSIONS			-1011 /* GetObject() failed.  Couldn't get the DDB's dimensions															*/
#define	IGE_COULD_NOT_GET_DDB_BITS					-1012 /* GetDIBits() failed.  Couldn't get the DDB's image data															*/
#define	IGE_CREATE_BITMAP_FAILED					-1013 /* CreateBitmap() failed.  Couldn't create a DDB.																		*/
#define	IGE_COULD_DISPLAY_DDB						-1014 /* BitBlt()	failed.  Couldn't display the DDB																			*/
#define	IGE_INVALID_PATTERN_BITMAP					-1015 /* The DDB was > 1 bit per pixel or the width was > 8 or the height was > 8									*/
#define	IGE_PASSWORD_INVALID							-1016 /* The Password is not recognized																							*/	
#define  IGE_THUMBNAIL_NOT_PRESENT              -2000 /* Thumnails are supported but non can be found in this image file                                    */
#define  IGE_THUMBNAIL_READ_ERROR               -2001 /* A read error occured while reading a thumbnail                                                     */
#define  IGE_THUMBNAIL_NOT_SUPPORTED            -2002 /* Thumbnails are not supported by this format                                                        */
#define  IGE_PAGE_NOT_PRESENT                   -2005 /* The requested image page does not exit in the file                                                 */
#define  IGE_PAGE_INVALID                       -2006 /* The page number provided is outside of the range of valid pages for this file                      */
#define  IGE_PAGE_COULD_NOT_BE_READ             -2007 /* The page number could not be determined                                                            */
#define  IGE_CANT_DETECT_FORMAT                 -2010 /* The format of the file can not be determined                                                       */
#define  IGE_FILE_CANT_BE_OPENED                -2030 /* An attempt to open a file failed, it may not exist in the provided path                            */
#define  IGE_FILE_CANT_BE_CREATED					-2031 /* An attempt to create a file failed, it may already exist in the provided path                      */
#define  IGE_FILE_CANT_BE_CLOSED						-2032 /* An attempt to close a file failed	                      														*/
#define  IGE_FILE_TO_SMALL_TO_BE_BMFH           -2033 /* The file is too small to be a valid BMFH                                                           */
#define  IGE_FILE_IS_NOT_BMP                    -2034 /* The BMFH Magic number is invalid                                                                   */
#define  IGE_FILE_TO_SMALL_TO_BE_BMIH           -2035 /* The file is too small to be a valid BMIH                                                           */
#define  IGE_BMP_IS_COMPRESSED                  -2040 /* The BMP file is in compressed (RLE) format                                                         */
#define  IGE_FILE_SIZE_WRITE_ERROR					-2041	/* Could not write file size field to BMP																					*/
#define  IGE_CANT_READ_PALETTE                  -2050
#define  IGE_CANT_READ_PIXELS                   -2051
#define  IGE_CANT_READ_HEADER                   -2052
#define  IGE_INVALID_FILE_TYPE                  -2060
#define  IGE_INVALID_HEADER                     -2061
#define  IGE_CANT_WRITE_PALETTE                 -2070
#define  IGE_CANT_WRITE_PIXELS                  -2071
#define  IGE_CANT_WRITE_HEADER                  -2072
#define  IGE_FORMAT_NOT_DETECTABLE					-2073
#define  IGE_INVALID_COMPRESSION                -2080
#define  IGE_INSTANCE_FAILURE                   -2090
#define  IGE_CANT_READ_FILE                     -2100
#define  IGE_INVALID_IMAGE_FORMAT               -2110 /* The image file is invalid as the expected format      															*/
#define  IGE_FILE_FORMAT_IS_READONLY            -2111 /* The image file is read only and can not be written to 															*/
#define  IGE_INVALID_BITCOUNT_FOR_FORMAT        -2112 /* The bitcount found is not supported by this format    															*/
#define  IGE_INTERRUPTED_BY_USER						-2113 /* Status bar callback returned FALSE																						*/
#define	IGE_NO_BITMAP_REGION                   -2390
#define  IGE_BAD_FILE_FORMAT                    -2391 /* Format is not correct.  																									*/
#define	IGE_EPS_NO_PREVIEW							-2392 /* EPS file has no screen preview image to load 																		*/
#define	IGE_CANT_WRITE_FILE							-2393
#define	IGE_NO_BITMAP_FOUND							-2394	/* WPG, WMF etc.  No raster image exists in file																		*/
#define	IGE_PALETTE_FILE_TYPE_INVALID				-2395	/* IG_PALETTE_ value is not known																							*/
#define	IGE_PALETTE_FILE_WRITE_ERROR				-2396	/* Error writing to a palette file																							*/
#define	IGE_PALETTE_FILE_READ_ERROR				-2397	/* Error reading from a palette file																						*/
#define	IGE_PALETTE_FILE_NOT_DETECTED				-2398	/* The file is not a valid palette file																					*/
#define	IGE_PALETTE_FILE_INVALID_HALO_PAL		-2399	/* Detected Dr. Halo Palette file is invalid																				*/
#define	IGE_G4_PREMATURE_EOF_AT_SCAN_LINE      -2400
#define	IGE_G4_PREMATURE_EOL_AT_SCAN_LINE      -2401
#define	IGE_G4_BAD_2D_CODE_AT_SCAN_LINE        -2402
#define	IGE_G4_BAD_DECODING_STATE_AT_SCAN_LINE	-2403
#define	IGE_G3_PREMATURE_EOF_AT_SCAN_LINE		-2410
#define	IGE_G3_BAD_1D_CODE_AT_SCAN_LINE			-2411						                      
#define	IGE_G3_PREMATURE_EOL_AT_SCAN_LINE		-2412
#define	IGE_BITDEPTH_NOTSUPPORTED					-2413
#define	IGE_DIRECTORY_CREATE_ERROR					-2414
#define	IGE_LOG_FILE_CREATE_ERROR					-2415
#define	IGE_NAME_CONV_NOT_SUPPORTED				-2416

#define	IGE_PJPEG_INVALID_SCAN_CONFIGURATION	-2420
#define	IGE_PJPEG_INVALID_SCAN_COUNT				-2421
#define	IGE_JPG_UNRECOGNIZED							-2422	/**/
#define	IGE_JPG_INVALID_QTABLE_ID					-2423
#define	IGE_JPG_INVALID_QTABLE_PRECISION			-2424
#define	IGE_JPG_INVALID_HUFFMAN_ID					-2425
#define	IGE_JPG_INVALID_HUFFMAN_TABLE				-2426
#define	IGE_PJPEG_NOT_SUPPORTED						-2427

#define	IGE_OPERATION_IS_NOT_ALLOWED				-2432
#define	IGE_PROC_INVAL_FOR_RUNS_DIB 				-2433 /* This function can not be used on DIBs in the Runs format - convert first IG_IP_convert_runs_to_DIB	*/
#define	IGE_CAN_NOT_OPEN_TEMP_FILE 				-2434 /* The temporary file need for this function could not be opened/created	*/
#define	IGE_ALLOC_SELECTOR_FAILED					-2435 /* AllocSelector() failed, couldn't get an entry into the Global Descriptor Table */
#define	IGE_LOAD_FUNCTION_GET_FAILED				-2436 /* Was not able to intialize the filer load function */
#define	IGE_STANDARD_FEATURE							-2437	/* This feature is not available in Lite version */
#define  IGE_PNG_CHUNK_WRITE_FAILED					-2438 /* failed to write the correct number of bytes for png chunk*/
#define  IGE_PNG_WRITE_FAILED							-2439 /* failed to write png data*/
#define  IGE_PNG_CHUNK_READ_FAILED					-2440 /* failed to READ the correct number of bytes for png chunk*/
#define  IGE_PNG_READ_FAILED							-2441 /* failed to READ png data*/
#define  IGE_PNG_NO_IDAT_CHUNK						-2442 /* failed to READ a manditory IDAT Chunk */

#define	IGE_NOT_SUPPORTED_COMP						-2443 /*Compression is not supported at this time		*/

#define	IGE_UNDEFNIED_COLOR_SPACE_ID				-2444	/* color space ID is not defined */
/***************************************************************************/
/* TIFF filter specific errors															*/
/***************************************************************************/

#define 	IGE_INVALID_TAG								-2450	/*Tag Read did not contain correct num of bytes	*/
#define 	IGE_INVALID_IFD								-2451 /*IFD Read did not contain correct num of bytes	*/
#define 	IGE_IFD_PROC_FAILURE							-2452 /*Invalid IFD information was detected				*/
#define 	IGE_SEEK_FAILURE								-2453	/*IOS position seek failed								*/
#define 	IGE_INVALID_BYTE_ORDER 						-2454 /*Byte order flag was not Intel or Motarola		*/
#define 	IGE_CANT_READ_TAG_DATA						-2455	/*Was unable to read all TAG information			*/
#define	IGE_INVALID_BITS_PER_SAMPLE  				-2456 /*Bits per sample tag was invalid					*/
#define	IGE_INVALID_COLOR_MAP						-2457	/*Color Map was found to be invalid					*/
#define	IGE_INVALID_PHOTOMETRIC						-2458 /*Photometric tag value was found to be invalid */
#define	IGE_INVALID_REQ_INFO							-2459 /*Required information was not supplied			*/
#define	IGE_COMP_NOT_SUPPORTED						-2460 /*Compression is not supported at this time		*/
#define	IGE_RASTER_FEED_ERROR    					-2461	/*Error feeding raster data to the DIB				*/
#define	IGE_IMAGE_DATA_READ_ERROR    				-2462	/*Was unable to read all image data requested	*/
#define	IGE_HEADER_WRITE_FAILED						-2463	/*Header write failed									*/
#define	IGE_DIB_GET_FAILURE							-2464	/*Was unable to retreive the DIB information		*/
#define	IGE_CANT_REALLOC_MEM							-2465	/*Was not able to reallocate memory					*/
#define	IGE_IFD_WRITE_ERROR							-2466	/*Was not able to write IFD info to the IOS		*/
#define	IGE_TAG_WRITE_ERROR							-2467	/*Was not able to write TAG info to the IOS		*/
#define	IGE_IMAGE_DATA_WRITE_ERROR					-2468	/*Was not able to write IMAGE data to the IOS	*/
#define	IGE_PLANAR_CONFIG_ERROR						-2469	/*Planar Config detected is unsupported			*/
#define	IGE_RASTER_TO_LONG							-2470	/*One raster lines exceeds the max num of bytes */
#define	IGE_LZW_ERROR									-2471	/*Error occured in LZW decode							*/
#define	IGE_INVALID_IMG_DEM							-2472	/*Image Dimension was invalid							*/
#define	IGE_BAD_DATA_TYPE								-2473	/*Data type detected is not valid					*/
#define	IGE_PAGE_COUNT_FAILURE						-2474	/*count not count the number of pages in the file*/
#define	IGE_CORRUPTED_FILE							-2475	/*data in file was not what was expected and could not be interp*/
#define	IGE_INVALID_STRIP_BYTE_CNT					-2476	/*strip byte count was zero and could not be estimated*/
#define	IGE_INVALID_COMP_BIT_DEPTH					-2477	/*bit depth is invalid for this compression scheme*/
#define  IGE_REPAGE_FAILED								-2478	/*unable to write new page numbers while repaging file */
#define  IGE_PRIV_TAG_TYPE_INVALID					-2479	/*private user tag had an invalid type				*/
#define	IGE_LZW_EXTENSION_NOT_LOADED				-2480 /*LZW Extension has not been loaded					*/
#define	IGE_TILE_NOT_PRESENT							-2481 /*Tile is not present									*/
#define	IGE_RASTER_WRITE_FAILURE					-2482 /*Unable to write Raster to Output Device (Full Device)*/
#define	IGE_IMBEDDED_IMAGE_FAILURE					-2483 /*Failure occured while reading a file format imbedded in another*/

#define	IGE_ABIC_EXTENSION_NOT_LOADED				-2484 /*ABIC Extension has not been loaded					*/
#define	IGE_JBIG_EXTENSION_NOT_LOADED				-2485 /*jbig Extension has not been loaded					*/
#define	IGE_JBG_IMG_CNTRL_NOT_FOUND				-2486 /*jbig Extension Image Control not found for save	*/

#define	IGE_CLP_INVALID_FORMAT_ID					-2500	/* Windows clipboard file contains an unsupport Format ID at this page											*/
#define	IGE_ICA_COMP_NOT_SUPPORTED					-2510	/* MO:DCA/IOCA Compression is not supported at this time	*/
#define	IGE_ICA_IBM_MMR_COMP_ERROR					-2520	/* Error in IBM MMR IOCA/MO:DCA compression					*/
#define	IGE_TIF_INVALID_CLASS_F_IMAGE				-2530	/* error writing TIF class F format */

#define IGE_JBIG_STREAM_OPEN_FAILURE				-2540	/* JBIG support library returned an error in return code*/

#define	IGE_CANNT_OPEN_FTP_FILE						-2550	
#define	IGE_CANNT_OPEN_HTTP_FILE					-2560
#define	IGE_CANNT_OPEN_GOPHER_FILE					-2570
#define	IGE_CANNT_OPEN_TEMPORARY_FILE				-2580
#define	IGE_CANNT_OPEN_INTERNET_CONNECTION		-2590
#define	IGE_CANNT_OPEN_INTERNET_SESSION			-2600

/* OCX error codes */
#define IGE_OCX_CANT_DELETE_PAGE						-2700
#define IGE_OCX_CANT_LOADMEM							-2701
#define IGE_OCX_CANT_GET_FILETYPE					-2702
#define IGE_OCX_CANT_GET_COMPRESS_TYPE				-2703
#define IGE_OCX_CANT_SET_IMAGE_INDEX				-2704
#define IGE_OCX_COLOR_SEPARATE_FAILED				-2705
#define IGE_OCX_DOC_NODE_IS_INVALID					-2706
#define IGE_OCX_CAN_GET_PAGE_COUNT					-2707
#define IGE_OCX_CANT_LOAD_DOC							-2708
#define IGE_OCX_COLOR_COMBINE_FAILED				-2709

/***************************************************************************/
/* Image Processing Error codes                                            */
/***************************************************************************/

#define  IGE_WRONG_DIB_BIT_COUNT                -3000 /* The DIB has bit with the wrong bit count for this routine                                          */
#define  IGE_LOCK_FAILED                        -3010 /* Memory required for this routine could not be locked, most likly running out of memory resources   */
#define  IGE_ALLOC_FAILED                       -3020 /* Memory required for this routine could not be allocated, free up some resources and try again      */
#define  IGE_FREE_FAILED                        -3030 /* An internal memory free has failed, ussally a bad handle, or corrupted system                      */
#define  IGE_BAD_KERN_TYPE                      -3040 /**/
#define  IGE_AI_HANDLES_USED_UP                 -3050 /* The maximum number od AccuSoft handles has been used - no more left.  Free up some and try again   */
#define  IGE_AI_HANDLE_INVALID                  -3060 /* This routine requires an AccuSoft handle.  The handle passed in was not allocated by AccuSoft      */
#define  IGE_DIBS_ARE_INCOMPATIBLE              -3070 /* The images are not compatible for this function, either dimension, bit count, or both              */
#define  IGE_DIB_DIMENSIONS_NOT_EQUAL           -3090 /* The images must be the same dimensions                                                             */
#define  IGE_DIB_BIT_COUNTS_NOT_EQUAL           -3100 /* The images must have the same bit count                                                            */
#define  IGE_DIB_HAS_NO_PALETTE                 -3101 /* The image passed in does not have a palette and this routine requires one                          */
#define  IGE_ROI_WRONG_TYPE                     -3110 /**/
#define  IGE_REQUIRES_CONVEX_ROI                -3120 /* This function requires a convex ROI. The one passed in must be convex                              */
#define  IGE_INVALID_RAMP_DIRECTION             -3130 /* The contrast ramps direction is not supported                                                      */
#define  IGE_INVALID_LUT_ARITH_FUNC             -3140 /* The LUT_ARITH_FUNC is not a valid function number, check the constant                              */
#define  IGE_INVALID_KERN_MOTION_EXTENT         -3150 /**/
#define  IGE_INVALID_NOISE_TYPE                 -3160 /**/
#define  IGE_INVALID_KERN_NORMALIZER            -3170 /**/
#define  IGE_INVALID_SIGMA                      -3180 /**/
#define  IGE_INVALID_SKEW_POINTS                -3190 /* A valid line could not be drawn through the two point provided.  Y1==Y2                            */
#define  IGE_TILE_IS_LARGER_THAN_IMAGE          -3200 /* The tile image must be the same size or smaller in both dimensions than the original source        */
#define  IGE_COLOR_SPACE_INVALID                -3210 /* Invalid type of color space MODE                                                                   */
#define  IGE_DIB_POINTER_IS_NULL                -3220 /* The DIB pointer about to be used is NULL (invalid)                                                 */
#define  IGE_PROC_INVAL_FOR_BIT_COUNT           -3300 /* This function can not be used on images with this one's bit count                                  */
#define  IGE_PROC_INVAL_FOR_PALETTE_IMG         -3310 /* This function can not be used on 8-bit color images - try to promote to 24-bit                     */
#define  IGE_PARAMETER_OUT_OF_LIMITS            -3320 /* A parameter is out of its legal range                                                              */
#define  IGE_INVALID_POINTER                    -3330 /* A pointer was detected to be NULL                                                                  */
#define  IGE_INVALID_ENCRYPT_MODE					-3340	/* The selected Encryption Method is invalid																				*/
#define  IGE_PASSWORD_LENGTH_INVALID				-3350	/* Password must be at least 1-byte long (should be at least 5 bytes)											*/
#define  IGE_PROC_REQUIRE_8BIT_GRAYSCALE			-3360	/* This functionn can be used on 8-bit grayscale images only														*/
#define	IGE_INVALID_RESOLUTION_UNIT				-3370 /* The units of the image resolution are not supported																*/
#define	IGE_POINTER_IS_NULL							-3380	/* Pointer passed to an IP function is NULL, but it would point at object										*/
#define	IGE_INVALID_BIT_MASK							-3390	/* The red, green and blue components of the bit mask overlap.														*/
#define	IGE_DIB_DIMENSIONS_ARE_INVALID			-3400	/* Either height or width of the DIB is wrong.																			*/
#define	IGE_PROC_INVAL_FOR_8BIT_INDEXED			-3410 /* Proc does not work on the 8i images	*/
#define	IGE_RASTER_LINE_INVALID						-3420 /* The given raster line is invalid, it should be between 0 and the height of the image -1 */


/***************************************************************************/
/* Disk File Access Error codes                                            */
/***************************************************************************/

#define	IGE_FILE_CANT_OPEN                     -3440	/* Cannot open file   */
#define	IGE_FILE_CANT_SAVE                     -3441	/* Cannot save file   */
#define	IGE_FILE_CANT_DELETE                   -3442	/* Cannot delete file */
#define	IGE_FILE_INVALID_FILENAME              -3443	/* Invalid file name  */
#define	IGE_FILE_INVALID_PATH		            -3450	/* Invalid path		 */
             

/***************************************************************************/
/* Auto Detect Error codes                                                	*/
/***************************************************************************/

#define	IGE_FILE_IS_SYSTEM_FILE						-3600	/* The image file passed in is really one of the following system files and not an image	*/
#define	IGE_FILE_IS_EXE								-3601	/* File is a EXE, DLL, DRV, FNT, OCX, or 386		*/
#define	IGE_FILE_IS_ZIP								-3602	/* File is a PKZIP file							*/
#define	IGE_FILE_IS_DOC								-3603	/* File is a Microsoft DOC file				*/
#define	IGE_FILE_IS_HLP								-3604	/* File is a Microsoft sytem Help file		*/

/***************************************************************************/
/* TWAIN Scanning Function Error codes                                     */
/***************************************************************************/

#define	IGE_TW_SM_SUCCESS                      -4000	/* Operation worked                                                            */
#define	IGE_TW_SM_BUMMER                       -4001	/* General failure due to unknown causes                                       */
#define	IGE_TW_SM_LOWMEMORY                    -4002	/* Not enough memory to complete operation                                     */
#define	IGE_TW_SM_NODS                         -4003	/* Source Manager unable to find the specified Source                          */
#define	IGE_TW_SM_MAXCONNECTIONS               -4004	/* Source is connected to maximum number of applications                       */
#define	IGE_TW_SM_OPERATIONERROR               -4005	/* Source or Source Manager reported an error to the user                      */
#define	IGE_TW_SM_BADCAP                       -4006	/* Capability not supported by Source or operation not supported by capability */	
#define	IGE_TW_SM_BADPROTOCOL                  -4007	/* Unrecognized operation triplet                                              */
#define	IGE_TW_SM_BADVALUE                     -4008	/* Data parameter out of supported range                                       */
#define	IGE_TW_SM_SEQERROR                     -4009	/* Illegal operation for current Source Manager or Source state                */
#define	IGE_TW_SM_BADDEST                      -4010	/* Unknown destination in DSM_ENTRY                                            */
#define	IGE_TW_CANT_OPENDSM                    -4011	/* Cannot load Source Manager                                                  */
#define  IGE_TW_CANT_OPENDS                     -4012 /* Cannot load data Source                                                     */
#define	IGE_TW_CANT_ENABLEDS                   -4013	/* Cannot enable data Source                                                   */
#define	IGE_TW_CANT_FINDDSM                    -4014	/* Cannot find Source Manager                                                  */
#define	IGE_TW_CANT_LOADDSM                    -4015	/* Cannot load Source Manager to memory                                        */
#define	IGE_TW_CANT_SCAN_PAGES                 -4016	/* Cannot scan pages                                                           */
#define	IGE_TW_CANT_TRANSFERIMAGE              -4017	/* Cannot transfer image to application                                        */
#define	IGE_TW_CANT_GETDSMADDR                 -4018	/* Cannot get address of the Source Manager                                    */
#define	IGE_TW_CANT_PROCESSMSG                 -4019	/* Cannot process TWAIN message                                                */
#define	IGE_TW_INVALID_DIBHANDLE               -4020	/* Invalid DIB handle                                                          */
#define	IGE_TW_CANT_SET_ICAP_PIX_FLAVOR			-4021	/* Cannot set Pixel Flavor																		 */
#define	IGE_TW_CANT_SET_ICAP_PIXELTYPE			-4022
#define	IGE_TW_CANT_SET_ICAP_BITDEPTH				-4023
#define	IGE_TW_CANT_SET_ICAP_UNITS					-4024
#define	IGE_TW_CANT_SET_ICAP_XRESOLUTION			-4025
#define	IGE_TW_CANT_SET_ICAP_YRESOLUTION			-4026
#define	IGE_TW_CANT_SET_IMAGELAYOUT				-4027
#define	IGE_TW_CANT_SET_ICAP_BRIGHTNESS			-4028
#define	IGE_TW_CANT_SET_ICAP_CONTRAST				-4029
#define  IGE_TW_CANT_SET_SCAN_PARAMETETRS			-4030
#define	IGE_TW_CANT_OPEN_DEF_SRC					-4031
#define	IGE_TW_USER_CANCEL_SEL_SRC					-4032
#define	IGE_TW_IMAGE_TRANSFER_IS_CANCELED		-4033
#define	IGE_TW_CAN_NOT_GET_IMAGE_INFO				-4034
#define	IGE_TW_WHERE_IS_NO_TWAIN_DLL				-4035
#define	IGE_TW_CAN_NOT_ENABLE_ADF					-4041
#define	IGE_TW_CAN_NOT_DISABLE_ADF					-4042
#define	IGE_TW_SCANNING_IS_CANCELED				-4043

#define	IGE_TW_ERROR_GET_FILE_VERSION				-4044

#define	IGE_TW_BAD_VALUE_FOR_BITS_PER_PIX		-4045
#define	IGE_TW_CANT_CLOSE_DSM						-4046
#define	IGE_TW_CANT_OPEN_DS							-4047
#define	IGE_TW_CANT_CLOSE_DS							-4048
#define	IGE_TW_CANT_ENABLE_DS						-4049
#define	IGE_TW_CANT_DISABLE_DS						-4050
#define	IGE_TW_WRONG_CONT_TYPE						-4051

/* begin IG7: added for the TWAIN non native transfer modes,
	more TWAIN caps set */
#define	IGE_TW_TRANSFER_MODE_REQUEST_FAILED			-4100
#define	IGE_TW_DS_DOES_NOT_SUPPORT_TRANSFER_MODE	-4101
#define	IGE_TW_FILE_FORMATS_REQUEST_FAILED			-4102
#define	IGE_TW_DS_DOESN_SUPPORT_FILE_FORMAT			-4103
#define	IGE_TW_CAN_NOT_SET_TRANSFER_PARAMETERS		-4104
#define	IGE_TW_CANT_SET_TRANSFER_MODE					-4105
#define	IGE_TW_DISK_FILE_TRANSFER_FAILED				-4106
#define	IGE_TW_CAN_NOT_GET_TRANSFER_PARAMETERS		-4107
#define	IGE_TW_INVALID_IMAGE_PARAMETERS				-4108
#define	IGE_TW_COMPRESSION_IS_NOT_SUPPORTED			-4109
#define	IGE_TW_INVALID_TRANSFER_MODE					-4110

#define	IGE_TW_IMAGE_TRANSFER_FAILED					-4120

#define	IGE_TW_CANT_ALLOC_CONTAINER					-4130
#define	IGE_TW_CANT_SET_ICAP_COMPRESSION          -4131
#define	IGE_TW_CANT_SET_ICAP_AUTOBRIGHT           -4132
#define	IGE_TW_CANT_SET_ICAP_GAMMA                -4133
#define	IGE_TW_WRONG_ICAP_ROTATION_VALUE          -4134
#define	IGE_TW_CANT_SET_ICAP_ROTATION             -4135
#define	IGE_TW_WRONG_ICAP_HIGHLIGHT_VALUE         -4136
#define	IGE_TW_CANT_SET_ICAP_HIGHLIGHT            -4137
#define	IGE_TW_WRONG_ICAP_SHADOW_VALUE            -4138
#define	IGE_TW_CANT_SET_ICAP_SHADOW               -4139
#define	IGE_TW_WRONG_CAP_FO_VALUE                 -4140
#define	IGE_TW_CANT_SET_ICAP_FO                   -4141
#define	IGE_TW_WRONG_CAP_KFACTOR_VALUE            -4142
#define	IGE_TW_CANT_SET_ICAP_KFACTOR              -4143
#define	IGE_TW_WRONG_CAP_PMI_VALUE                -4144
#define	IGE_TW_CANT_SET_ICAP_PMI                  -4145
#define	IGE_TW_WRONG_ICAP_XSCALING_VALUE				-4146
#define	IGE_TW_CANT_SET_ICAP_XSCALING					-4147
#define	IGE_TW_WRONG_ICAP_YSCALING_VALUE				-4148
#define	IGE_TW_CANT_SET_ICAP_YSCALING					-4149
#define	IGE_TW_CANT_SET_ICAP_TILES						-4150
#define	IGE_TW_CANT_SET_ICAP_ORIENTATION				-4151
#define	IGE_TW_CANT_SET_ICAP_JPEGPIXELTYPE			-4152

/* DS enum error codes */
#define	IGE_TW_CANT_FIND_DS_SPECIFIED					-4200
#define	IGE_TW_DS_ENUMARATE_FAILED						-4201
#define	IGE_TW_DS_INFO_IS_NOT_AVAIL					-4202
#define	IGE_TW_CANT_GET_DEFAULT_DS						-4203

/* query caps */
#define	IGE_CANT_LOCK_CONTAINER							-4210
#define	IGE_TW_UNSUPPORTED_CONT_TYPE              -4211
#define	IGE_TW_CAP_QUERY_FAILED                   -4212
#define	IGE_TW_CAP_ID_NOT_FOUND							-4213
#define	IGE_TW_CANT_GET_CAP_STORAGE					-4214
#define	IGE_TW_WRONG_CAP_INTERNAL_TYPE				-4215
#define	IGE_TW_NO_MORE_CAP_VALUES						-4216

#define	IGE_TW_NO_NEXT_CAP_VALUE_INFO					-4217
#define	IGE_TW_NO_DFLT_CAP_VALUE_INFO					-4218
#define	IGE_TW_NO_CURR_CAP_VALUE_INFO					-4219
#define	IGE_TW_CANT_QUERY_CAP							-4230
#define	IGE_TW_CANT_SET_FEEDER							-4231

#define	IGE_TW_WRONG_CAP_TYPE							-4240
#define	IGE_TW_WROND_INDEX								-4241
#define	IGE_TW_CANT_GET_CURRENT_VALUE					-4242

#define	IGE_TW_PIX_TYPE_MISMATCH						-4243
#define	IGE_TW_CANT_GET_PALETTE							-4244
#define	IGE_PALETTE_TYPE_NOT_SUPPORTED				-4245
#define	IGE_TW_PALETTE_SIZE_MISMATCH					-4246
#define	IGE_TW_PIXEL_TYPE_UNSUPPORTED					-4247

/* TWAIN CB */
#define	IGE_TW_WRONG_CB_TYPE								-4300
#define	IGE_TW_CAN_NOT_GET_IMAGE_LAYOUT				-4301
#define	IGE_TW_TRANSFER_CB_CANCELED					-4302

/* end IG7: ... */


/***************************************************************************/
/* GUI Function Error codes                                                */
/***************************************************************************/

#define  IGE_REGISTER_CLASS_FAILED              -6000 /* Microsoft Windows function:  RegisterClass() failed. */
#define  IGE_CREATE_WINDOW_FAILED               -6010 /* Microsoft Windows function:  CreateWindow() failed.  */
#define  IGE_WINDOW_NOT_ASSOCATED               -6020 /* An attempt was made to disassociate a window that was never assocated.  */
#define	IGE_INVALID_WINDOW							-6030	/* An invalid window handle was passed as one of the parameters to the function. */



/***************************************************************************/
/* NRA error codes                                               */
/***************************************************************************/
#ifdef IMAGEGEAR_7_0

#define INVALID_REGION_DATA										-9000
#define INVALID_PARAMETER_FOR_PROCESSING_WITH_REGION		-9001 

#endif

/***************************************************************************/
/* VBX/OCX level error codes                                               */
/***************************************************************************/

#define  IGE_VBX_INVALID_FUNCTION_NUM           -7000 /* Invalid VBX function number 							*/
#define  IGE_ROTATE_ENUMERATED_VALUES_NOT_USED  -7010 /* The enumerated values for rotate were not used	*/
#define  IGE_GUIWINDOW_TYPE_INVALID					-7020 /* GUIWindow type invalid 									*/


/***************************************************************************/
/* OS2 error codes																			*/
/***************************************************************************/

#define  IGE_UNREGISTERED_HAB							-8000 /* HAB is not registered */



/***************************************************************************/
/* Application error codes must be less that the following last number used*/
/***************************************************************************/

#define  IGE_LAST_ERROR_NUMBER                  -9999 




/***************************************************************************/
/* Extension Error Code Reserved Ranges												*/
/*																									*/
/*	These error code ranges are reserved for use by an extension and must	*/
/* not be used for ImageGear																*/
/***************************************************************************/

/* Medical Extension										-22xxx	*/
/* ART Extension											-24xxx	*/
/* FlashPix Extension									-26xxx	*/


/*#ifndef __GEARERRS_H__*/
#endif
