/*
 * CMPOBJ.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This header file contains the class definition for ALCompressedObject.
 *
 * CLASS DEFINITIONS:
 *
 *  ALCompressedObject
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  July 13, 1994 1.0B  : Modified ReadHeaderData() and WriteHeaderData()
 *                        to have a pointer to the object being
 *                        inserted/extracted.
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _CMPOBJ_H
#define _CMPOBJ_H

#if defined( __cplusplus )

/*
 * class ALCompressedObject
 *
 * DESCRIPTION
 *
 * This is the class definition for ALCompressedObject.  A compressed
 * object is a very simple artifact that contains some compressed data,
 * a checksum, and a couple of length variables.  No comments, no engine
 * data, no archive data, nothing else.  This give this type of object
 * a very low overhead.  It also assumes that when you create a compressed
 * object using a storage object and a compression engine that you will
 * know what type of storage object and compression engine to use when
 * expanding it.
 *
 * If you decide you want to add some private data to your compressed
 * object, it isn't hard.  Just derive a new class, and implement the
 * two virtual functions defined here to read and write your own private
 * data during compression and decompression.
 *
 * DATA MEMBERS
 *
 *  mpCompressor         : A pointer to the compression engine that will
 *                         be used by this object.  This is assigned when
 *                         the object is created, and used to insert objects.
 *
 *  mpDecompressor       : A pointer to the decompression engine that will
 *                         be used by this object.  This is assigned when
 *                         the object is created, and used to extract objects.
 *                         Note that either of the two engine pointers can be
 *                         0 if they aren't needed by an application.
 *
 *  mpStorageObject      : A pointer to the storage object where this
 *                         object will live.
 *
 *  mStatus              : A standard ArchiveLib status object.
 *
 * MEMBER FUNCTIONS
 *
 *  ALCompressedObject()  : The only constructor for ALCompressedObject.
 *  ~ALCompressedObject() : The virtual destructor.
 *  operator new()        : Memory allocation operator, used in DLL only.
 *  WriteHeaderData()     : Virtual function to allow for storage of
 *                          customized data in the object header.
 *  ReadHeaderData()      : The virtual complement to the previous function,
 *                          lets you read in some customized data.
 *  Insert()              : Insert a single storage object into the Compressed
 *                          object.
 *  Extract()             : Extract the storage object from the Compressed
 *                          object.
 *  ClearError()          : A shortcut function to clear the error code
 *                          in the mStatus member.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

class AL_CLASS_TYPE ALCompressedObject {  /* Tag public class */
/*
 * Constructors, destructors, declarations, friends
 */
    public :
        AL_PROTO ALCompressedObject( ALStorage AL_DLL_FAR & storage_object,
                                     ALCompressor AL_DLL_FAR *compressor,
                                     ALDecompressor AL_DLL_FAR *decompressor );
        virtual AL_PROTO ~ALCompressedObject();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * Define the copy constructor and assignment operator here, that way
 * the compiler won't attempt to.
 */
    protected :
        AL_PROTO ALCompressedObject( ALCompressedObject AL_DLL_FAR & );
        ALCompressedObject AL_DLL_FAR & AL_PROTO operator = ( ALCompressedObject AL_DLL_FAR & );
/*
 * Member functions
 */
    protected :
        virtual int AL_PROTO WriteHeaderData( ALStorage AL_DLL_FAR * storage = 0 );
        virtual int AL_PROTO ReadHeaderData( ALStorage AL_DLL_FAR * storage = 0 );
    public :
        int AL_PROTO Insert( ALStorage AL_DLL_FAR &input_object );
        int AL_PROTO Extract( ALStorage AL_DLL_FAR &output_object );
        void AL_PROTO ClearError();
/*
 * Data members
 */
    protected :
        ALCompressor AL_DLL_FAR *mpCompressor;
        ALDecompressor AL_DLL_FAR *mpDecompressor;
        ALStorage AL_DLL_FAR *mpStorageObject;

    public :
        ALStatus mStatus;
        AL_CLASS_TAG( _ALCompressedObjectTag );
};

#include "cmpobj.inl"

#else /* #if defined( __cplusplus ) ... */

AL_LINKAGE void AL_FUNCTION ALCompressedClearError( hALCompressed this_object );
AL_LINKAGE hALCompressed AL_FUNCTION newALCompressed(
                            hALStorage storage,
                            hALCompressor compressor,
                            hALDecompressor decompressor );
AL_LINKAGE void AL_FUNCTION deleteALCompressed( hALCompressed this_object );
AL_LINKAGE int AL_FUNCTION
ALCompressedInsert( hALCompressed this_object,
                    hALStorage input_object );
AL_LINKAGE int AL_FUNCTION
ALCompressedExtract( hALCompressed this_object,  hALStorage output_object );
AL_LINKAGE int AL_FUNCTION
ALCompressedGetStatusCode( hALCompressed this_object );
AL_LINKAGE int AL_FUNCTION
ALCompressedSetError( hALCompressed this_object,
                      int error,
                      char AL_DLL_FAR *text );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALCompressedGetStatusString( hALCompressed this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALCompressedGetStatusDetail( hALCompressed this_object );

#endif /* #if defined( __cplusplus ) ... #else ... */

#endif /* #ifdef _CMPOBJ_H           */

