using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class WelcomeView
	{
		[Import]
		public MainViewModel ViewModel
		{
			get { return DataContext as MainViewModel; }
			set { DataContext = value; }
		}

		public WelcomeView()
		{
			InitializeComponent();
		}
	}
}
