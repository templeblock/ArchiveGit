using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Runtime.Serialization;
using System.Xml.Serialization;
using ClassLibrary;
using Id = System.Int64;
using JulianDay = System.UInt32;

namespace Money
{
[DataContract]
[XmlRoot]
	public class Package : ViewModelBase
	{
[XmlAttribute]
[DataMember]
		public Id Id { get { return _Id; } set { SetProperty(ref _Id, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Id _Id;
[XmlAttribute]
[DataMember]
		public Id ParentId { get { return _ParentId; } set { SetProperty(ref _ParentId, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private Id _ParentId;
[XmlAttribute]
[DataMember]
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Name;
[XmlAttribute]
[DataMember]
		public string Icon { get { return _Icon; } set { SetProperty(ref _Icon, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Icon;
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
		public TransactionCollection Transactions { get { return _Transactions; } set { SetProperty(ref _Transactions, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private TransactionCollection _Transactions = new TransactionCollection();
[XmlElement]
[DataMember]
		public PackageCollection Packages { get { return _Packages; } set { SetProperty(ref _Packages, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private PackageCollection _Packages = new PackageCollection();
[XmlAttribute]
[DataMember]
		public string Options { get { return _Options; } set { _Options = value; } }
		private string _Options;
[XmlAttribute]
[DataMember]
		public double Sort { get { return _Sort; } set { _Sort = value; } }
		private double _Sort;
[XmlIgnore]
[IgnoreDataMember]
		public bool TransactionsLoaded { get { return _TransactionsLoaded; } set { _TransactionsLoaded = value; } }
		private bool _TransactionsLoaded;

		internal static Package CreateDefault()
		{
			Package package = new Package();
			package._Id = IdHelper.RandomLongTimeOrder(); //j GuidToLong() RandomLong()
			return package;
		}

		internal static Package DebugCreate(string name, ProfileCode startProfileCode, ProfileCode endProfileCode, IEnumerable<Transaction> transactions)
		{
			return DebugCreate(name, startProfileCode.ToProfileDate(), endProfileCode.ToProfileDate(), transactions);
		}

		private static Package DebugCreate(string name, ProfileDate startDate, ProfileDate endDate, IEnumerable<Transaction> transactions)
		{
			Package package = Package.CreateDefault();
			package.Name = name;
			package.Icon = "unknown";
			package.StartProfileDate = startDate;
			package.EndProfileDate = endDate;
			package.Transactions = transactions.ToObservableCollection<TransactionCollection,Transaction>();
			return package;
		}

		public Package()
		{
			_Id = 0;
			_ParentId = 0;
			_Name = "Untitled";
			_Icon = "unknown";
			_StartProfileDate.Init();
			_EndProfileDate.Init();
			_TransactionsLoaded = false;
			_Options = "";
			_Sort = 0;
		}

		public Package(Package package)
			: this()
		{
			if (package == null)
				return;

			_Id = package._Id;
			_ParentId = package._ParentId;
			_Name = package._Name;
			_Icon = package._Icon;
			_StartProfileDate = package._StartProfileDate;
			_EndProfileDate = package._EndProfileDate;
			_Transactions = package._Transactions;
			_Packages = package._Packages;
			_TransactionsLoaded = package._TransactionsLoaded;
			_Options = package._Options;
			_Sort = package._Sort;
		}

		public override void Dispose()
		{
			base.Dispose();
			Transactions.Dispose();
			Packages.Dispose();
//j			Transactions = new TransactionCollection();
//j			Packages = new PackageCollection();
		}

		internal void SetDates(ProfileDate startDate, ProfileDate endDate)
		{
			_StartProfileDate = startDate;
			_EndProfileDate = endDate;

			// Notify the sub-packages
			foreach (Package package in _Packages)
				package.SetDates(startDate, endDate);
		}

		internal void RunPrepare(Document document)
		{
			// Update any profile dependent dates
			_StartProfileDate.UpdateFromProfile();
			_EndProfileDate.UpdateFromProfile();

			// Update the document's SpendingBalances
			// For now, process all spending, regardless of...
			//		Type, Deduction, ReceivedBy, SourceAccount, TargetAccount, TaxExempt
			// Spread the spending evenly over the duration, taking later sampling into account
			JulianDay startDate = _StartProfileDate.JulianDay;
			JulianDay actualEndDate = _EndProfileDate.JulianDay;
			foreach (Transaction transaction in _Transactions)
			{
				JulianDay endDate = (transaction.Frequency != Frequency.OneTime ? actualEndDate : startDate);
				double days = transaction.Frequency.ToDays();
				if (days > 0)
				{
					double sampleAmount = transaction.Amount.ToDouble() / days;

					//j HACK ALERT: Since we will only sample the SpendingBalances on a yearly basis...
					// make sure that each duration is at least a year
					if (endDate - startDate < 365)
					{
						sampleAmount *= (endDate - startDate + 1);
						endDate = startDate + 365;
						sampleAmount /= (endDate - startDate + 1);
					}

					document.SpendingBalances.ModifyAmount(startDate, sampleAmount.ToMillicents());
					document.SpendingBalances.ModifyAmount(endDate, -sampleAmount.ToMillicents());
				}
			}

			// Notify the sub-packages
			_Packages.RunPrepare(document);
		}

		internal void Run(Document document, JulianDay date)
		{
			// If the date is outside of the date range for this package, get out
			JulianDay startDate = _StartProfileDate.JulianDay;
			JulianDay endDate = _EndProfileDate.JulianDay;
			if (date >= startDate && date <= endDate)
			{
				// Run the transactions for the given date
				_Transactions.Run(document, date, startDate, endDate);
			}

			// Notify the sub-packages
			_Packages.Run(document, date);
		}

		internal void MustHaveATransaction()
		{
			if (_Transactions.Count != 0)
				return;

			// Prevent packages with no transactions
			Transaction transaction = Transaction.CreateDefault(_Id);
			transaction.Name = _Name;
			_Transactions.Add(transaction);
		}

		internal bool HasOption(char option)
		{
			return (_Options != null ? _Options.IndexOf(option) >= 0 : false);
		}

		internal void OptionsAddRemove(bool add, string options)
		{
			foreach (char option in options)
			{
				if (_Options == null) _Options = "";
				int index = _Options.IndexOf(option);
				if (add && index < 0) // Add
					_Options += option;
				if (!add & index >= 0) // Remove
					_Options = _Options.Substring(0, index) + _Options.Substring(index+1);
			}
		}
	}

[DataContract]
[XmlRoot]
	public class PackageCollection : ObservableCollection<Package>, IDisposable
	{
		public void Dispose()
		{
			foreach (Package package in this)
				package.Dispose();
			Clear();
		}

		internal void RunPrepare(Document document)
		{
			foreach (Package package in this)
			{
				package.RunPrepare(document);
			}
		}

		internal void Run(Document document, JulianDay date)
		{
			foreach (Package package in this)
			{
				package.Run(document, date);
			}
		}

		internal void AddAndRun(Package package, Document document)
		{
			if (package == null || Contains(package))
				return;

			Add(package);
			document.BackgroundRun(package.StartProfileDate.JulianDay, package.EndProfileDate.JulianDay);
		}

		internal void RemoveAndRun(Package package, Document document)
		{
			if (package == null || !Contains(package))
				return;

			Remove(package);
			document.BackgroundRun(package.StartProfileDate.JulianDay, package.EndProfileDate.JulianDay);
		}

		internal void UpdateAndRun(Package packageOld, Package package, Document document)
		{
			if (packageOld == null || package == null)
				return;
			if (packageOld == package)
				return;

			int index = IndexOf(packageOld);
			if (index >= 0)
				Remove(packageOld);
			else
				index = Count;

			Insert(index, package);
			JulianDay startDate = Math.Min(packageOld.StartProfileDate.JulianDay, package.StartProfileDate.JulianDay);
			JulianDay endDate = Math.Max(packageOld.EndProfileDate.JulianDay, package.EndProfileDate.JulianDay);
			document.BackgroundRun(startDate, endDate);
		}
	}
}
