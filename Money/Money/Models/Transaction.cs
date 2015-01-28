using System;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Runtime.Serialization;
using System.Windows;
using System.Windows.Data;
using System.Xml.Serialization;
using ClassLibrary;
using Id = System.Int64;
using JulianDay = System.UInt32;
using Millicents = System.Int64;

namespace Money
{
[DataContract]
[XmlRoot]
	public enum TransactionType
	{
		Add,
		Subtract,
		Transfer,
		AddPct,
		SubtractPct,
		TransferPct,
		Set,
	}

[DataContract]
[XmlRoot]
	public enum Frequency
	{
		Daily,
		OneTime,
		Weekly,
		Monthly,
		Annually,
		Quarterly,		// four times a year (every three months)
		BiWeekly,		// every two weeks
		SemiMonthly,	// twice a month
		BiMonthly,		// every two months (six times a year)
		SemiWeekly,		// twice a week
		BiAnnually,		// every two years
		SemiAnnually,	// twice a year (every six months)
		Weekdays,
		Weekends,
		Never,
	}

[DataContract]
[XmlRoot]
	public class Transaction : ViewModelBase
	{
[XmlIgnore]
[IgnoreDataMember]
		public Id Id { get { return _Id; } set { SetProperty(ref _Id, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Id _Id;
[XmlIgnore]
[IgnoreDataMember]
		public Id PackageId { get { return _PackageId; } set { SetProperty(ref _PackageId, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Id _PackageId;
[XmlAttribute]
[DataMember]
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Name;
[XmlAttribute]
[DataMember]
		public Frequency Frequency { get { return _Frequency; } set { SetProperty(ref _Frequency, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Frequency _Frequency;
[XmlAttribute]
[DataMember]
		public TransactionType Type { get { return _Type; } set { SetProperty(ref _Type, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private TransactionType _Type;
[XmlAttribute]
[DataMember]
		public Millicents Amount { get { return _Amount; } set { SetProperty(ref _Amount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents _Amount;
[XmlAttribute]
[DataMember]
		public AccountType SourceAccount { get { return _SourceAccount; } set { SetProperty(ref _SourceAccount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private AccountType _SourceAccount;
[XmlAttribute]
[DataMember]
		public AccountType TargetAccount { get { return _TargetAccount; } set { SetProperty(ref _TargetAccount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private AccountType _TargetAccount;
[XmlAttribute]
[DataMember] // Do not use internally; for serialization purposes only
		public int Duration { get { return _Duration; } set { SetProperty(ref _Duration, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _Duration;
		public int _DurationDays { get { return (_Duration * _Frequency.ToDays()).ToInt32(); } }
[XmlAttribute]
[DataMember] // Do not use internally; for serialization purposes only
		public int StartLate { get { return _StartLate; } set { SetProperty(ref _StartLate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _StartLate;
		private int _SkipDays { get { return (_StartLate * _Frequency.ToDays()).ToInt32(); } }

		internal static Transaction CreateDefault(Id packageId)
		{
			Transaction transaction = Create("Untitled", Frequency.Monthly, TransactionType.Subtract, 1.0, AccountType.NoAccount, AccountType.Checking);
			transaction.PackageId = packageId;
			return transaction;
		}

		internal static Transaction Create(string name, Frequency frequency, TransactionType type, double amount, AccountType sourceAccount, AccountType targetAccount)
		{
			return Create(name, frequency, type, amount, sourceAccount, targetAccount, 0, 0);
		}

		internal static Transaction Create(string name, Frequency frequency, TransactionType type, double amount, AccountType sourceAccount, AccountType targetAccount, int duration, int startLate)
		{
			Transaction transaction = new Transaction();
			transaction.Id = IdHelper.RandomLongTimeOrder(); //j GuidToLong();
			transaction.Name = name;
			transaction.Frequency = frequency;
			transaction.Type = type;
			transaction.Amount = amount.ToMillicents();
			transaction.SourceAccount = sourceAccount;
			transaction.TargetAccount = targetAccount;
			transaction.Duration = duration;
			transaction.StartLate = startLate;
			return transaction;
		}

		public Transaction()
		{
			_Id = IdHelper.RandomLongTimeOrder(); //j GuidToLong();
			_Name = "";
			_Frequency = Frequency.Never;
			_Type = TransactionType.Subtract;
			_Amount = 0;
			_SourceAccount = AccountType.NoAccount;
			_TargetAccount = AccountType.NoAccount;
			_Duration = 0;
			_StartLate = 0;
		}

		public override void Dispose()
		{
			base.Dispose();
		}

		internal int DaysInMonth(int month, int year)
		{
			if (month == 2)
				return ((year % 4) == 0 ? 29 : 28);
			if (month == 9 || month == 4 || month == 6 || month == 11)
				return 30;
			return 31;
		}

		internal bool Run(Document document, JulianDay date, JulianDay parentStartDate, JulianDay parentEndDate)
		{
			if (document == null || _Frequency == Frequency.Never  || _TargetAccount == AccountType.NoAccount)
				return false;

			// Compute the start and end dates for the transaction
			JulianDay startDate =
				(JulianDay)(_SkipDays >= 0 ? parentStartDate + _SkipDays : parentEndDate + _SkipDays);
			JulianDay endDate =
				(JulianDay)(_DurationDays > 0 ? startDate + _DurationDays - 1 : parentEndDate + _DurationDays + 1);

			// Clip the start and end dates to the parent transaction
			if ((parentStartDate != 0 || parentEndDate != 0) &&
				(_SkipDays != 0 || _DurationDays != 0))
			{
				startDate = Math.Max(startDate, parentStartDate);
				endDate = Math.Min(endDate, parentEndDate);
			}

			// If the date is outside of the date range for this transaction, get out
			if (date < startDate || date > endDate)
				return false;

			// If this transaction should not be run, get out
			if (!_Frequency.IsDateMatch(date, startDate))
				return false;

			return ProcessTransaction(document, date);
		}

		private bool ProcessTransaction(Document document, JulianDay date)
		{
			// Determine the data we need for processing
			bool bNeedSourceBalance = false;
			bool bNeedTargetBalance = false;
			switch (_Type)
			{
				case TransactionType.Set:
				case TransactionType.Add:
				case TransactionType.Subtract:
				{
					bNeedSourceBalance = false;
					bNeedTargetBalance = true;
					break;
				}
				case TransactionType.AddPct:
				case TransactionType.SubtractPct:
				case TransactionType.Transfer:
				case TransactionType.TransferPct:
				{
					bNeedSourceBalance = true;
					bNeedTargetBalance = true;
					break;
				}
			}

			//j Could be faster if we only looked up the account(s) once
			Account targetAccount = null;
			Millicents targetBalance = 0;
			if (bNeedTargetBalance)
			{
				targetAccount = document.Accounts.Find(_TargetAccount, true/*bCreate*/);
				if (targetAccount == null)
					return false;
				targetBalance = targetAccount.GetBalance(date, false/*bStartOfTheDay*/);
			}

			Account sourceAccount = null;
			Millicents sourceBalance = 0;
			if (bNeedSourceBalance)
			{
				if (_SourceAccount == AccountType.NoAccount)
				{
					sourceAccount = targetAccount;
					sourceBalance = targetBalance;
				}
				else
				{
					sourceAccount = document.Accounts.Find(_SourceAccount, true/*bCreate*/);
					if (sourceAccount == null)
						return false;
					sourceBalance = sourceAccount.GetBalance(date, false/*bStartOfTheDay*/);
				}
			}

			// Process the transaction based on its type
			Millicents targetIncrement = 0;
			Millicents sourceIncrement = 0;
			switch (_Type)
			{
				case TransactionType.Set:
				{
					targetIncrement = _Amount - targetBalance;
					break;
				}
				case TransactionType.Add:
				case TransactionType.Subtract:
				{
					targetIncrement = _Amount;
					if (_Type == TransactionType.Subtract)
						targetIncrement = -targetIncrement;
					break;
				}
				case TransactionType.Transfer:
				{
					targetIncrement = _Amount;
					sourceIncrement = -targetIncrement;
					break;
				}
				case TransactionType.AddPct:
				case TransactionType.SubtractPct:
				{
					double fValue = _Amount.ToDoublePecent() * sourceBalance;
					targetIncrement = fValue.ToInt64();
					if (_Type == TransactionType.SubtractPct)
						targetIncrement = -targetIncrement;
					break;
				}
				case TransactionType.TransferPct:
				{
					double fValue = _Amount.ToDoublePecent() * sourceBalance;
					targetIncrement = fValue.ToInt64();
					sourceIncrement = -targetIncrement;
					break;
				}
			}

			// Ensure that the transaction can occur
			// If not, perform this transaction on the to account's fallback account
			//j	bool bOKtoProceed = (targetAccount.GetAllowNegative() || targetBalance + targetIncrement >= 0);
			//j if (!bOKtoProceed) return false;
			//j bOKtoProceed2 = (sourceAccount.GetAllowNegative() || sourceBalance + sourceIncrement >= 0);
			//j if (!bOKtoProceed) return false;

			if (sourceIncrement != 0)
			{
				sourceBalance += sourceIncrement;

				//j // Correct the daily balance if we need to
				//j if (!sourceAccount.GetAllowNegative() && sourceBalance < 0) sourceBalance = 0;

				// Set the account's daily balance
				sourceAccount.SetBalance(date, sourceBalance);
				document.Categories.IncreaseCategoryBalance(_Name, date, sourceIncrement);
			}

			if (targetIncrement != 0)
			{
				targetBalance += targetIncrement;

				//j // Correct the daily balance if we need to
				//j if (!targetAccount.GetAllowNegative() && targetBalance < 0) targetBalance = 0;

				// Set the account's daily balance
				targetAccount.SetBalance(date, targetBalance);
				document.Categories.IncreaseCategoryBalance(_Name, date, targetIncrement);
			}

			return true;
		}
	}

[DataContract]
[XmlRoot]
	public class TransactionCollection : ObservableCollection<Transaction>, IDisposable
	{
		public void Dispose()
		{
			foreach (Transaction transaction in this)
				transaction.Dispose();
			Clear();
		}

		internal void Run(Document document, JulianDay date, JulianDay startDate, JulianDay endDate)
		{
			foreach (Transaction transaction in this)
				transaction.Run(document, date, startDate, endDate);
		}
	}

	public class TransactionTypeToAmountPercentVisible : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return Visibility.Visible;

			TransactionType transactionType = (TransactionType)value;
			switch (transactionType)
			{
				case TransactionType.Add:
				case TransactionType.Subtract:
				case TransactionType.Set:
				case TransactionType.Transfer:
				default:
					return Visibility.Collapsed;

				case TransactionType.AddPct:
				case TransactionType.SubtractPct:
				case TransactionType.TransferPct:
					return Visibility.Visible;
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class TransactionTypeToAmountCurrencyVisible : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return Visibility.Visible;

			TransactionType transactionType = (TransactionType)value;
			switch (transactionType)
			{
				case TransactionType.Add:
				case TransactionType.Subtract:
				case TransactionType.Set:
				case TransactionType.Transfer:
				default:
					return Visibility.Visible;

				case TransactionType.AddPct:
				case TransactionType.SubtractPct:
				case TransactionType.TransferPct:
					return Visibility.Collapsed;
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class TransactionTypeToSourceAccountVisible : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return Visibility.Visible;

			TransactionType transactionType = (TransactionType)value;
			switch (transactionType)
			{
				case TransactionType.Add:
				case TransactionType.Subtract:
				case TransactionType.Set:
				default:
					return Visibility.Collapsed;

				case TransactionType.Transfer:
				case TransactionType.AddPct:
				case TransactionType.SubtractPct:
				case TransactionType.TransferPct:
					return Visibility.Visible;
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class TransactionTypeToSourceAccountLabel : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;

			TransactionType transactionType = (TransactionType)value;
			switch (transactionType)
			{
				case TransactionType.Add:
				case TransactionType.Subtract:
				case TransactionType.Set:
				default:
					return "";
				case TransactionType.Transfer:
					return " from ";
				case TransactionType.AddPct:
				case TransactionType.SubtractPct:
				case TransactionType.TransferPct:
					return " of ";
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class TransactionTypeToTargetAccountLabel : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;

			TransactionType transactionType = (TransactionType)value;
			switch (transactionType)
			{
				case TransactionType.Add:
				case TransactionType.AddPct:
				case TransactionType.Transfer:
				case TransactionType.TransferPct:
					return " to ";
				case TransactionType.Subtract:
				case TransactionType.SubtractPct:
					return " from ";
				case TransactionType.Set:
					return " into ";
				default:
					return "";
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	internal static class FrequencyExt
	{
		// Extension for Frequency
		internal static double ToDays(this Frequency frequency)
		{
			switch (frequency)
			{
				default:
				case Frequency.Never:			return 0;
				case Frequency.OneTime:			return 1;
				case Frequency.Daily:			return 1;
				case Frequency.Weekly:			return 7;
				case Frequency.Monthly:			return 365.25 / 12;
				case Frequency.Annually:		return 365.25;
				case Frequency.Quarterly:		return 365.25 / 4;	// four times a year (every three months)
				case Frequency.BiWeekly:		return 14;			// every two weeks (every fourteen days)
				case Frequency.SemiMonthly:		return 365.25 / 24;	// twice a month
				case Frequency.BiMonthly:		return 365.25 / 6;	// every two months (six times a year)
				case Frequency.SemiWeekly:		return 7 / 2;		// twice a week
				case Frequency.BiAnnually:		return 365.25 * 2;	// every two years
				case Frequency.SemiAnnually:	return 365.25 / 2;	// twice a year (every six months)
				case Frequency.Weekdays:		return 7 / 5;		// five times a week
				case Frequency.Weekends:		return 7 / 2;		// twice a week
			}
		}

		// Extension for Frequency
		internal static double ToAnnualPeriods(this Frequency frequency)
		{
			switch (frequency)
			{
				case Frequency.Never:			return 0;
				case Frequency.OneTime:			return 365.25;
				case Frequency.Daily:			return 365.25;
				case Frequency.Weekly:			return 52;
				case Frequency.Monthly:			return 12;
				case Frequency.Annually:		return 1;
				case Frequency.Quarterly:		return 4;		// four times a year (every three months)
				case Frequency.BiWeekly:		return 26;		// every two weeks (every fourteen days)
				case Frequency.SemiMonthly:		return 24;		// twice a month
				case Frequency.BiMonthly:		return 6;		// every two months (six times a year)
				case Frequency.SemiWeekly:		return 104.36;	// twice a week
				case Frequency.BiAnnually:		return 0.5;		// every two years
				case Frequency.SemiAnnually:	return 2;		// twice a year (every six months)
				case Frequency.Weekdays:		return 260.89;	// five times a week
				case Frequency.Weekends:		return 104.36;	// twice a week
			}

			return 0;
		}

		// Extension for Frequency
		internal static bool IsDateMatch(this Frequency frequency, JulianDay date, JulianDay startDate)
		{
			// Check to see if the date matches the frequency
			int month; int day; int year; int dayOfWeek;
			date.ComputeMDY(out month, out day, out year, out dayOfWeek);

			int startMonth; int startDay; int startYear; int startDayOfWeek;
			startDate.ComputeMDY(out startMonth, out startDay, out startYear, out startDayOfWeek);

			switch (frequency)
			{
				case Frequency.Never:
					return false;
				case Frequency.OneTime:
					return (date == startDate);
				case Frequency.Daily:
					return true;
				case Frequency.Weekdays:
					return (dayOfWeek <= 5);
				case Frequency.Weekends:
					return (dayOfWeek >= 6);
				case Frequency.Weekly:
					return (dayOfWeek == startDayOfWeek);
				case Frequency.SemiWeekly: // twice a week
					return (dayOfWeek == startDayOfWeek ||
						(startDayOfWeek <= 4 ? dayOfWeek == startDayOfWeek + 3
											 : dayOfWeek == startDayOfWeek - 3));
				case Frequency.BiWeekly: // every two weeks (every fourteen days)
					return ((date % 14) == (startDate % 14));
				case Frequency.Monthly:
					return (day == (startDay <= 28 ? startDay : Math.Min(startDay, DaysInMonth(month, year))));
				case Frequency.SemiMonthly: // twice a month
					return (day == (startDay <= 28 ? startDay : Math.Min(startDay, DaysInMonth(month, year))) ||
						(day == (startDay <= 14 ? startDay + 14 : (startDay <= 28 ? startDay - 14 : 15))));
				case Frequency.BiMonthly: // every two months (six times a year)
					return (day == (startDay <= 28 ? startDay : Math.Min(startDay, DaysInMonth(month, year))) &&
						((month % 2) == (startMonth % 2)));
				case Frequency.Quarterly: // four times a year (every three months)
					return (day == (startDay <= 28 ? startDay : Math.Min(startDay, DaysInMonth(month, year))) &&
						((month % 3) == (startMonth & 3)));
				case Frequency.Annually:
					return (day == (startDay <= 28 ? startDay : Math.Min(startDay, DaysInMonth(month, year))) &&
						(month == startMonth));
				case Frequency.SemiAnnually: // twice a year (every six months)
					return (day == (startDay <= 28 ? startDay : Math.Min(startDay, DaysInMonth(month, year))) &&
						((month % 6) == (startMonth & 6)));
				case Frequency.BiAnnually: // every two years
					return (day == (startDay <= 28 ? startDay : Math.Min(startDay, DaysInMonth(month, year))) &&
						(month == startMonth) && ((year % 2) == (startYear % 2)));
			}

			return false;
		}

		internal static int DaysInMonth(int month, int year)
		{
			if (month == 2)
				return ((year % 4) == 0 ? 29 : 28);
			if (month == 9 || month == 4 || month == 6 || month == 11)
				return 30;
			return 31;
		}
	}
}
