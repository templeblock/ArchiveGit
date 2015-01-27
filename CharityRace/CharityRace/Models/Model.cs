using System;
using System.Collections.Generic;

namespace CharityRace
{
	public class Model
	{
		public IEnumerable<Rider> AllRiders { get; private set; }
		public IEnumerable<Donor> AllDonors { get; private set; }
		public IEnumerable<Donation> AllDonations { get; private set; }

		public Model()
		{
		}

		public void Load()
		{
			AllDonors = Donor.Load();
			AllRiders = Rider.Load();
			AllDonations = Donation.Load();
		}
	}
}
