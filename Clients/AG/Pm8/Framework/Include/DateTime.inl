// ****************************************************************************
// @Doc
// @Module Date.inl |
//
//  File Name:			Date.inl
//
//  Project:			Renaissance
//
//  Author:				Michael G. Dunlap
//
//  Description:		Implementation of the RDate class inlines
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /Development/CalendarComp/Include/CalendarApplication.h        $
//   $Author:: Steve                                                          $
//     $Date:: 11/22/96 4:52p                                                 $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifdef TPSDEBUG
#define	INLINE
#else
#define	INLINE inline
#endif



//------------------------------------------
// Internal Support
//------------------------------------------

//
// @MFunc Return the number of leap days in the given year
//
// @RDesc 1 if it's a leap year else 0
//
INLINE int RDate::GetLeapDays(
			uWORD uwYear )	// @Parm the year to test for leap year
{
	//
	// every 4th year
	if( !(uwYear%4) )
		{
		//
		// Leap years occur every 4 years, except where the year
		// is evenly divisible by 100 and NOT evenly divisible by
		// 400 (go figure)...
		if( uwYear%100 )
			{
			//
			// NOT evenly divisible by 100, so it IS a leap year...
			return 1;
			}
		if( !(uwYear%400) )
			{
			//
			// IS evenly divisible by 100, but is NOT evenly divisible
			// by 400, so it IS a leap year...
			return 1;
			}
		}
	return 0;
}


//
// @MFunc Get the number of days in the given month in the given year
//
// @RDesc the number of days in the month
//
INLINE int RDate::GetDaysInMonth(
			uBYTE	ubMonth,	// @Parm the month
			uWORD	uwYear )	// @Parm the year
{
	return int( kRDateDaysInMonth[ubMonth-1] + ((ubMonth==2)? GetLeapDays(uwYear) : 0) );
}

//
// @MFunc Get the number of days before the given month in the given year
//
// @RDesc the number of days
//
INLINE int RDate::GetDaysBeforeMonth(
			uBYTE	ubMonth,	// @Parm the month
			uWORD	uwYear )	// @Parm the year
{
	return int( kRDateDaysBeforeMonth[ubMonth-1] + ((ubMonth>2)? GetLeapDays(uwYear) : 0) );
}

//
// @MFunc Return the number of days in the given year
//
// @RDesc the number of days
//
INLINE int RDate::GetDaysInYear( uWORD uwYear )
{
	return int( 365 + GetLeapDays(uwYear) );
}

//------------------------------------------
// Date Construction
//------------------------------------------


//
// @MFunc Construct an RDate
//
// @Syntax	RDate()
// @Syntax	RDate( long nDays ) (internal)
// @Syntax	RDate( const RDate& rhs )
// @Syntax	RDate( uBYTE ubMonth, uBYTE ubDay, uWORD uwYear )
//
// @Parm long | nDays | the number of days since the base date
// @Parm const RDate& | rhs | const reference to the date to copy
// @Parm uBYTE | ubMonth | the month
// @Parm uBYTE | ubDay | the day of the month
// @Parm uWORD | uwYear | the year
//
INLINE RDate::RDate() :
	m_lDate( 0 ),
	m_ubMonth( 1 ),
	m_ubDay( 1 ),
	m_uwYear( kRDateBaseYear )
{
}

INLINE RDate::RDate( uBYTE ubMonth, uBYTE ubDay, uWORD uwYear ) :
		m_ubMonth( ubMonth ),
		m_ubDay( ubDay ),
		m_uwYear( uwYear ),
		m_lDate( GetDaysSinceBase(ubMonth,ubDay,uwYear) )
{
}

INLINE RDate::RDate( const RDate& rhs ) :
	m_lDate( rhs.m_lDate ),
	m_uwYear( rhs.m_uwYear ),
	m_ubMonth( rhs.m_ubMonth ),
	m_ubDay( rhs.m_ubDay )
{
}

//
// @CMember Get the current date
INLINE RDate RDate::CurrentDate()
{
#ifdef _WINDOWS
	SYSTEMTIME	tm;
	GetLocalTime( &tm );
	return RDate( uBYTE(tm.wMonth), uBYTE(tm.wDay), uWORD(tm.wYear) );
#else
// REVIEW - APR 4/9/97
// Added Mac date-time code
//#error "Need MAC RDate::CurrentDate() implementation"
	DateTimeRec	tm;
	
	::GetTime(&tm);
	return RDate( uBYTE(tm.month), uBYTE(tm.day), uWORD(tm.year) );
#endif
}

//
// @MFunc assignment operator
//
// @RDesc const reference to self
//
INLINE const RDate& RDate::operator=(
			const RDate& rhs )	// @Parm const reference to RDate being copied
{
	m_lDate		= rhs.m_lDate;
	m_uwYear		= rhs.m_uwYear;
	m_ubMonth	= rhs.m_ubMonth;
	m_ubDay		= rhs.m_ubDay;
	return *this;
}


//------------------------------------------
// Date Comparisons
//------------------------------------------


//
// @MFunc Compare two dates
//
// @RDesc TRUE if this date is the same as the specified date else FALSE
//
INLINE int RDate::operator==(
			const RDate& rhs )	// @Parm const reference to RDate to compare to
			const
{
	return (m_lDate == rhs.m_lDate);
}

//
// @MFunc Compare two dates
//
// @RDesc TRUE if this date is NOT the same as the specified date else FALSE
//
INLINE int RDate::operator!=(
			const RDate& rhs )	// @Parm const reference to RDate to compare to
			const
{
	return (m_lDate != rhs.m_lDate);
}

//
// @MFunc Compare two dates
//
// @RDesc TRUE if this date is before the specified date else FALSE
//
INLINE int RDate::operator<(
			const RDate& rhs )	// @Parm const reference to RDate to compare to
			const
{
	return (m_lDate < rhs.m_lDate);
}

//
// @MFunc Compare two dates
//
// @RDesc TRUE if this date is after than the specified date else FALSE
//
INLINE int RDate::operator>(
			const RDate& rhs )	// @Parm const reference to RDate to compare to
			const
{
	return (m_lDate > rhs.m_lDate);
}

//
// @MFunc Compare two dates
//
// @RDesc TRUE if this date is at or before the specified date else FALSE
//
INLINE int RDate::operator<=(
			const RDate& rhs )	// @Parm const reference to RDate to compare to
			const
{
	return (m_lDate <= rhs.m_lDate);
}

//
// @MFunc Compare two dates
//
// @RDesc TRUE if this date is at or after the specified date else FALSE
//
INLINE int RDate::operator>=(
			const RDate& rhs )	// @Parm const reference to RDate to compare to
			const
{
	return (m_lDate >= rhs.m_lDate);
}


//------------------------------------------
// Date Manipulation
//------------------------------------------


//
// @MFunc Return the date that is the given number of days after this date
//
// @RDesc the date that is nDays after this date
//
INLINE RDate RDate::operator+(
		long nDays ) const	// @Parm the number of days to add
{
	return RDate( m_lDate+nDays );
}

//
// @MFunc Add the given number of days to this date
//
// @RDesc const reference to this date
//
INLINE const RDate& RDate::operator+=(
			long nDays )		// @Parm the number of days to add
{
	*this = RDate( m_lDate + nDays );
	return *this;
}

//
// @MFunc Return the date that is the given number of days before this date
//
// @RDesc the date that is nDays prior to this date
//
INLINE RDate RDate::operator-(
			long nDays ) const	// @Parm the number of days to subtract
{
	return RDate( m_lDate - nDays );
}

//
// @MFunc Subtract the given number of days from this date
//
// @RDesc const reference to this date
//
INLINE const RDate& RDate::operator-=(
			long nDays )	// @Parm the number of days to subtract
{
	*this = RDate( m_lDate - nDays );
	return *this;
}

//
// @MFunc Get the span between this date and the given date
//
// @RDesc the date span
//
INLINE long RDate::operator-(
			const RDate& rhs ) const	// @Parm the date to subtract
{
	return m_lDate - rhs.m_lDate;
}

//
// @MFunc Return TRUE if this date is in a leap year
//
// @RDesc TRUE if this is a leap year else FALSE
//
INLINE BOOLEAN RDate::IsLeapYear() const
{
	return BOOLEAN( GetLeapDays(m_uwYear) );
}

//
// @MFunc Return the number of days in this month for this year
//
// @RDesc the number of days in this month
//
// @Comm  If this date is a leap year, and the month is february(2), then this returns 29
//
INLINE uBYTE RDate::GetDaysInMonth() const
{
	return uBYTE( GetDaysInMonth(m_ubMonth,m_uwYear) );
}

//
// @MFunc Return the number of days in this year
//
// @RDesc the number of days in this year
//
INLINE int RDate::GetDaysInYear() const
{
	return GetDaysInYear( m_uwYear );
}

//
// @MFunc Return the day of the month for this date
//
// @RDesc the day of the month (1-nDaysInMonth) 
//
INLINE uBYTE RDate::GetDay() const
{
	return m_ubDay;
}

//
// @MFunc Return the month for this date
//
// @RDesc the month in this date (1-12)
//
INLINE uBYTE RDate::GetMonth() const
{
	return m_ubMonth;
}

//
// @MFunc Return the full year of this date (including century)
//
// @RDesc the year in this date
//
INLINE uWORD RDate::GetYear() const
{
	return m_uwYear;
}

//
// @MFunc Return the day of the week for this date, using the given number for sunday
//
// @RDesc the day of the week (1-7) of this date, based on the given sunday index
//
// @Comm  This will always return a number 1-7, so if you set sunday to 7, then
//        monday is 1.
//
INLINE uBYTE RDate::GetDayOfWeek(
			uBYTE ubSunday ) const	// @Parm the index (1-7) to be used as sunday
{
	TpsAssert( ubSunday>=1 && ubSunday<=7, "Attempting to set sunday to a non-weekday" );
	//
	// Compute day of week.  The base date's first day of the week is given
	// in kRDateBaseDayOfWeek with sunday=1...
	return uBYTE( ((m_lDate+(kRDateBaseDayOfWeek-1)+(ubSunday-1)) % 7) + 1 );
}

//
// Serialization
//
INLINE RArchive& operator>>( RArchive& ar, RDate& date )
{
	date.Read( ar );
	return ar;
}

INLINE RArchive& operator<<( RArchive& ar, const RDate& date )
{
	date.Write( ar );
	return ar;
}


//
//	@MFunc Constructor
// Private constructor using the number of minutes since midnight
//
INLINE RTime::RTime( 
			uLONG ulTime,							//	@Parm	 number of minutes since midnight
			BOOLEAN f24HourFormat ) :			//	@Parm  the format for the time
	m_ubHour( static_cast<uBYTE>( ulTime / kMinutesInHour ) % kHoursInDay ),
	m_ubMinute( static_cast<uBYTE>( ulTime % kMinutesInHour ) ),
	m_f24HourFormat( f24HourFormat )
{
	m_lTime = ( m_ubHour * kMinutesInHour ) + m_ubMinute;
}

//
// @MFunc Constructor
//
INLINE RTime::RTime( ) :
	m_lTime( 0 ),
	m_ubHour( 0 ),
	m_ubMinute( 0 ),
	m_f24HourFormat( FALSE )
{
}

//
// @MFunc Constructor with Hours and Minutes
//
INLINE RTime::RTime(
			uBYTE ubHour,					// @Parm number of minutes in time
			uBYTE ubMinute,				//	@Parm number of minutes in time
			BOOLEAN f24HourFormat) :	//	@Parm format for the time
	m_lTime( ( ( ubHour % kHoursInDay ) * kMinutesInHour) + ubMinute ),
	m_ubHour( ubHour % kHoursInDay ),
	m_ubMinute( ubMinute ),
	m_f24HourFormat( f24HourFormat )
{
}

//
// @MFunc Copy Constructor
//
INLINE RTime::RTime( 
			const RTime& rhs ) :	//	@Parm rhs of constructor
	m_lTime( rhs.m_lTime ),
	m_ubHour( rhs.m_ubHour ),
	m_ubMinute( rhs.m_ubMinute ),
	m_f24HourFormat( rhs.m_f24HourFormat )
{
}

//
// @CMember Get the current Time
INLINE RTime RTime::CurrentTime()
{
#ifdef _WINDOWS
	SYSTEMTIME	tm;
	GetLocalTime( &tm );
	return RTime( uBYTE(tm.wHour), uBYTE(tm.wMinute) /*, FALSE*/ );
#else
// REVIEW - APR 4/9/97
// Added Mac date-time code
//#error "Need MAC RDate::CurrentDate() implementation"
	DateTimeRec	tm;
	::GetTime(&tm);
	return RTime( tm.hour, tm.minute, FALSE );
#endif
}

//
// @MFunc assignment operator
//
INLINE const RTime& RTime::operator=( 
			const RTime& rhs )	// @Parm rhs of assignment
{
	m_lTime				= rhs.m_lTime;
	m_ubHour				= rhs.m_ubHour;
	m_ubMinute			= rhs.m_ubMinute;
	m_f24HourFormat	= rhs.m_f24HourFormat;
	return *this;
}


//
// @MFunc Compare two Times for equality
//
// @RDesc TRUE if this time is the same as the specified time else FALSE
//					A change in format does not mean the times are different
//
INLINE int RTime::operator==(
			const RTime& rhs )	// @Parm const reference to RTime to compare to
			const
{
	return (m_lTime == rhs.m_lTime);
}

//
// @MFunc Compare two Times for inequality
//
// @RDesc TRUE if this time is NOT the same as the specified time else FALSE
//					A change in format does not mean the times are different
//
INLINE int RTime::operator!=(
			const RTime& rhs )	// @Parm const reference to RTime to compare to
			const
{
	return (m_lTime != rhs.m_lTime);
}

//
// @MFunc Compare two times
//
// @RDesc TRUE if this time is before the specified time else FALSE
//					A change in format does not mean the times are different
//
INLINE int RTime::operator<(
			const RTime& rhs )	// @Parm const reference to RTime to compare to
			const
{
	return (m_lTime < rhs.m_lTime);
}

//
// @MFunc Compare two times
//
// @RDesc TRUE if this times is after than the specified time else FALSE
//					A change in format does not mean the times are different
//
INLINE int RTime::operator>(
			const RTime& rhs )	// @Parm const reference to RTime to compare to
			const
{
	return (m_lTime > rhs.m_lTime);
}

//
// @MFunc Compare two times
//
// @RDesc TRUE if this Time is at or before the specified time else FALSE
//					A change in format does not mean the times are different
//
INLINE int RTime::operator<=(
			const RTime& rhs )	// @Parm const reference to RTime to compare to
			const
{
	return (m_lTime <= rhs.m_lTime);
}

//
// @MFunc Compare two times
//
// @RDesc TRUE if this time is at or after the specified time else FALSE
//					A change in format does not mean the times are different
//
INLINE int RTime::operator>=(
			const RTime& rhs )	// @Parm const reference to RTime to compare to
			const
{
	return (m_lTime >= rhs.m_lTime);
}


//
// @MFunc Return the time that is the given number of minutes after this time
//
// @RDesc the time that is nMinutes after this time
//
INLINE RTime RTime::operator+(
		uLONG ulMinutes ) const	// @Parm the number of minutes to add
{
	return RTime( m_lTime+ulMinutes, m_f24HourFormat );
}

//
// @MFunc Add the given number of minutes to this date
//
// @RDesc const reference to this time
//
INLINE const RTime& RTime::operator+=(
			uLONG ulMinutes )		// @Parm the number of minutes to add
{
	*this = RTime( m_lTime + ulMinutes, m_f24HourFormat );
	return *this;
}

//
// @MFunc Return the time that is the given number of minutes before this time
//
// @RDesc the time that is ulMinutes prior to this time
//
INLINE RTime RTime::operator-(
			uLONG ulMinutes ) const	// @Parm the number of minutes to subtract
{
	uLONG tempTime = m_lTime;

	while( tempTime < ulMinutes )
		tempTime += kMaxMinutesInTime;

	return RTime( tempTime - ulMinutes, m_f24HourFormat );
}

//
// @MFunc Subtract the given number of minutes from this time
//
// @RDesc const reference to this time
//
INLINE const RTime& RTime::operator-=(
			uLONG ulMinutes )	// @Parm the number of days to subtract
{
	while( m_lTime < ulMinutes )
		m_lTime += kMaxMinutesInTime;

	*this = RTime( m_lTime - ulMinutes, m_f24HourFormat );
	return *this;
}

//
// @MFunc Retrieve the number of hours in this time
//
// @RDesc m_ubHour
//
INLINE uBYTE RTime::GetHour( ) const
{
	return m_ubHour;
}

//
// @MFunc Retreive the number of minutes in this time
//
// @RDesc m_ubMinutes
//
INLINE uBYTE RTime::GetMinutes( ) const
{
	return m_ubMinute;
}

//
// @MFunc Member retrieval
//
// @RDesc m_f24HourFormat
//
INLINE BOOLEAN RTime::Get24HourFormat( ) const
{
	return m_f24HourFormat;
}

//
// @MFunc Set the format for this time
//
// @RDesc m_f24HourFormat
//
INLINE void RTime::Set24HourFormat( BOOLEAN f24HourFormat )
{
	m_f24HourFormat = f24HourFormat;
}

//
// Serialization
//
INLINE RArchive& operator>>( RArchive& ar, RTime& time )
{
	time.Read( ar );
	return ar;
}

INLINE RArchive& operator<<( RArchive& ar, const RTime& time )
{
	time.Write( ar );
	return ar;
}

//
// @MFunc Construct an RDateTime
// @Syntax	RDateTime()
// @Syntax	RDateTime( const RDateTime& datetime )
// @Syntax	RDateTime( const RDate& date )
// @Syntax	RDateTime( const RTime& time )
// @Syntax	RDateTime( const RDate& date, const RTime& time )
//
// @Parm const RDateTime& | datetime | datetime to copy
// @Parm const RDate& | date | date to initialize with
// @Parm const RDate& | time | time to initialize with
//
INLINE RDateTime::RDateTime( )
#ifdef	TPSDEBUG
	: m_fDateSet( FALSE )
	, m_fTimeSet( FALSE )
#endif
{
}

//
// copy ctor
//
INLINE RDateTime::RDateTime( const RDateTime& datetime ) :
	m_date( datetime.m_date ),
	m_time( datetime.m_time )
#ifdef	TPSDEBUG
	, m_fDateSet( datetime.m_fDateSet )
	, m_fTimeSet( datetime.m_fTimeSet )
#endif
{
}

//
// ctor with date only
//
INLINE RDateTime::RDateTime( const RDate& date ) :
	m_date( date )
#ifdef	TPSDEBUG
	, m_fDateSet( TRUE )
	, m_fTimeSet( FALSE )
#endif
{
}

//
// ctor with time only
//
INLINE RDateTime::RDateTime( const RTime& time ) :
	m_time( time )
#ifdef	TPSDEBUG
	, m_fDateSet( FALSE )
	, m_fTimeSet( TRUE )
#endif
{
}

//
// ctor with Date & Time
//
INLINE RDateTime::RDateTime( const RDate& date, const RTime& time ) :
	m_date( date ),
	m_time( time )
#ifdef	TPSDEBUG
	, m_fDateSet( TRUE )
	, m_fTimeSet( TRUE )
#endif
{
}

//
// @MFunc Assignment operator
//
INLINE const RDateTime& RDateTime::operator=( const RDateTime& rhs )
{
	if( &rhs != this )
		{
		m_date	= rhs.m_date;
		m_time	= rhs.m_time;
#ifdef	TPSDEBUG
		m_fDateSet	= rhs.m_fDateSet;
		m_fTimeSet	= rhs.m_fTimeSet;
#endif
		}
	return *this;
}

//
// @MFunc Member retrieval
//
INLINE RDate RDateTime::GetDate( ) const
{
	return m_date;
}

INLINE void RDateTime::SetDate( const RDate& rDate)
{
	m_date = rDate;
}

//
// @MFunc Member retrieval
//
INLINE RTime RDateTime::GetTime( ) const
{
	return m_time;
}

INLINE void RDateTime::SetTime( const RTime& rTime )
{
	m_time = rTime;
}

//
// Serialization
//
INLINE RArchive& operator>>( RArchive& ar, RDateTime& dateTime )
{
	dateTime.Read( ar );
	return ar;
}

INLINE RArchive& operator<<( RArchive& ar, const RDateTime& dateTime )
{
	dateTime.Write( ar );
	return ar;
}

