#ifndef _SIMPON_H
#define _SIMPMON_H
#include "monitor.h"

#if defined( __cplusplus )

/*
 * class ALSimpleMonitor
 *
 * DESCRIPTION
 *
 *  This class is used by the simplified interface functions as a monitor.
 *  For C and C++ programmers, the monitor simply calls a callback function
 *  supplied by the user.  VB and Delphi users see the monitor as an object
 *  that sends text messages to windows they are using as part of their
 *  user interface.
 *
 *  Regardless of the language you are using, this monitor class is hidden
 *  from you.  However, it is pretty simple, and the details of what it
 *  is doing may help you understand the simplified interfaces.
 *
 * DATA MEMBERS
 *
 *  mpCallbackFunction          : A pointer to a callback function that
 *                                the monitor will call from both
 *                                Progress() and ArchiveOperation().
 *
 *  mhTextWindow                : The text window where file names will
 *                                be sent from ArchiveOperation().
 *
 *  mhFileProgressWindow        : The text window where progress ratios
 *                                will be sent from Progress().
 *
 *  mhJobProgressWindow         : The text window were job ratios will be
 *                                sent from Progress().
 *
 * ALSimpleMonitor()     : The constructor, two versions
 * ~ALSimpleMonitor()    : The destructor.
 * Progress()            : The archiving file/job progress function
 * ArchiveOperation()    : The function called at key points in the
 *                         archiving routine.
 *
 * REVISION HISTORY
 *
 *  February 14, 1996  2.0A : First release
 */

class AL_CLASS_TYPE ALSimpleMonitor : public ALMonitor {  /* Tag public class */
/*
 * Constructors, destructors, and friend classes
 */
     public :
          AL_PROTO ALSimpleMonitor( CALLBACK_FN fn );
//
// Alternate constructor, used by VB and Delphi
//
#if defined( AL_VB ) || defined( AL_VB32 )
          AL_PROTO ALSimpleMonitor( HWND text_window,
                                    HWND file_progress_window,
                                    HWND job_progress_window );
#endif
          virtual AL_PROTO ~ALSimpleMonitor();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.
 */
     protected :
          AL_PROTO ALSimpleMonitor( const ALSimpleMonitor AL_DLL_FAR & );
          ALSimpleMonitor AL_DLL_FAR & AL_PROTO operator=( const ALSimpleMonitor AL_DLL_FAR & );
/*
 * Member functions
 */
     protected :
          virtual void AL_PROTO Progress( long object_so_far,
                                          ALStorage AL_DLL_FAR & object );
          virtual void AL_PROTO
          ArchiveOperation( ALArchiveOperation operation,
                            ALArchive AL_DLL_FAR *archive,
                            ALEntry AL_DLL_FAR *job );
/*
 * Data Members
 */
    protected :
        CALLBACK_FN mpCallbackFunction;
#if defined( AL_VB ) || defined( AL_VB32 )
        HWND mhTextWindow;
        HWND mhFileProgressWindow;
        HWND mhJobProgressWindow;
#endif
     public :
          AL_CLASS_TAG( _ALSimpleMonitorTag );
};
#endif

#endif /* #ifdef _SIMPMON_H */

