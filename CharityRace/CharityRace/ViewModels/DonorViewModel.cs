using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using ClassLibrary;

namespace CharityRace
{
	[Export]
	public class DonorViewModel : ViewModelBase
	{
		public IEnumerable<Donor> AllDonors { get; private set; }

		public IEnumerable<Donor> DonorsTop10 { get {
			return AllDonors
				.Where(item => item.DonorId < 10);
		} }

		public DonorViewModel()
		{
			AllDonors = App.Model.AllDonors;
		}
	}
}
