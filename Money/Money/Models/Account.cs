using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.Serialization;
using System.Windows.Data;
using System.Xml.Serialization;
using ClassLibrary;
using JulianDay = System.UInt32;
using Millicents = System.Int64;

namespace Money
{
[DataContract]
[XmlRoot]
	public enum AccountType
	{ // These names are stored in documents, so take care to preserve them
		Checking,		// Asset 
		Savings,		// Asset includes money market
		Credit,			// Liability automaticly tracked
		Property,		// Asset
//j		Loans,			// Liability
		Investments,	// Asset includes portfolio - is taxable
		Retirement1,	// Asset qualified retirement plans; tax-advantaged
		Retirement2,	// Asset
		Retirement3,	// Asset
		Retirement4,	// Asset
		Retirement5,	// Asset
		Retirement6,	// Asset
		Retirement7,	// Asset
		Retirement8,	// Asset
		Retirement9,	// Asset
//j		Placeholder1,	// Generic
//j		Placeholder2,	// Generic
		NoAccount,		// Generic
	}

	public class AccountComparer : IComparer<Account>
	{
		public int Compare(Account xAccount, Account yAccount)
		{
			// Less than zero if x is less than y
			// Greater than zero x is greater than y
			// Zero if x equals y
			return xAccount.Name.CompareTo(yAccount.Name);
		}
	}

[DataContract]
[XmlRoot]
	public class Account : ViewModelBase
	{
[XmlAttribute]
[DataMember]
		public string Name { get { return _Name.IsEmpty() ? _AccountType.ToString() : _Name; } set { SetProperty(ref _Name, value, () => Name); } }
		private string _Name;
[XmlAttribute]
[DataMember]
		public AccountType AccountType { get { return _AccountType; } set { SetProperty(ref _AccountType, value, () => AccountType); } }
		private AccountType _AccountType;
[XmlAttribute]
[DataMember]
		public bool AllowNegative { get { return _AllowNegative; } set { SetProperty(ref _AllowNegative, value, () => AllowNegative); } }
		private bool _AllowNegative;
[XmlAttribute]
[DataMember]
		public bool Hidden { get { return _Hidden; } set { SetProperty(ref _Hidden, value, () => Hidden); } }
		private bool _Hidden;
[XmlAttribute]
[DataMember]
		public bool Internal { get { return _Internal; } set { SetProperty(ref _Internal, value, () => Internal); } }
		private bool _Internal;
[XmlAttribute]
[DataMember]
		public Millicents StartingBalance { get { return _StartingBalance; } set { SetProperty(ref _StartingBalance, value, () => StartingBalance); } }
		private Millicents _StartingBalance;
[XmlIgnore]
[IgnoreDataMember]
		public Millicents Balance { get { return _Balance; } set { SetProperty(ref _Balance, value, () => Balance); } }
		private Millicents _Balance;
[XmlIgnore]
[IgnoreDataMember]
		public BalanceList Balances { get { return _Balances; } }
		private BalanceList _Balances = new BalanceList();

		public static readonly Account[] DefaultList = 
		{																	// allowNeg	hidden	internal
			new Account("Checking",					AccountType.Checking,		false,	false,	false),
			new Account("Savings & Money Market",	AccountType.Savings,		false,	false,	false),
			new Account("Credit Cards",				AccountType.Credit,			true,	false,	false),
			new Account("Valuable Property",		AccountType.Property,		false,	false,	false),
			new Account("Equities",					AccountType.Investments,	false,	false,	false),
			new Account("My 401K",					AccountType.Retirement1,	false,	false,	false),
			new Account("My IRA",					AccountType.Retirement2,	false,	false,	false),
			new Account("Spouse 401K",				AccountType.Retirement3,	false,	false,	false),
			new Account("Spouse IRA",				AccountType.Retirement4,	false,	false,	false),
			new Account("",							AccountType.Retirement5,	false,	true,	false),
			new Account("",							AccountType.Retirement6,	false,	true,	false),
			new Account("",							AccountType.Retirement7,	false,	true,	false),
			new Account("",							AccountType.Retirement8,	false,	true,	false),
			new Account("",							AccountType.Retirement9,	false,	true,	false),
			//new Account("Placeholder1",			AccountType.Placeholder1,	true,	true,	true),
			//new Account("Placeholder2",			AccountType.Placeholder2,	true,	true,	true),
			new Account("None",						AccountType.NoAccount,		true,	true,	true),
		};

		public Account()
		{
			_Name = "";
			_AllowNegative = true;
			_AccountType = AccountType.NoAccount;
			_StartingBalance = 0;
			_Balance = 0;
		}

		public Account(string name, AccountType type, bool allowNegative, bool hidden, bool _internal)
			: this()
		{
			_Name = name;
			_AccountType = type;
			_AllowNegative = allowNegative;
			_Hidden = hidden;
			_Internal = _internal;
		}

		public override void Dispose()
		{
			base.Dispose();
			_Balances.Dispose();
		}

		internal Millicents GetBalance(JulianDay date, bool startOfTheDay)
		{
			return _Balances.GetAmount(date, startOfTheDay) + _StartingBalance;
		}

		internal void SetBalance(JulianDay date, Millicents balance)
		{
			_Balances.SetAmount(date, balance - _StartingBalance);
		}

		internal void TruncateBalance(JulianDay date)
		{
			_Balances.Truncate(date);
		}

		internal bool IsUnused()
		{
			return (_Balances.Count <= 0);
		}
	}

[DataContract]
[XmlRoot]
	public class AccountCollection : ObservableCollection<Account>, IDisposable
	{
		private static readonly AccountType[] _AccountPeckingOrder = {
			AccountType.Checking, AccountType.Savings, AccountType.Investments, 
			AccountType.Retirement1, AccountType.Retirement2, AccountType.Retirement3,
			AccountType.Retirement4, AccountType.Retirement5, AccountType.Retirement6,
			AccountType.Retirement7, AccountType.Retirement8, AccountType.Retirement9,
		};

		public void Dispose()
		{
			foreach (Account account in this)
				account.Dispose();
			Clear();
		}

		internal void BorrowFromCredit(Document document, JulianDay date)
		{
			//j Take out loans if necessary?
			//j Consider allowing money to be swept into savings (conditional transaction)
			//j if triggered, move money between accounts
			// At the end of each day, handle any negative account balances by putting them on the credit account
			foreach (Account account in this)
			{
				if (account.AllowNegative)
					continue;
				Millicents balance = account.GetBalance(date, false/*bStartOfTheDay*/);
				if (balance >= 0)
					continue;

				Transaction transaction = Transaction.Create("Borrow from Credit Line", Frequency.OneTime, TransactionType.Transfer, -balance.ToDouble(), AccountType.Credit, account.AccountType);
				bool transactionRan = transaction.Run(document, date, date/*startDate*/, date/*endDate*/);
			}
		}

		internal void PayoffCredit(Document document, JulianDay date)
		{
			Account creditAccount = Find(AccountType.Credit, false/*bCreate*/);
			Millicents creditBalance = (creditAccount == null ? 0 : creditAccount.GetBalance(date, false/*bStartOfTheDay*/));
			if (creditBalance >= 0)
				return;

			foreach (AccountType accountType in _AccountPeckingOrder)
			{
				Account account = Find(accountType, false/*bCreate*/);
				if (account == null)
					continue;

				Millicents lBalance = account.GetBalance(date, false/*bStartOfTheDay*/);
				if (lBalance <= 0)
					continue;

				Millicents transferBalance = Math.Min(lBalance, -creditBalance);
				Transaction transaction = Transaction.Create("Pay down Credit Line from " + accountType.ToString(), Frequency.OneTime, TransactionType.Transfer, transferBalance.ToDouble(), accountType, AccountType.Credit);
				bool transactionRan = transaction.Run(document, date, date/*startDate*/, date/*endDate*/);
				if (!transactionRan)
					continue;

				creditBalance += transferBalance;
				if (creditBalance == 0)
					break;
			}
		}

		internal Account Find(AccountType eType, bool create)
		{
			// Loop thru all the accounts
			foreach (Account account in this)
			{
				// If we found a type match, return it
				if (account.AccountType == eType)
					return account;
			}

			// We didn't find the account and we won't create a new one, so get out
			if (!create)
				return null;

			// Create a new account, by request
			Account accountNew = new Account();
			accountNew.AccountType = eType;
			accountNew.Name = eType.ToString();
			Add(accountNew);
			return accountNew;
		}

		internal void TruncateBalance(JulianDay startDate)
		{
			foreach (Account account in this)
				account.TruncateBalance(startDate);
		}

		internal void UpdateBalances(JulianDay date, BackgroundWorker worker, DoWorkEventArgs e)
		{
			foreach (Account account in this)
			{ // Delete any new items
				if (worker != null && worker.CancellationPending)
				{
					e.Cancel = true;
					return;
				}

				if (!account.Internal)
					account.Balance = account.GetBalance(date, false/*bStartOfTheDay*/);
			}
		}

		internal IEnumerable<Account> VisibleAccountsFilter()
		{ // Possible problem: The result is not an ObservableCollection
			foreach (Account account in this)
			{
				if (!account.Internal && !account.Hidden)
					yield return account;
			}
		}
	}

	public class AccountTypeToName : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (value.GetType() != typeof(AccountType))
				throw new Exception(string.Format("'{0}' is not an AccountType", value));

			AccountType accountType = (AccountType)value;
			foreach (Account account in App.Model.DocumentHolder.Document.Accounts)
			{
				if (account.AccountType == accountType)
					return account.Name;
			}

			// Not found; use the generic name
			return value.ToString().ExpandMixedCase();
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (!(value is string))
				throw new Exception(string.Format("'{0}' is not a string", value));
			if (targetType != typeof(AccountType))
				throw new Exception(string.Format("'{0}' is not an AccountType", targetType));

			string name = (string)value;
			foreach (Account account in App.Model.DocumentHolder.Document.Accounts)
			{
				if (!account.Hidden && account.Name == name)
					return (object)account.AccountType;
			}

			// Not found; use the generic name
			name = name.Replace(" ", null);
			if (!Enum.IsDefined(targetType, name))
				return null;

			return Enum.Parse(targetType, name, true/*ignoreCase*/);
		}
	}

	public class AccountTypeToList : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
		{
			if (value == null) // Special case
				return null;
			if (value.GetType() != typeof(AccountType))
				throw new Exception(string.Format("'{0}' is not an AccountType", value));

			AccountType accountType = (AccountType)value;
			Collection<string> enumList = new Collection<string>();
			foreach (Account account in App.Model.DocumentHolder.Document.Accounts)
			{
				if (!account.Hidden || account.AccountType == accountType)
					enumList.Add(account.Name);
			}

			return enumList;
		}

		public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
		{
			throw new NotSupportedException();
		}
	}

#if NOTUSED
	public class FilteredObservableCollection<T> : ObservableCollection<T>
	{
		private ObservableCollection<T> _collection;
		private Predicate<T> _filter;
		private event NotifyCollectionChangedEventHandler _collectionchanged;
		private event PropertyChangedEventHandler _propertychanged;

		public FilteredObservableCollection(ObservableCollection<T> collection)
		{
			_filter = null;
			_collection = collection;
			_collection.CollectionChanged += OnCollectionChanged;
			((INotifyPropertyChanged)_collection).PropertyChanged += OnPropertyChanged;
		}

		private void OnCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
		{
			if (_collectionchanged != null)
			{
				// Check the NewItems
				Collection<T> newlist = new Collection<T>();
				if (e.NewItems != null)
					foreach (T item in e.NewItems)
						if (_filter(item) == true)
							newlist.Add(item);

				// Check the OldItems
				Collection<T> oldlist = new Collection<T>();
				if (e.OldItems != null)
					foreach (T item in e.OldItems)
						if (_filter(item) == true)
							oldlist.Add(item);

				// Create the Add/Remove/Replace lists
				Collection<T> addlist = new Collection<T>();
				Collection<T> removelist = new Collection<T>();
				Collection<T> replacelist = new Collection<T>();

				// Fill the Add/Remove/Replace lists
				foreach (T item in newlist)
					if (oldlist.Contains(item))
						replacelist.Add(item);
					else
						addlist.Add(item);
				foreach (T item in oldlist)
					if (newlist.Contains(item))
						continue;
					else
						removelist.Add(item);

				// Send the corrected event
				switch (e.Action)
				{
					case NotifyCollectionChangedAction.Add:
					case NotifyCollectionChangedAction.Move:
					case NotifyCollectionChangedAction.Remove:
					case NotifyCollectionChangedAction.Replace:
						if (addlist.Count > 0)
							_collectionchanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, addlist));
						if (replacelist.Count > 0)
							_collectionchanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, replacelist));
						if (removelist.Count > 0)
							_collectionchanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, removelist));
						break;
					case NotifyCollectionChangedAction.Reset:
						_collectionchanged(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
						break;
				}
			}
		}
	}
#endif
}
