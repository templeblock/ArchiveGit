using System;
using System.Windows;
using System.Windows.Controls;
using ClassLibrary;

namespace Money
{
	public partial class IncomeDialog : DialogPanel
	{
		private Income m_RootIncome;
		private Income m_RootIncomeOrig;
		private bool m_bCreateNew;

		public IncomeDialog(Income income, bool bCreateNew)
			: base()
		{
			InitializeComponent();
			InitializeDialogPanel(true/*bModal*/, x_StartDate);
			base.Loaded += OnLoaded;
			base.Closed += OnDialogClosed;
			base.DataContext = null;

			m_RootIncomeOrig = income;
			m_bCreateNew = bCreateNew;
			if (bCreateNew)
			{
				ProfileDate startProfileDate = ProfileCode.StartOfPlan.ToProfileDate();// or App.Model.ProfileHolder.Profile.StartProfileDate, or DateTime.Now.ToJulian()
				ProfileDate endProfileDate = ProfileCode.EndOfPlan.ToProfileDate();// or App.Model.ProfileHolder.Profile.EndProfileDate;
				m_RootIncomeOrig.SetDates(startProfileDate, endProfileDate);
			}
		}

		public override void Dispose()
		{
			base.DataContext = null;
			x_ListControl.ItemsSource = null;
		}

		private void OnLoaded(object sender, RoutedEventArgs e)
		{
			base.Loaded -= OnLoaded;

			m_RootIncome = m_RootIncomeOrig.DeepClone();
			m_RootIncome.Transactions.Clear(); // Transactions will be rebuilt when the income is run
			base.DataContext = m_RootIncome;
			x_ListControl.ItemsSource = m_RootIncome.Deductions;

			string titleFormat = (m_bCreateNew ? "Add" : "Edit") + " the '{0}' income source";
			x_Title.Text = string.Format(titleFormat, m_RootIncome.Name);
		}

		private void OnDialogClosed(object sender, EventArgs e)
		{
			base.Closed -= OnDialogClosed;
			if (DialogResult == MessageBoxResult.Cancel)
				return;

			Document document = App.Model.DocumentHolder.Document;
			if (m_bCreateNew)
				document.Incomes.AddAndRun(m_RootIncome, document);
			else
				document.Incomes.UpdateAndRun(m_RootIncomeOrig, m_RootIncome, document);
		}

		private void DeleteSelectedDeduction(object sender)
		{
			ContentPresenter item = (sender as DependencyObject).ParentOfType<ContentPresenter>();
			if (item == null)
				return;
			Deduction deduction = (item.Content as Deduction);
			try
			{
				if (deduction != null)
					m_RootIncome.Deductions.Remove(deduction);
			}
			catch (Exception ex)
			{
				ex.DebugOutput();
			}
		}

		private void InsertNewDeduction(object sender)
		{
			ContentPresenter item = (sender as DependencyObject).ParentOfType<ContentPresenter>();
			Deduction deduction = (item != null ? item.Content as Deduction : null);
			int index = m_RootIncome.Deductions.IndexOf(deduction);
			if (index >= 0)
				index++;
			else
				index = m_RootIncome.Deductions.Count;

			m_RootIncome.Deductions.Insert(index, Deduction.CreateDefault(m_RootIncome.Id));
			//j	x_ListControl.SelectedIndex = index;
		}

		private void OnAddDeductionClick(object sender, RoutedEventArgs e)
		{
			InsertNewDeduction(null);
		}

		private void OnInsertClick(object sender, RoutedEventArgs e)
		{
			InsertNewDeduction(sender);
		}

		private void OnDeleteClick(object sender, RoutedEventArgs e)
		{
			DeleteSelectedDeduction(sender);
		}

		private void OnOkClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.OK;
		}

		private void OnCancelClick(object sender, RoutedEventArgs e)
		{
			DialogResult = MessageBoxResult.Cancel;
		}

		private void OnDropDownClosed(object sender, EventArgs e)
		{
			if (sender is Control)
				(sender as Control).FocusEx();
		}
	}
}
