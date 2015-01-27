using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class AboutView
	{
		[Import]
		public MainViewModel ViewModel
		{
			get { return DataContext as MainViewModel; }
			set { DataContext = value; }
		}

		public AboutView()
		{
			InitializeComponent();
		}
	}
}
