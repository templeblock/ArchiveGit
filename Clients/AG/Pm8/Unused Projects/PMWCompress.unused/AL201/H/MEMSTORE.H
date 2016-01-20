/*
 * MEMSTORE.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This header file contains the class declaration for ALMemoryBase and
 *  its three descendants.
 *
 * CLASS DEFINITIONS:
 *
 *  ALMemoryBase
 *  ALMemory
 *  ALHugeMemory
 *  ALWinMemory
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : New release
 *
 */

#ifndef _MEMSTORE_H
#define _MEMSTORE_H

#include "arclib.h"

#if defined( __cplusplus )

/*
 * class ALMemoryBase : public ALStorage
 *
 * DESCRIPTION
 *
 *  Class ALMemoryBase is ALStorage class that serves as a parent
 *  class to three other classes.  The three derived classes store
 *  data in three different types of memory buffers:  standard buffers
 *  created from the heap, huge buffers created from the far heap, and
 *  windows memory blocks created using the Windows API.
 *
 *  ALMemoryBase doesn't know much about pointers on its own, so it
 *  has some pure virtual functions that have to get filled in by
 *  the derived classes.  They are the classes that you instantiate
 *  to create memory based storage objects.
 *
 * DATA MEMBERS
 *
 *  mfUserOwnsBuffer    : If this flag is set, it indicates that the user
 *                        owns the buffer, not the class.  This means
 *                        the class can't grow the buffer if it runs out
 *                        of space, and it can't delete it in the
 *                        ALMemoryBase destructor.
 *
 *  mlUserBufferSize    : The actual size of the buffer, whether it is
 *                        owned by the user or not.  This is a long always.
 *
 * MEMBER FUNCTIONS
 *
 *  ALMemoryBase()        : The constructor, only called by derived classes.
 *  ~ALMemoryBase()       : The virtual destructor.
 *  operator new()        : Memory allocation operator, only used when the
 *                          library is in a DLL.  Note that this isn't the
 *                          operator used to allocate the buffer, just the
 *                          one to allocate a class object.
 *  _LoadBuffer()         : A virtual function defined in the derived class.
 *                          When more data is needed from the memory buffer,
 *                          this class gets called.
 *  _FlushBuffer()        : Another virtual function that will be defined
 *                          in the derived class.  When ALMemoryBase decides
 *                          it needs to store some data, it has to call this
 *                          function to do the work.
 *  Open()                : Open the storage object for reading and writing.
 *  Create()              : Create a new buffer to write to.
 *  Close()               : Close the existing memory object.
 *  LoadBuffer()          : Load a new block from the memory object into
 *                          the I/O buffer. (Needs _LoadBuffer() to do the work.)
 *  FlushBuffer()         : Flush the contents of the I/O buffer, sending
 *                          the contents into the memory object.  (Needs to
 *                          call _FlushBuffer() to do the work).
 *  Seek()                : Seek to a new location in the memory object.
 *  Rename()              : Give the object a new name.  Names are pretty
 *                          irrelevant for memory objects, feel free to use
 *                          whatever you want here.
 *  UnRename()            : Restore the old name.
 *  Delete()              : Delete the memory object.  It is gone forever.
 *  RenameToBackup()      : Give the memory object an arbitrary new name.
 *  GrowUserBuffer()      : A private function used to give us more space
 *                          when the memory object is owner of the buffer.
 *                          This is pure virtual, it must be implemented
 *                          by a derived memory class.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0: New release
 */

class AL_CLASS_TYPE ALMemoryBase : public ALStorage {  /* Tag public class */
/*
 * Constructors, destructors, assignment operator, friends, declarations
 */

    public :
        AL_PROTO ALMemoryBase( const char AL_DLL_FAR *buffer_name = "",
                           ALCase name_case = AL_MIXED );
        virtual AL_PROTO ~ALMemoryBase();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * As usual, I don't want the compiler to generate a default copy constructor,
 * or an assignment operator here.  I force it to back off by declaring them
 * here.  They do not exist!
 */
    private   :
        AL_PROTO ALMemoryBase( ALMemoryBase AL_DLL_FAR & );
        ALMemoryBase AL_DLL_FAR & AL_PROTO operator=( const ALMemoryBase AL_DLL_FAR & );

/*
 * Member functions, grouped by category.
 *
 *
 * Protected member manipulation, used inside library, not for public use.
 */
    protected :
        virtual void AL_PROTO _LoadBuffer( long address ) = 0;
        virtual void AL_PROTO _FlushBuffer( long address ) = 0;
/*
 * The file I/O access public interface
 */
    public :
        virtual int AL_PROTO Open();
        virtual int AL_PROTO Create( long desired_size = -1L );
        virtual int AL_PROTO Close();
        virtual int AL_PROTO LoadBuffer( long address );
        virtual int AL_PROTO FlushBuffer();
        virtual int AL_PROTO Seek( long address );

/*
 * File name and underlying object manipulation public interface
 */
    public :
        virtual int AL_PROTO Rename( const char AL_DLL_FAR *new_name = 0,
                                     int delete_on_clash = 1 );
        virtual int AL_PROTO UnRename( int delete_on_clash = 1 );
        virtual int AL_PROTO Delete() = 0;
        virtual int AL_PROTO RenameToBackup( int delete_on_clash = 1 );
/*
 * Unique to this class
 */
    protected :
        virtual int AL_PROTO GrowUserBuffer( long minimum_new_size ) = 0;
/*
 * Data members
 */
    protected :
    public :   /* Should some of these might be better off private */
        int mfUserOwnsBuffer;
        long mlUserBufferSize;
        AL_CLASS_TAG( _ALMemoryBaseTag );
};

/*
 * class ALMemory : ALMemoryBase
 *
 * DESCRIPTION
 *
 *  Class ALMemory is an ALStorage class that stores its data in
 *  buffers allocated from the default heap.  This means that it
 *  has a pointer to a memory buffer using an unadorned pointer, and
 *  it can keep track of its length in a size_t integer.
 *
 *  It has to define all of the functions that were left as pure
 *  virtual in ALMemoryBase.  These functions are all the functions
 *  that actually manage or use the memory buffer.
 *
 * DATA MEMBERS
 *
 *  mpcUserBuffer       : A pointer to the big buffer that is going to
 *                        hold the data.  This is a buffer that gets
 *                        allocated with malloc() and realloc(), and
 *                        released using free().  Retro, but cool.
 *
 * MEMBER FUNCTIONS
 *
 *  ALMemory()            : The constructor.  This is where you decide
 *                          whether to let ALMemory allocate and manage
 *                          your buffer, or decide to use your own buffer.
 *  ~ALMemory()           : The virtual destructor.
 *  operator new()        : Memory allocation operator, only used when the
 *                          library is in a DLL.  Note that this isn't the
 *                          operator used to allocate the buffer, just the
 *                          one to allocate a class object.
 *  _LoadBuffer()         : A virtual function defined that loads more
 *                          data out of the memory buffer so it can
 *                          be stuffed into the I/O buffer.
 *  _FlushBuffer()        : Another virtual function that takes data out
 *                          of the I/O buffer and stuffs it into the
 *                          memory buffer.
 *  GrowUserBuffer()      : A private function used to give us more space
 *                          when the memory object is owner of the buffer.
 *                          This function tries to call realloc() to do
 *                          its work.  The presence of realloc() accounts
 *                          for the reason we use free() and malloc() instead
 *                          of new and delete.
 *  Clone()               : If you place an ALMemory object in a toolkit,
 *                          this function can be called to create a new
 *                          ALMemory object.
 *  Open()                : Open the storage object for reading and writing.
 *  Create()              : Create a new buffer to write to.
 *  Delete()              : Delete the memory object.  It is gone forever.
 *  Close()               : Close the existing memory object.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0: New release
 */

class AL_CLASS_TYPE ALMemory : public ALMemoryBase {  /* Tag public class */
/*
 * Constructors, destructors, assignment operator, friends, declarations
 */
    public :
        AL_PROTO ALMemory( const char AL_DLL_FAR *buffer_name = "",
                           char AL_DLL_FAR *user_buffer = 0,
                           size_t user_buffer_size = 0,
                           ALCase name_case = AL_MIXED );
        virtual AL_PROTO ~ALMemory();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * As usual, I don't want the compiler to generate a default copy constructor,
 * or an assignment operator here.  I force it to back off by declaring them
 * here.  They do not exist!
 */
    protected :
        AL_PROTO ALMemory( ALMemory AL_DLL_FAR & );
        ALMemory AL_DLL_FAR & AL_PROTO operator=( const ALMemory AL_DLL_FAR & );
    protected :
        virtual void AL_PROTO _LoadBuffer( long address );
        virtual void AL_PROTO _FlushBuffer( long address );
/*
 * Unique to this class
 */
    protected :
        virtual int AL_PROTO GrowUserBuffer( long minimum_new_size );
    public :
        virtual ALStorage AL_DLL_FAR * AL_PROTO Clone( const char AL_DLL_FAR *name,
                                            int object_type ) const;
        virtual int AL_PROTO Open();
        virtual int AL_PROTO Create( long desired_size = -1L );
        virtual int AL_PROTO Delete();
        virtual int AL_PROTO Close();
        char AL_DLL_FAR *mpcUserBuffer;
        AL_CLASS_TAG( _ALMemoryTag );
};

/*
 * class ALHugeMemory : ALMemoryBase
 *
 * DESCRIPTION
 *
 *  Class ALHugeMemory is an ALStorage class that stores its data in
 *  huge buffers allocated from the far heap.  If you are writing
 *  16 bit programs under MS-DOS, this gives you a way to create
 *  memory based storage objects that can span more than 64Kbytes.
 *
 * DATA MEMBERS
 *
 *  mpcUserBuffer       : A pointer to the huge buffer that is going to
 *                        hold the data.  This is a buffer that gets
 *                        allocated with halloc() or faralloc().
 *
 * MEMBER FUNCTIONS
 *
 *  ALHugeMemory()        : The constructor.  This is where you decide
 *                          whether to let ALHugeMemory allocate and manage
 *                          your buffer, or decide to use your own buffer.
 *  ~ALHugeMemory()       : The virtual destructor.
 *  operator new()        : Memory allocation operator, only used when the
 *                          library is in a DLL.  Note that this isn't the
 *                          operator used to allocate the buffer, just the
 *                          one to allocate a class object.
 *  _LoadBuffer()         : A virtual function defined that loads more
 *                          data out of the memory buffer so it can
 *                          be stuffed into the I/O buffer.
 *  _FlushBuffer()        : Another virtual function that takes data out
 *                          of the I/O buffer and stuffs it into the
 *                          memory buffer.
 *  GrowUserBuffer()      : A private function used to give us more space
 *                          when the memory object is owner of the buffer.
 *                          This function tries to call farrealloc() to do
 *                          its work.
 *  Clone()               : If you place an ALHugeMemory object in a toolkit,
 *                          this function can be called to create a new
 *                          ALHugeMemory object.
 *  Open()                : Open the storage object for reading and writing.
 *  Create()              : Create a new buffer to write to.
 *  Delete()              : Delete the memory object.  It is gone forever.
 *  Close()               : Close the existing memory object.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0: New release
 */


#if !defined( AL_FLAT_MODEL )

class AL_CLASS_TYPE ALHugeMemory : public ALMemoryBase {  /* Tag public class */
/*
 * Constructors, destructors, assignment operator, friends, declarations
 */
    public :
        AL_PROTO ALHugeMemory( const char AL_DLL_FAR *buffer_name = "",
                               char _huge *user_buffer = 0,
                               long user_buffer_size = 0,
                               ALCase name_case = AL_MIXED );
        virtual AL_PROTO ~ALHugeMemory();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * As usual, I don't want the compiler to generate a default copy constructor,
 * or an assignment operator here.  I force it to back off by declaring them
 * here.  They do not exist!
 */
    protected :
        AL_PROTO ALHugeMemory( ALHugeMemory AL_DLL_FAR & );
        ALHugeMemory AL_DLL_FAR & AL_PROTO operator=( const ALHugeMemory AL_DLL_FAR & );
    protected :
        virtual void AL_PROTO _LoadBuffer( long address );
        virtual void AL_PROTO _FlushBuffer( long address );
/*
 * Unique to this class
 */
    protected :
        virtual int AL_PROTO GrowUserBuffer( long minimum_new_size );
    public :
        virtual ALStorage AL_DLL_FAR * AL_PROTO Clone( const char AL_DLL_FAR *name,
                                            int object_type ) const;
        virtual int AL_PROTO Open();
        virtual int AL_PROTO Create( long desired_size = -1L );
        virtual int AL_PROTO Delete();
        virtual int AL_PROTO Close();
        char _huge *mpcUserBuffer;
        AL_CLASS_TAG( _ALHugeMemoryTag );
};

#endif /* #if !defined( AL_FLAT_MODEL ) */

/*
 * class ALWinMemory : ALMemoryBase
 *
 * DESCRIPTION
 *
 *  Class ALWinMemory is an ALStorage class that stores its data in
 *  huge buffers allocated from the Windows heap.  If you are writing
 *  16 bit programs, this gives you a way to create memory based storage
 *  objects that can span more than 64Kbytes.
 *
 * DATA MEMBERS
 *
 *  mpcUserBuffer       : A pointer to the huge buffer that is going to
 *                        hold the data.  This is a buffer that gets
 *                        allocated with GlobalAlloc().
 *  mhUserMemoryHandle  : The Window handle for the memory block.
 *
 * MEMBER FUNCTIONS
 *
 *  ALWinMemory()         : The constructor.  This is where you decide
 *                          whether to let ALWinMemory allocate and manage
 *                          your buffer, or decide to use your own buffer.
 *  ~ALWinMemory()        : The virtual destructor.
 *  operator new()        : Memory allocation operator, only used when the
 *                          library is in a DLL.  Note that this isn't the
 *                          operator used to allocate the buffer, just the
 *                          one to allocate a class object.
 *  _LoadBuffer()         : A virtual function defined that loads more
 *                          data out of the memory buffer so it can
 *                          be stuffed into the I/O buffer.
 *  _FlushBuffer()        : Another virtual function that takes data out
 *                          of the I/O buffer and stuffs it into the
 *                          memory buffer.
 *  GrowUserBuffer()      : A private function used to give us more space
 *                          when the memory object is owner of the buffer.
 *                          This function tries to call GlobalRealloc() to do
 *                          its work.
 *  Clone()               : If you place an ALHugeMemory object in a toolkit,
 *                          this function can be called to create a new
 *                          ALHugeMemory object.
 *  Open()                : Open the storage object for reading and writing.
 *  Create()              : Create a new buffer to write to.
 *  Delete()              : Delete the memory object.  It is gone forever.
 *  Close()               : Close the existing memory object.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0: New release
 */

#if defined( AL_WINDOWS)
class AL_CLASS_TYPE ALWinMemory : public ALMemoryBase {  /* Tag public class */
/*
 * Constructors, destructors, assignment operator, friends, declarations
 */
    public :
        AL_PROTO ALWinMemory( const char AL_DLL_FAR *buffer_name = "",
                           char AL_HUGE *user_buffer = 0,
                           DWORD user_buffer_size = 0,
                           ALCase name_case = AL_MIXED );
        virtual AL_PROTO ~ALWinMemory();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * As usual, I don't want the compiler to generate a default copy constructor,
 * or an assignment operator here.  I force it to back off by declaring them
 * here.  They do not exist!
 */
    protected :
        AL_PROTO ALWinMemory( ALWinMemory AL_DLL_FAR & );
        ALWinMemory AL_DLL_FAR & AL_PROTO operator=( const ALWinMemory AL_DLL_FAR & );
    protected :
        virtual void AL_PROTO _LoadBuffer( long address );
        virtual void AL_PROTO _FlushBuffer( long address );
/*
 * Unique to this class
 */
    protected :
        virtual int AL_PROTO GrowUserBuffer( long minimum_new_size );
    public :
        virtual ALStorage AL_DLL_FAR * AL_PROTO Clone( const char AL_DLL_FAR *name,
                                            int object_type ) const;
        virtual int AL_PROTO Open();
        virtual int AL_PROTO Create( long desired_size = -1L );
        virtual int AL_PROTO Delete();
        virtual int AL_PROTO Close();
        HGLOBAL mhUserMemoryHandle;
        char AL_HUGE *mpcUserBuffer;
        AL_CLASS_TAG( _ALWinMemoryTag );
};
#endif
#else /* #if !defined( __cplusplus ) ... */

#if !defined( AL_FLAT_MODEL )
AL_LINKAGE hALStorage AL_FUNCTION
newALHugeMemory( char AL_DLL_FAR *buffer_name,
                 char _huge *user_buffer,
                 long user_buffer_size );
#endif
AL_LINKAGE hALStorage AL_FUNCTION
newALMemory( char AL_DLL_FAR *buffer_name,
             char AL_DLL_FAR *user_buffer,
             size_t user_buffer_size );

AL_LINKAGE void AL_FUNCTION
ALMemoryBaseSetBufferOwner( hALStorage this_object, int user_owns_buffer );
AL_LINKAGE long AL_FUNCTION
ALMemoryBaseGetBufferSize( hALStorage this_object );
AL_LINKAGE int AL_FUNCTION
ALMemoryBaseGetBufferOwner( hALStorage this_object );

AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALMemoryGetBuffer( hALStorage this_object );

#if !defined( AL_FLAT_MODEL )

AL_LINKAGE char _huge * AL_FUNCTION
ALHugeMemoryGetBuffer( hALStorage this_object );

#endif

#if defined( AL_WINDOWS )

AL_LINKAGE hALStorage AL_FUNCTION
newALWinMemory( char AL_DLL_FAR *buffer_name,
                char AL_HUGE *user_buffer,
                DWORD user_buffer_size );
AL_LINKAGE UINT AL_FUNCTION ALWinMemoryGetHandle( hALStorage this_object );
AL_LINKAGE char AL_HUGE * AL_FUNCTION
ALWinMemoryGetBuffer( hALStorage this_object );

#endif

#endif /* #if defined( __cplusplus ) ... #else ... */

#endif /* #ifndef _MEMSTORE_H        */
