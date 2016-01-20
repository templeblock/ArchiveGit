using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;
using System.Xml.Serialization;
using System.Xml;
using System.IO;

namespace TrumpITReporting
{
	public enum PersonType
	{
		Unknown = -1,
		Lead,
		Customer,
		Marketer,
		Both,
	}

	public class PersonCollection : ObservableCollection<Person>, IDisposable
	{
		public void Dispose()
		{
			foreach (Person item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public partial class Person : NotifyPropertyChanges
	{
		public PersonType Type { get { return _Type; } set { SetProperty(ref _Type, value); } }
		private PersonType _Type = PersonType.Unknown;

		public Address Address { get { return _Address; } set { SetProperty(ref _Address, value); } }
		private Address _Address = new Address();

		private int _PersonID;
		private int _AccountID;
		private string _NameType;
		private byte _CompanyInd;
		private byte _PrimaryInd;
		private string _LastName;
		private string _FirstName;
		private string _MiddleName;
		private string _NickName;
		private string _Title;
		private string _Suffix;
		private string _Credentials;
		private string _SalutationName;
		private string _FiscalType;
		private string _FiscalID;
		private Nullable<char> _Sex;
		private Nullable<DateTime> _DateOfBirth;
		private Nullable<byte> _CnnVolNum;
		private Nullable<byte> _FnVolNum;
		private string _DayPhone;
		private string _EveningPhone;
		private string _FaxPhone;
		private string _EMail;
		private string _FindName;
		private byte _ActiveInd;
		private DateTime _ChangeDate;
		private string _Usr;
		private string _ProgName;
		private string _DupKeyS;
		private string _DupKeyL;
		private Nullable<byte> _SendCheckInd;
		private string _LoginName;
		private string _LoginPassword;
		private Nullable<DateTime> _CreateDate;
		private string _ChangeUser;
		private Nullable<int> _RefPaidOrderNum;
		private Nullable<byte> _SignedHIPAAInd;
		private string _Ethnicity;
		private string _ReferrerName;
		private Nullable<int> _RefPersonID;

		public Person()
		{
		}
		
		public override void Dispose()
		{
			base.Dispose();
		}

		public int PersonID { get { return _PersonID; } set { SetProperty(ref _PersonID, value); } }
		public int AccountID { get { return _AccountID; } set { SetProperty(ref _AccountID, value); } }
		public string NameType { get { return _NameType; } set { SetProperty(ref _NameType, value); } }
		public byte CompanyInd { get { return _CompanyInd; } set { SetProperty(ref _CompanyInd, value); } }
		public byte PrimaryInd { get { return _PrimaryInd; } set { SetProperty(ref _PrimaryInd, value); } }
		public string LastName { get { return _LastName; } set { SetProperty(ref _LastName, value); } }
		public string FirstName { get { return _FirstName; } set { SetProperty(ref _FirstName, value); } }
		public string MiddleName { get { return _MiddleName; } set { SetProperty(ref _MiddleName, value); } }
		public string NickName { get { return _NickName; } set { SetProperty(ref _NickName, value); } }
		public string Title { get { return _Title; } set { SetProperty(ref _Title, value); } }
		public string Suffix { get { return _Suffix; } set { SetProperty(ref _Suffix, value); } }
		public string Credentials { get { return _Credentials; } set { SetProperty(ref _Credentials, value); } }
		public string SalutationName { get { return _SalutationName; } set { SetProperty(ref _SalutationName, value); } }
		public string FiscalType { get { return _FiscalType; } set { SetProperty(ref _FiscalType, value); } }
		public string FiscalID { get { return _FiscalID; } set { SetProperty(ref _FiscalID, value); } }
		public Nullable<char> Sex { get { return _Sex; } set { SetProperty(ref _Sex, value); } }
		public Nullable<DateTime> DateOfBirth { get { return _DateOfBirth; } set { SetProperty(ref _DateOfBirth, value); } }
		public Nullable<byte> CnnVolNum { get { return _CnnVolNum; } set { SetProperty(ref _CnnVolNum, value); } }
		public Nullable<byte> FnVolNum { get { return _FnVolNum; } set { SetProperty(ref _FnVolNum, value); } }
		public string DayPhone { get { return _DayPhone; } set { SetProperty(ref _DayPhone, value); } }
		public string EveningPhone { get { return _EveningPhone; } set { SetProperty(ref _EveningPhone, value); } }
		public string FaxPhone { get { return _FaxPhone; } set { SetProperty(ref _FaxPhone, value); } }
		public string EMail { get { return _EMail; } set { SetProperty(ref _EMail, value); } }
		public string FindName { get { return _FindName; } set { SetProperty(ref _FindName, value); } }
		public byte ActiveInd { get { return _ActiveInd; } set { SetProperty(ref _ActiveInd, value); } }
		public DateTime ChangeDate { get { return _ChangeDate; } set { SetProperty(ref _ChangeDate, value); } }
		public string Usr { get { return _Usr; } set { SetProperty(ref _Usr, value); } }
		public string ProgName { get { return _ProgName; } set { SetProperty(ref _ProgName, value); } }
		public string DupKeyS { get { return _DupKeyS; } set { SetProperty(ref _DupKeyS, value); } }
		public string DupKeyL { get { return _DupKeyL; } set { SetProperty(ref _DupKeyL, value); } }
		public Nullable<byte> SendCheckInd { get { return _SendCheckInd; } set { SetProperty(ref _SendCheckInd, value); } }
		public string LoginName { get { return _LoginName; } set { SetProperty(ref _LoginName, value); } }
		public string LoginPassword { get { return _LoginPassword; } set { SetProperty(ref _LoginPassword, value); } }
		public Nullable<DateTime> CreateDate { get { return _CreateDate; } set { SetProperty(ref _CreateDate, value); } }
		public string ChangeUser { get { return _ChangeUser; } set { SetProperty(ref _ChangeUser, value); } }
		public Nullable<int> RefPaidOrderNum { get { return _RefPaidOrderNum; } set { SetProperty(ref _RefPaidOrderNum, value); } }
		public Nullable<byte> SignedHIPAAInd { get { return _SignedHIPAAInd; } set { SetProperty(ref _SignedHIPAAInd, value); } }
		public string Ethnicity { get { return _Ethnicity; } set { SetProperty(ref _Ethnicity, value); } }
		public string ReferrerName { get { return _ReferrerName; } set { SetProperty(ref _ReferrerName, value); } }
		public Nullable<int> RefPersonID { get { return _RefPersonID; } set { SetProperty(ref _RefPersonID, value); } }
	}
}
