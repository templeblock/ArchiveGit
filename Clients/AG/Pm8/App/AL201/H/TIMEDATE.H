/*
 * TIMEDATE.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 *  This header file contains the class declaration for ALTimeDate.
 *
 * CLASS DEFINITIONS:
 *
 *  ALTimeDate
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A   : First release
 *
 *  August 10, 1994 1.0B : Added #ifdefs to exclude the DOS functions
 *                         when compiled under UNIX.
 *
 *  February 14, 1996  2.0A : New release
 */

#ifndef _TIMEDATE_H
#define _TIMEDATE_H

#include <time.h>
/*
 * class
 *
 * DESCRIPTION
 *
 *  class ALTimeDate is used strictly to keep track of the time/date
 *  stamp of an ALStorage object.  The only place this class appears is
 *  as the mTimeDate member of ALStorage.  It has a number of conversion
 *  utilities for loading and exporting its values in various formats.
 *  Since most of the ways we have to set time stamps for objects
 *  are not ANSI standard, we end up with quite a few conversion utilities.
 *
 * DATA MEMBERS
 *
 *  miYear   : The year just like you would expect, e.g 1994.  A value of
 *             0 in this field indicates an invalid time.
 *
 *  miMonth  : The month, 1-12.
 *
 *  miDate   : The date, 1-31.
 *
 *  miHour   : In 2400 format, 0 - 23.
 *
 *  miMinute : 0 - 59
 *
 *  miSecond : 0 -59,
 *
 * MEMBER FUNCTIONS
 *
 *  ALTimeDate()        : The constructor, sets all members to 0.
 *  ~ALTimeDate()       : The destructor, has no work to do.
 *  operator new()      : The memory allocation operator, only used
 *                        when the library is inside a DLL.
 *  ToJulian()          : Convert the internal m/d/y members to
 *                        a julian day number.
 *  FromJulian()        : Convert a julian day number to internal
 *                        data members m/d/y.
 *  GetUnixTime()       : Convert all members to a long in unix format,
 *                        total seconds since 1/1/1970.
 *  GetDosTime()        : Convert h:m:s data members to the unsigned int
 *                        used in certain DOS commands.
 *  GetDosDate()        : Convert m/d/y data members to the unsigned int
 *                        used in certain DOS commands.
 *  SetTimeDate(long)   : Set internal data members from a UNIX long.
 *  SetTimeDate(struct tm*) : Set internal data members from a DOS
 *                            struct tm *.
 *  GetTimeDate()       : Convert internal data members to a DOS struct tm *.
 *  Valid()             : Indicate if a valid time has been set.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0: New release
 */

#ifdef __cplusplus
class AL_CLASS_TYPE ALTimeDate {  /* Tag public class */
/*
 * Constructors, destructors, declarations, assignment operator
 */
    public :
        AL_PROTO ALTimeDate();
        AL_PROTO ~ALTimeDate();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * I usually hide the copy constructor and assignment operators,
 * but in this case they are OK
 *
 *
 * Member functions
 */
    public :
        long AL_PROTO ToJulian();
        void AL_PROTO FromJulian( long jdn );
        long AL_PROTO GetUnixTime();
        unsigned short int AL_PROTO GetDosTime();
        unsigned short int AL_PROTO GetDosDate();
        void AL_PROTO SetTimeDate( long unix_time );
        void AL_PROTO SetTimeDate( struct tm AL_DLL_FAR *tblock );
        void AL_PROTO GetTimeDate( struct tm AL_DLL_FAR *tblock );
        int AL_PROTO Valid();
/*
 * Data members
 */
    protected :
        short int miYear;       /* What you expect, e.g. 1995  */
        short int miMonth;      /* 1-12                        */
        short int miDate;       /* 1-31                        */
        short int miHour;       /* 0-23                        */
        short int miMinute;     /* 0-59                        */
        short int miSecond;     /* 0-59                        */
};

#include "timedate.inl"

#else /* #ifdef __cplusplus ... */

#endif  /* #ifdef __cplusplus ... #else ...*/
#endif
