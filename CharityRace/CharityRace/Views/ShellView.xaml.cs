using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class ShellView
	{
		[Import]
		public HeaderView HeaderView
		{
			get { return x_HeaderRegion.Child as HeaderView; }
			set { x_HeaderRegion.Child = value; }
		}

		[Import]
		public NavigationView NavigationView
		{
			get { return x_NavigationRegion.Child as NavigationView; }
			set { x_NavigationRegion.Child = value; }
		}

		[Import]
		public ActiveContentView ActiveContentView
		{
			get { return x_ActiveContentRegion.Child as ActiveContentView; }
			set { x_ActiveContentRegion.Child = value; }
		}

		public ShellView()
		{
			InitializeComponent();
		}
	}
}