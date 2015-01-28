using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using ClassLibrary;
using JulianDay = System.UInt32;
using Millicents = System.Int64;

namespace Money
{
	public class Balance : ViewModelBase
	{
		public JulianDay Date { get { return _Date; } set { SetProperty(ref _Date, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private JulianDay _Date;

		public Millicents Amount { get { return _Amount; } set { SetProperty(ref _Amount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents _Amount;

		public Balance()
		{
			Date = 0;
			Amount = 0;
		}

		public Balance(JulianDay date, Millicents amount)
		{
			Date = date;
			Amount = amount;
		}
	}

	public class BalanceBoundaries : ViewModelBase
	{
		public Millicents MinBalance { get { return _MinBalance; } set { SetProperty(ref _MinBalance, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents _MinBalance;

		public Millicents MaxBalance { get { return _MaxBalance; } set { SetProperty(ref _MaxBalance, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents _MaxBalance;

		public JulianDay StartDate { get { return _StartDate; } set { SetProperty(ref _StartDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private JulianDay _StartDate;

		public JulianDay EndDate { get { return _EndDate; } set { SetProperty(ref _EndDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private JulianDay _EndDate;
	}

	public class BalanceCollection : ObservableCollection<Balance>, IDisposable
	{
		public void Dispose()
		{
			foreach (Balance balance in this)
				balance.Dispose();
			Clear();
		}
	}

	public class BalanceList : List<Balance>, IDisposable
	{
		private static BalanceComparer _BalanceComparer = new BalanceComparer();
		private Balance _WorkingBalance = new Balance(0, 0);
		private JulianDay _LastDate;
		private bool _LastStartOfTheDay;
		private Millicents _LastAmount;
		private int _LastIndex;

		public void Dispose()
		{
			Clear();
		}

		internal Millicents GetAmount(JulianDay date, bool startOfTheDay)
		{
			if (base.Count <= 0)
			{
				_LastDate = date;
				_LastStartOfTheDay = startOfTheDay;
				_LastAmount = 0;
				_LastIndex = -1;
				return _LastAmount;
			}

			if (_LastDate == date && _LastStartOfTheDay == startOfTheDay)
				return _LastAmount;

			try
			{
				_WorkingBalance.Date = date;
				int index = base.BinarySearch(_WorkingBalance, _BalanceComparer);
				if (index < 0)
				{
					// A negative index is the bitwise complement of the insertion point
					index = ~index; // index AT the insertion point
					index--; // index BEFORE the insertion point
				}
				else
				{
					// The target date was found
					index -= (startOfTheDay ? 1 : 0);
				}

				_LastDate = date;
				_LastStartOfTheDay = startOfTheDay;
				_LastAmount = (index >= 0 ? base[index].Amount : 0);
				_LastIndex = index;
				return _LastAmount;
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return 0;
		}

		internal void SetAmount(JulianDay date, Millicents amount)
		{
			if (_LastDate == date && _LastStartOfTheDay == false && _LastAmount == amount)
				return;

			try
			{
				_WorkingBalance.Date = date;
				int index = base.BinarySearch(_WorkingBalance, _BalanceComparer);
				if (index < 0)
				{
					// A negative index is the bitwise complement of the insertion point
					index = ~index; // index AT the insertion point
					base.Insert(index, new Balance(date, amount));
				}
				else
				{
					// The target date was found - modify the list item in place
					base[index].Amount = amount;
				}

				_LastDate = date;
				_LastStartOfTheDay = false;
				_LastAmount = amount;
				_LastIndex = index;

				// Truncate the list from this point on - the remaining entries are out of date
				TruncateList(++index);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}
		}

		internal void ModifyAmount(JulianDay date, Millicents amountIncrement)
		{
			if (amountIncrement == 0)
				return;

			try
			{
				_WorkingBalance.Date = date;
				int index = base.BinarySearch(_WorkingBalance, _BalanceComparer);
				if (index < 0)
				{
					// A negative index is the bitwise complement of the insertion point
					index = ~index; // index AT the insertion point
					Millicents amount = (index - 1 >= 0 ? base[index - 1].Amount : 0);
					base.Insert(index, new Balance(date, amount));
				}
				else
				{
					// The target date was found
				}

				_LastDate = date;
				_LastStartOfTheDay = false;
				_LastAmount = base[index].Amount + amountIncrement;
				_LastIndex = index;

				// The tail end of the list is incremented
				for (int i = index; i < base.Count; i++)
					base[i].Amount += amountIncrement;
			}
			catch (Exception ex)
			{
				ex.Alert();
			}
		}

		internal void Truncate(JulianDay date)
		{
			if (base.Count <= 0)
			{
				_LastDate = date;
				_LastStartOfTheDay = false;
				_LastAmount = 0;
				_LastIndex = -1;
				return;
			}

			try
			{
				_WorkingBalance.Date = date;
				int index = base.BinarySearch(_WorkingBalance, _BalanceComparer);
				if (index < 0)
				{
					// A negative index is the bitwise complement of the insertion point
					index = ~index; // index AT the insertion point
				}
				else
				{
					// The target date was found
				}

				_LastDate = date - 1;
				_LastStartOfTheDay = false;
				_LastAmount = (index - 1 >= 0 ? base[index - 1].Amount : 0);
				_LastIndex = index - 1;

				// Truncate the list from this point on - the remaining entries are out of date
				TruncateList(index);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}
		}

		private void TruncateList(int index)
		{
			try
			{
				int count = base.Count - index;
				if (count > 0)
					base.RemoveRange(index, count);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}
		}

		private class BalanceComparer : IComparer<Balance>
		{
			public int Compare(Balance dbx, Balance dby)
			{
				if (dbx.Date < dby.Date)
					return -1; // Less than zero if x is less than y

				if (dbx.Date > dby.Date)
					return 1; // Greater than zero x is greater than y

				return 0; // Zero if x equals y
			}
		}
	}
}
