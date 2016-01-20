using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace TrumpITReporting
{
	public partial class CampaignsPanel : UserControl
	{
		public CampaignsPanel()
		{
			InitializeComponent();
			base.Loaded += OnLoaded;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			x_CampaignDetails.Visibility = Visibility.Collapsed;
		}

		private void OnCampaignSelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
		{
			foreach (Campaigns item in e.AddedItems)
				item.IsSelected = true;
			foreach (Campaigns item in e.RemovedItems)
				item.IsSelected = false;
		}

		private void OnCampaignSeeDetailsClick(object sender, RoutedEventArgs e)
		{
			x_CampaignDetails.Visibility = Visibility.Visible;
			x_CampaignWrapPanel.Visibility = Visibility.Collapsed;
		}

		private void OnCampaignCloseDetailsClick(object sender, RoutedEventArgs e)
		{
			x_CampaignDetails.Visibility = Visibility.Collapsed;
			x_CampaignWrapPanel.Visibility = Visibility.Visible;
		}
	}
}
