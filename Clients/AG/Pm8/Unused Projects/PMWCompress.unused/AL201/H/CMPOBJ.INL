//
//  CMPOBJ.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALCompressedObject::ClearError()
//
// DESCRIPTION
//
//  Inline functions for class ALCompressedObject.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

//
// NAME
//
//  ALCompressedObject::ClearError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Reset the error status for a compressed object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "cmpobj.h"
//
//  void ALCompressedObject::ClearError()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//
//  AL_LINKAGE void ALCompressedClearError( hALCompressed this_object )
//
// VB SYNOPSIS
//
//  Declare Sub ALArchiveClearError Lib "AL20LW" (ByVal this_object& )
//
// DELPHI SYNOPSIS
//
//  procedure ALCompressedClearError( this_object : hALCompressed );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALCompressedObject that
//                  is going to have its status reset.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  An ALCompressedObject carries around a status object in its mStatus
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
//   February 14, 1996  2.0A : New release
//

inline void AL_INLINE_PROTO
ALCompressedObject::ClearError()  /* Tag public function */
{
    mStatus.SetError( AL_SUCCESS, 0 );
}


