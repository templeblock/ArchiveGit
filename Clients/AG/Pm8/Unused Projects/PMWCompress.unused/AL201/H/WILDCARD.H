/*
 * WILDCARD.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This file contains the class declaration for ALWildCardExpander,
 *  the class used to expand wildcard file specifications under
 *  DOS, Win32, and OS/2
 *
 * CLASS DEFINITIONS:
 *
 *  ALWildCardExpander
 *
 * ENUMERATED TYPES:
 *
 *   ALExpanderState (embedded in ALWildCardExpander)
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0: New release
 */

#ifndef _WILDCARD_H
#define _WILDCARD_H

#include "arclib.h"

#if defined( __cplusplus )


/*
 * class ALWildCardExpander
 *
 * DESCRIPTION
 *
 * This class is used to expand wild card specifications on a DOS, OS/2,
 * or NT file system. Note that you can do exciting things with this, like
 * traversing through subdirectories, and separate various specs using
 * commas or white space.
 *
 * DATA MEMBERS
 *
 *  mState          : The current state of the expander.  This is a value
 *                    from ALExpander state that lets use keep track
 *                    of where we are between calls to the expander.
 *
 *  mpNextExpander  : If we are traversing subdirectories, we will
 *                    open a new expander for each subdirectory.  This
 *                    is a pointer to any subdirectory we might already
 *                    have open for a search in progress.
 *
 *  mInputLine      : The list of wildcard filespecs passed in as an
 *                    argument.  This gets whittled away, and will be
 *                    smaller and smaller as all the names are parsed out.
 *
 *  mFullWildName   : The current wild card file spec that is being
 *                    expanded.
 *
 *  mWildNameOnly   : The current wild name that is being expanded,
 *                    stripped of its drive and path information.
 *
 *  mWildPathOnly   : The current drive and path being expanded, stripped
 *                    of its filename and extension.
 *
 *  mResultFileName : The file name that is returned to the calling
 *                    program.
 *
 *  miTraverseFlag  : This flag indicates whether the search should traverse
 *                    traverse through subdirectories or just search
 *                    in the current directory.
 *
 *  mpOsData        : Pointer to OS specific data.  This is needed because
 *                    the O/S services needed to scan directories vary
 *                    widely between DOS, Win32, and OS/2.
 *
 *  mpcDelimiters   : An array of characters that are delimiters in the
 *                    input file list.
 *
 *  mCase           : Indicates whether file names should always be forced
 *                    to upper case, forced to lower case, or left mixed.
 *
 * MEMBER FUNCTIONS
 *
 *  ALWildCardExpander()    : Constructor, everything we need to know to
 *                            perform the search is defined here.
 *  ~ALWildCardExpander()   : Destructor.
 *  operator new()          : Memory allocation function, used when the
 *                            library is in a DLL.
 *  GetNextWildName()       : Protected routine to get the next wild name
 *                            from the input line.
 *
 *  ConstructOsData()       : There are different versions of this
 *                            function for each O/S.  It takes care of
 *                            allocating the data structures and services
 *                            needed to start a directory scan.
 *
 *  DestroyOsData()         : Likewise, there is a cleanup function for
 *                            each O/S as well.
 *
 *  IsDelimiter()           : A little function that is used internally
 *                            to test an individual character for delimiter
 *                            status.
 *
 *  GetNextFile()           : The function to get the next expanded file
 *                            name.  It keeps chunking out names until
 *                            the search is complete.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0: New release
 *
 *  March 21, 1996     2.01A : Added support for AL_MIXED_UP case.
 */

class AL_CLASS_TYPE ALWildCardOsData; /* An incomplete type.  I'll know what to do here
                                         in the expansion routines, which are O/S specfic */

class AL_CLASS_TYPE ALWildCardExpander {  /* Tag public class */
/*
 * Constructors, destructors, assignment operators, declarations
 */
    protected :
        enum ALExpanderState {  /* Tag protected class */
            GET_NEXT_WILD_NAME,
            GET_FIRST_FILE_NAME,
            GET_NEXT_FILE_NAME,
            GET_FIRST_DIRECTORY,
            GET_NEXT_DIRECTORY
        };
    public :
        AL_PROTO ALWildCardExpander( const char AL_DLL_FAR *file_list,
                                     int traverse_flag = 0,
#if defined( AL_WIN32 ) || defined( AL_OS2 )
                                     ALCase name_case = AL_MIXED_UP );
#elif defined( AL_UNIX )
                                     ALCase name_case = AL_MIXED );
#else
                                     ALCase name_case = AL_LOWER );
#endif
        AL_PROTO ALWildCardExpander( const char AL_DLL_FAR *file_list,
                                     const char AL_DLL_FAR *delimiters,
                                     int traverse_flag = 0,
#if defined( AL_WIN32 ) || defined( AL_OS2 )
                                     ALCase name_case = AL_MIXED_UP );
#elif defined( AL_UNIX )
                                     ALCase name_case = AL_MIXED );
#else
                                     ALCase name_case = AL_LOWER );
#endif
        AL_PROTO ~ALWildCardExpander();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * Disable copy constructor and assignment operator
 */
    protected :
        ALWildCardExpander AL_DLL_FAR & AL_PROTO operator=( ALWildCardExpander AL_DLL_FAR & );
        AL_PROTO ALWildCardExpander( ALWildCardExpander AL_DLL_FAR & );
/*
 * Member functions
 */
    protected :
        int AL_PROTO GetNextWildName();
        void AL_PROTO ConstructOsData();
        void AL_PROTO DestroyOsData();
        int AL_PROTO IsDelimiter( char c );
    public :
        char AL_DLL_FAR * AL_PROTO GetNextFile();
/*
 * Data members
 */
    protected :
        ALExpanderState mState;
        ALWildCardExpander AL_DLL_FAR *mpNextExpander;
        ALName mInputLine;
        ALName mFullWildName;
        ALName mWildNameOnly;
        ALName mWildPathOnly;
        ALName mResultFileName;
        int miTraverseFlag;
        ALWildCardOsData AL_DLL_FAR *mpOsData;
        const char AL_DLL_FAR *mpcDelimiters;
    public :
        const ALCase mCase;
        AL_CLASS_TAG( _ALWildCardExpanderTag );
};
#else  /* #if defined( __cplusplus ) ... */

AL_LINKAGE hALExpander AL_FUNCTION
newALExpander( char AL_DLL_FAR *wild_file_list,
               int traverse_flag,
               enum ALCase name_case );
AL_LINKAGE hALExpander AL_FUNCTION
newALExpanderWithDelimiters( char AL_DLL_FAR *wild_file_list,
                             char AL_DLL_FAR *delimiters,
                             int traverse_flag,
                             enum ALCase name_case );
AL_LINKAGE void AL_FUNCTION deleteALExpander( hALExpander this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALExpanderGetNextFile( hALExpander this_object );

#endif /* #if defined( __cplusplus ) .... #else ...*/

#endif /* #ifdef _WINMON_H           */
