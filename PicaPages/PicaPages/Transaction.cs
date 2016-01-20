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
	public enum ETransactionType
	{
		eNoOp,
	}


[DataContract(Name="Transaction")]
[XmlRoot("Transaction")]
	public class Transaction
	{
[XmlAttribute]
[DataMember]
		public string Name { set { m_sName = value; } get { return m_sName; } }
		private string m_sName;
[XmlAttribute]
[DataMember]
		public tMillicents Value { set { m_lValue = value; } get { return m_lValue; } }
		private tMillicents m_lValue;
[XmlAttribute]
[DataMember]
		public ETransactionType Type { set { m_eType = value; } get { return m_eType; } }
		private ETransactionType m_eType;

		/////////////////////////////////////////////////////////////////////////////
		public Transaction()
		{
			m_sName = "";
			m_lValue = 0;
			m_eType = ETransactionType.eNoOp;
		}

		/////////////////////////////////////////////////////////////////////////////
		public Transaction(string _sName, ETransactionType _eType, double _fValue, EAccountType _eAccountType, EAccountType _eAccountType2, JulianDay lStartDateLimit, JulianDay lEndDateLimit)
		{
			m_sName = _sName;
			m_lValue = (tMillicents)(_fValue * 1000.0);
			m_eType = _eType;
		}

		/////////////////////////////////////////////////////////////////////////////
		public Transaction(string _sName, ETransactionType _eType, double _fValue, EAccountType _eAccountType, EAccountType _eAccountType2)
			: this(_sName, _eType, _fValue, _eAccountType, _eAccountType2, 0, 0)
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		~Transaction()
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		public bool Run(Document document, JulianDay lDate, JulianDay lParentStartDate, JulianDay lParentEndDate)
		{
			return false;
		}
	}
}
