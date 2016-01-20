using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Xml.Linq;
using ClassLibrary;

namespace TrumpITReporting
{
	public class AddressDBCollection : ObservableCollection<AddressDB>, IDisposable
	{
		public void Dispose()
		{
			foreach (AddressDB item in this)
				item.Dispose();
			Clear();
		}

		internal void Initialize()
		{
		}

		internal void GetAddressFromAddressID(string addressID)
		{
			AddressDBSqlQuery query = new AddressDBSqlQuery(addressID);
			query.Result += delegate(Collection<AddressDB> collection)
			{
				base.Clear();
				if (collection == null)
					return;

				foreach (AddressDB item in collection)
					base.Add(item);
			};
			query.Execute("Globenet");
		}
	}

	internal class AddressDBSqlQuery : DBServiceBase<AddressDB>
	{
		private string _addressID;

		internal AddressDBSqlQuery(string addressID)
		{
			_addressID = addressID;
		}

		internal override string SqlQuery()
		{
			return string.Format(@"
				SELECT *
				FROM Address
				WHERE (AddressID = '{0}')", _addressID);
		}

		internal override IEnumerable LinqQuery(XDocument xmlDoc)
		{
			IEnumerable<AddressDB> query = from item in xmlDoc.Root.Descendants()
			select new AddressDB
			{
				AddressID = item.AttributeValue("AddressID").ToInt(),
				AccountID = item.AttributeValue("AccountID").ToInt(),
				AdrTypeID = (short)item.AttributeValue("AdrTypeID").ToInt(),
				Address1 = item.AttributeValue("Address1"),
				Address2 = item.AttributeValue("Address2"),
				City = item.AttributeValue("City"),
				County = item.AttributeValue("County"),
				State = item.AttributeValue("State"),
				ZipCode = item.AttributeValue("ZipCode"),
				Longitude = item.AttributeValue("Longitude").ToDecimal(),
				Latitude = item.AttributeValue("Latitude").ToDecimal(),
				DupKeyA = item.AttributeValue("DupKeyA"),
				DupKeyH = item.AttributeValue("DupKeyH"),
				CreateDate = item.AttributeValue("CreateDate").ToDateTime(),
				ChangeDate = item.AttributeValue("ChangeDate").ToDateTime(),
				ChangeUser = item.AttributeValue("ChangeUser"),
				ProgName = item.AttributeValue("ProgName"),
				AddressTy = (short)item.AttributeValue("AddressTy").ToInt(),
				Zip = item.AttributeValue("Zip"),
			};

			return query;
		}
	}

	public partial class AddressDB : NotifyPropertyChanges
	{
		private int _AddressID;
		private int _AccountID;
		private short _AdrTypeID;
		private string _Address1;
		private string _Address2;
		private string _City;
		private string _County;
		private string _State;
		private string _ZipCode;
		private decimal _Longitude;
		private decimal _Latitude;
		private string _DupKeyA;
		private string _DupKeyH;
		private System.DateTime _CreateDate;
		private System.Nullable<System.DateTime> _ChangeDate;
		private string _ChangeUser;
		private string _ProgName;
		private short _AddressTy;
		private string _Zip;

		public AddressDB()
		{
		}
		
		public override void Dispose()
		{
			base.Dispose();
		}

		public int AddressID { get { return _AddressID; } set { SetProperty(ref _AddressID, value); } }
		
		public int AccountID { get { return _AccountID; } set { SetProperty(ref _AccountID, value); } }
		
		public short AdrTypeID { get { return _AdrTypeID; } set { SetProperty(ref _AdrTypeID, value); } }
		
		public string Address1 { get { return _Address1; } set { SetProperty(ref _Address1, value); } }
		
		public string Address2 { get { return _Address2; } set { SetProperty(ref _Address2, value); } }
		
		public string City { get { return _City; } set { SetProperty(ref _City, value); } }
		
		public string County { get { return _County; } set { SetProperty(ref _County, value); } }
		
		public string State { get { return _State; } set { SetProperty(ref _State, value); } }
		
		public string ZipCode { get { return _ZipCode; } set { SetProperty(ref _ZipCode, value); } }
		
		public decimal Longitude { get { return _Longitude; } set { SetProperty(ref _Longitude, value); } }
		
		public decimal Latitude { get { return _Latitude; } set { SetProperty(ref _Latitude, value); } }
		
		public string DupKeyA { get { return _DupKeyA; } set { SetProperty(ref _DupKeyA, value); } }
		
		public string DupKeyH { get { return _DupKeyH; } set { SetProperty(ref _DupKeyH, value); } }
		
		public System.DateTime CreateDate { get { return _CreateDate; } set { SetProperty(ref _CreateDate, value); } }
		
		public System.Nullable<System.DateTime> ChangeDate { get { return _ChangeDate; } set { SetProperty(ref _ChangeDate, value); } }
		
		public string ChangeUser { get { return _ChangeUser; } set { SetProperty(ref _ChangeUser, value); } }
		
		public string ProgName { get { return _ProgName; } set { SetProperty(ref _ProgName, value); } }
		
		public short AddressTy { get { return _AddressTy; } set { SetProperty(ref _AddressTy, value); } }
		
		public string Zip { get { return _Zip; } set { SetProperty(ref _Zip, value); } }
	}
}
