using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class RiderView
	{
		[Import]
		public RiderViewModel ViewModel
		{
			get { return DataContext as RiderViewModel; }
			set { DataContext = value; }
		}

		public RiderView()
		{
			InitializeComponent();
		}
	}
}
