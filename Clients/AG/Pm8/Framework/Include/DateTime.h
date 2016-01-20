// ****************************************************************************
// @Doc
// @Module DateTime.h |
//
//  File Name:			DateTime.h
//
//  Project:			Renaissance
//
//  Author:				Michael G. Dunlap
//
//  Description:		Declaration of the RDate, RTime, and RDateTime class
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
//  $Logfile:: /PM8/Framework/Include/DateTime.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DATETIME_H_
#define		_DATETIME_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// @Const the base year (gregorian shift of 11 days occurred in 1752, so years prior to 1753 would be difficult)
const uWORD	kRDateBaseYear					= 1753;
//
// @Const day of week of jan 1 in the base year assuming sunday=1
const uBYTE	kRDateBaseDayOfWeek			= 2;
//
// @Const table of the number of days in each month in a non leap-year
const uBYTE	kRDateDaysInMonth[12]		= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
//
// @Const table of the number of days prior to the first of the month in a non leap-year
const int	kRDateDaysBeforeMonth[12]	= {  0, 31, 59, 90,120,151,181,212,243,273,304,334 };
//
// Just a note:
//  We use a table, but the number of days in a month can be calculated
//  as follows:
//
//	[   1    2    3    4    5    6    7    8    9    10   11   12 ]
//	[ 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 ]
//	[   31   28   31   30   31   30   31   31   30   31   30   31 ]
//
// if( m_ubMonth==2 ) return uBYTE( 28 + (IsLeapYear()? 1 : 0) );
// return uBYTE( (((m_ubMonth>>3) ^ m_ubMonth)&0x0001)? 31 : 30 );

//
// @ENum Specifier for strings
enum EDateString
{
	kLongMonthName,	// @EMem The long name for the month
	kShortMonthName,	// @EMem the short name for the month
	kLongDayName,		// @EMem the long name for the day of the week
	kShortDayName,		// @EMem the short name for the day of the week
	kDayNameInitial	// @EMem the smallest name for the day of the week
};

//
//	@ENum Identify the Days of the week
enum EDayOfWeek
{
	kSunday	= 1,
	kMonday,
	kTuesday,
	kWednesday,
	kThursday,
	kFriday,
	kSaturday,
};

//
// @Class Container for dates and date manipulation
class FrameworkLinkage RDate
{
//
// @Access Internal Data
private:
	//
	// @CMember The date as the number of days since January 1st, m_guwBaseYear
	long				m_lDate;
	//
	// @CMember the month of the year (1-12)
	uBYTE				m_ubMonth;
	//
	// @CMember the day of the month (1-nDaysInMonth)
	uBYTE				m_ubDay;
	//
	// @CMember the year (with century)
	uWORD				m_uwYear;
//
// @Access Global Data
private:
	static BOOLEAN			g_fStringsLoaded;
	static BOOLEAN			g_fFirstDayLoaded;
	static RMBCString		g_szLongDayNames[7];
	static RMBCString		g_szShortDayNames[7];
	static RMBCString		g_szDayNameInitials[7];
	static RMBCString		g_szLongMonthNames[12];
	static RMBCString		g_szShortMonthNames[12];
	static EDayOfWeek		g_eFirstDayOfWeek;
//
// @Access Internal Support
private:
	//
	// @CMember Compute the number of days since the base date (1/1/baseyear)
	static long		GetDaysSinceBase( uBYTE ubMonth, uBYTE ubDay, uWORD uwYear );
	//
	// @CMember Test if the given year is a leap year (1 if is, 0 if not)
	static int		GetLeapDays( uWORD uwYear );
	//
	// @CMember Get the number of days in the given year
	static int		GetDaysInYear( uWORD uwYear );
	//
	// @CMember Get the number of days before the given month in the given year
	static int		GetDaysBeforeMonth( uBYTE ubMonth, uWORD uwYear );
	//
	// @CMember Get the number of days in the given month in the given year
	static int		GetDaysInMonth( uBYTE ubMonth, uWORD uwYear );
	//
	// @CMember ctor with number of days since base
	RDate( long nDays );

//
// @Access Construction
public:
	//
	// @CMember default ctor
	RDate();
	//
	// @CMember ctor with month, day and year
	RDate( uBYTE month, uBYTE day, uWORD year );
	//
	// @CMember copy ctor
	RDate( const RDate& rhs );
	//
	// @CMember Get the current date
	static RDate CurrentDate();
	//
	//	@CMember Get the first Day in a monthly calendar grid week
	//		(i.e.  kSunday - english, kMonday - German, etc.)
	static EDayOfWeek	GetFirstDayOfWeek();
	//
	// @CMember assignment operator
	const RDate& operator=( const RDate& rhs );
//
// @Access Comparison Operators
public:
	//
	// @CMember return TRUE if this date is the same as the given date
	int operator==( const RDate& rhs ) const;
	//
	// @CMember return TRUE if this date is NOT the same as the given date
	int operator!=( const RDate& rhs ) const;
	//
	// @CMember return TRUE if this date is before the given date
	int operator<( const RDate& rhs ) const;
	//
	// @CMember return TRUE if this date is after the given date
	int operator>( const RDate& rhs ) const;
	//
	// @CMember return TRUE if this date is on or before the given date
	int operator<=( const RDate& rhs ) const;
	//
	// @CMember return TRUE if this date is on or after the given date
	int operator>=( const RDate& rhs ) const;
//
// @Access Operations
public:
	//
	// @CMember Read this date from the given archive
	void	Read( RArchive& ar );
	//
	// @CMember Write this date to the given archive
	void	Write( RArchive& ar ) const;
//
// @Access Manipulation
public:
	//
	// @CMember Increment this date
	const RDate&	operator++();
	//
	// @CMember Decrement this date
	const RDate&	operator--();
	//
	// @CMember Return the date that is the given number of days after this date
	RDate				operator+( long nDays ) const;
	//
	// @CMember Add the given number of days to this date
	const RDate&	operator+=( long nDays );
	//
	// @CMember Return the date that is the given number of days before this date
	RDate				operator-( long nDays ) const;
	//
	// @CMember Subtract the given number of days from this date
	const RDate&	operator-=( long nDays );
	//
	// @CMember Get the number of days between this date and the given date
	long				operator-( const RDate& rhs ) const;
//
// @Access Accessors
public:
	//
	// @CMember Return TRUE if this date is in a leap year
	BOOLEAN				IsLeapYear() const;
	//
	// @CMember Return the number of days in this month for this year
	uBYTE					GetDaysInMonth() const;
	//
	// @CMember Return the number of days prior to this month for this year
	int					GetDaysBeforeMonth() const;
	//
	// @CMember Return the number of days in this year
	int					GetDaysInYear() const;
	//
	// @CMember Return the day of the month (1-nDaysInMonth) for this date
	uBYTE					GetDay() const;
	//
	// @CMember Return the month (1-12) for this date
	uBYTE					GetMonth() const;
	//
	// @CMember Return the full year of this date (including century)
	uWORD					GetYear() const;
	//
	// @CMember Return the day of the week (1-7) for this date, using the given number for sunday
	uBYTE					GetDayOfWeek( uBYTE ubSunday=1 ) const;
	//
	// @CMember Return the specified name for the current date
	const RMBCString&	GetName( EDateString eWhat ) const;
	//
	// @CMember Return the specified name
	static const RMBCString&	GetName( EDateString eWhat, int nWhich );
};

FrameworkLinkage RArchive& operator>>( RArchive& ar, RDate& date );
FrameworkLinkage RArchive& operator<<( RArchive& ar, const RDate& date );


const unsigned int	kMinutesInHour		= 60;
const unsigned int	kHoursInDay			= 24;
const unsigned int	kHoursUntilNoon	= 12;
const unsigned int	kMaxMinutesInTime	= kHoursInDay * kMinutesInHour;
//
// @Class Container for Time and time manipulation
class FrameworkLinkage RTime
{
//
// @Access Internal Data
private:
	//
	// @CMember The time as the number of minutes since midnight
	uLONG				m_lTime;
	//
	// @CMember the Hour of the day (0-23)
	uBYTE				m_ubHour;
	//
	// @CMember the Minutes of the hour (0 - 59)
	uBYTE				m_ubMinute;
	//
	//	@CMember the 24hour time format
	BOOLEAN			m_f24HourFormat;
//
// @Access Internal Support
private:
	//
	//	@CMember Load the strings for AM & PM
	static void		LoadStrings( );
	//
	//	@CMember ctor with minutes from midnight
	RTime( uLONG ulTime, BOOLEAN f24HourFormat );
//
// @Access Global Data
private:
	static BOOLEAN			g_fStringsLoaded;
	static BOOLEAN			g_fLeadingZeros;
	static RMBCString		g_szSeparator;
	static RMBCString		g_szAM;
	static RMBCString		g_szPM;
//
// @Access Construction
public:
	//
	// @CMember default ctor
	RTime();
	//
	// @CMember ctor with hours and Minutes
	RTime( uBYTE ubHour, uBYTE ubMinutes, BOOLEAN f24HourFormat = FALSE );
	//
	// @CMember copy ctor
	RTime( const RTime& rhs );
	//
	// @CMember Get the current date
	static RTime CurrentTime();
	//
	// @CMember assignment operator
	const RTime& operator=( const RTime& rhs );
//
// @Access Comparison Operators
public:
	//
	// @CMember return TRUE if this date is the same as the given Time
	int operator==( const RTime& rhs ) const;
	//
	// @CMember return TRUE if this date is NOT the same as the given time
	int operator!=( const RTime& rhs ) const;
	//
	// @CMember return TRUE if this date is before the given time
	int operator<( const RTime& rhs ) const;
	//
	// @CMember return TRUE if this date is after the given time
	int operator>( const RTime& rhs ) const;
	//
	// @CMember return TRUE if this date is on or before the given time
	int operator<=( const RTime& rhs ) const;
	//
	// @CMember return TRUE if this date is on or after the given time
	int operator>=( const RTime& rhs ) const;
//
// @Access Operations
public:
	//
	// @CMember Read this time from the given archive
	void	Read( RArchive& ar );
	//
	// @CMember Write this time to the given archive
	void	Write( RArchive& ar ) const;
	//
	// @CMember Retrieve the static member for the separator
	const RMBCString&	GetSeparator( ) const;
	//
	// @CMember Retrieve the AM static string
	const RMBCString& GetAM( ) const;
	//
	// @CMember Retrieve the PM static string
	const RMBCString& GetPM( ) const;

//
// @Access Manipulation
public:
	//
	// @CMember Increment this time by 1 minute
	const RTime&	operator++();
	//
	// @CMember Decrement this date by 1 minute
	const RTime&	operator--();
	//
	// @CMember Return the time that is the given number of minutes after this time
	RTime				operator+( uLONG ulMinutes ) const;
	//
	// @CMember Add the given number of minutes to this time
	const RTime&	operator+=( uLONG ulMinutes );
	//
	// @CMember Return the time that is the given minutes before this time
	RTime				operator-( uLONG ulMinutes ) const;
	//
	// @CMember Subtract the given number of minutes from this time
	const RTime&	operator-=( uLONG ulMinutes );
//
// @Access Accessors
public:
	//
	// @CMember Return the Hours in this time (0-23)
	uBYTE					GetHour() const;
	//
	// @CMember Return the Minutes in this hour (0-59)
	uBYTE					GetMinutes() const;
	//
	// @CMember Return the specified name for the current date
	void					Format( RMBCString& time, RMBCString& ampm ) const;
	//
	// @CMember Return the 24Hour Format boolean
	BOOLEAN				Get24HourFormat( ) const;
	//
	// @CMember Set the 24Hour Format
	void					Set24HourFormat( BOOLEAN f24HourFormat );
	//
	// @CMember Return the specified name
//	static const RMBCString&	GetName( EDateString eWhat, int nWhich );
};

FrameworkLinkage RArchive& operator>>( RArchive& ar, RTime& time );
FrameworkLinkage RArchive& operator<<( RArchive& ar, const RTime& time );


//
// @Class Container for Date&Time and date time manipulation
class FrameworkLinkage RDateTime
{
//
// @Access Internal Data
private:
	//
	// @CMember The date portion
	RDate				m_date;
	//
	// @CMember the Time portion
	RTime				m_time;
	//
#ifdef	TPSDEBUG
	//
	//	@CMember The date had been set
	BOOLEAN			m_fDateSet;
	//
	//	@CMember The time had been set
	BOOLEAN			m_fTimeSet;
#endif	//	TPSDEBUG
//
// @Access Construction
public:
	//
	// @CMember default ctor
	RDateTime();
	//
	// @CMember copy ctor
	RDateTime( const RDateTime& datetime );
	//
	// @CMember Date only ctor
	RDateTime( const RDate& date );
	//
	// @CMember Time only ctor
	RDateTime( const RTime& time );
	//
	// @CMember Date & Time ctor
	RDateTime( const RDate& date, const RTime& time );
//
// @Access Manipulation
public:
	//
	//	@CMember Assignment Operator
	const RDateTime& operator=( const RDateTime& rhs );
//
// @Access Accessors
public:
	//
	//	@CMember return a copy of the date stored in this class
	RDate				GetDate( ) const;
	void				SetDate( const RDate& rDate );
	//
	//	@CMember	return a copy of the time stored in this class
	RTime				GetTime( ) const;
	void				SetTime( const RTime& rTime );

//
// @Access Operations
public:
	//
	// @CMember Read this time from the given archive
	void	Read( RArchive& ar );
	//
	// @CMember Write this time to the given archive
	void	Write( RArchive& ar ) const;
	//
	// @CMember Format this time to a given MBCString
	YCounter	FormatString( const RMBCString& formatString, RMBCString& outputString );
};

FrameworkLinkage RArchive& operator>>( RArchive& ar, RDateTime& time );
FrameworkLinkage RArchive& operator<<( RArchive& ar, const RDateTime& time );


#ifndef TPSDEBUG
#include	"DateTime.inl"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // !defined(_DATETIME_H_)
