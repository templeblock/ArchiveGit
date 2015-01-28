using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace Money
{
	public partial class IncomeAddDialog : DialogPanel
	{
		public event Action<Income> AddIncome;
		private static IncomeCollection m_IncomeSources;
		private string m_IncomeSourceDefault;

		public IncomeAddDialog(string incomeSourceDefault)
		{
			InitializeComponent();
			base.Loaded += OnLoaded;

			m_IncomeSourceDefault = incomeSourceDefault;

			InitializeDialogPanel(true/*bModal*/, x_IncomeSourceList as Control);
		}

		public override void Dispose()
		{
			// Don't clear m_IncomeSources since we retain it as a static list
			x_IncomeSourceList.ItemsSource = null;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			if (m_IncomeSources == null)
				DownloadIncomes();
			else
				AttachIncomes();
		}

		private void DownloadIncomes()
		{
			StringBuilder sql = new StringBuilder();
			Sql.SelectIncomes(sql);
			Sql.Get(sql, Sql.IncomesLinqQuery, IncomesLinqQueryToResult);
		}

		private void IncomesLinqQueryToResult(IEnumerable query)
		{
			if (query == null)
				return;

			IEnumerable<Income> incomes = (IEnumerable<Income>)query;
			IncomeCollection incomesList = incomes.ToObservableCollection<IncomeCollection,Income>();
			m_IncomeSources = incomesList;

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

			x_IncomeSourceList.ItemsSource = m_IncomeSources;
			x_IncomeSourceList.UpdateLayout();

			if (defaultIndex < 0)
				defaultIndex = 0;

			x_IncomeSourceList.SelectedIndex = defaultIndex;
		}

		private void OnSelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			if (x_IncomeSourceList.SelectedIndex >= 0)
			{
				string description = m_IncomeSources[x_IncomeSourceList.SelectedIndex].Description;
				x_Description.HtmlUri = "Content/Income/" + description;
			}
			else
				x_Description.Html = "";
		}

		public void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}

		public void OnAddClick(object sender, RoutedEventArgs e)
		{
#if ALTERNATE
			base.Closed += OnAddDialogClosed;
#else
			Income income = x_IncomeSourceList.SelectedItem as Income;
			if (AddIncome != null && income != null)
				AddIncome(income);
#endif
			DialogResult = MessageBoxResult.OK;
		}

#if ALTERNATE
		private void OnAddDialogClosed(object sender, EventArgs e)
		{
			base.Closed -= OnAddDialogClosed;

			Income income = x_IncomeSourceList.SelectedItem as Income;
			if (AddIncome != null && income != null)
				AddIncome(income);
		}
#endif
	}
}
