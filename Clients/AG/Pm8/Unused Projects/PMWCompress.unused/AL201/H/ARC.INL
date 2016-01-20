//
// ARC.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALArchive::ClearError()
//  ALArchive::GetComment()
//  ALArchive::GetVersion()
//  ALArchive::GetStorageObject()
//
//
// DESCRIPTION
//
//  Inline member functions of class ALArchive.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

//
// NAME
//
//  ALArchive::ClearError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Reset the error status for an entry list.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALArchive::ClearError()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALArchiveClearError( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Sub ALArchiveClearError Lib "AL20LW" (ByVal this_object& )
//
// DELPHI SYNOPSIS
//
//  procedure ALArchiveClearError( this_object : hALArchive );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object that
//                  is going to have its status reset.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  An ALArchive object carries around a status object in its mStatus
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
// REVISION HISTORY
//
//  February 14, 1996  2.0A : New release
//

inline void AL_INLINE_PROTO
ALArchive::ClearError()  /* Tag public function */
{
    mStatus.SetError( AL_SUCCESS, 0 );
}

//
// NAME
//
//  ALArchive::GetComment()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the comment attached to an ALArchive object.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  const char * ALArchive::GetComment();
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  char * ALArchiveGetComment( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveGetComment Lib "AL20LW"
//    Alias "ALArchiveGetCommentVB"
//    (ByVal this_object&) As String
//
// DELPHI SYNOPSIS
//
//  function ALArchiveGetComment( this_object : hALArchive ) : PChar;
//
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument pointing to this_object,
//                  since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  This function is used to retrieve the comment associated with
//  an ALArchive object.  Comments can be set manually at run time with
//  the SetComment() function, or they can be read into the ALArchive
//  object when reading the directory from the physical archive.
//
//  This is a real simple function, so in C++ it will normally be implemented
//  as inline.  The other supported languages will still have to call the
//  normal C function, found in CXL_ARCH.CPP.
//
// RETURNS
//
//  A string containing the comment.  For C++, C, and Delphi programmers,
//  this will be a pointer to a null-terminated C-Style character array.
//  The string itself is actually stored in the ALEntry object, and YOU
//  MUST NOT MODIFY IT!!! C++ will prevent you from doing this, but C
//  and Delphi programs are simply on their honor.
//
//  C-style character strings aren't very fun to work with in VB, so the
//  VB version of this function actually returns a true VB string.  Since
//  this VB string has no connection to the ALArchive object, you are free
//  to jack with it to your heart's delight.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A  : New release
//

inline const char AL_DLL_FAR * AL_INLINE_PROTO
ALArchive::GetComment()  /* Tag public function */
{
    return mComment;
}

//
// NAME
//
//  ALArchive::GetVersion()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read the version number for an archive object.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  short ALArchive::GetVersion()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  int ALArchiveGetVersion( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveGetVersion Lib "AL20LW"
//   (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALArchiveGetVersion( this_object : hALArchive ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument for this_object,
//                  since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  An ALArchive object carries around a version number in its miVersion
//  member.  Sometimes we might like to see what that version number is,
//  but it's a protected member, so we need an access function.  That's
//  what this guy is.
//
//  This is a real simple function, so it will normally be implemented as
//  inline.  The rest of the supported languages don't have
//  this luxury.
//
// RETURNS
//
//  At this time, all ALGlArchive objects are going to return 0x100 for
//  their version number.  ALPkArchive objects should always return 0x20.
//  Since their haven't been any upgrades of either format since ArchiveLib
//  was first created, version numbers aren't too important yet.  As
//  archive formats are updated, this will change.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline short AL_INLINE_PROTO
ALArchive::GetVersion()  /* Tag public function */
{
    return miVersion;
}

//
// NAME
//
//  ALArchive::GetStorageObject()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get a pointer to the storage object being used by the ALArchive.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ALStorage * ALArchive::GetStorageObject()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  hALStorage ALArchiveGetStorage( hALArchive this_object );
//
// VB SYNOPSIS
//
//  Declare Function ALArchiveGetStorage Lib "AL20LW"
//    (ByVal archive&) As Long
//
// DELPHI SYNOPSIS
//
//  function ALArchiveGetStorage( this_object : hALArchive ) : hALStorage;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALArchive object in
//                  question.  Note that the C++ version of this function
//                  doesn't have an explicit argument for this_object,
//                  since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  An ALArchive object always has a storage object underlying it.  The
//  storage object is where the archived objects are actually stored.
//  Sometimes you might want to check out this storage object, for various
//  reasons.  You can get at it with this function.
//
//  This is a real simple function, so it will normally be implemented as
//  an inline function.  The rest of the supported languages don't have
//  this luxury.
//
// RETURNS
//
//  A reference or pointer to the storage object underlying the archive.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline ALStorage AL_DLL_FAR * AL_INLINE_PROTO
ALArchive::GetStorageObject()  /* Tag public function */
{
    return mpArchiveStorageObject;
}

