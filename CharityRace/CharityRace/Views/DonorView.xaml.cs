using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class DonorView
	{
		[Import]
		public DonorViewModel ViewModel
		{
			get { return DataContext as DonorViewModel; }
			set { DataContext = value; }
		}

		public DonorView()
		{
			InitializeComponent();
		}
	}
}
