using System;
using MasterOfCoin.Common;
using MasterOfCoin.Data;
using Windows.ApplicationModel.Resources;
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
            var itemId = ((SampleDataItem)e.ClickedItem).UniqueId;
	        if (Frame.Navigate(typeof(ItemPage), itemId))
		        return;
	        var resourceLoader = ResourceLoader.GetForCurrentView("Resources");
	        throw new Exception(resourceLoader.GetString("NavigationFailedExceptionMessage"));
        }

		// Populates the page with content passed during navigation.
		// Any saved state is also provided when recreating a page from a prior session.
		// The state will be null the first time a page is visited.
		private async void NavigationHelperLoadState(object sender, LoadStateEventArgs e)
		{
			var group = await SampleDataSource.GetGroupAsync((string)e.NavigationParameter);
			DefaultViewModel["Group"] = group;
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