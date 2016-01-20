//
//  OBJNAME.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALName::GetName()
//  ALName::GetOldName()
//  operator<<( ostream& stream, const ALName &object )
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
//  ALName::GetName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Retrieve a character pointer from an object name
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  const char * ALName::GetName() const
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
//  To help make my life a little easier, storage object names are
//  stored in the ALName class, instead of as raw strings.  But sometimes
//  you need a standard C character pointer, instead of an ALName object.
//  For example, if you are calling a function such as fopen() that
//  expects a character pointer.  When that happens, we can use this
//  function to retrieve a pointer to the real string inside the object.
//  It returns a constant character pointer, which means you aren't allowed
//  to jack with the contents of the string, good thing.
//
//  Why is there no C translation version of this function?  C can't deal
//  with ALName objects very well, so I provide functions that return
//  the name from a storage object directly, without having to track down
//  the mName member of ALStorage first.
//
// RETURNS
//
//  A character pointer pointing to the string inside the object.  Note
//  that under some circumstances, this could be a NULL pointer.  ALName
//  objects by default are created with no string at all.
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
ALName::GetName() const  /* Tag public function */
{
    return mszName;
}

//
// NAME
//
//  ALName::GetOldName()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Retrieve a character pointer from an old object name
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  const char * ALName::GetOldName() const
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
//  To help make my life a little easier, storage object names are
//  stored in the ALName class, instead of as raw strings.  But sometimes
//  you need a standard C character pointer, instead of an ALName object.
//  For example, if you are calling a function such as fopen() that
//  expects a character pointer.  When that happens, we can use this
//  function to retrieve a pointer to the real string inside the object.
//  It returns a constant character pointer, which means you aren't allowed
//  to jack with the contents of the string, good thing.
//
//  This function returns the old name contained in the ALName object,
//  instead of the name itself.  The old name gets updated whenever the
//  main name changes.
//
//  Why is there no C translation version of this function?  C can't deal
//  with ALName objects very well, so I provide functions that return
//  the name from a storage object directly, without having to track down
//  the mName member of ALStorage first.
//
// RETURNS
//
//  A character pointer pointing to the string inside the object.  Note
//  that under some circumstances, this could be a NULL pointer.  ALName
//  objects by default are created with no string at all.
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
ALName::GetOldName() const  /* Tag public function */
{
    return mszOldName;
}

//
// NAME
//
//  ostream& operator<<( ostream&, const ALName &)
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++
//
// SHORT DESCRIPTION
//
//  Sends an ALName object to an output stream.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//
//  ostream& operator<<( ostream& stream,
//                       const ALName &object )
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
//  stream  : An I/O stream.
//
//  object  : A reference to an ALName object.
//
// DESCRIPTION
//
//  This stream operator makes it easy to send ALName objects
//  to an output stream.  I need to define this function as inline,
//  because it is tough to use far references to ostreams from a DLL.
//  There are other problems associated with using this function
//  in a DLL, and I don't understand them all.
//
// RETURNS
//
//  A reference to the stream provided as an operator.
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
operator<<( ostream& stream, const ALName AL_DLL_FAR &object )  /* Tag public function */
{
#if defined( AL_USING_DLL ) && !defined( AL_LARGE_MODEL ) && !defined( AL_FLAT_MODEL )
    const char _far *p = (STRINGF) object;
    char *near_string = new char[ _fstrlen( p ) + 1 ];
    if ( near_string ) {
        _fstrcpy( near_string, p );
        stream << near_string;
        delete near_string;
    } else
         stream << "Memory allocation failure!";
#else
    stream << (STRINGF) object;
#endif
    return stream;
}

