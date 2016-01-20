/*
 * PKENGH.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the class declaration for ALPkCompressor
 *  and ALPkDecompressor.  These are the two classes that together
 *  impelement the complete PKWare compatible engine.
 *
 * CLASS DEFINITIONS:
 *
 *  ALPkCompressor
 *  ALPkDecompressor
 *
 * REVISION HISTORY
 *
f*  February 14, 1996  2.0A : First release
 */

#ifndef _PKENGN_H
#define _PKENGN_H

#if defined( __cplusplus )

/*
 * class ALPkCompressor
 *
 * DESCRIPTION
 *
 * ALPkCompressor is the class that provides an interface to the
 * ZLIB compression engine.  Compression engines have a simple API, so there
 * aren't too many functions.  This class has four data members that
 * are initialized in the constructor.  The members don't have to be
 * properly saved with the data, because the decompression engine
 * doesn't need any a priori information about the incoming stream, other
 * than the knowledge that it was created with a deflate compressor.
 *
 * DATA MEMBERS
 *
 *  option :     One of the four settings defined by the PKZip format.
 *               Note that this is a value that is kind of arbitrary.
 *
 *  miLevel   :  A number between 1 and 9.  1 gives the best speed, 9 gives
 *               the best compression.
 *
 *  miWindowBits : The base 2 logarithm of the number of bytes in the history
 *                 buffer.  Values between 8 and 15 are legal.  More bits,
 *                 more memory.
 *
 *  miMemLevel   : How much memory should be allocated for the internal
 *                 compression state. memLevel=1 uses minimum memory but
 *                 is slow and reduces compression ratio; memLevel=9 uses
 *                 maximum memory for optimal speed. (quoted from zlib.h)
 *
 * MEMBER FUNCTIONS
 *
 *  ALPkCompressor()        : The constructor.
 *  ~ALPkCompressor()       : The virtual destructor.
 *  operator new()          : The memory allocation operator, which is
 *                            only used when the library is in a DLL.
 *  Compress()              : The routine that actually performs the
 *                            compression.
 *  Clone()                 : If a compression engine is added to a
 *                            toolkit, this function can be called to
 *                            create a new compressor.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : First release
 */

class AL_CLASS_TYPE ALPkCompressor : public ALCompressor {  /* Tag public class */
/*
 * Declarations, friends, constructors, destructors
 */
    public :
#if defined( AL_FLAT_MODEL ) || defined( AL_FLAT_MODEL )
        AL_PROTO ALPkCompressor( int level = 6, int window_bits = 15, int mem_level = 8 );
#else
        AL_PROTO ALPkCompressor( int level = 6, int window_bits = 13, int mem_level = 6 );
#endif
        virtual AL_PROTO ~ALPkCompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALPkCompressor( ALPkCompressor AL_DLL_FAR & );
        ALPkCompressor AL_DLL_FAR & AL_PROTO operator=( ALPkCompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    protected :
/*
 * No private data at this time
 *
 *      virtual int AL_PROTO WriteEngineData( ALStorage AL_DLL_FAR * archive );
 *      virtual int AL_PROTO ReadEngineData( ALStorage AL_DLL_FAR * archive );
 */
    public :
        virtual ALCompressor AL_DLL_FAR * AL_PROTO Clone( int engine_type ) const;
        virtual int AL_PROTO Compress( ALStorage AL_DLL_FAR &input,
                                       ALStorage AL_DLL_FAR &output );
/*
 * Data members
 */
    enum _option { /* Tag protected type */
        NORMAL,
        MAXIMUM,
        FAST,
        SUPER_FAST } option;
        int miLevel;
        int miWindowBits;
        int miMemLevel;
    public :
        AL_CLASS_TAG( _ALPkCompressorTag );
};

/*
 * class ALPkDecompressor
 *
 * DESCRIPTION
 *
 * ALPkDecompressor is the class that provides an interface to the
 * ZLIB decompression engine.  Decompression engines have a simple API, so
 * there aren't too many functions.  This class has one data member that
 * is initialized when the directory is read in from a PKZIP file.
 *
 * DATA MEMBERS
 *
 *  option :     One of the four settings defined by the PKZip format.
 *               Note that this is a value that is kind of arbitrary.
 *
 * MEMBER FUNCTIONS
 *
 *  ALPkDecompressor()        : The constructor.
 *  ~ALPkDecompressor()       : The virtual destructor.
 *  operator new()            : The memory allocation operator, which is
 *                              only used when the library is in a DLL.
 *  Decompress()              : The routine that actually performs the
 *                              decompression.
 *  Clone()                   : If a compression engine is added to a
 *                              toolkit, this function can be called to
 *                              create a new compressor.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : First release
 */

class AL_CLASS_TYPE ALPkDecompressor : public ALDecompressor {  /* Tag public class */
/*
 * Declarations, friends, constructors, destructors
 */
    public :
        AL_PROTO ALPkDecompressor();
        virtual AL_PROTO ~ALPkDecompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALPkDecompressor( ALPkDecompressor AL_DLL_FAR & );
        ALPkDecompressor AL_DLL_FAR & AL_PROTO operator=( ALPkDecompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    protected :
/*
 * No private data at this time
 *
 *      virtual int AL_PROTO WriteEngineData( ALStorage AL_DLL_FAR * archive );
 *      virtual int AL_PROTO ReadEngineData( ALStorage AL_DLL_FAR * archive );
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
    enum _option {  /* Tag protected type */
        NORMAL,
        MAXIMUM,
        FAST,
        SUPER_FAST } option;
        AL_CLASS_TAG( _ALDecompressorTag );
};

#else /* #if defined( __cplusplus ) ... */

AL_LINKAGE short int AL_FUNCTION ALPkCompressorLevel( hALCompressor this_object );
AL_LINKAGE short int AL_FUNCTION ALPkDecompressorLevel( hALDecompressor this_object );
AL_LINKAGE hALCompressor AL_FUNCTION newALPkCompressor( int level,
                                                        int window_bits,
                                                        int mem_level );
AL_LINKAGE hALDecompressor AL_FUNCTION newALPkDecompressor( void );

#endif  /* #if defined( __cplusplus ) ... #else ... */

#endif /* #ifndef _PKENGN_H */

