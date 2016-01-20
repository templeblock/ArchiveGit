using System.Windows;
using ClassLibrary;

namespace TrumpITReporting
{
	public partial class DataAccessDialog : DialogPanel
	{
		public DataAccessDialog()
			: base()
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, x_DataGrid);
			base.Loaded += OnLoaded;
		}

		public override void Dispose()
		{
			base.DataContext = null;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			x_DataGrid.ItemsSource = App.Model.ILincDataCollection;
			x_PersonDataGrid.ItemsSource = App.Model.PersonCollection;
			x_CampaignDataGrid.ItemsSource = App.Model.CampaignCollection;
			x_MarketingLeadDataGrid.ItemsSource = App.Model.MarketingLeadCollection;
			x_AddressDataGrid.ItemsSource = App.Model.AddressCollection;
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}

		private void OnGetSessionFromUserIDClick(object sender, System.Windows.RoutedEventArgs e)
		{
			App.Model.ILincDataCollection.GetSessionFromUserID(x_UserID.Text);
		}

		private void OnGetSessionFromActivityIDClick(object sender, System.Windows.RoutedEventArgs e)
		{
			App.Model.ILincDataCollection.GetSessionFromActivityID(x_ActivityID.Text);
		}

		private void OnGetPersonFromAccountIDClick(object sender, System.Windows.RoutedEventArgs e)
		{
			App.Model.PersonCollection.GetPersonFromAccountID(x_AccountID.Text);
		}

		private void OnGetCampaignFromAccountIDClick(object sender, System.Windows.RoutedEventArgs e)
		{
			App.Model.CampaignCollection.GetCampaignFromAccountID(x_AccountID.Text);
		}

		private void OnGetMarketingLeadFromAccountIDClick(object sender, System.Windows.RoutedEventArgs e)
		{
			App.Model.MarketingLeadCollection.GetFromAccountID(x_AccountID.Text);
		}

		private void OnGetAddressFromAddressIDClick(object sender, System.Windows.RoutedEventArgs e)
		{
			App.Model.AddressCollection.GetAddressFromAddressID(x_AddressID.Text);
		}
	}
}
