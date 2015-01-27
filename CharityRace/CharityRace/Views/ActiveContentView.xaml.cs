using System.ComponentModel.Composition;

namespace CharityRace
{
	[Export]
	public partial class ActiveContentView
	{
		[Import]
		public MainViewModel ViewModel
		{
			get { return DataContext as MainViewModel; }
			set { DataContext = value; }
		}

		[Import]
		public WelcomeView WelcomeView
		{
			get { return x_WelcomeRegion.Child as WelcomeView; }
			set { x_WelcomeRegion.Child = value; }
		}

		[Import]
		public RiderView RiderView
		{
			get { return x_RiderRegion.Child as RiderView; }
			set { x_RiderRegion.Child = value; }
		}

		[Import]
		public DonorView DonorView
		{
			get { return x_DonorRegion.Child as DonorView; }
			set { x_DonorRegion.Child = value; }
		}

		[Import]
		public DonationView DonationView
		{
			get { return x_DonationRegion.Child as DonationView; }
			set { x_DonationRegion.Child = value; }
		}

		[Import]
		public AboutView AboutView
		{
			get { return x_AboutRegion.Child as AboutView; }
			set { x_AboutRegion.Child = value; }
		}

		public ActiveContentView()
		{
			InitializeComponent();
		}
	}
}
