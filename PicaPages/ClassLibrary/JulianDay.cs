using System;

namespace ClassLibrary
{
	public class JulianDay
	{
		private int m_JulianDay = 0;

		// The Julian day (JD) is expressed in days since:
		//		noon November 24, 4713 BC (Gregorian Calendar)
		//		noon January 1, 4713 BC (Julian Calendar)

		public int Month { get { int m = 0; int d = 0; int y = 0; ComputeMDY(out m, out d, out y); return m; } }
		public int Day   { get { int m = 0; int d = 0; int y = 0; ComputeMDY(out m, out d, out y); return d; } }
		public int Year  { get { int m = 0; int d = 0; int y = 0; ComputeMDY(out m, out d, out y); return y; } }
		public int DayOfWeek { get { int dayOfWeek = (m_JulianDay % 7) + 1; return dayOfWeek; } }

		public JulianDay()
			: this(0)
		{
		}
		
		public JulianDay(int julianDay)
		{
			m_JulianDay = julianDay;
		}
		
		public JulianDay(int month, int day, int year)
		{
			m_JulianDay = Create(month, day, year);
		}

		public static JulianDay FromY(int year)
		{
			return new JulianDay(1, 1, year);
		}

		public static implicit operator int(JulianDay day)
		{
			return day.m_JulianDay;
		}

		public static implicit operator JulianDay(int julianDay)
		{
			return new JulianDay(julianDay);
		}

		// Create a Julian day from a MDY date
		private static int Create(int month, int day, int year)
		{
			int a = (14 - month) / 12;
			int m = month + (a * 12) - 3;
			int m30 = (((m * 153) + 2) / 5);
			int y = year + 4800 - a;
			int y4 = y / 4;
			int y100 = y / 100;
			int y400 = y / 400;
			int lJulianDate = day + m30 + (y * 365) + y4 - y100 + y400 - 32045;
			return lJulianDate;
		}

		// Expand a Julian day to a MDY date
		public void ComputeMDY(out int month, out int day, out int year)
		{
			int j = m_JulianDay + 32044;
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

		// Compute a DateTime date from a Julian day
		public DateTime ToDateTime()
		{
			int month = 0; int day = 0; int year = 0;
			ComputeMDY(out month, out day, out year);
			return new DateTime(year, month, day);
		}

		public JulianDay IncrementMonth(int increment)
		{
			int month = 0; int day = 0; int year = 0;
			ComputeMDY(out month, out day, out year);
			return Create(month + increment, day, year);
		}

		public JulianDay IncrementDay(int increment)
		{
			return m_JulianDay + increment;
		}

		public JulianDay IncrementYear(int increment)
		{
			int month = 0; int day = 0; int year = 0;
			ComputeMDY(out month, out day, out year);
			return Create(month, day, year + increment);
		}
	}
}
