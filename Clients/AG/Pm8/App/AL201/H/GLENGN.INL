//
//  GLENGN.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALGlCompressor::CompressionLevel()
//  ALGlDecompressor::CompressionLevel()
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
//  ALGlCompressor::CompressionLevel()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read the compression level for a Greenleaf engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  short int ALGlCompressor::CompressionLevel()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  short int ALGlCompressorLevel( hALCompressor this_object )
//
// VB SYNOPSIS
//
//  Declare Function ALGlCompressorLevel "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALGlCompressorLevel( this_object : hALCompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALGlCompressor object
//                  whose compression level will be returned.  In C++, this
//                  version of this call doesn't have an explicit argument
//                  referencing this_object, since it has access to 'this'
//                  implicitly.
//
// DESCRIPTION
//
//  The Greenleaf Compressor has a compression level member that
//  dictates how much memory the compressor will devote to its
//  job. The more memory, the better compression you get.  The number
//  returned here will normally be between AL_GREENLEAF_LEVEL_0 and
//  AL_GREENLEAF_LEVEL_4.  A value of AL_GREENLEAF_COPY can also be
//  used, which gives no compressino at all.
//
//  This is a real simple function, so it will normally be implemented as
//  an inline function.  The rest of the supported languages don't have
//  this luxury.
//
// RETURNS
//
//  A short integer that contains the current compression level for the
//  Greenleaf Engine.  If a compression pass fails because a file is
//  incompressible, this guy will return AL_GREENLEAF_COPY, otherwise
//  it should normally be one of AL_GREENLEAF_LEVEL_*.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//


inline short int AL_INLINE_PROTO
ALGlCompressor::CompressionLevel()  /* Tag public function */
{
    return miCompressionLevel;
}

//
// NAME
//
//  ALGlDecompressor::CompressionLevel()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Read the compression level for a Greenleaf engine.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  short int ALGlDecompressor::CompressionLevel()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "glengn.h"
//
//  short int ALGlDecompressorLevel( hALDecompressor this_object )
//
// VB SYNOPSIS
//
//  Declare Function ALGlDecompressorLevel "AL20LW"
//    (ByVal this_object&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALGlDecompressorLevel( this_object : hALDecompressor ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALGlDecompressor object
//                  whose compression level will be returned.  In C++, this
//                  version of this call doesn't have an explicit argument
//                  referencing this_object, since it has access to 'this'
//                  implicitly.
//
// DESCRIPTION
//
//  The Greenleaf Compressor has a compression level member that
//  dictates how much memory the compressor will devote to its
//  job. The more memory, the better compression you get.  The number
//  returned here will normally be between AL_GREENLEAF_LEVEL_0 and
//  AL_GREENLEAF_LEVEL_4.  A value of AL_GREENLEAF_COPY can also be
//  used, which gives no compressino at all.
//
//  When you create a decompressor, you need to match up the compression
//  level in the decompressor with the level that the compressor used to
//  create the data.  In ArchiveLib, this is done automatically for all
//  Archiving functions, as well as the Compressed object.  If you are
//  performing raw compression on blocks of data, you will need to ensure
//  manually that the two levels are the same when creating the engine.
//
//  This is a real simple function, so it will normally be implemented as
//  an inline function.  The rest of the supported languages don't have
//  this luxury.
//
// RETURNS
//
//  A short integer that contains the current compression level for the
//  Greenleaf Engine.  If a compression pass fails because a file is
//  incompressible, this guy will return AL_GREENLEAF_COPY, otherwise
//  it should normally be one of AL_GREENLEAF_LEVEL_*.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline short int AL_INLINE_PROTO
ALGlDecompressor::CompressionLevel()  /* Tag public function */
{
    return miCompressionLevel;
}
