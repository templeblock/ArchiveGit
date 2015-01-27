using System;
using MasterOfCoin.Common;
using MasterOfCoin.Data;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace MasterOfCoin
{
    public sealed partial class HubPage : Page
    {
		public NavigationHelper NavigationHelper { get; private set; }
		public ObservableDictionary DefaultViewModel { get; private set; }

        public HubPage()
        {
			DefaultViewModel = new ObservableDictionary();
			InitializeComponent();

			NavigationHelper = new NavigationHelper(this);
			NavigationHelper.LoadState += NavigationHelperLoadState;
        }

        void HubSectionHeaderClick(object sender, HubSectionHeaderClickEventArgs e)
        {
            HubSection section = e.Section;
            var group = section.DataContext;
            Frame.Navigate(typeof(SectionPage), ((SampleDataGroup)group).UniqueId);
        }

        void ItemViewItemClick(object sender, ItemClickEventArgs e)
        {
            // Navigate to the appropriate destination page, configuring the new page
            // by passing required information as a navigation parameter
            var itemId = ((SampleDataItem)e.ClickedItem).UniqueId;
			//j Frame.Navigate(typeof(ItemPage), itemId);
			Frame.Navigate(typeof(CalculatorsPage), itemId);
		}

		private async void NavigationHelperLoadState(object sender, LoadStateEventArgs e)
		{
			// TODO: Create an appropriate data model for your problem domain to replace the sample data
			var sampleDataGroup = await SampleDataSource.GetGroupAsync("Group-4");
			DefaultViewModel["Section3Items"] = sampleDataGroup;
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
