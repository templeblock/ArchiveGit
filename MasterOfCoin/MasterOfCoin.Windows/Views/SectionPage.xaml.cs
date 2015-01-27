using MasterOfCoin.Common;
using MasterOfCoin.Data;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace MasterOfCoin
{
    public sealed partial class SectionPage : Page
    {
		public NavigationHelper NavigationHelper { get; private set; }
		public ObservableDictionary DefaultViewModel { get; private set; }

        public SectionPage()
        {
			DefaultViewModel = new ObservableDictionary();
			InitializeComponent();

			NavigationHelper = new NavigationHelper(this);
			NavigationHelper.LoadState += NavigationHelperLoadState;
        }

		private void ItemViewItemClick(object sender, ItemClickEventArgs e)
		{
			// Navigate to the appropriate destination page, configuring the new page by passing required information as a navigation parameter
			var itemId = ((SampleDataItem)e.ClickedItem).UniqueId;
			Frame.Navigate(typeof(ItemPage), itemId);
		}

		// Populates the page with content passed during navigation.
		// Any saved state is also provided when recreating a page from a prior session.
		// The state will be null the first time a page is visited.
        private async void NavigationHelperLoadState(object sender, LoadStateEventArgs e)
        {
            var group = await SampleDataSource.GetGroupAsync((string)e.NavigationParameter);
            DefaultViewModel["Group"] = group;
            DefaultViewModel["Items"] = group.Items;
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