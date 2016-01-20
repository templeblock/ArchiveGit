/*
 * ARCENTRY.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 * This header file contains the definitions for class ALEntry.
 * ALEntry objects contain the information needed to insert or
 * extract an object from an archive, such as its name, size, etc.
 * The entries are gathered together into an ALEntryList, which is
 * typically passed as an argument to an archiving function.
 *
 * In addition to the information about a file that is either in an
 * archive or is going to go into an archive, the job entry also
 * has a special mark.  Each job entry is created in the marked state.
 * All of the archive functions that accept a list as an argument only
 * work on marked files, they ignore items in the list that aren't marked.
 * Various member functions can be used to clear or set marks on files.
 * There are several criteria you might use to clear or set marks,
 * such as matching a wild card specification, being older than a
 * certain date, or being a certain size.
 *
 * CLASS DEFINITIONS:
 *
 *  ALEntry             : A description of an entry in an Archive.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _ARCENTRY_H
#define _ARCENTRY_H

#if defined( __cplusplus )

/*
 * Forward declarations.
 */

class AL_CLASS_TYPE ALEntryList;

/*
 * class ALEntry
 *
 * DESCRIPTION
 *
 *  ALEntry objects describe an entry in an archive.  When you read in
 *  the directory from an archive, it consists of a list of ALEntry
 *  objects.  The description can also refer to objects that you
 *  want to put in an archive.
 *
 *  An ALEntry object has pointers to both a compression engine and
 *  a storage object.  It also has the position of an object in an archive,
 *  its CRC-32, and more.  Some of this data will not be filled in when
 *  you pass a list of these objects as an argument to an Archive command
 *  like Create().
 *
 *
 * DATA MEMBERS
 *
 *  mszComment             : The comment stored with the archive.
 *
 *  mpNextItem             : A pointer to the next ALEntry object in
 *                           the list.  (Note that ALEntry items are
 *                           always in a list.)
 *
 *  mpPreviousItem         : A pointer to the previous item in the list.
 *
 *  mrList                 : A reference to the list that the ALEntry
 *                           guy is a member of.
 *
 *  mlCompressedSize       : How big the object is after it is compressed.
 *                           If you are inserting an object for the first
 *                           time, you will have to wait for the ALArchiveBase
 *                           member function to fill this guy in after
 *                           the insertion takes place.
 *
 *  mlCompressedObjectPosition : Where the object is found in the archive.
 *                               Another field that gets filled in during
 *                               insertion.
 *
 *  mlCrc32                 : The CRC-32 of the uncompressed object.  This
 *                            gets filled in during compression.
 *
 *  miMark                  : The object's mark.  If the mark is not set,
 *                            most of the archive commands will ignore
 *                            this entry.
 *
 *
 * mpStorageObject          : A pointer to the storage object associated
 *                            with this entry.  This is a public member,
 *                            so you can dink with it.  It will be destroyed
 *                            by the ALEntry destructor!
 *
 * mpCompressor             : A pointer to the compression engine associated
 *                            with this archive entry.
 *
 * mpDecompressor           : A pointer to the decompression engine associated
 *                            with this archive entry.  Note that either of
 *                            the two engine pointers can be set to 0 if
 *                            the program doesn't need them.
 *
 * MEMBER FUNCTIONS
 *
 * ALEntry()             : The constructor.
 * ~ALEntry()            : The destructor.
 * operator new()        : Memory allocation operator, only used when the
 *                         constructor is inside the DLL.
 * InsertBefore()        : A private function used when updating an
 *                         ALEntrylist
 * GetNextEntry()        : A routine used when interating an ALEntrylist.
 * GetCompressedSize()   : An access routine to get a protected member.
 * GetCompressedObjectPosition() : Ditto.
 * GetCrc32()            : An access routine to get a protected member.
 * GetComment()          : An access routine to get a protected member.
 * SetMark()             : Set the mark for an ALEntry, the default state is set.
 * ClearMark()           : Clear the mark for an ALEntry.
 * SetMarkState()        : Private function to set or clear the mark.
 * SetComment()          : Set the comment for an entry.
 * GetMark()             : Get the current state of the mark.
 * CompressionRatio()    : Calculate the compression ratio for an object.
 * Duplicate()           : Test to see if an entry is found in a list.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 */
struct AL_CLASS_TYPE ALZipDir;

class AL_CLASS_TYPE ALEntry {  /* Tag public class */
/*
 * Constructors, destructors, friends
 */
    friend class AL_CLASS_TYPE ALArchive;
    friend class AL_CLASS_TYPE ALPkArchive;
    friend class AL_CLASS_TYPE ALGlArchive;
    friend void AL_FUNCTION _UpdateEntry( ALEntry AL_DLL_FAR * entry,
                                          ALZipDir AL_DLL_FAR * z );
    public :
        AL_PROTO ALEntry( ALEntryList AL_DLL_FAR &,
                          ALStorage AL_DLL_FAR *,
                          ALCompressor AL_DLL_FAR *,
                          ALDecompressor AL_DLL_FAR * );
        AL_PROTO ~ALEntry();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator are not supported.  I
 * declare them here because I don't want the compiler to generate
 * default versions for me.
 */
    protected :
        AL_PROTO operator=( ALEntry AL_DLL_FAR & );
        AL_PROTO ALEntry( ALEntry AL_DLL_FAR & );
/*
 * Member functions
 */
    protected :
        void AL_PROTO InsertBefore( ALEntry AL_DLL_FAR & );

    public :
        ALEntry AL_DLL_FAR * AL_PROTO GetNextEntry();
        long AL_PROTO GetCompressedSize() const;
        long AL_PROTO GetCompressedObjectPosition() const;
        long AL_PROTO GetCrc32() const;
        const char AL_DLL_FAR * AL_PROTO GetComment();
        void AL_PROTO SetMark();
        void AL_PROTO ClearMark();
        void AL_PROTO SetMarkState( short int new_state );
        int AL_PROTO SetComment( const char AL_DLL_FAR *comment );
        int AL_PROTO GetMark();
        int AL_PROTO CompressionRatio();
        int AL_PROTO Duplicate( ALEntryList AL_DLL_FAR &list );
/*
 * Data members
 */
    protected :
        char AL_DLL_FAR *mszComment;
        ALEntry AL_DLL_FAR *mpNextItem;
        ALEntry AL_DLL_FAR *mpPreviousItem;
        ALEntryList AL_DLL_FAR &mrList;
        long mlCompressedSize;
        long mlCompressedObjectPosition;
        long mlCrc32;
        short int miMark;

    public :
        ALStorage AL_DLL_FAR *mpStorageObject;
        ALCompressor AL_DLL_FAR *mpCompressor;
        ALDecompressor AL_DLL_FAR *mpDecompressor;
        AL_CLASS_TAG( _ALEntryTag );
};

#include "arcentry.inl"

#else /* #if defined( __cplusplus ) ... */

AL_LINKAGE long AL_FUNCTION ALEntryGetCompressedSize( hALEntry this_object );
AL_LINKAGE long AL_FUNCTION ALEntryGetCompressedPosition( hALEntry this_object );
AL_LINKAGE long AL_FUNCTION ALEntryGetCrc32( hALEntry this_object );
AL_LINKAGE char *AL_FUNCTION ALEntryGetComment( hALEntry this_object );
AL_LINKAGE int AL_FUNCTION ALEntryGetMark( hALEntry this_object );
AL_LINKAGE void AL_FUNCTION ALEntrySetMark( hALEntry this_object );
AL_LINKAGE void AL_FUNCTION ALEntryClearMark( hALEntry this_object );
AL_LINKAGE void AL_FUNCTION ALEntrySetMarkState( hALEntry this_object, short int new_state );
AL_LINKAGE hALEntry AL_FUNCTION newALEntry( hALEntryList list,
                                            hALStorage storage,
                                            hALCompressor compressor,
                                            hALDecompressor decompressor );
AL_LINKAGE void AL_FUNCTION deleteALEntry( hALEntry this_object );
AL_LINKAGE hALEntry AL_FUNCTION ALEntryGetNextEntry( hALEntry this_object );
AL_LINKAGE int AL_FUNCTION ALEntryCompressionRatio( hALEntry this_object );
AL_LINKAGE int AL_FUNCTION ALEntryDuplicate( hALEntry this_object,
                                             hALEntryList list );
AL_LINKAGE int AL_FUNCTION ALEntrySetComment( hALEntry this_object,
                                              char AL_DLL_FAR *comment );
AL_LINKAGE hALStorage AL_FUNCTION ALEntryGetStorage( hALEntry this_object );
AL_LINKAGE void AL_FUNCTION
ALEntrySetStorage( hALEntry this_object, hALStorage storage );
AL_LINKAGE hALCompressor AL_FUNCTION
ALEntryGetCompressor( hALEntry this_object );
AL_LINKAGE hALDecompressor AL_FUNCTION
ALEntryGetDecompressor( hALEntry this_object );
AL_LINKAGE void AL_FUNCTION
ALEntrySetCompressor( hALEntry this_object, hALCompressor compressor );
AL_LINKAGE void AL_FUNCTION
ALEntrySetDecompressor( hALEntry this_object, hALDecompressor decompressor );

#endif /* #if defined( __cplusplus ) ... #else ... */

#endif /* #ifdef _ARCENTRY_H         */

