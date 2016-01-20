using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Serialization;
using System.Runtime.Serialization;
using tMillicents = System.Int64;
using ClassLibrary;

namespace PicaPages
{
[DataContract(Name="TransactionOccurs")]
[XmlRoot("TransactionOccurs")]
	public class TransactionOccurs
	{
[XmlElement]
[DataMember]
		public List<Transaction> TransactionList { set { m_Transactions = value; } get { return m_Transactions; } }
		private List<Transaction> m_Transactions;

		/////////////////////////////////////////////////////////////////////////////
		public TransactionOccurs()
		{
			m_Transactions = new List<Transaction>();
		}

		/////////////////////////////////////////////////////////////////////////////
		public TransactionOccurs(JulianDay lStartDate, JulianDay lEndDate, List<Transaction> transactions)
			: this()
		{
			// Load the transaction data
			for (int i = 0; i < transactions.Count; i++)
				AddTransaction(transactions[i]);
		}

		/////////////////////////////////////////////////////////////////////////////
		public TransactionOccurs(JulianDay lStartDate, JulianDay lEndDate, Transaction[] transactions)
			: this(lStartDate, lEndDate, new List<Transaction>(transactions))
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public TransactionOccurs(JulianDay lStartDate, JulianDay lEndDate, Transaction transaction)
			: this(lStartDate, lEndDate, new Transaction[] { transaction })
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		~TransactionOccurs()
		{
			m_Transactions = null;
		}

		/////////////////////////////////////////////////////////////////////////////
		public void AddTransaction(Transaction transaction)
		{
			if (transaction == null || m_Transactions.Contains(transaction))
				return;

			m_Transactions.Add(transaction);
		}

		/////////////////////////////////////////////////////////////////////////////
		public bool Run(Document document, JulianDay lDate)
		{
			return false;
		}
	}
}
