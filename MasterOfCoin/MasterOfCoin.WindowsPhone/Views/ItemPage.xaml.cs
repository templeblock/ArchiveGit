using MasterOfCoin.Common;
using MasterOfCoin.Data;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace MasterOfCoin
{
    public sealed partial class ItemPage : Page
    {
		public NavigationHelper NavigationHelper { get; private set; }
		public ObservableDictionary DefaultViewModel { get; private set; }

        public ItemPage()
        {
			DefaultViewModel = new ObservableDictionary();
			InitializeComponent();

			NavigationHelper = new NavigationHelper(this);
			NavigationHelper.LoadState += NavigationHelperLoadState;
		} 

		// Populates the page with content passed during navigation.
		// Any saved state is also provided when recreating a page from a prior session.
		// The state will be null the first time a page is visited.
		private async void NavigationHelperLoadState(object sender, LoadStateEventArgs e)
		{
			var item = await SampleDataSource.GetGroupAsync((string)e.NavigationParameter);
			DefaultViewModel["Item"] = item;
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