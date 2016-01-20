//
//  FILEATTR.INL
//
//  Source file for ArchiveLib 2.0
//  Inline function definitions
//
//  Copyright (c) Greenleaf Software, Inc. 1994-1996
//  All Rights Reserved
//
// CONTENTS
//
//  ALFileAttributes::ReadOnly()
//  ALFileAttributes::System()
//  ALFileAttributes::Hidden()
//  ALFileAttributes::Archive()
//  ALFileAttributes::Directory()
//
// DESCRIPTION
//
//  Inline functions for class ALFileAttribute.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release


//
// NAME
//
//  ALFileAttributes::ReadOnly()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the ReadOnly file protection bit for a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALFileAttributes::ReadOnly()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALStorageReadOnly( hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageReadOnly Lib "AL20LW" (ByVal storage&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageReadOnly( storage : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  storage : The C++ version of this function is a member function that
//            is applied to the mAttribute member of a storage object.
//            As such, it doesn't have an explicit parameter, since it
//            uses the mAttribute object as its implicit this.  Things are
//            a little trickier for C, VB, and Delphi.  Since these languages
//            don't have easy access to the mAttribute member like we do in
//            C++, we give these programmers a shortcut.  Instead of applying
//            this function to an mAttribute member, they get to apply it
//            directly to a storage object.  That's what this argument is.
//
// DESCRIPTION
//
//  Every storage object has a set of attributes that correspond to the
//  attributes kept by the file system.  These might not make sense for
//  objects such as ALMemory, but they are kept on hand anyway.  This
//  function is one of a collection that is used to read those attribute
//  bits.
//
// RETURNS
//
//  An integer that will be set to 0 if the ReadOnly bit of the storage
//  object is clear, or 1 if it is set.
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
ALFileAttributes::ReadOnly()  /* Tag public function */
{
    return miReadOnly;
}

//
// NAME
//
//  ALFileAttributes::System()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the System file protection bit for a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALFileAttributes::System()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALStorageSystem( hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageSystem Lib "AL20LW" (ByVal storage&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageSystem( storage : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  storage : The C++ version of this function is a member function that
//            is applied to the mAttribute member of a storage object.
//            As such, it doesn't have an explicit parameter, since it
//            uses the mAttribute object as its implicit this.  Things are
//            a little trickier for C, VB, and Delphi.  Since these languages
//            don't have easy access to the mAttribute member like we do in
//            C++, we give these programmers a shortcut.  Instead of applying
//            this function to an mAttribute member, they get to apply it
//            directly to a storage object.  That's what this argument is.
//
// DESCRIPTION
//
//  Every storage object has a set of attributes that correspond to the
//  attributes kept by the file system.  These might not make sense for
//  objects such as ALMemory, but they are kept on hand anyway.  This
//  function is one of a collection that is used to read those attribute
//  bits.
//
// RETURNS
//
//  An integer that will be set to 0 if the System bit of the storage
//  object is clear, or 1 if it is set.
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
ALFileAttributes::System()  /* Tag public function */
{
    return miSystem;
}

//
// NAME
//
//  ALFileAttributes::Hidden()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the Hidden file protection bit for a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALFileAttributes::Hidden()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALStorageHidden( hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageHidden Lib "AL20LW" (ByVal storage&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageHidden( storage : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  storage : The C++ version of this function is a member function that
//            is applied to the mAttribute member of a storage object.
//            As such, it doesn't have an explicit parameter, since it
//            uses the mAttribute object as its implicit this.  Things are
//            a little trickier for C, VB, and Delphi.  Since these languages
//            don't have easy access to the mAttribute member like we do in
//            C++, we give these programmers a shortcut.  Instead of applying
//            this function to an mAttribute member, they get to apply it
//            directly to a storage object.  That's what this argument is.
//
// DESCRIPTION
//
//  Every storage object has a set of attributes that correspond to the
//  attributes kept by the file system.  These might not make sense for
//  objects such as ALMemory, but they are kept on hand anyway.  This
//  function is one of a collection that is used to read those attribute
//  bits.
//
// RETURNS
//
//  An integer that will be set to 0 if the Hidden bit of the storage
//  object is clear, or 1 if it is set.
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
ALFileAttributes::Hidden()  /* Tag public function */
{
    return miHidden;
}

//
// NAME
//
//  ALFileAttributes::Archive()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the Archive (or backup) file protection bit for a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALFileAttributes::Archive()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALStorageArchive( hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageArchive Lib "AL20LW" (ByVal storage&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageArchive( storage : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  storage : The C++ version of this function is a member function that
//            is applied to the mAttribute member of a storage object.
//            As such, it doesn't have an explicit parameter, since it
//            uses the mAttribute object as its implicit this.  Things are
//            a little trickier for C, VB, and Delphi.  Since these languages
//            don't have easy access to the mAttribute member like we do in
//            C++, we give these programmers a shortcut.  Instead of applying
//            this function to an mAttribute member, they get to apply it
//            directly to a storage object.  That's what this argument is.
//
// DESCRIPTION
//
//  Every storage object has a set of attributes that correspond to the
//  attributes kept by the file system.  These might not make sense for
//  objects such as ALMemory, but they are kept on hand anyway.  This
//  function is one of a collection that is used to read those attribute
//  bits.
//
// RETURNS
//
//  An integer that will be set to 0 if the Archive bit of the storage
//  object is clear, or 1 if it is set.
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
ALFileAttributes::Archive()  /* Tag public function */
{
    return miArchive;
}

//
// NAME
//
//  ALFileAttributes::Directory()
//
// PLATFORMS/ENVIRONMENTS
//
//  Console  Windows  PM
//  C++  C  VB  Delphi
//
// SHORT DESCRIPTION
//
//  Get the Directory file protection bit for a storage object.
//
// C++ SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALFileAttributes::Directory()
//
// C SYNOPSIS
//
//  #include "arclib.h"
//  #include "fileattr.h"
//
//  int ALStorageDirectory( hALStorage storage );
//
// VB SYNOPSIS
//
//  Declare Function ALStorageDirectory Lib "AL20LW" (ByVal storage&) As Integer
//
// DELPHI SYNOPSIS
//
//  function ALStorageDirectory( storage : hALStorage ) : Integer;
//
// ARGUMENTS
//
//  storage : The C++ version of this function is a member function that
//            is applied to the mAttribute member of a storage object.
//            As such, it doesn't have an explicit parameter, since it
//            uses the mAttribute object as its implicit this.  Things are
//            a little trickier for C, VB, and Delphi.  Since these languages
//            don't have easy access to the mAttribute member like we do in
//            C++, we give these programmers a shortcut.  Instead of applying
//            this function to an mAttribute member, they get to apply it
//            directly to a storage object.  That's what this argument is.
//
// DESCRIPTION
//
//  Every storage object has a set of attributes that correspond to the
//  attributes kept by the file system.  These might not make sense for
//  objects such as ALMemory, but they are kept on hand anyway.  This
//  function is one of a collection that is used to read those attribute
//  bits.
//
//  This attribute bit is a little different than all the other bits
//  in the attribute structure.  If an object has the directory bit set,
//  it is more like a placeholder than anything else, and by definition
//  it will have a length of 0.  When the expansion routines are running,
//  they take special action when creating a file whose directory bit is
//  set.
//
//  At this time, ArchiveLib won't create any entries in an archive with
//  the directory bit set.  However, it will attempt to create directories
//  it finds in PK archives.
//
// RETURNS
//
//  An integer that will be set to 0 if the Directory bit of the storage
//  object is clear, or 1 if it is set.
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
ALFileAttributes::Directory()  /* Tag public function */
{
    return miDirectory;
}

//
// The rest of these functions were in use once, when ArchiveLib briefly
// had a history on UNIX boxes.  Now, they are sad relics of greater times
// gone by.  As such, they aren't documented, and they aren't supported.
// Even worse, they don't have any C/VB/Delphi wrapper functions.
//

inline int AL_INLINE_PROTO
ALFileAttributes::UnixBitsPresent()  /* Tag public function */
{
    return miUnixBitsPresent;
}

inline int AL_INLINE_PROTO
ALFileAttributes::UserRead()  /* Tag public function */
{
    return miUserRead;
}

inline int AL_INLINE_PROTO
ALFileAttributes::UserWrite()  /* Tag public function */
{
    return miUserWrite;
}

inline int AL_INLINE_PROTO
ALFileAttributes::UserExecute()  /* Tag public function */
{
    return miUserExecute;
}

inline int AL_INLINE_PROTO
ALFileAttributes::OtherRead()  /* Tag public function */
{
    return miOtherRead;
}

inline int AL_INLINE_PROTO
ALFileAttributes::OtherWrite()  /* Tag public function */
{
    return miOtherWrite;
}

inline int AL_INLINE_PROTO
ALFileAttributes::OtherExecute()  /* Tag public function */
{
    return miOtherExecute;
}

inline int AL_INLINE_PROTO
ALFileAttributes::GroupRead()  /* Tag public function */
{
    return miGroupRead;
}

inline int AL_INLINE_PROTO
ALFileAttributes::GroupWrite()  /* Tag public function */
{
    return miGroupWrite;
}

inline int AL_INLINE_PROTO
ALFileAttributes::GroupExecute()  /* Tag public function */
{
    return miGroupExecute;
}


