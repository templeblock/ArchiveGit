/*****************************************************************************

          Copyright (c) Micrografx, Inc., 1992.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
             from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

******************************************************************************
******************************************************************************

                                  mdaapi.h

******************************************************************************
******************************************************************************


*****************************************************************************/
#ifndef MDAAPI_H
#define MDAAPI_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************** Includes **********************************/
#ifdef MDAAPI_PRIVATE
    #include <mhost.h>
    #include "mda.h"
    #include "manifest.h"
#endif

/******************************** Constants *********************************/

#define MDAAPI_VERSION_STRING   "MDAAPI.LIB 1.80  10/02/92"

#define MDAERROR_ARCHIVE_CORRUPT        100
#define MDAERROR_ARCHIVE_NOT_FOUND      101
#define MDAERROR_DISK_WRITE             102
#define MDAERROR_DISK_READ              103
#define MDAERROR_DISK_FULL              104
#define MDAERROR_MANIFEST_CORRUPT       105
#define MDAERROR_MANIFEST_NOT_FOUND     106
#define MDAERROR_MEMORY                 107
#define MDAERROR_MEMBER_NOT_FOUND       108
#define MDAERROR_PACKAGE_NOT_FOUND      109
#define MDAERROR_PATH_INVALID           110
#define MDAERROR_USER_CANCEL            111
#define MDAERROR_UNKNOWN                255

#define MDAAPI  FAR PASCAL

#define MDAMSG_PATH                     100
#define MDAMSG_FILE                     101
#define MDAMSG_PCT                      102
#define MDAMSG_CRC                      103

/********************************** Types ***********************************/

typedef BOOL (FAR PASCAL *MDA_PROC) (UINT      Msg,
                                     LPSTR     lpName,
                                     int       Num,
                                     LPVOID    lpUser);

typedef struct
{
// These fields are loaded from the Manifest when it is opened:

    char     ProductName[40];   // Name/version of product in manifest
    char     Language[40];      // Language specified in manifest

// These fields are private and should not be used at the app. level:
#ifdef MDAAPI_PRIVATE
    char     ArchivePath[80];
    MDA_PROC lpCallBack;
    LPVOID   lpUser;
    DWORD    dwStatusTotal;
    DWORD    dwStatusCurrent;
    WORD     wLastArchive;
    MANIFEST Manifest;
#endif
} MDA_Info, FAR *LPMDAINFO;

/******************************* Public Data ********************************/
extern int __cdecl MDAAPI_LastError;

/***************************** Public Functions *****************************/

int MDAAPI MDAAPI_Version (LPSTR lpVersion,int nSize);

void MDAAPI CloseMDA (LPMDAINFO lpMDA);

BOOL MDAAPI ExtractMarked (LPMDAINFO lpMDA,
                           LPSTR     lpDest,
                           MDA_PROC  lpCallBack,
                           LPVOID    lpUser);

BOOL MDAAPI ExtractMember (LPMDAINFO lpMDA,
                           LPSTR     lpPackage,
                           LPSTR     lpFile,
                           LPSTR     lpDest,
                           LPSTR     lpDestName,
                           MDA_PROC  lpCallBack,
                           LPVOID    lpUser);

DWORD MDAAPI GetMemberSize (LPMDAINFO lpMDA,LPSTR lpPackage,LPSTR lpFile);

BOOL MDAAPI MarkMember (LPMDAINFO lpMDA,LPSTR lpPackage,LPSTR lpFile);

LPMDAINFO MDAAPI OpenMDA (LPSTR lpPath);

BOOL MDAAPI UnmarkMember (LPMDAINFO hMDA,LPSTR lpPackage,LPSTR lpFile);

BOOL MDAAPI VerifyMDA (LPMDAINFO lpMDA,MDA_PROC lpCallBack,LPVOID lpUser);

#ifdef __cplusplus
}
#endif

/****************************************************************************/
#endif
/****************************************************************************/
