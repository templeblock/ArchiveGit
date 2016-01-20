/*
 * ENGN.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This header file contains the class declaration for ALEngine,
 *  the base class used by all ArchiveLib compression and decompression
 *  engines.  It also contains the base class definition for two classes
 *  derived from ALEngine: ALCompressor and ALDecompressor.
 *
 * CLASS DEFINITIONS:
 *
 *  ALEngine
 *  ALCompressor
 *  ALDecompressor
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _CMPENGN_H
#define _CMPENGN_H

#if defined( __cplusplus )


/*
 * class ALEngine
 *
 * DESCRIPTION
 *
 * This header file defines the ALEngine class.  It is a base
 * class with pure virtual functions, so it cannot be instantiated.
 * The ALCompressor and ALDecompressor classes are both derived from
 * ALEngine.  ALEngine doesn't do any compression or decompression itself,
 * but it does keep track of the compression type, error status, and a
 * bunch of other useful stuff.
 *
 * DATA MEMBERS
 *
 *  miCompressionType   :  The compression type integer is what gets stored
 *                         in the archive directory, and what the archive
 *                         class looks at when extracting so it can figure
 *                         out what type of compression engine to construct
 *                         for a specific type of object.
 *
 *  mszCompressionType  :  This string just describes the compression type
 *                         in ASCII format suitable for printing or display.
 *
 *  mStatus             :  A standard ArchiveLib status object, stored
 *                         with the compression engine.  Check this after
 *                         completing a compression or decompression to
 *                         see how things went.
 *
 * MEMBER FUNCTIONS
 *
 *  ALEngine()             : The constructor.  Only called by the ctors for
 *                           derived classes.
 *  ~ALEngine()            : The virtual destructor.
 *  operator new()         : Memory allocation operator, only gets used
 *                           when the library is in a DLL.
 *  WriteEngineData()      : Private virtual function used to store engine
 *                           specific data.
 *  ReadEngineData()       : Private virtual function used to read back
 *                           engine specific data.
 *  ClearError()           : A helper function used to reset the mStatus
 *                           member if it gets in an error state.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  Februrary 14, 1996  2.0A : New release
 */

class AL_CLASS_TYPE ALEngine {  /* Tag public class */
/*
 * Constructors, destructors, declarations, and friends
 */
    public :
        AL_PROTO ALEngine( ALCompressionType compression_type_int,
                           const char AL_DLL_FAR *compression_type_string );
        virtual AL_PROTO ~ALEngine();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALEngine( ALEngine AL_DLL_FAR & );
        ALEngine AL_DLL_FAR & AL_PROTO operator=( ALEngine AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    public :
        virtual int AL_PROTO WriteEngineData( ALStorage AL_DLL_FAR * archive );
        virtual int AL_PROTO ReadEngineData( ALStorage AL_DLL_FAR * archive );
        void AL_PROTO ClearError();
/*
 * Data members
 */
    public :
        const ALCompressionType miCompressionType;
        const char AL_DLL_FAR *mszCompressionType;
        ALStatus mStatus;
        AL_CLASS_TAG( _ALEngineTag );
};

/*
 * class ALCompressor
 *
 * DESCRIPTION
 *
 * ALCompressor is the base class for all of the compression engines.
 * It defines the virtual function Compress(), which is used by
 * the archiving functions to perform compression.  This is the base
 * class for the three compressors in ArchiveLib: the copy, greenleaf,
 * and Pkware compressors.
 *
 * DATA MEMBERS
 *
 *
 * MEMBER FUNCTIONS
 *
 *  ALCompressor()         : The constructor.  Only called by the ctors for
 *                           derived classes.
 *  ~ALCompressor()        : The virtual destructor.
 *  operator new()         : Memory allocation operator, only gets used
 *                           when the library is in a DLL.
 *  Clone()                : If the engine is placed in a toolkit, this
 *                           function can be called to provoke it into
 *                           making a copy of itself.
 *  Compress()             : Compress an input object to an output object.
 *                           This is the big deal.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  Februrary 14, 1996  2.0A : New release
 */

class AL_CLASS_TYPE ALCompressor : public ALEngine {  /* Tag public class */
/*
 * Constructors, destructors, declarations, and friends
 */
    public :
        AL_PROTO ALCompressor( ALCompressionType compression_type_int,
                               const char AL_DLL_FAR *compression_type_string );
        virtual AL_PROTO ~ALCompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALCompressor( ALCompressor AL_DLL_FAR & );
        ALCompressor AL_DLL_FAR & AL_PROTO operator=( ALCompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    public :
        virtual ALCompressor AL_DLL_FAR * AL_PROTO Clone( int engine_type ) const = 0;
        virtual int AL_PROTO Compress( ALStorage AL_DLL_FAR &input_object,
                                       ALStorage AL_DLL_FAR &output_object ) = 0;
/*
 * Data members
 */
    public :
        AL_CLASS_TAG( _ALCompressorTag );
};

/*
 * class ALDecompressor
 *
 * DESCRIPTION
 *
 * ALDecompressor is the base class for all of the decompression engines.
 * It defines the virtual function Decompress(), which is used by
 * the archiving functions to perform decompression.  This is the base
 * class for the three decompressors in ArchiveLib: the copy, greenleaf,
 * and Pkware decompressors.
 *
 * DATA MEMBERS
 *
 *
 * MEMBER FUNCTIONS
 *
 *  ALDecompressor()       : The constructor.  Only called by the ctors for
 *                           derived classes.
 *  ~ALDecompressor()      : The virtual destructor.
 *  operator new()         : Memory allocation operator, only gets used
 *                           when the library is in a DLL.
 *  Clone()                : If the engine is placed in a toolkit, this
 *                           function can be called to provoke it into
 *                           making a copy of itself.
 *  Decompress()           : Decompress an input object to an output object.
 *                           This is the big deal.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  Februrary 14, 1996  2.0A : New release
 */

class AL_CLASS_TYPE ALDecompressor : public ALEngine {  /* Tag public class */
/*
 * Constructors, destructors, declarations, and friends
 */
    public :
        AL_PROTO ALDecompressor( ALCompressionType compression_type_int,
                                 const char AL_DLL_FAR *compression_type_string );
        virtual AL_PROTO ~ALDecompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALDecompressor( ALDecompressor AL_DLL_FAR & );
        ALDecompressor AL_DLL_FAR & AL_PROTO operator=( ALDecompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    public :
        virtual ALDecompressor AL_DLL_FAR * AL_PROTO Clone( int engine_type ) const = 0;
        virtual int AL_PROTO Decompress( ALStorage AL_DLL_FAR &input_object,
                                         ALStorage AL_DLL_FAR &output_object,
                                         long compressed_length = -1L ) = 0;
/*
 * Data members
 */
    public :
        AL_CLASS_TAG( _ALDecompressorTag );
};

#include "engn.inl"

#else /* #if defined( __cplusplus ) ... */

AL_LINKAGE void AL_FUNCTION ALDecompressorClearError( hALDecompressor this_object );
AL_LINKAGE int AL_FUNCTION ALCompress( hALCompressor this_object,
                                       hALStorage input,
                                       hALStorage output );
AL_LINKAGE int AL_FUNCTION ALDecompress( hALDecompressor this_object,
                                         hALStorage input,
                                         hALStorage output,
                                         long compressed_length );
AL_LINKAGE void AL_FUNCTION deleteALCompressor( hALCompressor this_object );
AL_LINKAGE void AL_FUNCTION deleteALDecompressor( hALDecompressor this_object );

AL_LINKAGE void AL_FUNCTION ALCompressorClearError( hALCompressor this_object );
AL_LINKAGE int AL_FUNCTION
ALCompressorSetError( hALCompressor this_object,
                      int error,
                      char AL_DLL_FAR *text );
AL_LINKAGE int AL_FUNCTION
ALCompressorGetStatusCode( hALCompressor this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALCompressorGetStatusString( hALCompressor this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALCompressorGetStatusDetail( hALCompressor this_object );
AL_LINKAGE int AL_FUNCTION
ALCompressorGetTypeCode( hALCompressor this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALCompressorGetTypeString( hALCompressor this_object );

AL_LINKAGE void AL_FUNCTION ALDecompressorClearError( hALDecompressor this_object );
AL_LINKAGE int AL_FUNCTION
ALDecompressorSetError( hALDecompressor this_object,
                        int error,
                        char AL_DLL_FAR *text );
AL_LINKAGE int AL_FUNCTION
ALDecompressorGetStatusCode( hALDecompressor this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALDecompressorGetStatusString( hALDecompressor this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALDecompressorGetStatusDetail( hALDecompressor this_object );
AL_LINKAGE int AL_FUNCTION
ALDecompressorGetTypeCode( hALDecompressor this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALDecompressorGetTypeString( hALDecompressor this_object );

#endif /* #if defined( __cplusplus ) ... else ...*/


#endif /* #ifndef _CMPENGN_H         */

