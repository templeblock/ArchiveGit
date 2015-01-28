using System;
using System.Globalization;
using System.Windows.Data;
using JulianDay = System.UInt32;

namespace ClassLibrary
{
	internal static class DateTimeExt
	{
		// Extension for DateTime
		// Compute a Julian day from a DateTime date
		internal static JulianDay ToJulian(this DateTime date)
		{
			return JulianDate.Create(date.Month, date.Day, date.Year);
		}
	}

	public class JulianToDateTime : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			JulianDay date = (JulianDay)value;
			if (date == 0) // Special case
				return null;
			return date.ToDateTime();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			DateTime dateTime = (DateTime)value;
			return dateTime.ToJulian();
		}
	}

	internal static class JulianDate
	{
		// A Julian day is expressed in days since:
		//	noon November 24, 4713 BC (Gregorian calendar algorithms USED in this class)
		// or...
		//	noon January 1, 4713 BC (Julian calendar algorithms NOT USED in this class)

		// BASELINE can be 0 for true Julian days, or can be set to any other practical value
		// This class will set BASELINE to Jan 1, 0000 so that all dates will fit into 21 bits (5741 years)
		private static int BASELINE = (int)Create(1, 1, 0);
		// MaxValue can be Uint32.MaxValue for true Julian days
		private const int MaxValue = 0x1FFFFF;
		public const int Mask = MaxValue;
		public const int Bits = 21;

		// Compute the number of days from a number of years
		internal static uint YearsToDays(double years)
		{
			return (uint)(years * 365.25);
		}

		// Compute the number of years from a number of days
		internal static double DaysToYears(uint days)
		{
			return (double)days / 365.25;
		}

		// Create a Julian day from a calendar year
		internal static JulianDay Years(int years)
		{
			return Create(1, 1, years);
		}

		// Create a Julian day from a calendar MDY date
		internal static JulianDay Create(int month, int day, int year)
		{
			int a = (14 - month) / 12;
			int m = month + (a * 12) - 3;
			int m30 = (((m * 153) + 2) / 5);
			int y = year + 4800 - a;
			int y4 = y / 4;
			int y100 = y / 100;
			int y400 = y / 400;
			int date = (day + m30 + (y * 365) + y4 - y100 + y400 - 32045);
			date -= BASELINE;
			if (date < 0) date = 0;
			if (date > MaxValue) date = MaxValue;
			return (JulianDay)date;
		}

		// Expand a Julian day to a calendar MDY date
		internal static void ComputeMDY(this JulianDay date, out int month, out int day, out int year, out int dayOfWeek)
		{
			int dateAdjusted = (int)date;
			if (dateAdjusted < 0) dateAdjusted = 0;
			if (dateAdjusted > MaxValue) dateAdjusted = MaxValue;
			dateAdjusted += BASELINE;

			// Mon == 1, Sun == 7
			dayOfWeek = (dateAdjusted % 7) + 1;

			int j = dateAdjusted + 32044;
			int g = j / 146097;
			int dg = j % 146097;
			int c = (((dg / 36524) + 1) * 3) / 4;
			int dc = dg - (c * 36524);
			int b = dc / 1461;
			int db = dc % 1461;
			int a = (((db / 365) + 1) * 3) / 4;
			int da = db - (a * 365);
			int y = (g * 400) + (c * 100) + (b * 4) + a;
			int m = (((da * 5) + 308) / 153) - 2;
			int d = da - (((m + 4) * 153) / 5) + 122;
			year = y - 4800 + ((m + 2) / 12);
			month = ((m + 2) % 12) + 1;
			day = d + 1;
		}

		// Compute the day of the week from a Julian day
		internal static int DayOfWeek(this JulianDay date)
		{
			int dateAdjusted = (int)date;
			if (dateAdjusted < 0) dateAdjusted = 0;
			if (dateAdjusted > MaxValue) dateAdjusted = MaxValue;
			dateAdjusted += BASELINE;

			// Mon == 1, Sun == 7
			int dayOfWeek = (dateAdjusted % 7) + 1;
			return dayOfWeek;
		}

		// Compute a DateTime date from a Julian day
		internal static DateTime ToDateTime(this JulianDay date)
		{
			if (date == 0)
				return new DateTime();

			int month; int day; int year; int dayOfWeek;
			date.ComputeMDY(out month, out day, out year, out dayOfWeek);
			try
			{
				return new DateTime(year, month, day);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
				return new DateTime();
			}
		}

		// Compute the month from a Julian day
		internal static int Month(this JulianDay date)
		{
			int month; int day; int year; int dayOfWeek;
			date.ComputeMDY(out month, out day, out year, out dayOfWeek);
			return month;
		}

		// Compute the day of the month from a Julian day
		internal static int Day(this JulianDay date)
		{
			int month; int day; int year; int dayOfWeek;
			date.ComputeMDY(out month, out day, out year, out dayOfWeek);
			return day;
		}

		// Compute the year from a Julian day
		internal static int Year(this JulianDay date)
		{
			int month; int day; int year; int dayOfWeek;
			date.ComputeMDY(out month, out day, out year, out dayOfWeek);
			return year;
		}

		internal static JulianDay IncrementMonth(this JulianDay date, int increment)
		{
			int month; int day; int year; int dayOfWeek;
			date.ComputeMDY(out month, out day, out year, out dayOfWeek);
			return Create(month + increment, day, year);
		}

		internal static JulianDay IncrementDay(this JulianDay date, int increment)
		{
			return (JulianDay)((int)date + increment);
		}

		internal static JulianDay IncrementYear(this JulianDay date, int increment)
		{
			int month; int day; int year; int dayOfWeek;
			date.ComputeMDY(out month, out day, out year, out dayOfWeek);
			return Create(month, day, year + increment);
		}
	}
}
