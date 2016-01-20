/*
 * ARCLIST.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 * This header file contains the definitions for class ALEntryList.
 * ALEntryList objects contain lists of objects that are either going
 * to be inserted into an archive, or already reside there.
 *
 * CLASS DEFINITIONS:
 *
 *  ALEntryList         : A description of entries in an Archive.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 *
 *  March 20, 1996     2.01A : Minor modification to support the test drive.
 */

#ifndef _ARCLIST_H
#define _ARCLIST_H

#if defined( __cplusplus )

/*
 * Forward declarations.
 */

class AL_CLASS_TYPE ALMonitor;

/*
 * class ALEntryList
 *
 * DESCRIPTION
 *
 *  This class is simply a list of ALEntry objects.  There are
 *  quite a few member functions that operate on this list.
 *  ALEntryList objects are passed as arguments to many of the
 *  archive functions, such as ReadDirectory(), Create(), Extract(),
 *  and more.
 *
 * DATA MEMBERS
 *
 *  mpListHead   : The head of the list is a dummy entry that is a
 *                 placeholder.
 *
 *  &mrMonitor   : A reference to the monitor associated with this list.
 *                 The monitor will take care of generating all the
 *                 user interface activity to go with this list.
 *
 *  mToolKit     : The toolkit that is used to create new compressors,
 *                 decompressors, and storage objects as requested.
 *
 *  mStatus      : A standard status member, the status of the whole list.
 *
 * MEMBER FUNCTIONS
 *
 *  ALEntryList()         : The constructor
 *  ALEntryList()         : Copy constructor
 *  ~ALEntryList()        : The destructor.
 *  operator new()        : Memory allocation for the class, only used
 *                          when the ctor is in a DLL.
 *  SetMarkState()        : Set the marks of items in the list to 1 or 0
 *  GetFirstEntry()       : A list iterator function, starts the iteration
 *  SetMarks()            : Set some of the marks in the list
 *  ClearMarks(           : Clear some of the marks in the list
 *  DeleteUnmarked()      : Delete list entries that aren't marked
 *  ToggleMarks()         : Toggle every mark in the list
 *  UnmarkDuplicates()    : Use this to avoid processing duplicates
 *  AddWildCardFiles()    : Add a batch of files to the entry list
 *  ClearError()          : Reset the mStatus member
 *  MakeEntriesFromListBox: Add a batch of files to the entry list
 *  FillListBox()         : Fill a list box up with a list
 *  SetMarksFromListBox() : Use list feedback to set marks
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0: New release
 */
class AL_CLASS_TYPE ALEntryList {  /* Tag public class */
/*
 * Constructors, destructors, friends
 */
    friend class AL_CLASS_TYPE ALEntry;

    public :
/*
 * I have to do this because VC++ 4.0 foolishly requires a
 * copy of FullTools(), even if it isn't ever used, when
 * creating the 32 bit DLL.
 */
#if defined( AL_DEMO )
        AL_PROTO ALEntryList( ALMonitor AL_DLL_FAR *,
                              ALToolKit toolkit );
#else
        AL_PROTO ALEntryList( ALMonitor AL_DLL_FAR * = 0,
                              ALToolKit toolkit = FullTools() );
#endif
        AL_PROTO ~ALEntryList();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator are not supported.  I
 * declare them here because I don't want the compiler to generate
 * default versions for me.
 */
    protected :
        AL_PROTO operator=( ALEntryList AL_DLL_FAR & );
/*
 * Warning!  ALEntryList is one of the few objects in archivelib that
 * has a copy constructor.  I need this in a few of the archiving
 * routines, because I have to make a new list that will hold all of
 * the current entries in an archive.
 */

    public :
        AL_PROTO ALEntryList( ALEntryList AL_DLL_FAR & );
/*
 * Member Functions
 */
    protected :
        int AL_PROTO SetMarkState( const char AL_DLL_FAR *name, short int state );

    public :
        ALEntry AL_DLL_FAR * AL_PROTO GetFirstEntry();
        int AL_PROTO SetMarks( const char AL_DLL_FAR *pattern = 0 );
        int AL_PROTO ClearMarks( const char AL_DLL_FAR *pattern = 0 );
        int AL_PROTO DeleteUnmarked();
        int AL_PROTO ToggleMarks();
        void AL_PROTO UnmarkDuplicates( ALEntryList AL_DLL_FAR &list,
                                        const char AL_DLL_FAR *error_message = 0 );
        int AL_PROTO AddWildCardFiles( const char AL_DLL_FAR *pattern = "*.*",
                                       int traverse_flag = 0 );
        void AL_PROTO ClearError();
/*
 * A windows utility function
 */
#if defined( AL_WINDOWS ) || defined( AL_OS2 )
        int AL_PROTO MakeEntriesFromListBox( HWND hDlg, int list_box_id = -1 );
        int AL_PROTO FillListBox( HWND hDlg, int list_box_id = -1 );
        int AL_PROTO SetMarksFromListBox( HWND hDlg, int list_box_id = -1 );
#endif
    public :
/*
 * Data members
 */
    protected :
        ALEntry *mpListHead; /* The head is never used */

    public :
        ALMonitor AL_DLL_FAR &mrMonitor;
        ALToolKit mToolKit;
        ALStatus mStatus;
        AL_CLASS_TAG( _ALEntryListTag );
};

#include "arclist.inl"

#else /* #if defined( __cplusplus ) */

AL_LINKAGE hALEntryList AL_FUNCTION
newALEntryList( hALMonitor monitor, hALToolKit toolkit );
AL_LINKAGE int AL_FUNCTION
ALEntryListAddWildCardFiles( hALEntryList this_object,
                             char AL_DLL_FAR *pattern,
                             int traverse );
AL_LINKAGE void AL_FUNCTION ALEntryListClearError( hALEntryList this_object );
AL_LINKAGE int AL_FUNCTION ALEntryListSetMarks( hALEntryList this_object,
                                                char AL_DLL_FAR *wild_name );
AL_LINKAGE int AL_FUNCTION ALEntryListClearMarks( hALEntryList this_object,
                                                  char AL_DLL_FAR *wild_name );
AL_LINKAGE void AL_FUNCTION deleteALEntryList( hALEntryList this_object );
AL_LINKAGE hALEntry AL_FUNCTION ALEntryListGetFirstEntry( hALEntryList this_object );
AL_LINKAGE int AL_FUNCTION ALEntryListDeleteUnmarked( hALEntryList this_object );
AL_LINKAGE int AL_FUNCTION ALEntryListToggleMarks( hALEntryList this_object );
AL_LINKAGE void AL_FUNCTION
ALEntryListUnmarkDuplicates( hALEntryList this_object,
                             hALEntryList list,
                             char AL_DLL_FAR *error_message );
AL_LINKAGE int AL_FUNCTION
ALEntryListGetStatusCode( hALEntryList this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALEntryListGetStatusString( hALEntryList this_object );
AL_LINKAGE char AL_DLL_FAR * AL_FUNCTION
ALEntryListGetStatusDetail( hALEntryList this_object );

#if defined( AL_OS2 )  || defined( AL_WINDOWS )

AL_LINKAGE int AL_FUNCTION
ALEntryListSetMarksFromListBox( hALEntryList this_object, HWND hDlg, int list_box_id );
AL_LINKAGE int AL_FUNCTION
ALEntryListFillListBox( hALEntryList this_object,
                        HWND hDlg,
                        int list_box_id );
AL_LINKAGE int AL_FUNCTION
ALEntryListMakeEntriesFromListBox( hALEntryList this_object,
                                   HWND hDlg,
                                   int list_box_id );

#endif

#endif /* #if defined( __cplusplus ) */

#endif /* #ifdef _ARCLIST_H         */

