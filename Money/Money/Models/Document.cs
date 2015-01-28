using System;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Windows.Threading;
using System.Xml.Serialization;
using ClassLibrary;
using DOCTYPE = System.Int32;
using JulianDay = System.UInt32;

namespace Money
{
	public class DocumentHolder : ViewModelBase
	{
		public Document Document { get { return _Document; } set { SetProperty(ref _Document, value, System.Reflection.MethodBase.GetCurrentMethod().Name, OnDocumentCreated); } }
		private Document _Document = new Document();

		public string CurrentDocumentFileName { get { return _CurrentDocumentFileName; } set { SetProperty(ref _CurrentDocumentFileName, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _CurrentDocumentFileName;

		public event PropertyChangedEventHandler DocumentCreated;

		private DispatcherTimer _AutoSaveTimer = new DispatcherTimer();

		public DocumentHolder()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
			Document.Dispose();
			DocumentCreated = null;
			_AutoSaveTimer = new DispatcherTimer();
		}

		internal void Initialize()
		{
			// Start listening to the profile holder
			App.Model.ProfileHolder.ProfileCreated += OnProfileCreated;
			OnProfileCreated(null, null);

			// Load the most recent document filename from the application's settings
			_CurrentDocumentFileName = Settings.Read<string>("MostRecentDocumentFileName", ResxResources.UntitledDocumentFileName);

			// Load the user defaults
			OpenCurrentDocument();

			// AutoSave the current document every 60 seconds
			_AutoSaveTimer.Interval = new TimeSpan(60 * 1000 * TimeSpan.TicksPerMillisecond);
			_AutoSaveTimer.Tick += delegate(object sender, EventArgs e)
			{
				if (Document != null && !AutoSave())
					MessageBoxEx.ShowError("AutoSave", string.Format("Cannot AutoSave the current document"), null);
			};
			_AutoSaveTimer.Start();
		}

		private void OnDocumentCreated(Document document)
		{
			if (DocumentCreated != null)
				DocumentCreated(this, null);

			Document.BackgroundRun(0, 0);
		}

		private void OnProfileCreated(object sender, PropertyChangedEventArgs e)
		{
			// Start listening to the new profile
			App.Model.ProfileHolder.Profile.ProfileChanged += OnProfileChanged;
			OnProfileChanged(null, null);
		}

		private void OnProfileChanged(object sender, PropertyChangedEventArgs e)
		{
			if (Document == null || Document.Accounts.Count == 0)
				return;

			if (e == null || e.PropertyName == "StartDate" || e.PropertyName == "EndDate")
			{
				Document.BackgroundRun(0, 0);
			}
			else
			if (e.PropertyName == "CurrentDate")
			{
				Document.BackgroundUpdateBalances();
			}
		}

		internal bool OpenCurrentDocument()
		{
			if (Open(ResxResources.DefaultDocumentFileName))
				return true;

			if (Open(_CurrentDocumentFileName))
				return true;

			//MessageBoxEx.ShowOK("Alert", string.Format("The document was located.  Please create one in the Plan tab."), null);
			return Open(null/*empty*/);
		}

		internal bool Open(object obj)
		{
			Document document = null;
			string fileName = (obj as string);
			if (obj is FileInfoHelper || (fileName != null && fileName != "Sample"))
			{
				document = Document.Open(obj);
			}
			else
			{
				document = new Document();

				// Load the default account list
				foreach (Account account in Account.DefaultList)
				{
					Account newAccount = new Account(account.Name, account.AccountType, account.AllowNegative, account.Hidden, account.Internal);
					document.Accounts.Add(newAccount);
				}

				if (fileName == "Sample")
				{
					DebugDocumentData.Load(document);
					fileName = null;
				}
			}

			if (document == null)
				return false;

			if (fileName != ResxResources.DefaultDocumentFileName)
			{
				if (obj is FileInfoHelper)
					_CurrentDocumentFileName = (obj as FileInfoHelper).Name;
				else
				if (fileName != null)
					_CurrentDocumentFileName = fileName;
				else
					_CurrentDocumentFileName = ResxResources.UntitledDocumentFileName;
				Settings.Write<string>("MostRecentDocumentFileName", _CurrentDocumentFileName);
			}

			Document = document;
			return true;
		}

		internal bool AutoSave()
		{
			return Document.Save(ResxResources.DefaultDocumentFileName);
		}

		internal bool Save(string fileName)
		{
			if (!Document.Save(fileName))
				return false;

			_CurrentDocumentFileName = fileName;
			Settings.Write<string>("MostRecentDocumentFileName", fileName);
			return true;
		}

#if NOTUSED
		internal void Close()
		{
			Document = new Document();
		}
#endif
	}

[DataContract]
[XmlRoot]
	public class Document : ViewModelBase
	{
		const int DOC_VERSION = 1;
		const int DOCTYPE_FF = 0;
[XmlAttribute]
[DataMember]
		public int DocVersion { get { return _DocVersion; } set { SetProperty(ref _DocVersion, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
private int _DocVersion = DOC_VERSION;
[XmlAttribute]
[DataMember]
		public DOCTYPE DocType { get { return _DocType; } set { SetProperty(ref _DocType, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private DOCTYPE _DocType = DOCTYPE_FF;
[XmlElement]
[DataMember]
		public IncomeCollection Incomes { get { return _Incomes; } set { SetProperty(ref _Incomes, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private IncomeCollection _Incomes = new IncomeCollection();
[XmlElement]
[DataMember]
		public PackageCollection Packages { get { return _Packages; } set { SetProperty(ref _Packages, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private PackageCollection _Packages = new PackageCollection();
[XmlElement]
[DataMember]
		public AccountCollection Accounts { get { return _Accounts; } set { SetProperty(ref _Accounts, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private AccountCollection _Accounts = new AccountCollection();
[XmlIgnore]
[IgnoreDataMember]
		public CategoryCollection Categories { get { return _Categories; } set { SetProperty(ref _Categories, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private CategoryCollection _Categories = new CategoryCollection();
[XmlIgnore]
[IgnoreDataMember]
		public BalanceList IncomeBalances { get { return _IncomeBalances; } set { SetProperty(ref _IncomeBalances, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private BalanceList _IncomeBalances = new BalanceList();
[XmlIgnore]
[IgnoreDataMember]
		public BalanceList SpendingBalances { get { return _SpendingBalances; } set { SetProperty(ref _SpendingBalances, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private BalanceList _SpendingBalances = new BalanceList();
[XmlIgnore]
[IgnoreDataMember]
		public int RunCount { get { return _RunCount; } set { SetProperty(ref _RunCount, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _RunCount;
[XmlIgnore]
[IgnoreDataMember]
		public bool Modified { get { return _Modified; } set { SetProperty(ref _Modified, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private bool _Modified;

		private BackgroundTask _BackgroundRun;
		private BackgroundTask _BackgroundUpdateBalances;

		// New features:
		// Automatically sweep from stocks to checking to avoid negative balances
		// Add spending accounts, like income taxes, etc. that don't need daily balances to be tracked
		// Add linked overdraft accounts to handle negative balances
		// Add conditional transfers, i.e., when a balance goes negative or below a certain threshold

		public Document()
		{
			_BackgroundRun = new BackgroundTask(false/*runOnUIThread*/, 200/*runDelay*/, BackgroundRunWorker, BackgroundRunComplete);
			_BackgroundUpdateBalances = new BackgroundTask(false/*runOnUIThread*/, 200/*runDelay*/, BackgroundUpdateBalancesWorker, BackgroundUpdateBalancesComplete);
		}

		public override void Dispose()
		{
			base.Dispose();
			Accounts.Dispose();
			Incomes.Dispose();
			Packages.Dispose();
			Categories.Dispose();
			Accounts = new AccountCollection();
			Incomes = new IncomeCollection();
			Packages = new PackageCollection();
			Categories = new CategoryCollection();
			_BackgroundRun = null;
			_BackgroundUpdateBalances = null;
		}

		internal static Document Open(object obj)
		{
			try
			{
				if (!IsolatedFileHelper.IsStorageAllowed())
					return null;
				if (obj is FileInfoHelper)
					return Load((obj as FileInfoHelper).ReadString());
				if (obj is string)
					return Load(IsolatedFileHelper.ReadFileString(obj as string));
				return null;
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return null;
		}

		private static Document Load(string document)
		{
			try
			{
				if (document == null)
					return null;

				Serializer serializer = new Serializer(false/*isDCS*/);
				return serializer.Deserialize<Document>(document);
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return null;
		}

		internal bool Save(string fileName)
		{
			try
			{
				Serializer serializer = new Serializer(false/*isDCS*/);
				string document = serializer.Serialize(this);
				if (document == null)
					return false;

				IsolatedFileHelper.WriteFileString(fileName, document);
				return true;
			}
			catch (Exception ex)
			{
				ex.Alert();
			}

			return false;
		}

		internal void BackgroundUpdateBalances()
		{
			// Update the current balances in the background to avoid slowing UI performance
			if (_BackgroundUpdateBalances != null)
				_BackgroundUpdateBalances.Run(null, true/*bCancelPrevious*/);
		}

		private void BackgroundUpdateBalancesWorker(object sender, DoWorkEventArgs e)
		{
			UpdateBalances(sender as BackgroundWorker, e);
		}

		private void BackgroundUpdateBalancesComplete(object sender, RunWorkerCompletedEventArgs e)
		{
			if (e.Error == null)
				return;

			// Re-run the background task
			BackgroundUpdateBalances();
			e.Error.DebugOutput();
		}

		private void UpdateBalances(BackgroundWorker worker, DoWorkEventArgs e)
		{
			JulianDay currentDate = App.Model.ProfileHolder.Profile.CurrentDate;
			if (currentDate == 0)
				return;

			_Accounts.UpdateBalances(currentDate, worker, e);
			_Categories.UpdateBalances(currentDate, worker, e);
		}

		internal void BackgroundRun(JulianDay startDate, JulianDay endDate)
		{
			// Run the document in the background to avoid slowing UI performance
			if (_BackgroundRun != null)
			{
				Pair argument = new Pair() { X = startDate, Y = endDate };
				_BackgroundRun.Run(argument, true/*bCancelPrevious*/);
			}
		}

		private void BackgroundRunWorker(object sender, DoWorkEventArgs e)
		{
			Pair argument = e.Argument as Pair;
			JulianDay startDate = (JulianDay)argument.X;
			JulianDay endDate = (JulianDay)argument.Y;
			Run(startDate, endDate, sender as BackgroundWorker, e);
		}

		private void BackgroundRunComplete(object sender, RunWorkerCompletedEventArgs e)
		{
			if (e.Error == null)
				return;

			// Re-run the background task
			BackgroundRun(0, 0);
			e.Error.DebugOutput();
		}

		private void Run(JulianDay startDate, JulianDay endDate)
		{
			DateTime time = DateTime.Now;
			Run(startDate, endDate, null, null);
			TimeSpan timeSpan = DateTime.Now - time;
		}

		private void Run(JulianDay startDate, JulianDay endDate, BackgroundWorker worker, DoWorkEventArgs e)
		{
			JulianDay profileStartDate = App.Model.ProfileHolder.Profile.StartProfileDate.JulianDay;
			JulianDay profileEndDate = App.Model.ProfileHolder.Profile.EndProfileDate.JulianDay;
			// profileStartDate and profileEndDate could be 0

			if (startDate == 0 && endDate == 0)
			{
				startDate = profileStartDate;
				endDate = profileEndDate;
			}

			// Clip the passed start date to the document start date
			if (startDate < profileStartDate)
				startDate = profileStartDate;

			// Always run to the end of the document since changes ripple to the end
			endDate = profileEndDate;

			// From the start date forward, truncate the daily balances
			// Don't leave before doing this!
			_Accounts.TruncateBalance(startDate);
			_Categories.TruncateBalance(startDate);

			// If Dates were never set or nothing to do
			if ((profileStartDate == 0 && profileEndDate == 0) || startDate > profileEndDate)
			{
				UpdateBalances(worker, e);
				return;
			}
#if NOTUSED
			Helper.Trace("Calculating account values between {0,2:d2}/{1,2:d2}/{2,2:d2} and {3,2:d2}/{4,2:d2}/{5,2:d2}",
				startDate.Month(), startDate.Day(), startDate.Year(), endDate.Month(), endDate.Day(), endDate.Year());
			DateTime SystemTime = DateTime.Now;
			Helper.Trace("Start: {0,2:d2}:{1,2:d2}:{2,2:d2}:{3,2:d2}", SystemTime.Hour, SystemTime.Minute, SystemTime.Second, SystemTime.Millisecond);
#endif
			// Let the packages know we are about to run
			IncomeBalances.Clear();
			SpendingBalances.Clear();
			_Incomes.RunPrepare(this);
			_Packages.RunPrepare(this);

			// Loop thru all the dates in the running date range
			for (JulianDay date = startDate; date <= endDate; date++)
			{
				if (worker != null && worker.CancellationPending)
				{
					e.Cancel = true;
					return;
				}

				// Run the packages for the given date
				_Incomes.Run(this, date);
				_Packages.Run(this, date);

				// At the end of each day, handle any negative account balances by putting them on the credit account
				_Accounts.BorrowFromCredit(this, date);

				// At the end of each month, make credit payments if needed
				JulianDay tomorrow = date + 1;
				bool bMonthlyRun = (tomorrow.Day() == 1);
				if (bMonthlyRun)
					_Accounts.PayoffCredit(this, date);
			}

			// Update the RunCount for anyone listening
			RunCount++;

			UpdateBalances(worker, e);
#if NOTUSED
			SystemTime = DateTime.Now;
			Helper.Trace("End: {0,2:d2}:{1,2:d2}:{2,2:d2}:{3,2:d2}", SystemTime.Hour, SystemTime.Minute, SystemTime.Second, SystemTime.Millisecond);
#endif
		}
	}
}
