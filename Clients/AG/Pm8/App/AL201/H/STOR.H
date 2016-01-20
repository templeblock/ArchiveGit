/*
 * STOR.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the class declaration for the very important
 *  base class ALStorage.
 *
 * CLASS DEFINITIONS:
 *
 *  ALStorage
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A   : First release
 *
 *  August 10, 1994 1.0B : Added some copyright info.
 *
 *  January 1, 1995 1.01A : Renamed this file to prevent a clash with
 *                          STORAGE.H used by compiler vendors for
 *                          OLE implementation. :(
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _STOR_H
#define _STOR_H


#include <stddef.h>   /* need for size_t */

#include "timedate.h"
#include "fileattr.h"

#if defined( __cplusplus )

/*
 * class ALStorage
 *
 * DESCRIPTION
 *
 * ALStorage is a base class that defines the different types
 * of storage objects used by Archive Library.  The two most
 * commonly used storage object types are file objects and memory
 * objects, defined by the derived classes ALFile and
 * ALMemory.
 *
 * ALStorage objects are used to store and retrieve objects from archives.
 * They are also used to store and retrieve the archives themselves,
 * allowing archives to be stored in files or directly in memory.
 *
 * The ALStorage adds buffering to the storage object, allowing for
 * fast access to data presently cached in memory.  This is very similar to
 * the buffering provided for FILE types in stdio.h.  Note that this
 * buffering is generally only efficient/useful if lots of sequential
 * reads or writes are being done, as opposed to random accessess.
 *
 * ALStorage objects give up a lot of flexibility in order to provide
 * quick and efficient access to data.  The primary way this affects
 * use of the class is that the I/O buffer can only be used for reading
 * or writing, but not both simultaneously.  The class doesn't check
 * for this at run time, so programmers need to enforce it themselves.
 *
 * When a read is initiated for the first time, the buffer is loaded up,
 * and subsequent reads are performed out of the I/O buffer.  To switch
 * to writing mode, a call to FlushBuffer needs to be performed, which
 * will reset the input and output indices.  Likewise, when, done writing,
 * a call to FlushBuffer() can be performed to clear the indices.  A
 * read can be done subsequently.
 *
 * DATA MEMBERS
 *
 *  mpcBuffer            : This is the I/O buffer.  I read big blocks of
 *                         data into this buffer, then I can perform
 *                         character reads from an inline function that
 *                         doesn't have to access any virtual fns.  Speeds
 *                         things up tremendously.  Likewise, I write
 *                         to this buffer using inline functions until it
 *                         it is full.  Only then do I call a virtual
 *                         to flush it to disk, memory, or whatever.
 *
 *  muBufferValidData    : This keeps track of the end of valid data,
 *                         both when reading and writing.  When you
 *                         read in a block of data, this index is set
 *                         to the end of the data.  When writing, this
 *                         index is continually updated to reflect the
 *                         end of the user written data.
 *
 *  muWriteIndex         : The index in the I/O buffer where the next byte
 *                         is going to be written.
 *
 *  muReadIndex          : The index in the I/O buffer where the next read
 *                         will come from.
 *
 *  mlFilePointer        : The current location of the read/write pointer
 *                         in the underlying object, e.g. a file.  This
 *                         is the location where the data will be written
 *                         out of the I/O buffer when a FlushBuffer() call
 *                         is made.  Or, if reading, it is where the next
 *                         LoadBuffer() will read data from.
 *
 *  mlSize               : The size of the file/object.  This will ordinarily
 *                         be set to -1 when we create an object, because
 *                         we don't know the size yet.  When you call Open()
 *                         for an existing object, the value will usually
 *                         be loaded using some sort of system call.  We
 *                         also can figure out what the size is when we do
 *                         a ReadDirectory call on an archive.
 *
 *  mlCrc32              : The CRC-32 for the object.  This value normally
 *                         won't be known until an object has been placed
 *                         in an archive, or when the information has
 *                         been read out using in ReadDirectory().
 *
 *  miUpdateCrcFlag      : This flag is set to indicate that we are in the
 *                         process of calculating the CRC while the file
 *                         is being compressed.
 *
 *  miCreated            : This flag will be set if the file was opened
 *                         using Create(), clear if it was opened using
 *                         Open().  When miCreated is set, we will try
 *                         to set the file time, date and attributes when
 *                         we close the file.  This is so we can set these
 *                         attributes when we are recreating a file that
 *                         was stored in an archive.
 *
 *  miStorageObjectType  : An integer that is assigned when the object was
 *                         constructed.  Usually one of the enumerated
 *                         constants found in ALDEFS.H.  This is the number
 *                         that gets stored in the Archive directory with
 *                         the object, so we can figure out what type of
 *                         object to create when extracting.
 *
 *  muBufferSize         : The size of the I/O buffer.
 *
 *  mpMonitor            : A pointer to the monitor attached to this object.
 *                         During the archiving process, this pointer gets
 *                         set by the archive routine for each storage object
 *                         as it is being processes.  A value of 0 just
 *                         means no monitor is watching this object at the
 *                         moment.
 *
 *  mTimeDate            : The time and date stamp for the file, this usually
 *                         gets set when the object is opened using Open(),
 *                         it is also set when we read in a storage object's
 *                         information using ReadDirectory().
 *
 *  mAttributes          : The attributes associated with the file. R/H/S/A/D.
 *
 *  mName                : The name of the storage object.
 *
 *  mStatus              : The current status of the object.
 *
 * MEMBER FUNCTIONS
 *
 *  ALStorage()          : The constructor, creates the object, but doesn't
 *                         necessarily create the file/whatever.
 *  operator=()          : Assignment operator.
 *  operator new()       : The memory allocation operator.  This is only
 *                         used if the library is in a DLL.
 *  ~ALStorage()         : Virtual destructor.
 *  UpdateCrc()          : Protected function used internally when the
 *                         crc is being calculated
 *  WriteStorageObjectData() : Protected function to write custom data needed
 *                             for a particular derived classe.
 *  ReadStorageObjectData()  : Protected function read that data back in.
 *  ReadChar()           : Superfast inline function to read a bytee
 *  WriteChar()          : Fast inline function to write a byte.
 *  ReadBuffer()         : Function to read blocks of data.
 *  WriteBuffer()        : Function to write blocks of data.
 *  Open()               : Open() used to prepare an existing object for I/O.
 *  Create()             : Create a new underlying object for I/O.
 *  Close()              : Called when I/O is complete.
 *  LoadBuffer()         : Called to reload the I/O buffer, used internally
 *                         when ReadChar() runs out of stuff to read.
 *  FlushBuffer()        : Called to flush the I/O buffer to the underlying
 *                         object.  Called when WriteChar() has gone too far.
 *  Seek()               : Called to reposition the I/O pointer of the
 *                         underlying object.
 *  YieldTime()          : Called whenever a FlushBuffer() or LoadBuffer()
 *                         takes place.  Used to update the Monitor attached
 *                         to the file, and to yield time to the O/S.
 *  Compare()            : Compare two storage objects.
 *  InitCrc32()          : Called to start calculating the CRC for an object.
 *  WriteGlShort()       : Write 16 bit integer in little endian format.
 *  WritePkShort()       : Write 16 bit integer in big endian format.
 *  WriteGlLong()        : Write 32 bit integer in little endian format.
 *  WritePkLong()        : Write 32 bit integer in big endian format.
 *  ReadGlShort()        : Read 16 bit integer in little endian format.
 *  ReadPkShort()        : Read 16 bit integer in big endian format.
 *  ReadGlLong()         : Read 32 bit integer in little endian format.
 *  ReadPkLong()         : Read 32 bit integer in big endian format.
 *  ReadString()         : Read a string in ArchiveLib's proprietary format.
 *  WriteString()        : Write string in ArchiveLib format.
 *  Rename()             : Rename the underlying object.
 *  UnRename()           : Undo a rename operation.
 *  RenameToBackup()     : Rename to a special backup name.
 *  Delete()             : Delete an underlying storage object.
 *  GetCrc32()           : Return value of the CRC member.
 *  GetSize()            : Reeturn value of the size member.
 *  IsOpen()             : Indicate if the file is open.
 *  Tell()               : Indicate where the next read or write will
 *                         take place.
 *  ReadCopyright()      : A function to get at the Greenleaf Copyright.
 *  Clone()              : A function used to make a copy of an ALStorage
 *                         object.
 *  ClearError()         : A helper function that clears the error from
 *                         the mStatus member.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */


/*
 * Forward declaration
 */

class AL_CLASS_TYPE ALEntry;
class AL_CLASS_TYPE ALMonitor;
struct AL_CLASS_TYPE ALZipDir;

class AL_CLASS_TYPE ALStorage {  /* Tag public class */
    public :
/*
 * Classes I trust
 */
    friend class AL_CLASS_TYPE ALArchive;
    friend class AL_CLASS_TYPE ALPkArchive;
    friend class AL_CLASS_TYPE ALGlArchive;
    friend class AL_CLASS_TYPE ALCompressedObject;
    friend class AL_CLASS_TYPE ALPkCompressor;
    friend class AL_CLASS_TYPE ALPkDecompressor;
    friend void AL_FUNCTION _UpdateEntry( ALEntry AL_DLL_FAR * entry,
                                          ALZipDir AL_DLL_FAR * z );
/*
 * Constructors, destructors, assignment operator
 */
    protected :
        AL_PROTO ALStorage( const char AL_DLL_FAR *file_name,
                            size_t buffer_size,
                            const ALStorageType storage_type,
                            ALCase name_case = AL_MIXED );
        ALStorage AL_DLL_FAR & AL_PROTO operator=( const ALStorage AL_DLL_FAR & );
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
    public :
        virtual AL_PROTO ~ALStorage();
/*
 * I don't want to allow the copy constructor to exist.
 */
    protected :
        AL_PROTO ALStorage( const ALStorage AL_DLL_FAR & );

/*
 * Member functions, grouped somewhat
 *
 *
 * Private member manipulation
 */
    protected :
        void AL_PROTO UpdateCrc( size_t count );
/*
 * This is private, because it allocates memory in the DLL, so it
 * must be deleted in the DLL as well.
 */
    protected :
        virtual int AL_PROTO
        WriteStorageObjectData( ALStorage AL_DLL_FAR * archive );
        virtual int AL_PROTO
        ReadStorageObjectData( ALStorage AL_DLL_FAR * archive );
/*
 * The file I/O access public interface
 */
    public :
        int AL_PROTO ReadChar();
        int AL_PROTO WriteChar( int c );
        size_t AL_PROTO ReadBuffer( unsigned char AL_DLL_FAR *buffer,
                                    size_t length );
        /* Please keep this arg const, breaks WriteString o/w */
        size_t AL_PROTO WriteBuffer( const unsigned char AL_DLL_FAR *buffer,
                                     size_t length );
        virtual int AL_PROTO Open();
        virtual int AL_PROTO Create( long desired_size = -1L );
        virtual int AL_PROTO Close();
        virtual int AL_PROTO LoadBuffer( long address ) = 0;
        virtual int AL_PROTO FlushBuffer() = 0;
        virtual int AL_PROTO Seek( long address ) = 0;
        virtual void AL_PROTO YieldTime();
        int AL_PROTO Compare( ALStorage AL_DLL_FAR &test_object );
        void AL_PROTO InitCrc32( unsigned long seed = 0xffffffffl );
        int AL_PROTO WriteGlShort( short int short_data );
        int AL_PROTO WritePkShort( short int short_data );
        int AL_PROTO WriteGlLong( long long_data );
        int AL_PROTO WritePkLong( long long_data );
        int AL_PROTO ReadGlShort( short int AL_DLL_FAR &short_data );
        int AL_PROTO ReadPkShort( short int AL_DLL_FAR &short_data );
        int AL_PROTO ReadGlLong( long AL_DLL_FAR &long_data );
        int AL_PROTO ReadPkLong( long AL_DLL_FAR &long_data );
        int AL_PROTO ReadString( ALName AL_DLL_FAR &name, short int length = -1 );
        int AL_PROTO WriteString( const char AL_DLL_FAR *string_data );
/*
 * File manipulation public interface
 */
    public :
        virtual int AL_PROTO Rename( const char AL_DLL_FAR *new_name = 0,
                                     int delete_on_clash = 1 ) = 0;
        virtual int AL_PROTO UnRename( int delete_on_clash = 1 ) = 0;
        virtual int AL_PROTO RenameToBackup( int delete_on_clash = 1 ) = 0;
        virtual int AL_PROTO Delete() = 0;
/*
 * Access functions
 */
    public :
        long AL_PROTO GetCrc32();
        long AL_PROTO GetSize() const;
        int AL_PROTO IsOpen();
        long AL_PROTO Tell();
        static char AL_DLL_FAR * AL_PROTO ReadCopyright();
        virtual ALStorage AL_DLL_FAR * AL_PROTO Clone( const char AL_DLL_FAR *name,
                                                       int object_type ) const = 0;
        void AL_PROTO ClearError();
/*
 * Data members
 */
    protected :
        unsigned char AL_DLL_FAR *mpcBuffer;
        size_t muBufferValidData;
        size_t muWriteIndex;
        size_t muReadIndex;
        long mlFilePointer;
        long mlSize;
        long mlCrc32;
        short int miUpdateCrcFlag;
        short int miCreated;
/*
 * Public members
 */
    public :
        const ALStorageType miStorageObjectType;
        const size_t muBufferSize;
        ALMonitor AL_DLL_FAR *mpMonitor;
        ALTimeDate mTimeDate;
        ALFileAttributes mAttributes;
        ALName mName;
        ALStatus mStatus;
        AL_CLASS_TAG( _ALStorageTag );
};

#include "stor.inl"

#else  /* #if defined( __cplusplus ) ... */

AL_LINKAGE long AL_FUNCTION ALStorageGetSize( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageIsOpen( hALStorage this_object );
AL_LINKAGE void AL_FUNCTION ALStorageClearError( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageReadChar( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageWriteChar( hALStorage this_object, int c );
AL_LINKAGE void AL_FUNCTION deleteALStorage( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageOpen( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION
ALStorageCreate( hALStorage this_object, long desired_size );
AL_LINKAGE int AL_FUNCTION ALStorageClose( hALStorage this_object );
AL_LINKAGE size_t AL_FUNCTION
ALStorageReadBuffer( hALStorage this_object,
                     unsigned char AL_DLL_FAR *buffer,
                     size_t length );
 AL_LINKAGE size_t AL_FUNCTION
ALStorageWriteBuffer( hALStorage this_object,
                      unsigned char AL_DLL_FAR *buffer,
                      size_t length );
AL_LINKAGE int AL_FUNCTION
ALStorageWriteString( hALStorage this_object,
                      char AL_DLL_FAR *string );
AL_LINKAGE long AL_FUNCTION ALStorageTell( hALStorage this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION ALReadCopyright( void );
AL_LINKAGE int AL_FUNCTION
ALStorageCompare( hALStorage this_object, hALStorage test_object );
AL_LINKAGE long AL_FUNCTION ALStorageGetCrc32( hALStorage this_object );
AL_LINKAGE void AL_FUNCTION
ALStorageInitCrc32( hALStorage this_object, long seed );
AL_LINKAGE int AL_FUNCTION
ALStorageReadPkLong( hALStorage this_object, long AL_DLL_FAR *long_data );
AL_LINKAGE int AL_FUNCTION
ALStorageReadPkShort( hALStorage this_object, short AL_DLL_FAR *short_data );
AL_LINKAGE int AL_FUNCTION
ALStorageWritePkLong( hALStorage this_object, long long_data );
AL_LINKAGE int AL_FUNCTION
ALStorageWritePkShort( hALStorage this_object, short short_data );
AL_LINKAGE int AL_FUNCTION
ALStorageReadGlShort( hALStorage this_object, short int AL_DLL_FAR *short_data );
AL_LINKAGE int AL_FUNCTION
ALStorageReadGlLong( hALStorage this_object, long int AL_DLL_FAR *long_data );
AL_LINKAGE int AL_FUNCTION
ALStorageWriteGlLong( hALStorage this_object, long long_data );
AL_LINKAGE int AL_FUNCTION
ALStorageWriteGlShort( hALStorage this_object, short short_data );
AL_LINKAGE void AL_FUNCTION ALStorageSetMonitor( hALStorage this_object,
                                                 hALMonitor monitor );
AL_LINKAGE int AL_FUNCTION ALStorageDelete( hALStorage this_object );
AL_LINKAGE void AL_FUNCTION ALStorageSetName( hALStorage this_object,
                                              char AL_DLL_FAR *object_name );
AL_LINKAGE int AL_FUNCTION
ALStorageWildCardMatch( hALStorage this_object,
                        char AL_DLL_FAR *pattern );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageChangeExtension( hALStorage this_object,
                          char AL_DLL_FAR *new_extension );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageChangeTrailingChar( hALStorage this_object, char new_char );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageGetName( hALStorage this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageGetOldName( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION
ALStorageGetStatusCode( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageSetError( hALStorage this_object,
                                              int error,
                                              char AL_DLL_FAR *text );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageGetStatusString( hALStorage this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALStorageGetStatusDetail( hALStorage this_object );
AL_LINKAGE long AL_FUNCTION ALStorageGetUnixTime( hALStorage this_object );
AL_LINKAGE long AL_FUNCTION ALStorageToJulian( hALStorage this_object );

AL_LINKAGE void AL_FUNCTION
ALStorageFromJulian( hALStorage this_object, long jdn );
AL_LINKAGE void AL_FUNCTION
ALStorageSetTimeDateFromStruc( hALStorage this_object,
                               struct tm AL_DLL_FAR * time_struct );
AL_LINKAGE void AL_FUNCTION
ALStorageGetStrucFromTimeDate( hALStorage this_object,
                               struct tm AL_DLL_FAR *time_struct );
AL_LINKAGE void AL_FUNCTION
ALStorageSetTimeDateFromUnix( hALStorage this_object,long unix_time );
/*
 * Last minute additions, sorry, these 4 aren't in the manual!
 */
AL_LINKAGE void AL_FUNCTION
ALStorageGetTime( hALStorage this_object,
                  short int AL_DLL_FAR *hour,
                  short int AL_DLL_FAR *minute,
                  short int AL_DLL_FAR *second );
AL_LINKAGE void AL_FUNCTION
ALStorageSetTime( hALStorage this_object,
                  short int hour,
                  short int minute,
                  short int second );
AL_LINKAGE void AL_FUNCTION
ALStorageGetDate( hALStorage this_object,
                  short int AL_DLL_FAR *month,
                  short int AL_DLL_FAR *date,
                  short int AL_DLL_FAR *year );
AL_LINKAGE void AL_FUNCTION
ALStorageSetDate( hALStorage this_object,
                  short int month,
                  short int date,
                  short int year );

AL_LINKAGE int AL_FUNCTION ALStorageFlushBuffer( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION
ALStorageLoadBuffer( hALStorage this_object, long address );
AL_LINKAGE int AL_FUNCTION
ALStorageRename( hALStorage this_object,
                 char AL_DLL_FAR *new_name,
                 int delete_on_clash );
AL_LINKAGE int AL_FUNCTION
ALStorageRenameToBackup( hALStorage this_object, int delete_on_clash );
AL_LINKAGE int AL_FUNCTION ALStorageSeek( hALStorage this_object,
                                          long address );
AL_LINKAGE int AL_FUNCTION ALStorageUnRename( hALStorage this_object,
                                              int delete_on_clash );
AL_LINKAGE int AL_FUNCTION ALStorageGetType( hALStorage this_object );
AL_LINKAGE unsigned short int AL_FUNCTION
ALStorageGetDosTime( hALStorage this_object );
AL_LINKAGE unsigned short int AL_FUNCTION
ALStorageGetDosDate( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageValidTimeDate( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageReadOnly( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageSystem( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageHidden( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageArchive( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageDirectory( hALStorage this_object );
AL_LINKAGE long AL_FUNCTION ALStorageGetSize( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageIsOpen( hALStorage this_object );
AL_LINKAGE void AL_FUNCTION ALStorageClearError( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageReadChar( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION ALStorageWriteChar( hALStorage this_object, int c );

#endif /* #if defined( __cplusplus ) ... #else ...*/

#endif /* #ifndef _STOR_H         */
