/*
 * SPINNER.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This header file contains the class definition for the ALMonitor
 *  derived object ALSpinner.
 *
 * CLASS DEFINITIONS:
 *
 *  ALSpinner
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  January 9, 1995  1.01A : The spinner class was modified so that it
 *                           could be used in a DOS DLL.  This mostly
 *                           meant adding those far definitions to the
 *                           member function prototypes.  It also meant
 *                           creation of ALSpinner::new().
 *
 * February 14, 1996  2.0: New release
 */

#ifndef _SPINNER_H
#define _SPINNER_H

#include "arclib.h"

/*
 * class ALSpinner : public ALMonitor
 *
 * DESCRIPTION
 *
 *  ALSpinner is a very simple monitor class, and is only useful under
 *  DOS, not Windows.  All ALSpinner does is spin a little propellor
 *  around while the file is being processed.  This lets you know
 *  that something is happening in an otherwise boring process.
 *
 * DATA MEMBERS
 *
 *  miSpinIndex     : A Static variable that keeps track of which
 *                    position the propellor should be in.  The
 *                    propellor travels through one of four different
 *                    positions.
 *
 *  mrStream        : A reference to the stream that the propellor is
 *                    going to be written on.
 *
 * MEMBER FUNCTIONS
 *
 *  ALSpinner()         : The one and only constructor.
 *  ~ALSpinner()        : The virtual destructor.
 *  Progress()          : The routine that gets called to make the
 *                        propellor twitch.
 *  ArchiveOperation()  : The routine that gets called when archiving
 *                        starts, stops, etc.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
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

class AL_CLASS_TYPE ALSpinner : public ALMonitor {  /* Tag public class */
/*
 *  Constructors, destructors, and friend classes
 */
    public :
        AL_PROTO ALSpinner( ALMonitorType monitor_type,
                            ostream AL_DLL_FAR & stream = cout );
        virtual AL_PROTO ~ALSpinner();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 *  The copy constructor and assignment operator do not exist.
 */
    protected :
        AL_PROTO ALSpinner( const ALSpinner AL_DLL_FAR & );
        ALSpinner AL_DLL_FAR & AL_PROTO operator=( const ALSpinner AL_DLL_FAR & );
/*
 *  Member functions
 */
    protected :
        virtual void AL_PROTO Progress( long mlObjectSoFar,
                                        ALStorage AL_DLL_FAR & object );
        virtual void AL_PROTO ArchiveOperation( ALArchiveOperation operation,
                                                ALArchive AL_DLL_FAR *archive,
                                                ALEntry AL_DLL_FAR *job );
/*
 * Data Members
 */
    protected :
        static int miSpinIndex;
        ostream AL_DLL_FAR & mrStream;
    public :
        AL_CLASS_TAG( _ALSpinnerTag );
};

#else  /* #if defined( __cplusplus ) ... */

AL_LINKAGE hALMonitor AL_FUNCTION
newALSpinner( enum ALMonitorType monitor_type );

#endif /* #if defined( __cplusplus ) ... #else ... */

#endif /* #if !defined( AL_MICROSOFT) || !defined( AL_BUILDING_DLL ) */

#endif /* #ifdef SPINNER_H           */
