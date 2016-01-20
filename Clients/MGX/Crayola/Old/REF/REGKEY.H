/******************************************************************************
 (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.  This material is
 confidential and a trade secret.  Permission to use this work for any purpose
 must be obtained in writing from MICROGRAFX, 1303 Arapaho Rd., Richardson,
 Texas  75081.

*******************************************************************************

                                    regkey.h

*******************************************************************************/

/*
    MAX_LICENSE_INFO indicates how may characters will be used for both the
    code and the key.  Both the app and the generator app MUST agree on this value.
*/
#define MAX_LICENSE_INFO        10 // a 4 byte value requires this
#define DEFAULT_LIC_COUNT       1
#define DEF_APP_COUNT           1

typedef struct
{ // the size of this structure should be compatible with MAX_LICENSE_INFO
//    WORD    wLicenseCount;
//    BYTE    bApp;
    BYTE    bNull;
    WORD    wTickCount;
    BYTE    bChecksum;
} LICENSEINFO, *PLICENSEINFO;

#define LICENSE_ERR_BADDATA     -8

int     MGXGenerateLicenseKey(LPSTR lpszLicenseInfo, WORD wNewLicenseCount,
                              LPWORD lpwLicenseCount, LPBYTE lpbApp,
                              LPSTR lpszKey);

void    LicenseToString(PLICENSEINFO pLicenseInfo, LPSTR lpszLicenseString);
void    StringToLicense(LPSTR lpszLicenseString, PLICENSEINFO pLicenseInfo);
