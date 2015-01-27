using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class DonationView
	{
		[Import]
		public DonationViewModel ViewModel
		{
			get { return DataContext as DonationViewModel; }
			set { DataContext = value; }
		}

		public DonationView()
		{
			InitializeComponent();
		}
	}
}
