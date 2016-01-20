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
	public class PersonDBCollection : ObservableCollection<PersonDB>, IDisposable
	{
		public void Dispose()
		{
			foreach (PersonDB item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}

		internal void GetPersonFromAccountID(string accountID)
		{
			PersonDBSqlQuery query = new PersonDBSqlQuery(accountID);
			query.Result += delegate(Collection<PersonDB> collection)
			{
				base.Clear();
				if (collection == null)
					return;

				foreach (PersonDB item in collection)
					base.Add(item);
			};
			query.Execute("Globenet");
		}
	}

	internal class PersonDBSqlQuery : DBServiceBase<PersonDB>
	{
		private string _accountID;

		internal PersonDBSqlQuery(string accountID)
		{
			_accountID = accountID;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT p.PersonID, p.AccountID, p.CompanyInd, p.PrimaryInd, p.LastName, p.FirstName, p.MiddleName, 
					p.NickName, p.Title, p.Suffix, p.Credentials, p.SalutationID, p.FiscalID, p.Sex, p.DateOfBirth, 
					p.CnnVolNum, p.FnVolNum, p.DayPhone, p.EveningPhone, p.FaxPhone, p.EMail, p.FindName, p.ActiveInd, 
					p.ChangeDate, p.Usr, p.ProgName, p.DupKeyS, p.DupKeyL, p.SendCheckInd, p.LoginName, p.LoginPassword, 
					p.CreateDate, p.ChangeUser, p.RefPaidOrderNum, p.SignedHIPAAInd, p.ReferrerName, p.RefPersonID, 
					Salutation.Name AS 'SalutationName', FiscalIDTy.Descr AS 'FiscalType', NameTy.Descr AS 'NameType'
				FROM Person AS p
				LEFT OUTER JOIN Salutation ON p.SalutationID = Salutation.SalutationID
				LEFT OUTER JOIN NameTy ON p.NameTy = NameTy.NameTy
				LEFT OUTER JOIN FiscalIDTy ON p.FiscalIdTy = FiscalIDTy.FiscalIDTy
				LEFT OUTER JOIN Ethnicity ON p.EthnicityCode = Ethnicity.EthnicityCode
				WHERE (AccountID = '{0}')", _accountID);
			//, Ethnicity.EthnicityName AS 'Ethnicity'
		}

		internal override IEnumerable LinqQuery(XDocument xmlDoc)
		{
			IEnumerable<PersonDB> query = from item in xmlDoc.Root.Descendants()
			//select Extract(item);
			select new PersonDB
			{
				PersonID = item.AttributeValue("PersonID").ToInt(),
				AccountID = item.AttributeValue("AccountID").ToInt(),
				NameType = item.AttributeValue("NameType"),
				CompanyInd = item.AttributeValue("CompanyInd").ToByte(),
				PrimaryInd = item.AttributeValue("PrimaryInd").ToByte(),
				LastName = item.AttributeValue("LastName"),
				FirstName = item.AttributeValue("FirstName"),
				MiddleName = item.AttributeValue("MiddleName"),
				NickName = item.AttributeValue("NickName"),
				Title = item.AttributeValue("Title"),
				Suffix = item.AttributeValue("Suffix"),
				Credentials = item.AttributeValue("Credentials"),
				SalutationName = item.AttributeValue("SalutationName"),
				FiscalType = item.AttributeValue("FiscalType"),
				FiscalID = item.AttributeValue("FiscalID"),
				Sex = item.AttributeValue("Sex").ToChar(),
				DateOfBirth = item.AttributeValue("DateOfBirth").ToDateTime(),
				CnnVolNum = item.AttributeValue("CnnVolNum").ToByte(),
				FnVolNum = item.AttributeValue("FnVolNum").ToByte(),
				DayPhone = item.AttributeValue("DayPhone"),
				EveningPhone = item.AttributeValue("EveningPhone"),
				FaxPhone = item.AttributeValue("FaxPhone"),
				EMail = item.AttributeValue("EMail"),
				FindName = item.AttributeValue("FindName"),
				ActiveInd = item.AttributeValue("ActiveInd").ToByte(),
				ChangeDate = item.AttributeValue("ChangeDate").ToDateTime(),
				Usr = item.AttributeValue("Usr"),
				ProgName = item.AttributeValue("ProgName"),
				DupKeyS = item.AttributeValue("DupKeyS"),
				DupKeyL = item.AttributeValue("DupKeyL"),
				SendCheckInd = item.AttributeValue("SendCheckInd").ToByte(),
				LoginName = item.AttributeValue("LoginName"),
				LoginPassword = item.AttributeValue("LoginPassword"),
				CreateDate = item.AttributeValue("CreateDate").ToDateTime(),
				ChangeUser = item.AttributeValue("ChangeUser"),
				RefPaidOrderNum = item.AttributeValue("RefPaidOrderNum").ToInt(),
				SignedHIPAAInd = item.AttributeValue("SignedHIPAAInd").ToByte(),
				Ethnicity = item.AttributeValue("Ethnicity"),
				ReferrerName = item.AttributeValue("ReferrerName"),
				RefPersonID = item.AttributeValue("RefPersonID").ToInt(),
			};

			return query;
		}
	}

	public partial class PersonDB : NotifyPropertyChanges
	{
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

		public PersonDB()
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
