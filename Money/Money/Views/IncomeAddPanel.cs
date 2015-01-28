using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ClassLibrary;

namespace Money
{
	public partial class IncomeAddPanel : ViewPanel
	{
		private IncomeCollection m_IncomeSources = new IncomeCollection();
		private string m_IncomeSourceDefault;
		private IncomePage m_Page;

		public IncomeAddPanel()
		{
			base.Loaded += OnLoaded;
			m_IncomeSourceDefault = null;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;
			m_Page = this.ParentOfType<IncomePage>();

			if (Helper.IsInDesignTool)
				return;

			DownloadIncomes();
		}

		public override void Dispose()
		{
			m_Page.x_IncomeSourceList.ItemsSource = null;
			m_IncomeSources.Dispose();
			m_IncomeSources = null;
		}

		private void DownloadIncomes()
		{
			IncomeSqlQuery query = new IncomeSqlQuery();
			query.Result += IncomesQueryResult;
			query.Execute();
		}

		private void IncomesQueryResult(Collection<Income> collection, string error)
		{
			if (error != null)
			{
				MessageBoxEx.ShowError("Web access", error, null);
				return;
			}

			if (collection == null || collection.Count <= 0)
				return;

			m_IncomeSources = collection.ToObservableCollection<IncomeCollection,Income>();;

			AttachIncomes();
		}

		private void AttachIncomes()
		{
			int defaultIndex = -1;
			int index = 0;
			foreach (Income income in m_IncomeSources)
			{
				if (income.Name == m_IncomeSourceDefault)
				{
					defaultIndex = index;
					break;
				}

				index++;
			}

			m_Page.x_IncomeSourceList.ItemsSource = m_IncomeSources;
			m_Page.x_IncomeSourceList.UpdateLayout();

			if (defaultIndex < 0)
				defaultIndex = 0;

			m_Page.x_IncomeSourceList.SelectedIndex = defaultIndex;
		}

		internal void OnAddIncomeKeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter)
				OnAddClick(sender, e);
		}

		internal void OnDoubleClick(object sender, MouseButtonEventArgs e)
		{
			bool doubleClick = MouseButtonHelper.IsDoubleClick(sender, e);
			if (doubleClick)
				OnAddClick(sender, e);
		}

		internal void OnSelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			int index = m_Page.x_IncomeSourceList.SelectedIndex;
			if (index >= 0 && index < m_IncomeSources.Count)
			{
				string description = m_IncomeSources[index].Description;
				m_Page.x_Description.HtmlUri = UriHelper.UriAppRelative("Content/Income/" + description).ToString();
			}
			else
				m_Page.x_Description.Html = "";
		}

		internal void OnAddClick(object sender, RoutedEventArgs e)
		{
			Income income = m_Page.x_IncomeSourceList.SelectedItem as Income;
			if (income != null)
				new IncomeDialog(income, true/*bCreateNew*/);
		}
	}
}
