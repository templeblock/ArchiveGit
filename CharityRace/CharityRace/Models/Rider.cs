using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using ClassLibrary;

namespace CharityRace
{
	public class Rider : ViewModelBase
	{
		public int RiderId { get { return _RiderId; } set { SetProperty(ref _RiderId, value, MethodBase.GetCurrentMethod().Name); } }
		private int _RiderId;

		public string FirstName { get { return _FirstName; } set { SetProperty(ref _FirstName, value, MethodBase.GetCurrentMethod().Name); } }
		private string _FirstName;

		public string LastName { get { return _LastName; } set { SetProperty(ref _LastName, value, MethodBase.GetCurrentMethod().Name); } }
		private string _LastName;

		public string MessageToDonors { get { return _MessageToDonors; } set { SetProperty(ref _MessageToDonors, value, MethodBase.GetCurrentMethod().Name); } }
		private string _MessageToDonors;

		public double DonationsGoal { get { return _DonationsGoal; } set { SetProperty(ref _DonationsGoal, value, MethodBase.GetCurrentMethod().Name); } }
		private double _DonationsGoal;

		public double TotalDonations { get { return _TotalDonations; } set { SetProperty(ref _TotalDonations, value, MethodBase.GetCurrentMethod().Name); } }
		private double _TotalDonations;

		public Rider()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
		}

		public static IEnumerable<Rider> Load()
		{
			List<Dictionary<string, string>> dataset = CsvReader.FromResource("CharityRace;component/Content/SampleRiderData.csv");
			if (dataset == null)
				return null;
			return from fieldset in dataset
				select new Rider
				{
					RiderId = Convert.ToInt32(fieldset["RiderId"]),
					FirstName = fieldset["FirstName"],
					LastName = fieldset["LastName"],
					MessageToDonors = fieldset["MessageToDonors"],
					DonationsGoal = Convert.ToDouble(fieldset["DonationsGoal"]),
					TotalDonations = Convert.ToDouble(fieldset["TotalDonations"]),
				};
		}
	}
}
