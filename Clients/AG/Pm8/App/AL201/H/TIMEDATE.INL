//
//  TIMEDATE.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//   ALTimeDate::Valid()
//
// DESCRIPTION
//
//  Inline functions for class ALGlCompressor and ALGlDecompressor.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release


//
// NAME
//
//  ALTimeDate::Valid()
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
//  inline int ALTimeDate::Valid()
//
// C SYNOPSIS
//
//  None.
//
// VB SYNOPSIS
//
//  None.
//
// DELPHI SYNOPSIS
//
//  None.
//
// ARGUMENTS
//
//  None.
//
// DESCRIPTION
//
//  ALStorage objects carry around a time date stamp.  The time date stamp
//  isn't valid until you have a chance to either read it from the file
//  system or from an archive.  But how do you tell if the time date stamp
//  you have is valid?  Easy, just call this function.
//
//  BTW, the reason we don't have C/VB/Delphi wrapper functions is that
//  those languages don't have immediate access to the mTimeDate member
//  of the storage object.  So instead, they attach their Valid() function
//  to a storage object.  The result is ALStorageTimeDateValid(), or
//  something like that.
//
// RETURNS
//
//  Boolean 1 or 0.
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
ALTimeDate::Valid()  /* Tag public function */
{
    return miYear != 0;
}


