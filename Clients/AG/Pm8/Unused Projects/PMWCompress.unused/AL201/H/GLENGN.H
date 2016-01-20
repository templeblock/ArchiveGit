/*
 * GLENGH.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the class declaration for ALGlCompressor
 *  and ALGlDecompressor.  These are the two classes that together
 *  impelement the complete greenleaf compression engine.
 *
 * CLASS DEFINITIONS:
 *
 *  ALGlCompressor
 *  ALGlDecompressor
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
 *  August 10, 1994 1.0B : Added an inline function that will return the
 *                         compression level.  This is needed if you are
 *                         going to do a nice listing of objects stored
 *                         in an archive.
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _GRENENGN_H
#define _GRENENGN_H

#include "arclib.h"

#if defined( __cplusplus )


#include "engn.h"

/*
 * class ALGlCompressor
 *
 * DESCRIPTION
 *
 * ALGlCompressor is the Greenleaf proprietary compression
 * engine.  Compression engines have a simple API, so there aren't
 * too many functions.  This class has two data members that
 * are initialized in the constructor.  One of them, the compression level,
 * has to be saved with the data in order for decompression to work
 * properly.  It is saved and read with the virtual functions
 * ReadEngineData() and WriteEngineData(), using a single short
 * in the archive directory.
 *
 * DATA MEMBERS
 *
 *  miCompressionLevel    : This is the compression level that the
 *                          compressor will attempt to use, selected
 *                          from one of the five settings found in
 *                          the enumerated type in ALDEFS.H.  If the
 *                          file is incompressible and the engine
 *                          performs a straight binary copy this value
 *                          changes to AL_GREENLEAF_COPY.
 *
 *  miFailUncompressible  : This data member is used to flag the
 *                          action the compressor takes if a file
 *                          turns out to be incompressible.  If this
 *                          member is set, the engine will stop compressing,
 *                          seek back to the start of the file, and
 *                          just copy the data.  If this member is clear,
 *                          we don't ever check to see if the file is
 *                          compressing properly.
 *
 * MEMBER FUNCTIONS
 *
 *  ALGlCompressor()        : The constructor.
 *  ~ALGlCompressor()       : The virtual destructor.
 *  operator new()          : The memory allocation operator, which is
 *                            only used when the library is in a DLL.
 *  WriteEngineData()       : The virtual function that writes private
 *                            configuration data.  For this class, this
 *                            function writes out a single short int
 *                            containing the compression level.
 *  ReadEngineData()        : The complement for the above function, used
 *                            during extraction.
 *  Compress()              : The routine that actually performs the
 *                            compression.
 *  Clone()                 : If a compression engine is added to a
 *                            toolkit, this function can be called to
 *                            create a new compressor.
 *  CompressionLevel()      : A member function that returns the compression
 *                            level for the engine.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0: New release
 */

class AL_CLASS_TYPE ALGlCompressor : public ALCompressor {  /* Tag public class */
/*
 * Declarations, friends, constructors, destructors
 */
    public :
#if defined( AL_FLAT_MODEL )
        AL_PROTO ALGlCompressor( short int compression_level = AL_GREENLEAF_LEVEL_4,
                                 short int fail_uncompressible = 0 );
#else
        AL_PROTO ALGlCompressor( short int compression_level = AL_GREENLEAF_LEVEL_2,
                                 short int fail_uncompressible = 0 );
#endif
        virtual AL_PROTO ~ALGlCompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALGlCompressor( ALGlCompressor AL_DLL_FAR & );
        ALGlCompressor AL_DLL_FAR & AL_PROTO operator=( ALGlCompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    protected :
        virtual int AL_PROTO WriteEngineData( ALStorage AL_DLL_FAR * archive );
        virtual int AL_PROTO ReadEngineData( ALStorage AL_DLL_FAR * archive );

    public :
        virtual ALCompressor AL_DLL_FAR * AL_PROTO Clone( int engine_type ) const;
        virtual int AL_PROTO Compress( ALStorage AL_DLL_FAR &input,
                                       ALStorage AL_DLL_FAR &output );
        short int AL_PROTO CompressionLevel();
/*
 * Data members
 */
    protected :
        short int miCompressionLevel;
        short int miFailUncompressible;
    public :
        AL_CLASS_TAG( _ALGlCompressorTag );
};


/*
 * class ALGlDecompressor
 *
 * DESCRIPTION
 *
 * ALGlDecompressor is the Greenleaf proprietary decompression
 * engine.  Decompression engines have a simple API, so there aren't
 * too many functions.  This class has a single data member (the
 * compression level) that is initialized in the constructor.  The
 * compression level has to be the same as it was when in the compressor
 * when the file was compressed.
 *
 * DATA MEMBERS
 *
 *  miCompressionLevel    : This is the compression level that the
 *                          decompressor will use, selected
 *                          from one of the five settings found in
 *                          the enumerated type in ALDEFS.H.  This must
 *                          match the compression level that was used
 *                          by the ALGlCompressor engine when the
 *                          object was first compressed.
 *
 * MEMBER FUNCTIONS
 *
 *  ALGlDecompressor()      : The constructor.
 *  ~ALGlDecompressor()     : The virtual destructor.
 *  operator new()          : The memory allocation operator, which is
 *                            only used when the library is in a DLL.
 *  WriteEngineData()       : The virtual function that writes private
 *                            configuration data.  For this class, this
 *                            function writes out a single short int
 *                            containing the compression level.
 *  ReadEngineData()        : The complement for the above function, used
 *                            during extraction.
 *  Decompress()            : The routine that actually performs the
 *                            decompression.
 *  Clone()                 : If a compression engine is added to a
 *                            toolkit, this function can be called to
 *                            create a new compressor.
 *  CompressionLevel()      : A member function that returns the compression
 *                            level for the engine.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0: New release
 */

class AL_CLASS_TYPE ALGlDecompressor : public ALDecompressor {  /* Tag public class */
/*
 * Declarations, friends, constructors, destructors
 */
    public :
#if defined( AL_FLAT_MODEL ) || defined( AL_FLAT_MODEL )
        AL_PROTO ALGlDecompressor( short int compression_level = AL_GREENLEAF_LEVEL_4 );
#else
        AL_PROTO ALGlDecompressor( short int compression_level = AL_GREENLEAF_LEVEL_2 );
#endif
        virtual AL_PROTO ~ALGlDecompressor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.  I define
 * them here to prevent the compiler from creating default versions.
 */
    protected :
        AL_PROTO ALGlDecompressor( ALGlDecompressor AL_DLL_FAR & );
        ALGlDecompressor AL_DLL_FAR & AL_PROTO operator=( ALGlDecompressor AL_DLL_FAR & rhs );
/*
 * Member functions
 */
    protected :
        virtual int AL_PROTO WriteEngineData( ALStorage AL_DLL_FAR * archive );
        virtual int AL_PROTO ReadEngineData( ALStorage AL_DLL_FAR * archive );

    public :
        virtual ALDecompressor AL_DLL_FAR * AL_PROTO Clone( int engine_type ) const;
        virtual int AL_PROTO Decompress( ALStorage AL_DLL_FAR &input,
                                         ALStorage AL_DLL_FAR &output,
                                         long compressed_length = -1L );
        short int AL_PROTO CompressionLevel();
/*
 * Data members
 */
    public :
        short int miCompressionLevel;
        AL_CLASS_TAG( _ALGlDecompressorTag );
};

#include "glengn.inl"

#else /* #ifdef __cplusplus ... */

AL_LINKAGE short int AL_FUNCTION ALGlCompressorLevel( hALCompressor this_object );
AL_LINKAGE short int AL_FUNCTION ALGlDecompressorLevel( hALDecompressor this_object );
AL_LINKAGE hALCompressor AL_FUNCTION
newALGlCompressor( short int compression_level, short int fail_uncompressible );
AL_LINKAGE hALDecompressor AL_FUNCTION
newALGlDecompressor( short int compression_level );

#endif /* #ifdef __cplusplus ... #else ... */

#endif /* #ifdef _GRENENGN_H */
