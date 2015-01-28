using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Navigation;
using ClassLibrary;

namespace Money
{
	public partial class IncomePage : Page
	{
		public IncomePage()
		{
			InitializeComponent();
			HtmlTextBlock content = base.Content.ChildOfType<HtmlTextBlock>();
			content.HyperlinkClick += App.MainPage.OnHyperlinkClick;
		}

		private void OnAddIncomeClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<IncomeDataGridPanel>().OnAddClick(sender, e);
		}

		private void OnEditIncomeClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<IncomeDataGridPanel>().OnEditClick(sender, e);
		}

		private void OnDeleteIncomeClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<IncomeDataGridPanel>().OnDeleteClick(sender, e);
		}

		private void OnAddIncomeKeyDown(object sender, KeyEventArgs e)
		{
			this.ChildOfType<IncomeAddPanel>().OnAddIncomeKeyDown(sender, e);
		}

		private void OnAddIncomeDoubleClick(object sender, MouseButtonEventArgs e)
		{
			this.ChildOfType<IncomeAddPanel>().OnDoubleClick(sender, e);
		}

		private void OnAddIncomeSelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			this.ChildOfType<IncomeAddPanel>().OnSelectionChanged(sender, e);
		}

		private void OnAddNewIncomeClick(object sender, RoutedEventArgs e)
		{
			this.ChildOfType<IncomeAddPanel>().OnAddClick(sender, e);
		}
	}
}
