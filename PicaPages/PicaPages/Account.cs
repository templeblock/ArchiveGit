using System;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Serialization;
using System.Runtime.Serialization;
using tMillicents = System.Int64;
using ClassLibrary;

namespace PicaPages
{
	/////////////////////////////////////////////////////////////////////////////
	public enum EAccountType
	{ // Order by accessibility
		eNoAccount,
	}

	class AccountComparer : IComparer<Account>
	{
		/////////////////////////////////////////////////////////////////////////////
		public int Compare(Account xAccount, Account yAccount)
		{
			// Less than zero if x is less than y
			// Greater than zero x is greater than y
			// Zero if x equals y
			return xAccount.Name.CompareTo(yAccount.Name);
		}
	}

[DataContract(Name="Account")]
[XmlRoot("Account")]
	public class Account
	{
[XmlAttribute]
[DataMember]
		public string Name { set { m_sName = value; } get { return m_sName; } }
		private string m_sName;
[XmlAttribute]
[DataMember]
		public EAccountType AccountType { set { m_eType = value; } get { return m_eType; } }
		private EAccountType m_eType;

		/////////////////////////////////////////////////////////////////////////////
		public Account()
		{
			m_sName = "";
			m_eType = EAccountType.eNoAccount;
		}

		/////////////////////////////////////////////////////////////////////////////
		public Account(string _sName, EAccountType _eType, bool bAllowNegative)
			: this()
		{
			m_sName = _sName;
			m_eType = _eType;
		}

		/////////////////////////////////////////////////////////////////////////////
		~Account()
		{
		}
	}
}
