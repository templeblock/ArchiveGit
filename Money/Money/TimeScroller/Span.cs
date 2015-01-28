using System;
using System.Collections.ObjectModel;
using ClassLibrary;
using JulianDay = System.UInt32;

namespace Money
{
	internal enum SpanUnit { Week, Month, Year, Decade };

	internal class Span
	{
		public SpanUnit Unit { get { return m_Unit; } set { m_Unit = value; } }
		private SpanUnit m_Unit;

		internal JulianDay OffsetDate(JulianDay date, int spans)
		{
			DateTime startDateTime = date.ToDateTime();
			if (spans == 0)
				return date;

			switch (m_Unit)
			{
				//case SpanUnit.Hour:
				//{
				//    TimeSpan timeSpan = new TimeSpan(0, spans, 0, 0);
				//    startDateTime += timeSpan;
				//    break;
				//}
				//case SpanUnit.Day:
				//{
				//    TimeSpan timeSpan = new TimeSpan(spans, 0, 0, 0);
				//    startDateTime += timeSpan;
				//    break;
				//}
				case SpanUnit.Week:
				{
					TimeSpan timeSpan = new TimeSpan(7 * spans, 0, 0, 0);
					startDateTime += timeSpan;
					break;
				}
				case SpanUnit.Month:
				{
					int count = (spans > 0 ? spans : -spans);
					if (count > 12)
					{ // Move by years first
						startDateTime = new DateTime(startDateTime.Year + spans / 12, startDateTime.Month, startDateTime.Day);
						spans %= 12;
						count %= 12;
					}

					while (--count >= 0)
					{
						int month = startDateTime.Month + (spans < 0 ? -1 : 0);
						int year = startDateTime.Year;
						if (month == 0) { month = 12; year--; }
						TimeSpan timeSpan = new TimeSpan(DateTime.DaysInMonth(year, month), 0, 0, 0);
						startDateTime += (spans < 0 ? -timeSpan : timeSpan);
					}
					break;
				}
				case SpanUnit.Year:
				{
					startDateTime = new DateTime(startDateTime.Year + spans, startDateTime.Month, startDateTime.Day);
					break;
				}
				case SpanUnit.Decade:
				{
					spans *= 10;
					startDateTime = new DateTime(startDateTime.Year + spans, startDateTime.Month, startDateTime.Day);
					break;
				}
			}

			return startDateTime.ToJulian();
		}

		internal JulianDay GetStartOfSpan(JulianDay date)
		{
			DateTime dateTime = date.ToDateTime();
			switch (m_Unit)
			{
				//case SpanUnit.Hour:
				//    new DateTime(dateTime.Year, dateTime.Month, dateTime.Day, dateTime.Hour, 0, 0); // beginning of hour
				//    break;
				//case SpanUnit.Day:
				//    dateTime.Date; // beginning of day
				//    break;
				case SpanUnit.Week:
					dateTime = new DateTime(dateTime.Year, dateTime.Month, dateTime.Day) - new TimeSpan((int)dateTime.DayOfWeek, 0, 0, 0); // beginning of week, start on Sunday
					break;
				case SpanUnit.Month:
					dateTime = new DateTime(dateTime.Year, dateTime.Month, 1); // beginning of month
					break;
				case SpanUnit.Year:
					dateTime = new DateTime(dateTime.Year, 1, 1); // beginning of year
					break;
				case SpanUnit.Decade:
					dateTime = new DateTime(dateTime.Year - (dateTime.Year % 10), 1, 1); // beginning of decade
					break;
			}

			return dateTime.ToJulian();
		}

		internal string GetLabel(JulianDay date)
		{
			DateTime dateTime = date.ToDateTime();
			switch (m_Unit)
			{
				//case SpanUnit.Hour:
				//    return dateTime.ToString("MMMM dd, yyyy");
				//case SpanUnit.Day:
				//    return dateTime.ToString("MMMM dd, yyyy");
				case SpanUnit.Week:
					return "Week of " + dateTime.ToString("MMMM dd, yyyy");
				case SpanUnit.Month:
					return dateTime.ToString("MMMM, yyyy");
				case SpanUnit.Year:
					return dateTime.ToString("yyyy");
				case SpanUnit.Decade:
					return dateTime.ToString("yyyy");
			}

			return null;
		}

		internal int GetIntervals(JulianDay date)
		{
			switch (m_Unit)
			{
				//case SpanUnit.Hour: // 5 minute intervals
				//    return 12;
				//case SpanUnit.Day: // 12 2-hour blocks
				//    return 12;
				case SpanUnit.Week: // 7 days
					return 7;
				case SpanUnit.Month:
					DateTime dateTime = date.ToDateTime();
					return DateTime.DaysInMonth(dateTime.Year, dateTime.Month);
				case SpanUnit.Year: // 12 months
					return 12;
				case SpanUnit.Decade: // 10 years
					return 10;
			}

			return 0;
		}

		internal string GetText(JulianDay date, int i)
		{
			string text = null;
			switch (m_Unit)
			{
				//case SpanUnit.Hour: // 5 minute intervals
				//    text = new DateTime(1, 1, 1, dateTime.Hour, i * 5, 0).ToString("h:mm tt");
				//    break;
				//case SpanUnit.Day: // 12 2-hour blocks
				//    text = new DateTime(1, 1, 1, i * 2, 0, 0).ToString("h tt");
				//    break;
				case SpanUnit.Week: // 7 days
					// add days to a random Sunday
					text = new DateTime(1, 1, 7+i, 0, 0, 0).ToString("ddddddd");
					break;
				case SpanUnit.Month:
					text = (i + 1).ToString();
					break;
				case SpanUnit.Year: // 12 months
					text = new DateTime(1, 1+i, 1).ToString("MMM");
					break;
				case SpanUnit.Decade: // 10 years
					DateTime dateTime = date.ToDateTime();
					text = new DateTime(dateTime.Year + i, 1, 1).ToString("yyyy");
					break;
			}

			return text;
		}

		internal double PixelsPerDay(double viewportWidth)
		{
			double days = 0;
			switch (m_Unit)
			{
				//case SpanUnit.Hour:
				//    days = 1.0/24;
				//    break;
				//case SpanUnit.Day:
				//    days = 1;
				//    break;
				case SpanUnit.Week:
					days = 7.0;
					break;
				case SpanUnit.Month:
					days = 365.25 / 12;
					break;
				case SpanUnit.Year:
					days = 365.25;
					break;
				case SpanUnit.Decade:
					days = 3652.5;
					break;
				default:
					return 0;
			}

			return viewportWidth / days;
		}
	}

	internal class Spans : ObservableCollection<Span>
	{
		private static Collection<Span> m_Spans = Create();

		private static Collection<Span> Create()
		{
			Collection<Span> spans = new Collection<Span>();
			//spans.Add(new Span() { Unit = SpanUnit.Hour });
			//spans.Add(new Span() { Unit = SpanUnit.Day });
			spans.Add(new Span() { Unit = SpanUnit.Week });
			spans.Add(new Span() { Unit = SpanUnit.Month });
			spans.Add(new Span() { Unit = SpanUnit.Year });
			spans.Add(new Span() { Unit = SpanUnit.Decade });
			return spans;
		}

		internal static Span Find(SpanUnit unit)
		{
			foreach (Span view in m_Spans)
			{
				if (view.Unit == unit)
					return view;
			}
			return null;
		}

		internal static Span Find(string spanName)
		{
			foreach (Span view in m_Spans)
			{
				if (view.Unit.ToString() == spanName)
					return view;
			}
			return null;
		}
	}
}
