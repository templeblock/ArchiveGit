using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ClassLibrary;

namespace Money
{
	public partial class SpendingPage : Page
	{
		public SpendingPageViewModel VM { set { base.DataContext = value; } get { return base.DataContext as SpendingPageViewModel; } }

		public SpendingPage()
		{
			InitializeComponent();
			//VM = new SpendingPageViewModel(); // What about binding to the App.Model also?
		}

		private void OnAddNewSpendingClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<SpendingPackagesPanel>().OnAddNewSpendingClick(sender, e);
		}

		private void OnSearchTextChanged(object sender, TextChangedEventArgs e)
		{
			this.ChildOfType<SpendingPackagesPanel>().OnSearchTextChanged(sender, e);
		}

		private void OnSearchResetClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<SpendingPackagesPanel>().OnSearchResetClick(sender, e);
		}

		private void OnPackagesTreeKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
		{
			this.ChildOfType<SpendingPackagesPanel>().OnPackagesTreeKeyDown(sender, e);
		}

		private void OnPackagesTreeClick(object sender, MouseButtonEventArgs e)
		{
			this.ChildOfType<SpendingPackagesPanel>().OnPackagesTreeClick(sender, e);
		}

		private void OnAddSpendingClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<SpendingDataGridPanel>().OnAddClick(sender, e);
		}

		private void OnEditSpendingClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<SpendingDataGridPanel>().OnEditClick(sender, e);
		}

		private void OnDeleteSpendingClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<SpendingDataGridPanel>().OnDeleteClick(sender, e);
		}
	}

	public partial class SpendingPageViewModel : ViewModelBase
	{
		public ICommand AddNewSpendingCommand { get; set; }

		public SpendingPageViewModel()
		{
			AddNewSpendingCommand = new Command(AddNewSpendingExecute);
		}

		public void AddNewSpendingExecute(object parameter)
		{
			//this.ChildOfType<SpendingPackagesPanel>().OnAddNewSpendingClick(sender, e);
		}
	}
}
