using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class HeaderView
	{
		[Import]
		public MainViewModel ViewModel
		{
			get { return DataContext as MainViewModel; }
			set { DataContext = value; }
		}

		public HeaderView()
		{
			InitializeComponent();
		}
	}
}
