//
//  ENGN.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEngine::ClearError()
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
//  ALEngine::ClearError()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Reset the error status for a compression/decompression engine
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  void ALEngine::ClearError()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALCompressorClearError( hALCompressor this_object );
//  void ALDecompressorClearError( hALDecompressor this_object );
//
// VB SYNOPSIS
//
//  Declare Sub ALCompressorClearError Lib "AL20LW" (ByVal this_object& )
//  Declare Sub ALDecompressorClearError Lib "AL20LW" (ByVal this_object& )
//
// DELPHI SYNOPSIS
//
//  procedure ALCompressorClearError( this_object : hALCompressor );
//  procedure ALDecompressorClearError( this_object : hALDecompressor );
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEngine object that
//                  is going to have its status reset.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  An ALEngine object carries around a status object in its mStatus
//  member.  For various reasons, this member might get set to an error
//  condition.  Error conditions aren't cleared automatically by the library,
//  so the user will have to manually clear it with a call to this function.
//
//  This is a real simple function, so in C++ it will be implemented as
//  an inline function.  The rest of the supported languages don't have
//  this luxury.
//
//  Note that there are two different versions of this function for C, VB,
//  an Delphi.  This was necessary to preserve some type safety in languages
//  where we aren't supporting the concept of inheritance.
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
ALEngine::ClearError()  /* Tag public function */
{
    mStatus.SetError( AL_SUCCESS, 0 );
}

