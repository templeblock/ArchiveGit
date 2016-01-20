/*
 * ALSIMPLE.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 * This header file contains the definitions for the C/C++
 * simplified interface.  This interface consists of eight
 * functions and a structure definition, all of which are
 * found here.  The idea is that this interface lets you
 * manipulate PKZip archives containing files with little
 * or not knowledge about the depths of ArchiveLib.
 *
 * CLASS DEFINITIONS:
 *
 *  ALZipDir            : A description of a single entry in an ZIP
 *                        file. Some of the simplified interface
 *                        functions use an array of ALZipDir entries
 *                        to describe an archive.
 *
 *  ALSimpleMonitor     : The monitor used by the simplified interface
 *                        functions.  However, you don't have to know
 *                        anything about this monitor class, its use
 *                        is hidden.
 *
 * FUNCTIONS
 *
 *   ALFreeDir()
 *   ALReadDir()
 *   ALWriteDir()
 *   ALSetName()
 *   ALSetComment()
 *   ALCreate()
 *   ALAppend()
 *   ALExtract()
 *   ALDelete()
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : First release
 *
 *  March 21, 1996     2.01A :  First, moved all of the simple monitor
 *                              code out of this header and into a separate
 *                              file. That allows me to exclude it from
 *                              the test drive.  Second, changed most of
 *                              the char * parameters in the simple API
 *                              to const char *, so CStrings can be passed.
 *                              Third, export of ALZipDir is not done when
 *                              calling from C.  Probably don't need it
 *                              in C++ either, since the struct has no
 *                              member functions.
 */

#if !defined( _ALSIMPLE_H )
#define _ALSIMPLE_H

#include "aldefs.h"

#if !defined( __cplusplus )
struct ALZipDir {
#else
struct AL_CLASS_TYPE ALZipDir {
#endif
    char AL_DLL_FAR *name;      /* These two members are allocated using  */
    char AL_DLL_FAR *comment;   /* new char[].  Let the library take care */
                                /* of allocation and deletion.            */
    long compressed_size;
    long compressed_position;
    long size;
    long crc;
    short int mark;
    short int month;
    short int date;
    short int year;
    short int hour;
    short int minute;
    short int second;
    unsigned char r;
    unsigned char a;
    unsigned char s;
    unsigned char h;
    unsigned char d;
    unsigned char level;
};

typedef
void ( AL_EXPORT AL_DLL_FAR * CALLBACK_FN )( const char AL_DLL_FAR *file_name,
                                             int file_ratio,
                                             int job_ratio );

#if defined( __cplusplus )
extern "C" {
#endif

AL_LINKAGE void AL_FUNCTION ALFreeDir( struct ALZipDir AL_DLL_FAR *z );
AL_LINKAGE struct ALZipDir AL_DLL_FAR * AL_FUNCTION
ALReadDir( const char AL_DLL_FAR *filename,
           int AL_DLL_FAR *count,
           int AL_DLL_FAR *error );
AL_LINKAGE int AL_FUNCTION ALWriteDir( struct ALZipDir AL_DLL_FAR *z );
AL_LINKAGE void AL_FUNCTION ALSetName( struct ALZipDir AL_DLL_FAR *z,
                                       const char AL_DLL_FAR *name );
AL_LINKAGE void AL_FUNCTION ALSetComment( struct ALZipDir AL_DLL_FAR *z,
                                          const char AL_DLL_FAR *comment );
AL_LINKAGE int AL_FUNCTION ALCreate( const char AL_DLL_FAR *name,
                                     const char AL_DLL_FAR *file_list,
                                     int strip_path,
                                     CALLBACK_FN callback );
AL_LINKAGE int AL_FUNCTION ALAppend( const char AL_DLL_FAR *archive_name,
                                     const char AL_DLL_FAR *input_files,
                                     int strip_path,
                                     CALLBACK_FN callback );
AL_LINKAGE int AL_FUNCTION ALExtract( struct ALZipDir AL_DLL_FAR *z,
                                      int strip_path,
                                      CALLBACK_FN callback );
AL_LINKAGE int AL_FUNCTION ALDelete( struct ALZipDir AL_DLL_FAR *z,
                                     CALLBACK_FN callback );

#if defined( __cplusplus )
};
#endif

#endif

