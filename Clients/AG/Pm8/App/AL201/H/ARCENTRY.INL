//
// ARCENTRY.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntry::GetCompressedSize()
//  ALEntry::GetCrc32()
//  ALEntry::GetComment()
//  ALEntry::SetMark()
//  ALEntry::ClearMark()
//  ALEntry::SetMarkState()
//  ALEntry::GetMark()
//  ALEntry::GetCompressedObjectPosition()
//
// DESCRIPTION
//
//  Inline functions for class ALEntry.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

//
// NAME
//
//  ALEntry::GetCompressedSize()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the compressed size of the storage object for an ALEntry.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  long ALEntry::GetCompressedSize() const;
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  long ALEntryGetCompressedSize( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetCompressedSize Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetCompressedSize( this_object : hALEntry ) : LongInt;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object that
//                  refers to the storage object in question.  Note that
//                  the C++ version of this call doesn't have an explicit
//                  argument here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This function is used to determine the compressed size of an object
//  referrred to in an ALEntry object.  Note that the compressed size
//  will only be available after executing an archive function that stores
//  this object in an archive, or after reading the directory.  You cannot
//  use it to predict a compressed size in advance of the actual
//  compression procedure!
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  A long value, indicating the compressed size.  Note that this information
//  will only be available after reading a directory, or performing an
//  operation that compresses the file.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline long AL_INLINE_PROTO
ALEntry::GetCompressedSize() const  /* Tag public function */
{
    return mlCompressedSize;
}

//
// NAME
//
//  ALEntry::GetCrc32()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the CRC-32 of the storage object for an ALEntry.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  long ALEntry::GetCrc32() const;
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  long ALEntryGetCrc32( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetCrc32 Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetCrc32( this_object : hALEntry ) : LongInt;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object that
//                  refers to the storage object in question.  Note that
//                  the C++ version of this call doesn't have an explicit
//                  argument here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This function is used to read the previously calculated CRC-32 of an
//  object referrred to in an ALEntry object.  Note that the CRC
//  will only be available after executing an archive function that stores
//  this object in an archive, or after reading the directory.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  A long value, containing the CRC-32.  Note that this information
//  will only be available after reading a directory, or performing an
//  operation that compresses the file.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline long AL_INLINE_PROTO
ALEntry::GetCrc32() const  /* Tag public function */
{
    return mlCrc32;
}

//
// NAME
//
//  ALEntry::GetComment()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the comment attached to an ALEntry object.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  const char * ALEntry::GetComment();
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  char *ALEntryGetComment( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetComment Lib "AL20LW"
//    Alias "ALEntryGetCommentVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetComment( this_object : hALEntry ) : PChar;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument here, since it has
//                  access to 'this' implicitly.
//
// DESCRIPTION
//
//  This function is used to retrieve the comment associated with
//  an ALEntry object.  Comments can be set manually at run time with
//  the SetComment() function, or they can be read into the list when
//  reading the directory.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  A string containing the comment.  For C++, C, and Delphi programmers,
//  this will be a pointer to a null-terminated C-Style character array.
//  The string itself is actually stored in the ALEntry object, and YOU
//  MUST NOT MODIFY IT!!!  C++ will prevent you from making this horrible
//  faux pas, but C and Delphi programs are simply on their honor.
//
//  C-style character strings aren't very fun to work with in VB, so the
//  VB version of this function actually returns a true VB string.  Since
//  this VB string has no connection to the ALEntry object, you are free
//  to jack with it to your heart's delight.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline const char AL_DLL_FAR * AL_INLINE_PROTO
ALEntry::GetComment()  /* Tag public function */
{
    return mszComment;
}

//
// NAME
//
//  ALEntry::SetMark()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set the marker attached to an ALEntry object.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntry::SetMark()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntrySetMark( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntrySetMark Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntrySetMark( this_object : hALEntry );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument here, since it has
//                  access to 'this' implicitly.
//
// DESCRIPTION
//
//  Every ALEntry object has a mark associated with it.  Archiving
//  functions such as Create(), Delete(), etc., only perform their
//  operations on entries whose marks are set.  By default, entries
//  are created with their marks set, but various operations such
//  as ClearMark() can be used to clear them.  After it's been cleared,
//  you might want to use this function to set it again.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline void AL_INLINE_PROTO
ALEntry::SetMark()  /* Tag public function */
{
    miMark = 1;
}

//
// NAME
//
//  ALEntry::ClearMark()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Clear the marker attached to an ALEntry object.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntry::ClearMark()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntryClearMark( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntryClearMark Lib "AL20LW" (ByVal this_object&)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntryClearMark( this_object : hALEntry );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument here, since it has
//                  access to 'this' implicitly.
//
// DESCRIPTION
//
//  Every ALEntry object has a mark associated with it.  Archiving
//  functions such as Create(), Delete(), etc., only perform their
//  operations on entries whose marks are set.  By default, entries
//  are created with their marks set.  If you want to exclude an entry
//  from some archiving operation, you can call this function to
//  clear its mark.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline void AL_INLINE_PROTO
ALEntry::ClearMark()  /* Tag public function */
{
    miMark = 0;
}

//
// NAME
//
//  ALEntry::SetMarkState()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set or clear the marker attached to an ALEntry object.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntry::SetMarkState( short int new_state )
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntrySetMarkState( hALEntry this_object,
//                            short int new_state );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntrySetMarkState Lib "AL20LW"
//    (ByVal this_object&, ByVal new_state%)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntrySetMarkState( this_object : hALEntry;
//                                 new_state : Integer );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument here, since it has
//                  access to 'this' implicitly.
//
//  new_state    :  The desired new state of the ALEntry object.  0 is used
//                  to clear the mark, and any non-zero value is used to
//                  set it.
//
// DESCRIPTION
//
//  Every ALEntry object has a mark associated with it.  Archiving
//  functions such as Create(), Delete(), etc., only perform their
//  operations on entries whose marks are set.  By default, entries
//  are created with their marks set.  This function can be used to
//  either set or clear the mark associated with an ALEntry object.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  Nothing.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline void AL_INLINE_PROTO
ALEntry::SetMarkState( short int new_state )  /* Tag public function */
{
    miMark = new_state;
}

//
// NAME
//
//  ALEntry::GetMark()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Retrieve the mark status of an ALEntry object.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  int ALEntry::GetMark()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntrySetMarkState( hALEntry this_object,
//                            short int new_state );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntrySetMarkState Lib "AL20LW"
//    (ByVal this_object&, ByVal new_state%)
//
// DELPHI SYNOPSIS
//
//  procedure ALEntrySetMarkState( this_object : hALEntry;
//                                 new_state : Integer );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument here, since it has
//                  access to 'this' implicitly.
//
//  new_state    :  The desired new state of the ALEntry object.  0 is used
//                  to clear the mark, and any non-zero value is used to
//                  set it.
//
// DESCRIPTION
//
//  Every ALEntry object has a mark associated with it.  Archiving
//  functions such as Create(), Delete(), etc., only perform their
//  operations on entries whose marks are set.  By default, entries
//  are created with their marks set.  This function can be used to
//  either set or clear the mark associated with an ALEntry object.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  Nothing.
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
ALEntry::GetMark()  /* Tag public function */
{
    return miMark;
}

//
// NAME
//
//  ALEntry::GetCompressedObjectPosition()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the position of the storage object in the archive.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  long ALEntry::GetCompressedObjectPosition() const;
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  long ALEntryGetCompressedObjectPosition( hALEntry this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryGetCompressedObjectPosition Lib "AL20LW"
//    (ByVal this_object&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALEntryGetCompressedObjectPosition( this_object : hALEntry ) : LongInt;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntry object that
//                  refers to the storage object in question.  Note that
//                  the C++ version of this call doesn't have an explicit
//                  argument here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This function is used to determine the position of an object
//  in an archive object.  Note that the compressed object position
//  will only be available after executing an archive function that stores
//  this object in an archive, or after reading the directory.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ENTR.CPP.
//
// RETURNS
//
//  A long value, indicating the compressed size.  Note that this information
//  will only be available after reading a directory, or performing an
//  operation that compresses the file.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline long AL_INLINE_PROTO
ALEntry::GetCompressedObjectPosition() const  /* Tag public function */
{
    return mlCompressedObjectPosition;
}

