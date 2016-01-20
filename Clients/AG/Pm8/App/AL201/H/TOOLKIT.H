/*
 * TOOLKIT.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the class declaration for ALToolKit,
 *  the class used to hold toolkits.  ALToolKit is used by
 *  ALEntryList to build new storage objects and compression engines.
 *
 * CLASS DEFINITIONS:
 *
 *  ALToolKit
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : New release
 */

#if !defined( _TOOLKIT_H )
#define _TOOLKIT_H

#if defined( __cplusplus )

/*
 * class ALToolKit
 *
 * DESCRIPTION
 *
 * A toolkit is simply a loose collection of storage objects and
 * compression engines.  Every ALEntryList gets a toolkit.  When
 * an archive function is called to extract objects, it has to
 * go to the toolkit to try to create new storage objects and
 * engines.  This is done by calling the Clone() function for all
 * of the objects in the toolkit bag.  So, if you want to extract
 * from an PKZip archive into ALMyFile objects instead of ALFile
 * objects, create a toolkit that has ALMyFile in it, with the
 * appropriate Clone() function.
 *
 * In order to make a toolkit easy to use, you can employ
 * the addition operator to add a compressor, decompressor, or
 * storage object to a toolkit.  This increases its bag-like
 * qualities.
 *
 * DATA MEMBERS
 *
 *  mpCompressors    : An array of all the compression objects the
 *                     toolkit contains.  When the CreateCompressor()
 *                     routine is called, it iterates through this list
 *                     calling Clone() until it finds a hit.  This list
 *                     is terminated with a null pointer.
 *
 *  mpDecompressors : A list of decompression objects.  See the above
 *                    description.
 *
 *  mpStorages      : A list of storage objects.  See the above description.
 *
 * MEMBER FUNCTIONS
 *
 *  ALToolKit()          : The toolkit class has five constructors.
 *                         It has a default constructor, a copy
 *                         constructor, and a constructor that uses
 *                         storage objects, compressors, or decompressors.
 *
 *  ~ToolKit()           : Destructor.
 *
 *  operator new()       : Memory allocation function, used when the
 *                         library is in a DLL.
 *
 *  operator=()          : The toolkit assignment operator.
 *
 *  operator+()          :  Three operators, one for each of the object
 *                          types that can be added to a toolkit.
 *
 *  CreateCompressor()   :  The function that is called to create a new
 *                          compressor.
 *
 *  CreateDecompressor() :  See above.
 *
 *  CreateStorageObject() : See above.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : New release
 *
 */

class AL_CLASS_TYPE ALToolKit {  /* Tag public class */
    public :
        AL_PROTO ALToolKit();
        AL_PROTO ALToolKit( const ALCompressor AL_DLL_FAR & );
        AL_PROTO ALToolKit( const ALDecompressor AL_DLL_FAR & );
        AL_PROTO ALToolKit( const ALStorage AL_DLL_FAR & );
        AL_PROTO ALToolKit( const ALToolKit AL_DLL_FAR & );
        AL_PROTO ~ALToolKit();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
    protected :
        AL_PROTO operator=( ALEntry AL_DLL_FAR & );

    protected :
        ALCompressor AL_DLL_FAR * AL_DLL_FAR *mpCompressors;
        ALDecompressor AL_DLL_FAR * AL_DLL_FAR *mpDecompressors;
        ALStorage AL_DLL_FAR * AL_DLL_FAR *mpStorages;
    public :
        ALToolKit AL_DLL_FAR & AL_PROTO operator+( const ALCompressor AL_DLL_FAR &);
        ALToolKit AL_DLL_FAR & AL_PROTO operator+( const ALDecompressor AL_DLL_FAR &);
        ALToolKit AL_DLL_FAR & AL_PROTO operator+( const ALStorage AL_DLL_FAR &);
        ALCompressor AL_DLL_FAR * AL_PROTO CreateCompressor( int engine_type );
        ALDecompressor AL_DLL_FAR * AL_PROTO CreateDecompressor( int engine_type );
        ALStorage AL_DLL_FAR * AL_PROTO CreateStorageObject( const char AL_DLL_FAR *name, int storage_type );

        AL_CLASS_TAG( _ALToolKitTag );
};

/*
 * These standalone operators are here to make the toolkit even
 * easier to use.
 */
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALCompressor AL_DLL_FAR &, const ALCompressor AL_DLL_FAR & );
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALCompressor AL_DLL_FAR &, const ALDecompressor AL_DLL_FAR &);
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALCompressor AL_DLL_FAR &, const ALStorage AL_DLL_FAR &);
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALDecompressor AL_DLL_FAR &, const ALCompressor AL_DLL_FAR &);
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALDecompressor AL_DLL_FAR &, const ALDecompressor AL_DLL_FAR &);
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALDecompressor AL_DLL_FAR &, const ALStorage AL_DLL_FAR &);
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALStorage AL_DLL_FAR &, const ALCompressor AL_DLL_FAR &);
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALStorage AL_DLL_FAR &, const ALDecompressor AL_DLL_FAR &);
AL_LINKAGE ALToolKit AL_FUNCTION operator+( const ALStorage AL_DLL_FAR &, const ALStorage AL_DLL_FAR &);

#if defined( AL_FLAT_MODEL ) || defined( AL_WINDOWS )
#define AL_PK_DEFAULT_WINDOW_BITS 15
#define AL_PK_DEFAULT_MEM_LEVEL 8
#define AL_GL_DEFAULT_LEVEL AL_GREENLEAF_LEVEL_4
#else
#define AL_PK_DEFAULT_WINDOW_BITS 13
#define AL_PK_DEFAULT_MEM_LEVEL 6
#define AL_GL_DEFAULT_LEVEL AL_GREENLEAF_LEVEL_2
#endif

AL_LINKAGE ALToolKit AL_FUNCTION FullTools();
AL_LINKAGE ALToolKit AL_FUNCTION
    PkTools( int level = 6,
             int window_bits = AL_PK_DEFAULT_WINDOW_BITS,
             int mem_level = AL_PK_DEFAULT_MEM_LEVEL );
AL_LINKAGE ALToolKit AL_FUNCTION
    PkCompressTools( int level = 6,
                     int window_bits = AL_PK_DEFAULT_WINDOW_BITS,
                     int mem_level = AL_PK_DEFAULT_MEM_LEVEL );
AL_LINKAGE ALToolKit AL_FUNCTION PkDecompressTools();
AL_LINKAGE ALToolKit AL_FUNCTION
    PkCompressFileTools( int level = 6,
                         int window_bits = AL_PK_DEFAULT_WINDOW_BITS,
                         int mem_level = AL_PK_DEFAULT_MEM_LEVEL );
AL_LINKAGE ALToolKit AL_FUNCTION PkDecompressFileTools();
AL_LINKAGE ALToolKit AL_FUNCTION
    GlTools( short int compresssion_level = AL_GL_DEFAULT_LEVEL );
AL_LINKAGE ALToolKit AL_FUNCTION
    GlCompressTools( short int compression_level = AL_GL_DEFAULT_LEVEL );
AL_LINKAGE ALToolKit AL_FUNCTION
    GlDecompressTools();
AL_LINKAGE ALToolKit AL_FUNCTION
    GlCompressFileTools( short int compression_level = AL_GL_DEFAULT_LEVEL );
AL_LINKAGE ALToolKit AL_FUNCTION
    GlDecompressFileTools();
AL_LINKAGE ALToolKit AL_FUNCTION
    CopyTools();

#else /* #if defined( __cplusplus ) ... */

AL_LINKAGE void AL_FUNCTION deleteALToolKit( hALToolKit this_object );

/*
 * C shortcuts for building entry lists with a given toolkit.
 */

AL_LINKAGE hALEntryList AL_FUNCTION newALListFullTools( hALMonitor monitor );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListPkTools( hALMonitor monitor,
                  int level,
                  int window_bits,
                  int mem_level );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListPkCompressTools( hALMonitor monitor,
                          int level,
                          int window_bits,
                          int mem_level );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListPkCompressFileTools( hALMonitor monitor,
                              int level,
                              int window_bits,
                              int mem_level );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListPkDecompressTools( hALMonitor monitor );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListPkDecompressFileTools( hALMonitor monitor );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListGlTools( hALMonitor monitor, short int_compression_level );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListGlCompressTools( hALMonitor monitor, short int_compression_level );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListGlCompressFileTools( hALMonitor monitor, short int_compression_level );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListGlDecompressTools( hALMonitor monitor );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListGlDecompressFileTools( hALMonitor monitor );
AL_LINKAGE hALEntryList AL_FUNCTION
newALListCopyTools( hALMonitor monitor );

#endif
#endif /* #if !defined( _TOOLKIT_H ) */
