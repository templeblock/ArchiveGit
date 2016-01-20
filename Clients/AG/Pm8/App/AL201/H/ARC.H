/*
 * ARC.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 * This header file contains the definitions for the base class
 * ALArchive.
 *
 * CLASS DEFINITIONS:
 *
 *  ALArchive           : The base class definition for an Archive.
 *                        Both PK format and Greenleaf format archives
 *                        are derived from ALArchive.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 */

#ifndef _ARC_H
#define _ARC_H

#if defined( __cplusplus )

/*
 * class ALArchive
 *
 * DESCRIPTION
 *
 *  This is the base class for both types of Archives supported
 *  by ArchiveLib.  As much functionality as possible is kept in
 *  the base class.  Ideally, this should mean that you can switch
 *  archive types with just a single line of code in your program.
 *
 *
 * DATA MEMBERS
 *
 *  mpArchiveStorageObject  : A pointer to the storage object that the
 *                            archive resides in.
 *
 *  mComment                : The commetn associated with the archive.
 *
 *  mlDirectoryOffset       : The offset of the archive directory in the
 *                            storage object.  Not valid until some operation
 *                            that reads or writes the directory takes place.
 *
 *  miVersion               : The archive version.  Read or written to the file.
 *
 *  miCount                 : The count of objects in the directory.  Once
 *                            again, only valid after some operation that
 *                            reads or writes the directory.
 *
 *  mfDeleteStorageObject   : If this flag is true, the C++ destructor for
 *                            the storage object is called from the archive's
 *                            destructor.
 *
 *  mfStripPathOnInsert     : If this member is set, the path names of the
 *                            input objects will be stripped as they are
 *                            written to the directory.
 *
 *  mfStripPathOnExtract    : If this member is set, the path names of the
 *                            internal objects will be stripped as they are
 *                            extracted from the archive.
 *
 *  mStatus                 :  The status object that most objects in this
 *                             library have.
 *
 * MEMBER FUNCTIONS
 *
 *  ALArchive()      : The constructor, which requires a storage
 *                     obect as an argument.
 *
 *  CalculateJobSize() : This function is called before major archive
 *                       operations.  Knowing the total job size lets
 *                       the monitor functions update progress bars
 *                       with real numbers.
 *
 *  CalculateCompressedJobSize() :  Ditto
 *
 *  ScanStatus         : This protected function is used to check on
 *                       operation status after public operations/
 *
 *  PreCreate()        : The base class calls this virtual function before
 *                       creating a new archive.
 *
 *  PostCreate()       : The base class calls this virtual function after
 *                       creating a new archive.
 *
 *  PreCompress()      : The base class calls this virtual function before
 *                       compressing an object and placing it into the
 *                       archive.
 *
 *  PostCompress()     : The base class calls this virtual function after
 *                       compressing an object and placing it into the
 *                       archive.
 *
 *  PreDecompress()    : The base class calls this virtual function before
 *                       decompressing an object and extracting it from the
 *                       archive.
 *
 *  PostDecompress()   : The base class calls this virtual function after
 *                       decompressing an object and extracting it from the
 *                       archive.
 *
 *  PreWriteDir()      : The base class calls this virtual function just
 *                       before writing the directory.
 *
 *  WriteDirEntry()    : This virtual function writes out a single directory
 *                       entry.
 *
 *  PostWriteDir()     : The base class calls this virtual function just
 *                       after writing the directory.
 *
 *  PreCopyInput()     : The base class calls this virtual function before
 *                       copying a compressed object from one archive to another.
 *
 *  PostCopyInput()    : The base class calls this virtual function after
 *                       copying a compressed object from one archive to another.
 *
 *  CompressJobs()     : This base class function is called to compress a
 *                       batch of files.  The base class tries to do the
 *                       work with the help of a few virtual functions.
 *
 *  CopyJobs()         : This base class function is called to copy a
 *                       batch of files from one archive to another.  The
 *                       base class tries to do most of the work with
 *                       the help of a few virtual functions.
 *
 *  WriteArchiveData() : Derived classes can override this function in order
 *                       to write class specific data out to an archive.
 *                       Not a PKZIP archive, thought.
 *
 *  ReadArchiveData()  : Ditto.
 *
 *  Create()           : The two versions of this function are part of the
 *                       public interface.  They create a new archive given
 *                       a list of input objects.
 *
 *  Append()           : Two versions of this, just like Create().  They are
 *                       the public functions used to add new objects to
 *                       an existing archive.
 *
 *  Extract()          : The public API function used to extract objects from
 *                       an archive.
 *
 *  Delete()           : The public API function used to delete objects
 *                       from an archive.
 *
 *  SetComment()       : The base class function that sets the comment for
 *                       an archive.  Doesn't write it out to the file,
 *                       however.
 *
 *  ReadDirectory()    : This virtual function reads the directory from an
 *                       archive into an ALEntryList.
 *
 *  WriteDirectory()   : This function writes the directory back out to an
 *                       archive, using a couple of virtual functions.
 *
 *  GetVersion()       : Returns a copy of the version.
 *
 *  GetStorageObject() : Returns a copy of the storage object where the
 *                       archive resides.
 *
 *  FillListBox()      : This user interface function is used to dumpp the
 *                       contents of an archive out to a list box, using
 *                       either OS/2 or Windows functions.
 *
 *  ClearError()       : Clears the error from the mStatus member.
 *
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : New release
 *
 */

class AL_CLASS_TYPE ALArchive {  /* Tag public class */
/*
 * Constructors, destructors, declarations, and friends
 */
    protected :
    public :
        AL_PROTO ALArchive( ALStorage AL_DLL_FAR *, short int delete_in_dtor );
        virtual AL_PROTO ~ALArchive();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif

/* The copy constructor and assignment operator do not exist. */
    protected :
        AL_PROTO ALArchive( const ALArchive AL_DLL_FAR & );
        ALArchive AL_DLL_FAR & AL_PROTO operator=( ALArchive AL_DLL_FAR &rhs );
/*
 * Member functions
 */
    protected :
        long AL_PROTO CalculateJobSize( ALEntryList AL_DLL_FAR &list );
        long AL_PROTO CalculateCompressedJobSize( ALEntryList AL_DLL_FAR &list );
        void AL_PROTO ScanStatus( ALEntryList AL_DLL_FAR &list );
/*
 * The virtual functions used by the base class to implement
 * the high level functions
 */
        virtual void AL_INLINE_PROTO PreCreate(){;}
        virtual void AL_INLINE_PROTO PostCreate(){;}

        virtual void AL_INLINE_PROTO PreCompress( ALEntry AL_DLL_FAR & /* entry */ ){;}
        virtual void AL_INLINE_PROTO PostCompress( ALEntry AL_DLL_FAR & /* entry */ ){;}

        virtual void AL_INLINE_PROTO PreDecompress( ALEntry AL_DLL_FAR & /* entry */ ){;}
        virtual void AL_INLINE_PROTO PostDecompress( ALEntry AL_DLL_FAR & /* entry */ ){;}

        virtual void AL_INLINE_PROTO PreWriteDir(){;}
        virtual void AL_INLINE_PROTO WriteDirEntry( ALEntry AL_DLL_FAR &entry ) = 0;
        virtual void AL_INLINE_PROTO PostWriteDir(){;}

        virtual void AL_INLINE_PROTO PreCopyInput( ALEntry AL_DLL_FAR & /* entry */ ){;}
        virtual void AL_INLINE_PROTO PostCopyInput( ALEntry AL_DLL_FAR & /* entry */ ){;}

        int AL_PROTO CompressJobs( ALEntryList AL_DLL_FAR &list );
        int AL_PROTO CopyJobs( ALArchive AL_DLL_FAR &source_archive,
                               ALEntryList AL_DLL_FAR &source_list );
    protected :
        virtual int AL_PROTO WriteArchiveData();
        virtual int AL_PROTO ReadArchiveData();
/*
 * These functions provide the public API to an archive
 */
    public :
        int AL_PROTO Create( ALEntryList AL_DLL_FAR &list );
        int AL_PROTO Create( ALArchive AL_DLL_FAR &source_archive,
                             ALEntryList AL_DLL_FAR &source_list );
/*
 * This used to be a virtual function, but I'm moving it
 * to the base class
 */
        int AL_PROTO Append( ALEntryList AL_DLL_FAR &list );
        int AL_PROTO Append( ALArchive AL_DLL_FAR &source_archive,
                             ALEntryList AL_DLL_FAR &source_list );
        int AL_PROTO Extract( ALEntryList AL_DLL_FAR &list );
        int AL_PROTO Delete( ALEntryList AL_DLL_FAR &list,
                             ALArchive AL_DLL_FAR &output_archive );
        const char AL_DLL_FAR * AL_PROTO GetComment();
        int AL_PROTO SetComment( char AL_DLL_FAR *comment );
        virtual int AL_PROTO ReadDirectory( ALEntryList AL_DLL_FAR &list ) = 0;
        int AL_PROTO WriteDirectory( ALEntryList AL_DLL_FAR &list );
        short AL_PROTO GetVersion();
        ALStorage AL_DLL_FAR * AL_PROTO GetStorageObject();
#if defined( AL_WINDOWS ) || defined( AL_OS2 )
        int AL_PROTO FillListBox( HWND hWnd, int list_box_id = -1 );
#endif
        void AL_PROTO ClearError();
/*
 * Data members
 */
    protected :
        ALStorage AL_DLL_FAR *mpArchiveStorageObject;
        ALName mComment;
        long mlDirectoryOffset;
        short miVersion;
        int miCount;
        const int mfDeleteStorageObject : 1;

    public :
        int mfStripPathOnInsert : 1;
        int mfStripPathOnExtract : 1;
        ALStatus mStatus;
        AL_CLASS_TAG( _ALArchiveTag );
};

#include "arc.inl"

#else /* #if defined( __cplusplus ) */

AL_LINKAGE void AL_FUNCTION ALArchiveClearError( hALArchive this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALArchiveGetComment( hALArchive this_object );
AL_LINKAGE int AL_FUNCTION ALArchiveGetVersion( hALArchive this_object );
AL_LINKAGE hALStorage AL_FUNCTION ALArchiveGetStorage( hALArchive this_object );
AL_LINKAGE void AL_FUNCTION deleteALArchive( hALArchive this_object );
AL_LINKAGE int AL_FUNCTION ALArchiveAppend( hALArchive this_object, hALEntryList list );
AL_LINKAGE int AL_FUNCTION
ALArchiveAppendFromArchive( hALArchive this_object,
                            hALArchive input_archive,
                            hALEntryList list );
AL_LINKAGE int AL_FUNCTION ALArchiveCreate( hALArchive this_object,
                                            hALEntryList list );
AL_LINKAGE int AL_FUNCTION
ALArchiveCreateFromArchive( hALArchive this_object,
                            hALArchive input_archive,
                            hALEntryList list );
AL_LINKAGE int AL_FUNCTION ALArchiveDelete( hALArchive this_object,
                                            hALEntryList list,
                                            hALArchive output_archive );
AL_LINKAGE int AL_FUNCTION
ALArchiveExtract( hALArchive this_object, hALEntryList list );
AL_LINKAGE int AL_FUNCTION ALArchiveSetComment( hALArchive this_object,
                                                char AL_DLL_FAR *comment );
AL_LINKAGE int AL_FUNCTION ALArchiveWriteDirectory( hALArchive this_object,
                                                    hALEntryList list );
AL_LINKAGE int AL_FUNCTION
ALArchiveReadDirectory( hALArchive this_object, hALEntryList list );
AL_LINKAGE int AL_FUNCTION
ALArchiveSetError( hALArchive this_object,
                   int error,
                   char AL_DLL_FAR *text );
AL_LINKAGE int AL_FUNCTION
ALArchiveGetStatusCode( hALArchive this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALArchiveGetStatusString( hALArchive this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALArchiveGetStatusDetail( hALArchive this_object );

#if defined( AL_OS2 ) || defined( AL_WINDOWS )

AL_LINKAGE int AL_FUNCTION
ALArchiveFillListBox( hALArchive this_object,
                      HWND hWnd,
                      int list_box_id );
AL_LINKAGE void AL_FUNCTION
ALArchiveSetStripOnInsert( hALArchive this_object, int flag );
AL_LINKAGE void AL_FUNCTION
ALArchiveSetStripOnExtract( hALArchive this_object, int flag );

#endif

#endif /* #if defined( __cplusplus ) */

#endif /* #ifndef _ARC_H         */
