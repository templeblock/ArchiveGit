/*
 * FILEATTR.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the class definition for ALFileAttributes.
 *
 * CLASS DEFINITIONS:
 *
 *  ALFileAttributes
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A   : First release
 *
 *   February 14, 1996  2.0A : New release
 */

#ifndef _FILEATTR_H
#define _FILEATTR_H

/*
 * class ALFileAttributes
 *
 * DESCRIPTION
 *
 * This class is used to carry around file attributes.  Its only
 * real job at this time is to sit in the ALStorage class
 * and then produce packed attributes for inclusion in an archive,
 * and vice versa.  We will need to add some additional members
 * here for searching archives based on certain attribute criteria.
 *
 * Note that most of the classes in Archive library deliberately
 * withhold the copy constructor and assignment operator.  In this case,
 * however, the compiler is able to generate an adequate version of
 * these functions, so they aren't disabled.
 *
 * DATA MEMBERS
 *
 *  miReadOnly  : The read only file attribute bit, set when the file
 *                is opened or when the directory is read from the archive.
 *
 *  miSystem    : The system file attribute bit.
 *
 *  miHidden    : The hidden file attribute bit.
 *
 *  miArchive   : The archive (backup) file attribute bit.
 *
 * miDirectory  : The directory file attribute bit.
 *
 * MEMBER FUNCTIONS
 *
 *  ALFileAttributes()          : The constructor, doesn't have to do much.
 *  ~ALFileAttributes()         : The destructor, doesn't have to do anything.
 *  operator new( size_t size ) : Memory allocation operator, only used if
 *                                the library is inside a DLL.
 *  PackedAttributes()          : Returns the bits packed into an integer
 *                                in ArchiveLib proprietary format.
 *  SetFromPackedAttributes()   : Sets the member bits using as input a
 *                                short int in proprietary ArchiveLib format.
 *
 *  ReadOnly()                  : Is the Read Only attribute bit set?
 *  System()                    : Is the system attribute bit set?
 *  Hidden()                    : Is the Hidden attribute set?
 *  Archive()                   : Is the Archive (backup) bit set?
 *  Directory()                 : Is the directory attribute bit set?
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

#if defined( __cplusplus )

class AL_CLASS_TYPE ALFileAttributes {  /* Tag public class */
/*
 * Constructors, destructors, declarations, and friends
 */
    friend class AL_CLASS_TYPE ALFile;

    public :
        AL_PROTO ALFileAttributes();
        AL_PROTO ~ALFileAttributes();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * Member functions
 */
    public :
        short unsigned int AL_PROTO PackedAttributes();
        void AL_PROTO SetFromPackedAttributes( short int attributes );
/*
 * Access functions, all inline.  These provide readonly access to the
 * individual bits.  I need to add some support for C programmers for
 * these functions as well.
 */
        int AL_PROTO ReadOnly();
        int AL_PROTO System();
        int AL_PROTO Hidden();
        int AL_PROTO Archive();
        int AL_PROTO Directory();
/*
 * All of these guys are going to be undocumented for the time being.
 * If we ever add support for UNIX, these additional bits might
 * be useful.
 */
        int AL_PROTO UnixBitsPresent();
        int AL_PROTO UserRead();
        int AL_PROTO UserWrite();
        int AL_PROTO UserExecute();
        int AL_PROTO OtherRead();
        int AL_PROTO OtherWrite();
        int AL_PROTO OtherExecute();
        int AL_PROTO GroupRead();
        int AL_PROTO GroupWrite();
        int AL_PROTO GroupExecute();
/*
 * Data members
 */
    protected :
/*
 * The DOS attributes.
 */
        int miReadOnly        : 1;
        int miSystem          : 1;
        int miHidden          : 1;
        int miArchive         : 1;
/*
 * Additional attributes that might be used for UNIX files.  The
 * DOS bits are applied to the UNIX user settings.
 */
        int miUnixBitsPresent : 1;
        int miUserRead        : 1;
        int miUserWrite       : 1;
        int miUserExecute     : 1;
        int miOtherRead       : 1;
        int miOtherWrite      : 1;
        int miOtherExecute    : 1;
        int miGroupRead       : 1;
        int miGroupWrite      : 1;
        int miGroupExecute    : 1;
/*
 * The directory attribute is common to UNIX and DOS
 */
        int miDirectory       : 1;
};

#include "fileattr.inl"

#else /* #if defined( __cplusplus ) ... */

AL_LINKAGE int AL_FUNCTION ALStorageReadOnly( hALStorage storage );
AL_LINKAGE int AL_FUNCTION ALStorageSystem( hALStorage storage );
AL_LINKAGE int AL_FUNCTION ALStorageHidden( hALStorage storage );
AL_LINKAGE int AL_FUNCTION ALStorageArchive( hALStorage storage );
AL_LINKAGE int AL_FUNCTION ALStorageDirectory( hALStorage storage );
AL_LINKAGE void AL_FUNCTION
ALStorageSetFromPackedAtts( hALStorage this_object,
                            unsigned short int packed_attributes );
AL_LINKAGE unsigned short int AL_FUNCTION
ALStoragePackedAttributes( hALStorage this_object );

#endif /* #if defined( __cplusplus ) ... #else ... */

/*
 * Masks for accessing this stuff via the packed attributes byte.
 * This isn't a nested enum, mostly because it cuts back on typing.
 * This might change later.
 */
enum ALPackedAttributeBits {  /* Tag public type */
    ATTR_READ_ONLY          = 1,
    ATTR_SYSTEM             = 2,
    ATTR_HIDDEN             = 4,
    ATTR_ARCHIVE            = 8,
    ATTR_UNIX_BITS_PRESENT  = 16,
    ATTR_USER_READ          = 32,
    ATTR_USER_WRITE         = 64,
    ATTR_USER_EXECUTE       = 128,
    ATTR_OTHER_READ         = 256,
    ATTR_OTHER_WRITE        = 512,
    ATTR_OTHER_EXECUTE      = 1024,
    ATTR_GROUP_READ         = 2048,
    ATTR_GROUP_WRITE        = 4096,
    ATTR_GROUP_EXECUTE      = 8192,
    ATTR_DIRECTORY          = 16384,
    ATTR_LABEL              = (int) 0x8000
};

#endif /* #ifndef _FILEATTR_H */
