using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using ClassLibrary;

namespace CharityRace
{
	[Export]
	public class DonationViewModel : ViewModelBase
	{
		public IEnumerable<Donation> AllDonations { get; private set; }

		public IEnumerable<Donation> DonationsTop10 { get {
			return AllDonations
				.Where(item => item.DonationId < 10);
		} }

		public DonationViewModel()
		{
			AllDonations = App.Model.AllDonations;
		}
	}
}
