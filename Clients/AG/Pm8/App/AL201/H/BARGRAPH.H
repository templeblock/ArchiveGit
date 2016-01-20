/*
 * BARGRAPH.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  The class definition for ALBarGraph is found here.
 *
 * CLASS DEFINITIONS:
 *
 *  class ALBarGraph
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  January 9, 1995  1.01A : The bargraph class was modified so that it
 *                           could be used in a DOS DLL.  This mostly
 *                           meant adding those far definitions to the
 *                           member function prototypes.  It also meant
 *                           creation of ALBarGraph::new().
 *
 *  February 14, 1996  2.0: New release
 */

#ifndef _BARGRAPH_H
#define _BARGRAPH_H

#include "arclib.h"


/*
 * class ALBarGraph : public ALMonitor
 *
 * DESCRIPTION
 *
 *  This is a utility class.  The constructor opens a file for input,
 *  and keeps track of whether it was already open or not.  The destructor
 *  will automatically close the file if it was closed when the
 *  ctor was invoked.
 *
 * DATA MEMBERS
 *
 *  miCurrentOffset   : The current offset of the bargraph, in screen
 *                      units.  Usually the bar itself is 20 characters
 *                      long, in which case this value will be somewhere
 *                      between 0 and 19.
 *
 *  miBarLength       : The length of the bar, defined when the 
 *                      constructor is called.  This is a const member,
 *                      which means we can leave it public.
 *
 *  mrStream          : Reference to an output stream.  This is the
 *                      stream where the bargraph gets drawn.
 *                    
 * MEMBER FUNCTIONS
 *
 *  ALBarGraph()        : The constructor.
 *  ~ALBarGraph()       : Virtual destructor.
 *  Progress()          : The progress routine, where the bargraph
 *                        gets updated.
 *  ArchiveOperation()  : The routine that gets called when files
 *                        are opened, closed, etc.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

/*
 * Microsoft refuses to define cout if you are building a
 * windows DLL.  So, we work around that here by preventing
 * anyone from using the bargraph class from a DLL.  That's
 * okay anyway, since Microsoft doesn't have a 16 bit
 * DLL RTL.
 */

#if !defined( AL_MICROSOFT) || !defined( AL_BUILDING_DLL ) || ( defined( AL_MICROSOFT ) && defined( AL_FLAT_MODEL ) )

#if defined( __cplusplus )

class AL_CLASS_TYPE ALBarGraph : public ALMonitor {  /* Tag public class */
/*
 * Constructors, destructors, and friend classes
 */
     public :
          AL_PROTO ALBarGraph( ALMonitorType monitor_type,
                               ostream AL_DLL_FAR& stream = cout,
                               int bar_length = 25 );
          virtual AL_PROTO ~ALBarGraph();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * The copy constructor and assignment operator do not exist.
 */
     protected :
          AL_PROTO ALBarGraph( const ALBarGraph AL_DLL_FAR & );
          ALBarGraph AL_DLL_FAR & AL_PROTO operator=( const ALBarGraph AL_DLL_FAR & );
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
          int miCurrentOffset;
          const int miBarLength;
          ostream AL_DLL_FAR & mrStream;
     public :
          AL_CLASS_TAG( _ALBarGraphTag );
};

#else  /* #if defined( __cplusplus ) ... */

AL_LINKAGE hALMonitor AL_FUNCTION
newALBarGraph( enum ALMonitorType monitor_type );

#endif /* #if defined( __cplusplus ) ... #else ... */

#endif /* #if !defined( AL_MICROSOFT) || !defined( AL_BUILDING_DLL ) */


#endif /* #ifdef _BARGRAPH_H          */
