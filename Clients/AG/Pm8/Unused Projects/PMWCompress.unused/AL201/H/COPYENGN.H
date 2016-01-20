/*
 * COPYENGN.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994, 1995 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This header file contains the class definition for the derived
 *  classes ALCopyCompressor and ALCopyDecompressor.
 *
 * CLASS DEFINITIONS:
 *
 *  ALCopyCompressor
 *  ALCopyDecompressor
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  July 8, 1994  1.0B  : In most of my class definitions I create a prototype
 *                        for a private/protected copy constructor and
 *                        assignment operator, because I don't want the compiler
 *                        to create default versions.  I forgot to do it for
 *                        this class, which didn't cause any trouble under
 *                        MS-DOS.  gcc under UNIX flagged the problem, which
 *                        I fixed by adding the prototypes.
 *
 *  January 1, 1995 1.01A : Changed include of storage.h to stor.h.  Had
 *                          clash with the new header file
 *                          STORAGE.H used by compiler vendors for
 *                          OLE implementation.
 *
 *  February 14, 1996  2.0: New release
 */

#ifndef _COPYENG_H
#define _COPYENG_H

#include "arclib.h"

#if defined( __cplusplus )

#include "engn.h"
#include "stor.h"

/*
 * class ALCopyCompressor
 *
 * DESCRIPTION
 *
 * ALCopyCompressor is a real simple class.  All it does is
 * perform a raw binary copy of input to output.
 *
 * DATA MEMBERS
 *
 *  None!
 *
 * MEMBER FUNCTIONS
 *
 *  ALCopyCompressor()     : The constructor.
 *  ~ALCopyCompressor()    : The virtual destructor.
 *  operator new()         : Memory allocation operator, only gets used
 *                           when the library is in a DLL.
 *  Clone()                : If an ALCopyCompressor object is stored in a
 *                           toolkit, this function will be called to
 *                           create new copies of itself.
 *  Compress()             : The compression routine.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996 2.0A  : New release.
 *
 */

class AL_CLASS_TYPE ALCopyCompressor : public ALCompressor {  /* Tag public class */
/*
 * Constructors, destructors, declarations, friends
 */
    public :
        AL_PROTO ALCopyCompressor();
        virtual AL_PROTO ~ALCopyCompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALCopyCompressor( ALCopyCompressor AL_DLL_FAR & );
        ALCopyCompressor AL_DLL_FAR & AL_PROTO operator=( ALCopyCompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    public :
        virtual ALCompressor AL_DLL_FAR * AL_PROTO Clone( int engine_type ) const;
        virtual int AL_PROTO Compress( ALStorage AL_DLL_FAR &input,
                                    ALStorage AL_DLL_FAR &output );
/*
 * Data members
 */
    public :
        AL_CLASS_TAG( _ALCopyCompressorTag );
};

/*
 * class ALCopyDecompressor
 *
 * DESCRIPTION
 *
 * ALCopyDecompressor is a real simple class.  All it does is
 * perform a raw binary copy of input to output.  It looks
 * an awful lot like its siblinb, ALCopyCompressor.
 *
 * DATA MEMBERS
 *
 *  None!
 *
 * MEMBER FUNCTIONS
 *
 *  ALCopyDecompressor()   : The constructor.
 *  ~ALCopyDecompressor()  : The virtual destructor.
 *  operator new()         : Memory allocation operator, only gets used
 *                           when the library is in a DLL.
 *  Clone()                : If an ALCopyCompressor object is stored in a
 *                           toolkit, this function will be called to
 *                           create new copies of itself.
 *  DeCompress()           : The decompression routine.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996 2.0A  : New release.
 *
 */

class AL_CLASS_TYPE ALCopyDecompressor : public ALDecompressor {  /* Tag public class */
/*
 * Constructors, destructors, declarations, friends
 */
    public :
        AL_PROTO ALCopyDecompressor();
        virtual AL_PROTO ~ALCopyDecompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALCopyDecompressor( ALCopyDecompressor AL_DLL_FAR & );
        ALCopyDecompressor AL_DLL_FAR & AL_PROTO operator=( ALCopyDecompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    public :
        virtual ALDecompressor AL_DLL_FAR * AL_PROTO Clone( int engine_type ) const;
        virtual int AL_PROTO Decompress( ALStorage AL_DLL_FAR &input,
                                         ALStorage AL_DLL_FAR &output,
                                         long compressed_length = -1 );
/*
 * Data members
 */
    public :
        AL_CLASS_TAG( _ALCopyDecompressorTag );
};

#endif /* #if defined( __cplusplus ) */

#if !defined( __cplusplus )

AL_LINKAGE hALCompressor AL_FUNCTION newALCopyCompressor( void );
AL_LINKAGE hALDecompressor AL_FUNCTION newALCopyDecompressor( void );

#endif /* #if !defined( __cplusplus ) */

#endif /* #ifndef _COPYENG_H         */
