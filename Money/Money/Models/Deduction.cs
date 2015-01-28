using System;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Runtime.Serialization;
using System.Windows;
using System.Windows.Data;
using System.Xml.Serialization;
using ClassLibrary;
using Id = System.Int64;
using Millicents = System.Int64;

namespace Money
{
[DataContract]
[XmlRoot]
	public enum DeductionType
	{
		// Add removed
		Subtract = TransactionType.Subtract,
		Transfer = TransactionType.Transfer,
		// AddPct removed
		SubtractPct = TransactionType.SubtractPct,
		TransferPct = TransactionType.TransferPct,
		// Set removed
	}

[DataContract]
[XmlRoot]
	public class Deduction : ViewModelBase
	{
[XmlAttribute]
[DataMember]
	public string Name { get { return _Name; } set { SetProperty(ref _Name, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Name;
[XmlAttribute]
[DataMember] // Do not use internally; for serialization purposes only
[DependsOn("TaxExempt")]
		public TaxExemptFlags TaxExemptFlags { get { return TaxExempt.ToFlags(); } set { TaxExempt = value; } }
[XmlIgnore]
[IgnoreDataMember]
public TaxExempt TaxExempt { get { return _TaxExempt; } set { SetProperty(ref _TaxExempt, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private TaxExempt _TaxExempt = new TaxExempt();
[XmlAttribute]
[DataMember]
		public DeductionType Type { get { return _Type; } set { SetProperty(ref _Type, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private DeductionType _Type = DeductionType.Subtract;
[XmlAttribute]
[DataMember]
		public Millicents Amount { get { return _Amount; } set { SetProperty(ref _Amount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents _Amount;
[XmlAttribute]
[DataMember]
		public AccountType TargetAccount { get { return _TargetAccount; } set { SetProperty(ref _TargetAccount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private AccountType _TargetAccount;
[XmlAttribute]
[DataMember]
		public Millicents AnnualMaxAmount { get { return _AnnualMaxAmount; } set { SetProperty(ref _AnnualMaxAmount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Millicents _AnnualMaxAmount;

		internal static Deduction CreateDefault(Id id)
		{
			Deduction deduction = new Deduction();
			deduction.Name = "Untitled";
			deduction.Type = DeductionType.Subtract;
			return deduction;
		}

		internal static Deduction DebugCreate(string name, TaxExemptFlags taxExempt, DeductionType type, double amount, double annualMaxAmount, AccountType targetAccount)
		{
			Deduction deduction = new Deduction();
			deduction.Name = name;
			deduction.TaxExempt = taxExempt;
			deduction.Type = type;
			deduction.Amount = amount.ToMillicents();
			deduction.AnnualMaxAmount = annualMaxAmount.ToMillicents();
			deduction.TargetAccount = targetAccount;
			return deduction;
		}

		public Deduction()
		{
		}
	}

[DataContract]
[XmlRoot]
	public class DeductionCollection : ObservableCollection<Deduction>, IDisposable
	{
		public void Dispose()
		{
			foreach (Deduction deduction in this)
				deduction.Dispose();
			Clear();
		}
	}

	public class DeductionTypeToAmountPercentVisible : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return Visibility.Visible;

			DeductionType deductionType = (DeductionType)value;
			switch (deductionType)
			{
				case DeductionType.Subtract:
				case DeductionType.Transfer:
				default:
					return Visibility.Collapsed;

				case DeductionType.SubtractPct:
				case DeductionType.TransferPct:
					return Visibility.Visible;
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class DeductionTypeToAmountCurrencyVisible : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return Visibility.Visible;

			DeductionType deductionType = (DeductionType)value;
			switch (deductionType)
			{
				case DeductionType.Subtract:
				case DeductionType.Transfer:
				default:
					return Visibility.Visible;

				case DeductionType.SubtractPct:
				case DeductionType.TransferPct:
					return Visibility.Collapsed;
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class DeductionTypeToTargetAccountVisible : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return Visibility.Visible;

			DeductionType deductionType = (DeductionType)value;
			switch (deductionType)
			{
				case DeductionType.Subtract:
				case DeductionType.SubtractPct:
				default:
					return Visibility.Collapsed;

				case DeductionType.Transfer:
				case DeductionType.TransferPct:
					return Visibility.Visible;
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

	public class DeductionTypeToTargetAccountLabel : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;

			DeductionType deductionType = (DeductionType)value;
			switch (deductionType)
			{
				case DeductionType.Transfer:
				case DeductionType.TransferPct:
					return " to ";
				case DeductionType.Subtract:
				case DeductionType.SubtractPct:
					return " from ";
				default:
					return "";
			}
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}
}
