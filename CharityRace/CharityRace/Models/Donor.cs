using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using ClassLibrary;

namespace CharityRace
{
	public class Donor : ViewModelBase
	{
		public int DonorId { get { return _Donord; } set { SetProperty(ref _Donord, value, MethodBase.GetCurrentMethod().Name); } }
		private int _Donord;

		public string FirstName { get { return _FirstName; } set { SetProperty(ref _FirstName, value, MethodBase.GetCurrentMethod().Name); } }
		private string _FirstName;

		public string LastName { get { return _LastName; } set { SetProperty(ref _LastName, value, MethodBase.GetCurrentMethod().Name); } }
		private string _LastName;

		public string StreetAddress { get { return _StreetAddress; } set { SetProperty(ref _StreetAddress, value, MethodBase.GetCurrentMethod().Name); } }
		private string _StreetAddress;

		public string City { get { return _City; } set { SetProperty(ref _City, value, MethodBase.GetCurrentMethod().Name); } }
		private string _City;

		public string State { get { return _State; } set { SetProperty(ref _State, value, MethodBase.GetCurrentMethod().Name); } }
		private string _State;

		public Donor()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
		}

		public static IEnumerable<Donor> Load()
		{
			List<Dictionary<string, string>> dataset = CsvReader.FromResource("CharityRace;component/Content/SampleDonorData.csv");
			if (dataset == null)
				return null;
			return from fieldset in dataset
				select new Donor
				{
					DonorId = Convert.ToInt32(fieldset["DonorId"]),
					FirstName = fieldset["FirstName"],
					LastName = fieldset["LastName"],
					StreetAddress = fieldset["StreetAddress"],
					City = fieldset["City"],
					State = fieldset["State"],
				};
		}
	}
}
