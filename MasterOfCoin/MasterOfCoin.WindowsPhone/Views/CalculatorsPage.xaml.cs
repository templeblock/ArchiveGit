using MasterOfCoin.Common;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace MasterOfCoin
{
	public sealed partial class CalculatorsPage : Page
	{
		public NavigationHelper NavigationHelper { get; private set; }

		private readonly CalculatorViewModel _viewModel;

		public CalculatorsPage()
		{
			InitializeComponent();
			_viewModel = new CalculatorViewModel();
			DataContext = _viewModel;

			NavigationHelper = new NavigationHelper(this);
			NavigationHelper.LoadState += NavigationHelper_LoadState;
			NavigationHelper.SaveState += NavigationHelper_SaveState;
		}

		// Populates the page with content passed during navigation.
		// Any saved state is also provided when recreating a page from a prior session.
		// The state will be null the first time a page is visited.
		private void NavigationHelper_LoadState(object sender, LoadStateEventArgs e)
		{
		}

		// Preserves state associated with this page in case the application is suspended or the page is discarded from the navigation cache.
		// Values must conform to the serialization requirements of SuspensionManager.SessionState.
		private void NavigationHelper_SaveState(object sender, SaveStateEventArgs e)
		{
		}

		protected override void OnNavigatedTo(NavigationEventArgs e)
		{
			NavigationHelper.OnNavigatedTo(e);
		}

		protected override void OnNavigatedFrom(NavigationEventArgs e)
		{
			NavigationHelper.OnNavigatedFrom(e);
		}
	}
}
