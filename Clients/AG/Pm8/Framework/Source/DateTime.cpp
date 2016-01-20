// ****************************************************************************
// @Doc
// @Module DateTime.cpp |
//
//  File Name:			DateTime.cpp
//
//  Project:			Renaissance
//
//  Author:				Michael G. Dunlap
//
//  Description:		Implementation of the RDate, RTime, and RDateTime classes (non inlines)
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
//  $Logfile:: /PM8/Framework/Source/DateTime.cpp                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"DateTime.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"ApplicationGlobals.h"
#include	"ResourceManager.h"
#include	"FrameworkResourceIds.h"

#ifdef MAC
#include	<stdio.h>
#endif	// Mac

//
// Globals
BOOLEAN			RDate::g_fStringsLoaded		= FALSE;
BOOLEAN			RDate::g_fFirstDayLoaded	= FALSE;
RMBCString		RDate::g_szLongDayNames[7];
RMBCString		RDate::g_szShortDayNames[7];
RMBCString		RDate::g_szDayNameInitials[7];
RMBCString		RDate::g_szLongMonthNames[12];
RMBCString		RDate::g_szShortMonthNames[12];
EDayOfWeek		RDate::g_eFirstDayOfWeek	= kSunday;

//
// Private constructor using the number of days since the base date
//
RDate::RDate( long nDays ) :
	m_lDate( nDays ),
	m_ubMonth( 1 ),
	m_ubDay( 1 ),
	m_uwYear( kRDateBaseYear )
{
	long	tmp;

	while( nDays >= (tmp=GetDaysInYear()) )
		{
		nDays -= tmp;
		++m_uwYear;
		}

	while( nDays >= (tmp=GetDaysInMonth()) )
		{
		nDays -= tmp;
		++m_ubMonth;
		}

	m_ubDay = uBYTE( nDays + 1 );
}

//
// @MFunc Compute the number of days between the base date and the given date
//
// @RDesc the number of days
//
long RDate::GetDaysSinceBase(
			uBYTE	ubTargetMonth,	// @Parm the target month
			uBYTE	ubTargetDay,	// @Parm the target day of the month
			uWORD	uwTargetYear )	// @Parm the target year
{
	uBYTE	ubMonth	= 1;
	uBYTE	ubDay		= 1;
	uWORD	uwYear	= kRDateBaseYear;
	long	nDays		= 0L;

	TpsAssert( uwTargetYear>=kRDateBaseYear, "Year prior to base year not possible" );
	TpsAssert( ubTargetMonth>=1 && ubTargetMonth<=12, "Invalid month" );
	TpsAssert( ubTargetDay>=1 && ubTargetDay<=GetDaysInMonth(ubTargetMonth,uwTargetYear), "Invalid day" );

	//
	// Compute the number of days...
	while( uwYear < uwTargetYear )
		{
		int	nLeapDays	= GetLeapDays( uwYear );
		if( nLeapDays && (uwTargetYear-uwYear)>3 )
			{
			nDays		+= long( 365*4 + 1 );
			uwYear	= uWORD( uwYear + 4 );
			}
		else
			{
			nDays += long( 365 + nLeapDays );
			++uwYear;
			}
		}
	return nDays + GetDaysBeforeMonth(ubTargetMonth,uwTargetYear) + ubTargetDay-1;
}

//
// @MFunc Return the first day for the week in a Monthly calendar
//				This value is different for some Locals (ie German is kMonday)
//
//	@RDesc m_eFirstDayOfWeek
EDayOfWeek RDate::GetFirstDayOfWeek()
{
	if( !g_fFirstDayLoaded )
		{
		//	I have tried to load the string...
		g_fFirstDayLoaded = TRUE;

		RMBCString	day = GetResourceManager( ).GetResourceString( STRING_FIRST_DAY_IN_CALENDAR_GRID );
		if( !day.IsEmpty() )
			{
			int			nDayAsInt	= atoi( (LPCSTR)day );
			EDayOfWeek	eDay			= EDayOfWeek( nDayAsInt );
			//	If date is in valid range...
			if ( eDay>=kSunday || eDay <= kSaturday )
				g_eFirstDayOfWeek = eDay;
			else
				{
				TpsAssertAlways( "Day in resource file is not in valid range" );
				}
			}
		}

	return g_eFirstDayOfWeek;
}


//
// @MFunc Increment this date
//
// @RDesc const reference to this date
//
const RDate& RDate::operator++()
{
	++m_lDate;
	if( (++m_ubDay) > GetDaysInMonth() )
		{
		m_ubDay	= 1;
		if( (++m_ubMonth) > 12 )
			{
			++m_uwYear;
			m_ubMonth	= 1;
			}
		}
	return *this;
}

//
// @MFunc Decrement this date
//
// @RDesc const reference to this date
//
const RDate& RDate::operator--()
{
	--m_lDate;
	if( (--m_ubDay)==0 )
		{
		if( (--m_ubMonth)==0 )
			{
			--m_uwYear;
			m_ubMonth	= 12;
			}
		m_ubDay = GetDaysInMonth();
		}
	return *this;
}


//
// @MFunc Return the specified name for the current date
//
// @RDesc const reference to the requested string
//
const RMBCString&	RDate::GetName(
			EDateString	eWhat )	// @Parm identifier of the requested string
			const
{
	if( eWhat==kLongMonthName || eWhat==kShortMonthName )
		return GetName( eWhat, int(GetMonth()) );
	return GetName( eWhat, int(GetDayOfWeek()) );
}

//
// @MFunc Return the specified name
//
// @RDesc const reference to the requested string
//
const RMBCString&	RDate::GetName(
			EDateString	eWhat,	// @Parm identifier of the requested string
			int			nWhich )	// @Parm index of the requested string (bounds checked)
{
	static RMBCString	szEmpty( "" );

	if( !g_fStringsLoaded )
		{
		//
		// Load the strings...
#ifdef _WINDOWS
		static LCTYPE	idShortMonthName[12]	=
			{
			LOCALE_SABBREVMONTHNAME1,	// january
			LOCALE_SABBREVMONTHNAME2,
			LOCALE_SABBREVMONTHNAME3,
			LOCALE_SABBREVMONTHNAME4,
			LOCALE_SABBREVMONTHNAME5,
			LOCALE_SABBREVMONTHNAME6,
			LOCALE_SABBREVMONTHNAME7,
			LOCALE_SABBREVMONTHNAME8,
			LOCALE_SABBREVMONTHNAME9,
			LOCALE_SABBREVMONTHNAME10,
			LOCALE_SABBREVMONTHNAME11,
			LOCALE_SABBREVMONTHNAME12
			};
		static LCTYPE	idLongMonthName[12]	=
			{
			LOCALE_SMONTHNAME1,	// january
			LOCALE_SMONTHNAME2,
			LOCALE_SMONTHNAME3,
			LOCALE_SMONTHNAME4,
			LOCALE_SMONTHNAME5,
			LOCALE_SMONTHNAME6,
			LOCALE_SMONTHNAME7,
			LOCALE_SMONTHNAME8,
			LOCALE_SMONTHNAME9,
			LOCALE_SMONTHNAME10,
			LOCALE_SMONTHNAME11,
			LOCALE_SMONTHNAME12
			};
		static LCTYPE	idLongDayName[7] =
			{
			LOCALE_SDAYNAME7,	// sunday
			LOCALE_SDAYNAME1,
			LOCALE_SDAYNAME2,
			LOCALE_SDAYNAME3,
			LOCALE_SDAYNAME4,
			LOCALE_SDAYNAME5,
			LOCALE_SDAYNAME6
			};
		static LCTYPE	idShortDayName[7] =
			{
			LOCALE_SABBREVDAYNAME7,	// sunday
			LOCALE_SABBREVDAYNAME1,
			LOCALE_SABBREVDAYNAME2,
			LOCALE_SABBREVDAYNAME3,
			LOCALE_SABBREVDAYNAME4,
			LOCALE_SABBREVDAYNAME5,
			LOCALE_SABBREVDAYNAME6
			};
		char	szTemp[ 50 ];
		int	iString;
		for( iString=0 ; iString<12 ; ++iString )
			{
			GetLocaleInfo( LOCALE_USER_DEFAULT, idLongMonthName[iString], szTemp, sizeof(szTemp) );
			g_szLongMonthNames[iString]	= szTemp;
			GetLocaleInfo( LOCALE_USER_DEFAULT, idShortMonthName[iString], szTemp, sizeof(szTemp) );
			g_szShortMonthNames[iString]	= szTemp;
			}
		for( iString=0 ; iString<7 ; ++iString )
			{
			GetLocaleInfo( LOCALE_USER_DEFAULT, idLongDayName[iString], szTemp, sizeof(szTemp) );
			g_szLongDayNames[iString]	= szTemp;
			GetLocaleInfo( LOCALE_USER_DEFAULT, idShortDayName[iString], szTemp, sizeof(szTemp) );
			g_szShortDayNames[iString]	= szTemp;
			//
			// Create the day name initial...
			szTemp[1]	= 0;
			g_szDayNameInitials[iString] = szTemp;
			}
		g_fStringsLoaded = TRUE;
#else
	#ifndef IMAGER
		#error "Need to get the local representations of date strings"
	#endif	// IMAGER
#endif
		}

	switch( eWhat )
		{
		case kLongMonthName:
			TpsAssert( nWhich>=1 && nWhich<=12, "Invalid month" );
			if( nWhich>=1 && nWhich<=12 )
				return g_szLongMonthNames[nWhich-1];
			break;
		case kShortMonthName:
			TpsAssert( nWhich>=1 && nWhich<=12, "Invalid month" );
			if( nWhich>=1 && nWhich<=12 )
				return g_szShortMonthNames[nWhich-1];
			break;
		case kLongDayName:
			TpsAssert( nWhich>=1 && nWhich<=7, "Invalid day of week" );
			if( nWhich>=1 && nWhich<=12 )
				return g_szLongDayNames[nWhich-1];
			break;
		case kShortDayName:
			TpsAssert( nWhich>=1 && nWhich<=7, "Invalid day of week" );
			if( nWhich>=1 && nWhich<=12 )
				return g_szShortDayNames[nWhich-1];
			break;
		case kDayNameInitial:
			TpsAssert( nWhich>=1 && nWhich<=7, "Invalid day of week" );
			if( nWhich>=1 && nWhich<=12 )
				return g_szDayNameInitials[nWhich-1];
			break;
		}
	return szEmpty;
}


//
// @MFunc Read this date from the given archive
//
// @RDesc nothing
//
void RDate::Read(
			RArchive&	ar )	// @Parm the archive to read from
{
	ar	>> m_ubMonth
		>> m_ubDay
		>> m_uwYear;
	m_lDate = GetDaysSinceBase( m_ubMonth, m_ubDay, m_uwYear );
}


//
// @MFunc Write this date to the given archive
//
// @RDesc nothing
//
void RDate::Write(
			RArchive&	ar )	// @Parm the archive to write to
			const
{
	ar	<< m_ubMonth
		<< m_ubDay
		<< m_uwYear;
}



BOOLEAN		RTime::g_fStringsLoaded = FALSE;
BOOLEAN		RTime::g_fLeadingZeros	= FALSE;
RMBCString	RTime::g_szSeparator;
RMBCString	RTime::g_szAM;
RMBCString	RTime::g_szPM;

//
// @MFunc Load the strings for AM & PM that are localized
//
// @RDesc Get the local information and set it into the strings
//
void	RTime::LoadStrings( )
{
	if( !g_fStringsLoaded )
		{
#ifdef _WINDOWS
		char			szData[ 32 ];
		GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STIME, szData, sizeof(szData) );
		g_szSeparator	= RMBCString( szData );
		GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_S1159, szData, sizeof(szData) );
		g_szAM			= RMBCString( szData );
		GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_S2359, szData, sizeof(szData) );
		g_szPM			= RMBCString( szData );
		GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szData, sizeof(szData) );
		g_fLeadingZeros	= BOOLEAN( szData[0]=='1' );
#else
	#ifndef IMAGER
		#error "Need time formatting info for locale"
	#endif	// IMAGER
#endif
		g_fStringsLoaded = TRUE;
		}
}

//
// @MFunc Read this time from the given archive
//
// @RDesc nothing
//
void RTime::Read(
			RArchive&	ar )	// @Parm the archive to read from
{
	ar	>> m_ubHour
		>> m_ubMinute;
	uWORD	uwValue;
	ar	>> uwValue;
	m_f24HourFormat	= static_cast< BOOLEAN >( uwValue );
	m_lTime = (m_ubHour * kMinutesInHour) + m_ubMinute;
}


//
// @MFunc Write this date to the given archive
//
// @RDesc nothing
//
void RTime::Write(
			RArchive&	ar )	// @Parm the archive to write to
			const
{
	ar	<< m_ubHour
		<< m_ubMinute;
	uWORD	uwValue;
	uwValue	= static_cast< uWORD >( m_f24HourFormat );
	ar << uwValue;
}


//
// @MFunc Format the time into the given MBCString
//
// @RDesc nothing
//
void RTime::Format(
			RMBCString&	time,					// @Parm	 the formatted string
			RMBCString&	ampm )				// @Parm  Set to the local specific Am or Pm designator if not 24hours
			const
{
	LoadStrings( );

#ifdef _WINDOWS

	CString	szTmp;
	if( m_f24HourFormat )
		{
		if( g_fLeadingZeros )
			szTmp.Format( "%02d%s%02d", int(m_ubHour), g_szSeparator, int(m_ubMinute) );
		else
			szTmp.Format( "%d%s%02d", int(m_ubHour), g_szSeparator, int(m_ubMinute) );
		}
	else
		{
		uBYTE		ubHour	= m_ubHour;
		BOOLEAN	fPM		= FALSE;
		if( ubHour >= kHoursUntilNoon )
			{
			fPM		= TRUE;
			ubHour	-= kHoursUntilNoon;
			}
		if( (ubHour == 0) && !m_f24HourFormat )
			ubHour = 12;
		if( g_fLeadingZeros )
			szTmp.Format( "%02d%s%02d", int(ubHour), g_szSeparator, int(m_ubMinute) );
		else
			szTmp.Format( "%d%s%02d", int(ubHour), g_szSeparator, int(m_ubMinute) );
		}

	time	= RMBCString( szTmp );

#else
	#ifndef IMAGER
		#error "Need to format time string"
	#endif	// IMAGER
#endif

	if ( m_ubHour >= kHoursUntilNoon)
		ampm	= g_szPM;
	else
		ampm	= g_szAM;
}

//
// @MFunc Return the separator field of the time
//
// @RDesc g_szSeparator
//
const RMBCString&	RTime::GetSeparator( ) const
{
	return g_szSeparator;
}
//
// @MFunc Return the AM field of the time
//
// @RDesc g_szAM
//
const RMBCString& RTime::GetAM( ) const
{
	return g_szAM;
}
//
// @MFunc Return the PM field of the time
//
// @RDesc g_szPM
//
const RMBCString& RTime::GetPM( ) const
{
	return g_szPM;
}

//
// @MFunc Read this date from the given archive
//
// @RDesc nothing
//
void RDateTime::Read(
			RArchive&	ar )	// @Parm the archive to read from
{
	ar	>> m_date
		>> m_time;
}


//
// @MFunc Write this date to the given archive
//
// @RDesc nothing
//
void RDateTime::Write(
			RArchive&	ar )	// @Parm the archive to write to
			const
{
	ar	<< m_date
		<< m_time;
}


//
// @MFunc Format the string that describes this data/time
//
// @RDesc The number of bytes copied
//
// %dM month name
// %daM abbreviated month name
// %dm month number (2 digits)
// %dam month number (minimum digits)
// %dD day name
// %daD abbreviated day name
// %dd day number (2 digits)
// %dad day number (minimum digits)
// %dC century (2 digits)
// %daC century (minimum digits)
// %dY year w/ century
// %dy year w/o century (2 digits)
// %day year w/o century (minimum digits)

// %th hour (default 12/24, 2 digits)
// %tah hour (default 12/24, minumum digits)
// %t4 hour (24 hour format, 2 digits)
// %ta4 hour (24 hour format, minimum digits)
// %t2 hour (12 hour format, 2 digits)
// %ta2 hour (12 hour format, minumum digits)
// %tm minutes (2 digits)
// %tam minutes (minimum digits)
// %t: time separator
// %ta: (abbreviated is ignored) time separator
// %tp am/pm
// %tP AM/PM
// %tap a/p
// %taP A/P
//
const RCharacter kFormatIdentifierTag( '%' );
const	RCharacter kDayIdentifierTag( 'd' );
const	RCharacter kTimeIdentifierTag( 't' );
const RCharacter kAbbreviatedTag( 'a' );

const RCharacter kMonthNameTag( 'M' );
const RCharacter kMonthNumberTag( 'm' );
const RCharacter kDayNameTag( 'D' );
const RCharacter kDayNumberTag( 'd' );
const RCharacter kCenturyTag( 'C' );
const RCharacter kYearWithCentury( 'Y' );
const RCharacter kYearWithOutCentury( 'y' );

const RCharacter kDefaultHourTag( 'h' );
const RCharacter k24HourTag( '4' );
const RCharacter k12HourTag( '2' );
const RCharacter kMinutesTag( 'm' );
const RCharacter kTimeSeparator( ':' );
const RCharacter ksmAMPMTag( 'p' );
const RCharacter kCapAMPMTag( 'P' );

const	int	kLargestFormattedString	= 32;

YCounter RDateTime::FormatString( const RMBCString& szFormat, RMBCString& szOutput )
{
	sBYTE		sbBuffer[ kLargestFormattedString ];

	szOutput.Empty();
	RMBCStringIterator	itr = szFormat.Start();
	while( itr != szFormat.End() )
		{
		BOOLEAN	fAbbreviated = FALSE;
		if( kFormatIdentifierTag != (*itr) )
			{
			szOutput += *itr;
			}
		else
			{
			++itr;
			if( kTimeIdentifierTag == *itr )
				{
				//
				// time item
				++itr;
				if( kAbbreviatedTag == *itr )
					{
					fAbbreviated = TRUE;
					++itr;
					}

				RCharacter	timeChar	= *itr;
				if ( timeChar == kDefaultHourTag )
					{
					//
					// %th hour (default 12/24, 2 digits)
					// %tah hour (default 12/24, minumum digits)
					int	nHour	= int(m_time.GetHour());
					if( !m_time.Get24HourFormat() )
						{
						nHour %= 12;
						if( !nHour ) nHour = 12;
						}
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", nHour );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", nHour );
#endif
					szOutput += sbBuffer;
					}
				else if ( timeChar == k24HourTag )
					{
					//
					// %t4 hour (24 hour format, 2 digits)
					// %ta4 hour (24 hour format, minimum digits)
					int	nHour	= int(m_time.GetHour());
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", nHour );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", nHour );
#endif
					szOutput += sbBuffer;
					}
				else if ( timeChar == k12HourTag )
					{
					//
					// %t2 hour (12 hour format, 2 digits)
					// %ta2 hour (12 hour format, minumum digits)
					int	nHour	= int(m_time.GetHour())%12;
					if( !nHour ) nHour = 12;
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", nHour );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", nHour );
#endif
					szOutput += sbBuffer;
					}
				else if ( timeChar == kMinutesTag )
					{
					//
					// %tm minutes (2 digits)
					// %tam minutes (minimum digits)
					int	nMinutes = int(m_time.GetMinutes());
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", nMinutes );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", nMinutes );
#endif
					szOutput += sbBuffer;
					}
				else if ( timeChar == kTimeSeparator )
					{
					//
					// %t: time separator
					szOutput += m_time.GetSeparator();
					}
				else if ( timeChar == ksmAMPMTag )
					{
					//
					// %tp am/pm
					RMBCString	mbcTmp = (m_time.GetHour()>=12)? m_time.GetPM() : m_time.GetAM();
					mbcTmp.ToLower();
					if( fAbbreviated )
						szOutput += mbcTmp.Head(1);
					else
						szOutput += mbcTmp;
					}
				else if ( timeChar == kCapAMPMTag )
					{
					//
					// %tP AM/PM
					RMBCString	mbcTmp = (m_time.GetHour()>=12)? m_time.GetPM() : m_time.GetAM();
					mbcTmp.ToUpper();
					if( fAbbreviated )
						szOutput += mbcTmp.Head(1);
					else
						szOutput += mbcTmp;
					}
				else
					{
					throw kUnknownError;
					}
				}
			else if ( kDayIdentifierTag == *itr )
				{
				//
				// date item
				++itr;
				if( kAbbreviatedTag == *itr )
					{
					fAbbreviated = TRUE;
					++itr;
					}

				RCharacter dateChar = *itr;
				if ( dateChar == kMonthNameTag )
					{
					//
					// %dM month name
					// %daM abbreviated month name
					szOutput += m_date.GetName( fAbbreviated? kShortMonthName : kLongMonthName );
					}
				else if ( dateChar == kMonthNumberTag )
					{
					//
					// %dm month number (2 digits)
					// %dam month number (minimum digits)
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", int(m_date.GetMonth()) );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", int(m_date.GetMonth()) );
#endif
					szOutput += sbBuffer;
					}
				else if ( dateChar == kDayNameTag )
					{
					//
					// %dD day name
					// %daD abbreviated day name
					szOutput += m_date.GetName( fAbbreviated? kShortDayName : kLongDayName );
					}
				else if ( dateChar == kDayNumberTag )
					{
					//
					// %dd day number (2 digits)
					// %dad day number (minimum digits)
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", int(m_date.GetDay()) );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", int(m_date.GetDay()) );
#endif
					szOutput += sbBuffer;
					}
				else if ( dateChar == kCenturyTag )
					{
					//
					// %dC century (2 digits)
					// %daC century (minimum digits)
					int	nCentury	= int(m_date.GetYear())/100;
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", nCentury );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", nCentury );
#endif
					szOutput += sbBuffer;
					}
				else if ( dateChar == kYearWithCentury )
					{
					//
					// %dY year w/ century
#ifdef _WINDOWS
					wsprintf( sbBuffer, "%d", int(m_date.GetYear()) );
#else
					sprintf( sbBuffer, "%d", int(m_date.GetYear()) );
#endif
					szOutput += sbBuffer;
					}
				else if ( dateChar == kYearWithOutCentury )
					{
					//
					// %dy year w/o century (2 digits)
					// %day year w/o century (minimum digits)
					int	nYear	= int(m_date.GetYear())%100;
#ifdef _WINDOWS
					wsprintf( sbBuffer, fAbbreviated? "%d":"%02d", nYear );
#else
					sprintf( sbBuffer, fAbbreviated? "%d":"%02d", nYear );
#endif
					szOutput += sbBuffer;
					}
				else
					{
					throw kUnknownError;
					}
				}
			else
				{
				//
				// nothing special, so keep the percent...
				--itr;
				szOutput += *itr;
				}
			}
		++itr;
		}
	return szOutput.GetStringLength();
}



#ifdef TPSDEBUG
#include	"DateTime.inl"
#endif
