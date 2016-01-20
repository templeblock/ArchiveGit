using System;
using System.Collections.Generic;
using System.Xml.Serialization;
using System.Runtime.Serialization;
using ClassLibrary;
using tMillicents = System.Int64;
using DOCTYPE = System.Int32;
using System.Threading;
using System.Linq;
using System.Xml.Linq;

namespace PicaPages
{
	public class DocumentHolder
	{
		private Document m_Document;
		
		public Document DocumentProp { get { return m_Document; } }

		// Define the delegates for the our events
		public delegate void CloseEventHandler(Document document, EventArgs e);
		public delegate void ChangeEventHandler(Document document, EventArgs e);

		// Define our events
		public event CloseEventHandler CloseListeners;
		public event ChangeEventHandler ChangeListeners;

		/////////////////////////////////////////////////////////////////////////////
		public DocumentHolder()
		{
			m_Document = null;
		}

		/////////////////////////////////////////////////////////////////////////////
		public bool Open(string filename)
		{
			Close();
			if (m_Document != null)
				return false;

			bool fakeUserData = (filename == null);
			if (fakeUserData)
			{
				List<Account> accounts = WebResources.Accounts;
				if (accounts == null)
					return false;

				m_Document = new Document(this);

				// Load the account table
				for (int i = 0; i < accounts.Count; i++)
					m_Document.AddAccount(accounts[i]);
			}
			else
				m_Document = Document.Open(filename);

			m_Document.DocHolder = this;
			JulianDay lStartDate = new JulianDay(1, 1, 1955);
			JulianDay lEndDate = new JulianDay(12, 31, 1955 + 120);
			JulianDay lDate = DateTime.Today.ToJulian();
			Run(lStartDate, lEndDate, lDate);

			SendChangeEvent();
			return true;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void Close()
		{
			if (m_Document != null)
				SendCloseEvent();
			m_Document = null;
		}

		/////////////////////////////////////////////////////////////////////////////
		public bool Save(string filename)
		{
			if (m_Document == null)
				return false;

			m_Document.Save(filename);
			return true;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void Run(JulianDay lStartDate, JulianDay lEndDate, JulianDay lDate)
		{
			if (m_Document == null)
				return;

			m_Document.Run(lStartDate, lEndDate);
			m_Document.CurrentDate = lDate;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void SendChangeEvent()
		{
			// Trigger a ChangeEvent, so all views that are ChangeListeners will be notified
			if (ChangeListeners != null) // The second parameter can contain information which the listeners might find useful
				ChangeListeners(m_Document, null);
		}

		/////////////////////////////////////////////////////////////////////////////
		public void SendCloseEvent()
		{
			// Trigger a CloseEvent, so all views that are CloseListeners will be notified
			if (CloseListeners != null) // The second parameter can contain information which the listeners might find useful
				CloseListeners(m_Document, null);
		}
	}

[DataContract(Name="Document")]
[XmlRoot("Document")]
	public class Document
	{
[XmlIgnore]
		private DocumentHolder m_DocumentHolder;
[XmlIgnore]
		public DocumentHolder DocHolder { get { return m_DocumentHolder; } set { m_DocumentHolder = value; } }

		const byte DOC_ID = (byte)'A';
		const int DOC_VERSION = 1;
		const int DOCTYPE_FF = 0;
		const int DOCTYPE_LAST = 1;

[XmlAttribute]
[DataMember]
		public int DocVersion { set { m_DocVersion = value; } get { return m_DocVersion; } }
		private int m_DocVersion;
[XmlAttribute]
[DataMember]
		public DOCTYPE DocType { set { m_DocType = value; SendChangeEvent(); } get { return m_DocType; } }
		private DOCTYPE m_DocType;
[XmlAttribute]
[DataMember]
		public string FullDocPath { set { m_strFullDocPath = value; SendChangeEvent(); } get { return m_strFullDocPath; } }
		private string m_strFullDocPath;
[XmlAttribute]
[DataMember]
		public JulianDay CurrentDate { set { m_lCurrentDate = value; SendChangeEvent(); } get { return m_lCurrentDate; } }
		private JulianDay m_lCurrentDate;
[XmlElement]
[DataMember]
		public List<TransactionOccurs> TransactionOccursList { set { m_TransactionEvents = value; SendChangeEvent(); } get { return m_TransactionEvents; } }
		private List<TransactionOccurs> m_TransactionEvents;
[XmlElement]
[DataMember]
		public List<Account> AccountList { set { m_Accounts = value; } get { return m_Accounts; } }
		private List<Account> m_Accounts;
[XmlAttribute]
[DataMember]
		public JulianDay StartDate { set { m_lStartDate = value; } get { return m_lStartDate; } }
		private JulianDay m_lStartDate;
[XmlAttribute]
[DataMember]
		public JulianDay EndDate { set { m_lEndDate = value; } get { return m_lEndDate; } }
		private JulianDay m_lEndDate;
[XmlIgnore]
		private List<Account> m_TransactionLogAccounts;
[XmlIgnore]
		private bool m_bModified;

		// New features:
		// Automatically sweep from stocks to checking to avoid negative balances
		// Add spending accounts, like income taxes, etc. that don't need daily balances to be tracked
		// Add linked overdraft accounts to handle negative balances
		// Add conditional transfers, i.e., when a balance goes negative or below a certain threshold

		/////////////////////////////////////////////////////////////////////////////
		public Document()
			: this(null)
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public Document(DocumentHolder documentHolder)
		{
			m_DocumentHolder = documentHolder;
			m_DocType = DOCTYPE_FF;
			m_DocVersion = DOC_VERSION;
			m_strFullDocPath = null;
			m_Accounts = new List<Account>();
			m_TransactionLogAccounts = new List<Account>();
			m_TransactionEvents = new List<TransactionOccurs>();
			m_lStartDate = DateTime.Today.ToJulian();
			m_lEndDate = DateTime.Today.ToJulian();
			m_bModified = false;
			m_lCurrentDate = 0;
		}

		/////////////////////////////////////////////////////////////////////////////
		~Document()
		{
			m_Accounts = null;
			m_TransactionLogAccounts = null;
			m_TransactionEvents = null;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void SendChangeEvent()
		{
			if (m_DocumentHolder != null)
				m_DocumentHolder.SendChangeEvent();
		}

		/////////////////////////////////////////////////////////////////////////////
		public void GetFileName(ref string strFullDocPath, ref string strPath, ref string strFileName)
		{
			strFullDocPath = m_strFullDocPath;

			int i = Math.Max(strFullDocPath.LastIndexOf('/'), strFullDocPath.LastIndexOf('\\'));
			strFileName = strFullDocPath.Substring(i + 1);
			strPath = strFullDocPath.Substring(0, i);
			if (strPath.Substring(0, 7) == "file://")
				strPath = strPath.Substring(7);
			if (strPath.Substring(0, 7) == "http://" || strPath.IndexOf("product") >= 0)
				strPath = null; // We don't want users to be able to overwrite web files or production files
		}

		/////////////////////////////////////////////////////////////////////////////
		public static Document Open(string sFileName)
		{
			try
			{
				DocumentIO docIO = new DocumentIO();
				bool bOK = docIO.Open(sFileName, false/*bWriter*/);
				if (!bOK)
					return null;

				string sDocument = docIO.ReadToEnd();
				docIO.Close();
				if (sDocument == null)
					return null;

				Serializer serializer = new Serializer(true/*bXml*/);
				return serializer.Deserialize<Document>(sDocument);
			}
			catch (Exception e)
			{
				System.Diagnostics.Debug.WriteLine(e.Message);
				e.Assert();
			}

			return null;
		}

		/////////////////////////////////////////////////////////////////////////////
		public bool Save(string sFileName)
		{
			try
			{
				Serializer serializer = new Serializer(true/*bXml*/);
				string sDocument = serializer.Serialize<Document>(this);
				if (sDocument == null)
					return false;

				DocumentIO docIO = new DocumentIO();
				bool bOK = docIO.Open(sFileName, true/*bWriter*/);
				if (!bOK)
					return false;

				docIO.Write(sDocument);
				docIO.Close();
				return true;
			}
			catch (Exception e)
			{
				System.Diagnostics.Debug.WriteLine(e.Message);
				e.Assert();
			}

			return false;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void Run(JulianDay lStartDate, JulianDay lEndDate)
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public void AddTransactionOccurs(TransactionOccurs transaction)
		{
			if (transaction == null || m_TransactionEvents.Contains(transaction))
				return;

			m_TransactionEvents.Add(transaction);
		}

		/////////////////////////////////////////////////////////////////////////////
		public void AddTransactionLogAccount(Account transactionLogAccount)
		{
			if (transactionLogAccount == null || m_TransactionLogAccounts.Contains(transactionLogAccount))
				return;

			m_TransactionLogAccounts.Add(transactionLogAccount);
		}

		/////////////////////////////////////////////////////////////////////////////
		public void AddAccount(Account account)
		{
			if (account == null || m_Accounts.Contains(account))
				return;

			m_Accounts.Add(account);
		}

		/////////////////////////////////////////////////////////////////////////////
		public Account FindAccount(EAccountType eType, bool bCreate)
		{
			// Loop thru all the accounts
			for (int i = 0; i < m_Accounts.Count; i++)
			{
				Account account = m_Accounts[i];

				// If we found a type match, return it
				if (account.AccountType == eType)
					return account;
			}

			// We didn't find the account and we won't create a new one, so get out
			if (!bCreate)
				return null;

			// Create a new account, by request
			Account accountNew = new Account();
			accountNew.AccountType = eType;
			accountNew.Name = eType.ToString();
			AddAccount(accountNew);
			return accountNew;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void SetFileName(string strFullDocPath)
		{
			m_strFullDocPath = System.Uri.UnescapeDataString(strFullDocPath);
		}

		/////////////////////////////////////////////////////////////////////////////
		public void SetModified(bool bModified)
		{
			m_bModified = bModified;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void SetDocType(DOCTYPE DocType)
		{
			m_DocType = DocType;
		}
	}
}
