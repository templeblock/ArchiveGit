using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using System.Xml.Serialization;
using ClassLibrary;
using Id = System.Int64;
using JulianDay = System.UInt32;
using Millicents = System.Int64;

namespace Money
{
[DataContract]
[XmlRoot]
	public enum IncomeType
	{
		Add = TransactionType.Add,
		// Subtract removed
		Transfer = TransactionType.Transfer,
		AddPct = TransactionType.AddPct,
		// SubtractPct removed
		TransferPct = TransactionType.TransferPct,
		// Set removed
	}

[DataContract]
[XmlRoot]
	public class Income : ViewModelBase
	{
[XmlAttribute]
[DataMember]
		public Id Id { get { return _Id; } set { SetProperty(ref _Id, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Id _Id;
[XmlAttribute]
[DataMember]
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Name;
[XmlAttribute]
[DataMember]
		public string Description { get { return _Description; } set { SetProperty(ref _Description, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Description;
[XmlAttribute]
[DataMember] // Do not use internally; for serialization purposes only; use TaxExempt instead
[DependsOn("TaxExempt")]
		public TaxExemptFlags TaxExemptFlags { get { return TaxExempt.ToFlags(); } set { TaxExempt = value; } }
[XmlIgnore]
[IgnoreDataMember]
		public TaxExempt TaxExempt { get { return _TaxExempt; } set { SetProperty(ref _TaxExempt, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private TaxExempt _TaxExempt = new TaxExempt();
[XmlAttribute]
[DataMember]
		public int ReceivedBy { get { return _ReceivedBy; } set { SetProperty(ref _ReceivedBy, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _ReceivedBy;
[XmlAttribute]
[DataMember]
		public Frequency Frequency { get { return _Frequency; } set { SetProperty(ref _Frequency, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Frequency _Frequency;
[XmlAttribute]
[DataMember]
		public IncomeType Type { get { return _Type; } set { SetProperty(ref _Type, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private IncomeType _Type = IncomeType.Add;
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
[DependsOn("StartProfileDate")]
		public uint StartDate { get { return StartProfileDate.Value; } set { StartProfileDate.Value = value; } }
[XmlIgnore]
[IgnoreDataMember]
		public ProfileDate StartProfileDate { get { return _StartProfileDate; } set { SetProperty(ref _StartProfileDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private ProfileDate _StartProfileDate = new ProfileDate();
[XmlAttribute]
[DataMember] // Do not use internally; for serialization purposes only
[DependsOn("EndProfileDate")]
		public uint EndDate { get { return EndProfileDate.Value; } set { EndProfileDate.Value = value; } }
[XmlIgnore]
[IgnoreDataMember]
		public ProfileDate EndProfileDate { get { return _EndProfileDate; } set { SetProperty(ref _EndProfileDate, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private ProfileDate _EndProfileDate = new ProfileDate();
[XmlElement]
[DataMember]
		public DeductionCollection Deductions { get { return _Deductions; } set { SetProperty(ref _Deductions, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private DeductionCollection _Deductions = new DeductionCollection();
[XmlIgnore]
[IgnoreDataMember]
		public TransactionCollection Transactions { get { return _Transactions; } set { SetProperty(ref _Transactions, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private TransactionCollection _Transactions = new TransactionCollection();

		internal static Income DebugCreate(string name, Frequency frequency, IncomeType type, double amount, AccountType sourceAccount, AccountType targetAccount, TaxExemptFlags taxExempt, int receivedBy, ProfileCode startProfileCode, ProfileCode endProfileCode, IEnumerable<Deduction> deductions)
		{
			return DebugCreate(name, frequency, type, amount, sourceAccount, targetAccount, taxExempt, receivedBy, startProfileCode.ToProfileDate(), endProfileCode.ToProfileDate(), deductions);
		}

		private static Income DebugCreate(string name, Frequency frequency, IncomeType type, double amount, AccountType sourceAccount, AccountType targetAccount, TaxExemptFlags taxExempt, int receivedBy, ProfileDate startDate, ProfileDate endDate, IEnumerable<Deduction> deductions)
		{
			Income income = new Income();
			income.Name = name;
			income.Frequency = frequency;
			income.Type = type;
			income.Amount = amount.ToMillicents();
			income.SourceAccount = sourceAccount;
			income.TargetAccount = targetAccount;
			income.TaxExempt = taxExempt;
			income.ReceivedBy = receivedBy;
			income.StartProfileDate = startDate;
			income.EndProfileDate = endDate;
			income.Deductions = deductions.ToObservableCollection<DeductionCollection,Deduction>();
			return income;
		}

		public Income()
		{
			_Id = 0;
			_Name = null;
			_Description = null;
			_Frequency = Frequency.Never;
			_Type = IncomeType.Add;
			_Amount = 0;
			_SourceAccount = AccountType.NoAccount;
			_TargetAccount = AccountType.NoAccount;
			_TaxExempt = TaxExemptFlags.None;
			_ReceivedBy = 0;
			_StartProfileDate.Init();
			_EndProfileDate.Init();
		}

		public Income(Income income)
			: this()
		{
			if (income == null)
				return;

			_Id = income._Id;
			_Name = income._Name;
			_Description = income._Description;
			_Frequency = income._Frequency;
			_Type = income._Type;
			_Amount = income._Amount;
			_SourceAccount = income._SourceAccount;
			_TargetAccount = income._TargetAccount;
			_TaxExempt = income._TaxExempt;
			_ReceivedBy = income._ReceivedBy;
			_StartProfileDate = income._StartProfileDate;
			_EndProfileDate = income._EndProfileDate;

			_Transactions = income._Transactions;
			_Deductions = income._Deductions;
		}

		public override void Dispose()
		{
			base.Dispose();
			Transactions.Dispose();
			Deductions.Dispose();
//j			Transactions = new TransactionCollection();
//j			Deductions = new DeductionCollection();
		}

		internal void BuildTransactions()
		{
			_Transactions.Clear();

			// Add the Gross transaction
			Transaction transaction = new Transaction();
			transaction.Name = _Name;
			transaction.Frequency = _Frequency;
			transaction.Type = (TransactionType)_Type;
			transaction.Amount = _Amount;
			transaction.SourceAccount = _SourceAccount;
			transaction.TargetAccount = _TargetAccount;
			_Transactions.Add(transaction);

			// Copy Gross into GrossForFICA, GrossForFed, GrossForState, GrossForLocal
			Millicents GrossForFICA = _Amount;
			Millicents GrossForFed = _Amount;
			Millicents GrossForState = _Amount;
			Millicents GrossForLocal = _Amount;

			// For any deduction that has not reached it annual maximum...
			// Reduce taxable income for deductions that are exempt
			// Add the transaction for each deduction
			foreach (Deduction deduction in Deductions)
			{
				// Compare against deduction.AnnualMaxAmount

				// Reduce taxable income
				if (deduction.TaxExempt.FICA)
					GrossForFICA -= deduction.Amount;
				else
				if (deduction.TaxExempt.Federal)
					GrossForFed -= deduction.Amount;
				else
				if (deduction.TaxExempt.State)
					GrossForState -= deduction.Amount;
				else
				if (deduction.TaxExempt.Local)
					GrossForLocal -= deduction.Amount;

				// Add the transaction that subtracts the deduction
				transaction = new Transaction();
				transaction.Name = deduction.Name;
				transaction.Frequency = _Frequency;
				transaction.Type = (TransactionType)deduction.Type;
				transaction.Amount = deduction.Amount;
				transaction.TargetAccount = _TargetAccount;
				if (transaction.Type == TransactionType.Subtract || transaction.Type == TransactionType.SubtractPct)
					transaction.SourceAccount = AccountType.NoAccount;
				else
					transaction.SourceAccount = _TargetAccount;

				_Transactions.Add(transaction);
			}

			Person person = App.Model.ProfileHolder.Profile.PersonNumToPerson(_ReceivedBy);
			Tax.AddTaxTransactions(GrossForFICA, GrossForFed, GrossForState, GrossForLocal, this, person);
		}

		internal void SetDates(ProfileDate startDate, ProfileDate endDate)
		{
			_StartProfileDate = startDate;
			_EndProfileDate = endDate;
		}

		internal void RunPrepare(Document document)
		{
			// Update any profile dependent dates
			_StartProfileDate.UpdateFromProfile();
			_EndProfileDate.UpdateFromProfile();

			// Update the income's transactions
			if (_Transactions.Count == 0)
				BuildTransactions();

			// Update the document's IncomeBalances
			// For now, process all income, regardless of...
			//		Type, Deduction, ReceivedBy, SourceAccount, TargetAccount, TaxExempt
			// Spread the income evenly over the duration, taking later sampling into account
			JulianDay startDate = _StartProfileDate.JulianDay;
			JulianDay actualEndDate = _EndProfileDate.JulianDay;
			JulianDay endDate = (_Frequency != Frequency.OneTime ? actualEndDate : startDate);
			double days = _Frequency.ToDays();
			if (days > 0)
			{
				double sampleAmount = _Amount.ToDouble() / days;

				//j HACK ALERT: Since we will only sample the IncomeBalances on a yearly basis...
				// make sure that each duration is at least a year
				if (endDate - startDate < 365)
				{
					sampleAmount *= (endDate - startDate + 1);
					endDate = startDate + 365;
					sampleAmount /= (endDate - startDate + 1);
				}

				document.IncomeBalances.ModifyAmount(startDate, sampleAmount.ToMillicents());
				document.IncomeBalances.ModifyAmount(endDate, -sampleAmount.ToMillicents());
			}
		}

		internal void Run(Document document, JulianDay date)
		{
			// If the date is outside of the date range for this Income, get out
			JulianDay startDate = _StartProfileDate.JulianDay;
			JulianDay endDate = _EndProfileDate.JulianDay;
			if (date >= startDate && date <= endDate)
			{
				// Run the transactions for the given date
				_Transactions.Run(document, date, startDate, endDate);
			}
		}
	}

[DataContract]
[XmlRoot]
	public class IncomeCollection : ObservableCollection<Income>, IDisposable
	{
		public void Dispose()
		{
			foreach (Income income in this)
				income.Dispose();
			Clear();
		}

		internal void RunPrepare(Document document)
		{
			foreach (Income income in this)
			{
				income.RunPrepare(document);
			}
		}

		internal void Run(Document document, JulianDay date)
		{
			foreach (Income income in this)
			{
				income.Run(document, date);
			}
		}

		internal void AddAndRun(Income income, Document document)
		{
			if (income == null || Contains(income))
				return;

			Add(income);
			document.BackgroundRun(income.StartProfileDate.JulianDay, income.EndProfileDate.JulianDay);
		}

		internal void RemoveAndRun(Income income, Document document)
		{
			if (income == null || !Contains(income))
				return;

			Remove(income);
			document.BackgroundRun(income.StartProfileDate.JulianDay, income.EndProfileDate.JulianDay);
		}

		internal void UpdateAndRun(Income incomeOld, Income income, Document document)
		{
			if (incomeOld == null || income == null)
				return;
			if (incomeOld == income)
				return;

			int index = IndexOf(incomeOld);
			if (index >= 0)
				Remove(incomeOld);
			else
				index = Count;

			Insert(index, income);
			JulianDay startDate = Math.Min(incomeOld.StartProfileDate.JulianDay, income.StartProfileDate.JulianDay);
			JulianDay endDate = Math.Max(incomeOld.EndProfileDate.JulianDay, income.EndProfileDate.JulianDay);
			document.BackgroundRun(startDate, endDate);
		}
	}
}
