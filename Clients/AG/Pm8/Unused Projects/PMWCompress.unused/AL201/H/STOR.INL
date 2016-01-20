//
//  STOR.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//   ALStorage::GetSize()
//   ALStorage::IsOpen()
//   ALStorage::ClearError()
//   ALStorage::ReadChar()
//   ALStorage::WriteChar()
//
// DESCRIPTION
//
//  Inline functions for class ALStorage.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

//
// NAME
//
//  ALStorage::GetSize()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Returns the size of a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  inline long ALStorage::GetSize() const
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  long ALStorageGetSize( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageGetSize Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALStorageGetSize( this_object : hALStorage ) : LongInt;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object whose
//                  size is desired.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  Once an ALStorage object has been opened and closed, you can determine
//  what size it was by looking at the mlSize member.  This member is
//  protected so people can't jack with it.
//
//  Be sure to note that just creating a storage object doesn't initialize
//  the mlSize member to its correct value.   You have to open the file and
//  close it. If you are creating the file, you need to do all your output
//  as well.
//
//  There is an important exception to this, however.  When you read in
//  a directory from an Archive, the size (and other stats) of the stored
//  files are loaded and ready for perusal.
//
// RETURNS
//
//  A long integer containing the file length.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::GetCrc32()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline long AL_INLINE_PROTO
ALStorage::GetSize() const  /* Tag public function */
{
    return mlSize;
}

//
// NAME
//
//  ALStorage::IsOpen()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Indicates whether a storage object is open or not.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  inline int ALStorage::IsOpen()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageIsOpen( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageIsOpen Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageIsOpen( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object whose
//                  status is desired.  Note that the C++ version of this
//                  call doesn't have an explicit argument for this_object,
//                  since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  Sometimes you might want to know if a storage object is open.  This
//  function does just that.  It relies on the fact that an open storage
//  object has its mpcBuffer allocated to determine this status.
//
// RETURNS
//
//  A boolean value telling whether the file is open.
//
// EXAMPLE
//
// SEE ALSO
//
// ALStorage::Open(), ALStorage::Close()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline int AL_INLINE_PROTO
ALStorage::IsOpen()  /* Tag public function */
{
    return mpcBuffer != 0;
}

//
// NAME
//
//  ALStorage::ClearError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Reset the error status for a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALStorage::ClearError()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALStorageClearError( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Sub ALStorageClearError Lib "AL20LW" (ByVal this_object& )
//
// DELPHI SYNOPSIS
//
//  procedure ALStorageClearError( this_object : hALStorage );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to have its status reset.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  An ALStorage object carries around a status object in its mStatus
//  member.  For various reasons, this member might get set to an error
//  condition.  Error conditions aren't cleared automatically by the library,
//  so the user will have to manually clear it with a call to this function.
//
//  This is a real simple function, so in C++ it will be implemented as
//  an inline function.  The rest of the supported languages don't have
//  this luxury.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStatus::SetError()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline void AL_INLINE_PROTO
ALStorage::ClearError()  /* Tag public function */
{
    mStatus.SetError( AL_SUCCESS, 0 );
}

//
// NAME
//
//  ALStorage::ReadChar()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read a character from a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  inline int ALStorage::ReadChar()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageReadChar( hALStorage this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageReadChar Lib "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageReadChar( this_object : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be read from.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This is an inline function that is able to quickly do buffered I/O.
//  By utilizing an I/O buffer we can make this routine very fast, since
//  it doesn't have to call a virtual function.  The virtual function
//  only has to be called when LoadBuffer() gets called.
//
//  Different compilers have different abilities to make this code inline,
//  so sometimes it needs to be tinkered with.  If you see anything in here
//  that looks funny, that probably explains why.
//
//
// It is really important to keep this routine inline!!!!
//
// RETURNS
//
//  Either the next character available from the I/O buffer, or
//  AL_END_OF_FILE.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline int AL_INLINE_PROTO
ALStorage::ReadChar()  /* Tag public function */
{
    int result;

    AL_ASSERT( muWriteIndex == 0, "ReadChar(): Attempt to read while in write mode" ); /*Can't read if I've done a write!*/
    result = muBufferValidData - muReadIndex;
    if ( result <= 0 )
        result = LoadBuffer( mlFilePointer );
    AL_ASSERT( mpcBuffer != 0, "ReadChar(): Attempt to read from closed file" );    /*Potential disaster*/
    if ( result < 0 )
        return result;
    else
        return mpcBuffer[ muReadIndex++ ] & 0xff;
}

//
// NAME
//
//  ALStorage::WriteChar()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Write a character to a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  inline int ALStorage::WriteChar( int c )
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStorageWriteChar( hALStorage this_object, int c );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageWriteChar Lib "AL20LW"
//    (ByVal this_object&, ByVal c%) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageWriteChar( this_object : hALStorage;
//                               c : Integer ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALStorage object that
//                  is going to be written to.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
//  c            :  The character that is going to be written.
//
// DESCRIPTION
//
//  This is an inline function that is able to quickly do buffered I/O.
//  By utilizing an I/O buffer we can make this routine very fast, since
//  it doesn't have to call a virtual function.  The virtual function
//  only has to be called when FlushBuffer() gets called.
//
//  Different compilers have different abilities to make this code inline,
//  so sometimes it needs to be tinkered with.  If you see anything in here
//  that looks funny, that probably explains why.
//
//
// It is really important to keep this routine inline!!!!
//
// RETURNS
//
//  Either the character that we just wrote out, or an error < AL_SUCCESS.
//
// EXAMPLE
//
// SEE ALSO
//
//  ALStorage::ReadChar(), ALStorage::WriteBuffer()
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline int AL_INLINE_PROTO
ALStorage::WriteChar( int c )  /* Tag public function */
{
    int result;

/*    assert( muReadIndex == 0 ); */ /* Can't write if I've done a read */
    AL_ASSERT( mpcBuffer != 0, "WriteChar(): Attempt to write to closed file" );   /* Disaster! */
    result = muBufferSize - muWriteIndex;
    if ( result <= 0 )
        result = FlushBuffer();
    if ( result < 0 )
        return mStatus;
    else
        return mpcBuffer[ muWriteIndex++ ] = (char) c;
}

