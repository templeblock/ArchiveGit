using System;
using MasterOfCoin.Common;
using MasterOfCoin.Data;
using Windows.ApplicationModel.Resources;
using Windows.Graphics.Display;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace MasterOfCoin
{
    public sealed partial class HubPage : Page
    {
		public NavigationHelper NavigationHelper { get; private set; }
		public ObservableDictionary DefaultViewModel { get; private set; }
		
		private readonly ResourceLoader _resourceLoader = ResourceLoader.GetForCurrentView("Resources");

        public HubPage()
        {
			DefaultViewModel = new ObservableDictionary();
			InitializeComponent();

            NavigationCacheMode = NavigationCacheMode.Required;

			NavigationHelper = new NavigationHelper(this);
			NavigationHelper.LoadState += NavigationHelperLoadState;

			// Hub is only supported in Portrait orientation
			DisplayInformation.AutoRotationPreferences = DisplayOrientations.Portrait;
		}

        private void GroupSectionItemClick(object sender, ItemClickEventArgs e)
        {
            var groupId = ((SampleDataGroup)e.ClickedItem).UniqueId;
            if (!Frame.Navigate(typeof(SectionPage), groupId))
            {
                throw new Exception(_resourceLoader.GetString("NavigationFailedExceptionMessage"));
            }
        }

        private void ItemViewItemClick(object sender, ItemClickEventArgs e)
        {
            var itemId = ((SampleDataItem)e.ClickedItem).UniqueId;
			//j if (!Frame.Navigate(typeof(ItemPage), itemId))
			if (!Frame.Navigate(typeof(CalculatorsPage), itemId))
			{
                throw new Exception(_resourceLoader.GetString("NavigationFailedExceptionMessage"));
            }
        }

		// Populates the page with content passed during navigation.
		// Any saved state is also provided when recreating a page from a prior session.
		// The state will be null the first time a page is visited.
		private async void NavigationHelperLoadState(object sender, LoadStateEventArgs e)
		{
			var sampleDataGroups = await SampleDataSource.GetGroupsAsync();
			DefaultViewModel["Groups"] = sampleDataGroups;
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