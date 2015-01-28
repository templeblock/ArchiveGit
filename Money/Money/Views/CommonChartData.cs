using System.Collections.Generic;
using ClassLibrary;
using JulianDay = System.UInt32;
using Millicents = System.Int64;

namespace Money
{
	public abstract class CommonChartData : ViewModelBase
	{
		public BalanceBoundaries Boundaries { get { return m_Boundaries; } set { SetProperty(ref m_Boundaries, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private BalanceBoundaries m_Boundaries = new BalanceBoundaries();

		public BalanceCollection Balances { get { return m_Balances; } set { SetProperty(ref m_Balances, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private BalanceCollection m_Balances = new BalanceCollection();

		public BalanceCollection TrendBalances { get { return m_TrendBalances; } set { SetProperty(ref m_TrendBalances, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private BalanceCollection m_TrendBalances = new BalanceCollection();

		private SplineFit m_TrendLine = new SplineFit();

//j		private const int k_PolyfitDegrees = 50;
		private const Frequency k_SamplingFrequency = Frequency.Annually;
		private static double k_SamplingFrequencyDays = k_SamplingFrequency.ToDays();

		public override void Dispose()
		{
			base.Dispose();
			Boundaries.Dispose();
			Balances.Dispose();
			TrendBalances.Dispose();
			Balances.Dispose();
		}

		internal abstract List<JulianDay> BuildDays(JulianDay startDate, JulianDay endDate);

		internal abstract BalanceList GetBalanceList();

		private double GetRawBalance(JulianDay date)
		{
			BalanceList balanceList = GetBalanceList();
			return balanceList.GetAmount(date, false/*startOfDay*/).ToDouble();
		}

		internal Millicents GetBalance(JulianDay date)
		{
			return (GetRawBalance(date) * k_SamplingFrequencyDays).ToMillicents();
		}

		internal Millicents GetTrendBalance(JulianDay date)
		{
			return (m_TrendLine.GetValue(date) * k_SamplingFrequencyDays).ToMillicents();
		}

		internal void Build()
		{
			JulianDay startDate = App.Model.ProfileHolder.Profile.StartProfileDate.JulianDay;
			JulianDay endDate = App.Model.ProfileHolder.Profile.EndProfileDate.JulianDay;
			if (startDate <= 0 || endDate <= 0)
				return;

			List<JulianDay> days = BuildDays(startDate, endDate);

			// Also create a SplineFit for the trend line from SamplingBalances 
//j			int numsamples = (int)((endDate - startDate) / k_SamplingFrequencyDays);
//j			int degree = Math.Min(k_PolyfitDegrees, numsamples);
			m_TrendLine.Init(0/*degree*/, startDate, endDate);

			// Compute the Balances, Boundaries, and SplineFit
			Millicents minBalance = 0;
			Millicents maxBalance = Millicents.MinValue;
			Balances.Clear();
			for (double doubledate = startDate; doubledate <= endDate; doubledate += k_SamplingFrequencyDays)
			{
				JulianDay date = (JulianDay)doubledate;
				double balance = GetRawBalance(date);
				Millicents milliBalance = (balance * k_SamplingFrequencyDays).ToMillicents();
				Balances.Add(new Balance(date, milliBalance));
				m_TrendLine.Add(date, balance);

				if (minBalance > milliBalance)
					minBalance = milliBalance;
				if (maxBalance < milliBalance)
					maxBalance = milliBalance;
			}

			Boundaries.StartDate = startDate;
			Boundaries.EndDate = endDate;
			Boundaries.MinBalance = minBalance;
			Boundaries.MaxBalance = maxBalance; //j (maxBalance * 105) / 100;

//j			foreach (JulianDay date in days)
//j				m_TrendLine.Add(date, GetRawBalance(date));
	
			// Compute the TrendBalances
			TrendBalances.Clear();
			m_TrendLine.Build();
			for (double doubledate = startDate; doubledate <= endDate; doubledate += k_SamplingFrequencyDays)
			{
				JulianDay date = (JulianDay)doubledate;
				Millicents trendBalance = (m_TrendLine.GetValue(date) * k_SamplingFrequencyDays).ToMillicents();
				TrendBalances.Add(new Balance(date, trendBalance));
			}
		}
	}

	public class IncomeChartData : CommonChartData
	{
		internal override BalanceList GetBalanceList()
		{
			return App.Model.DocumentHolder.Document.IncomeBalances;
		}

		internal override List<JulianDay> BuildDays(JulianDay startDate, JulianDay endDate)
		{
			if (startDate <= 0 || endDate <= 0)
				return null;

			// Create a temporary list of the key dates
			List<JulianDay> days = new List<JulianDay>();
			days.Add(startDate);
			days.Add(endDate);
			foreach (Income income in App.Model.DocumentHolder.Document.Incomes)
			{
				JulianDay start = income.StartProfileDate.JulianDay;
				JulianDay end = income.EndProfileDate.JulianDay;
				days.Add(start - 1);
				days.Add(start);
				days.Add((start + end - 1) / 2);
				days.Add(end - 1);
				days.Add(end);
			}
			days.Sort();
			return days;
		}
	}

	public class SpendingChartData : CommonChartData
	{
		internal override BalanceList GetBalanceList()
		{
			return App.Model.DocumentHolder.Document.SpendingBalances;
		}

		internal override List<JulianDay> BuildDays(JulianDay startDate, JulianDay endDate)
		{
			if (startDate <= 0 || endDate <= 0)
				return null;

			// Create a temporary list of the key dates
			List<JulianDay> days = new List<JulianDay>();
			days.Add(startDate);
			days.Add(endDate);
			foreach (Package package in App.Model.DocumentHolder.Document.Packages)
			{
				JulianDay start = package.StartProfileDate.JulianDay;
				JulianDay end = package.EndProfileDate.JulianDay;
				days.Add(start-1);
				days.Add(start);
				days.Add((start + end - 1) /2);
				days.Add(end-1);
				days.Add(end);
			}
			days.Sort();
			return days;
		}
	}

	public class PortfolioChartData : CommonChartData
	{
		internal override BalanceList GetBalanceList()
		{
			return App.Model.DocumentHolder.Document.SpendingBalances;
		}

		internal override List<JulianDay> BuildDays(JulianDay startDate, JulianDay endDate)
		{
			if (startDate <= 0 || endDate <= 0)
				return null;

			// Create a temporary list of the key dates
			List<JulianDay> days = new List<JulianDay>();
			days.Add(startDate);
			days.Add(endDate);
			foreach (Package package in App.Model.DocumentHolder.Document.Packages)
			{
				JulianDay start = package.StartProfileDate.JulianDay;
				JulianDay end = package.EndProfileDate.JulianDay;
				days.Add(start - 1);
				days.Add(start);
				days.Add((start + end - 1) / 2);
				days.Add(end - 1);
				days.Add(end);
			}
			days.Sort();
			return days;
		}
	}
}
