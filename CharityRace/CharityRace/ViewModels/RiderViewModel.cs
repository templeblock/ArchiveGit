using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using ClassLibrary;

namespace CharityRace
{
	[Export]
	public class RiderViewModel : ViewModelBase
	{
		public IEnumerable<Rider> AllRiders { get; private set; }

		public IEnumerable<Rider> RidersTop10 { get {
			return AllRiders
				.Where(item => item.RiderId < 10);
		} }

		//public IEnumerable<Rider> RidersTop10 { get {
		//    var dataset = AllRiders
		//        .Where(item => item.RiderId < 10);
		//    return from fieldset in dataset
		//           select new
		//           {
		//               DonorId = fieldset.RiderId,
		//               Name = fieldset.Name,
		//               DonationsGoal = fieldset.DonationsGoal,
		//               MessageToDonors = fieldset.MessageToDonors,
		//               TotalDonations = fieldset.TotalDonations,
		//           };
		//} }

		public RiderViewModel()
		{
			AllRiders = App.Model.AllRiders;
		}
	}
}
