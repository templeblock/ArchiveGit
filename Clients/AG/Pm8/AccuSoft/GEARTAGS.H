/***************************************************************************/
/*                                                                         */
/* MODULE:  GearTags.h - Image Gear header callback TAG constants.         */
/*                                                                         */
/*                                                                         */
/* DATE CREATED:  04/19/1996                                               */ 
/*                                                                         */
/*    $Date:: 3/03/99 6:00p  $                                             */
/*    $Revision:: 1          $                                             */
/*                                                                         */
/* Copyright (c) 1996-97, AccuSoft Corporation.  All rights reserved.      */
/*                                                                         */
/***************************************************************************/

#ifndef __GEARTAGS_H__
#define __GEARTAGS_H__


/***************************************************************************/
/* Image Gear field tags for use with tag callbacks (uIGTag parameter.)    */
/***************************************************************************/
/*WARNING: Please do not use tag values greater than     */
/*    32768.  These tag values are reserved for private  */
/*    TIFF tags which will be defined by the custore     */
/*                                                       */
/*********************************************************/

#define IGTAG_BMP_SIZE                    100
#define IGTAG_BMP_WIDTH                   101
#define IGTAG_BMP_HEIGHT                  102
#define IGTAG_BMP_PLANES                  103
#define IGTAG_BMP_BITCOUNT                104
#define IGTAG_BMP_COMPRESSION             105
#define IGTAG_BMP_XPELSPERMETER           106
#define IGTAG_BMP_YPELSPERMETER           107
#define IGTAG_BMP_CLRUSED                 108
#define IGTAG_BMP_CLRIMPORTANT            109
#define IGTAG_BMP_UNITS                   110
#define IGTAG_BMP_RECORDING               111
#define IGTAG_BMP_RENDERING               112
#define IGTAG_BMP_SIZE1                   113
#define IGTAG_BMP_SIZE2                   114
#define IGTAG_BMP_COLORENCODING           115
#define IGTAG_BMP_IDENTIFIER              116
#define IGTAG_JPG_JFIF_ID                 200
#define IGTAG_JPG_JFIF_VERSION            201
#define IGTAG_JPG_JFIF_UNITS              202

#define IGTAG_JPG_JFIF_X_RESOLUTION       203
#define IGTAG_JPG_JFIF_Y_RESOLUTION       204
#define IGTAG_JPG_JFIF_THUMBNAIL_WIDTH    205
#define IGTAG_JPG_JFIF_THUMBNAIL_HEIGHT   206                     

#define IGTAG_JPG_COMMENT                 207

#define IGTAG_JPG_QUANT1                  208
#define IGTAG_JPG_QUANT2                  209
#define IGTAG_JPG_QUANT3                  210
#define IGTAG_JPG_QUANT4                  211
#define IGTAG_JPG_FRAME_PRECISION         212
#define IGTAG_JPG_FRAME_LINES             213
#define IGTAG_JPG_FRAME_SAMPLES_PER_LINE  214
#define IGTAG_JPG_FRAME_COMPONENTS        215


#define IGTAG_JPG_FRAME_COMPID1           216
#define IGTAG_JPG_FRAME_COMPID2           217
#define IGTAG_JPG_FRAME_COMPID3           218

#define IGTAG_JPG_FRAME_MCU_HV1           219
#define IGTAG_JPG_FRAME_MCU_HV2           220
#define IGTAG_JPG_FRAME_MCU_HV3           221

#define IGTAG_JPG_FRAME_QUANT1            225
#define IGTAG_JPG_FRAME_QUANT2            226
#define IGTAG_JPG_FRAME_QUANT3            227

#define IGTAG_JPG_SCAN_COMPONENTS         228
#define IGTAG_JPG_SCAN_COMP_SELECT1       229
#define IGTAG_JPG_SCAN_COMP_SELECT2       230
#define IGTAG_JPG_SCAN_COMP_SELECT3       231

#define IGTAG_JPG_SCAN_DC_AC1             232
#define IGTAG_JPG_SCAN_DC_AC2             233
#define IGTAG_JPG_SCAN_DC_AC3             234

#define IGTAG_JPG_SCAN_SPECT_START        235
#define IGTAG_JPG_SCAN_SPECT_END          236
#define IGTAG_JPG_SCAN_AH_AL              237

#define IGTAG_JPG_APPDATA                 238         
#define IGTAG_JPG_APPDATA_LAST				(IGTAG_JPG_APPDATA+14)

#define IGTAG_CAL_SPECVERSION             300
#define IGTAG_CAL_SRCDOCID                301
#define IGTAG_CAL_DSTDOCID                302
#define IGTAG_CAL_DATFILID                303
#define IGTAG_CAL_MODULEID                304
#define IGTAG_CAL_DTYPE                   305
#define IGTAG_CAL_RORIENT                 306
#define IGTAG_CAL_RPELCNT                 307
#define IGTAG_CAL_RDENSITY                308
#define IGTAG_CAL_DIDID                   309
#define IGTAG_CAL_DOCCLS                  310
#define IGTAG_CAL_FOSIPUBID               311
#define IGTAG_CAL_NOTES                   312
    
#define IGTAG_PCX_MANUFACTURER            400
#define IGTAG_PCX_VERSION_INFO            401
#define IGTAG_PCX_ENCODE                  402
#define IGTAG_PCX_BIT_PER_PLANE           403
#define IGTAG_PCX_X1                      404
#define IGTAG_PCX_Y1                      405
#define IGTAG_PCX_X2                      406
#define IGTAG_PCX_Y2                      407
#define IGTAG_PCX_H_RES                   408
#define IGTAG_PCX_V_RES                   409
#define IGTAG_PCX_PALETTE_TABLE           410
#define IGTAG_PCX_VIDEO_MODE              411
#define IGTAG_PCX_NUM_OF_PLANES           412
#define IGTAG_PCX_BYTES_PER_LINE          413
#define IGTAG_PCX_PALETTE_INFO            414
#define IGTAG_PCX_SCANNER_H_RES           415
#define IGTAG_PCX_SCANNER_V_RES           416
#define IGTAG_PCX_EXTRA                   417

#define IGTAG_DCX_MAGIC                   500
#define IGTAG_DCX_PAGE_LIST               501

#define IGTAG_GEM_VERSION                 600
#define IGTAG_GEM_HEADERSIZE              601
#define IGTAG_GEM_PLANES                  602
#define IGTAG_GEM_PATTERNLENGTH           603
#define IGTAG_GEM_WIDTH                   604
#define IGTAG_GEM_HEIGHT                  605
    
#define IGTAG_EPS_VERSION                 700
#define IGTAG_EPS_WIDTH                   701
#define IGTAG_EPS_HEIGHT                  702
#define IGTAG_EPS_TITLE                   703     
#define IGTAG_EPS_CREATOR                 704
#define IGTAG_EPS_BOUNDINGBOX             705
#define IGTAG_EPS_TRANSLATE               706 
#define IGTAG_EPS_SCALE                   707
#define IGTAG_EPS_IMAGE                   708

#define IGTAG_IFF_WIDE                    800
#define IGTAG_IFF_HIGH                    801
#define IGTAG_IFF_XORG                    802
#define IGTAG_IFF_YORG                    803
#define IGTAG_IFF_PLANES                  804
#define IGTAG_IFF_MASK                    805
#define IGTAG_IFF_COMPRESSION             806
#define IGTAG_IFF_TRAN_ASPT               807
#define IGTAG_IFF_PAGE_W                  808
#define IGTAG_IFF_PAGE_H                  809
#define IGTAG_IFF_VIEW_MODE               810

#define IGTAG_BTR_MANUFACTURER            900
#define IGTAG_BTR_VERSION                 901
#define IGTAG_BTR_IMAGETYPE               902
#define IGTAG_BTR_HORZRES                 903
#define IGTAG_BTR_VERTRES                 904
#define IGTAG_BTR_BITSPERPIXEL            905
#define IGTAG_BTR_PIXELSPERLINE           906
#define IGTAG_BTR_STORAGEFMT              907
#define IGTAG_BTR_TRANSFMT                908
#define IGTAG_BTR_PREVPAGE                909
#define IGTAG_BTR_NEXTPAGE                910
#define IGTAG_BTR_NUMLINES                911

#define IGTAG_IMT_TYPE                    1000
#define IGTAG_IMT_FMT                     1001
#define IGTAG_IMT_HEIGHT                  1002
#define IGTAG_IMT_WIDTH                   1003
#define IGTAG_IMT_RESOLUTION              1004
#define IGTAG_IMT_BITSWAP                 1005
#define IGTAG_IMT_SWAB                    1006
#define IGTAG_IMT_INVERT                  1007

#define IGTAG_LV_YORIGIN                  1100
#define IGTAG_LV_XORIGIN                  1101
#define IGTAG_LV_LINES                    1102
#define IGTAG_LV_PIXELS                   1103
#define IGTAG_LV_BITSPIX                  1104
#define IGTAG_LV_COMPRESSION              1105
#define IGTAG_LV_BYTEFORMAT               1106
#define IGTAG_LV_COMPVERSION              1107
#define IGTAG_LV_YAXIS                    1108
#define IGTAG_LV_XAXIS                    1109
#define IGTAG_LV_NBLOCKTYPE               1110
#define IGTAG_LV_DISPLAYMETHOD            1111
#define IGTAG_LV_XSEPERATION              1112
#define IGTAG_LV_YSEPERATION              1113
#define IGTAG_LV_BLOCKLENGTH              1114
#define IGTAG_LV_TEXT                     1115
                  
#define IGTAG_ICA_WIDTH                   1200
#define IGTAG_ICA_HEIGHT                  1201
#define IGTAG_ICA_DEPTH                   1202
#define IGTAG_ICA_XDPI                    1203
#define IGTAG_ICA_YDPI                    1204
#define IGTAG_ICA_BITORDER                1205
#define IGTAG_ICA_BASE                    1206
#define IGTAG_ICA_COMPRESSION             1207
#define IGTAG_ICA_FILLORDER               1208

#define IGTAG_RAS_MAGIC                   1300
#define IGTAG_RAS_WIDTH                   1301
#define IGTAG_RAS_HEIGHT                  1302
#define IGTAG_RAS_DEPTH                   1303
#define IGTAG_RAS_LENGTH                  1304
#define IGTAG_RAS_TYPE                    1305
#define IGTAG_RAS_COLOR_MAP_TYPE          1306
#define IGTAG_RAS_COLOR_MAP_LENGTH        1307
                  
#define IGTAG_SGI_MAGIC                   1400
#define IGTAG_SGI_STORAGE                 1401
#define IGTAG_SGI_BPC                     1402
#define IGTAG_SGI_DIMENSION               1403
#define IGTAG_SGI_X_SIZE                  1404
#define IGTAG_SGI_Y_SIZE                  1405
#define IGTAG_SGI_Z_SIZE                  1406
#define IGTAG_SGI_PIX_MIN                 1407
#define IGTAG_SGI_PIX_MAX                 1408
#define IGTAG_SGI_DUMMY1                  1409
#define IGTAG_SGI_IMAGE_NAME              1410
#define IGTAG_SGI_COLOR_MAP               1411
#define IGTAG_SGI_DUMMY2                  1412
                                 
#define IGTAG_GIF_SCREEN_ASPECT				1501
#define IGTAG_GIF_SCREEN_BG_COLOR			1502
#define IGTAG_GIF_SCREEN_FLAGS				1503
#define IGTAG_GIF_SCREEN_HEIGHT				1504
#define IGTAG_GIF_SCREEN_WIDTH				1505
#define IGTAG_GIF_SCREEN_PALETTE				1506
#define IGTAG_GIF_IMAGE_LEFT					1507
#define IGTAG_GIF_IMAGE_TOP					1508
#define IGTAG_GIF_IMAGE_FLAGS					1509
#define IGTAG_GIF_IMAGE_PALETTE				1510
#define IGTAG_GIF_EXT_NUMBER_BEFORE_IMG	1511
#define IGTAG_GIF_EXT_BEFORE_IMG				1512
#define IGTAG_GIF_EXT_NUMBER_AFTER_IMG		1513
#define IGTAG_GIF_EXT_AFTER_IMG				1514

#define IGTAG_WMF_FH_KEY                  1601
#define IGTAG_WMF_FH_HANDLE               1602
#define IGTAG_WMF_FH_LEFT                 1603
#define IGTAG_WMF_FH_TOP                  1604
#define IGTAG_WMF_FH_RIGHT                1605
#define IGTAG_WMF_FH_BOTTOM               1606
#define IGTAG_WMF_FH_INCH                 1607
#define IGTAG_WMF_FH_RESERVED             1608
#define IGTAG_WMF_MH_FILE_TYPE            1620
#define IGTAG_WMF_MH_HEADER_SIZE          1621
#define IGTAG_WMF_MH_VERSION              1622
#define IGTAG_WMF_MH_FILE_SIZE            1623
#define IGTAG_WMF_MH_NUM_OBJECTS          1624
#define IGTAG_WMF_MH_MAX_RECORD_SIZE      1625
#define IGTAG_WMF_MH_NO_PARAMETERS        1626

#define IGTAG_CLP_FILE_ID                 1701
#define IGTAG_CLP_FORMAT_COUNT            1702

#define IGTAG_MSP_KEY1                    1800
#define IGTAG_MSP_KEY2                    1801
#define IGTAG_MSP_WIDTH                   1802
#define IGTAG_MSP_HEIGHT                  1803
#define IGTAG_MSP_X_AR_BITMAP             1804
#define IGTAG_MSP_Y_AR_BITMAP             1805
#define IGTAG_MSP_X_AR_PRINTER            1806
#define IGTAG_MSP_Y_AR_PRINTER            1807
#define IGTAG_MSP_X_PRINTER_WIDTH         1808
#define IGTAG_MSP_Y_PRINTER_HEIGHT        1809
#define IGTAG_MSP_X_ASPECT_CORR           1800
#define IGTAG_MSP_Y_ASPECT_CORR           1810
#define IGTAG_MSP_CHECKSUM                1811
#define IGTAG_MSP_PADDING                 1812

#define IGTAG_KFX_ID                      1901
#define IGTAG_KFX_HDR_SIZE                1902
#define IGTAG_KFX_HDR_VER                 1903
#define IGTAG_KFX_IMAGE_ID                1904
#define IGTAG_KFX_WIDTH                   1905
#define IGTAG_KFX_LENGTH                  1906
#define IGTAG_KFX_FORMAT                  1907
#define IGTAG_KFX_BIT_SEX                 1908
#define IGTAG_KFX_COLOR                   1909
#define IGTAG_KFX_XRES                    1910
#define IGTAG_KFX_YRES                    1911
#define IGTAG_KFX_PLANES                  1912
#define IGTAG_KFX_BITS_PER_PIX            1913
#define IGTAG_KFX_PAPER_SIZE              1914
#define IGTAG_KFX_DATE_CRT                1915
#define IGTAG_KFX_DATE_MOD                1916
#define IGTAG_KFX_DATE_ACC                1917
#define IGTAG_KFX_IDX_OFFSET              1918
#define IGTAG_KFX_IDX_LEN                 1919
#define IGTAG_KFX_COM_OFFSET              1920
#define IGTAG_KFX_COM_LEN                 1921
#define IGTAG_KFX_USER_OFFSET             1922
#define IGTAG_KFX_USER_LEN                1923
#define IGTAG_KFX_DATA_OFFSET             1924
#define IGTAG_KFX_DATA_LEN                1925


#define IGTAG_NCR_DATA_FORMAT             2001
#define IGTAG_NCR_OPTIONS                 2002
#define IGTAG_NCR_ENCRYPTION              2003
#define IGTAG_NCR_AUTHENTICATION          2004
#define IGTAG_NCR_AUTH_MAC                2005
#define IGTAG_NCR_DATA_SIZE               2006
#define IGTAG_NCR_REAL_BPP                2007
#define IGTAG_NCR_STORE_BPP               2008
#define IGTAG_NCR_REAL_WIDTH              2009
#define IGTAG_NCR_STORE_WIDTH             2010
#define IGTAG_NCR_REAL_HEIGHT             2011
#define IGTAG_NCR_STORE_HEIGHT            2012
#define IGTAG_NCR_ORIENT                  2013
#define IGTAG_NCR_PMI                     2014
#define IGTAG_NCR_DATA_ENDIAN             2015
#define IGTAG_NCR_FILL_ORDER              2016
#define IGTAG_NCR_GRANULARITY             2017
#define IGTAG_NCR_MIN_PIX_VALUE           2018
#define IGTAG_NCR_MAX_PIX_VALUE           2019
#define IGTAG_NCR_X_RES                   2020
#define IGTAG_NCR_Y_RES                   2021
#define IGTAG_NCR_RES_UNIT                2022
#define IGTAG_NCR_ERROR                   2023

#define IGTAG_PSD_SIGNATURE               2100     
#define IGTAG_PSD_VERSION                 2101
#define IGTAG_PSD_ROWS                    2102
#define IGTAG_PSD_COLS                    2103
#define IGTAG_PSD_DEPTH                   2104
#define IGTAG_PSD_MODE                    2105
#define IGTAG_PSD_MODE_LEN                2106
#define IGTAG_PSD_COMPRESSION             2107

#define IGTAG_AFX_SIGNATURE               3000                 
#define IGTAG_AFX_VER_MAJOR               3001
#define IGTAG_AFX_VER_MINOR               3002
#define IGTAG_AFX_VER_REV                 3003
#define IGTAG_AFX_VER_DEV                 3004
#define IGTAG_AFX_HDR_SIZE                3005
#define IGTAG_AFX_CHK_SUM                 3006
#define IGTAG_AFX_TYPE                    3007
#define IGTAG_AFX_PRD_VER                 3008
#define IGTAG_AFX_ENC_METHOD              3009
#define IGTAG_AFX_COMMENT                 3010
#define IGTAG_AFX_DATA_START              3011
#define IGTAG_AFX_DATA_SIZE               3012
#define IGTAG_AFX_RES                     3013                 

#define IGTAG_PCD_IPICA_RESERVED                4000
#define IGTAG_PCD_IPICA_IMAGE_PACK_PARAMS       4001
#define IGTAG_PCD_IPICA_BASE4_STOP_OFFSET       4002
#define IGTAG_PCD_IPICA_BASE16_STOP_OFFSET      4003
#define IGTAG_PCD_IPICA_IPE_STOP_OFFSET         4004
#define IGTAG_PCD_IPICA_IP_INTERLEAVE_RATIO     4005

#define IGTAG_PCD_IPI_SIGNATURE                 4006
#define IGTAG_PCD_IPI_VERSION_NUMBER            4007
#define IGTAG_PCD_IPI_SOFTWARE_RELEASE          4008
#define IGTAG_PCD_IPI_IMAGE_MAG_DESCRIPTION     4009
#define IGTAG_PCD_IPI_IMAGE_SCAN_TIME           4010
#define IGTAG_PCD_IPI_LAST_MODIFICATION_DATE    4011
#define IGTAG_PCD_IPI_MED_ORIGINAL_RECORDING    4012
#define IGTAG_PCD_IPI_TYPE_ORIGINAL_RECORDING   4013
#define IGTAG_PCD_IPI_SCANNER_VENDOR            4014
#define IGTAG_PCD_IPI_SCANNER_PRODUCT           4015
#define IGTAG_PCD_IPI_SCANNER_FIRMWARE_LEVEL    4016
#define IGTAG_PCD_IPI_SCANNER_FIRMWARE_DATE     4017
#define IGTAG_PCD_IPI_SCANNER_SERIAL_NUMBER     4018
#define IGTAG_PCD_IPI_SCANNER_PIXEL_SIZE        4019
#define IGTAG_PCD_IPI_EQUIPMENT_MANUFACTURER    4020
#define IGTAG_PCD_IPI_PHOTONAME_CHAR_SET        4021
#define IGTAG_PCD_IPI_PHOTONAME_ESC_SEQ         4022
#define IGTAG_PCD_IPI_PHOTONAME                 4023
#define IGTAG_PCD_IPI_SBA_DATA                  4024
#define IGTAG_PCD_IPI_COPYRIGHT_STATUS          4025
#define IGTAG_PCD_IPI_COPYRIGHT_FILENAME        4026

/****************************************************************************
   POS   DESCRIPTION
   1     Tag is readable by Tiff Read Function.
   2     Does the user receive this tag via call back 
         functions if read by Tiff Read Function.
   3     Is Tag written during Tiff Write Function.
   4     Does the user recieve this tag via call back
         functions during Tiff Write Function.
   5     If Tag is received via call back function
         can it be modified and registered for Tiff Write Function
   6     Can this tag be registered by the user
   All private tags must be registered with tag id's > 32768
****************************************************************************/

#define IGTAG_TIF_NEWSUBFILETYPE                   10254       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_SUBFILETYPE                      10255       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_IMAGEWIDTH                       10256       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_IMAGEHEIGHT                      10257       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_BITSPERSAMPLE                    10258       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_COMPRESSION                      10259       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_PHOTOMETRICINTERPRETATION        10262       /*Y,Y,Y,Y,Y,N*/
#define IGTAG_TIF_THRESHOLDING                     10263       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_CELLWIDTH                        10264       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_CELLLENGTH                       10265       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_FILLORDER                        10266       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_DOCUMENTNAME                     10269       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_IMAGEDESCRIPTION                 10270       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_MAKE                             10271       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_MODEL                            10272       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_STRIPOFFSETS                     10273       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_ORIENTATION                      10274       /*Y,Y,Y,Y,Y,N*/
#define IGTAG_TIF_SAMPLESPERPIXEL                  10277       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_ROWSPERSTRIP                     10278       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_STRIPBYTECOUNTS                  10279       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_MINSAMPLEVALUE                   10280       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_MAXSAMPLEVALUE                   10281       /*Y,Y,N,N,N,Y*/

#define IGTAG_TIF_XRESNUMERATOR                    10282       /*Y,Y,Y,Y,Y,N*/
#define IGTAG_TIF_XRESDENOMINATOR                  11282       /*Y,Y,Y,Y,Y,N*/
#define IGTAG_TIF_YRESNUMERATOR                    10283       /*Y,Y,Y,Y,Y,N*/
#define IGTAG_TIF_YRESDENOMINATOR                  11283       /*Y,Y,Y,Y,Y,N*/

#define IGTAG_TIF_PLANARCONFIGURATION              10284       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_PAGENAME                         10285       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_XPOSITION                        10286       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_YPOSITION                        10287       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_FREEOFFSETS                      10288       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_FREEBYTECOUNTS                   10289       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_GRAYRESPONSEUNIT                 10290       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_GRAYRESPONSECURVE                10291       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_T4OPTIONS                        10292       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_T6OPTIONS                        10293       /*Y,Y,Y,N,N,N*/
#define IGTAG_TIF_RESOLUTIONUNIT                   10296       /*Y,Y,Y,Y,Y,N*/
#define IGTAG_TIF_PAGENUMBER                       10297       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_COLORRESPONSEUNIT                10300       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_TRANSFERFUNCTION                 10301       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_SOFTWARE                         10305       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_DATETIME                         10306       /*Y,Y,N,Y,Y,N*/
#define IGTAG_TIF_ARTIST                           10315       /*Y,Y,N,Y,Y,N*/
#define IGTAG_TIF_HOSTCOMPUTER                     10316       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_PREDICTOR                        10317       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_WHITPOINT                        10318       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_PRIMARYCHROMATICITIES            10319       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_COLORMAP                         10320       /*Y,Y,Y,Y,Y,N*/
#define IGTAG_TIF_HALFTONEHINTS                    10321       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_TILEWIDTH                        10322       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_TILELENGTH                       10323       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_TILEOFFSETS                      10324       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_TILEBYTECOUNTS                   10325       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_BADFAXLINES                      10326       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_CLEANFAXDATA                     10327       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_CONSECUTIVEBADFAXLINES           10328       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_INKSET                           10332       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_INKNAMES                         10333       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_NUMBEROFINKS                     10334       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_DOTRANGE                         10336       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_TARGETPRINTER                    10337       /*Y,Y,N,N,N,Y*/
#define IGTAG_TIF_EXTRASAMPLES                     10338       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_SAMPLEFORMAT                     10339       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_SMINSAMPLEVALUE                  10340       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_SMAXSAMPLEVALUE                  10341       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_TRANSFERRANGE                    10342       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGPROC                         10512       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGINTERCHANGEFORMAT            10513       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGINTERCHANGEFORMATLENGTH      10514       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGRESTARTINTERVAL              10515       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGLOSSLESSPREDICCTORS          10517       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGPOINTTRANSFORMS              10518       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGQTABLES                      10519       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGDCTTABLES                    10520       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_JPEGACTTABLES                    10521       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_YCBCRCOEFFICIENTS                10529       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_YCBCRSUBSAMPLING                 10530       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_YCBCRPOSITIONING                 10531       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_REFERENCEBLACKWHITE              10532       /*Y,Y,N,N,N,N*/
#define IGTAG_TIF_COPYRIGHT                        33432       /*Y,Y,N,N,N,Y*/
/*WARNING: Please do not use tag values greater than     */
/*    32768.  These tag values are reserved for private  */
/*    TIFF tags which will be defined by the customer    */
/*                                                       */


                                 

/* #ifndef __GEARTAGS_H__ */
#endif
