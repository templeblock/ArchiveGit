using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class MarketingLeadCollection : ObservableCollection<MarketingLead>, IDisposable
	{
		public ListOrder ListOrder { get { return _ListOrder; } set { _ListOrder = value; } }
		private ListOrder _ListOrder = new ListOrder();

		public int NewMarketers { get { return _NewMarketers; } set { _NewMarketers = value; } }
		private int _NewMarketers;

		public int NewCustomers { get { return _NewCustomers; } set { _NewCustomers = value; } }
		private int _NewCustomers;

		public void Dispose()
		{
			foreach (MarketingLead item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}
	}

	public class MarketingLead : Person
	{
		public bool IsFriend { get { return _IsFriend; } set { SetProperty(ref _IsFriend, value); } }
		private bool _IsFriend;

		public bool IsMarketter { get { return _IsMarketter; } set { SetProperty(ref _IsMarketter, value); } }
		private bool _IsMarketter;

		public bool IsCustomer { get { return _IsCustomer; } set { SetProperty(ref _IsCustomer, value); } }
		private bool _IsCustomer;

		public bool IsReferral { get { return _IsReferral; } set { SetProperty(ref _IsReferral, value); } }
		private bool _IsReferral;

		private int _LeadID;
		private string _LeadType;
		private Nullable<int> _ListOrderID;
		private Nullable<int> _SponsorID;
		//private Nullable<int> _AccountID;
		//private Nullable<int> _PersonID;
		private string _First_Name;
		private string _Middle_Name;
		private string _Last_Name;
		private string _Address1;
		private string _Addr2;
		private string _City;
		private string _State;
		private string _ZIP;
		private string _Country;
		private string _Email;
		private string _Phone;
		private string _RelationshipToConnector;
		private string _PersonalityType;
		//private Nullable<DateTime> _DateOfBirth;
		private string _Gender;
		private string _EstimatedHHIncome;
		private string _EstimatedHHValue;
		private string _HomeOwnership;
		private string _FamilyType;
		private Nullable<DateTime> _LeadCreateDate;
		private Nullable<DateTime> _LeadExpiryDateForAccount;
		private Nullable<DateTime> _LicenseExpiryDate;
		private Nullable<DateTime> _LeadConvertedDate;
		private Nullable<char> _DM_Flag;
		private Nullable<char> _Email_Flag;
		
		public MarketingLead()
		{
		}
		
		public override void Dispose()
		{
			base.Dispose();
		}

		public int LeadID { get { return this._LeadID; } set { SetProperty(ref _LeadID, value); } }
		public string LeadType { get { return this._LeadType; } set { SetProperty(ref _LeadType, value); } }
		public Nullable<int> ListOrderID { get { return this._ListOrderID; } set { SetProperty(ref _ListOrderID, value); } }
		public Nullable<int> SponsorID { get { return this._SponsorID; } set { SetProperty(ref _SponsorID, value); } }
		//public Nullable<int> AccountID { get { return this._AccountID; } set { SetProperty(ref _AccountID, value); } }
		//public Nullable<int> PersonID { get { return this._PersonID; } set { SetProperty(ref _PersonID, value); } }
		public string First_Name { get { return this._First_Name; } set { SetProperty(ref _First_Name, value); } }
		public string Middle_Name { get { return this._Middle_Name; } set { SetProperty(ref _Middle_Name, value); } }
		public string Last_Name { get { return this._Last_Name; } set { SetProperty(ref _Last_Name, value); } }
		public string Address1 { get { return this._Address1; } set { SetProperty(ref _Address1, value); } }
		public string Addr2 { get { return this._Addr2; } set { SetProperty(ref _Addr2, value); } }
		public string City { get { return this._City; } set { SetProperty(ref _City, value); } }
		public string State { get { return this._State; } set { SetProperty(ref _State, value); } }
		public string ZIP { get { return this._ZIP; } set { SetProperty(ref _ZIP, value); } }
		public string Country { get { return this._Country; } set { SetProperty(ref _Country, value); } }
		public string Email { get { return this._Email; } set { SetProperty(ref _Email, value); } }
		public string Phone { get { return this._Phone; } set { SetProperty(ref _Phone, value); } }
		public string RelationshipToConnector { get { return this._RelationshipToConnector; } set { SetProperty(ref _RelationshipToConnector, value); } }
		public string PersonalityType { get { return this._PersonalityType; } set { SetProperty(ref _PersonalityType, value); } }
		//public Nullable<DateTime> DateOfBirth { get { return this._DateOfBirth; } set { SetProperty(ref _DateOfBirth, value); } }
		public string Gender { get { return this._Gender; } set { SetProperty(ref _Gender, value); } }
		public string EstimatedHHIncome { get { return this._EstimatedHHIncome; } set { SetProperty(ref _EstimatedHHIncome, value); } }
		public string EstimatedHHValue { get { return this._EstimatedHHValue; } set { SetProperty(ref _EstimatedHHValue, value); } }
		public string HomeOwnership { get { return this._HomeOwnership; } set { SetProperty(ref _HomeOwnership, value); } }
		public string FamilyType { get { return this._FamilyType; } set { SetProperty(ref _FamilyType, value); } }
		public Nullable<DateTime> LeadCreateDate { get { return this._LeadCreateDate; } set { SetProperty(ref _LeadCreateDate, value); } }
		public Nullable<DateTime> LeadExpiryDateForAccount { get { return this._LeadExpiryDateForAccount; } set { SetProperty(ref _LeadExpiryDateForAccount, value); } }
		public Nullable<DateTime> LicenseExpiryDate { get { return this._LicenseExpiryDate; } set { SetProperty(ref _LicenseExpiryDate, value); } }
		public Nullable<DateTime> LeadConvertedDate { get { return this._LeadConvertedDate; } set { SetProperty(ref _LeadConvertedDate, value); } }
		public Nullable<char> DM_Flag { get { return this._DM_Flag; } set { SetProperty(ref _DM_Flag, value); } }
		public Nullable<char> Email_Flag { get { return this._Email_Flag; } set { SetProperty(ref _Email_Flag, value); } }
	}
}
