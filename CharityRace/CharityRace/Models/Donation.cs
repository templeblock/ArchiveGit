using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using ClassLibrary;

namespace CharityRace
{
	public class Donation : ViewModelBase
	{
		public int DonationId { get { return _DonationId; } set { SetProperty(ref _DonationId, value, MethodBase.GetCurrentMethod().Name); } }
		private int _DonationId;

		public string DonorId { get { return _Donord; } set { SetProperty(ref _Donord, value, MethodBase.GetCurrentMethod().Name); } }
		private string _Donord;

		public string RiderId { get { return _RiderId; } set { SetProperty(ref _RiderId, value, MethodBase.GetCurrentMethod().Name); } }
		private string _RiderId;

		public double Amount { get { return _Amount; } set { SetProperty(ref _Amount, value, MethodBase.GetCurrentMethod().Name); } }
		private double _Amount;

		public string Date { get { return _Date; } set { SetProperty(ref _Date, value, MethodBase.GetCurrentMethod().Name); } }
		private string _Date;

		public string MessageToRider { get { return _MessageToRider; } set { SetProperty(ref _MessageToRider, value, MethodBase.GetCurrentMethod().Name); } }
		private string _MessageToRider;

		public Donation()
		{
		}

		public override void Dispose()
		{
			base.Dispose();
		}

		public static IEnumerable<Donation> Load()
		{
			List<Dictionary<string, string>> dataset = CsvReader.FromResource("CharityRace;component/Content/SampleDonationData.csv");
			if (dataset == null)
				return null;
			return from fieldset in dataset
				select new Donation
				{
					DonationId = Convert.ToInt32(fieldset["DonationId"]),
					DonorId = fieldset["DonorId"],
					RiderId = fieldset["RiderId"],
					Amount = Convert.ToDouble(fieldset["Amount"]),
					Date = fieldset["Date"],
					MessageToRider = fieldset["MessageToRider"],
				};
		}
	}
}
