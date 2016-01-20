/*
 *  sampver.h - internal header file to define the build version for
 *    sample code
 *
 */

/* 
 *  All strings MUST have an explicit \0  
 *
 *  MMSAMPRELEASE should be changed every build
 *
 *  Version string should be changed each build
 *
 *  Remove build extension on final release
 */

#define OFFICIAL                1
#define FINAL                   1

#define MMSAMPVERSION            00
#define MMSAMPREVISION           09
#define MMSAMPRELEASE            00

#if defined(DEBUG)
#define MMSAMPVERSIONSTR         "Debug Version 0.9.0\0"
#else
#define MMSAMPVERSIONSTR         "Version 0.9\0"
#endif


/***************************************************************************
 *  DO NOT TOUCH BELOW THIS LINE                                           *
 ***************************************************************************/

#ifdef RC_INVOKED

#define MMVERSIONCOMPANYNAME    "Microsoft Corporation\0"
#define MMVERSIONPRODUCTNAME	"Multimedia Sample Code\0"
#define MMVERSIONCOPYRIGHT      "Copyright \251 Microsoft Corp. 1992. All Rights Reserved\0"

/*
 *  Version flags 
 */

#ifndef OFFICIAL
#define MMVER_PRIVATEBUILD      VS_FF_PRIVATEBUILD
#else
#define MMVER_PRIVATEBUILD      0
#endif

#ifndef FINAL
#define MMVER_PRERELEASE        VS_FF_PRERELEASE
#else
#define MMVER_PRERELEASE        0
#endif

#if defined(DEBUG)
#define MMVER_DEBUG             VS_FF_DEBUG    
#else
#define MMVER_DEBUG             0
#endif

#define MMVERSIONFLAGS          (MMVER_PRIVATEBUILD|MMVER_PRERELEASE|MMVER_DEBUG)
#define MMVERSIONFILEFLAGSMASK  0x0000003FL


#endif
