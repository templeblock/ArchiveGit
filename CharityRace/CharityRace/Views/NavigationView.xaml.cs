using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class NavigationView
	{
		[Import]
		public MainViewModel ViewModel
		{
			get { return DataContext as MainViewModel; }
			set { DataContext = value; }
		}

		public NavigationView()
		{
			InitializeComponent();
		}
	}
}
