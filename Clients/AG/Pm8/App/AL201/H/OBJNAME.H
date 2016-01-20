/*
 * OBJNAME.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the class definition for ALName.  Most of the time
 *  this class is used to contain file names.
 *
 * CLASS DEFINITIONS:
 *
 *  ALName
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _OBJNAME_H
#define _OBJNAME_H

#if defined( __cplusplus )

#include <string.h>
#include <iostream.h>

/*
 * class ALName
 *
 * DESCRIPTION
 *
 * Object names are mostly used for names of storage objects.
 * There are enough things that I do with these guys to justify
 * a class of their own.  Having all the object name functions in their
 * own class also cuts back on the number of functions in ALStorage,
 * which is already cluttered.
 *
 * Besides serving as the mName member in ALStorage, this class is
 * also pressed into service in ALWildCardExpander, where it is very
 * handy.
 *
 * DATA MEMBERS
 *
 *  mszName     : A pointer to the name associated with this object.
 *                This pointer can be a null pointer.  The object is
 *                responsible for deleting this guy in the ALName
 *                destructor, along with the next member, mszOldName.
 *
 *  mszOldName  : A pointer to the last name associated with this
 *                object.  When you assign a new name to one of these
 *                objects, the old name gets stored here.  This makes
 *                it easy to revert to the old name in case of trouble.
 *
 *  mCase       : One of AL_UPPER, AL_LOWER, or AL_MIXED.  If the value
 *                is AL_UPPER or AL_LOWER, the name is forced to all
 *                upper or lower case whenever it is assigned to the
 *                object.
 *
 * MEMBER FUNCTIONS
 *
 *  ALName(const ALName &)   : The copy constructor.
 *  ALName(const char *)     : Constructor that initializes with a char *.
 *  operator=(const ALName&) : Assignment operator.
 *  operator=(const char *)  : Assignment operator for char *.
 *  ~ALName()                : Destructor, has to clean up dynamic storage.
 *  operator new()           : Memory allocation operatory, only used
 *                             when the library is inside the DLL.  Be
 *                             aware that this operator allocates space for
 *                             the object itself, not the strings that it
 *                             will contain.
 *  Strcpy()                 : A protected member function, copies and
 *                             converts to the appropriate case if necessary.
 *  GetName()                : Returns a pointer to the name string, might
 *                             be 0.
 *  GetOldName()             : Returns a pointer to the previous name
 *                             string, might be 0.
 *  GetSafeName()            : Returns a pointer to the name string, but
 *                             is guaranteed not to return 0.
 *  GetSafeOldName()         : Returns a pointer to the old name string, but
 *                             is guaranteed not to return 0.
 *  ChangeExtension()        : Change a filename extension to a new one.
 *  ChangeTrailingChar()     : Change the trailing character in the filename.
 *  StripFileName()          : Remove the filename, leaving the path and drive.
 *  StripPath()              : Remove path and drive, leaving the filename.
 *  WildCardMatch()          : Test for a match against a regular expression.
 *  operator const char *()  : Return a char *.
 *  operator+()              : Append a string to this string.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 */

/*
 * Microsoft won't let me create a cast operator for char _far *.
 * But they will let me cast to this typedef.  Ugly, but it works.
 */

typedef char AL_DLL_FAR * STRINGF;

class AL_CLASS_TYPE ALName { /* Tag public class */
/*
 * Constructors, destructors, assignment operator, and friends
 */
    public :
        AL_PROTO ALName( const ALName AL_DLL_FAR & );
        AL_PROTO ALName( const char AL_DLL_FAR *s = "",
                         ALCase name_case = AL_MIXED );
        ALName AL_DLL_FAR & AL_PROTO operator = ( const ALName AL_DLL_FAR & rhs );
        ALName AL_DLL_FAR & AL_PROTO operator = ( const char AL_DLL_FAR * rhs );
        AL_PROTO ~ALName();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 *  Note that I don't have the normal prohibition against a copy constructor
 *  or an assignment operator in this class, because I support them here.
 */

/*
 *  Member Functions
 */
    protected :
        void AL_PROTO Strcpy( const char AL_DLL_FAR *s );

    public :
        const char AL_DLL_FAR * AL_PROTO GetName() const;
        const char AL_DLL_FAR * AL_PROTO GetOldName() const;
        const char AL_DLL_FAR * AL_PROTO GetSafeName() const;
        const char AL_DLL_FAR * AL_PROTO GetSafeOldName() const;
        ALName AL_DLL_FAR & AL_PROTO
        ChangeExtension( const char AL_DLL_FAR *new_extension = ".bak" );
        ALName AL_DLL_FAR & AL_PROTO ChangeTrailingChar( char new_char = '@' );
        ALName AL_DLL_FAR & AL_PROTO StripFileName();
        ALName AL_DLL_FAR & AL_PROTO StripPath();
        int AL_PROTO WildCardMatch( const char AL_DLL_FAR *pattern );
/*
 *  Operators
 */
    public :
#if defined( AL_MICROSOFT ) && ( AL_MICROSOFT < 800 ) && ( defined( AL_BUILDING_DLL ) || defined( AL_USING_DLL ) ) /*??? DON'T ASK ME WHY */
        AL_PROTO operator STRINGF() const;
#else
        AL_PROTO operator const STRINGF() const;
#endif
        ALName AL_PROTO operator + ( const char AL_DLL_FAR *rhs );
/*
 * Data members
 */
    protected :
        char AL_DLL_FAR * mszName;
        char AL_DLL_FAR * mszOldName;
    public :
        const ALCase mCase;
        AL_CLASS_TAG( _ALNameTag );
};

#include "objname.inl"

#else /* #if defined( __cplusplus ) ... */

AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION StripPath( char AL_DLL_FAR *filename );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION StripFileName( char AL_DLL_FAR *filename );

#endif /* #if defined( __cplusplus ) ... #else ... */

#endif /* #ifndef _OBJNAME_H         */

