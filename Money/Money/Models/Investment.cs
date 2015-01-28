using System.Runtime.Serialization;
using System.Xml.Serialization;
using ClassLibrary;

namespace Money
{
[DataContract]
[XmlRoot]
	public enum InvestmentType
	{
		Stock,
		Fund,
	}

[DataContract]
[XmlRoot]
	public class Investment : ViewModelBase
	{
[XmlAttribute]
[DataMember]
		public InvestmentType Type { get { return _Type; } set { SetProperty(ref _Type, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private InvestmentType _Type;
[XmlIgnore]
[IgnoreDataMember]
		public string Symbol { get { return _Symbol; } set { SetProperty(ref _Symbol, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Symbol;
[XmlIgnore]
[IgnoreDataMember]
		public string Name { get { return _Name; } set { SetProperty(ref _Name, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private string _Name;
[XmlAttribute]
[DataMember]
		public int Shares { get { return _Shares; } set { SetProperty(ref _Shares, value, System.Reflection.MethodBase.GetCurrentMethod().Name); } }
		private int _Shares;
	}
}
