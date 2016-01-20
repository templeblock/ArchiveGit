//
//  STATUS.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALStatus::GetStatusCode()
//  ALStatus::operator int()
//  operator<<( ostream& stream, const ALStatus &status )
//
// DESCRIPTION
//
//  Inline functions for class ALStatus.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

//
// NAME
//
//  ALStatus::GetStatusCode()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Read the status integer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  int ALStatus::GetStatusCode()
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
//  Lots of objects in ArchiveLib carry around a status member.  The
//  status member has two parts.  First, it has a numeric value that
//  is 0 when things are cool and < 0 when things aren't.  This function
//  takes care of returning that value.  The second part is the text
//  of the error message, and we don't worry about that here.
//
//  So why no C/VB/Delphi translation for this function?  To simplify things,
//  C and other programs have individual customized routines that know how
//  to get the status from ALArchive, ALCompressors, etc.  That way
//  they don't have to go through a two step process of getting the
//  status member of an object first, then getting the integer value
//  out of that status object.
//
// RETURNS
//
//  The integer value contained in the status member.
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
ALStatus::GetStatusCode()  /* Tag public function */
{
    return miStatus;
}

//
// NAME
//
//  ALStatus::operator int()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Get the status integer.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  inline ALStatus::operator int()
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
//  Lots of objects in ArchiveLib carry around a status member.  The
//  status member has two parts.  First, it has a numeric value that
//  is 0 when things are cool and < 0 when things aren't.  This function
//  takes care of returning that value.  The second part is the text
//  of the error message, and we don't worry about that here.
//
//  This overloaded operator int is just the ticket for converting an
//  ALStatus member to its integer whenever needed.
//
// RETURNS
//
//  The integer value contained in the status member.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline AL_INLINE_PROTO
ALStatus::operator int()  /* Tag public function */
{
    return miStatus;
}

//
// NAME
//
//  ostream& operator<<( ostream& , const ALStatus &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Send the text description of an ALStatus out a stream.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  inline ostream& operator<<( ostream& stream, const ALStatus &status )
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
//  Lots of objects in ArchiveLib carry around a status member.  The
//  status member has two parts.  First, it has a numeric value that
//  is 0 when things are cool and < 0 when things aren't.  Whoever
//  set the message to a value less than zero is also responsible for
//  adding a text description of the message to the status member.  That
//  text description is what gets output to the stream in this function.
//
// RETURNS
//
//  A reference to the output stream, so you can cascade the output.
//
// EXAMPLE
//
// SEE ALSO
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
//

inline ostream&
operator<<( ostream& stream, const ALStatus AL_DLL_FAR &status )  /* Tag public function */
{
#if defined( AL_USING_DLL ) && !defined( AL_LARGE_MODEL ) && !defined( AL_FLAT_MODEL )
    const char _far *p = status.GetStatusDetail();
    char *near_string = new char[ _fstrlen( p ) + 1 ];
    if ( near_string ) {
        _fstrcpy( near_string, p );
        stream << near_string;
        delete near_string;
    } else
         stream << "Memory allocation failure!";
    return stream;
#else
    return stream << status.GetStatusDetail();
#endif
}

