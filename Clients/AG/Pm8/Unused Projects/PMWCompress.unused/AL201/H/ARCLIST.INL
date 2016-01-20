//
// ARCLIST.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALEntryList::ClearError()
//  ALEntryList::SetMarks()
//  ALEntryList::ClearMarks()
//
// DESCRIPTION
//
//  Inline functions for class ALEntryList.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release

//
// NAME
//
//  ALEntryList::ClearError()
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
//  #include <arclib.h>
//
//  void ALEntryList::ClearError()
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  void ALEntryListClearError( hALEntryList this_object );
//
// VB SYNOPSIS
//
//  Declare Sub ALEntryListClearError Lib "AL20LW" (ByVal this_object& )
//
// DELPHI SYNOPSIS
//
//  procedure ALEntryListClearError( this_object : hALEntryList )
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is going to have its status reset.  Note that the C++
//                  version of this call doesn't have an explicit argument
//                  here, since it has access to 'this' implicitly.
//
// DESCRIPTION
//
//  An ALEntryList object carries around a status object in its mStatus
//  member.  For various reasons, this member might get set to an error
//  condition.  Error conditions aren't cleared automatically by the library,
//  so the user will have to manually clear it with a call to this function.
//
//  This is a real simple function, so it will normally be implemented as
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
ALEntryList::ClearError()  /* Tag public function */
{
    mStatus.SetError( AL_SUCCESS, 0 );
}

//
// NAME
//
//  ALEntryList::SetMarks()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Set marks in ALEntry objects based on wild card specification.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  int ALEntryList::SetMarks( const char *wild_name );
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  int ALEntryListSetMarks( hALEntryList this_object,
//                           char *wild_name );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListSetMarks Lib "AL20LW"
//    (ByVal this_object&, ByVal wild_name$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntryListSetMarks( this_object : hALEntryList;
//                                wild_name : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is going to be updated.  Note that the C++ version of
//                  this call doesn't have an explicit copy of this argument,
//                  since it has access to 'this' implicitly.
//
//  wild_name    :  A wild card pattern that is compared to the name of
//                  each storage object in the entry list.  The wild card
//                  matching in this case is not done according to the
//                  rules of the operating system, but rather according to
//                  ArchiveLib's match() function.
//
// DESCRIPTION
//
//  An ALEntryList is composed of a bunch of ALEntry objects.  Each ALEntry
//  has a marker attached to it that can be set or cleared.  Archive
//  operations such as Create(), Append(), and so on, only operate on
//  ALEntry objects that have their mark set.  Marks are set by default when
//  the object is created, but they can be cleared for various reasons.
//
//  This function is used to set the marks of multiple objects at once.
//  It is done by simply checking the names of the storage objects in the
//  ALEntry object.  If a wild card match is successful, the mark is set,
//  otherwise it is left alone.
//
//  This is a real simple function, so it will normally be implemented as
//  a C++ inline function. VB et. al. don't get this boon.
//  this luxury.
//
// RETURNS
//
//  An integer count detailing how many of the marks were set in the list.
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
ALEntryList::SetMarks( const char AL_DLL_FAR *name )  /* Tag public function */
{
    return SetMarkState( name, 1 );
}

//
// NAME
//
//  ALEntryList::ClearMarks()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Clear marks in ALEntry objects based on a wild card specification.
//
// C++ SYNOPSIS
//
//  #include <arclib.h>
//
//  int ALEntryList::ClearMarks( const char *wild_name );
//
// C SYNOPSIS
//
//  #include <arclib.h>
//
//  int ALEntryListClearMarks( hALEntryList this_object,
//                             char *wild_name );
//
// VB SYNOPSIS
//
//  Declare Function ALEntryListClearMarks Lib "AL20LW"
//    (ByVal this_object&, ByVal wild_name$) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALEntryListClearMarks( this_object : hALEntryList;
//                                  wild_name : PChar ) : Integer;
//
// ARGUMENTS
//
//  this_object  :  A reference or pointer to the ALEntryList object that
//                  is going to be updated.  Note that the C++ version of
//                  this call doesn't have an explicit copy of this argument,
//                  since it has access to 'this' implicitly.
//
//  wild_name    :  A wild card pattern that is compared to the name of
//                  each storage object in the entry list.  The wild card
//                  matching in this case is not done according to the
//                  rules of the operating system, but rather according to
//                  ArchiveLib's match() function.
//
// DESCRIPTION
//
//  An ALEntryList is composed of a bunch of ALEntry objects.  Each ALEntry
//  has a marker attached to it that can be set or cleared.  Archive
//  operations such as Create(), Append(), and so on, only operate on
//  ALEntry objects that have their mark set.  Marks are set by default when
//  the object is created, but they can be cleared for various reasons.
//
//  This function is used to clear the marks of multiple objects at once.
//  It is done by simply checking the names of the storage objects in the
//  ALEntry object.  If a wild card match is successful, the mark is cleared,
//  otherwise it is left alone.
//
//  This is a real simple function, so it will normally be implemented as
//  a C++ inline function. VB et. al. don't get this boon.
//  this luxury.
//
// RETURNS
//
//  An integer count detailing how many of the marks were cleared in the list.
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
ALEntryList::ClearMarks( const char AL_DLL_FAR *name )  /* Tag public function */
{
    return SetMarkState( name, 0 );
}

